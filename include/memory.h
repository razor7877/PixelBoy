#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#include "cpu.h"

extern uint8_t wram[0x8000];
extern uint8_t hram[0x80];

// CGB only
extern uint8_t SVBK; // WRAM bank register

// Attempts to read 1 byte at given address
uint8_t read_byte(uint16_t address);
// Attempts to read 2 bytes at given address
uint16_t read_word(uint16_t address);

// Attempts to write 1 byte at given address
void write_byte(uint16_t address, uint8_t value);
// Attempts to write 2 bytes at given address
void write_word(uint16_t address, uint16_t value);

void reset_memory();

#endif