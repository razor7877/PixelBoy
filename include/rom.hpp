#ifndef ROM_H
#define ROM_H

#include <cstdint>
#include <string>

#define ROM_BANK_SIZE 16384

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

struct MBC1
{
	bool ram_enable;
	uint8_t rom_bank; // 5 bit register (upper 3 bits are discarded)
	uint8_t ram_bank;
	uint8_t banking_mode;
};

extern uint8_t boot_rom[256];
extern bool boot_done;
// Enough space to load a 32 kB ROM, no MBC support for now
extern uint8_t* rom;
extern uint32_t rom_size;
extern bool is_MBC_cartridge;

int load_rom(std::string path);
int load_boot_rom(std::string path);
void unload_rom();
void dump_header();

uint8_t read_rom(uint16_t address);
void write_rom(uint16_t address, uint8_t value);

#endif