#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

extern uint8_t IE; // Interrupt enable register
extern uint8_t IF; // Interrupt flags register

typedef enum
{
	INTERRUPT_VBLANK = 0b00000001,
	INTERRUPT_LCD_STAT = 0b00000010,
	INTERRUPT_TIMER = 0b00000100,
	INTERRUPT_SERIAL = 0b00001000,
	INTERRUPT_JOYPAD = 0b00010000,
} interrupts;

void service_interrupts();

void interrupt_request(interrupts interrupt);
bool IF_get(interrupts interrupt);
void IF_clear(interrupts interrupt);
bool IE_get(interrupts interrupt);
void IE_clear(interrupts interrupt);

void reset_interrupts();

#endif