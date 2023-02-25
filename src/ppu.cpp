#include "ppu.h"
#include "cpu.h"

uint32_t ppu_cycle_count{};

uint8_t vram[8192]{};
uint8_t OAM[160]{};
uint8_t LCDC = 0x91;
uint8_t STAT = 0x81;
uint8_t SCY{};
uint8_t SCX{};
uint8_t LY = 0x91;
uint8_t LYC = 0x00;
uint8_t DMA = 0xFF;
uint8_t BGP = 0xFC;
uint8_t OBP0{};
uint8_t OBP1{};
uint8_t WY{};
uint8_t WX{};

void tick_ppu(uint8_t cycles)
{
	ppu_cycle_count += cycles / 2;

	if (ppu_cycle_count > PPU_FREQ)
		ppu_cycle_count %= PPU_FREQ;
}

uint8_t read_vram(uint16_t address)
{
	if ((STAT & 0x03) == 0x03) // VRAM inaccessible during mode 3
		return 0xFF;
	else
		return vram[address];
}

void write_vram(uint16_t address, uint8_t value)
{
	if ((STAT & 0x03) == 0x03) // VRAM inaccessible during mode 3, ignore writes
		return;
	else
		vram[address] = value;
}

uint8_t read_oam(uint16_t address)
{
	// OAM inaccessible during mode 2 and 3
	if (((STAT & 0x03) == 0x03) || ((STAT & 0x03) == 0x02))
		return 0xFF;
	else
		return OAM[address];
}

void write_oam(uint16_t address, uint8_t value)
{
	// OAM inaccessible during mode 2 and 3
	if (((STAT & 0x03) == 0x03) || ((STAT & 0x03) == 0x02))
		return;
	else
		OAM[address] = value;
}

uint8_t read_ppu(uint16_t address)
{
	if (address == 0xFF40)
		return LCDC;

	if (address == 0xFF41)
		return STAT;

	if (address == 0xFF42)
		return SCY;

	if (address == 0xFF43)
		return SCY;

	if (address == 0xFF44)
		return LY;

	if (address == 0xFF45)
		return LYC;

	if (address == 0xFF46)
		return DMA;

	if (address == 0xFF47)
		return BGP;

	if (address == 0xFF48)
		return OBP0;

	if (address == 0xFF49)
		return OBP1;

	if (address == 0xFF4A)
		return WY;

	if (address == 0xFF4B)
		return WX;
}

void write_ppu(uint16_t address, uint8_t value)
{
	if (address == 0xFF40)
		LCDC = value;

	if (address == 0xFF41) // Lower 2 bits are read only
		STAT = (value & 0xFC) | (STAT & 0x02);

	if (address == 0xFF42)
		SCY = value;

	if (address == 0xFF43)
		SCX = value;

	if (address == 0xFF44) // LY is read only
		return;

	if (address == 0xFF45)
		LYC = value;

	if (address == 0xFF46)
	{
		DMA = value;
		dma_transfer();
	}

	if (address == 0xFF47)
		BGP = value;

	if (address == 0xFF48)
		OBP0 = value;

	if (address == 0xFF49)
		OBP1 = value;

	if (address == 0xFF4A)
		WY = value;

	if (address == 0xFF4B)
		WX = value;
}