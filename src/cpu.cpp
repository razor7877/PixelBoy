#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>

#include "cpu.hpp"
#include "cpu_cb.hpp"
#include "memory.hpp"
#include "interrupts.hpp"
#include "io.hpp"
#include "ppu.hpp"

uint32_t cycle_count{};
uint16_t dma_cycles_left{};

uint16_t AF = 0x01B0;
uint16_t BC = 0x0013;
uint16_t DE = 0x00D8;
uint16_t HL = 0x014D;

uint16_t sp = 0xFFFE; // Stack pointer
uint16_t pc = 0x100; // Program counter
uint8_t opcode{};

uint16_t operand{};

bool cpu_stopped{};
bool cpu_halted{};
bool IME_toggle{}; // Toggle to enable IME after one instruction with EI
bool IME{}; // Interrupt Master Enable

// Get info regarding the DMA (Direct Memory Access) transfers
//#define DMA_DEBUG

void execute_frame()
{
	while (!new_frame_ready)
	{
		handle_instruction();
	}

	new_frame_ready = false;
}

void handle_instruction()
{
	if (IME_toggle)
	{
		IME_toggle = 0;
		IME = 1;
	}
	
	service_interrupts();

	if (!cpu_stopped && !cpu_halted)
	{
		opcode = read_byte(pc++);

		// Switch over the operand length to correctly call the function and pass arguments
		switch (instructions[opcode].operand_length)
		{
			case 0:
				operand = 0x00;
				((void (*)(void))instructions[opcode].function)();
				break;

			case 1:
				operand = read_byte(pc);
				pc += 1;
				((void (*)(uint8_t))instructions[opcode].function)(operand);
				break;

			case 2:
				operand = read_word(pc);
				pc += 2;
				((void (*)(uint16_t))instructions[opcode].function)(operand);
				break;
		}
	}

	tick(instructions[opcode].duration);
}

void tick(uint8_t cycles)
{
	if (cycle_count > CPU_FREQ)
	{
		cycle_count %= CPU_FREQ;
		printf("Ran full CPU cycle\n");
	}

	cycle_count += cycles;
	tick_ppu(cycles / 2); // PPU runs at half the clock speed of the CPU
	tick_timer(cycles);

	if (dma_cycles_left > 0)
	{
		if (cycles > dma_cycles_left)
		{
			dma_cycles_left = 0;
			uint16_t dma_source = read_byte(0xFF46) << 8;
#ifdef DMA_DEBUG
			printf("DMA Source: %x\n", dma_source);
#endif
			// DMA sources outside of DFFF are mapped back to SRAM
			if (dma_source >= 0xDFFF)
				dma_source -= 0x2000;
			// Fill OAM memory once DMA cycles done
			for (int i = 0; i < 160; i++)
			{
#ifdef DMA_DEBUG
				printf("Writing from adr %x value %x\n", (dma_source + i), read_byte(dma_source + i));
#endif
				write_byte(0xFE00 + i, read_byte(dma_source + i));
			}
		}
		else { dma_cycles_left -= cycles; }
#ifdef DMA_DEBUG
		printf("In DMA routine, cycles left: %d\n", dma_cycles_left);
#endif
	}
}

void dma_transfer()
{
	dma_cycles_left = DMA_DURATION;
}

uint8_t lower_byte(uint16_t value)
{
	return value & 0x00FF;
}

uint8_t upper_byte(uint16_t value)
{
	return (value & 0xFF00) >> 8;
}

void set_reg_hi(uint16_t& reg, uint8_t value)
{
	reg = (value << 8) | (reg & 0x00FF);
}

void set_reg_lo(uint16_t& reg, uint8_t value)
{
	reg = (reg & 0xFF00) | value;
}

void set_flags(uint8_t flags)
{
	set_reg_lo(AF, lower_byte(AF) | flags);
}

void clear_flags(uint8_t flags)
{
	set_reg_lo(AF, lower_byte(AF) & ~(flags));
}

bool get_flags(uint8_t flags)
{
	return lower_byte(AF) & flags;
}

void and_r(uint8_t value)
{
	set_reg_hi(AF, upper_byte(AF) & value);

	if (upper_byte(AF) == 0x00)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE | FLAG_CARRY);
	set_flags(FLAG_HALFCARRY);
}

void xor_r(uint8_t value)
{
	set_reg_hi(AF, upper_byte(AF) ^ value);

	if (upper_byte(AF) == 0x00)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE | FLAG_HALFCARRY | FLAG_CARRY);
}

void or_r(uint8_t value)
{
	set_reg_hi(AF, upper_byte(AF) | value);

	if (upper_byte(AF) == 0x00)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE | FLAG_HALFCARRY | FLAG_CARRY);
}

void cp_r(uint8_t value)
{
	if (upper_byte(AF) == value)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	if (upper_byte(AF) < value)
		set_flags(FLAG_CARRY);
	else
		clear_flags(FLAG_CARRY);

	if ( (upper_byte(AF) & 0x0F) < (value & 0x0F) )
		set_flags(FLAG_HALFCARRY);
	else
		clear_flags(FLAG_HALFCARRY);

	set_flags(FLAG_NEGATIVE);
}

uint8_t inc_r(uint8_t value)
{
	if (((value & 0x0F) + 1) > 0x0F)
		set_flags(FLAG_HALFCARRY);
	else
		clear_flags(FLAG_HALFCARRY);

	value++;

	if (value == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE);

	return value;
}

