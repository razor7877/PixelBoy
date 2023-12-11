#include <stdio.h>

#include "cpu_cb.hpp"
#include "memory.hpp"

void cb_n(uint8_t cb_opcode)
{
	((void(*)(void))cb_instructions[cb_opcode].function)();
	tick(cb_instructions[cb_opcode].duration);
}

uint8_t rlc(uint8_t value)
{
	(value & 0x80) ? set_flags(FLAG_CARRY) : clear_flags(FLAG_CARRY);

	value = (value << 1) | ((value & 0x80) >> 7);

	if (value == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE | FLAG_HALFCARRY);

	return value;
}

uint8_t rrc(uint8_t value)
{
	(value & 0x01) ? set_flags(FLAG_CARRY) : clear_flags(FLAG_CARRY);

	value = (value >> 1) | ((value & 0x01) << 7);

	if (value == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE | FLAG_HALFCARRY);

	return value;
}

uint8_t rl(uint8_t value)
{
	bool bit_data = (value & 0x80);

	value = (value << 1) | get_flags(FLAG_CARRY);
	
	bit_data ? set_flags(FLAG_CARRY) : clear_flags(FLAG_CARRY);

	if (value == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE | FLAG_HALFCARRY);

	return value;
}

uint8_t rr(uint8_t value)
{
	bool bit_data = (value & 0x01);

	value = (value >> 1) | (get_flags(FLAG_CARRY) << 7);

	bit_data ? set_flags(FLAG_CARRY) : clear_flags(FLAG_CARRY);

	if (value == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE | FLAG_HALFCARRY);

	return value;
}

uint8_t sla(uint8_t value)
{
	if (value & 0x80)
		set_flags(FLAG_CARRY);
	else
		clear_flags(FLAG_CARRY);

	value <<= 1;

	if (value == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE | FLAG_HALFCARRY);

	return value;
}

uint8_t sra(uint8_t value)
{
	if (value & 0x01)
		set_flags(FLAG_CARRY);
	else
		clear_flags(FLAG_CARRY);

	value = (value >> 1) | (value & 0x80);

	if (value == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE | FLAG_HALFCARRY);

	return value;
}

uint8_t swap(uint8_t value)
{
	value = ((value & 0xF0) >> 4 | (value & 0x0F) << 4);

	if (value == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE | FLAG_HALFCARRY | FLAG_CARRY);

	return value;
}

uint8_t srl(uint8_t value)
{
	if (value & 0x01)
		set_flags(FLAG_CARRY);
	else
		clear_flags(FLAG_CARRY);

	value >>= 1;

	if (value == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE | FLAG_HALFCARRY);

	return value;
}

void bit(uint8_t bit, uint8_t value)
{
	if ((value & (0x01 << bit)) == 0)
		set_flags(FLAG_ZERO);
	else
		clear_flags(FLAG_ZERO);

	clear_flags(FLAG_NEGATIVE);
	set_flags(FLAG_HALFCARRY);
}

void rlc_b() { set_reg_hi(BC, rlc(upper_byte(BC))); }
void rlc_c() { set_reg_lo(BC, rlc(lower_byte(BC))); }
void rlc_d() { set_reg_hi(DE, rlc(upper_byte(DE))); }
void rlc_e() { set_reg_lo(DE, rlc(lower_byte(DE))); }
void rlc_h() { set_reg_hi(HL, rlc(upper_byte(HL))); }
void rlc_l() { set_reg_lo(HL, rlc(lower_byte(HL))); }
void rlc_hlp() { write_byte(HL, rlc(read_byte(HL))); }
void rlc_a() { set_reg_hi(AF, rlc(upper_byte(AF))); }
void rrc_b() { set_reg_hi(BC, rrc(upper_byte(BC))); }
void rrc_c() { set_reg_lo(BC, rrc(lower_byte(BC))); }
void rrc_d() { set_reg_hi(DE, rrc(upper_byte(DE))); }
void rrc_e() { set_reg_lo(DE, rrc(lower_byte(DE))); }
void rrc_h() { set_reg_hi(HL, rrc(upper_byte(HL))); }
void rrc_l() { set_reg_lo(HL, rrc(lower_byte(HL))); }
void rrc_hlp() { write_byte(HL, rrc(read_byte(HL))); }
void rrc_a() { set_reg_hi(AF, rrc(upper_byte(AF))); }

void rl_b() { set_reg_hi(BC, rl(upper_byte(BC))); }
void rl_c() { set_reg_lo(BC, rl(lower_byte(BC))); }
void rl_d() { set_reg_hi(DE, rl(upper_byte(DE))); }
void rl_e() { set_reg_lo(DE, rl(lower_byte(DE))); }
void rl_h() { set_reg_hi(HL, rl(upper_byte(HL))); }
void rl_l() { set_reg_lo(HL, rl(lower_byte(HL))); }
void rl_hlp() { write_byte(HL, rl(read_byte(HL))); }
void rl_a() { set_reg_hi(AF, rl(upper_byte(AF))); }
void rr_b() { set_reg_hi(BC, rr(upper_byte(BC))); }
void rr_c() { set_reg_lo(BC, rr(lower_byte(BC))); }
void rr_d() { set_reg_hi(DE, rr(upper_byte(DE))); }
void rr_e() { set_reg_lo(DE, rr(lower_byte(DE))); }
void rr_h() { set_reg_hi(HL, rr(upper_byte(HL))); }
void rr_l() { set_reg_lo(HL, rr(lower_byte(HL))); }
void rr_hlp() { write_byte(HL, rr(read_byte(HL))); }
void rr_a() { set_reg_hi(AF, rr(upper_byte(AF))); }

