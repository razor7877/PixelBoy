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

	else if (address == 0xFF11) // Only bits 6-7 are readable
		return (NR11 & 0xC0);

	else if (address == 0xFF12)
		return NR12;

	else if (address == 0xFF13) // NR13 is write-only
		return 0xFF;

	else if (address == 0xFF14) // Return bits 3-6 (0-2 and 7 are write-only)
		return (NR14 & 0x78);

	// 0xFF15 unused

	else if (address == 0xFF16)
		return NR21;

	else if (address == 0xFF17)
		return NR22;

	else if (address == 0xFF18) // NR23 is write-only
		return 0xFF;

	else if (address == 0xFF19) // Return bits 3-6 (0-2 and 7 are write-only)
		return (NR24 & 0x78);

	else if (address == 0xFF1A)
		return NR30;

	else if (address == 0xFF1B) // Write-only
		return 0xFF;

	else if (address == 0xFF1C)
		return NR32;

	else if (address == 0xFF1D) // Write-only
		return 0xFF;

	else if (address == 0xFF1E) // Only bit 6 readable
		return (NR34 & 0x40);

	// 0xFF1F unused?

	else if (address == 0xFF20)
		return NR41;

	else if (address == 0xFF21)
		return NR42;

	else if (address == 0xFF22)
		return NR43;

	else if (address == 0xFF23) // Highest bit is write only
		return (NR44 & 0x7F);

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
	if (address == 0xFF10)
		NR10 = value;

	else if (address == 0xFF11)
		NR11 = value;

	else if (address == 0xFF12)
		NR12 = value;

	else if (address == 0xFF13)
		NR13 = value;

	else if (address == 0xFF14)
		NR14 = value;

	// 0xFF15 unused

	else if (address == 0xFF16)
		NR21 = value;

	else if (address == 0xFF17)
		NR22 = value;

	else if (address == 0xFF18)
		NR23 = value;

	else if (address == 0xFF19)
		NR24 = value;

	else if (address == 0xFF1A)
		NR30 = (NR30 & 0x7F) | (value & 0x80); // Only bit 7 is used

	else if (address == 0xFF1B)
		NR31 = value;

	else if (address == 0xFF1C)
		NR32 = (NR32 & 0x9F) | (value & 0x60); // Only bits 5-6 are used

	else if (address == 0xFF1D)
		NR33 = value;

	else if (address == 0xFF1E) // Bits 3-5 are unused
		NR34 = (NR34 & 0x38) | (value & 0xC7);

	// 0xFF1F unused?

	else if (address == 0xFF20)
		NR41 = value;

	else if (address == 0xFF21)
		NR42 = value;

	else if (address == 0xFF22)
		NR43 = value;

	else if (address == 0xFF23) // Only bits 6-7 used
		NR44 = (NR44 & 0x3F) | (value & 0xC0);

	else if (address == 0xFF24)
		NR50 = value;

	else if (address == 0xFF25)
		NR51 = value;

	else if (address == 0xFF26)
		NR52 = (NR52 & 0x7F) | (value & 0x80); // Only the highest bit is writeable

	else if (address >= 0xFF30 && address <= 0xFF3F) // TODO : Wave RAM isn't always accessible depending on other audio registers
	{
		printf("Wave RAM write ADR %x VAL %x\n", address, value);
		wave_ram[address - 0xFF30] = value;
	}

	if (address >= 0xFF10 && address <= 0xFF26) { printf("Audio register write ADR %x value %x\n", address, value); } // Audio
	else if (address >= 0xFF30 && address <= 0xFF3F) { printf("Wave pattern write ADR %x value %x\n", address, value); } // Wave pattern
}
