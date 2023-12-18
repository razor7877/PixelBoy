#include "mbc/mbc1.h"
#include "rom.h"

static uint8_t read_rom(uint16_t address)
{
    if (address <= 0x3FFF)
    {
        if (mbc1.banking_mode == 0) // In banking mode 0, always read bank 0
            return rom[address];
        else // Otherwise bank number depends on second ROM/RAM bank register
        {
            uint32_t bank = (mbc1.ram_bank << 5);
            printf("Ram bank: %x, shifted: %x\n", mbc1.ram_bank, bank);

            // We mask the written value to the number of bits required to choose among the available number of ROM banks
            uint8_t mask = (0b11 << cartridge_header.cartridge_size);
            bank &= mask;

            rom[ROM_BANK_SIZE * (bank - 1) + address];
        }
    }

    uint32_t bank;
    if (mbc1.banking_mode == 0)
        bank = mbc1.rom_bank;
    else
        bank = (mbc1.ram_bank << 5) | (mbc1.rom_bank);

    // We mask the written value to the number of bits required to choose among the available number of ROM banks
    uint8_t mask = (0b11 << cartridge_header.cartridge_size);
    bank &= mask;

    // Otherwise, if MBC1, and reading one of the mapped banks, return content of the currently mapped bank
    return rom[ROM_BANK_SIZE * (bank - 1) + address];
}

static void write_rom(uint16_t address, uint8_t value)
{
    if (address >= 0000 && address <= 0x1FFF) // Ram enable/disable 
    {
        if ((value & 0x0F) == 0x0A) // MBC1 enables RAM when the 4 lower bits of the written value equals 0xA
        {
            mbc1.ram_enable = true;
#ifdef ROM_DEBUG
            printf("Write 0xA at ADR %x, cartridge RAM enable\n", address);
#endif
        }
        else
        {
            mbc1.ram_enable = false;
#ifdef ROM_DEBUG
            printf("Write %x at ADR %x, cartridge RAM disable\n", value, address);
#endif
        }
    }
    else if (address >= 0x2000 && address <= 0x3FFF) // Select ROM bank number
    {
        // We mask the written value to the number of bits required to choose among the available number of ROM banks
        uint8_t mask = (0b11 << cartridge_header.cartridge_size - 1);
        mbc1.rom_bank = value & mask;
        if (mbc1.rom_bank == 0x00) mbc1.rom_bank = 0x01; // Assumed that we can safely change value to 1, since 0 maps to 1 anyway on real hardware
#ifdef ROM_DEBUG
        //printf("Change ROM bank number VALUE %x\n", value);
#endif
    }
    else if (address >= 0x4000 && address <= 0x5FFF) // Select RAM bank number OR ROM bank number on large ROMs
    {
        mbc1.ram_bank = value & 0b11;
#ifdef ROM_DEBUG
        printf("Change RAM bank number VALUE %x\n", value);
#endif
    }
    else if (address >= 0x6000 && address <= 0x7FFF) // Banking mode select
    {
        mbc1.banking_mode = value & 0b1;
#ifdef ROM_DEBUG
        printf("Change banking mode VALUE %x\n", value);
#endif
    }
}

struct MBC mbc1 = {
    read_rom,
    write_rom,
    false,
    1,
    0,
    0,
};
