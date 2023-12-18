#include <stdio.h>

#include "memory.h"
#include "io.h"
#include "interrupts.h"
#include "ppu.h"
#include "rom.h"

uint8_t memory[0x10000] = {0};

uint8_t read_byte(uint16_t address)
{
	//if (address >= 0xFF80 && address <= 0xFFFE)
	//	printf("HRAM read adr %x val %x\n", address, memory[address]);

	if (address >= 0x0000 && address <= 0x7FFF)
		return read_rom(address);

	if (address >= 0x8000 && address <= 0x9FFF)
		return read_vram(address - 0x8000);

	if (address >= 0xA000 && address <= 0xBFFF)
		return read_external_ram(address);

	else if (address >= 0xFE00 && address <= 0xFE9F)
		return read_oam(address - 0xFE00);

	if ((address >= 0xFF00 && address <= 0xFF7F) || (address == 0xFFFF))
		return read_io(address);

	if (address >= 0xE000 && address <= 0xFDFF) // Echo RAM, mapped to same memory as 0xC000-0xCFFF
		return memory[address - 0x2000];

	return memory[address];
}

uint16_t read_word(uint16_t address)
{
	return read_byte(address) | (read_byte(address + 1) << 8);
}

void write_byte(uint16_t address, uint8_t value)
{
	//if (address >= 0xFF80 && address <= 0xFFFE && value != 0)
	//	printf("Write at adr 0xFFB6 val %x\n", value);

	//if (address == 0xFF01) printf("Write at adr 0xFF01 val %02x\n", value);

	if (address >= 0x0000 && address <= 0x7FFF)
		write_rom(address, value);

	else if (address >= 0x8000 && address <= 0x9FFF)
		write_vram(address - 0x8000, value);

	if (address >= 0xA000 && address <= 0xBFFF)
		return write_external_ram(address, value);

	else if (address >= 0xFE00 && address <= 0xFE9F)
		write_oam(address - 0xFE00, value);

	else if ((address >= 0xFF00 && address <= 0xFF7F) || (address == 0xFFFF))
		write_io(address, value);

	else if (address >= 0xE000 && address <= 0xFDFF) {} // Ignore echo RAM writes

	else
		memory[address] = value;

	//if (address >= 0xFF80 && address <= 0xFFFE && value != 0)
	//	printf("HRAM write adr %x val %x\n", address, value);
}

void write_word(uint16_t address, uint16_t value)
{
	write_byte(address, (value & 0x00FF));
	write_byte(address + 1, (value & 0xFF00) >> 8);
}

void reset_memory()
{
	memset(&memory, 0, sizeof(memory));
}
