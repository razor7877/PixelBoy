#include "interrupts.h"
#include "cpu.h"

#include <stdio.h>

uint8_t IE{};
uint8_t IF = 0xE1;

void service_interrupts()
{
	// Service interrupts
	if (IME && (IF & 0x1F) && (IE & 0x1F))
	{
		if (IF_get(INTERRUPT_VBLANK) && IE_get(INTERRUPT_VBLANK))
		{
			IME = 0; cpu_halted = false;
			IF_clear(INTERRUPT_VBLANK);
			call_nn(0x40); tick(20);
		}
		else if (IF_get(INTERRUPT_LCD_STAT) && IE_get(INTERRUPT_LCD_STAT))
		{
			IME = 0; cpu_halted = false;
			IF_clear(INTERRUPT_LCD_STAT);
			call_nn(0x48); tick(20);
		}
		else if (IF_get(INTERRUPT_TIMER) && IE_get(INTERRUPT_TIMER))
		{
			IME = 0; cpu_halted = false;
			IF_clear(INTERRUPT_TIMER);
			call_nn(0x50); tick(20);
		}
		else if (IF_get(INTERRUPT_SERIAL) && IE_get(INTERRUPT_SERIAL))
		{
			IME = 0; cpu_halted = false;
			IF_clear(INTERRUPT_SERIAL);
			call_nn(0x58); tick(20);
		}
		else if (IF_get(INTERRUPT_JOYPAD) && IE_get(INTERRUPT_JOYPAD))
		{
			IME = 0; cpu_halted = false;
			IF_clear(INTERRUPT_JOYPAD);
			call_nn(0x60); tick(20);
		}
	}
}

void interrupt_request(interrupts interrupt)
{
	if (!IME && cpu_halted)
		cpu_halted = false;

	IF |= interrupt;
}

bool IF_get(interrupts interrupt)
{
	return (IF & interrupt);
}

void IF_clear(interrupts interrupt)
{
	IF &= ~interrupt;
}

bool IE_get(interrupts interrupt)
{
	return (IE & interrupt);
}

void IE_clear(interrupts interrupt)
{
	IE &= ~interrupt;
}
