#include "mbc/mbc1.h"
#include "rom.h"

uint8_t read_rom_mbc1(uint16_t address)
{
	return 0xFF;
}

void write_rom_mbc1(uint16_t address, uint8_t value)
{

}

struct MBC mbc1 = {
    read_rom_mbc1,
    write_rom_mbc1,
    false,
    0,
    0,
    0,
};
