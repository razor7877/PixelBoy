#include <cstdint>

#include "input.h"

uint8_t io_register = 0xFF;

void set_key(joypad key)
{
	io_register &= ~key;
}

void clear_keys()
{
	io_register |= 0x0F;
}

void clear_key(joypad key)
{
	io_register |= key;
}

bool get_key(joypad key)
{
	return (io_register & key);
}