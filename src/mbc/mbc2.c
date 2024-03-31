#include "mbc/mbc1.h"
#include "rom.h"
#include "logging.h"

uint8_t read_rom_mbc2(uint16_t address)
{

}

void write_rom_mbc2(uint16_t address, uint8_t value)
{
	// RAM enable or ROM bank number
	if (address >= 0x0000 && address <= 0x3FFF)
	{
		// If bit 8 is set: change ROM bank number
		if (address & 0x0100)
		{

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