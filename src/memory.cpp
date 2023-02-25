#include <stdio.h>

#include "memory.h"
#include "io.h"
#include "interrupts.h"
#include "ppu.h"
#include "rom.h"

uint8_t memory[0x10000]{};

uint8_t read_byte(uint16_t address)
{
	if (address >= 0x0000 && address <= 0x7FFF)
		return read_rom(address);

	if (address >= 0x8000 && address <= 0x9FFF)
		return read_vram(address - 0x8000);

	if (address == 0xFF44)
		return 0x90;

	else if (address >= 0xFE00 && address <= 0xFE9F)
		return read_oam(address - 0xFE00);

	if ((address >= 0xFF00 && address <= 0xFF7F) || (address == 0xFFFF))
		return read_io(address);

	return memory[address];
}

uint16_t read_word(uint16_t address)
{
	return read_byte(address) | (read_byte(address + 1) << 8);
}

void write_byte(uint16_t address, uint8_t value)
{
	if (address == 0xFF01) printf("%02x ", value);

	if (address >= 0x0000 && address <= 0x7FFF)
		write_rom(address, value);

	else if (address >= 0x8000 && address <= 0x9FFF)
		write_vram(address - 0x8000, value);

	else if (address >= 0xFE00 && address <= 0xFE9F)
		write_oam(address - 0xFE00, value);

	else if ((address >= 0xFF00 && address <= 0xFF7F) || (address == 0xFFFF))
		write_io(address, value);

	else
		memory[address] = value;
}

void write_word(uint16_t address, uint16_t value)
{
	write_byte(address, (value & 0x00FF));
	write_byte(address + 1, (value & 0xFF00) >> 8);
}
