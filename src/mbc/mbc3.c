#include "mbc/mbc3.h"
#include "rom.h"

uint8_t read_rom_mbc3(uint16_t address)
{
	// Read ROM bank 00
	if (address >= 0x0000 && address <= 0x3FFF)
		return rom[address];
	
	// Read ROM banks 01-7F
	if (address >= 0x4000 && address <= 0x7FFF)
	{
		// 14 lower bits are obtained from 14 lower bits of address
		// 5 next bits obtained from the selected rom bank number
		uint32_t mapped_address = (mbc3.rom_bank << 14) | (address & 0x3FFF);
		return rom[mapped_address];
	}

	// Read RAM bank or RTC register
	if (address >= 0xA000 && address <= 0xBFFF)
	{

	}
}

void write_rom_mbc3(uint16_t address, uint8_t value)
{
	// Enable/disable RAM banks and RTC registers
	if (address >= 0x0000 && address <= 0x1FFF)
	{
		if ((value & 0x0F) == 0x0A)
			mbc3.ram_enable = true;
		else
			mbc3.ram_enable = false;
	}

	// 7 bit ROM bank number register
	if (address >= 0x2000 && address <= 0x3FFF)
	{
		mbc3.rom_bank = value & 0x7F; // 7 lower bits mask
		if (mbc3.rom_bank == 0) // 0 value behaves as a 1 on real hardware
			mbc3.rom_bank = 1;
	}

	// Write to RAM bank or RTC register
	if (address >= 0xA000 && address <= 0xBFFF)
	{

	}
}

struct MBC mbc3 = {
	read_rom_mbc3,
	write_rom_mbc3,
	false,
	0,
	0,
	0,
};