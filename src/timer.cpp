#include "timer.hpp"
#include "cpu.hpp"
#include "interrupts.hpp"

// 16-bit timer, but only upper 8 bits exposed by memory controller
// Therefore, gets incremented every 256 cycles
uint16_t DIV = 0xAB00;
uint8_t TIMA = 0x00;
uint8_t TMA = 0x00;
uint8_t TAC = 0xF8;

uint32_t TIMA_cycle_count{};
uint32_t TIMA_max_cycles = 1024; // GB starts with TAC clock 00

uint8_t read_timer(uint16_t address)
{
	if (address == 0xFF04) // DIV : Only the 8 upper bits are exposed
		return DIV & 0xFF00 >> 8;

	if (address == 0xFF05)
		return TIMA;

	if (address == 0xFF06)
		return TMA;

	if (address == 0xFF07)
		return TAC;
}

void write_timer(uint16_t address, uint8_t value)
{
	if (address == 0xFF04) // Reset timer
	{
		DIV = 0x00;
		TIMA = 0x00;
	}

	else if (address == 0xFF05)
		TIMA = value;

	else if (address == 0xFF06)
		TMA = value;

	else if (address == 0xFF07)
		update_timer_freq(value);
}

void update_timer_freq(uint8_t value)
{
	if ((TAC & 0x03) != (value & 0x03)) // If timer clock is changed
	{
		switch (value & 0x03)
		{
		case 0x00: // CPU clock / 1024
			TIMA_max_cycles = CPU_FREQ / 4096;
			break;

		case 0x01: // CPU clock / 16
			TIMA_max_cycles = CPU_FREQ / 262144;
			break;

		case 0x02: // CPU clock / 64
			TIMA_max_cycles = CPU_FREQ / 65536;
			break;

		case 0x03: // CPU clock / 256
			TIMA_max_cycles = CPU_FREQ / 16382;
			break;
		}

		TIMA = 0; // Reset the timer
	}

	TAC = value;
}

void tick_timer(uint8_t cycles)
{
	for (uint8_t i = 0; i < cycles; i += 4)
	{
		DIV += 4;

		if (TAC & 0x04) // Bit 2 enabled : TIMA timer enabled
		{
			TIMA_cycle_count += 4;

			if (TIMA_cycle_count > TIMA_max_cycles)
			{
				if (TIMA == 0xFF) // If TIMA overflowing, reset and send IRQ
				{
					TIMA = TMA;
					interrupt_request(INTERRUPT_TIMER);
				}
				else
				{
					TIMA++;
					//printf("Incrementing TIMA, cycles: %d\n", cycle_count);
				}
				TIMA_cycle_count %= TIMA_max_cycles;
			}
		}
	}
}

void reset_timer()
{
	DIV = 0xAB00;
	TIMA = 0x00;
	TMA = 0x00;
	TAC = 0xF8;

	TIMA_cycle_count = 0;
	TIMA_max_cycles = 1024;
}