void sla_b() { set_reg_hi(BC, sla(upper_byte(BC))); }
void sla_c() { set_reg_lo(BC, sla(lower_byte(BC))); }
void sla_d() { set_reg_hi(DE, sla(upper_byte(DE))); }
void sla_e() { set_reg_lo(DE, sla(lower_byte(DE))); }
void sla_h() { set_reg_hi(HL, sla(upper_byte(HL))); }
void sla_l() { set_reg_lo(HL, sla(lower_byte(HL))); }
void sla_hlp() { write_byte(HL, sla(read_byte(HL))); }
void sla_a() { set_reg_hi(AF, sla(upper_byte(AF))); }
void sra_b() { set_reg_hi(BC, sra(upper_byte(BC))); }
void sra_c() { set_reg_lo(BC, sra(lower_byte(BC))); }
void sra_d() { set_reg_hi(DE, sra(upper_byte(DE))); }
void sra_e() { set_reg_lo(DE, sra(lower_byte(DE))); }
void sra_h() { set_reg_hi(HL, sra(upper_byte(HL))); }
void sra_l() { set_reg_lo(HL, sra(lower_byte(HL))); }
void sra_hlp() { write_byte(HL, sra(read_byte(HL))); }
void sra_a() { set_reg_hi(AF, sra(upper_byte(AF))); }

void swap_b() { set_reg_hi(BC, swap(upper_byte(BC))); }
void swap_c() { set_reg_lo(BC, swap(lower_byte(BC))); }
void swap_d() { set_reg_hi(DE, swap(upper_byte(DE))); }
void swap_e() { set_reg_lo(DE, swap(lower_byte(DE))); }
void swap_h() { set_reg_hi(HL, swap(upper_byte(HL))); }
void swap_l() { set_reg_lo(HL, swap(lower_byte(HL))); }
void swap_hlp() { write_byte(HL, swap(read_byte(HL))); }
void swap_a() { set_reg_hi(AF, swap(upper_byte(AF))); }
void srl_b() { set_reg_hi(BC, srl(upper_byte(BC))); }
void srl_c() { set_reg_lo(BC, srl(lower_byte(BC))); }
void srl_d() { set_reg_hi(DE, srl(upper_byte(DE))); }
void srl_e() { set_reg_lo(DE, srl(lower_byte(DE))); }
void srl_h() { set_reg_hi(HL, srl(upper_byte(HL))); }
void srl_l() { set_reg_lo(HL, srl(lower_byte(HL))); }
void srl_hlp() { write_byte(HL, srl(read_byte(HL))); }
void srl_a() { set_reg_hi(AF, srl(upper_byte(AF))); }

void bit_0_b() { bit(0, upper_byte(BC)); }
void bit_0_c() { bit(0, lower_byte(BC)); }
void bit_0_d() { bit(0, upper_byte(DE)); }
void bit_0_e() { bit(0, lower_byte(DE)); }
void bit_0_h() { bit(0, upper_byte(HL)); }
void bit_0_l() { bit(0, lower_byte(HL)); }
void bit_0_hlp() { bit(0, read_byte(HL)); }
void bit_0_a() { bit(0, upper_byte(AF)); }
void bit_1_b() { bit(1, upper_byte(BC)); }
void bit_1_c() { bit(1, lower_byte(BC)); }
void bit_1_d() { bit(1, upper_byte(DE)); }
void bit_1_e() { bit(1, lower_byte(DE)); }
void bit_1_h() { bit(1, upper_byte(HL)); }
void bit_1_l() { bit(1, lower_byte(HL)); }
void bit_1_hlp() { bit(1, read_byte(HL)); }
void bit_1_a() { bit(1, upper_byte(AF)); }

void bit_2_b() { bit(2, upper_byte(BC)); }
void bit_2_c() { bit(2, lower_byte(BC)); }
void bit_2_d() { bit(2, upper_byte(DE)); }
void bit_2_e() { bit(2, lower_byte(DE)); }
void bit_2_h() { bit(2, upper_byte(HL)); }
void bit_2_l() { bit(2, lower_byte(HL)); }
void bit_2_hlp() { bit(2, read_byte(HL)); }
void bit_2_a() { bit(2, upper_byte(AF)); }
void bit_3_b() { bit(3, upper_byte(BC)); }
void bit_3_c() { bit(3, lower_byte(BC)); }
void bit_3_d() { bit(3, upper_byte(DE)); }
void bit_3_e() { bit(3, lower_byte(DE)); }
void bit_3_h() { bit(3, upper_byte(HL)); }
void bit_3_l() { bit(3, lower_byte(HL)); }
void bit_3_hlp() { bit(3, read_byte(HL)); }
void bit_3_a() { bit(3, upper_byte(AF)); }

void bit_4_b() { bit(4, upper_byte(BC)); }
void bit_4_c() { bit(4, lower_byte(BC)); }
void bit_4_d() { bit(4, upper_byte(DE)); }
void bit_4_e() { bit(4, lower_byte(DE)); }
void bit_4_h() { bit(4, upper_byte(HL)); }
void bit_4_l() { bit(4, lower_byte(HL)); }
void bit_4_hlp() { bit(4, read_byte(HL)); }
void bit_4_a() { bit(4, upper_byte(AF)); }
void bit_5_b() { bit(5, upper_byte(BC)); }
void bit_5_c() { bit(5, lower_byte(BC)); }
void bit_5_d() { bit(5, upper_byte(DE)); }
void bit_5_e() { bit(5, lower_byte(DE)); }
void bit_5_h() { bit(5, upper_byte(HL)); }
void bit_5_l() { bit(5, lower_byte(HL)); }
void bit_5_hlp() { bit(5, read_byte(HL)); }
void bit_5_a() { bit(5, upper_byte(AF)); }

