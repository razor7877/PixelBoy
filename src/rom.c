#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>

#include "rom.h"
#include "memory.h"
#include "cpu.h"

// Use this define for ROM related logging
//#define ROM_DEBUG

uint8_t boot_rom[256] = {0};

bool boot_done = true;
bool rom_loaded = false;
uint8_t* rom = 0;
uint32_t rom_size = 0;
bool is_MBC_cartridge = false;

struct CartridgeHeader cartridge_header = {0};
struct MBC1 mbc1 = {0}; // All MBC1 registers are initialized to 0 on GB, so zero-initializing struct should be fine

int load_rom(const char* path)
{
    // Make sure header info is empty
    memset(&cartridge_header, 0, sizeof(cartridge_header));
    is_MBC_cartridge = false;

    FILE* romFile = fopen(path, "rb");
    if (!romFile)
    {
        printf("Error opening ROM file!\n");
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
        printf("Loaded ROM with a mapper that is not emulated: %x\n", cartridge_header.cartridge_type);
        return -1;
        break;
    }

    rom = malloc(rom_size);

    if (rom == NULL)
    {
        // Handle allocation failure
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    // Finally, read full ROM contents into the array
    fread(rom, sizeof(uint8_t), rom_size, romFile);
    fclose(romFile);

    rom_loaded = true;

    return 0;
}

int load_boot_rom(const char* path)
{
    boot_done = false;
    pc = 0x00;

    FILE* romFile = fopen(path, "rb");
    if (!rom)
    {
        printf("Error opening BOOT ROM file!\n");
        return -1;
    }

    fseek(romFile, 0, SEEK_END); // Get file size
    size_t size = ftell(romFile);

    rewind(romFile);
    fread(&boot_rom, sizeof(char), size, romFile);
    fclose(romFile);

    // Dirty fix for blargg's indidivual test ROMs having cartridge size of 0 (which causes incorrect ROM reads)
    if (cartridge_header.cartridge_type == 0x01 && cartridge_header.cartridge_size == 0x00)
    {
        memset((void*)&(cartridge_header.cartridge_size), 0x01, sizeof(char));
    }

    return 0;
}

void unload_rom()
{
    free(rom);
    rom = NULL;
    rom_size = 0x00;

    rom_loaded = false;
}

void dump_header()
{
    printf("Loaded ROM into memory, cartridge header info:\n\nTitle: %s\n", cartridge_header.title);
    printf("New licensee code: %x%x\n", cartridge_header.new_licensee_code[0], cartridge_header.new_licensee_code[1]);
    printf("SGB flag: %x\n", cartridge_header.sgb_flag);
    printf("Cartridge type: %x\n", cartridge_header.cartridge_type);
    printf("Cartridge size: %x\n", cartridge_header.cartridge_size);
    printf("Ram size: %x\n", cartridge_header.ram_size);
    printf("Destination code: %x\n", cartridge_header.destination_code);
    printf("Old licensee code: %x\n", cartridge_header.old_licensee_code);
    printf("Mask ROM version: %x\n", cartridge_header.mask_rom_version);
    printf("Header checksum: %x\n", cartridge_header.header_checksum);
    printf("Global checksum: %x%x\n\n", cartridge_header.global_checksum[0], cartridge_header.global_checksum[1]);
}

uint8_t read_rom(uint16_t address)
{
    if (!boot_done && address < 0xFF) // Accesses to 0x00-0xFF during startup are mapped to boot ROM
        return boot_rom[address];
    else if (!is_MBC_cartridge || address <= 0x3FFF) // If ROM only or address lower than 0x3FFF, we are reading the fixed memory bank 0
        return rom[address];

    // Otherwise, if MBC1, and reading one of the mapped banks, return content of the currently mapped bank
    return rom[ROM_BANK_SIZE * (mbc1.rom_bank - 1) + address];
}

void write_rom(uint16_t address, uint8_t value)
{
    if (!is_MBC_cartridge)
    {
#ifdef ROM_DEBUG
        printf("Rom write attempt (no MBC) ADR %x VALUE %x\n", address, value);
#endif
    }
    else if (address >= 0000 && address <= 0x1FFF) // Ram enable/disable 
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
            printf("Write %x at ADR %x, cartridge RAM enable\n", value, address);
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
        printf("Change ROM bank number VALUE %x\n", value);
#endif
    }
    else if (address >= 0x4000 && address <= 0x5FFF) // Select RAM bank number
    {
#ifdef ROM_DEBUG
        printf("Change RAM bank number VALUE %x\n", value);
#endif
    }
    else if (address >= 0x6000 && address <= 0x7FFF) // Banking mode select
    {
#ifdef ROM_DEBUG
        printf("Change banking mode VALUE %x\n", value);
#endif
    }
    else
    {
#ifdef ROM_DEBUG
        printf("Rom write attempt. ADR %x VALUE %x\n", address, value);
#endif
    }
}
