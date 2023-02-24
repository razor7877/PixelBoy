#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <string>
#include <fstream>
#include <format>
#include <algorithm>

#include "main.h"
#include "memory.h"
#include "cpu.h"
#include "interface.h"

uint8_t rom[0x8000]{}; // Enough space to load a 32 kB ROM

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
    fread(&rom, sizeof(char), size, romFile);
}

// Main code
int main(int, char**)
{
    load_rom("roms/02-interrupts.gb"); // Load ROM into rom variable
    // Load ROM into memory (32kB at most to not overflow into the rest of the address space)
    memcpy(memory, rom, 0x8000);

    start_interface();

    // Main loop
    for (;;)
    {
        //execute_cycle();
        update_interface();
    }

    stop_interface();

    return 0;
}
