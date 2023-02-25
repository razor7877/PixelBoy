#include <stdio.h>

#include "memory.h"
#include "input.h"
#include "interrupts.h"

uint8_t memory[0x10000]{};

uint8_t read_byte(uint16_t address)
{
	if (address == 0xFF44)
		return 0x90;

	else if (address == 0xFF00)
		return io_register;

	else if (address == 0xFF01)
		return SB;

	else if (address == 0xFF02)
		return SC;

	else if (address == 0xFF04)
		return DIV & 0xFF00 >> 8;

	else if (address == 0xFF0F)
		return IF;

	else if (address == 0xFFFF)
		return IE;

	return memory[address];
}

uint16_t read_word(uint16_t address)
{
	return read_byte(address) | (read_byte(address + 1) << 8);
}

void write_byte(uint16_t address, uint8_t value)
{
	memory[address] = value;

	if (address == 0xFF01) printf("%02x ", value);

	else if (address == 0xFF00) // Write to joypad register
		io_register = value & 0xF0;

	else if (address == 0xFF01) // Write to serial tranfer data
		SB = value;

	else if (address == 0xFF02) // Write to serial transfer control
		SC = value;

	else if (address == 0xFF04) // Reset divider register
		DIV = 0x00;

	else if (address == 0xFF0F) // Interrupts requests
		IF = value;

	else if (address == 0xFFFF) // Interrupts enable
		IE = value;
}

void write_word(uint16_t address, uint16_t value)
{
	write_byte(address, (value & 0x00FF));
	write_byte(address + 1, (value & 0xFF00) >> 8);
}
