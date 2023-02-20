#pragma once

struct instruction
{
    const char* disassembly;
    uint8_t operand_length;
    void* function;
};

extern const instruction instructions[256];

/*
Functions are named as follow:
LD BC,d16 : ld_bc_nn
Spaces become underscores
nn is a 16 bit word
n is an 8 bit word
*/

void nop(); // 0x00
void ld_bc_nn(); // 0x01
void ld_bc_a(); // 0x02
void inc_bc(); // 0x03
void inc_b(); // 0x04
void dec_b(); // 0x05
void ld_b_n(); // 0x06
void rlca(); // 0x07
void ld_nn_sp(); // 0x08
void add_hl_bc(); // 0x09
void ld_a_bc(); // 0x0A
void dec_bc(); // 0x0B
void inc_c(); // 0x0C
void dec_c(); // 0x0D
void ld_c_n(); // 0x0E
void rrca(); // 0x0F

void stop(); // 0x10
void ld_de_nn(); // 0x11
void ld_de_a(); // 0x12
void inc_de(); // 0x13
void inc_d(); // 0x14
void dec_d(); // 0x15
void ld_d_n(); // 0x16
void rla(); // 0x17
void jr_n(); // 0x18
void add_hl_de(); // 0x19
void ld_a_de(); // 0x1A
void dec_de(); // 0x1B
void inc_e(); // 0x1C
void dec_e(); // 0x1D
void ld_e_n(); // 0x1E
void rra(); // 0x1F

void jr_nz_n();
void ld_hl_nn();
void ldi_hl_a();
void inc_hl();
void inc_h();
void dec_h();
void ld_h_n();
void daa();
void jr_z_n();
void add_hl_hl();
void ldi_a_hl();
void dec_hl();
void inc_l();
void dec_l();
void ld_l_n();
void cpl();

void jr_nc_n();
void ld_sp_nn();
void ldd_hl_a();
void inc_sp();
void inc_hl();
void dec_hl();
void ld_hl_n();
void scf();
void jr_c_n();
void add_hl_sp();
void ldd_a_hl();
void dec_sp();
void inc_a();
void dec_a();
void ld_a_n();
void ccf();

void ld_b_b();
void ld_b_c();
void ld_b_d();
void ld_b_e();
void ld_b_h();
void ld_b_l();
void ld_b_hl();
void ld_b_a();
void ld_c_b();
void ld_c_c();
void ld_c_d();
void ld_c_e();
void ld_c_h();
void ld_c_l();
void ld_c_hl();
void ld_c_a();

void ld_d_b();
void ld_d_c();
void ld_d_d();
void ld_d_e();
void ld_d_h();
void ld_d_l();
void ld_d_hl();
void ld_d_a();
void ld_e_b();
void ld_e_c();
void ld_e_d();
void ld_e_e();
void ld_e_h();
void ld_e_l();
void ld_e_hl();
void ld_e_a();

void ld_h_b();
void ld_h_c();
void ld_h_d();
void ld_h_e();
void ld_h_h();
void ld_h_l();
void ld_h_hl();
void ld_h_a();
void ld_l_b();
void ld_l_c();
void ld_l_d();
void ld_l_e();
void ld_l_h();
void ld_l_l();
void ld_l_hl();
void ld_l_a();

void ld_hl_b();
void ld_hl_c();
void ld_hl_d();
void ld_hl_e();
void ld_hl_h();
void ld_hl_l();
void halt();
void ld_hl_a();
void ld_a_b();
void ld_a_c();
void ld_a_d();
void ld_a_e();
void ld_a_h();
void ld_a_l();
void ld_a_hl();
void ld_a_a();