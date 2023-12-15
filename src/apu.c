#include <stdio.h>

#include "portaudio.h"

#include "apu.h"

// Logging info regarding APU registers and wave ram R/W
//#define APU_DEBUG

// Sound channel 1
uint8_t NR10 = 0x80;
uint8_t NR11 = 0xBF;
uint8_t NR12 = 0xF3;
uint8_t NR13 = 0xFF;
uint8_t NR14 = 0xBF;

// Sound channel 2
uint8_t NR21 = 0x3F;
uint8_t NR22 = 0x00;
uint8_t NR23 = 0xFF;
uint8_t NR24 = 0xBF;

// Sound channel 3
uint8_t NR30 = 0x7F;
uint8_t NR31 = 0xFF;
uint8_t NR32 = 0x9F;
uint8_t NR33 = 0xFF;
uint8_t NR34 = 0xBF;

// Sound channel 4
uint8_t NR41 = 0xFF;
uint8_t NR42 = 0x00;
uint8_t NR43 = 0x00;
uint8_t NR44 = 0xBF;

// Global registers
uint8_t NR50 = 0x77;
uint8_t NR51 = 0xF3;
uint8_t NR52 = 0xF1;

uint8_t wave_ram[16] = {0};

uint8_t frame_sequencer = 0;

uint8_t read_apu(uint16_t address)
{
	if (address == 0xFF10)
		return NR10;

	else if (address == 0xFF11) // Only bits 6-7 are readable
		return (NR11 & 0xC0);

	else if (address == 0xFF12)
		return NR12;

	else if (address == 0xFF13) // NR13 is write-only
		return 0xFF;

	else if (address == 0xFF14) // Return bits 3-6 (0-2 and 7 are write-only)
		return (NR14 & 0x78);

	// 0xFF15 unused

	else if (address == 0xFF16)
		return NR21;

	else if (address == 0xFF17)
		return NR22;

	else if (address == 0xFF18) // NR23 is write-only
		return 0xFF;

	else if (address == 0xFF19) // Return bits 3-6 (0-2 and 7 are write-only)
		return (NR24 & 0x78);

	else if (address == 0xFF1A)
		return NR30;

	else if (address == 0xFF1B) // Write-only
		return 0xFF;

	else if (address == 0xFF1C)
		return NR32;

	else if (address == 0xFF1D) // Write-only
		return 0xFF;

	else if (address == 0xFF1E) // Only bit 6 readable
		return (NR34 & 0x40);

	// 0xFF1F unused?

	else if (address == 0xFF20)
		return NR41;

	else if (address == 0xFF21)
		return NR42;

	else if (address == 0xFF22)
		return NR43;

	else if (address == 0xFF23) // Highest bit is write only
		return (NR44 & 0x7F);

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
		NR10 = value;

	else if (address == 0xFF11)
		NR11 = value;

	else if (address == 0xFF12)
		NR12 = value;

	else if (address == 0xFF13)
		NR13 = value;

	else if (address == 0xFF14)
	{
		NR14 = (NR14 & 0x38) | (value & 0xC7); // Bits 3-5 are unused
		if (value & 0x80)
		{
			NR52 |= 0x01;
			//printf("Toggle on CH1\n");
		}
		else
		{
			NR52 &= ~0x01;
			//printf("Toggle off CH1\n");
		}
	}
		

	// 0xFF15 unused

	else if (address == 0xFF16)
		NR21 = value;

	else if (address == 0xFF17)
		NR22 = value;

	else if (address == 0xFF18)
		NR23 = value;

	else if (address == 0xFF19)
	{
		NR24 = (NR24 & 0x38) | (value & 0xC7); // Bits 3-5 are unused
		if (value & 0x80)
		{
			NR52 |= 0x02;
			//printf("Toggle on CH2\n");
		}
		else
		{
			NR52 &= ~0x02;
			//printf("Toggle off CH2\n");
		}
	}

	else if (address == 0xFF1A)
		NR30 = (NR30 & 0x7F) | (value & 0x80); // Only bit 7 is used

	else if (address == 0xFF1B)
		NR31 = value;

	else if (address == 0xFF1C)
		NR32 = (NR32 & 0x9F) | (value & 0x60); // Only bits 5-6 are used

	else if (address == 0xFF1D)
		NR33 = value;

	else if (address == 0xFF1E)
	{
		NR34 = (NR34 & 0x38) | (value & 0xC7); // Bits 3-5 are unused
		if (value & 0x80)
		{
			NR52 |= 0x04;
			//printf("Toggle on CH3\n");
		}
		else
		{
			NR52 &= ~0x04;
			//printf("Toggle off CH3\n");
		}
	}

	// 0xFF1F unused?

	else if (address == 0xFF20)
		NR41 = value;

	else if (address == 0xFF21)
		NR42 = value;

	else if (address == 0xFF22)
		NR43 = value;

	else if (address == 0xFF23)
	{
		NR44 = (NR44 & 0x3F) | (value & 0xC0); // Only bits 6-7 used
		if (value & 0x80)
		{
			NR52 |= 0x08;
			//printf("Toggle on CH4\n");
		}
		else
		{
			NR52 &= ~0x08;
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

		printf("Wave RAM write ADR %x VAL %x\n", address, value);

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
	// If APU disabled
	if ((NR52 & AUDIO_ON) == 0)
		return;

	// Length control clock
	if (frame_sequencer % 2 == 0)
	{
		//printf("Length ctrl clock\n");
		// If channel 1 on and length timer enabled
		if ((NR14 & (CH_TRIGGER | CH_LENGTH_ENABLE)) == (CH_TRIGGER | CH_LENGTH_ENABLE))
		{
			uint8_t counter = NR11 & LENGTH_TIMER;
			// When length timer reaches 64, disable the channel
			if (counter++ >= 64)
			{
				NR14 &= ~CH_TRIGGER;
			}
			else
			{
				NR11 = (NR11 & WAVE_DUTY) | (counter & LENGTH_TIMER);
			}
		}
		// If channel 2 on and length timer enabled
		if ((NR24 & (CH_TRIGGER | CH_LENGTH_ENABLE)) == (CH_TRIGGER | CH_LENGTH_ENABLE))
		{
			uint8_t counter = NR21 & LENGTH_TIMER;
			// When length timer reaches 64, disable the channel
			if (counter++ >= 64)
			{
				NR14 &= ~CH_TRIGGER;
			}
			else
			{
				NR21 = (NR21 & WAVE_DUTY) | (counter & LENGTH_TIMER);
			}
		}
		// If channel 1 on and length timer enabled
		if ((NR34 & (CH_TRIGGER | CH_LENGTH_ENABLE)) == (CH_TRIGGER | CH_LENGTH_ENABLE))
		{

		}
		// If channel 1 on and length timer enabled
		if ((NR44 & (CH_TRIGGER | CH_LENGTH_ENABLE)) == (CH_TRIGGER | CH_LENGTH_ENABLE))
		{

		}
	}
		
	// Sweep clock
	if (frame_sequencer == 2 || frame_sequencer == 6)
	{
		//printf("Sweep clock\n");
	}

	// Envelope clock
	if (frame_sequencer++ == 7)
	{
		frame_sequencer = 0;
		//printf("Vol env clock\n");
	}
}