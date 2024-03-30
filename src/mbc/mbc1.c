#include "mbc/mbc1.h"
#include "rom.h"

static uint8_t read_rom(uint16_t address)
{
	return 0xFF;
}

static void write_rom(uint16_t address, uint8_t value)
{

}

struct MBC mbc1 = {
    read_rom,
    write_rom,
    false,
    0,
    0,
    0,
};