void bit_6_b() { bit(6, upper_byte(BC)); }
void bit_6_c() { bit(6, lower_byte(BC)); }
void bit_6_d() { bit(6, upper_byte(DE)); }
void bit_6_e() { bit(6, lower_byte(DE)); }
void bit_6_h() { bit(6, upper_byte(HL)); }
void bit_6_l() { bit(6, lower_byte(HL)); }
void bit_6_hlp() { bit(6, read_byte(HL)); }
void bit_6_a() { bit(6, upper_byte(AF)); }
void bit_7_b() { bit(7, upper_byte(BC)); }
void bit_7_c() { bit(7, lower_byte(BC)); }
void bit_7_d() { bit(7, upper_byte(DE)); }
void bit_7_e() { bit(7, lower_byte(DE)); }
void bit_7_h() { bit(7, upper_byte(HL)); }
void bit_7_l() { bit(7, lower_byte(HL)); }
void bit_7_hlp() { bit(7, read_byte(HL)); }
void bit_7_a() { bit(7, upper_byte(AF)); }

void res_0_b() { set_reg_hi(BC, upper_byte(BC) & ~0x01); }
void res_0_c() { set_reg_lo(BC, lower_byte(BC) & ~0x01); }
void res_0_d() { set_reg_hi(DE, upper_byte(DE) & ~0x01); }
void res_0_e() { set_reg_lo(DE, lower_byte(DE) & ~0x01); }
void res_0_h() { set_reg_hi(HL, upper_byte(HL) & ~0x01); }
void res_0_l() { set_reg_lo(HL, lower_byte(HL) & ~0x01); }
void res_0_hlp() { write_byte(HL, read_byte(HL) & ~0x01); }
void res_0_a() { set_reg_hi(AF, upper_byte(AF) & ~0x01); }
void res_1_b() { set_reg_hi(BC, upper_byte(BC) & ~0x02); }
void res_1_c() { set_reg_lo(BC, lower_byte(BC) & ~0x02); }
void res_1_d() { set_reg_hi(DE, upper_byte(DE) & ~0x02); }
void res_1_e() { set_reg_lo(DE, lower_byte(DE) & ~0x02); }
void res_1_h() { set_reg_hi(HL, upper_byte(HL) & ~0x02); }
void res_1_l() { set_reg_lo(HL, lower_byte(HL) & ~0x02); }
void res_1_hlp() { write_byte(HL, read_byte(HL) & ~0x02); }
void res_1_a() { set_reg_hi(AF, upper_byte(AF) & ~0x02); }

void res_2_b() { set_reg_hi(BC, upper_byte(BC) & ~0x04); }
void res_2_c() { set_reg_lo(BC, lower_byte(BC) & ~0x04); }
void res_2_d() { set_reg_hi(DE, upper_byte(DE) & ~0x04); }
void res_2_e() { set_reg_lo(DE, lower_byte(DE) & ~0x04); }
void res_2_h() { set_reg_hi(HL, upper_byte(HL) & ~0x04); }
void res_2_l() { set_reg_lo(HL, lower_byte(HL) & ~0x04); }
void res_2_hlp() { write_byte(HL, read_byte(HL) & ~0x04); }
void res_2_a() { set_reg_hi(AF, upper_byte(AF) & ~0x04); }
void res_3_b() { set_reg_hi(BC, upper_byte(BC) & ~0x08); }
void res_3_c() { set_reg_lo(BC, lower_byte(BC) & ~0x08); }
void res_3_d() { set_reg_hi(DE, upper_byte(DE) & ~0x08); }
void res_3_e() { set_reg_lo(DE, lower_byte(DE) & ~0x08); }
void res_3_h() { set_reg_hi(HL, upper_byte(HL) & ~0x08); }
void res_3_l() { set_reg_lo(HL, lower_byte(HL) & ~0x08); }
void res_3_hlp() { write_byte(HL, read_byte(HL) & ~0x08); }
void res_3_a() { set_reg_hi(AF, upper_byte(AF) & ~0x08); }

void res_4_b() { set_reg_hi(BC, upper_byte(BC) & ~0x10); }
void res_4_c() { set_reg_lo(BC, lower_byte(BC) & ~0x10); }
void res_4_d() { set_reg_hi(DE, upper_byte(DE) & ~0x10); }
void res_4_e() { set_reg_lo(DE, lower_byte(DE) & ~0x10); }
void res_4_h() { set_reg_hi(HL, upper_byte(HL) & ~0x10); }
void res_4_l() { set_reg_lo(HL, lower_byte(HL) & ~0x10); }
void res_4_hlp() { write_byte(HL, read_byte(HL) & ~0x10); }
void res_4_a() { set_reg_hi(AF, upper_byte(AF) & ~0x10); }
void res_5_b() { set_reg_hi(BC, upper_byte(BC) & ~0x20); }
void res_5_c() { set_reg_lo(BC, lower_byte(BC) & ~0x20); }
void res_5_d() { set_reg_hi(DE, upper_byte(DE) & ~0x20); }
void res_5_e() { set_reg_lo(DE, lower_byte(DE) & ~0x20); }
void res_5_h() { set_reg_hi(HL, upper_byte(HL) & ~0x20); }
void res_5_l() { set_reg_lo(HL, lower_byte(HL) & ~0x20); }
void res_5_hlp() { write_byte(HL, read_byte(HL) & ~0x20); }
void res_5_a() { set_reg_hi(AF, upper_byte(AF) & ~0x20); }

