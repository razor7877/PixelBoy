#include "mbc/mbc5.h"
#include "rom.h"

#include "logging.h"

static bool ram_enable = false;
static uint8_t rom_bank = 1;
static uint8_t rom_bank_msb = 0; // Most significant bit of rom bank number
static uint8_t ram_bank = 0;

static uint8_t read_rom_mbc5(uint16_t address)
{
	// Read ROM bank 00
	if (address >= 0x0000 && address <= 0x3FFF)
		return rom[address];

	// Read ROM banks
	if (address >= 0x4000 && address <= 0x7FFF)
	{
		// 14 lower bits are obtained from 14 lower bits of address
		// 9 bits from ROM bank number
		uint16_t full_rom_bank = (rom_bank_msb << 8) | rom_bank;

		// Keeps the ROM bank number in the range of ROM size
		uint8_t bank_count = (0b10 << cartridge_header.cartridge_size);
		if (full_rom_bank >= bank_count)
		{
			uint8_t mask = bank_count - 1;
			full_rom_bank &= mask;
		}
			
		uint32_t mapped_address = (full_rom_bank << 14) | (address & 0x3FFF);
		//log_info("MBC5 ROM bank read ADR %x VALUE %x %x\n", mapped_address, rom[mapped_address], rom[(1 << 14) | address]);
		return rom[mapped_address];
	}

	// Read RAM banks
	if (address >= 0xA000 && address <= 0xBFFF)
	{
		if (!ram_enable)
			return 0xFF;

		uint16_t mapped_address = (ram_bank << 12) | (address & 0x1FFF);

		return external_ram[mapped_address];
	}

	return 0xFF;
}

static void write_rom_mbc5(uint16_t address, uint8_t value)
{
	if (address >= 0x0000 && address <= 0x1FFF)
	{
		if ((value & 0x0F) == 0x0A)
			ram_enable = true;
		else
			ram_enable = false;
	}

	// 8 LSB of ROM bank number
	if (address >= 0x2000 && address <= 0x2FFF)
		rom_bank = value;

	// MSB of ROM bank number
	if (address >= 0x3000 && address <= 0x3FFF)
		rom_bank_msb = value & 0b1;

	//log_debug("ROM bank number %x\n", (rom_bank_msb << 8) | rom_bank);

	// RAM bank number
	if (address >= 0x4000 && address <= 0x5FFF)
	{
		ram_bank = value & 0x0F; // Range is 0x00-0x0F
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
		if (!ram_enable)
			return;

		uint16_t mapped_address = (ram_bank << 12) | (address & 0x1FFF);
		external_ram[mapped_address] = value;
	}

	//log_debug("MBC5 Rom bank number is: %x\n", (rom_bank_msb << 8) | rom_bank);
}

static void reset_mbc5()
{
	ram_enable = false;
	rom_bank = 1;
	rom_bank_msb = 0;
	ram_bank = 0;
}

struct MBC mbc5 = {
	read_rom_mbc5,
	write_rom_mbc5,
	reset_mbc5,
};