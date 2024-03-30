#include "mbc/mbc1.h"
#include "rom.h"
#include "logging.h"

uint8_t read_rom_mbc1(uint16_t address)
{
	return 0xFF;
}

void write_rom_mbc1(uint16_t address, uint8_t value)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        log_debug("Update RAM enable register\n");
        if ((value & 0x0F) == 0x0A)
            mbc1.ram_enable = true;
        else
            mbc1.ram_enable = false;
    }

    // 5 bit ROM bank number register
    if (address >= 0x2000 && address <= 0x3FFF)
    {
        mbc1.rom_bank = value & 0x1F; // 5 lower bits mask
        if (mbc1.rom_bank == 0) // 0 value behaves as a 1 on real hardware
            mbc1.rom_bank = 1;

        log_debug("Update ROM bank number - Value: %d\n", mbc1.rom_bank);
    }

    // 2 bit RAM bank number register 
    if (address >= 0x4000 && address <= 0x5FFF)
    {
        mbc1.ram_bank = value & 0x03; // 2 lower bits mask
        log_debug("Update RAM bank number - Value: %d\n", mbc1.ram_bank);
    }

    // 1 bit banking mode register
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        mbc1.banking_mode = value & 0b1;
        //log_debug("Update banking mode select - Value: %x\n", mbc1.banking_mode);
    }
}

struct MBC mbc1 = {
    read_rom_mbc1,
    write_rom_mbc1,
    false,
    0,
    0,
    0,
};
