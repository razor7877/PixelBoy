#ifndef MBC_H
#define MBC_H

#include <stdbool.h>
#include <stdint.h>

#define ROM_BANK_SIZE 0x4000

// TODO : Keep only read/write pointers in struct, all other MBC specific variables in implementations?
// A generic abstraction to easily implement the different types of mappers
typedef struct MBC
{
	uint8_t (*read_rom)(uint16_t address);
	void (*write_rom)(uint16_t address, uint8_t value);
	void (*reset_mbc)();
} MBC;

#endif
