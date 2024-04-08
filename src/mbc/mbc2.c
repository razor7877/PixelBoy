#include <string.h>

#include "mbc/mbc2.h"
#include "rom.h"

static bool ram_enable = false;
static uint8_t rom_bank = 1;
static uint8_t ram_bank = 0;

static uint8_t mbc2_ram[512] = { 0 };

static uint8_t read_rom_mbc2(uint16_t address)
{
	// Read ROM bank 00
	if (address >= 0x0000 && address <= 0x3FFF)
		return rom[address];

	// Read ROM banks 01-7F
	if (address >= 0x4000 && address <= 0x7FFF)
	{
		// 14 lower bits are obtained from 14 lower bits of address
		// 5 next bits obtained from the selected rom bank number
		if (rom_bank == 0)
			rom_bank = 1;
		uint32_t mapped_address = (rom_bank << 14) | (address & 0x3FFF);
		return rom[mapped_address];
	}

	// Read MBC2 built-in RAM
	if (address >= 0xA000 && address <= 0xBFFF)
	{
		if (ram_enable)
			return mbc2_ram[address & 0x01FF];
		else
			return 0xFF;
	}

	return 0xFF;
}

static void write_rom_mbc2(uint16_t address, uint8_t value)
{
	// RAM enable or ROM bank number
	if (address >= 0x0000 && address <= 0x3FFF)
	{
		// If bit 8 is set: change ROM bank number
		if (address & 0x0100)
		{
			rom_bank = value & 0x0F; // Lower 4 bits specify ROM bank number
		}
		// If bit 8 is unset: RAM enable/disable
		else
		{
			if (value == 0x0A)
				ram_enable = true;
			else
				ram_enable = false;
		}
	}
}

static void reset_mbc2()
{
	ram_enable = false;
	rom_bank = 1;
	ram_bank = 0;

	memset(mbc2_ram, 0, sizeof(mbc2_ram));
}

struct MBC mbc2 = {
	read_rom_mbc2,
	write_rom_mbc2,
	reset_mbc2
};