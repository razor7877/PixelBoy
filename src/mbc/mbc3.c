#include "mbc/mbc3.h"
#include "rom.h"
#include "cpu.h"

#include "logging.h"

#define MBC3_RTC_FREQ 32768

static bool ram_enable = false;
static uint8_t rom_bank = 0;
static uint8_t ram_bank = 0;

static uint32_t cpu_cycle_count = 0;
static uint32_t cpu_max_cycles = CPU_FREQ / MBC3_RTC_FREQ; // Every 128 CPU cycles, RTC should be ticked once
static uint32_t rtc_cycles = 0;

static uint8_t rtc_s = 0; // Seconds
static uint8_t rtc_m = 0; // Minutes
static uint8_t rtc_h = 0; // Hours
static uint8_t rtc_dl = 0; // Lower 8 bits of day counter
static uint8_t rtc_dh = 0; // Upper 1 bit of day counter, carry bit, halt flag

static uint8_t read_rom_mbc3(uint16_t address)
{
	// Read ROM bank 00
	if (address >= 0x0000 && address <= 0x3FFF)
		return rom[address];
	
	// Read ROM banks 01-7F
	if (address >= 0x4000 && address <= 0x7FFF)
	{
		// 14 lower bits are obtained from 14 lower bits of address
		// 5 next bits obtained from the selected rom bank number
		if (rom_bank == 0)
			rom_bank = 1;
		uint32_t mapped_address = (rom_bank << 14) | (address & 0x3FFF);
		return rom[mapped_address];
	}

	// Read RAM bank or RTC register
	if (address >= 0xA000 && address <= 0xBFFF)
	{
		if (ram_bank >= 0x00 && ram_bank <= 0x03)
		{
			// Read RAM banks 00-03
			if (!ram_enable)
				return 0xFF;

			uint16_t mapped_address = (ram_bank << 12) | (address & 0x1FFF);

			return external_ram[mapped_address];
		}
		// RTC register read
		else
		{
			switch (ram_bank)
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

static void write_rom_mbc3(uint16_t address, uint8_t value)
{
	// Enable/disable RAM banks and RTC registers
	if (address >= 0x0000 && address <= 0x1FFF)
	{
		if ((value & 0x0F) == 0x0A)
			ram_enable = true;
		else
			ram_enable = false;
	}

	// 7 bit ROM bank number register
	if (address >= 0x2000 && address <= 0x3FFF)
	{
		rom_bank = value & 0x7F; // 7 lower bits mask
	}

	// RAM bank number or RTC register select
	if (address >= 0x4000 && address <= 0x5FFF)
	{
		ram_bank = value & 0x0F; // Values are in range 0x00-0x0C
	}

	// Latch clock data
	if (address >= 0x6000 && address <= 0x7FFF)
	{
		// TODO
	}

	// Write to RAM bank or RTC register
	if (address >= 0xA000 && address <= 0xBFFF)
	{
		// RAM bank write
		if (ram_bank >= 0x00 && ram_bank <= 0x03)
		{
			if (!ram_enable)
				return;

			uint16_t mapped_address = (ram_bank << 12) | (address & 0x1FFF);
			external_ram[mapped_address] = value;
		}
		// RTC register write
		else
		{
			switch (ram_bank)
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

void tick_mbc3_rtc(uint8_t cycles)
{
	// If bit 6 is set, timer is stopped
	if (rtc_dh & 0x40)
		return;

	cpu_cycle_count += cycles;

	if (cpu_cycle_count >= cpu_max_cycles)
	{
		cpu_cycle_count %= cpu_max_cycles;

		rtc_cycles++;
		if (rtc_cycles >= MBC3_RTC_FREQ)
		{
			//log_warning("Done full RTC clock cycle s:m:h:d %d:%d:%d:%d\n", rtc_s, rtc_m, rtc_h, (rtc_dh & 0x01) | rtc_dl);
			rtc_cycles %= MBC3_RTC_FREQ;

			rtc_s++;
			if (rtc_s > 59) // Seconds counter
			{
				rtc_s = 0;
				rtc_m++;
			}
			if (rtc_m > 59) // Minutes counter
			{
				rtc_m = 0;
				rtc_h++;
			}
			if (rtc_h > 23) // Hours counter
			{
				rtc_h = 0;
				if (rtc_dl == 0xFF) // Handle overflow of lower 8 bits of day counter
				{
					rtc_dl = 0;

					if (rtc_dh & 0x01) // Day counter bit 8 already set
						rtc_dh = rtc_dh | 0x80; // Set day counter carry bit on overflow
					else
						rtc_dh = rtc_dh | 0x01; // Set day counter bit 8
				}
				else
					rtc_dl++;
			}
		}
	}
}

static void reset_mbc3()
{
	ram_enable = false;
	rom_bank = 0;
	ram_bank = 0;

	cpu_cycle_count = 0;
	rtc_cycles = 0;

	rtc_s = 0;
	rtc_m = 0;
	rtc_h = 0;
	rtc_dl = 0;
	rtc_dh = 0;
}

struct MBC mbc3 = {
	read_rom_mbc3,
	write_rom_mbc3,
	reset_mbc3
};