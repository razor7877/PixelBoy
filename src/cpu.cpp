#include <iostream>
#include <cstdint>

#include "memory.h"
#include "cpu.h"

#include <chrono>
#include <thread>


// Make sure to zero initialize all variables
uint16_t AF{};
uint16_t BC{};
uint16_t DE{};
uint16_t HL{};

uint16_t sp{}; // Stack pointer
uint16_t pc = 0x100; // Program counter
uint8_t opcode{};

void handle_instruction()
{
	opcode = memory[pc++];

	pc += instructions[opcode].operand_length;
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void undefined(){}
void nop(){} // 0x00
void ld_bc_nn(uint16_t operand){} // 0x01
void ld_bcp_a(){} // 0x02
void inc_bc(){} // 0x03
void inc_b(){} // 0x04
void dec_b(){} // 0x05
void ld_b_n(uint8_t operand){} // 0x06
void rlca(){} // 0x07
void ld_nnp_sp(uint16_t operand){} // 0x08
void add_hl_bc(){} // 0x09
void ld_a_bcp(){} // 0x0A
void dec_bc(){} // 0x0B
void inc_c(){} // 0x0C
void dec_c(){} // 0x0D
void ld_c_n(uint8_t operand){} // 0x0E
void rrca(){} // 0x0F

void stop(uint8_t operand){} // 0x10
void ld_de_nn(uint16_t operand){} // 0x11
void ld_dep_a(){} // 0x12
void inc_de(){} // 0x13
void inc_d(){} // 0x14
void dec_d(){} // 0x15
void ld_d_n(uint8_t operand){} // 0x16
void rla(){} // 0x17
void jr_n(uint8_t operand){} // 0x18
void add_hl_de(){} // 0x19
void ld_a_dep(){} // 0x1A
void dec_de(){} // 0x1B
void inc_e(){} // 0x1C
void dec_e(){} // 0x1D
void ld_e_n(uint8_t operand){} // 0x1E
void rra(){} // 0x1F

void jr_nz_n(uint8_t operand){} // 0x20
void ld_hl_nn(uint16_t operand){} // 0x21
void ldi_hlp_a(){} // 0x22
void inc_hl(){} // 0x23
void inc_h(){} // 0x24
void dec_h(){} // 0x25
void ld_h_n(uint8_t operand){} // 0x26
void daa(){} // 0x27
void jr_z_n(uint8_t operand){} // 0x28
void add_hl_hl(){} // 0x29
void ldi_a_hlp(){} // 0x2A
void dec_hl(){} // 0x2B
void inc_l(){} // 0x2C
void dec_l(){} // 0x2D
void ld_l_n(uint8_t operand){} // 0x2E
void cpl(){} // 0x2F

void jr_nc_n(uint8_t operand){} // 0x30
void ld_sp_nn(uint16_t operand){} // 0x31
void ldd_hlp_a(){} // 0x32
void inc_sp(){} // 0x33
void inc_hlp(){} // 0x34
void dec_hlp(){} // 0x35
void ld_hlp_n(uint8_t operand){} // 0x36
void scf(){} // 0x37
void jr_c_n(uint8_t operand){} // 0x38
void add_hl_sp(){} // 0x39
void ldd_a_hlp(){} // 0x3A
void dec_sp(){} // 0x3B
void inc_a(){} // 0x3C
void dec_a(){} // 0x3D
void ld_a_n(uint8_t operand){} // 0x3E
void ccf(){} // 0x3F

void ld_b_b(){}
void ld_b_c(){}
void ld_b_d(){}
void ld_b_e(){}
void ld_b_h(){}
void ld_b_l(){}
void ld_b_hlp(){}
void ld_b_a(){}
void ld_c_b(){}
void ld_c_c(){}
void ld_c_d(){}
void ld_c_e(){}
void ld_c_h(){}
void ld_c_l(){}
void ld_c_hlp(){}
void ld_c_a(){}

void ld_d_b(){}
void ld_d_c(){}
void ld_d_d(){}
void ld_d_e(){}
void ld_d_h(){}
void ld_d_l(){}
void ld_d_hlp(){}
void ld_d_a(){}
void ld_e_b(){}
void ld_e_c(){}
void ld_e_d(){}
void ld_e_e(){}
void ld_e_h(){}
void ld_e_l(){}
void ld_e_hlp(){}
void ld_e_a(){}

void ld_h_b(){}
void ld_h_c(){}
void ld_h_d(){}
void ld_h_e(){}
void ld_h_h(){}
void ld_h_l(){}
void ld_h_hlp(){}
void ld_h_a(){}
void ld_l_b(){}
void ld_l_c(){}
void ld_l_d(){}
void ld_l_e(){}
void ld_l_h(){}
void ld_l_l(){}
void ld_l_hlp(){}
void ld_l_a(){}

void ld_hlp_b(){}
void ld_hlp_c(){}
void ld_hlp_d(){}
void ld_hlp_e(){}
void ld_hlp_h(){}
void ld_hlp_l(){}
void halt(){}
void ld_hlp_a(){}
void ld_a_b(){}
void ld_a_c(){}
void ld_a_d(){}
void ld_a_e(){}
void ld_a_h(){}
void ld_a_l(){}
void ld_a_hlp(){}
void ld_a_a(){}

void add_a_b(){}
void add_a_c(){}
void add_a_d(){}
void add_a_e(){}
void add_a_h(){}
void add_a_l(){}
void add_a_hlp(){}
void add_a_a(){}
void adc_a_b(){}
void adc_a_c(){}
void adc_a_d(){}
void adc_a_e(){}
void adc_a_h(){}
void adc_a_l(){}
void adc_a_hlp(){}
void adc_a_a(){}

void sub_b(){}
void sub_c(){}
void sub_d(){}
void sub_e(){}
void sub_h(){}
void sub_l(){}
void sub_hlp(){}
void sub_a(){}
void sbc_a_b(){}
void sbc_a_c(){}
void sbc_a_d(){}
void sbc_a_e(){}
void sbc_a_h(){}
void sbc_a_l(){}
void sbc_a_hlp(){}
void sbc_a_a(){}

void and_b(){}
void and_c(){}
void and_d(){}
void and_e(){}
void and_h(){}
void and_l(){}
void and_hlp(){}
void and_a(){}
void xor_b(){}
void xor_c(){}
void xor_d(){}
void xor_e(){}
void xor_h(){}
void xor_l(){}
void xor_hlp(){}
void xor_a(){}

void or_b(){}
void or_c(){}
void or_d(){}
void or_e(){}
void or_h(){}
void or_l(){}
void or_hlp(){}
void or_a(){}
void cp_b(){}
void cp_c(){}
void cp_d(){}
void cp_e(){}
void cp_h(){}
void cp_l(){}
void cp_hlp(){}
void cp_a(){}

void ret_nz(){}
void pop_bc(){}
void jp_nz_nn(uint16_t operand){}
void jp_nn(uint16_t operand){}
void call_nz_nn(uint16_t operand){}
void push_bc(){}
void add_a_n(uint8_t operand){}
void rst_00(){}
void ret_z(){}
void ret(){}
void jp_z_nn(uint16_t operand){}
void cb(){}
void call_z_nn(uint16_t operand){}
void call_nn(uint16_t operand){}
void adc_a_n(uint8_t operand){}
void rst_08(){}

void ret_nc(){}
void pop_de(){}
void jp_nc_nn(uint16_t operand){}
// Placeholder - No opcode
void call_nc_nn(uint16_t operand){}
void push_de(){}
void sub_n(uint8_t operand){}
void rst_10(){}
void ret_c(){}
void reti(){}
void jp_c_nn(uint16_t operand){}
// Placeholder - No opcode
void call_c_nn(uint16_t operand){}
// Placeholder - No opcode
void sbc_a_n(uint8_t operand){}
void rst_18(){}

void ld_ff_n_ap(uint8_t operand){}
void pop_hl(){}
void ld_ff_c_a(){}
// Placeholder - No opcode
// Placeholder - No opcode
void push_hl(){}
void and_n(uint8_t operand){}
void rst_20(){}
void add_sp_n(uint8_t operand){}
void jp_hlp(){}
void ld_nn_a(uint16_t operand){}
// Placeholder - No opcode
// Placeholder - No opcode
// Placeholder - No opcode
void xor_n(uint8_t operand){}
void rst_28(){}

void ld_ff_ap_n(uint8_t operand){}
void pop_af(){}
void ld_ff_a_c(){}
void di(){}
// Placeholder - No opcode
void push_af(){}
void or_n(uint8_t operand){}
void rst_30(){}
void ld_hl_sp_n(uint8_t operand){}
void ld_sp_hl(){}
void ld_a_nnp(uint16_t operand){}
void ei(){}
// Placeholder - No opcode
// Placeholder - No opcode
void cp_n(uint8_t operand){}
void rst_38(){}

extern const instruction instructions[256] = {
	{ "NOP", 0, nop }, // 0x00
	{ "LD BC,d16", 2, ld_bc_nn }, // 0x1
	{ "LD (BC),A", 0, ld_bcp_a }, // 0x2
	{ "INC BC", 0, inc_bc }, // 0x3
	{ "INC B", 0, inc_b }, // 0x4
	{ "DEC B", 0, dec_b }, // 0x5
	{ "LD B,d8", 1, ld_b_n }, // 0x6
	{ "RLCA", 0, rlca }, // 0x7
	{ "LD (a16),SP", 2, ld_nnp_sp }, // 0x8
	{ "ADD HL,BC", 0, add_hl_bc }, // 0x9
	{ "LD A,(BC)", 0, ld_a_bcp }, // 0xa
	{ "DEC BC", 0, dec_bc }, // 0xb
	{ "INC C", 0, inc_c }, // 0xc
	{ "DEC C", 0, dec_c }, // 0xd
	{ "LD C,d8", 1, ld_c_n }, // 0xe
	{ "RRCA", 0, rrca }, // 0xf
	{ "STOP 0", 1, stop }, // 0x10
	{ "LD DE,d16", 2, ld_de_nn }, // 0x11
	{ "LD (DE),A", 0, ld_dep_a }, // 0x12
	{ "INC DE", 0, inc_de }, // 0x13
	{ "INC D", 0, inc_d }, // 0x14
	{ "DEC D", 0, dec_d }, // 0x15
	{ "LD D,d8", 1, ld_d_n }, // 0x16
	{ "RLA", 0, rla }, // 0x17
	{ "JR r8", 1, jr_n }, // 0x18
	{ "ADD HL,DE", 0, add_hl_de }, // 0x19
	{ "LD A,(DE)", 0, ld_a_dep }, // 0x1a
	{ "DEC DE", 0, dec_de }, // 0x1b
	{ "INC E", 0, inc_e }, // 0x1c
	{ "DEC E", 0, dec_e }, // 0x1d
	{ "LD E,d8", 1, ld_e_n }, // 0x1e
	{ "RRA", 0, rra }, // 0x1f
	{ "JR NZ,r8", 1, jr_nz_n }, // 0x20
	{ "LD HL,d16", 2, ld_hl_nn }, // 0x21
	{ "LD (HL+),A", 0, ldi_hlp_a }, // 0x22
	{ "INC HL", 0, inc_hl }, // 0x23
	{ "INC H", 0, inc_h }, // 0x24
	{ "DEC H", 0, dec_h }, // 0x25
	{ "LD H,d8", 1, ld_h_n }, // 0x26
	{ "DAA", 0, daa }, // 0x27
	{ "JR Z,r8", 1, jr_z_n }, // 0x28
	{ "ADD HL,HL", 0, add_hl_hl }, // 0x29
	{ "LD A,(HL+)", 0, ldi_a_hlp }, // 0x2a
	{ "DEC HL", 0, dec_hl }, // 0x2b
	{ "INC L", 0, inc_l }, // 0x2c
	{ "DEC L", 0, dec_l }, // 0x2d
	{ "LD L,d8", 1, ld_l_n }, // 0x2e
	{ "CPL", 0, cpl }, // 0x2f
	{ "JR NC,r8", 1, jr_nc_n }, // 0x30
	{ "LD SP,d16", 2, ld_sp_nn }, // 0x31
	{ "LD (HL-),A", 0, ldd_hlp_a }, // 0x32
	{ "INC SP", 0, inc_sp }, // 0x33
	{ "INC (HL)", 0, inc_hlp }, // 0x34
	{ "DEC (HL)", 0, dec_hlp }, // 0x35
	{ "LD (HL),d8", 1, ld_hlp_n }, // 0x36
	{ "SCF", 0, scf }, // 0x37
	{ "JR C,r8", 1, jr_c_n }, // 0x38
	{ "ADD HL,SP", 0, add_hl_sp }, // 0x39
	{ "LD A,(HL-)", 0, ldd_a_hlp }, // 0x3a
	{ "DEC SP", 0, dec_sp }, // 0x3b
	{ "INC A", 0, inc_a }, // 0x3c
	{ "DEC A", 0, dec_a }, // 0x3d
	{ "LD A,d8", 1, ld_a_n }, // 0x3e
	{ "CCF", 0, ccf }, // 0x3f
	{ "LD B,B", 0, ld_b_b }, // 0x40
	{ "LD B,C", 0, ld_b_c }, // 0x41
	{ "LD B,D", 0, ld_b_d }, // 0x42
	{ "LD B,E", 0, ld_b_e }, // 0x43
	{ "LD B,H", 0, ld_b_h }, // 0x44
	{ "LD B,L", 0, ld_b_l }, // 0x45
	{ "LD B,(HL)", 0, ld_b_hlp }, // 0x46
	{ "LD B,A", 0, ld_b_a }, // 0x47
	{ "LD C,B", 0, ld_c_b }, // 0x48
	{ "LD C,C", 0, ld_c_c }, // 0x49
	{ "LD C,D", 0, ld_c_d }, // 0x4a
	{ "LD C,E", 0, ld_c_e }, // 0x4b
	{ "LD C,H", 0, ld_c_h }, // 0x4c
	{ "LD C,L", 0, ld_c_l }, // 0x4d
	{ "LD C,(HL)", 0, ld_c_hlp }, // 0x4e
	{ "LD C,A", 0, ld_c_a }, // 0x4f
	{ "LD D,B", 0, ld_d_b }, // 0x50
	{ "LD D,C", 0, ld_d_c }, // 0x51
	{ "LD D,D", 0, ld_d_d }, // 0x52
	{ "LD D,E", 0, ld_d_e }, // 0x53
	{ "LD D,H", 0, ld_d_h }, // 0x54
	{ "LD D,L", 0, ld_d_l }, // 0x55
	{ "LD D,(HL)", 0, ld_d_hlp }, // 0x56
	{ "LD D,A", 0, ld_d_a }, // 0x57
	{ "LD E,B", 0, ld_e_b }, // 0x58
	{ "LD E,C", 0, ld_e_c }, // 0x59
	{ "LD E,D", 0, ld_e_d }, // 0x5a
	{ "LD E,E", 0, ld_e_e }, // 0x5b
	{ "LD E,H", 0, ld_e_h }, // 0x5c
	{ "LD E,L", 0, ld_e_l }, // 0x5d
	{ "LD E,(HL)", 0, ld_e_hlp }, // 0x5e
	{ "LD E,A", 0, ld_e_a }, // 0x5f
	{ "LD H,B", 0, ld_h_b }, // 0x60
	{ "LD H,C", 0, ld_h_c }, // 0x61
	{ "LD H,D", 0, ld_h_d }, // 0x62
	{ "LD H,E", 0, ld_h_e }, // 0x63
	{ "LD H,H", 0, ld_h_h }, // 0x64
	{ "LD H,L", 0, ld_h_l }, // 0x65
	{ "LD H,(HL)", 0, ld_h_hlp }, // 0x66
	{ "LD H,A", 0, ld_h_a }, // 0x67
	{ "LD L,B", 0, ld_l_b }, // 0x68
	{ "LD L,C", 0, ld_l_c }, // 0x69
	{ "LD L,D", 0, ld_l_d }, // 0x6a
	{ "LD L,E", 0, ld_l_e }, // 0x6b
	{ "LD L,H", 0, ld_l_h }, // 0x6c
	{ "LD L,L", 0, ld_l_l }, // 0x6d
	{ "LD L,(HL)", 0, ld_l_hlp }, // 0x6e
	{ "LD L,A", 0, ld_l_a }, // 0x6f
	{ "LD (HL),B", 0, ld_hlp_b }, // 0x70
	{ "LD (HL),C", 0, ld_hlp_c }, // 0x71
	{ "LD (HL),D", 0, ld_hlp_d }, // 0x72
	{ "LD (HL),E", 0, ld_hlp_e }, // 0x73
	{ "LD (HL),H", 0, ld_hlp_h }, // 0x74
	{ "LD (HL),L", 0, ld_hlp_l }, // 0x75
	{ "HALT", 0, halt }, // 0x76
	{ "LD (HL),A", 0, ld_hlp_a }, // 0x77
	{ "LD A,B", 0, ld_a_b }, // 0x78
	{ "LD A,C", 0, ld_a_c }, // 0x79
	{ "LD A,D", 0, ld_a_d }, // 0x7a
	{ "LD A,E", 0, ld_a_e }, // 0x7b
	{ "LD A,H", 0, ld_a_h }, // 0x7c
	{ "LD A,L", 0, ld_a_l }, // 0x7d
	{ "LD A,(HL)", 0, ld_a_hlp }, // 0x7e
	{ "LD A,A", 0, ld_a_a }, // 0x7f
	{ "ADD A,B", 0, add_a_b }, // 0x80
	{ "ADD A,C", 0, add_a_c }, // 0x81
	{ "ADD A,D", 0, add_a_d }, // 0x82
	{ "ADD A,E", 0, add_a_e }, // 0x83
	{ "ADD A,H", 0, add_a_h }, // 0x84
	{ "ADD A,L", 0, add_a_l }, // 0x85
	{ "ADD A,(HL)", 0, add_a_hlp }, // 0x86
	{ "ADD A,A", 0, add_a_a }, // 0x87
	{ "ADC A,B", 0, adc_a_b }, // 0x88
	{ "ADC A,C", 0, adc_a_c }, // 0x89
	{ "ADC A,D", 0, adc_a_d }, // 0x8a
	{ "ADC A,E", 0, adc_a_e }, // 0x8b
	{ "ADC A,H", 0, adc_a_h }, // 0x8c
	{ "ADC A,L", 0, adc_a_l }, // 0x8d
	{ "ADC A,(HL)", 0, adc_a_hlp }, // 0x8e
	{ "ADC A,A", 0, adc_a_a }, // 0x8f
	{ "SUB B", 0, sub_b }, // 0x90
	{ "SUB C", 0, sub_c }, // 0x91
	{ "SUB D", 0, sub_d }, // 0x92
	{ "SUB E", 0, sub_e }, // 0x93
	{ "SUB H", 0, sub_h }, // 0x94
	{ "SUB L", 0, sub_l }, // 0x95
	{ "SUB (HL)", 0, sub_hlp }, // 0x96
	{ "SUB A", 0, sub_a }, // 0x97
	{ "SBC A,B", 0, sbc_a_b }, // 0x98
	{ "SBC A,C", 0, sbc_a_c }, // 0x99
	{ "SBC A,D", 0, sbc_a_d }, // 0x9a
	{ "SBC A,E", 0, sbc_a_e }, // 0x9b
	{ "SBC A,H", 0, sbc_a_h }, // 0x9c
	{ "SBC A,L", 0, sbc_a_l }, // 0x9d
	{ "SBC A,(HL)", 0, sbc_a_hlp }, // 0x9e
	{ "SBC A,A", 0, sbc_a_a }, // 0x9f
	{ "AND B", 0, and_b }, // 0xa0
	{ "AND C", 0, and_c }, // 0xa1
	{ "AND D", 0, and_d }, // 0xa2
	{ "AND E", 0, and_e }, // 0xa3
	{ "AND H", 0, and_h }, // 0xa4
	{ "AND L", 0, and_l }, // 0xa5
	{ "AND (HL)", 0, and_hlp }, // 0xa6
	{ "AND A", 0, and_a }, // 0xa7
	{ "XOR B", 0, xor_b }, // 0xa8
	{ "XOR C", 0, xor_c }, // 0xa9
	{ "XOR D", 0, xor_d }, // 0xaa
	{ "XOR E", 0, xor_e }, // 0xab
	{ "XOR H", 0, xor_h }, // 0xac
	{ "XOR L", 0, xor_l }, // 0xad
	{ "XOR (HL)", 0, xor_hlp }, // 0xae
	{ "XOR A", 0, xor_a }, // 0xaf
	{ "OR B", 0, or_b }, // 0xb0
	{ "OR C", 0, or_c }, // 0xb1
	{ "OR D", 0, or_d }, // 0xb2
	{ "OR E", 0, or_e }, // 0xb3
	{ "OR H", 0, or_h }, // 0xb4
	{ "OR L", 0, or_l }, // 0xb5
	{ "OR (HL)", 0, or_hlp }, // 0xb6
	{ "OR A", 0, or_a }, // 0xb7
	{ "CP B", 0, cp_b }, // 0xb8
	{ "CP C", 0, cp_c }, // 0xb9
	{ "CP D", 0, cp_d }, // 0xba
	{ "CP E", 0, cp_e }, // 0xbb
	{ "CP H", 0, cp_h }, // 0xbc
	{ "CP L", 0, cp_l }, // 0xbd
	{ "CP (HL)", 0, cp_hlp }, // 0xbe
	{ "CP A", 0, cp_a }, // 0xbf
	{ "RET NZ", 0, ret_nz }, // 0xc0
	{ "POP BC", 0, pop_bc }, // 0xc1
	{ "JP NZ,a16", 2, jp_nz_nn }, // 0xc2
	{ "JP a16", 2, jp_nn }, // 0xc3
	{ "CALL NZ,a16", 2, call_nz_nn }, // 0xc4
	{ "PUSH BC", 0, push_bc }, // 0xc5
	{ "ADD A,d8", 1, add_a_n }, // 0xc6
	{ "RST 00H", 0, rst_00 }, // 0xc7
	{ "RET Z", 0, ret_z }, // 0xc8
	{ "RET", 0, ret }, // 0xc9
	{ "JP Z,a16", 2, jp_z_nn }, // 0xca
	{ "PREFIX CB", 0, cb }, // 0xcb
	{ "CALL Z,a16", 2, call_z_nn }, // 0xcc
	{ "CALL a16", 2, call_nn }, // 0xcd
	{ "ADC A,d8", 1, adc_a_n }, // 0xce
	{ "RST 08H", 0, rst_08 }, // 0xcf
	{ "RET NC", 0, ret_nc }, // 0xd0
	{ "POP DE", 0, pop_de }, // 0xd1
	{ "JP NC,a16", 2, jp_nc_nn }, // 0xd2
	{ "UNDEFINED", 0, undefined }, // 0xd3
	{ "CALL NC,a16", 2, call_nc_nn }, // 0xd4
	{ "PUSH DE", 0, push_de }, // 0xd5
	{ "SUB d8", 1, sub_n }, // 0xd6
	{ "RST 10H", 0, rst_10 }, // 0xd7
	{ "RET C", 0, ret_c }, // 0xd8
	{ "RETI", 0, reti }, // 0xd9
	{ "JP C,a16", 2, jp_c_nn }, // 0xda
	{ "UNDEFINED", 0, undefined }, // 0xdb
	{ "CALL C,a16", 2, call_c_nn }, // 0xdc
	{ "UNDEFINED", 0, undefined }, // 0xdd
	{ "SBC A,d8", 1, sbc_a_n }, // 0xde
	{ "RST 18H", 0, rst_18 }, // 0xdf
	{ "LDH (a8),A", 1, ld_ff_n_ap }, // 0xe0
	{ "POP HL", 0, pop_hl }, // 0xe1
	{ "LD (C),A", 1, ld_ff_c_a }, // 0xe2
	{ "UNDEFINED", 0, undefined }, // 0xe3
	{ "UNDEFINED", 0, undefined }, // 0xe4
	{ "PUSH HL", 0, push_hl }, // 0xe5
	{ "AND d8", 1, and_n }, // 0xe6
	{ "RST 20H", 0, rst_20 }, // 0xe7
	{ "ADD SP,r8", 1, add_sp_n }, // 0xe8
	{ "JP (HL)", 0, jp_hlp }, // 0xe9
	{ "LD (a16),A", 2, ld_nn_a }, // 0xea
	{ "UNDEFINED", 0, undefined }, // 0xeb
	{ "UNDEFINED", 0, undefined }, // 0xec
	{ "UNDEFINED", 0, undefined }, // 0xed
	{ "XOR d8", 1, xor_n }, // 0xee
	{ "RST 28H", 0, rst_28 }, // 0xef
	{ "LDH A,(a8)", 1, ld_ff_ap_n }, // 0xf0
	{ "POP AF", 0, pop_af }, // 0xf1
	{ "LD A,(C)", 1, ld_ff_a_c }, // 0xf2
	{ "DI", 0, di }, // 0xf3
	{ "UNDEFINED", 0, undefined }, // 0xf4
	{ "PUSH AF", 0, push_af }, // 0xf5
	{ "OR d8", 1, or_n }, // 0xf6
	{ "RST 30H", 0, rst_30 }, // 0xf7
	{ "LD HL,SP+r8", 1, ld_hl_sp_n}, // 0xf8
	{ "LD SP,HL", 0, ld_sp_hl }, // 0xf9
	{ "LD A,(a16)", 2, ld_a_nnp }, // 0xfa
	{ "EI", 0, ei }, // 0xfb
	{ "UNDEFINED", 0, undefined }, // 0xfc
	{ "UNDEFINED", 0, undefined }, // 0xfd
	{ "CP d8", 1, cp_n }, // 0xfe
	{ "RST 38H", 0, rst_38 }, // 0xff
};