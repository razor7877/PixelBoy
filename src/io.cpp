#include <cstdint>
#include <stdio.h>

#include "cpu.h"
#include "io.h"
#include "interrupts.h"
#include "ppu.h"
#include "rom.h"

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
uint32_t TIMA_max_cycles{};

uint8_t read_io(uint16_t address)
{
	if (address == 0xFF00)
		return io_register;

	if (address == 0xFF01)
		return SB;

	if (address == 0xFF02)
		return SC;

	if (address == 0xFF04) // DIV : Only the 8 upper bits are exposed
		return DIV & 0xFF00 >> 8;

	if (address == 0xFF05)
		return TIMA;

	if (address == 0xFF06)
		return TMA;

	if (address == 0xFF07)
		return TAC;

	if (address >= 0xFF10 && address <= 0xFF26) {} // Audio
	if (address >= 0xFF30 && address <= 0xFF3F) {} // Wave pattern

	if (address >= 0xFF40 && address <= 0xFF4F) // PPU registers
		return read_ppu(address);

	if (address == 0xFF0F)
		return IF;

	if (address == 0xFFFF)
		return IE;

	else
		return 0xFF;
}

void write_io(uint16_t address, uint8_t value)
{
	if (address == 0xFF00) // Write to joypad register
		io_register = value & 0x30; // Only bits 4-5 are writeable

	else if (address == 0xFF01) // Write to serial tranfer data
		SB = value;

	else if (address == 0xFF02) // Write to serial transfer control
		SC = value;

	else if (address == 0xFF04) // Reset timer
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

	else if (address >= 0xFF10 && address <= 0xFF26) {} // Audio
	else if (address >= 0xFF30 && address <= 0xFF3F) {} // Wave pattern

	else if (address >= 0xFF40 && address <= 0xFF4F) // PPU registers
		write_ppu(address, value);

	else if (address == 0xFF50) // Boot ROM writes to this register to unmap itself from 0x00-0xFF
		boot_done = true;

	else if (address == 0xFF0F) // Interrupts requests
		IF = value;

	else if (address == 0xFFFF)
		IE = value;
}

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

void update_timer_freq(uint8_t value)
{
	if ((TAC & 0x03) != (value & 0x03)) // If timer clock is changed
	{
		switch (value & 0x03)
		{
			case 0x00: // CPU clock / 1024
				TIMA_max_cycles = CPU_FREQ / 1024;
				break;

			case 0x01: // CPU clock / 16
				TIMA_max_cycles = (CPU_FREQ / 16);
				break;

			case 0x02: // CPU clock / 64
				TIMA_max_cycles = CPU_FREQ / 64;
				break;

			case 0x03: // CPU clock / 256
				TIMA_max_cycles = CPU_FREQ / 256;
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
}