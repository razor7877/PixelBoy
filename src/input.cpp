#include <cstdint>

#include "cpu.h"
#include "input.h"
#include "interrupts.h"

#define DIV_FREQ 16384

uint8_t io_register = 0xCF;

uint8_t SB{};
uint8_t SC = 0x7E;

// 16-bit timer, but only upper 8 bits exposed by memory controller
// Therefore, gets incremented every 256 cycles
uint16_t DIV = 0xAB00;
uint8_t TIMA = 0x00;
uint8_t TMA = 0x00;
uint8_t TAC = 0xF8;

uint32_t TIMA_cycle_count{};

void set_key(joypad key)
{
	if (!get_key(key))
		interrupt_request(INTERRUPT_JOYPAD);

	io_register &= ~key;
}

void clear_key(joypad key)
{
	io_register |= key;
}

bool get_key(joypad key)
{
	return (io_register & key);
}

void tick_timer(uint8_t cycles)
{
	DIV += cycles;

	if (TAC & 0x04) // Bit 2 enabled : TIMA timer enabled
	{
		TIMA_cycle_count += cycles;
		bool inc_TIMA = false;

		switch (TAC & 0x03)
		{
			case 0x00: // CPU clock / 1024
				if (TIMA_cycle_count > (CPU_FREQ / 1024))
				{
					inc_TIMA = true;
					TIMA_cycle_count %= (CPU_FREQ / 1024);
				}
				break;

			case 0x01: // CPU clock / 16
				if (TIMA_cycle_count > (CPU_FREQ / 16))
				{
					inc_TIMA = true;
					TIMA_cycle_count %= (CPU_FREQ / 16);
				}
				break;

			case 0x02: // CPU clock / 64
				if (TIMA_cycle_count > (CPU_FREQ / 64))
				{
					inc_TIMA = true;
					TIMA_cycle_count %= (CPU_FREQ / 64);
				}
				break;

			case 0x03: // CPU clock / 256
				if (TIMA_cycle_count > (CPU_FREQ / 256))
				{
					inc_TIMA = true;
					TIMA_cycle_count %= (CPU_FREQ / 256);
				}
				break;
		}

		if (inc_TIMA)
		{
			if (TIMA == 0xFF) // If TIMA overflowing, reset and send IRQ
			{
				TIMA = TMA;
				interrupt_request(INTERRUPT_TIMER);
			}
			else { TIMA++; }
		}
	}
}