#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "frontends/frontend.h"
#include "cpu.h"
#include "io.h"
#include "interrupts.h"
#include "ppu.h"
#include "rom.h"
#include "timer.h"
#include "apu.h"
#include "memory.h"

extern struct Frontend frontend;

uint8_t io_register = 0xCF;

uint8_t SB = 0;
uint8_t SC = 0x7E;

uint8_t KEY1 = 0x7E;
uint8_t RP = 0x3E;

// Get logs for IO register writes
//#define IO_DEBUG

uint8_t read_io(uint16_t address)
{
	if (address == 0xFF00)
	{
		frontend.update_io();
		return io_register;
	}

	if (address == 0xFF01)
		return SB;

	if (address == 0xFF02)
		return SC;

	if (address >= 0xFF04 && address <= 0xFF07) // Timer
		return read_timer(address);

	if (address == 0xFF0F)
		return IF;

	if (address >= 0xFF10 && address <= 0xFF26) { read_apu(address); } // Audio
	if (address >= 0xFF30 && address <= 0xFF3F) { read_apu(address); } // Wave pattern

	if (address >= 0xFF40 && address <= 0xFF4B) // PPU registers
		return read_ppu(address);

	if (address == 0xFF4D)
		return KEY1;

	if (address == 0xFF4F)
		return read_ppu(address);

	if (address >= 0xFF51 && address <= 0xFF55) // CBG VRAM DMA and palettes
		return read_ppu(address);

	if (address == 0xFF56)
		return RP;

	if (address >= 0xFF68 && address <= 0xFF6C)
		return read_ppu(address);

	if (address == 0xFF70)
		return SVBK & 0x7;

	if (address == 0xFFFF)
		return IE;

	else
		return 0xFF;
}

void write_io(uint16_t address, uint8_t value)
{
	if (address == 0xFF00) // Write to joypad register
	{
		io_register = (io_register & 0xCF) | (value & 0x30); // Only bits 4-5 are writeable
#ifdef IO_DEBUG
		printf("IO register write value %x\n", (value & 0x30));
#endif
	}

	else if (address == 0xFF01) // Write to serial tranfer data
		SB = value;

	else if (address == 0xFF02) // Write to serial transfer control
		SC = value;

	else if (address >= 0xFF04 && address <= 0xFF07)
		write_timer(address, value);

	else if (address == 0xFF0F) // Interrupts requests
		IF = value;

	else if (address >= 0xFF10 && address <= 0xFF26) { write_apu(address, value); } // Audio
	else if (address >= 0xFF30 && address <= 0xFF3F) { write_apu(address, value); } // Wave pattern

	else if (address >= 0xFF40 && address <= 0xFF4B) // PPU registers
		write_ppu(address, value);

	else if (address == 0xFF4D)
	{
		KEY1 = (KEY1 & 0xFE) | (value & 0b1); // Only bit 1 writeable
		log_info("KEY1 write val %x\n", KEY1);
	}

	else if (address == 0xFF4F)
		write_ppu(address, value);

	else if (address >= 0xFF51 && address <= 0xFF55) // CBG VRAM DMA and palettes
		write_ppu(address, value);

	else if (address == 0xFF56)
		RP = (RP & 0b00111110) | (value & 11000001);

	else if (address >= 0xFF68 && address <= 0xFF6C)
		write_ppu(address, value);

	else if (address == 0xFF50) // Boot ROM writes to this register to unmap itself from 0x00-0xFF
		boot_done = true;

	else if (address == 0xFF70)
		SVBK = value & 0x7;

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
	return !(io_register & key);
}

void reset_io()
{
	io_register = 0xCF;

	SB = 0;
	SC = 0x7E;

	reset_timer();
}
