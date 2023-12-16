#include <stdio.h>

#include "portaudio.h"

#include "apu.h"

// Logging info regarding APU registers and wave ram R/W
//#define APU_DEBUG

// Square channel 1
Channel NR1 = {
	0x80,
	0xBF,
	0xF3,
	0xFF,
	0xBF,
	0
};

// Square channel 2
Channel NR2 = {
	0, // Unused
	0x3F,
	0x00,
	0xFF,
	0xBF,
	0
};

// Wave channel
Channel NR3 = {
	0x7F,
	0xFF,
	0x9F,
	0xFF,
	0xBF,
	0
};

// Noise channel
Channel NR4 = {
	0, // Unused
	0xFF,
	0x00,
	0x00,
	0xBF,
	0
};

// Global registers
uint8_t NR50 = 0x77;
uint8_t NR51 = 0xF3;
uint8_t NR52 = 0xF1;

uint8_t wave_ram[16] = {0};

uint8_t frame_sequencer = 0;

uint8_t read_apu(uint16_t address)
{
	if (address == 0xFF10)
		return NR1.r0;

	else if (address == 0xFF11) // Only bits 6-7 are readable
		return (NR1.r1 & 0xC0);

	else if (address == 0xFF12)
		return NR1.r2;

	else if (address == 0xFF13) // NR1.r3 is write-only
		return 0xFF;

	else if (address == 0xFF14) // Return bits 3-6 (0-2 and 7 are write-only)
		return (NR1.r4 & 0x78);

	// 0xFF15 unused

	else if (address == 0xFF16)
		return NR2.r1;

	else if (address == 0xFF17)
		return NR2.r2;

	else if (address == 0xFF18) // NR2.r3 is write-only
		return 0xFF;

	else if (address == 0xFF19) // Return bits 3-6 (0-2 and 7 are write-only)
		return (NR2.r4 & 0x78);

	else if (address == 0xFF1A)
		return NR3.r0;

	else if (address == 0xFF1B) // Write-only
		return 0xFF;

	else if (address == 0xFF1C)
		return NR3.r2;

	else if (address == 0xFF1D) // Write-only
		return 0xFF;

	else if (address == 0xFF1E) // Only bit 6 readable
		return (NR3.r4 & 0x40);

	// 0xFF1F unused?

	else if (address == 0xFF20)
		return NR4.r1;

	else if (address == 0xFF21)
		return NR4.r2;

	else if (address == 0xFF22)
		return NR4.r3;

	else if (address == 0xFF23) // Highest bit is write only
		return (NR4.r4 & 0x7F);

	else if (address == 0xFF24)
		return NR50;

	else if (address == 0xFF25)
		return NR51;

	else if (address == 0xFF26)
		return NR52;

	else if (address >= 0xFF30 && address <= 0xFF3F)
	{
#ifdef APU_DEBUG
		printf("Wave RAM read ADR %x\n", address);
#endif
		return wave_ram[address - 0xFF30];
	}

#ifdef APU_DEBUG
	if (address >= 0xFF10 && address <= 0xFF26) { printf("Audio register read ADR %x\n", address); }
#endif

	return 0xFF;
}

void write_apu(uint16_t address, uint8_t value)
{
	// If APU disabled, registers are read only
	if ((NR52 & 0x80) == 0)
		return;

	if (address == 0xFF10)
		NR1.r0 = value;

	else if (address == 0xFF11)
		NR1.r1 = value;

	else if (address == 0xFF12)
		NR1.r2 = value;

	else if (address == 0xFF13)
		NR1.r3 = value;

	else if (address == 0xFF14)
	{
		NR1.r4 = (NR1.r4 & 0x38) | (value & 0xC7); // Bits 3-5 are unused
		if (value & 0x80)
		{
			set_apu_reg(&NR52, CH2_ON);
			//printf("Toggle on CH1\n");
		}
		else
		{
			unset_apu_reg(&NR52, CH2_ON);
			//printf("Toggle off CH1\n");
		}
	}
		

	// 0xFF15 unused

	else if (address == 0xFF16)
		NR2.r1 = value;

	else if (address == 0xFF17)
		NR2.r2 = value;

	else if (address == 0xFF18)
		NR2.r3 = value;

	else if (address == 0xFF19)
	{
		NR2.r4 = (NR2.r4 & 0x38) | (value & 0xC7); // Bits 3-5 are unused
		if (value & 0x80)
		{
			set_apu_reg(&NR52, CH2_ON);
			//printf("Toggle on CH2\n");
		}
		else
		{
			unset_apu_reg(&NR52, CH2_ON);
			//printf("Toggle off CH2\n");
		}
	}

	else if (address == 0xFF1A)
		NR3.r0 = (NR3.r0 & 0x7F) | (value & 0x80); // Only bit 7 is used

	else if (address == 0xFF1B)
		NR3.r1 = value;

	else if (address == 0xFF1C)
		NR3.r2 = (NR3.r2 & 0x9F) | (value & 0x60); // Only bits 5-6 are used

	else if (address == 0xFF1D)
		NR3.r3 = value;

	else if (address == 0xFF1E)
	{
		NR3.r4 = (NR3.r4 & 0x38) | (value & 0xC7); // Bits 3-5 are unused
		if (value & 0x80)
		{
			set_apu_reg(&NR52, CH3_ON);
			//printf("Toggle on CH3\n");
		}
		else
		{
			unset_apu_reg(&NR52, CH3_ON);
			//printf("Toggle off CH3\n");
		}
	}

	// 0xFF1F unused?

	else if (address == 0xFF20)
		NR4.r1 = value;

	else if (address == 0xFF21)
		NR4.r2 = value;

	else if (address == 0xFF22)
		NR4.r3 = value;

	else if (address == 0xFF23)
	{
		NR4.r4 = (NR4.r4 & 0x3F) | (value & 0xC0); // Only bits 6-7 used
		if (value & 0x80)
		{
			set_apu_reg(&NR52, CH4_ON);
			//printf("Toggle on CH4\n");
		}
		else
		{
			unset_apu_reg(&NR52, CH4_ON);
			//printf("Toggle off CH4\n");
		}
	}

	else if (address == 0xFF24)
		NR50 = value;

	else if (address == 0xFF25)
		NR51 = value;

	else if (address == 0xFF26)
		NR52 = (NR52 & 0x7F) | (value & 0x80); // Only the highest bit is writeable

	else if (address >= 0xFF30 && address <= 0xFF3F) // TODO : Wave RAM isn't always accessible depending on other audio registers
	{
		//printf("Wave RAM write ADR %x VAL %x\n", address, value);

		wave_ram[address - 0xFF30] = value;
	}

#ifdef APU_DEBUG
	if (address >= 0xFF10 && address <= 0xFF26) { printf("Audio register write ADR %x value %x\n", address, value); } // Audio
	else if (address >= 0xFF30 && address <= 0xFF3F) { printf("Wave pattern write ADR %x value %x\n", address, value); } // Wave pattern
#endif
}

