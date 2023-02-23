#include "cpu_cb.h"
#include "memory.h"

void cb_n(uint8_t cb_opcode)
{
	//((void(*)(void))cb_instructions[cb_opcode].function)();
}

uint8_t rlc(uint8_t value){}
uint8_t rrc(uint8_t value){}
uint8_t rl(uint8_t value){}
uint8_t rr(uint8_t value){}
uint8_t sla(uint8_t value){}
uint8_t sra(uint8_t value){}
uint8_t swap(uint8_t value){}
uint8_t srl(uint8_t value){}
uint8_t bit(uint8_t bit, uint8_t value){}

void rlc_b(){}
void rlc_c(){}
void rlc_d(){}
void rlc_e(){}
void rlc_h(){}
void rlc_l(){}
void rlc_hlp(){}
void rlc_a(){}
void rrc_b(){}
void rrc_c(){}
void rrc_d(){}
void rrc_e(){}
void rrc_h(){}
void rrc_l(){}
void rrc_hlp(){}
void rrc_a(){}

void rl_b(){}
void rl_c(){}
void rl_d(){}
void rl_e(){}
void rl_h(){}
void rl_l(){}
void rl_hlp(){}
void rl_a(){}
void rr_b(){}
void rr_c(){}
void rr_d(){}
void rr_e(){}
void rr_h(){}
void rr_l(){}
void rr_hlp(){}
void rr_a(){}

void sla_b(){}
void sla_c(){}
void sla_d(){}
void sla_e(){}
void sla_h(){}
void sla_l(){}
void sla_hlp(){}
void sla_a(){}
void sra_b(){}
void sra_c(){}
void sra_d(){}
void sra_e(){}
void sra_h(){}
void sra_l(){}
void sra_hlp(){}
void sra_a(){}

void swap_b(){}
void swap_c(){}
void swap_d(){}
void swap_e(){}
void swap_h(){}
void swap_l(){}
void swap_hlp(){}
void swap_a(){}
void srl_b(){}
void srl_c(){}
void srl_d(){}
void srl_e(){}
void srl_h(){}
void srl_l(){}
void srl_hlp(){}
void srl_a(){}

void bit_0_b(){}
void bit_0_c(){}
void bit_0_d(){}
void bit_0_e(){}
void bit_0_h(){}
void bit_0_l(){}
void bit_0_hlp(){}
void bit_0_a(){}
void bit_1_b(){}
void bit_1_c(){}
void bit_1_d(){}
void bit_1_e(){}
void bit_1_h(){}
void bit_1_l(){}
void bit_1_hlp(){}
void bit_1_a(){}

void bit_2_b(){}
void bit_2_c(){}
void bit_2_d(){}
void bit_2_e(){}
void bit_2_h(){}
void bit_2_l(){}
void bit_2_hlp(){}
void bit_2_a(){}
void bit_3_b(){}
void bit_3_c(){}
void bit_3_d(){}
void bit_3_e(){}
void bit_3_h(){}
void bit_3_l(){}
void bit_3_hlp(){}
void bit_3_a(){}

void bit_4_b(){}
void bit_4_c(){}
void bit_4_d(){}
void bit_4_e(){}
void bit_4_h(){}
void bit_4_l(){}
void bit_4_hlp(){}
void bit_4_a(){}
void bit_5_b(){}
void bit_5_c(){}
void bit_5_d(){}
void bit_5_e(){}
void bit_5_h(){}
void bit_5_l(){}
void bit_5_hlp(){}
void bit_5_a(){}

void bit_6_b(){}
void bit_6_c(){}
void bit_6_d(){}
void bit_6_e(){}
void bit_6_h(){}
void bit_6_l(){}
void bit_6_hlp(){}
void bit_6_a(){}
void bit_7_b(){}
void bit_7_c(){}
void bit_7_d(){}
void bit_7_e(){}
void bit_7_h(){}
void bit_7_l(){}
void bit_7_hlp(){}
void bit_7_a(){}

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

