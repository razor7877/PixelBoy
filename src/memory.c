#include <string.h>

#include "memory.h"
#include "io.h"
#include "interrupts.h"
#include "ppu.h"
#include "rom.h"

uint8_t memory[0x10000] = {0};
#ifdef CGB_MODE
uint8_t wram[0x8000] = {0}; // 8*4 KiB banks on CGB
#else
uint8_t wram[0x2000] = {0}; // 2*4 KiB banks on DMG
#endif

uint8_t SVBK = 0xF8;

uint8_t read_byte(uint16_t address)
{
	if (address >= 0x0000 && address <= 0x7FFF)
		return read_rom(address);

	if (address >= 0x8000 && address <= 0x9FFF)
		return read_vram(address - 0x8000);

	if (address >= 0xA000 && address <= 0xBFFF)
		return read_external_ram(address);

	if (address >= 0xC000 && address <= 0xCFFF) // WRAM bank 00
		return wram[address & 0x0FFF];

	if (address >= 0xD000 && address <= 0xDFFF) // Switchable WRAM bank
	{
#ifdef CGB_MODE
		uint16_t mapped_address = (SVBK << 12) | (address & 0x0FFF);
		return wram[mapped_address];
#else
		return wram[address & 0x0FFF];
#endif
	}

	if (address >= 0xFE00 && address <= 0xFE9F)
		return read_oam(address - 0xFE00);

	if ((address >= 0xFF00 && address <= 0xFF7F) || (address == 0xFFFF))
		return read_io(address);

	if (address >= 0xE000 && address <= 0xFDFF) // Echo RAM, mapped to same memory as 0xC000-0xCFFF
		return read_byte(address - 0x2000);

	return memory[address % sizeof(memory)];
}

uint16_t read_word(uint16_t address)
{
	return read_byte(address) | (read_byte(address + 1) << 8);
}

void write_byte(uint16_t address, uint8_t value)
{
	if (address >= 0x0000 && address <= 0x7FFF)
		write_rom(address, value);

	else if (address >= 0x8000 && address <= 0x9FFF)
		write_vram(address - 0x8000, value);

	else if (address >= 0xA000 && address <= 0xBFFF)
		write_external_ram(address, value);

	else if (address >= 0xC000 && address <= 0xCFFF)
		wram[address & 0x0FFF] = value;

	else if (address >= 0xD000 && address <= 0xDFFF) // Switchable WRAM bank
	{
#ifdef CGB_MODE
		uint16_t mapped_address = (SVBK << 12) | (address & 0x0FFF);
		wram[mapped_address] = value;
#else
		wram[address & 0x0FFF] = value;
#endif
	}

	else if (address >= 0xFE00 && address <= 0xFE9F)
		write_oam(address - 0xFE00, value);

	else if ((address >= 0xFF00 && address <= 0xFF7F) || (address == 0xFFFF))
		write_io(address, value);

	else if (address >= 0xE000 && address <= 0xFDFF) {} // Ignore echo RAM writes

	else
		memory[address % sizeof(memory)] = value;
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