void tick_apu(uint8_t cycles)
{

}

void tick_frame_sequencer()
{
	// If audio disabled
	if ((NR52 & AUDIO_ON) == 0)
		return;

	// Length control clock
	if (frame_sequencer % 2 == 0)
		tick_length_clocks();
		
	// Sweep clock
	if (frame_sequencer == 2 || frame_sequencer == 6)
		tick_sweep_clocks();

	// Envelope clock
	if (frame_sequencer++ == 7)
	{
		tick_envelope_clocks();
		frame_sequencer = 0;
	}
}

void tick_length_clocks()
{
	// If channel 1 on and length timer enabled
	if ((NR1.r4 & (CH_TRIGGER | CH_LENGTH_ENABLE)) == (CH_TRIGGER | CH_LENGTH_ENABLE))
	{
		uint8_t counter = NR1.r1 & LENGTH_TIMER;
		// When length timer reaches 64, disable the channel
		if (counter++ >= 64)
		{
			unset_apu_reg(&NR1.r4, CH_TRIGGER);
			unset_apu_reg(&NR52, CH1_ON);
		}
		NR1.r1 = (NR1.r1 & WAVE_DUTY) | (counter & LENGTH_TIMER);
	}
	// If channel 2 on and length timer enabled
	if ((NR2.r4 & (CH_TRIGGER | CH_LENGTH_ENABLE)) == (CH_TRIGGER | CH_LENGTH_ENABLE))
	{
		uint8_t counter = NR2.r1 & LENGTH_TIMER;
		// When length timer reaches 64, disable the channel
		if (counter++ >= 64)
		{
			unset_apu_reg(&NR1.r4, CH_TRIGGER);
			unset_apu_reg(&NR52, CH2_ON);
		}
		NR2.r1 = (NR2.r1 & WAVE_DUTY) | (counter & LENGTH_TIMER);
	}
	// If channel 3 on and length timer enabled
	if ((NR3.r4 & (CH_TRIGGER | CH_LENGTH_ENABLE)) == (CH_TRIGGER | CH_LENGTH_ENABLE))
	{
		// This register is a timer only
		uint16_t counter = NR3.r1;
		if (counter++ >= 256)
		{
			unset_apu_reg(&NR3.r4, CH_TRIGGER);
			unset_apu_reg(&NR52, CH3_ON);
		}
		NR3.r1 = counter;
	}
	// If channel 4 on and length timer enabled
	if ((NR4.r4 & (CH_TRIGGER | CH_LENGTH_ENABLE)) == (CH_TRIGGER | CH_LENGTH_ENABLE))
	{
		uint8_t counter = NR4.r1;
		if (counter++ >= 64)
		{
			unset_apu_reg(&NR4.r4, CH_TRIGGER);
			unset_apu_reg(&NR52, CH4_ON);
		}
		NR4.r1 = (NR4.r1 & WAVE_DUTY) | (counter & LENGTH_TIMER);
	}
}

void tick_sweep_clocks()
{

}

void tick_envelope_clocks()
{

}

void set_apu_reg(uint8_t* reg, uint8_t bitmask)
{
	*reg |= bitmask;
}

void unset_apu_reg(uint8_t* reg, uint8_t bitmask)
{
	*reg &= ~bitmask;
}
