#include <stdio.h>

#include "apu.hpp"

// Sound channel 1
uint8_t NR10 = 0x80;
uint8_t NR11 = 0xBF;
uint8_t NR12 = 0xF3;
uint8_t NR13 = 0xFF;
uint8_t NR14 = 0xBF;

// Sound channel 2
uint8_t NR21 = 0x3F;
uint8_t NR22 = 0x00;
uint8_t NR23 = 0xFF;
uint8_t NR24 = 0xBF;

// Sound channel 3
uint8_t NR30 = 0x7F;
uint8_t NR31 = 0xFF;
uint8_t NR32 = 0x9F;
uint8_t NR33 = 0xFF;
uint8_t NR34 = 0xBF;

// Sound channel 4
uint8_t NR41 = 0xFF;
uint8_t NR42 = 0x00;
uint8_t NR43 = 0x00;
uint8_t NR44 = 0xBF;

// Global registers
uint8_t NR50 = 0x77;
uint8_t NR51 = 0xF3;
uint8_t NR52 = 0xF1;

uint8_t wave_ram[16]{};

uint8_t read_apu(uint16_t address)
{
	if (address == 0xFF10)
		return NR10;

	else if (address == 0xFF11) // TODO: Lower bits write only?
		return NR11;

	else if (address == 0xFF12)
		return NR12;

	else if (address == 0xFF13)
		return NR13;

	else if (address == 0xFF14)
		return NR14;

	// 0xFF15 unused

	else if (address == 0xFF16)
		return NR21;

	else if (address == 0xFF17)
		return NR22;

	else if (address == 0xFF18)
		return NR23;

	else if (address == 0xFF19)
		return NR24;

	else if (address == 0xFF1A)
		return NR30;

	else if (address == 0xFF1B)
		return NR31;

	else if (address == 0xFF1C)
		return NR32;

	else if (address == 0xFF1D)
		return NR33;

	else if (address == 0xFF1E) // TODO : Only bit 6 readable?
		return NR34;

	else if (address == 0xFF24)
		return NR50;

	else if (address == 0xFF25)
		return NR51;

	else if (address == 0xFF26)
		return NR52;

	else if (address >= 0xFF30 && address <= 0xFF3F)
	{
		printf("Wave RAM read ADR %x\n", address);
		return wave_ram[address - 0xFF30];
	}

	if (address >= 0xFF10 && address <= 0xFF26) { printf("Audio register read ADR %x\n", address); } // Audio

	return 0xFF;
}

void write_apu(uint16_t address, uint8_t value)
{
	if (address >= 0xFF10 && address <= 0xFF26) { printf("Audio register write ADR %x value %x\n", address, value); } // Audio
	else if (address >= 0xFF30 && address <= 0xFF3F) { printf("Wave pattern write ADR %x value %x\n", address, value); } // Wave pattern
}
