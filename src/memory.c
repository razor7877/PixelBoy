#include <string.h>

#include "memory.h"
#include "io.h"
#include "interrupts.h"
#include "ppu.h"
#include "rom.h"

#include "logging.h"

uint8_t memory[0x10000] = { 0 };
uint8_t wram[0x8000] = {0}; // 8*4 KiB banks on CGB
uint8_t hram[0x80] = {0};

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
		if (cpuState.run_as_cgb)
		{
			uint8_t wram_bank = SVBK;
			if (wram_bank == 0) // When SVBK is 0, we map to WRAM bank 1
				wram_bank = 1;
			uint16_t mapped_address = (wram_bank << 12) | (address - 0xC000);

			return wram[mapped_address];
		}
		else
			return wram[address - 0xC000];
	}

	if (address >= 0xE000 && address <= 0xFDFF) // Echo RAM, mapped to same memory as 0xC000-0xCFFF
		return read_byte(address - 0x2000);

	if (address >= 0xFE00 && address <= 0xFE9F)
		return read_oam(address - 0xFE00);

	if (address >= 0xFEA0 && address <= 0xFEFF) // Unusable, return undefined value
		return 0xFF;

	if ((address >= 0xFF00 && address <= 0xFF7F) || (address == 0xFFFF))
		return read_io(address);

	if (address >= 0xFF80 && address <= 0xFFFE)
	{
		//if (hram[address - 0xFF80] != memory[address])
		//	log_error("HRAM read adr %d map_adr %d value %d %d\n", address, address - 0xFF80, hram[address - 0xFF80], memory[address]);
		return memory[address];
	}

	return 0xFF;
}

uint16_t read_word(uint16_t address)
{
	return read_byte(address) | (read_byte(address + 1) << 8);
}

void write_byte(uint16_t address, uint8_t value)
{
	//if (address >= sp)
	//	log_warning("Memory write in stack opcode %s pc %x HL %x sp %x\n", instructions[opcode].disassembly, pc, HL, sp);

	//if (address >= 0xC000 && address <= 0xC10F)
	//	log_warning("Writing at ADR %x VALUE %x pc %x\n", address, value, pc);

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
		if (cpuState.run_as_cgb)
		{
			uint16_t mapped_address = (SVBK << 12) | (address - 0xC000);
			wram[mapped_address] = value;
		}
		else
			wram[address - 0xC000] = value;
}

	else if (address >= 0xE000 && address <= 0xFDFF) // Echo RAM writes to WRAM
		write_byte(address - 0x2000, value);

	else if (address >= 0xFE00 && address <= 0xFE9F)
		write_oam(address - 0xFE00, value);

	else if (address >= 0xFEA0 && address <= 0xFEFF) {} // Unused

	else if ((address >= 0xFF00 && address <= 0xFF7F) || (address == 0xFFFF))
		write_io(address, value);

	else if (address >= 0xFF80 && address <= 0xFFFE)
	{
		// TODO : Why is value lost when reading hram or memory later if applying offset or bitmask????
		memory[address] = value;
		hram[address - 0xFF80] = value;
	}
		
}

void write_word(uint16_t address, uint16_t value)
{
	write_byte(address, (value & 0x00FF));
	write_byte(address + 1, (value & 0xFF00) >> 8);
}

void reset_memory()
{
	memset(&wram, 0, sizeof(wram));
	memset(&hram, 0, sizeof(hram));
	memset(&memory, 0, sizeof(memory));
}
