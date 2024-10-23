#ifndef APU_H
#define APU_H

#include <stdint.h>

#include "cpu.h"

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
	uint8_t r0; // NRx0
	uint8_t r1; // NRx1
	uint8_t r2; // NRx2
	uint8_t r3; // NRx3
	uint8_t r4; // NRx4
	int8_t volume; // Between 0x0 and 0xF, varies with envelope
	int8_t env_count;
	int period_counter;
	int current_amplitude;
} Channel;

typedef struct
{
	uint8_t r0; // NRx0
	uint8_t r1; // NRx1
	uint8_t r2; // NRx2
	uint8_t r3; // NRx3
	uint8_t r4; // NRx4
	int8_t volume; // Between 0x0 and 0xF, varies with envelope
	int8_t env_count;
	int period_counter;
	int waveform_index;
} SquareChannel;

extern SquareChannel NR1;
extern SquareChannel NR2;
extern Channel NR3;
extern Channel NR4;

#define TARGET_SAMPLE_RATE 44100
#define APU_SAMPLE_BUFFER_SIZE 1024
#define APU_SAMPLE_EMIT_CYCLES (CPU_FREQ / TARGET_SAMPLE_RATE)

typedef struct
{
	int sample_index; // Index of the current sample in the APU sample buffer
	float sample_buffer[APU_SAMPLE_BUFFER_SIZE]; // A buffer containing the samples emitted by the APU
	int apu_cycle_count;
} APUState;

extern APUState apu_state;

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
void tick_length_clocks();
void tick_sweep_clocks();
void tick_envelope_clocks();

void set_apu_reg(uint8_t* reg, uint8_t bitmask);
void unset_apu_reg(uint8_t* reg, uint8_t bitmask);

#endif