#include <stdio.h>

#include "memory.h"
#include "input.h"

uint8_t memory[0x10000]{};

uint8_t read_byte(uint16_t address)
{
	if (address == 0xFF44)
		return 0x90;

	if (address == 0xFF00)
		return io_register;

	if (address == 0xFF01)
		return SB;

	if (address == 0xFF02)
		return SC;

	return memory[address];
}

uint16_t read_word(uint16_t address)
{
	return read_byte(address) | (read_byte(address + 1) << 8);
}

void write_byte(uint16_t address, uint8_t value)
{
	memory[address] = value;

	if (address == 0xFF01)
		io_register = value;

	if (address == 0xFF01)
		SB = value;

	if (address == 0xFF02)
		SC = value;
}

void write_word(uint16_t address, uint16_t value)
{
	write_byte(address, (value & 0x00FF));
	write_byte(address + 1, (value & 0xFF00) >> 8);
}
