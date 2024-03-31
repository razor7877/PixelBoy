#include "mbc/mbc3.h"
#include "rom.h"

uint8_t read_rom_mbc3(uint16_t address)
{

}

void write_rom_mbc3(uint16_t address, uint8_t value)
{

}

struct MBC mbc3 = {
	read_rom_mbc3,
	write_rom_mbc3,
	false,
	0,
	0,
	0,
};