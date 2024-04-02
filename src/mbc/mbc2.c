#include "mbc/mbc2.h"
#include "rom.h"

uint8_t mbc2_ram[512] = { 0 };

uint8_t read_rom_mbc2(uint16_t address)
{
	// Read ROM bank 00
	if (address >= 0x0000 && address <= 0x3FFF)
		return rom[address];

	// Read ROM banks 01-7F
	if (address >= 0x4000 && address <= 0x7FFF)
	{
		// 14 lower bits are obtained from 14 lower bits of address
		// 5 next bits obtained from the selected rom bank number
		uint8_t rom_bank = mbc2.rom_bank;
		if (rom_bank == 0)
			rom_bank = 1;
		uint32_t mapped_address = (rom_bank << 14) | (address & 0x3FFF);
		return rom[mapped_address];
	}

	// Read MBC2 built-in RAM
	if (address >= 0xA000 && address <= 0xBFFF)
	{
		if (mbc2.ram_enable)
			return mbc2_ram[address & 0x01FF];
		else
			return 0xFF;
	}

	return 0xFF;
}

void write_rom_mbc2(uint16_t address, uint8_t value)
{
	// RAM enable or ROM bank number
	if (address >= 0x0000 && address <= 0x3FFF)
	{
		// If bit 8 is set: change ROM bank number
		if (address & 0x0100)
		{
			mbc2.rom_bank = value & 0x0F; // Lower 4 bits specify ROM bank number
		}
		// If bit 8 is unset: RAM enable/disable
		else
		{
			if (value == 0x0A)
				mbc2.ram_enable = true;
			else
				mbc2.ram_enable = false;
		}
	}
}

struct MBC mbc2 = {
	read_rom_mbc2,
	write_rom_mbc2,
	false,
	1,
	0,
	0,
};