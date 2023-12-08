#ifndef INPUT_H
#define INPUT_H

enum joypad
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
};

extern uint8_t io_register; // 0xFF00 : Joypad input

extern uint8_t SB; // 0xFF01 : Serial transfer data
extern uint8_t SC; // 0xFF02 : Serial transfer control

extern uint16_t DIV; // 0xFF04 : Divider register
extern uint8_t TIMA; // 0xFF05 : Timer counter
extern uint8_t TMA; // 0xFF06 : Timer modulo
extern uint8_t TAC; // 0xFF07 : Timer control

uint8_t read_io(uint16_t address);
void write_io(uint16_t address, uint8_t value);

void set_key(joypad key);
void clear_key(joypad key);
bool get_key(joypad key);

void update_timer_freq(uint8_t value);
void tick_timer(uint8_t cycles);

#endif