#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

typedef enum
{
	JOYPAD_ACTION = 0b00100000,
	JOYPAD_DIRECTION = 0b00010000,
	JOYPAD_DOWN = 0b00001000,
	JOYPAD_START = 0b00001000,
	JOYPAD_UP = 0b00000100,
	JOYPAD_SELECT = 0b00000100,
	JOYPAD_LEFT = 0b00000010,
	JOYPAD_B = 0b00000010,
	JOYPAD_RIGHT = 0b00000001,
	JOYPAD_A = 0b00000001,
} joypad;

extern uint8_t io_register; // 0xFF00 : Joypad input

extern uint8_t SB; // 0xFF01 : Serial transfer data
extern uint8_t SC; // 0xFF02 : Serial transfer control

extern uint8_t KEY1; // 0xFF4D : Prepare speed switch (CGB only)
extern uint8_t RP; // 0xFF56 : Infrared communications port (CGB only)

uint8_t read_io(uint16_t address);
void write_io(uint16_t address, uint8_t value);

void set_key(joypad key);
void clear_key(joypad key);
bool get_key(joypad key);

void reset_io();

#endif