void res_6_b() { set_reg_hi(BC, upper_byte(BC) & ~0x40); }
void res_6_c() { set_reg_lo(BC, lower_byte(BC) & ~0x40); }
void res_6_d() { set_reg_hi(DE, upper_byte(DE) & ~0x40); }
void res_6_e() { set_reg_lo(DE, lower_byte(DE) & ~0x40); }
void res_6_h() { set_reg_hi(HL, upper_byte(HL) & ~0x40); }
void res_6_l() { set_reg_lo(HL, lower_byte(HL) & ~0x40); }
void res_6_hlp() { write_byte(HL, read_byte(HL) & ~0x40); }
void res_6_a() { set_reg_hi(AF, upper_byte(AF) & ~0x40); }
void res_7_b() { set_reg_hi(BC, upper_byte(BC) & ~0x80); }
void res_7_c() { set_reg_lo(BC, lower_byte(BC) & ~0x80); }
void res_7_d() { set_reg_hi(DE, upper_byte(DE) & ~0x80); }
void res_7_e() { set_reg_lo(DE, lower_byte(DE) & ~0x80); }
void res_7_h() { set_reg_hi(HL, upper_byte(HL) & ~0x80); }
void res_7_l() { set_reg_lo(HL, lower_byte(HL) & ~0x80); }
void res_7_hlp() { write_byte(HL, read_byte(HL) & ~0x80); }
void res_7_a() { set_reg_hi(AF, upper_byte(AF) & ~0x80); }

void set_0_b() { set_reg_hi(BC, upper_byte(BC) | 0x01); }
void set_0_c() { set_reg_lo(BC, lower_byte(BC) | 0x01); }
void set_0_d() { set_reg_hi(DE, upper_byte(DE) | 0x01); }
void set_0_e() { set_reg_lo(DE, lower_byte(DE) | 0x01); }
void set_0_h() { set_reg_hi(HL, upper_byte(HL) | 0x01); }
void set_0_l() { set_reg_lo(HL, lower_byte(HL) | 0x01); }
void set_0_hlp() { write_byte(HL, read_byte(HL) | 0x01); }
void set_0_a() { set_reg_hi(AF, upper_byte(AF) | 0x01); }
void set_1_b() { set_reg_hi(BC, upper_byte(BC) | 0x02); }
void set_1_c() { set_reg_lo(BC, lower_byte(BC) | 0x02); }
void set_1_d() { set_reg_hi(DE, upper_byte(DE) | 0x02); }
void set_1_e() { set_reg_lo(DE, lower_byte(DE) | 0x02); }
void set_1_h() { set_reg_hi(HL, upper_byte(HL) | 0x02); }
void set_1_l() { set_reg_lo(HL, lower_byte(HL) | 0x02); }
void set_1_hlp() { write_byte(HL, read_byte(HL) | 0x02); }
void set_1_a(){ set_reg_hi(AF, upper_byte(AF) | 0x02); }

void set_2_b() { set_reg_hi(BC, upper_byte(BC) | 0x04); }
void set_2_c() { set_reg_lo(BC, lower_byte(BC) | 0x04); }
void set_2_d() { set_reg_hi(DE, upper_byte(DE) | 0x04); }
void set_2_e() { set_reg_lo(DE, lower_byte(DE) | 0x04); }
void set_2_h() { set_reg_hi(HL, upper_byte(HL) | 0x04); }
void set_2_l() { set_reg_lo(HL, lower_byte(HL) | 0x04); }
void set_2_hlp() { write_byte(HL, read_byte(HL) | 0x04); }
void set_2_a() { set_reg_hi(AF, upper_byte(AF) | 0x04); }
void set_3_b() { set_reg_hi(BC, upper_byte(BC) | 0x08); }
void set_3_c() { set_reg_lo(BC, lower_byte(BC) | 0x08); }
void set_3_d() { set_reg_hi(DE, upper_byte(DE) | 0x08); }
void set_3_e() { set_reg_lo(DE, lower_byte(DE) | 0x08); }
void set_3_h() { set_reg_hi(HL, upper_byte(HL) | 0x08); }
void set_3_l() { set_reg_lo(HL, lower_byte(HL) | 0x08); }
void set_3_hlp() { write_byte(HL, read_byte(HL) | 0x08); }
void set_3_a() { set_reg_hi(AF, upper_byte(AF) | 0x08); }

void set_4_b() { set_reg_hi(BC, upper_byte(BC) | 0x10); }
void set_4_c() { set_reg_lo(BC, lower_byte(BC) | 0x10); }
void set_4_d() { set_reg_hi(DE, upper_byte(DE) | 0x10); }
void set_4_e() { set_reg_lo(DE, lower_byte(DE) | 0x10); }
void set_4_h() { set_reg_hi(HL, upper_byte(HL) | 0x10); }
void set_4_l() { set_reg_lo(HL, lower_byte(HL) | 0x10); }
void set_4_hlp() { write_byte(HL, read_byte(HL) | 0x10); }
void set_4_a() { set_reg_hi(AF, upper_byte(AF) | 0x10); }
void set_5_b() { set_reg_hi(BC, upper_byte(BC) | 0x20); }
void set_5_c() { set_reg_lo(BC, lower_byte(BC) | 0x20); }
void set_5_d() { set_reg_hi(DE, upper_byte(DE) | 0x20); }
void set_5_e() { set_reg_lo(DE, lower_byte(DE) | 0x20); }
void set_5_h() { set_reg_hi(HL, upper_byte(HL) | 0x20); }
void set_5_l() { set_reg_lo(HL, lower_byte(HL) | 0x20); }
void set_5_hlp() { write_byte(HL, read_byte(HL) | 0x20); }
void set_5_a() { set_reg_hi(AF, upper_byte(AF) | 0x20); }

