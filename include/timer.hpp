#ifndef TIMER_H
#define TIMER_H

#include <cstdint>

#define DIV_FREQ 16384

extern uint16_t DIV; // 0xFF04 : Divider register
extern uint8_t TIMA; // 0xFF05 : Timer counter
extern uint8_t TMA; // 0xFF06 : Timer modulo
extern uint8_t TAC; // 0xFF07 : Timer control

uint8_t read_timer(uint16_t address);
void write_timer(uint16_t address, uint8_t value);
void update_timer_freq(uint8_t value);
void tick_timer(uint8_t cycles);
void reset_timer();

#endif