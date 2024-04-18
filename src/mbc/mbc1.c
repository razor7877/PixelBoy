#include "mbc/mbc1.h"
#include "rom.h"
#include "logging.h"

static bool ram_enable = false;
static uint8_t rom_bank = 0;
static uint8_t ram_bank = 0;
static uint8_t banking_mode = 0;

static uint8_t read_rom_mbc1(uint16_t address)
{
    // Read ROM bank 00
    if (address >= 0x0000 && address <= 0x3FFF)
        return rom[address];

    // Read ROM banks 01-7F
    if (address >= 0x4000 && address <= 0x7FFF)
    {
        // 14 lower bits are obtained from 14 lower bits of address
        // 5 next bits obtained from the selected rom bank number
        if (rom_bank == 0)
            rom_bank = 1;

        uint8_t bank_number = rom_bank;
        uint8_t bank_count = (0b10 << cartridge_header.cartridge_size);

        // Keeps the ROM bank number in the range of ROM size
        if (rom_bank >= bank_count)
        {
            uint8_t mask = bank_count - 1;
            bank_number = rom_bank & mask;
        }

        uint32_t mapped_address = (bank_number << 14) | (address & 0x3FFF);
        return rom[mapped_address];
    }

    // Read RAM banks 00-03
    if (address >= 0xA000 && address <= 0xBFFF)
    {
        //log_debug("External RAM read ADR %x - ram_enable is %x\n", address, ram_enable);
        // Read RAM banks 00-03
        if (!ram_enable)
            return 0xFF;

        uint16_t mapped_address = (ram_bank << 12) | (address & 0x1FFF);

        return external_ram[mapped_address];
    }

	return 0xFF;
}

static void write_rom_mbc1(uint16_t address, uint8_t value)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        if ((value & 0x0F) == 0x0A)
            ram_enable = true;
        else
            ram_enable = false;
    }

    // 5 bit ROM bank number register
    if (address >= 0x2000 && address <= 0x3FFF)
    {
        rom_bank = value & 0x1F; // 5 lower bits mask
    } 

    // 2 bit RAM bank number register 
    if (address >= 0x4000 && address <= 0x5FFF)
        ram_bank = value & 0x03; // 2 lower bits mask

    // 1 bit banking mode register
    if (address >= 0x6000 && address <= 0x7FFF)
        banking_mode = value & 0b1;

    if (address >= 0xA000 && address <= 0xBFFF)
    {
        if (!ram_enable)
            return;

        uint16_t mapped_address = (ram_bank << 12) | (address & 0x1FFF);
        external_ram[mapped_address] = value;
        //log_debug("External RAM write ADR %x VALUE %x\n", address, value);
    }
}

static void reset_mbc1()
{
    ram_enable = false;
    rom_bank = 0;
    ram_bank = 0;
    banking_mode = 0;
}

struct MBC mbc1 = {
    read_rom_mbc1,
    write_rom_mbc1,
    reset_mbc1,
};
