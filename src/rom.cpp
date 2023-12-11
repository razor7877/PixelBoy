#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>

#include "rom.hpp"
#include "memory.hpp"
#include "cpu.hpp"

uint8_t boot_rom[256]{};

bool boot_done = true;
uint8_t* rom;
uint32_t rom_size{};
bool is_MBC_cartridge{};

CartridgeHeader cartridge_header{};
MBC1 mbc1{}; // All MBC1 registers are initialized to 0 on GB, so zero-initializing struct should be fine

int load_rom(std::string path)
{
    // Make sure header info is empty
    memset(&cartridge_header, 0, sizeof(cartridge_header));

    FILE* romFile = fopen(path.c_str(), "rb");
    if (!romFile)
    {
        std::cout << "ERROR OPENING ROM FILE" << std::endl;
        return -1;
    }

    fseek(romFile, 0, SEEK_END); // Get file size
    size_t size = ftell(romFile);
    rewind(romFile);

    fseek(romFile, 0x100, SEEK_SET);
    // Header starts at 0x100 and is 80 bytes long, read it into the header struct
    fread(&cartridge_header, sizeof(uint8_t), 80, romFile);
    rewind(romFile);

    dump_header();

    // We dynamically allocate rom array depending on the cartridge type and size
    switch (cartridge_header.cartridge_type)
    {
    case 0x00: // ROM ONLY
        rom_size = 0x8000;
        break;
    case 0x01: // MBC1
        // With the MBC, number of ROM banks is 2 * 2**cartridge_size
        rom_size = 2 * ROM_BANK_SIZE * (1 << cartridge_header.cartridge_size);
        is_MBC_cartridge = true;
        break;
    default:
        return -1;
        break;
    }

    rom = new uint8_t[rom_size];

    // Finally, read full ROM contents into the array
    fread(rom, sizeof(uint8_t), rom_size, romFile);
    fclose(romFile);
}

int load_boot_rom(std::string path)
{
    boot_done = false;
    pc = 0x00;

    FILE* romFile = fopen(path.c_str(), "rb");
    if (!rom)
    {
        std::cout << "ERROR OPENING ROM FILE" << std::endl;
        return -1;
    }

    fseek(romFile, 0, SEEK_END); // Get file size
    size_t size = ftell(romFile);

    rewind(romFile);
    fread(&boot_rom, sizeof(char), size, romFile);
    fclose(romFile);
}

void unload_rom()
{
    delete[] rom;
    rom = nullptr;
    rom_size = 0x00;
}

void dump_header()
{
    printf("Title: %s\n", cartridge_header.title);
    printf("New licensee code: %x%x\n", cartridge_header.new_licensee_code[0], cartridge_header.new_licensee_code[1]);
    printf("SGB flag: %x\n", cartridge_header.sgb_flag);
    printf("Cartridge type: %x\n", cartridge_header.cartridge_type);
    printf("Cartridge size: %x\n", cartridge_header.cartridge_size);
    printf("Ram size: %x\n", cartridge_header.ram_size);
    printf("Destination code: %x\n", cartridge_header.destination_code);
    printf("Old licensee code: %x\n", cartridge_header.old_licensee_code);
    printf("Mask ROM version: %x\n", cartridge_header.mask_rom_version);
    printf("Header checksum: %x\n", cartridge_header.header_checksum);
    printf("Global checksum: %x%x\n", cartridge_header.global_checksum[0], cartridge_header.global_checksum[1]);
}

uint8_t read_rom(uint16_t address)
{
    if (!boot_done && address < 0xFF) // Accesses to 0x00-0xFF during startup are mapped to boot ROM
        return boot_rom[address];
    else if (!is_MBC_cartridge || address <= 0x3FFF) // If ROM only or address lower than 0x3FFF, we are reading the fixed memory bank 0
        return rom[address];

    // Otherwise, if MBC1, and reading one of the mapped banks, return content of the currently mapped bank
    return rom[ROM_BANK_SIZE * mbc1.rom_bank + address];
}

void write_rom(uint16_t address, uint8_t value)
{
    if (!is_MBC_cartridge)
    {
        printf("Rom write attempt (no MBC) ADR %x VALUE %x\n", address, value);
    }
    else if (address >= 0000 && address <= 0x1FFF) // Ram enable/disable 
    {
        if ((value & 0x0F) == 0x0A) // MBC1 enables RAM when the 4 lower bits of the written value equals 0xA
        {
            mbc1.ram_enable = true;
            printf("Write 0xA at ADR %x, cartridge RAM enable\n", address);
        }  
        else
        {
            mbc1.ram_enable = false;
            printf("Write %x at ADR %x, cartridge RAM enable\n", value, address);
        }
    }
    else if (address >= 0x2000 && address <= 0x3FFF) // Select ROM bank number
    {
        // We mask the written value to the number of bits required to choose among the available number of ROM banks
        uint8_t mask = (0b11 << cartridge_header.cartridge_size - 1);
        mbc1.rom_bank = value & mask;
        printf("Change ROM bank number VALUE %x\n", value);
    }
    else if (address >= 0x4000 && address <= 0x5FFF) // Select RAM bank number
    {
        printf("Change RAM bank number VALUE %x\n", value);
    }
    else if (address >= 0x6000 && address <= 0x7FFF) // Banking mode select
    {
        printf("Change banking mode VALUE %x\n", value);
    }
    else
    {
        printf("Rom write attempt. ADR %x VALUE %x\n", address, value);
    }
}