void set_6_b() { set_reg_hi(BC, upper_byte(BC) | 0x40); }
void set_6_c() { set_reg_lo(BC, lower_byte(BC) | 0x40); }
void set_6_d() { set_reg_hi(DE, upper_byte(DE) | 0x40); }
void set_6_e() { set_reg_lo(DE, lower_byte(DE) | 0x40); }
void set_6_h() { set_reg_hi(HL, upper_byte(HL) | 0x40); }
void set_6_l() { set_reg_lo(HL, lower_byte(HL) | 0x40); }
void set_6_hlp() { write_byte(HL, read_byte(HL) | 0x40); }
void set_6_a() { set_reg_hi(AF, upper_byte(AF) | 0x40); }
void set_7_b() { set_reg_hi(BC, upper_byte(BC) | 0x80); }
void set_7_c() { set_reg_lo(BC, lower_byte(BC) | 0x80); }
void set_7_d() { set_reg_hi(DE, upper_byte(DE) | 0x80); }
void set_7_e() { set_reg_lo(DE, lower_byte(DE) | 0x80); }
void set_7_h() { set_reg_hi(HL, upper_byte(HL) | 0x80); }
void set_7_l() { set_reg_lo(HL, lower_byte(HL) | 0x80); }
void set_7_hlp() { write_byte(HL, read_byte(HL) | 0x80); }
void set_7_a() { set_reg_hi(AF, upper_byte(AF) | 0x80); }

