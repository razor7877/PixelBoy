#include "mbc/mbc5.h"
#include "rom.h"

#include "logging.h"

// Most significant bit of rom bank number
uint8_t rom_bank_msb = 0;

uint8_t read_rom_mbc5(uint16_t address)
{
	// Read ROM bank 00
	if (address >= 0x0000 && address <= 0x3FFF)
		return rom[address];

	// Read ROM banks
	if (address >= 0x4000 && address <= 0x7FFF)
	{
		// 14 lower bits are obtained from 14 lower bits of address
		// 9 bits from ROM bank number
		uint16_t rom_bank = (rom_bank_msb << 8) | mbc5.rom_bank;
		if (rom_bank == 0)
			rom_bank = 1;
		uint32_t mapped_address = (rom_bank << 14) | (address & 0x3FFF);
		//log_info("MBC5 ROM bank read ADR %x VALUE %x\n", address, rom[mapped_address]);
		return rom[mapped_address];
	}

	// Read RAM banks
	if (address >= 0xA000 && address <= 0xBFFF)
	{
		if (!mbc5.ram_enable)
			return 0xFF;

		uint16_t mapped_address = (mbc5.ram_bank << 12) | (address & 0x1FFF);

		return external_ram[mapped_address];
	}

	return 0xFF;
}

void write_rom_mbc5(uint16_t address, uint8_t value)
{
	if (address >= 0x0000 && address <= 0x1FFF)
	{
		if ((value & 0x0F) == 0x0A)
			mbc5.ram_enable = true;
		else
			mbc5.ram_enable = false;
	}

	// 8 LSB of ROM bank number
	if (address >= 0x2000 && address <= 0x2FFF)
		mbc5.rom_bank = value;

	// MSB of ROM bank number
	if (address >= 0x3000 && address <= 0x3FFF)
		rom_bank_msb = value & 0b1;

	// RAM bank number
	if (address >= 0x4000 && address <= 0x5FFF)
	{
		mbc5.ram_bank = value & 0x0F; // Range is 0x00-0x0F
		if (value & 0x08) // If bit 3 is set (rumble)
		{
			// Any rumble logic would go there - Toggle ON
		}
		else
		{
			// Toggle Off
		}
	}

	// RAM writes
	if (address >= 0xA000 && address <= 0xBFFF)
	{
		if (!mbc5.ram_enable)
			return;

		uint16_t mapped_address = (mbc5.ram_bank << 12) | (address & 0x1FFF);
		external_ram[mapped_address] = value;
	}
}

struct MBC mbc5 = {
	read_rom_mbc5,
	write_rom_mbc5,
	false,
	0,
	0,
	0,
};