#ifndef ROM_H
#define ROM_H

#include <stdint.h>
#include <stdbool.h>

// Use this define for ROM related logging
//#define ROM_DEBUG

struct CartridgeHeader
{
	const uint8_t entry_point[4];
	const uint8_t nintendo_logo[48];
	const uint8_t title[16]; // 0x134 - 0x143
	const uint8_t new_licensee_code[2]; // 0x144 - 0x145
	const uint8_t sgb_flag; // 0x146
	const uint8_t cartridge_type; // 0x147
	const uint8_t cartridge_size; // 0x148
	const uint8_t ram_size; // 0x149
	const uint8_t destination_code; // 0x14A
	const uint8_t old_licensee_code; // 0x14B
	const uint8_t mask_rom_version; // 0x14C
	const uint8_t header_checksum; // 0x14D
	const uint8_t global_checksum[2]; // 0x14E - 0x14F
};

extern struct CartridgeHeader cartridge_header;

extern uint8_t boot_rom[256];

extern bool boot_done;
extern bool rom_loaded;
extern bool is_MBC_cartridge;
// Space is dynamically allocated depending on the loaded ROM
extern uint8_t* rom;
extern uint32_t rom_size;
// Same for RAM
extern uint8_t* external_ram;
extern uint32_t external_ram_size;

int load_rom(const char* path);
int load_boot_rom(const char* path);
void unload_rom();
void save_ram_to_file();
void dump_header();

uint8_t read_rom(uint16_t address);
void write_rom(uint16_t address, uint8_t value);

uint8_t read_external_ram(uint16_t address);
void write_external_ram(uint16_t address, uint8_t value);

#endif