extern const Instruction cb_instructions[256] = {
	{ "RLC B", 1, rlc_b, 8 }, // 0x0
	{ "RLC C", 1, rlc_c, 8 }, // 0x1
	{ "RLC D", 1, rlc_d, 8 }, // 0x2
	{ "RLC E", 1, rlc_e, 8 }, // 0x3
	{ "RLC H", 1, rlc_h, 8 }, // 0x4
	{ "RLC L", 1, rlc_l, 8 }, // 0x5
	{ "RLC (HL)", 1, rlc_hlp, 16 }, // 0x6
	{ "RLC A", 1, rlc_a, 8 }, // 0x7
	{ "RRC B", 1, rrc_b, 8 }, // 0x8
	{ "RRC C", 1, rrc_c, 8 }, // 0x9
	{ "RRC D", 1, rrc_d, 8 }, // 0xa
	{ "RRC E", 1, rrc_e, 8 }, // 0xb
	{ "RRC H", 1, rrc_h, 8 }, // 0xc
	{ "RRC L", 1, rrc_l, 8 }, // 0xd
	{ "RRC (HL)", 1, rrc_hlp, 16 }, // 0xe
	{ "RRC A", 1, rrc_a, 8 }, // 0xf
	{ "RL B", 1, rl_b, 8 }, // 0x10
	{ "RL C", 1, rl_c, 8 }, // 0x11
	{ "RL D", 1, rl_d, 8 }, // 0x12
	{ "RL E", 1, rl_e, 8 }, // 0x13
	{ "RL H", 1, rl_h, 8 }, // 0x14
	{ "RL L", 1, rl_l, 8 }, // 0x15
	{ "RL (HL)", 1, rl_hlp, 16 }, // 0x16
	{ "RL A", 1, rl_a, 8 }, // 0x17
	{ "RR B", 1, rr_b, 8 }, // 0x18
	{ "RR C", 1, rr_c, 8 }, // 0x19
	{ "RR D", 1, rr_d, 8 }, // 0x1a
	{ "RR E", 1, rr_e, 8 }, // 0x1b
	{ "RR H", 1, rr_h, 8 }, // 0x1c
	{ "RR L", 1, rr_l, 8 }, // 0x1d
	{ "RR (HL)", 1, rr_hlp, 16 }, // 0x1e
	{ "RR A", 1, rr_a, 8 }, // 0x1f
	{ "SLA B", 1, sla_b, 8 }, // 0x20
	{ "SLA C", 1, sla_c, 8 }, // 0x21
	{ "SLA D", 1, sla_d, 8 }, // 0x22
	{ "SLA E", 1, sla_e, 8 }, // 0x23
	{ "SLA H", 1, sla_h, 8 }, // 0x24
	{ "SLA L", 1, sla_l, 8 }, // 0x25
	{ "SLA (HL)", 1, sla_hlp, 16 }, // 0x26
	{ "SLA A", 1, sla_a, 8 }, // 0x27
	{ "SRA B", 1, sra_b, 8 }, // 0x28
	{ "SRA C", 1, sra_c, 8 }, // 0x29
	{ "SRA D", 1, sra_d, 8 }, // 0x2a
	{ "SRA E", 1, sra_e, 8 }, // 0x2b
	{ "SRA H", 1, sra_h, 8 }, // 0x2c
	{ "SRA L", 1, sra_l, 8 }, // 0x2d
	{ "SRA (HL)", 1, sra_hlp, 16 }, // 0x2e
	{ "SRA A", 1, sra_a, 8 }, // 0x2f
	{ "SWAP B", 1, swap_b, 8 }, // 0x30
	{ "SWAP C", 1, swap_c, 8 }, // 0x31
	{ "SWAP D", 1, swap_d, 8 }, // 0x32
	{ "SWAP E", 1, swap_e, 8 }, // 0x33
	{ "SWAP H", 1, swap_h, 8 }, // 0x34
	{ "SWAP L", 1, swap_l, 8 }, // 0x35
	{ "SWAP (HL)", 1, swap_hlp, 16 }, // 0x36
	{ "SWAP A", 1, swap_a, 8 }, // 0x37
	{ "SRL B", 1, srl_b, 8 }, // 0x38
	{ "SRL C", 1, srl_c, 8 }, // 0x39
	{ "SRL D", 1, srl_d, 8 }, // 0x3a
	{ "SRL E", 1, srl_e, 8 }, // 0x3b
	{ "SRL H", 1, srl_h, 8 }, // 0x3c
	{ "SRL L", 1, srl_l, 8 }, // 0x3d
	{ "SRL (HL)", 1, srl_hlp, 16 }, // 0x3e
	{ "SRL A", 1, srl_a, 8 }, // 0x3f
	{ "BIT 0,B", 1, bit_0_b, 8 }, // 0x40
	{ "BIT 0,C", 1, bit_0_c, 8 }, // 0x41
	{ "BIT 0,D", 1, bit_0_d, 8 }, // 0x42
	{ "BIT 0,E", 1, bit_0_e, 8 }, // 0x43
	{ "BIT 0,H", 1, bit_0_h, 8 }, // 0x44
	{ "BIT 0,L", 1, bit_0_l, 8 }, // 0x45
	{ "BIT 0,(HL)", 1, bit_0_hlp, 16 }, // 0x46
	{ "BIT 0,A", 1, bit_0_a, 8 }, // 0x47
	{ "BIT 1,B", 1, bit_1_b, 8 }, // 0x48
	{ "BIT 1,C", 1, bit_1_c, 8 }, // 0x49
	{ "BIT 1,D", 1, bit_1_d, 8 }, // 0x4a
	{ "BIT 1,E", 1, bit_1_e, 8 }, // 0x4b
	{ "BIT 1,H", 1, bit_1_h, 8 }, // 0x4c
	{ "BIT 1,L", 1, bit_1_l, 8 }, // 0x4d
	{ "BIT 1,(HL)", 1, bit_1_hlp, 16 }, // 0x4e
	{ "BIT 1,A", 1, bit_1_a, 8 }, // 0x4f
	{ "BIT 2,B", 1, bit_2_b, 8 }, // 0x50
	{ "BIT 2,C", 1, bit_2_c, 8 }, // 0x51
	{ "BIT 2,D", 1, bit_2_d, 8 }, // 0x52
	{ "BIT 2,E", 1, bit_2_e, 8 }, // 0x53
	{ "BIT 2,H", 1, bit_2_h, 8 }, // 0x54
	{ "BIT 2,L", 1, bit_2_l, 8 }, // 0x55
	{ "BIT 2,(HL)", 1, bit_2_hlp, 16 }, // 0x56
	{ "BIT 2,A", 1, bit_2_a, 8 }, // 0x57
	{ "BIT 3,B", 1, bit_3_b, 8 }, // 0x58
	{ "BIT 3,C", 1, bit_3_c, 8 }, // 0x59
	{ "BIT 3,D", 1, bit_3_d, 8 }, // 0x5a
	{ "BIT 3,E", 1, bit_3_e, 8 }, // 0x5b
	{ "BIT 3,H", 1, bit_3_h, 8 }, // 0x5c
	{ "BIT 3,L", 1, bit_3_l, 8 }, // 0x5d
	{ "BIT 3,(HL)", 1, bit_3_hlp, 16 }, // 0x5e
	{ "BIT 3,A", 1, bit_3_a, 8 }, // 0x5f
	{ "BIT 4,B", 1, bit_4_b, 8 }, // 0x60
	{ "BIT 4,C", 1, bit_4_c, 8 }, // 0x61
	{ "BIT 4,D", 1, bit_4_d, 8 }, // 0x62
	{ "BIT 4,E", 1, bit_4_e, 8 }, // 0x63
	{ "BIT 4,H", 1, bit_4_h, 8 }, // 0x64
	{ "BIT 4,L", 1, bit_4_l, 8 }, // 0x65
	{ "BIT 4,(HL)", 1, bit_4_hlp, 16 }, // 0x66
	{ "BIT 4,A", 1, bit_4_a, 8 }, // 0x67
	{ "BIT 5,B", 1, bit_5_b, 8 }, // 0x68
	{ "BIT 5,C", 1, bit_5_c, 8 }, // 0x69
	{ "BIT 5,D", 1, bit_5_d, 8 }, // 0x6a
	{ "BIT 5,E", 1, bit_5_e, 8 }, // 0x6b
	{ "BIT 5,H", 1, bit_5_h, 8 }, // 0x6c
	{ "BIT 5,L", 1, bit_5_l, 8 }, // 0x6d
	{ "BIT 5,(HL)", 1, bit_5_hlp, 16 }, // 0x6e
	{ "BIT 5,A", 1, bit_5_a, 8 }, // 0x6f
	{ "BIT 6,B", 1, bit_6_b, 8 }, // 0x70
	{ "BIT 6,C", 1, bit_6_c, 8 }, // 0x71
	{ "BIT 6,D", 1, bit_6_d, 8 }, // 0x72
	{ "BIT 6,E", 1, bit_6_e, 8 }, // 0x73
	{ "BIT 6,H", 1, bit_6_h, 8 }, // 0x74
	{ "BIT 6,L", 1, bit_6_l, 8 }, // 0x75
	{ "BIT 6,(HL)", 1, bit_6_hlp, 16 }, // 0x76
	{ "BIT 6,A", 1, bit_6_a, 8 }, // 0x77
	{ "BIT 7,B", 1, bit_7_b, 8 }, // 0x78
	{ "BIT 7,C", 1, bit_7_c, 8 }, // 0x79
	{ "BIT 7,D", 1, bit_7_d, 8 }, // 0x7a
	{ "BIT 7,E", 1, bit_7_e, 8 }, // 0x7b
	{ "BIT 7,H", 1, bit_7_h, 8 }, // 0x7c
	{ "BIT 7,L", 1, bit_7_l, 8 }, // 0x7d
	{ "BIT 7,(HL)", 1, bit_7_hlp, 16 }, // 0x7e
	{ "BIT 7,A", 1, bit_7_a, 8 }, // 0x7f
	{ "RES 0,B", 1, res_0_b, 8 }, // 0x80
	{ "RES 0,C", 1, res_0_c, 8 }, // 0x81
	{ "RES 0,D", 1, res_0_d, 8 }, // 0x82
	{ "RES 0,E", 1, res_0_e, 8 }, // 0x83
	{ "RES 0,H", 1, res_0_h, 8 }, // 0x84
	{ "RES 0,L", 1, res_0_l, 8 }, // 0x85
	{ "RES 0,(HL)", 1, res_0_hlp, 16 }, // 0x86
	{ "RES 0,A", 1, res_0_a, 8 }, // 0x87
	{ "RES 1,B", 1, res_1_b, 8 }, // 0x88
	{ "RES 1,C", 1, res_1_c, 8 }, // 0x89
	{ "RES 1,D", 1, res_1_d, 8 }, // 0x8a
	{ "RES 1,E", 1, res_1_e, 8 }, // 0x8b
	{ "RES 1,H", 1, res_1_h, 8 }, // 0x8c
	{ "RES 1,L", 1, res_1_l, 8 }, // 0x8d
	{ "RES 1,(HL)", 1, res_1_hlp, 16 }, // 0x8e
	{ "RES 1,A", 1, res_1_a, 8 }, // 0x8f
	{ "RES 2,B", 1, res_2_b, 8 }, // 0x90
	{ "RES 2,C", 1, res_2_c, 8 }, // 0x91
	{ "RES 2,D", 1, res_2_d, 8 }, // 0x92
	{ "RES 2,E", 1, res_2_e, 8 }, // 0x93
	{ "RES 2,H", 1, res_2_h, 8 }, // 0x94
	{ "RES 2,L", 1, res_2_l, 8 }, // 0x95
	{ "RES 2,(HL)", 1, res_2_hlp, 16 }, // 0x96
	{ "RES 2,A", 1, res_2_a, 8 }, // 0x97
	{ "RES 3,B", 1, res_3_b, 8 }, // 0x98
	{ "RES 3,C", 1, res_3_c, 8 }, // 0x99
	{ "RES 3,D", 1, res_3_d, 8 }, // 0x9a
	{ "RES 3,E", 1, res_3_e, 8 }, // 0x9b
	{ "RES 3,H", 1, res_3_h, 8 }, // 0x9c
	{ "RES 3,L", 1, res_3_l, 8 }, // 0x9d
	{ "RES 3,(HL)", 1, res_3_hlp, 16 }, // 0x9e
	{ "RES 3,A", 1, res_3_a, 8 }, // 0x9f
	{ "RES 4,B", 1, res_4_b, 8 }, // 0xa0
	{ "RES 4,C", 1, res_4_c, 8 }, // 0xa1
	{ "RES 4,D", 1, res_4_d, 8 }, // 0xa2
	{ "RES 4,E", 1, res_4_e, 8 }, // 0xa3
	{ "RES 4,H", 1, res_4_h, 8 }, // 0xa4
	{ "RES 4,L", 1, res_4_l, 8 }, // 0xa5
	{ "RES 4,(HL)", 1, res_4_hlp, 16 }, // 0xa6
	{ "RES 4,A", 1, res_4_a, 8 }, // 0xa7
	{ "RES 5,B", 1, res_5_b, 8 }, // 0xa8
	{ "RES 5,C", 1, res_5_c, 8 }, // 0xa9
	{ "RES 5,D", 1, res_5_d, 8 }, // 0xaa
	{ "RES 5,E", 1, res_5_e, 8 }, // 0xab
	{ "RES 5,H", 1, res_5_h, 8 }, // 0xac
	{ "RES 5,L", 1, res_5_l, 8 }, // 0xad
	{ "RES 5,(HL)", 1, res_5_hlp, 16 }, // 0xae
	{ "RES 5,A", 1, res_5_a, 8 }, // 0xaf
	{ "RES 6,B", 1, res_6_b, 8 }, // 0xb0
	{ "RES 6,C", 1, res_6_c, 8 }, // 0xb1
	{ "RES 6,D", 1, res_6_d, 8 }, // 0xb2
	{ "RES 6,E", 1, res_6_e, 8 }, // 0xb3
	{ "RES 6,H", 1, res_6_h, 8 }, // 0xb4
	{ "RES 6,L", 1, res_6_l, 8 }, // 0xb5
	{ "RES 6,(HL)", 1, res_6_hlp, 16 }, // 0xb6
	{ "RES 6,A", 1, res_6_a, 8 }, // 0xb7
	{ "RES 7,B", 1, res_7_b, 8 }, // 0xb8
	{ "RES 7,C", 1, res_7_c, 8 }, // 0xb9
	{ "RES 7,D", 1, res_7_d, 8 }, // 0xba
	{ "RES 7,E", 1, res_7_e, 8 }, // 0xbb
	{ "RES 7,H", 1, res_7_h, 8 }, // 0xbc
	{ "RES 7,L", 1, res_7_l, 8 }, // 0xbd
	{ "RES 7,(HL)", 1, res_7_hlp, 16 }, // 0xbe
	{ "RES 7,A", 1, res_7_a, 8 }, // 0xbf
	{ "SET O,B", 1, set_0_b, 8 }, // 0xc0
	{ "SET 0,C", 1, set_0_c, 8 }, // 0xc1
	{ "SET 0,D", 1, set_0_d, 8 }, // 0xc2
	{ "SET 0,E", 1, set_0_e, 8 }, // 0xc3
	{ "SET 0,H", 1, set_0_h, 8 }, // 0xc4
	{ "SET 0,L", 1, set_0_l, 8 }, // 0xc5
	{ "SET 0,(HL)", 1, set_0_hlp, 16 }, // 0xc6
	{ "SET 0,A", 1, set_0_a, 8 }, // 0xc7
	{ "SET 1,B", 1, set_1_b, 8 }, // 0xc8
	{ "SET 1,C", 1, set_1_c, 8 }, // 0xc9
	{ "SET 1,D", 1, set_1_d, 8 }, // 0xca
	{ "SET 1,E", 1, set_1_e, 8 }, // 0xcb
	{ "SET 1,H", 1, set_1_h, 8 }, // 0xcc
	{ "SET 1,L", 1, set_1_l, 8 }, // 0xcd
	{ "SET 1,(HL)", 1, set_1_hlp, 16 }, // 0xce
	{ "SET 1,A", 1, set_1_a, 8 }, // 0xcf
	{ "SET 2,B", 1, set_2_b, 8 }, // 0xd0
	{ "SET 2,C", 1, set_2_c, 8 }, // 0xd1
	{ "SET 2,D", 1, set_2_d, 8 }, // 0xd2
	{ "SET 2,E", 1, set_2_e, 8 }, // 0xd3
	{ "SET 2,H", 1, set_2_h, 8 }, // 0xd4
	{ "SET 2,L", 1, set_2_l, 8 }, // 0xd5
	{ "SET 2,(HL)", 1, set_2_hlp, 16 }, // 0xd6
	{ "SET 2,A", 1, set_2_a, 8 }, // 0xd7
	{ "SET 3,B", 1, set_3_b, 8 }, // 0xd8
	{ "SET 3,C", 1, set_3_c, 8 }, // 0xd9
	{ "SET 3,D", 1, set_3_d, 8 }, // 0xda
	{ "SET 3,E", 1, set_3_e, 8 }, // 0xdb
	{ "SET 3,H", 1, set_3_h, 8 }, // 0xdc
	{ "SET 3,L", 1, set_3_l, 8 }, // 0xdd
	{ "SET 3,(HL)", 1, set_3_hlp, 16 }, // 0xde
	{ "SET 3,A", 1, set_3_a, 8 }, // 0xdf
	{ "SET 4,B", 1, set_4_b, 8 }, // 0xe0
	{ "SET 4,C", 1, set_4_c, 8 }, // 0xe1
	{ "SET 4,D", 1, set_4_d, 8 }, // 0xe2
	{ "SET 4,E", 1, set_4_e, 8 }, // 0xe3
	{ "SET 4,H", 1, set_4_h, 8 }, // 0xe4
	{ "SET 4,L", 1, set_4_l, 8 }, // 0xe5
	{ "SET 4,(HL)", 1, set_4_hlp, 16 }, // 0xe6
	{ "SET 4,A", 1, set_4_a, 8 }, // 0xe7
	{ "SET 5,B", 1, set_5_b, 8 }, // 0xe8
	{ "SET 5,C", 1, set_5_c, 8 }, // 0xe9
	{ "SET 5,D", 1, set_5_d, 8 }, // 0xea
	{ "SET 5,E", 1, set_5_e, 8 }, // 0xeb
	{ "SET 5,H", 1, set_5_h, 8 }, // 0xec
	{ "SET 5,L", 1, set_5_l, 8 }, // 0xed
	{ "SET 5,(HL)", 1, set_5_hlp, 16 }, // 0xee
	{ "SET 5,A", 1, set_5_a, 8 }, // 0xef
	{ "SET 6,B", 1, set_6_b, 8 }, // 0xf0
	{ "SET 6,C", 1, set_6_c, 8 }, // 0xf1
	{ "SET 6,D", 1, set_6_d, 8 }, // 0xf2
	{ "SET 6,E", 1, set_6_e, 8 }, // 0xf3
	{ "SET 6,H", 1, set_6_h, 8 }, // 0xf4
	{ "SET 6,L", 1, set_6_l, 8 }, // 0xf5
	{ "SET 6,(HL)", 1, set_6_hlp, 16 }, // 0xf6
	{ "SET 6,A", 1, set_6_a, 8 }, // 0xf7
	{ "SET 7,B", 1, set_7_b, 8 }, // 0xf8
	{ "SET 7,C", 1, set_7_c, 8 }, // 0xf9
	{ "SET 7,D", 1, set_7_d, 8 }, // 0xfa
	{ "SET 7,E", 1, set_7_e, 8 }, // 0xfb
	{ "SET 7,H", 1, set_7_h, 8 }, // 0xfc
	{ "SET 7,L", 1, set_7_l, 8 }, // 0xfd
	{ "SET 7,(HL)", 1, set_7_hlp, 16 }, // 0xfe
	{ "SET 7,A", 1, set_7_a, 8 }, // 0xff
};