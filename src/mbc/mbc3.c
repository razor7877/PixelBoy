#include "mbc/mbc3.h"
#include "rom.h"
#include "cpu.h"

#include "logging.h"

#define MBC3_RTC_FREQ 32768

uint32_t cpu_cycle_count = 0;
uint32_t cpu_max_cycles = CPU_FREQ / MBC3_RTC_FREQ; // Every 128 CPU cycles, RTC should be ticked once
uint32_t rtc_cycles = 0;

uint8_t rtc_s = 0; // Seconds
uint8_t rtc_m = 0; // Minutes
uint8_t rtc_h = 0; // Hours
uint8_t rtc_dl = 0; // Lower 8 bits of day counter
uint8_t rtc_dh = 0; // Upper 1 bit of day counter, carry bit, halt flag

uint8_t read_rom_mbc3(uint16_t address)
{
	// Read ROM bank 00
	if (address >= 0x0000 && address <= 0x3FFF)
		return rom[address];
	
	// Read ROM banks 01-7F
	if (address >= 0x4000 && address <= 0x7FFF)
	{
		// 14 lower bits are obtained from 14 lower bits of address
		// 5 next bits obtained from the selected rom bank number
		uint32_t mapped_address = (mbc3.rom_bank << 14) | (address & 0x3FFF);
		return rom[mapped_address];
	}

	// Read RAM bank or RTC register
	if (address >= 0xA000 && address <= 0xBFFF)
	{
		if (mbc3.ram_bank >= 0x00 && mbc3.ram_bank <= 0x03)
		{
			// Read RAM banks 00-03
			if (!mbc3.ram_enable)
				return 0xFF;

			uint16_t mapped_address = (mbc3.ram_bank << 12) | (address & 0x1FFF);

			return external_ram[mapped_address];
		}
		// RTC register read
		else
		{
			switch (mbc3.ram_bank)
			{
			case 0x08:
				return rtc_s;
				break;
			case 0x09:
				return rtc_m;
				break;
			case 0x0A:
				return rtc_h;
				break;
			case 0x0B:
				return rtc_dl;
				break;
			case 0x0C:
				return rtc_dh;
				break;
			}
		}
	}

	return 0xFF;
}

void write_rom_mbc3(uint16_t address, uint8_t value)
{
	// Enable/disable RAM banks and RTC registers
	if (address >= 0x0000 && address <= 0x1FFF)
	{
		if ((value & 0x0F) == 0x0A)
			mbc3.ram_enable = true;
		else
			mbc3.ram_enable = false;
	}

	// 7 bit ROM bank number register
	if (address >= 0x2000 && address <= 0x3FFF)
	{
		mbc3.rom_bank = value & 0x7F; // 7 lower bits mask
		if (mbc3.rom_bank == 0) // 0 value behaves as a 1 on real hardware
			mbc3.rom_bank = 1;
	}

	// RAM bank number or RTC register select
	if (address >= 0x4000 && address <= 0x5FFF)
	{
		mbc3.ram_bank = value & 0x0F; // Values are in range 0x00-0x0C
	}

	// Latch clock data
	if (address >= 0x6000 && address <= 0x7FFF)
	{

	}

	// Write to RAM bank or RTC register
	if (address >= 0xA000 && address <= 0xBFFF)
	{
		// RAM bank write
		if (mbc3.ram_bank >= 0x00 && mbc3.ram_bank <= 0x03)
		{
			if (!mbc3.ram_enable)
				return;

			uint16_t mapped_address = (mbc3.ram_bank << 12) | (address & 0x1FFF);
			external_ram[mapped_address] = value;
		}
		// RTC register write
		else
		{
			switch (mbc3.ram_bank)
			{
			case 0x08:
				rtc_s = value;
				break;
			case 0x09:
				rtc_m = value;
				break;
			case 0x0A:
				rtc_h = value;
				break;
			case 0x0B:
				rtc_dl = value;
				break;
			case 0x0C:
				rtc_dh = value;
				break;
			}
		}
	}
}

tick_mbc3_rtc(uint8_t cycles)
{
	cpu_cycle_count += cycles;

	if (cpu_cycle_count >= cpu_max_cycles)
	{
		cpu_cycle_count %= cpu_max_cycles;

		rtc_cycles++;
		if (rtc_cycles >= MBC3_RTC_FREQ)
		{
			log_warning("Done full RTC clock cycle\n");
			rtc_cycles %= MBC3_RTC_FREQ;
		}
	}
}

struct MBC mbc3 = {
	read_rom_mbc3,
	write_rom_mbc3,
	false,
	0,
	0,
	0,
};