#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rom.h"
#include "memory.h"
#include "cpu.h"
#include "mbc/mbc.h"
#include "mbc/mbc1.h"

struct CartridgeHeader cartridge_header = {0};
struct MBC mbc;

uint8_t boot_rom[256] = { 0 };

bool boot_done = true;
bool rom_loaded = false;
bool is_MBC_cartridge = false;

uint8_t* rom = 0;
uint32_t rom_size = 0;

uint8_t* external_ram = 0;
uint32_t external_ram_size = 0;

int load_rom(const char* path)
{
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

    uint8_t header[80] = {0};
    memcpy(header, &cartridge_header, 80);

    // Validate the checksum to ensure we have a valid ROM
    uint8_t checksum = 0;
    for (uint16_t address = 0x134; address <= 0x14C; address++)
    {
        checksum = checksum - header[address - 0x100] - 1;
    }

    if (checksum != cartridge_header.header_checksum)
    {
        printf("Incorrect header checksum, file is invalid!\n");
        return -1;
    }

    dump_header();

    // We dynamically allocate rom array depending on the cartridge type and size
    switch (cartridge_header.cartridge_type)
    {
    case 0x00: // ROM ONLY
        break;
    case 0x01: // MBC1
        mbc = mbc1;
        is_MBC_cartridge = true;
        break;
    case 0x02: // MBC1 + RAM
        mbc = mbc1;
        is_MBC_cartridge = true;
        break;
    case 0x03: // MBC3 + RAM + battery
        mbc = mbc1;
        is_MBC_cartridge = true;
        break;
    default:
        printf("Loaded ROM with a mapper that is not emulated: %x\n", cartridge_header.cartridge_type);
        return -1;
        break;
    }

    // Calculate how much size we need to allocate to fit in the full ROM
    rom_size = 2 * ROM_BANK_SIZE * (1 << cartridge_header.cartridge_size);

    rom = malloc(rom_size);
    if (rom == NULL)
    {
        // Handle allocation failure
        fprintf(stderr, "ROM memory allocation failed\n");
        return -1;
    }

    switch (cartridge_header.ram_size)
    {
    case 0x02:
        external_ram_size = 0x2000; // 8 KiB
        break;
    case 0x03:
        external_ram_size = 0x8000; // 32 KiB
        break;
    case 0x04:
        external_ram_size = 0x20000; // 128 KiB
        break;
    case 0x05:
        external_ram_size = 0x10000; // 64 KiB
        break;
    }

    if (external_ram_size != 0)
    {
        external_ram = malloc(external_ram_size);
        if (external_ram == NULL)
        {
            // Handle allocation failure
            fprintf(stderr, "External RAM memory allocation failed\n");
            return -1;
        }
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

    return 0;
}

void unload_rom()
{
    // Make sure header info is empty
    memset(&cartridge_header, 0, sizeof(cartridge_header));
    is_MBC_cartridge = false;

    free(rom);
    rom = NULL;
    rom_size = 0x00;

    free(external_ram);
    external_ram = NULL;
    external_ram_size = 0x00;

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
    else if (!is_MBC_cartridge) // If ROM only
        return rom[address];

    return mbc1.read_rom(address);
}

void write_rom(uint16_t address, uint8_t value)
{
    if (!is_MBC_cartridge)
    {
#ifdef ROM_DEBUG
        printf("Rom write attempt (no MBC) ADR %x VALUE %x\n", address, value);
#endif
    }
    else
    {
        mbc1.write_rom(address, value);
    }
}

uint8_t read_external_ram(uint16_t address)
{
    if (!mbc.ram_enable)
        return 0xFF;

    return external_ram[address - 0xA000];
    printf("External RAM read ADR %x\n", address);
}

void write_external_ram(uint16_t address, uint8_t value)
{
    if (!mbc.ram_enable)
        return;

    if (mbc1.banking_mode == 1)
    {
        printf("Bank mode 1 RAM write\n");
    }

    external_ram[address - 0xA000] = value;
    printf("External RAM write ADR %x VALUE %x\n", address, value);
}
