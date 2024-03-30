#include "mbc/mbc1.h"
#include "rom.h"
#include "logging.h"

uint8_t read_rom_mbc1(uint16_t address)
{
    // Read ROM bank 00
    if (address >= 0x0000 && address <= 0x3FFF)
        return rom[address];

    // Read ROM banks 01-7F
    if (address >= 0x4000 && address <= 0x7FFF)
    {
        // 14 lower bits are obtained from 14 lower bits of address
        // 5 next bits obtained from the selected rom bank number
        uint32_t mapped_address = (mbc1.rom_bank << 14) |(address & 0x3FFF);
        return rom[mapped_address];
    }

    // Read RAM banks 00-03
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        log_debug("External RAM read ADR %x - ram_enable is %x\n", address, mbc1.ram_enable);
        // Read RAM banks 00-03
        if (!mbc1.ram_enable)
            return 0xFF;

        uint16_t mapped_address = (mbc1.ram_bank << 12) | (address & 0x1FFF);

        return external_ram[mapped_address];
    }

	return 0xFF;
}

void write_rom_mbc1(uint16_t address, uint8_t value)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        //log_debug("Update RAM enable register\n");
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

        //log_debug("Update ROM bank number - Value: %d\n", mbc1.rom_bank);
    }

    // 2 bit RAM bank number register 
    if (address >= 0x4000 && address <= 0x5FFF)
    {
        mbc1.ram_bank = value & 0x03; // 2 lower bits mask
        //log_debug("Update RAM bank number - Value: %d\n", mbc1.ram_bank);
    }

    // 1 bit banking mode register
    if (address >= 0x6000 && address <= 0x7FFF)
    {
        mbc1.banking_mode = value & 0b1;
        //log_debug("Update banking mode select - Value: %x\n", mbc1.banking_mode);
    }

    if (address >= 0xA000 && address <= 0xBFFF)
    {
        if (!mbc1.ram_enable)
            return;

        if (mbc1.banking_mode == 1)
        {
            log_debug("Bank mode 1 RAM write\n");
        }

        uint16_t mapped_address = (mbc1.ram_bank << 12) | (address & 0x1FFF);
        external_ram[mapped_address] = value;
        log_debug("External RAM write ADR %x VALUE %x\n", address, value);
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
