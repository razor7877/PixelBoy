#ifndef APU_HPP
#define APU_HPP

#include <cstdint>

uint8_t read_apu(uint16_t address);
void write_apu(uint16_t address, uint8_t value);

#endif