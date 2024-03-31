#ifndef MBC3_H
#define MBC3_H

#include "mbc/mbc.h"

// A MBC struct containing pointers for the read and write functions for the MBC3
extern struct MBC mbc3;

void tick_mbc3_rtc(uint8_t cycles);

#endif