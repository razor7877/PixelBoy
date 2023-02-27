#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>

#include "rom.h"
#include "memory.h"
#include "cpu.h"

uint8_t boot_rom[256]{};
bool boot_done = true;
uint8_t rom[0x8000]{};

int load_rom(std::string path)
{
    FILE* romFile = fopen(path.c_str(), "rb");
    if (!rom)
    {
        std::cout << "ERROR OPENING ROM FILE" << std::endl;
        return -1;
    }

    fseek(romFile, 0, SEEK_END); // Get file size
    size_t size = ftell(romFile);

    rewind(romFile);
    fread(&rom, sizeof(char), 0x8000, romFile);
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
    printf("Rom write attempt. ADR %x VALUE %x\n", address, value);
}
