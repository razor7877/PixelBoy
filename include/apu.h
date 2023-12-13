#ifndef APU_H
#define APU_H

#include <stdint.h>

// Sound channel 1
extern uint8_t NR10;
extern uint8_t NR11;
extern uint8_t NR12;
extern uint8_t NR13;
extern uint8_t NR14;

// Sound channel 2
extern uint8_t NR21;
extern uint8_t NR22;
extern uint8_t NR23;
extern uint8_t NR24;

// Sound channel 3
extern uint8_t NR30;
extern uint8_t NR31;
extern uint8_t NR32;
extern uint8_t NR33;
extern uint8_t NR34;

// Sound channel 4
extern uint8_t NR41;
extern uint8_t NR42;
extern uint8_t NR43;
extern uint8_t NR44;

// Global registers
extern uint8_t NR50;
extern uint8_t NR51;
extern uint8_t NR52;

extern uint8_t wave_ram[16];

// Reads to the APU registers and wave RAM
uint8_t read_apu(uint16_t address);

// Writes to the APU registers and wave RAM
void write_apu(uint16_t address, uint8_t value);

#endif