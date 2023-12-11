#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>

#include "rom.hpp"
#include "memory.hpp"
#include "cpu.hpp"

uint8_t boot_rom[256]{};
bool boot_done = true;
uint8_t rom[0x8000]{};

CartridgeHeader cartridge_header{};

int load_rom(std::string path)
{
    // Make sure header info is empty
    memset(&cartridge_header, 0, sizeof(cartridge_header));

    FILE* romFile = fopen(path.c_str(), "rb");
    if (!rom)
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

    // Finally, read full ROM contents into the array
    fread(&rom, sizeof(uint8_t), 0x8000, romFile);
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

void dump_header()
{
    printf("Title: %s\n", cartridge_header.title);
    printf("New licensee code: %x%x\n", cartridge_header.new_licensee_code[0], cartridge_header.new_licensee_code[1]);
    printf("SGB flag: %x\n", cartridge_header.sgb_flag);
    printf("Cartridge type: %x\n", cartridge_header.cartride_type);
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
    if (boot_done || address > 0xFF) // Accesses to 0x00-0xFF during startup are mapped to boot ROM
        return rom[address];
    else
        return boot_rom[address];
}

void write_rom(uint16_t address, uint8_t value)
{
    //printf("Rom write attempt. ADR %x VALUE %x\n", address, value);
}