extern const instruction cb_instructions[256] = {
	{ "RLC B", 1, }, // 0x0
	{ "RLC C", 1, }, // 0x1
	{ "RLC D", 1, }, // 0x2
	{ "RLC E", 1, }, // 0x3
	{ "RLC H", 1, }, // 0x4
	{ "RLC L", 1, }, // 0x5
	{ "RLC (HL)", 1, }, // 0x6
	{ "RLC A", 1, }, // 0x7
	{ "RRC A", 1, }, // 0x8
	{ "RRC B", 1, }, // 0x9
	{ "RRC C", 1, }, // 0xa
	{ "RRC D", 1, }, // 0xb
	{ "RRC E", 1, }, // 0xc
	{ "RRC H", 1, }, // 0xd
	{ "RRC (HL)", 1, }, // 0xe
	{ "RRC A", 1, }, // 0xf
	{ "RL B", 1, }, // 0x10
	{ "RL C", 1, }, // 0x11
	{ "RL D", 1, }, // 0x12
	{ "RL E", 1, }, // 0x13
	{ "RL H", 1, }, // 0x14
	{ "RL L", 1, }, // 0x15
	{ "RL (HL)", 1, }, // 0x16
	{ "RL A", 1, }, // 0x17
	{ "RR B", 1, }, // 0x18
	{ "RR C", 1, }, // 0x19
	{ "RR D", 1, }, // 0x1a
	{ "RR E", 1, }, // 0x1b
	{ "RR H", 1, }, // 0x1c
	{ "RR L", 1, }, // 0x1d
	{ "RR (HL)", 1, }, // 0x1e
	{ "RR A", 1, }, // 0x1f
	{ "SLA B", 1, }, // 0x20
	{ "SLA C", 1, }, // 0x21
	{ "SLA D", 1, }, // 0x22
	{ "SLA E", 1, }, // 0x23
	{ "SLA H", 1, }, // 0x24
	{ "SLA L", 1, }, // 0x25
	{ "SLA (HL)", 1, }, // 0x26
	{ "SLA A", 1, }, // 0x27
	{ "SRA B", 1, }, // 0x28
	{ "SRA C", 1, }, // 0x29
	{ "SRA D", 1, }, // 0x2a
	{ "SRA E", 1, }, // 0x2b
	{ "SRA H", 1, }, // 0x2c
	{ "SRA L", 1, }, // 0x2d
	{ "SRA (HL)", 1, }, // 0x2e
	{ "SRA A", 1, }, // 0x2f
	{ "SWAP B", 1, }, // 0x30
	{ "SWAP C", 1, }, // 0x31
	{ "SWAP D", 1, }, // 0x32
	{ "SWAP E", 1, }, // 0x33
	{ "SWAP H", 1, }, // 0x34
	{ "SWAP L", 1, }, // 0x35
	{ "SWAP (HL)", 1, }, // 0x36
	{ "SWAP A", 1, }, // 0x37
	{ "SRL B", 1, }, // 0x38
	{ "SRL C", 1, }, // 0x39
	{ "SRL D", 1, }, // 0x3a
	{ "SRL E", 1, }, // 0x3b
	{ "SRL H", 1, }, // 0x3c
	{ "SRL L", 1, }, // 0x3d
	{ "SRL (HL)", 1, }, // 0x3e
	{ "SRL A", 1, }, // 0x3f
	{ "BIT 0,B", 1, }, // 0x40
	{ "BIT 0,C", 1, }, // 0x41
	{ "BIT 0,D", 1, }, // 0x42
	{ "BIT 0,E", 1, }, // 0x43
	{ "BIT 0,H", 1, }, // 0x44
	{ "BIT 0,L", 1, }, // 0x45
	{ "BIT 0,(HL)", 1, }, // 0x46
	{ "BIT 1,A", 1, }, // 0x47
	{ "BIT 1,B", 1, }, // 0x48
	{ "BIT 1,C", 1, }, // 0x49
	{ "BIT 1,D", 1, }, // 0x4a
	{ "BIT 1,E", 1, }, // 0x4b
	{ "BIT 1,H", 1, }, // 0x4c
	{ "BIT 1,L", 1, }, // 0x4d
	{ "BIT 1,(HL)", 1, }, // 0x4e
	{ "BIT 1,A", 1, }, // 0x4f
	{ "BIT 2,B", 1, }, // 0x50
	{ "BIT 2,C", 1, }, // 0x51
	{ "BIT 2,D", 1, }, // 0x52
	{ "BIT 2,E", 1, }, // 0x53
	{ "BIT 2,H", 1, }, // 0x54
	{ "BIT 2,L", 1, }, // 0x55
	{ "BIT 2,(HL)", 1, }, // 0x56
	{ "BIT 2,A", 1, }, // 0x57
	{ "BIT 3,B", 1, }, // 0x58
	{ "BIT 3,C", 1, }, // 0x59
	{ "BIT 3,D", 1, }, // 0x5a
	{ "BIT 3,E", 1, }, // 0x5b
	{ "BIT 3,H", 1, }, // 0x5c
	{ "BIT 3,L", 1, }, // 0x5d
	{ "BIT 3,(HL)", 1, }, // 0x5e
	{ "BIT 3,A", 1, }, // 0x5f
	{ "BIT 4,B", 1, }, // 0x60
	{ "BIT 4,C", 1, }, // 0x61
	{ "BIT 4,D", 1, }, // 0x62
	{ "BIT 4,E", 1, }, // 0x63
	{ "BIT 4,H", 1, }, // 0x64
	{ "BIT 4,L", 1, }, // 0x65
	{ "BIT 4,(HL)", 1, }, // 0x66
	{ "BIT 4,A", 1, }, // 0x67
	{ "BIT 5,B", 1, }, // 0x68
	{ "BIT 5,C", 1, }, // 0x69
	{ "BIT 5,D", 1, }, // 0x6a
	{ "BIT 5,E", 1, }, // 0x6b
	{ "BIT 5,H", 1, }, // 0x6c
	{ "BIT 5,L", 1, }, // 0x6d
	{ "BIT 5,(HL)", 1, }, // 0x6e
	{ "BIT 5,A", 1, }, // 0x6f
	{ "BIT 6,B", 1, }, // 0x70
	{ "BIT 6,C", 1, }, // 0x71
	{ "BIT 6,D", 1, }, // 0x72
	{ "BIT 6,E", 1, }, // 0x73
	{ "BIT 6,H", 1, }, // 0x74
	{ "BIT 6,L", 1, }, // 0x75
	{ "BIT 6,(HL)", 1, }, // 0x76
	{ "BIT 6,A", 1, }, // 0x77
	{ "BIT 7,B", 1, }, // 0x78
	{ "BIT 7,C", 1, }, // 0x79
	{ "BIT 7,D", 1, }, // 0x7a
	{ "BIT 7,E", 1, }, // 0x7b
	{ "BIT 7,H", 1, }, // 0x7c
	{ "BIT 7,L", 1, }, // 0x7d
	{ "BIT 7,(HL)", 1, }, // 0x7e
	{ "BIT 7,A", 1, }, // 0x7f
	{ "RES 0,B", 1, }, // 0x80
	{ "RES 0,C", 1, }, // 0x81
	{ "RES 0,D", 1, }, // 0x82
	{ "RES 0,E", 1, }, // 0x83
	{ "RES 0,H", 1, }, // 0x84
	{ "RES 0,L", 1, }, // 0x85
	{ "RES 0,(HL)", 1, }, // 0x86
	{ "RES 0,A", 1, }, // 0x87
	{ "RES 1,B", 1, }, // 0x88
	{ "RES 1,C", 1, }, // 0x89
	{ "RES 1,D", 1, }, // 0x8a
	{ "RES 1,E", 1, }, // 0x8b
	{ "RES 1,H", 1, }, // 0x8c
	{ "RES 1,L", 1, }, // 0x8d
	{ "RES 1,(HL)", 1, }, // 0x8e
	{ "RES 1,A", 1, }, // 0x8f
	{ "RES 2,B", 1, }, // 0x90
	{ "RES 2,C", 1, }, // 0x91
	{ "RES 2,D", 1, }, // 0x92
	{ "RES 2,E", 1, }, // 0x93
	{ "RES 2,H", 1, }, // 0x94
	{ "RES 2,L", 1, }, // 0x95
	{ "RES 2,(HL)", 1, }, // 0x96
	{ "RES 2,A", 1, }, // 0x97
	{ "RES 3,B", 1, }, // 0x98
	{ "RES 3,C", 1, }, // 0x99
	{ "RES 3,D", 1, }, // 0x9a
	{ "RES 3,E", 1, }, // 0x9b
	{ "RES 3,H", 1, }, // 0x9c
	{ "RES 3,L", 1, }, // 0x9d
	{ "RES 3,(HL)", 1, }, // 0x9e
	{ "RES 3,A", 1, }, // 0x9f
	{ "RES 4,B", 1, }, // 0xa0
	{ "RES 4,C", 1, }, // 0xa1
	{ "RES 4,D", 1, }, // 0xa2
	{ "RES 4,E", 1, }, // 0xa3
	{ "RES 4,H", 1, }, // 0xa4
	{ "RES 4,L", 1, }, // 0xa5
	{ "RES 4,(HL)", 1, }, // 0xa6
	{ "RES 4,A", 1, }, // 0xa7
	{ "RES 5,B", 1, }, // 0xa8
	{ "RES 5,C", 1, }, // 0xa9
	{ "RES 5,D", 1, }, // 0xaa
	{ "RES 5,E", 1, }, // 0xab
	{ "RES 5,H", 1, }, // 0xac
	{ "RES 5,L", 1, }, // 0xad
	{ "RES 5,(HL)", 1, }, // 0xae
	{ "RES 5,A", 1, }, // 0xaf
	{ "RES 6,B", 1, }, // 0xb0
	{ "RES 6,C", 1, }, // 0xb1
	{ "RES 6,D", 1, }, // 0xb2
	{ "RES 6,E", 1, }, // 0xb3
	{ "RES 6,H", 1, }, // 0xb4
	{ "RES 6,L", 1, }, // 0xb5
	{ "RES 6,(HL)", 1, }, // 0xb6
	{ "RES 6,A", 1, }, // 0xb7
	{ "RES 7,B", 1, }, // 0xb8
	{ "RES 7,C", 1, }, // 0xb9
	{ "RES 7,D", 1, }, // 0xba
	{ "RES 7,E", 1, }, // 0xbb
	{ "RES 7,H", 1, }, // 0xbc
	{ "RES 7,L", 1, }, // 0xbd
	{ "RES 7,(HL)", 1, }, // 0xbe
	{ "RES 7,A", 1, }, // 0xbf
	{ "SET O,B", 1, }, // 0xc0
	{ "SET 0,C", 1, }, // 0xc1
	{ "SET 0,D", 1, }, // 0xc2
	{ "SET 0,E", 1, }, // 0xc3
	{ "SET 0,H", 1, }, // 0xc4
	{ "SET 0,L", 1, }, // 0xc5
	{ "SET 0,(HL)", 1, }, // 0xc6
	{ "SET 0,A", 1, }, // 0xc7
	{ "SET 1,B", 1, }, // 0xc8
	{ "SET 1,C", 1, }, // 0xc9
	{ "SET 1,D", 1, }, // 0xca
	{ "SET 1,E", 1, }, // 0xcb
	{ "SET 1,H", 1, }, // 0xcc
	{ "SET 1,L", 1, }, // 0xcd
	{ "SET 1,(HL)", 1, }, // 0xce
	{ "SET 1,A", 1, }, // 0xcf
	{ "SET 2,B", 1, }, // 0xd0
	{ "SET 2,C", 1, }, // 0xd1
	{ "SET 2,D", 1, }, // 0xd2
	{ "SET 2,E", 1, }, // 0xd3
	{ "SET 2,H", 1, }, // 0xd4
	{ "SET 2,L", 1, }, // 0xd5
	{ "SET 2,(HL)", 1, }, // 0xd6
	{ "SET 2,A", 1, }, // 0xd7
	{ "SET 3,B", 1, }, // 0xd8
	{ "SET 3,C", 1, }, // 0xd9
	{ "SET 3,D", 1, }, // 0xda
	{ "SET 3,E", 1, }, // 0xdb
	{ "SET 3,H", 1, }, // 0xdc
	{ "SET 3,L", 1, }, // 0xdd
	{ "SET 3,(HL)", 1, }, // 0xde
	{ "SET 3,A", 1, }, // 0xdf
	{ "SET 4,B", 1, }, // 0xe0
	{ "SET 4,C", 1, }, // 0xe1
	{ "SET 4,D", 1, }, // 0xe2
	{ "SET 4,E", 1, }, // 0xe3
	{ "SET 4,H", 1, }, // 0xe4
	{ "SET 4,L", 1, }, // 0xe5
	{ "SET 4,(HL)", 1, }, // 0xe6
	{ "SET 4,A", 1, }, // 0xe7
	{ "SET 5,B", 1, }, // 0xe8
	{ "SET 5,C", 1, }, // 0xe9
	{ "SET 5,D", 1, }, // 0xea
	{ "SET 5,E", 1, }, // 0xeb
	{ "SET 5,H", 1, }, // 0xec
	{ "SET 5,L", 1, }, // 0xed
	{ "SET 5,(HL)", 1, }, // 0xee
	{ "SET 5,A", 1, }, // 0xef
	{ "SET 6,B", 1, }, // 0xf0
	{ "SET 6,C", 1, }, // 0xf1
	{ "SET 6,D", 1, }, // 0xf2
	{ "SET 6,E", 1, }, // 0xf3
	{ "SET 6,H", 1, }, // 0xf4
	{ "SET 6,L", 1, }, // 0xf5
	{ "SET 6,(HL)", 1, }, // 0xf6
	{ "SET 6,A", 1, }, // 0xf7
	{ "SET 7,B", 1, }, // 0xf8
	{ "SET 7,C", 1, }, // 0xf9
	{ "SET 7,D", 1, }, // 0xfa
	{ "SET 7,E", 1, }, // 0xfb
	{ "SET 7,H", 1, }, // 0xfc
	{ "SET 7,L", 1, }, // 0xfd
	{ "SET 7,(HL)", 1, }, // 0xfe
	{ "SET 7,A", 1, }, // 0xff
};