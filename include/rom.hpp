#ifndef ROM_H
#define ROM_H

#include <cstdint>
#include <string>

extern uint8_t boot_rom[256];
extern bool boot_done;
// Enough space to load a 32 kB ROM, no MBC support for now
extern uint8_t rom[0x8000];

int load_rom(std::string path);
int load_boot_rom(std::string path);

uint8_t read_rom(uint16_t address);
void write_rom(uint16_t address, uint8_t value);

#endif