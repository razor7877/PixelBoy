#ifndef PPU_H
#define PPU_H

#include <cstdint>

#define PPU_FREQ 2097152

extern uint32_t ppu_cycle_count;

extern uint8_t vram[8192];
extern uint8_t OAM[160]; // Object access memory
extern uint8_t LCDC; // 0xFF40 : LCD Control
extern uint8_t STAT; // 0xFF41 : LCD Status
extern uint8_t SCY; // 0xFF42 : Viewport Y position
extern uint8_t SCX; // 0xFF43 : Viewport X position
extern uint8_t LY; // 0xFF44 : LCD Y coordinate
extern uint8_t LYC; // 0xFF45 : LY compare
extern uint8_t DMA; // 0xFF46 : Direct memory access
extern uint8_t BGP; // 0xFF47 : BG palette data
extern uint8_t OBP0; // 0xFF48 : OBJ palette 0
extern uint8_t OBP1; // 0xFF49 : OBJ palette 1
extern uint8_t WY; // 0xFF4A : Window Y position
extern uint8_t WX; // 0xFF4B : Window X position

void tick_ppu(uint8_t cycles);

uint8_t read_vram(uint16_t address);
void write_vram(uint16_t address, uint8_t value);

uint8_t read_oam(uint16_t address);
void write_oam(uint16_t address, uint8_t value);

uint8_t read_ppu(uint16_t address);
void write_ppu(uint16_t address, uint8_t value);

#endif