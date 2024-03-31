#ifndef MBC_H
#define MBC_H

#include <stdbool.h>
#include <stdint.h>

#define ROM_BANK_SIZE 0x4000

// A generic abstraction to easily implement the different types of mappers
typedef struct MBC
{
	uint8_t (*read_rom)(uint16_t address);
	void (*write_rom)(uint16_t address, uint8_t value);
	bool ram_enable;
	uint8_t rom_bank; // 5 bit register (upper 3 bits are discarded)
	uint8_t ram_bank;
	uint8_t banking_mode;
} MBC;

#endif