uint8_t dec_r(uint8_t value)
{
	if ((value & 0x0F) == 0)
		set_flags(FLAG_HALFCARRY);
	else
		clear_flags(FLAG_HALFCARRY);
	
	value--;

	if (value == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	set_flags(FLAG_NEGATIVE);

	return value;
}

void add_r(uint8_t value)
{
	if (((upper_byte(AF) & 0x0F) + (value & 0x0F)) > 0x0F)
		set_flags(FLAG_HALFCARRY);
	else
		clear_flags(FLAG_HALFCARRY);

	if (upper_byte(AF) + value > 0xFF)
		set_flags(FLAG_CARRY);
	else
		clear_flags(FLAG_CARRY);

	set_reg_hi(AF, upper_byte(AF) + value);

	if (upper_byte(AF) == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE);
}

void adc_r(uint8_t value)
{
	// Save the carry flag value, otherwise it might be cleared before doing the addition
	bool c_flag = get_flags(FLAG_CARRY);

	if (((upper_byte(AF) & 0x0F) + (value & 0x0F) + c_flag) > 0x0F)
		set_flags(FLAG_HALFCARRY);
	else
		clear_flags(FLAG_HALFCARRY);

	if (upper_byte(AF) + value + c_flag > 0xFF)
		set_flags(FLAG_CARRY);
	else
		clear_flags(FLAG_CARRY);

	set_reg_hi(AF, upper_byte(AF) + value + c_flag);

	if (upper_byte(AF) == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE);
}

void sub_r(uint8_t value)
{
	if (value > upper_byte(AF))
		set_flags(FLAG_CARRY);
	else
		clear_flags(FLAG_CARRY);

	if ((value & 0x0F) > (upper_byte(AF) & 0x0F))
		set_flags(FLAG_HALFCARRY);
	else
		clear_flags(FLAG_HALFCARRY);

	set_reg_hi(AF, upper_byte(AF) - value);

	if (upper_byte(AF) == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	set_flags(FLAG_NEGATIVE);
}

void sbc_r(uint8_t value)
{
	// Save the carry flag value, otherwise it might be cleared before doing the substraction
	bool c_flag = get_flags(FLAG_CARRY);

	if ((value + c_flag) > upper_byte(AF))
		set_flags(FLAG_CARRY);
	else
		clear_flags(FLAG_CARRY);

	if (((value & 0x0F) + c_flag) > (upper_byte(AF) & 0x0F))
		set_flags(FLAG_HALFCARRY);
	else
		clear_flags(FLAG_HALFCARRY);

	set_reg_hi(AF, upper_byte(AF) - value - c_flag);

	if (upper_byte(AF) == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	set_flags(FLAG_NEGATIVE);
}

void undefined() { printf("ERROR: UNDEFINED INSTRUCTION EXECUTED"); }
void nop(){} // 0x00
void ld_bc_nn(uint16_t operand) { BC = operand; } // 0x01
void ld_bcp_a() { write_byte(BC, upper_byte(AF)); } // 0x02
void inc_bc() { BC++; } // 0x03
void inc_b() { set_reg_hi(BC, inc_r(upper_byte(BC))); } // 0x04
void dec_b() { set_reg_hi(BC, dec_r(upper_byte(BC))); } // 0x05
void ld_b_n(uint8_t operand){ set_reg_hi(BC, operand); } // 0x06
void rlca() // 0x07
{
	(upper_byte(AF) & 0x80) ? set_flags(FLAG_CARRY) : clear_flags(FLAG_CARRY);

	set_reg_hi(AF, (upper_byte(AF) << 1) | ((upper_byte(AF) & 0x80)) >> 7);

	clear_flags(FLAG_ZERO | FLAG_NEGATIVE | FLAG_HALFCARRY);
}
void ld_nnp_sp(uint16_t operand) { write_word(operand, sp); } // 0x08
void add_hl_bc() // 0x09
{
	if ((HL & 0x0FFF) + (BC & 0x0FFF) > 0x0FFF)
		set_flags(FLAG_HALFCARRY);
	else
		clear_flags(FLAG_HALFCARRY);

	if (HL + BC > 0xFFFF)
		set_flags(FLAG_CARRY);
	else
		clear_flags(FLAG_CARRY);

	HL += BC;

	clear_flags(FLAG_NEGATIVE);
}
void ld_a_bcp() { set_reg_hi(AF, read_byte(BC)); } // 0x0A
void dec_bc() { BC--; } // 0x0B
void inc_c() { set_reg_lo(BC, inc_r(lower_byte(BC))); } // 0x0C
void dec_c() { set_reg_lo(BC, dec_r(lower_byte(BC))); } // 0x0D
void ld_c_n(uint8_t operand){ set_reg_lo(BC, operand); } // 0x0E
void rrca() // 0x0F
{
	(upper_byte(AF) & 0x01) ? set_flags(FLAG_CARRY) : clear_flags(FLAG_CARRY);

	set_reg_hi(AF, (upper_byte(AF) >> 1) | ((upper_byte(AF) & 0x01)) << 7);

	clear_flags(FLAG_ZERO | FLAG_NEGATIVE | FLAG_HALFCARRY);
}

void stop(uint8_t operand) { cpu_stopped = true; DIV = 0x00; } // 0x10
void ld_de_nn(uint16_t operand) { DE = operand; } // 0x11
void ld_dep_a() { write_byte(DE, upper_byte(AF)); } // 0x12
void inc_de() { DE++; } // 0x13
void inc_d(){ set_reg_hi(DE, inc_r(upper_byte(DE))); } // 0x14
void dec_d() { set_reg_hi(DE, dec_r(upper_byte(DE))); } // 0x15
void ld_d_n(uint8_t operand){ set_reg_hi(DE, operand); } // 0x16
void rla() // 0x17
{
	bool bit_data = (upper_byte(AF) & 0x80);

	set_reg_hi(AF, (upper_byte(AF) << 1) | get_flags(FLAG_CARRY));

	bit_data ? set_flags(FLAG_CARRY) : clear_flags(FLAG_CARRY);

	clear_flags(FLAG_ZERO | FLAG_NEGATIVE | FLAG_HALFCARRY);
}
void jr_n(int8_t operand) { pc += operand; } // 0x18
void add_hl_de() // 0x19
{
	if ((HL & 0x0FFF) + (DE & 0x0FFF) > 0x0FFF)
		set_flags(FLAG_HALFCARRY);
	else
		clear_flags(FLAG_HALFCARRY);

	if (HL + DE > 0xFFFF)
		set_flags(FLAG_CARRY);
	else
		clear_flags(FLAG_CARRY);

	HL += DE;

	clear_flags(FLAG_NEGATIVE);
}
void ld_a_dep() { set_reg_hi(AF, read_byte(DE)); } // 0x1A
void dec_de() { DE--; } // 0x1B
void inc_e(){ set_reg_lo(DE, inc_r(lower_byte(DE))); } // 0x1C
void dec_e() { set_reg_lo(DE, dec_r(lower_byte(DE))); } // 0x1D
void ld_e_n(uint8_t operand){ set_reg_lo(DE, operand); } // 0x1E
void rra() // 0x1F
{
	bool bit_data = (upper_byte(AF) & 0x01);

	set_reg_hi(AF, (upper_byte(AF) >> 1) | (get_flags(FLAG_CARRY) << 7));

	bit_data ? set_flags(FLAG_CARRY) : clear_flags(FLAG_CARRY);

	clear_flags(FLAG_ZERO | FLAG_NEGATIVE | FLAG_HALFCARRY);
}

void jr_nz_n(int8_t operand) // 0x20
{
	if (!get_flags(FLAG_ZERO))
	{
		pc += operand;
		tick(12);
	}
	else { tick(8); }
}
void ld_hl_nn(uint16_t operand) { HL = operand; } // 0x21
void ldi_hlp_a() { write_byte(HL++, upper_byte(AF)); } // 0x22
void inc_hl() { HL++; } // 0x23
void inc_h(){ set_reg_hi(HL, inc_r(upper_byte(HL))); } // 0x24
void dec_h() { set_reg_hi(HL, dec_r(upper_byte(HL))); } // 0x25
void ld_h_n(uint8_t operand){ set_reg_hi(HL, operand); } // 0x26
void daa() // 0x27
{
	if (get_flags(FLAG_NEGATIVE)) // After substraction, only adjust if carry/half-carry occurred
	{
		if (get_flags(FLAG_CARRY)) { set_reg_hi(AF, upper_byte(AF) - 0x60); }
		if (get_flags(FLAG_HALFCARRY)) { set_reg_hi(AF, upper_byte(AF) - 0x06); }
	}
	else // After addition, adjust if carry/half-carry or result out of bounds
	{
		if (get_flags(FLAG_CARRY) || upper_byte(AF) > 0x99) { set_reg_hi(AF, upper_byte(AF) + 0x60); set_flags(FLAG_CARRY); }
		if (get_flags(FLAG_HALFCARRY) || (upper_byte(AF) & 0x0f) > 0x09) { set_reg_hi(AF, upper_byte(AF) + 0x06); }
	}

	if (upper_byte(AF) == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_HALFCARRY);
}
void jr_z_n(int8_t operand) // 0x28
{
	if (get_flags(FLAG_ZERO))
	{
		pc += operand;
		tick(12);
	}
	else { tick(8); }
}
void add_hl_hl() // 0x29
{
	if ((HL & 0x0FFF) + (HL & 0x0FFF) > 0x0FFF)
		set_flags(FLAG_HALFCARRY);
	else
		clear_flags(FLAG_HALFCARRY);

	if (HL + HL > 0xFFFF)
		set_flags(FLAG_CARRY);
	else
		clear_flags(FLAG_CARRY);

	HL += HL;

	clear_flags(FLAG_NEGATIVE);
}
void ldi_a_hlp() { set_reg_hi(AF, read_byte(HL++)); } // 0x2A
void dec_hl() { HL--; } // 0x2B
void inc_l(){ set_reg_lo(HL, inc_r(lower_byte(HL))); } // 0x2C
void dec_l() { set_reg_lo(HL, dec_r(lower_byte(HL))); } // 0x2D
void ld_l_n(uint8_t operand){ set_reg_lo(HL, operand); } // 0x2E
void cpl() { set_reg_hi(AF, ~upper_byte(AF)); set_flags(FLAG_NEGATIVE | FLAG_HALFCARRY); } // 0x2F

void jr_nc_n(int8_t operand) // 0x30
{
	if (!get_flags(FLAG_CARRY))
	{
		pc += operand;
		tick(12);
	}
	else { tick(8); }
}
void ld_sp_nn(uint16_t operand) { sp = operand; } // 0x31
void ldd_hlp_a() { write_byte(HL, upper_byte(AF)); HL--; } // 0x32
void inc_sp() { sp++; } // 0x33
void inc_hlp() { write_byte(HL, inc_r(read_byte(HL))); } // 0x34
void dec_hlp() { write_byte(HL, dec_r(read_byte(HL))); } // 0x35
void ld_hlp_n(uint8_t operand) { write_byte(HL, operand); } // 0x36
void scf() { set_flags(FLAG_CARRY); clear_flags(FLAG_NEGATIVE | FLAG_HALFCARRY); } // 0x37
void jr_c_n(int8_t operand) // 0x38
{
	if (get_flags(FLAG_CARRY))
	{
		pc += operand;
		tick(12);
	}
	else { tick(8); }
}
void add_hl_sp() // 0x39
{
	if ((HL & 0x0FFF) + (sp & 0x0FFF) > 0x0FFF)
		set_flags(FLAG_HALFCARRY);
	else
		clear_flags(FLAG_HALFCARRY);

	if (HL + sp > 0xFFFF)
		set_flags(FLAG_CARRY);
	else
		clear_flags(FLAG_CARRY);

	HL += sp;

	clear_flags(FLAG_NEGATIVE);
}
void ldd_a_hlp() { set_reg_hi(AF, read_byte(HL)); HL--; } // 0x3A
void dec_sp() { sp--; } // 0x3B
void inc_a(){ set_reg_hi(AF, inc_r(upper_byte(AF))); } // 0x3C
void dec_a() { set_reg_hi(AF, dec_r(upper_byte(AF))); } // 0x3D
void ld_a_n(uint8_t operand){ set_reg_hi(AF, operand); } // 0x3E
void ccf() // 0x3F
{
	get_flags(FLAG_CARRY) ? clear_flags(FLAG_CARRY) : set_flags(FLAG_CARRY);

	clear_flags(FLAG_NEGATIVE | FLAG_HALFCARRY);
}

void ld_b_b(){}
void ld_b_c() { set_reg_hi(BC, lower_byte(BC)); }
void ld_b_d() { set_reg_hi(BC, upper_byte(DE)); }
void ld_b_e() { set_reg_hi(BC, lower_byte(DE)); }
void ld_b_h() { set_reg_hi(BC, upper_byte(HL)); }
void ld_b_l() { set_reg_hi(BC, lower_byte(HL)); }
void ld_b_hlp() { set_reg_hi(BC, read_byte(HL)); }
void ld_b_a() { set_reg_hi(BC, upper_byte(AF)); }
void ld_c_b() { set_reg_lo(BC, upper_byte(BC)); }
void ld_c_c(){}
void ld_c_d() { set_reg_lo(BC, upper_byte(DE)); }
void ld_c_e() { set_reg_lo(BC, lower_byte(DE)); }
void ld_c_h() { set_reg_lo(BC, upper_byte(HL)); }
void ld_c_l() { set_reg_lo(BC, lower_byte(HL)); }
void ld_c_hlp() { set_reg_lo(BC, read_byte(HL)); }
void ld_c_a() { set_reg_lo(BC, upper_byte(AF)); }

void ld_d_b() { set_reg_hi(DE, upper_byte(BC)); }
void ld_d_c() { set_reg_hi(DE, lower_byte(BC)); }
void ld_d_d(){}
void ld_d_e() { set_reg_hi(DE, lower_byte(DE)); }
void ld_d_h() { set_reg_hi(DE, upper_byte(HL)); }
void ld_d_l() { set_reg_hi(DE, lower_byte(HL)); }
void ld_d_hlp() { set_reg_hi(DE, read_byte(HL)); }
void ld_d_a() { set_reg_hi(DE, upper_byte(AF)); }
void ld_e_b() { set_reg_lo(DE, upper_byte(BC)); }
void ld_e_c() { set_reg_lo(DE, lower_byte(BC)); }
void ld_e_d() { set_reg_lo(DE, upper_byte(DE)); }
void ld_e_e(){}
void ld_e_h() { set_reg_lo(DE, upper_byte(HL)); }
void ld_e_l() { set_reg_lo(DE, lower_byte(HL)); }
void ld_e_hlp() { set_reg_lo(DE, read_byte(HL)); }
void ld_e_a() { set_reg_lo(DE, upper_byte(AF)); }

void ld_h_b() { set_reg_hi(HL, upper_byte(BC)); }
void ld_h_c() { set_reg_hi(HL, lower_byte(BC)); }
void ld_h_d() { set_reg_hi(HL, upper_byte(DE)); }
void ld_h_e() { set_reg_hi(HL, lower_byte(DE)); }
void ld_h_h(){}
void ld_h_l() { set_reg_hi(HL, lower_byte(HL)); }
void ld_h_hlp() { set_reg_hi(HL, read_byte(HL)); }
void ld_h_a() { set_reg_hi(HL, upper_byte(AF)); }
void ld_l_b() { set_reg_lo(HL, upper_byte(BC)); }
void ld_l_c() { set_reg_lo(HL, lower_byte(BC)); }
void ld_l_d() { set_reg_lo(HL, upper_byte(DE)); }
void ld_l_e() { set_reg_lo(HL, lower_byte(DE)); }
void ld_l_h() { set_reg_lo(HL, upper_byte(HL)); }
void ld_l_l(){}
void ld_l_hlp() { set_reg_lo(HL, read_byte(HL)); }
void ld_l_a() { set_reg_lo(HL, upper_byte(AF)); }

void ld_hlp_b() { write_byte(HL, upper_byte(BC)); }
void ld_hlp_c() { write_byte(HL, lower_byte(BC)); }
void ld_hlp_d() { write_byte(HL, upper_byte(DE)); }
void ld_hlp_e() { write_byte(HL, lower_byte(DE)); }
void ld_hlp_h() { write_byte(HL, upper_byte(HL)); }
void ld_hlp_l() { write_byte(HL, lower_byte(HL)); }
void halt() { cpu_halted = true; }
void ld_hlp_a() { write_byte(HL, upper_byte(AF)); }
void ld_a_b() { set_reg_hi(AF, upper_byte(BC)); }
void ld_a_c() { set_reg_hi(AF, lower_byte(BC)); }
void ld_a_d() { set_reg_hi(AF, upper_byte(DE)); }
void ld_a_e() { set_reg_hi(AF, lower_byte(DE)); }
void ld_a_h() { set_reg_hi(AF, upper_byte(HL)); }
void ld_a_l() { set_reg_hi(AF, lower_byte(HL)); }
void ld_a_hlp() { set_reg_hi(AF, read_byte(HL)); }
void ld_a_a(){}

void add_a_b() { add_r(upper_byte(BC)); }
void add_a_c() { add_r(lower_byte(BC)); }
void add_a_d() { add_r(upper_byte(DE)); }
void add_a_e() { add_r(lower_byte(DE)); }
void add_a_h() { add_r(upper_byte(HL)); }
void add_a_l() { add_r(lower_byte(HL)); }
void add_a_hlp() { add_r(read_byte(HL)); }
void add_a_a() { add_r(upper_byte(AF)); }
void adc_a_b() { adc_r(upper_byte(BC)); }
void adc_a_c() { adc_r(lower_byte(BC)); }
void adc_a_d() { adc_r(upper_byte(DE)); }
void adc_a_e() { adc_r(lower_byte(DE)); }
void adc_a_h() { adc_r(upper_byte(HL)); }
void adc_a_l() { adc_r(lower_byte(HL)); }
void adc_a_hlp() { adc_r(read_byte(HL)); }
void adc_a_a() { adc_r(upper_byte(AF)); }

void sub_b() { sub_r(upper_byte(BC)); }
void sub_c() { sub_r(lower_byte(BC)); }
void sub_d() { sub_r(upper_byte(DE)); }
void sub_e() { sub_r(lower_byte(DE)); }
void sub_h() { sub_r(upper_byte(HL)); }
void sub_l() { sub_r(lower_byte(HL)); }
void sub_hlp() { sub_r(read_byte(HL)); }
void sub_a() { sub_r(upper_byte(AF)); }
void sbc_a_b() { sbc_r(upper_byte(BC)); }
void sbc_a_c() { sbc_r(lower_byte(BC)); }
void sbc_a_d() { sbc_r(upper_byte(DE)); }
void sbc_a_e() { sbc_r(lower_byte(DE)); }
void sbc_a_h() { sbc_r(upper_byte(HL)); }
void sbc_a_l() { sbc_r(lower_byte(HL)); }
void sbc_a_hlp() { sbc_r(read_byte(HL)); }
void sbc_a_a() { sbc_r(upper_byte(AF)); }

void and_b() { and_r(upper_byte(BC)); }
void and_c() { and_r(lower_byte(BC)); }
void and_d() { and_r(upper_byte(DE)); }
void and_e() { and_r(lower_byte(DE)); }
void and_h() { and_r(upper_byte(HL)); }
void and_l() { and_r(lower_byte(HL)); }
void and_hlp() { and_r(read_byte(HL)); }
void and_a() { and_r(upper_byte(AF)); }
void xor_b() { xor_r(upper_byte(BC)); }
void xor_c() { xor_r(lower_byte(BC)); }
void xor_d() { xor_r(upper_byte(DE)); }
void xor_e() { xor_r(lower_byte(DE)); }
void xor_h() { xor_r(upper_byte(HL)); }
void xor_l() { xor_r(lower_byte(HL)); }
void xor_hlp() { xor_r(read_byte(HL)); }
void xor_a() { xor_r(upper_byte(AF)); }

void or_b() { or_r(upper_byte(BC)); }
void or_c() { or_r(lower_byte(BC)); }
void or_d() { or_r(upper_byte(DE)); }
void or_e() { or_r(lower_byte(DE)); }
void or_h() { or_r(upper_byte(HL)); }
void or_l() { or_r(lower_byte(HL)); }
void or_hlp() { or_r(read_byte(HL)); }
void or_a() { or_r(upper_byte(AF)); }
void cp_b() { cp_r(upper_byte(BC)); }
void cp_c() { cp_r(lower_byte(BC)); }
void cp_d() { cp_r(upper_byte(DE)); }
void cp_e() { cp_r(lower_byte(DE)); }
void cp_h() { cp_r(upper_byte(HL)); }
void cp_l() { cp_r(lower_byte(HL)); }
void cp_hlp() { cp_r(read_byte(HL)); }
void cp_a() { cp_r(upper_byte(AF)); }

void ret_nz()
{
	if (!get_flags(FLAG_ZERO))
	{
		pc = read_word(sp);
		sp += 2;
		tick(20);
	}
	else { tick(8); }
}
void pop_bc() { BC = read_word(sp); sp += 2; }
void jp_nz_nn(uint16_t operand)
{
	if (!get_flags(FLAG_ZERO))
	{
		pc = operand;
		tick(16);
	}
	else { tick(12); }
}
void jp_nn(uint16_t operand) { pc = operand; }
void call_nz_nn(uint16_t operand)
{
	if (!get_flags(FLAG_ZERO))
	{
		sp -= 2;
		write_word(sp, pc);
		pc = operand;
		tick(24);
	}
	else { tick(12); }
}
void push_bc() { sp -= 2; write_word(sp, BC); }
void add_a_n(uint8_t operand) { add_r(operand); }
void rst_00() { sp -= 2; write_word(sp, pc); pc = 0x00; }
void ret_z()
{
	if (get_flags(FLAG_ZERO))
	{
		pc = read_word(sp);
		sp += 2;
		tick(20);
	}
	else { tick(8); }
}
void ret() { pc = read_word(sp); sp += 2; }
void jp_z_nn(uint16_t operand)
{
	if (get_flags(FLAG_ZERO))
	{
		pc = operand;
		tick(16);
	}
	else { tick(12); }
}
void cb(uint8_t cb_opcode) { cb_n(cb_opcode); }
void call_z_nn(uint16_t operand)
{
	if (get_flags(FLAG_ZERO))
	{
		sp -= 2;
		write_word(sp, pc);
		pc = operand;
		tick(24);
	}
	else { tick(12); }
}
void call_nn(uint16_t operand)
{
	sp -= 2;
	write_word(sp, pc);
	pc = operand;
}
void adc_a_n(uint8_t operand) { adc_r(operand); }
void rst_08() { sp -= 2; write_word(sp, pc); pc = 0x08; }

void ret_nc()
{
	if (!get_flags(FLAG_CARRY))
	{
		pc = read_word(sp);
		sp += 2;
		tick(20);
	}
	else { tick(8); }
}
void pop_de() { DE = read_word(sp); sp += 2; }
void jp_nc_nn(uint16_t operand)
{
	if (!get_flags(FLAG_CARRY))
	{
		pc = operand;
		tick(16);
	}
	else { tick(12); }
}
// Placeholder - No opcode
void call_nc_nn(uint16_t operand)
{
	if (!get_flags(FLAG_CARRY))
	{
		sp -= 2;
		write_word(sp, pc);
		pc = operand;
		tick(24);
	}
	else { tick(12); }
}
void push_de() { sp -= 2; write_word(sp, DE); }
void sub_n(uint8_t operand) { sub_r(operand); }
void rst_10() { sp -= 2; write_word(sp, pc); pc = 0x10; }
void ret_c()
{
	if (get_flags(FLAG_CARRY))
	{
		pc = read_word(sp);
		sp += 2;
		tick(20);
	}
	else { tick(8); }
}
void reti()
{
	//std::cout << "reti called\n";
	pc = read_word(sp);
	sp += 2;
	IME = 1;
}
void jp_c_nn(uint16_t operand)
{
	if (get_flags(FLAG_CARRY))
	{
		pc = operand;
		tick(16);
	}
	else { tick(12); }
}
// Placeholder - No opcode
void call_c_nn(uint16_t operand)
{
	if (get_flags(FLAG_CARRY))
	{
		sp -= 2;
		write_word(sp, pc);
		pc = operand;
		tick(24);
	}
	else { tick(12); }
}
// Placeholder - No opcode
void sbc_a_n(uint8_t operand) { sbc_r(operand); }
void rst_18() { sp -= 2; write_word(sp, pc); pc = 0x18; }

void ldh_n_a(uint8_t operand) { write_byte(0xFF00 + operand, upper_byte(AF)); }
void pop_hl() { HL = read_word(sp); sp += 2; }
void ld_ff_c_a() { write_byte(0xFF00 + lower_byte(BC), upper_byte(AF)); }
// Placeholder - No opcode
// Placeholder - No opcode
void push_hl() { sp -= 2; write_word(sp, HL);}
void and_n(uint8_t operand) { and_r(operand); }
void rst_20() { sp -= 2; write_word(sp, pc); pc = 0x20; }
void add_sp_n(uint8_t operand)
{
	if ((sp & 0x0F) + (operand & 0x0F) > 0x0F)
		set_flags(FLAG_HALFCARRY);
	else
		clear_flags(FLAG_HALFCARRY);

	if ((sp & 0xFF) + operand > 0xFF)
		set_flags(FLAG_CARRY);
	else
		clear_flags(FLAG_CARRY);

	sp += (int8_t)operand;

	clear_flags(FLAG_ZERO | FLAG_NEGATIVE);
}
void jp_hlp() { pc = HL; }
void ld_nnp_a(uint16_t operand) { write_byte(operand, upper_byte(AF)); }
// Placeholder - No opcode
// Placeholder - No opcode
// Placeholder - No opcode
void xor_n(uint8_t operand) { xor_r(operand); }
void rst_28() { sp -= 2; write_word(sp, pc); pc = 0x28; }

void ldh_a_n(uint8_t operand) { set_reg_hi(AF, read_byte(0xFF00 + operand)); }
void pop_af() { AF = read_word(sp) & 0xFFF0; sp += 2; } // Lower 4 bits cannot be written to
void ld_ff_a_c() { set_reg_hi(AF, read_byte(0xFF00 + lower_byte(BC))); }
void di() { IME = 0; }
// Placeholder - No opcode
void push_af() { sp -= 2; write_word(sp, AF); }
void or_n(uint8_t operand) { or_r(operand); }
void rst_30(){ sp -= 2; write_word(sp, pc); pc = 0x30; }
void ld_hl_sp_n(uint8_t operand)
{
	if ((sp & 0xFF) + (operand & 0xFF) > 0xFF)
		set_flags(FLAG_CARRY);
	else
		clear_flags(FLAG_CARRY);

	if ((sp & 0x0F) + (operand & 0x0F) > 0x0F)
		set_flags(FLAG_HALFCARRY);
	else
		clear_flags(FLAG_HALFCARRY);

	HL = sp + (int8_t)operand;

	clear_flags(FLAG_ZERO | FLAG_NEGATIVE);
}
void ld_sp_hl() { sp = HL; }
void ld_a_nnp(uint16_t operand) { set_reg_hi(AF, read_byte(operand)); }
void ei() { IME_toggle = 1; }
// Placeholder - No opcode
// Placeholder - No opcode
void cp_n(uint8_t operand) { cp_r(operand); }
void rst_38() { sp -= 2; write_word(sp, pc); pc = 0x38; }

extern const instruction instructions[256] = {
	{ "NOP", 0, nop, 4 }, // 0x00
	{ "LD BC,d16", 2, ld_bc_nn, 12 }, // 0x1
	{ "LD (BC),A", 0, ld_bcp_a, 8 }, // 0x2
	{ "INC BC", 0, inc_bc, 8 }, // 0x3
	{ "INC B", 0, inc_b, 4 }, // 0x4
	{ "DEC B", 0, dec_b, 4 }, // 0x5
	{ "LD B,d8", 1, ld_b_n, 8 }, // 0x6
	{ "RLCA", 0, rlca, 4 }, // 0x7
	{ "LD (a16),SP", 2, ld_nnp_sp, 20 }, // 0x8
	{ "ADD HL,BC", 0, add_hl_bc, 8 }, // 0x9
	{ "LD A,(BC)", 0, ld_a_bcp, 8 }, // 0xa
	{ "DEC BC", 0, dec_bc, 8 }, // 0xb
	{ "INC C", 0, inc_c, 4 }, // 0xc
	{ "DEC C", 0, dec_c, 4 }, // 0xd
	{ "LD C,d8", 1, ld_c_n, 8 }, // 0xe
	{ "RRCA", 0, rrca, 4 }, // 0xf
	{ "STOP 0", 1, stop, 4 }, // 0x10
	{ "LD DE,d16", 2, ld_de_nn, 12 }, // 0x11
	{ "LD (DE),A", 0, ld_dep_a, 8 }, // 0x12
	{ "INC DE", 0, inc_de, 8 }, // 0x13
	{ "INC D", 0, inc_d, 4 }, // 0x14
	{ "DEC D", 0, dec_d, 4 }, // 0x15
	{ "LD D,d8", 1, ld_d_n, 8 }, // 0x16
	{ "RLA", 0, rla, 4 }, // 0x17
	{ "JR r8", 1, jr_n, 12 }, // 0x18
	{ "ADD HL,DE", 0, add_hl_de, 8 }, // 0x19
	{ "LD A,(DE)", 0, ld_a_dep, 8 }, // 0x1a
	{ "DEC DE", 0, dec_de, 8 }, // 0x1b
	{ "INC E", 0, inc_e, 4 }, // 0x1c
	{ "DEC E", 0, dec_e, 4 }, // 0x1d
	{ "LD E,d8", 1, ld_e_n, 8 }, // 0x1e
	{ "RRA", 0, rra, 4 }, // 0x1f
	{ "JR NZ,r8", 1, jr_nz_n, 0 }, // 0x20
	{ "LD HL,d16", 2, ld_hl_nn, 12 }, // 0x21
	{ "LD (HL+),A", 0, ldi_hlp_a, 8 }, // 0x22
	{ "INC HL", 0, inc_hl, 8 }, // 0x23
	{ "INC H", 0, inc_h, 4 }, // 0x24
	{ "DEC H", 0, dec_h, 4 }, // 0x25
	{ "LD H,d8", 1, ld_h_n, 8 }, // 0x26
	{ "DAA", 0, daa, 4 }, // 0x27
	{ "JR Z,r8", 1, jr_z_n, 0 }, // 0x28
	{ "ADD HL,HL", 0, add_hl_hl, 8 }, // 0x29
	{ "LD A,(HL+)", 0, ldi_a_hlp, 8 }, // 0x2a
	{ "DEC HL", 0, dec_hl, 8 }, // 0x2b
	{ "INC L", 0, inc_l, 4 }, // 0x2c
	{ "DEC L", 0, dec_l, 4 }, // 0x2d
	{ "LD L,d8", 1, ld_l_n, 8 }, // 0x2e
	{ "CPL", 0, cpl, 4 }, // 0x2f
	{ "JR NC,r8", 1, jr_nc_n, 0 }, // 0x30
	{ "LD SP,d16", 2, ld_sp_nn, 12 }, // 0x31
	{ "LD (HL-),A", 0, ldd_hlp_a, 8 }, // 0x32
	{ "INC SP", 0, inc_sp, 8 }, // 0x33
	{ "INC (HL)", 0, inc_hlp, 12 }, // 0x34
	{ "DEC (HL)", 0, dec_hlp, 12 }, // 0x35
	{ "LD (HL),d8", 1, ld_hlp_n, 12 }, // 0x36
	{ "SCF", 0, scf, 4 }, // 0x37
	{ "JR C,r8", 1, jr_c_n, 0 }, // 0x38
	{ "ADD HL,SP", 0, add_hl_sp, 8 }, // 0x39
	{ "LD A,(HL-)", 0, ldd_a_hlp, 8 }, // 0x3a
	{ "DEC SP", 0, dec_sp, 8 }, // 0x3b
	{ "INC A", 0, inc_a, 4 }, // 0x3c
	{ "DEC A", 0, dec_a, 4 }, // 0x3d
	{ "LD A,d8", 1, ld_a_n, 8 }, // 0x3e
	{ "CCF", 0, ccf, 4 }, // 0x3f
	{ "LD B,B", 0, ld_b_b, 4 }, // 0x40
	{ "LD B,C", 0, ld_b_c, 4 }, // 0x41
	{ "LD B,D", 0, ld_b_d, 4 }, // 0x42
	{ "LD B,E", 0, ld_b_e, 4 }, // 0x43
	{ "LD B,H", 0, ld_b_h, 4 }, // 0x44
	{ "LD B,L", 0, ld_b_l, 4 }, // 0x45
	{ "LD B,(HL)", 0, ld_b_hlp, 8 }, // 0x46
	{ "LD B,A", 0, ld_b_a, 4 }, // 0x47
	{ "LD C,B", 0, ld_c_b, 4 }, // 0x48
	{ "LD C,C", 0, ld_c_c, 4 }, // 0x49
	{ "LD C,D", 0, ld_c_d, 4 }, // 0x4a
	{ "LD C,E", 0, ld_c_e, 4 }, // 0x4b
	{ "LD C,H", 0, ld_c_h, 4 }, // 0x4c
	{ "LD C,L", 0, ld_c_l, 4 }, // 0x4d
	{ "LD C,(HL)", 0, ld_c_hlp, 8 }, // 0x4e
	{ "LD C,A", 0, ld_c_a, 4 }, // 0x4f
	{ "LD D,B", 0, ld_d_b, 4 }, // 0x50
	{ "LD D,C", 0, ld_d_c, 4 }, // 0x51
	{ "LD D,D", 0, ld_d_d, 4 }, // 0x52
	{ "LD D,E", 0, ld_d_e, 4 }, // 0x53
	{ "LD D,H", 0, ld_d_h, 4 }, // 0x54
	{ "LD D,L", 0, ld_d_l, 4 }, // 0x55
	{ "LD D,(HL)", 0, ld_d_hlp, 8 }, // 0x56
	{ "LD D,A", 0, ld_d_a, 4 }, // 0x57
	{ "LD E,B", 0, ld_e_b, 4 }, // 0x58
	{ "LD E,C", 0, ld_e_c, 4 }, // 0x59
	{ "LD E,D", 0, ld_e_d, 4 }, // 0x5a
	{ "LD E,E", 0, ld_e_e, 4 }, // 0x5b
	{ "LD E,H", 0, ld_e_h, 4 }, // 0x5c
	{ "LD E,L", 0, ld_e_l, 4 }, // 0x5d
	{ "LD E,(HL)", 0, ld_e_hlp, 8 }, // 0x5e
	{ "LD E,A", 0, ld_e_a, 4 }, // 0x5f
	{ "LD H,B", 0, ld_h_b, 4 }, // 0x60
	{ "LD H,C", 0, ld_h_c, 4 }, // 0x61
	{ "LD H,D", 0, ld_h_d, 4 }, // 0x62
	{ "LD H,E", 0, ld_h_e, 4 }, // 0x63
	{ "LD H,H", 0, ld_h_h, 4 }, // 0x64
	{ "LD H,L", 0, ld_h_l, 4 }, // 0x65
	{ "LD H,(HL)", 0, ld_h_hlp, 8 }, // 0x66
	{ "LD H,A", 0, ld_h_a, 4 }, // 0x67
	{ "LD L,B", 0, ld_l_b, 4 }, // 0x68
	{ "LD L,C", 0, ld_l_c, 4 }, // 0x69
	{ "LD L,D", 0, ld_l_d, 4 }, // 0x6a
	{ "LD L,E", 0, ld_l_e, 4 }, // 0x6b
	{ "LD L,H", 0, ld_l_h, 4 }, // 0x6c
	{ "LD L,L", 0, ld_l_l, 4 }, // 0x6d
	{ "LD L,(HL)", 0, ld_l_hlp, 8 }, // 0x6e
	{ "LD L,A", 0, ld_l_a, 4 }, // 0x6f
	{ "LD (HL),B", 0, ld_hlp_b, 8 }, // 0x70
	{ "LD (HL),C", 0, ld_hlp_c, 8 }, // 0x71
	{ "LD (HL),D", 0, ld_hlp_d, 8 }, // 0x72
	{ "LD (HL),E", 0, ld_hlp_e, 8 }, // 0x73
	{ "LD (HL),H", 0, ld_hlp_h, 8 }, // 0x74
	{ "LD (HL),L", 0, ld_hlp_l, 8 }, // 0x75
	{ "HALT", 0, halt, 4 }, // 0x76
	{ "LD (HL),A", 0, ld_hlp_a, 8 }, // 0x77
	{ "LD A,B", 0, ld_a_b, 4 }, // 0x78
	{ "LD A,C", 0, ld_a_c, 4 }, // 0x79
	{ "LD A,D", 0, ld_a_d, 4 }, // 0x7a
	{ "LD A,E", 0, ld_a_e, 4 }, // 0x7b
	{ "LD A,H", 0, ld_a_h, 4 }, // 0x7c
	{ "LD A,L", 0, ld_a_l, 4 }, // 0x7d
	{ "LD A,(HL)", 0, ld_a_hlp, 8 }, // 0x7e
	{ "LD A,A", 0, ld_a_a, 4 }, // 0x7f
	{ "ADD A,B", 0, add_a_b, 4 }, // 0x80
	{ "ADD A,C", 0, add_a_c, 4 }, // 0x81
	{ "ADD A,D", 0, add_a_d, 4 }, // 0x82
	{ "ADD A,E", 0, add_a_e, 4 }, // 0x83
	{ "ADD A,H", 0, add_a_h, 4 }, // 0x84
	{ "ADD A,L", 0, add_a_l, 4 }, // 0x85
	{ "ADD A,(HL)", 0, add_a_hlp, 8 }, // 0x86
	{ "ADD A,A", 0, add_a_a, 4 }, // 0x87
	{ "ADC A,B", 0, adc_a_b, 4 }, // 0x88
	{ "ADC A,C", 0, adc_a_c, 4 }, // 0x89
	{ "ADC A,D", 0, adc_a_d, 4 }, // 0x8a
	{ "ADC A,E", 0, adc_a_e, 4 }, // 0x8b
	{ "ADC A,H", 0, adc_a_h, 4 }, // 0x8c
	{ "ADC A,L", 0, adc_a_l, 4 }, // 0x8d
	{ "ADC A,(HL)", 0, adc_a_hlp, 8 }, // 0x8e
	{ "ADC A,A", 0, adc_a_a, 4 }, // 0x8f
	{ "SUB B", 0, sub_b, 4 }, // 0x90
	{ "SUB C", 0, sub_c, 4 }, // 0x91
	{ "SUB D", 0, sub_d, 4 }, // 0x92
	{ "SUB E", 0, sub_e, 4 }, // 0x93
	{ "SUB H", 0, sub_h, 4 }, // 0x94
	{ "SUB L", 0, sub_l, 4 }, // 0x95
	{ "SUB (HL)", 0, sub_hlp, 8 }, // 0x96
	{ "SUB A", 0, sub_a, 4 }, // 0x97
	{ "SBC A,B", 0, sbc_a_b, 4 }, // 0x98
	{ "SBC A,C", 0, sbc_a_c, 4 }, // 0x99
	{ "SBC A,D", 0, sbc_a_d, 4 }, // 0x9a
	{ "SBC A,E", 0, sbc_a_e, 4 }, // 0x9b
	{ "SBC A,H", 0, sbc_a_h, 4 }, // 0x9c
	{ "SBC A,L", 0, sbc_a_l, 4 }, // 0x9d
	{ "SBC A,(HL)", 0, sbc_a_hlp, 8 }, // 0x9e
	{ "SBC A,A", 0, sbc_a_a, 4 }, // 0x9f
	{ "AND B", 0, and_b, 4 }, // 0xa0
	{ "AND C", 0, and_c, 4 }, // 0xa1
	{ "AND D", 0, and_d, 4 }, // 0xa2
	{ "AND E", 0, and_e, 4 }, // 0xa3
	{ "AND H", 0, and_h, 4 }, // 0xa4
	{ "AND L", 0, and_l, 4 }, // 0xa5
	{ "AND (HL)", 0, and_hlp, 8 }, // 0xa6
	{ "AND A", 0, and_a, 4 }, // 0xa7
	{ "XOR B", 0, xor_b, 4 }, // 0xa8
	{ "XOR C", 0, xor_c, 4 }, // 0xa9
	{ "XOR D", 0, xor_d, 4 }, // 0xaa
	{ "XOR E", 0, xor_e, 4 }, // 0xab
	{ "XOR H", 0, xor_h, 4 }, // 0xac
	{ "XOR L", 0, xor_l, 4 }, // 0xad
	{ "XOR (HL)", 0, xor_hlp, 8 }, // 0xae
	{ "XOR A", 0, xor_a, 4 }, // 0xaf
	{ "OR B", 0, or_b, 4 }, // 0xb0
	{ "OR C", 0, or_c, 4 }, // 0xb1
	{ "OR D", 0, or_d, 4 }, // 0xb2
	{ "OR E", 0, or_e, 4 }, // 0xb3
	{ "OR H", 0, or_h, 4 }, // 0xb4
	{ "OR L", 0, or_l, 4 }, // 0xb5
	{ "OR (HL)", 0, or_hlp, 8 }, // 0xb6
	{ "OR A", 0, or_a, 4 }, // 0xb7
	{ "CP B", 0, cp_b, 4 }, // 0xb8
	{ "CP C", 0, cp_c, 4 }, // 0xb9
	{ "CP D", 0, cp_d, 4 }, // 0xba
	{ "CP E", 0, cp_e, 4 }, // 0xbb
	{ "CP H", 0, cp_h, 4 }, // 0xbc
	{ "CP L", 0, cp_l, 4 }, // 0xbd
	{ "CP (HL)", 0, cp_hlp, 8 }, // 0xbe
	{ "CP A", 0, cp_a, 4 }, // 0xbf
	{ "RET NZ", 0, ret_nz, 0 }, // 0xc0
	{ "POP BC", 0, pop_bc, 12 }, // 0xc1
	{ "JP NZ,a16", 2, jp_nz_nn, 0 }, // 0xc2
	{ "JP a16", 2, jp_nn, 16 }, // 0xc3
	{ "CALL NZ,a16", 2, call_nz_nn, 0 }, // 0xc4
	{ "PUSH BC", 0, push_bc, 16 }, // 0xc5
	{ "ADD A,d8", 1, add_a_n, 8 }, // 0xc6
	{ "RST 00H", 0, rst_00, 16 }, // 0xc7
	{ "RET Z", 0, ret_z, 0 }, // 0xc8
	{ "RET", 0, ret, 16 }, // 0xc9
	{ "JP Z,a16", 2, jp_z_nn, 0 }, // 0xca
	{ "PREFIX CB", 1, cb, 4 }, // 0xcb
	{ "CALL Z,a16", 2, call_z_nn, 0 }, // 0xcc
	{ "CALL a16", 2, call_nn, 24 }, // 0xcd
	{ "ADC A,d8", 1, adc_a_n, 8 }, // 0xce
	{ "RST 08H", 0, rst_08, 16 }, // 0xcf
	{ "RET NC", 0, ret_nc, 0 }, // 0xd0
	{ "POP DE", 0, pop_de, 12 }, // 0xd1
	{ "JP NC,a16", 2, jp_nc_nn, 0 }, // 0xd2
	{ "UNDEFINED", 0, undefined, 0 }, // 0xd3
	{ "CALL NC,a16", 2, call_nc_nn, 0 }, // 0xd4
	{ "PUSH DE", 0, push_de, 16 }, // 0xd5
	{ "SUB d8", 1, sub_n, 8 }, // 0xd6
	{ "RST 10H", 0, rst_10, 16 }, // 0xd7
	{ "RET C", 0, ret_c, 0 }, // 0xd8
	{ "RETI", 0, reti, 16 }, // 0xd9
	{ "JP C,a16", 2, jp_c_nn, 0 }, // 0xda
	{ "UNDEFINED", 0, undefined, 0 }, // 0xdb
	{ "CALL C,a16", 2, call_c_nn, 0 }, // 0xdc
	{ "UNDEFINED", 0, undefined, 0 }, // 0xdd
	{ "SBC A,d8", 1, sbc_a_n, 8 }, // 0xde
	{ "RST 18H", 0, rst_18, 16 }, // 0xdf
	{ "LDH (a8),A", 1, ldh_n_a, 12 }, // 0xe0
	{ "POP HL", 0, pop_hl, 12 }, // 0xe1
	{ "LD (C),A", 0, ld_ff_c_a, 8 }, // 0xe2
	{ "UNDEFINED", 0, undefined, 0 }, // 0xe3
	{ "UNDEFINED", 0, undefined, 0 }, // 0xe4
	{ "PUSH HL", 0, push_hl, 16 }, // 0xe5
	{ "AND d8", 1, and_n, 8 }, // 0xe6
	{ "RST 20H", 0, rst_20, 16 }, // 0xe7
	{ "ADD SP,r8", 1, add_sp_n, 16 }, // 0xe8
	{ "JP (HL)", 0, jp_hlp, 4 }, // 0xe9
	{ "LD (a16),A", 2, ld_nnp_a, 16 }, // 0xea
	{ "UNDEFINED", 0, undefined, 0 }, // 0xeb
	{ "UNDEFINED", 0, undefined, 0 }, // 0xec
	{ "UNDEFINED", 0, undefined, 0 }, // 0xed
	{ "XOR d8", 1, xor_n, 8 }, // 0xee
	{ "RST 28H", 0, rst_28, 16 }, // 0xef
	{ "LDH A,(a8)", 1, ldh_a_n, 12 }, // 0xf0
	{ "POP AF", 0, pop_af, 12 }, // 0xf1
	{ "LD A,(C)", 0, ld_ff_a_c, 8 }, // 0xf2
	{ "DI", 0, di, 4 }, // 0xf3
	{ "UNDEFINED", 0, undefined, 0 }, // 0xf4
	{ "PUSH AF", 0, push_af, 16 }, // 0xf5
	{ "OR d8", 1, or_n, 8 }, // 0xf6
	{ "RST 30H", 0, rst_30, 16 }, // 0xf7
	{ "LD HL,SP+r8", 1, ld_hl_sp_n, 12 }, // 0xf8
	{ "LD SP,HL", 0, ld_sp_hl, 8 }, // 0xf9
	{ "LD A,(a16)", 2, ld_a_nnp, 16 }, // 0xfa
	{ "EI", 0, ei, 4 }, // 0xfb
	{ "UNDEFINED", 0, undefined, 0 }, // 0xfc
	{ "UNDEFINED", 0, undefined, 0 }, // 0xfd
	{ "CP d8", 1, cp_n, 8 }, // 0xfe
	{ "RST 38H", 0, rst_38, 16 }, // 0xff
};