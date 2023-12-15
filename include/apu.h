#ifndef APU_H
#define APU_H

#include <stdint.h>

typedef enum
{
	AUDIO_ON = 0b10000000, // NR52
	CH4_ON = 0b00001000, // NR52
	CH3_ON = 0b00000100, // NR52
	CH2_ON = 0b00000010, // NR52
	CH1_ON = 0b00000001, // NR52
	WAVE_DUTY = 0b11000000, // NRx1
	LENGTH_TIMER = 0b00111111, // NRx1
	INITIAL_VOLUME = 0b11110000, // NRx2 except NR32
	ENV_DIR = 0b00001000, // NRx2 except NR32
	SWEEP_PACE = 0b00000111, // NRx2 except NR32
	OUTPUT_LEVEL = 0b01100000, // NR32
	// ADD NRx3 enums
	CH_TRIGGER = 0b10000000, // NRx4
	CH_LENGTH_ENABLE = 0b01000000, // NRx4
} apu_masks;

typedef struct
{
	uint8_t in; // The 4 bit input signal
	float out; // The output analog signal
} DAC;

extern DAC ch1_dac;
extern DAC ch2_dac;
extern DAC ch3_dac;
extern DAC ch4_dac;

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

extern uint8_t frame_sequencer;

// Reads to the APU registers and wave RAM
uint8_t read_apu(uint16_t address);
// Writes to the APU registers and wave RAM
void write_apu(uint16_t address, uint8_t value);

void tick_apu(uint8_t cycles);
void tick_frame_sequencer();

#endif