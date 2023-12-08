#ifndef PPU_H
#define PPU_H

#include <cstdint>

#define PPU_FREQ 2097152

// Used by CPU to handle instructions until one frame is drawn
extern bool new_frame_ready;

enum lcd_control
{
	LCD_ENABLE = 0b10000000,
	WINDOW_DATA_AREA = 0b01000000,
	WINDOW_ENABLE = 0b00100000,
	TILE_DATA_AREA = 0b00010000,
	BG_DATA_AREA = 0b00001000,
	OBJ_SIZE = 0b00000100,
	OBJ_ENABLE = 0b00000010,
	BG_ENABLE = 0b00000001,
};

enum lcd_status
{
	LYC_INTERRUPT_ENABLED = 0b01000000,
	OAM_INTERRUPT_ENABLED = 0b00100000,
	VBLANK_INTERRUPT_ENABLED = 0b00010000,
	HBLANK_INTERRUPT_ENABLED = 0b00001000,
	LYC_COMPARE_FLAG = 0b00000100,
	LCD_MODE_0 = 0b00000000,
	LCD_MODE_1 = 0b00000001,
	LCD_MODE_2 = 0b00000010,
	LCD_MODE_3 = 0b00000011,
};

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

extern uint8_t frame_buffer[160 * 144 * 4];

void tick_ppu(uint8_t cycles);
void step_ppu(uint8_t cycles);
void draw_scanline();
void draw_tiles();
void draw_sprites();
uint8_t ppu_mode();

uint8_t read_vram(uint16_t address);
void write_vram(uint16_t address, uint8_t value);

uint8_t read_oam(uint16_t address);
void write_oam(uint16_t address, uint8_t value);

uint8_t read_ppu(uint16_t address);
void write_ppu(uint16_t address, uint8_t value);

void update_LCDC(uint8_t value);

#endif