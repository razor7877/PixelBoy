#ifndef CPU_H
#define CPU_H

#include <cstdint>

#define CPU_FREQ 4194304
#define FRAME_CYCLES 70224
#define DMA_DURATION 640

// Corresponds to a CPU instruction/opcode
// represented by a string mnemonic, the size of the operand (if any) and a pointer to the corresponding function
struct Instruction
{
    const char* disassembly;
    uint8_t operand_length;
    void* function;
    uint8_t duration; // Expressed in t-cycles
};

// The flags of the F register
enum flags
{
    FLAG_ZERO = 0b10000000,
    FLAG_NEGATIVE = 0b01000000,
    FLAG_HALFCARRY = 0b00100000,
    FLAG_CARRY = 0b00010000,
};

extern const Instruction instructions[256];

extern uint32_t cycle_count;
extern uint16_t dma_cycles_left;

// Registers
// They are grouped two by two. AF corresponds to registers A (upper 8 bits) and F (lower 8 bits)
// F is the flag register
extern uint16_t AF;
extern uint16_t BC;
extern uint16_t DE;
extern uint16_t HL;

extern uint16_t sp; // Stack pointer
extern uint16_t pc; // Program counter
extern uint8_t opcode;

extern uint16_t operand;

extern bool cpu_stopped;
extern bool cpu_halted;
extern bool IME;

extern float delta_time;

// The CPU handles enough instructions for 1 frame
void execute_frame();
// The CPU handles the next instruction and progresses the internal clock
void handle_instruction();
// Increments the internal clock
void tick(uint8_t cycles);
// Starts a DMA transfer from ROM to OAM
void dma_transfer();

// Returns the lower or upper byte of a given register
uint8_t lower_byte(uint16_t value);
uint8_t upper_byte(uint16_t value);

// Sets the value of the upper 8 bits of the register passed as argument
void set_reg_hi(uint16_t& reg, uint8_t value);
// Sets the value of the lower 8 bits of the register passed as argument
void set_reg_lo(uint16_t& reg, uint8_t value);

void set_flags(uint8_t flags);
void clear_flags(uint8_t flags);
bool get_flags(uint8_t flags);

// Bitwise operations between A (accumulator) and passed values
void and_r(uint8_t value);
void xor_r(uint8_t value);
void or_r(uint8_t value);

void cp_r(uint8_t value);
uint8_t inc_r(uint8_t value);
uint8_t dec_r(uint8_t value);
void add_r(uint8_t value);
void adc_r(uint8_t value);
void sub_r(uint8_t value);
void sbc_r(uint8_t value);

/*
Functions are named as follow:
LD BC,d16 : ld_bc_nn
Spaces become underscores
nn is a 16 bit word
n is an 8 bit word
p : parenthesis (address to the value in memory)
*/

void undefined();
void nop(); // 0x00
void ld_bc_nn(uint16_t operand); // 0x01
void ld_bcp_a(); // 0x02
void inc_bc(); // 0x03
void inc_b(); // 0x04
void dec_b(); // 0x05
void ld_b_n(uint8_t operand); // 0x06
void rlca(); // 0x07
void ld_nnp_sp(uint16_t operand); // 0x08
void add_hl_bc(); // 0x09
void ld_a_bcp(); // 0x0A
void dec_bc(); // 0x0B
void inc_c(); // 0x0C
void dec_c(); // 0x0D
void ld_c_n(uint8_t operand); // 0x0E
void rrca(); // 0x0F

void stop(uint8_t operand); // 0x10
void ld_de_nn(uint16_t operand); // 0x11
void ld_dep_a(); // 0x12
void inc_de(); // 0x13
void inc_d(); // 0x14
void dec_d(); // 0x15
void ld_d_n(uint8_t operand); // 0x16
void rla(); // 0x17
void jr_n(int8_t operand); // 0x18
void add_hl_de(); // 0x19
void ld_a_dep(); // 0x1A
void dec_de(); // 0x1B
void inc_e(); // 0x1C
void dec_e(); // 0x1D
void ld_e_n(uint8_t operand); // 0x1E
void rra(); // 0x1F

void jr_nz_n(int8_t operand); // 0x20
void ld_hl_nn(uint16_t operand); // 0x21
void ldi_hlp_a(); // 0x22
void inc_hl(); // 0x23
void inc_h(); // 0x24
void dec_h(); // 0x25
void ld_h_n(uint8_t operand); // 0x26
void daa(); // 0x27
void jr_z_n(int8_t operand); // 0x28
void add_hl_hl(); // 0x29
void ldi_a_hlp(); // 0x2A
void dec_hl(); // 0x2B
void inc_l(); // 0x2C
void dec_l(); // 0x2D
void ld_l_n(uint8_t operand); // 0x2E
void cpl(); // 0x2F

void jr_nc_n(int8_t operand); // 0x30
void ld_sp_nn(uint16_t operand); // 0x31
void ldd_hlp_a(); // 0x32
void inc_sp(); // 0x33
void inc_hlp(); // 0x34
void dec_hlp(); // 0x35
void ld_hlp_n(uint8_t operand); // 0x36
void scf(); // 0x37
void jr_c_n(int8_t operand); // 0x38
void add_hl_sp(); // 0x39
void ldd_a_hlp(); // 0x3A
void dec_sp(); // 0x3B
void inc_a(); // 0x3C
void dec_a(); // 0x3D
void ld_a_n(uint8_t operand); // 0x3E
void ccf(); // 0x3F

void ld_b_b();
void ld_b_c();
void ld_b_d();
void ld_b_e();
void ld_b_h();
void ld_b_l();
void ld_b_hlp();
void ld_b_a();
void ld_c_b();
void ld_c_c();
void ld_c_d();
void ld_c_e();
void ld_c_h();
void ld_c_l();
void ld_c_hlp();
void ld_c_a();

void ld_d_b();
void ld_d_c();
void ld_d_d();
void ld_d_e();
void ld_d_h();
void ld_d_l();
void ld_d_hlp();
void ld_d_a();
void ld_e_b();
void ld_e_c();
void ld_e_d();
void ld_e_e();
void ld_e_h();
void ld_e_l();
void ld_e_hlp();
void ld_e_a();

void ld_h_b();
void ld_h_c();
void ld_h_d();
void ld_h_e();
void ld_h_h();
void ld_h_l();
void ld_h_hlp();
void ld_h_a();
void ld_l_b();
void ld_l_c();
void ld_l_d();
void ld_l_e();
void ld_l_h();
void ld_l_l();
void ld_l_hlp();
void ld_l_a();

void ld_hlp_b();
void ld_hlp_c();
void ld_hlp_d();
void ld_hlp_e();
void ld_hlp_h();
void ld_hlp_l();
void halt();
void ld_hlp_a();
void ld_a_b();
void ld_a_c();
void ld_a_d();
void ld_a_e();
void ld_a_h();
void ld_a_l();
void ld_a_hlp();
void ld_a_a();

void add_a_b();
void add_a_c();
void add_a_d();
void add_a_e();
void add_a_h();
void add_a_l();
void add_a_hlp();
void add_a_a();
void adc_a_b();
void adc_a_c();
void adc_a_d();
void adc_a_e();
void adc_a_h();
void adc_a_l();
void adc_a_hlp();
void adc_a_a();

void sub_b();
void sub_c();
void sub_d();
void sub_e();
void sub_h();
void sub_l();
void sub_hlp();
void sub_a();
void sbc_a_b();
void sbc_a_c();
void sbc_a_d();
void sbc_a_e();
void sbc_a_h();
void sbc_a_l();
void sbc_a_hlp();
void sbc_a_a();

void and_b();
void and_c();
void and_d();
void and_e();
void and_h();
void and_l();
void and_hlp();
void and_a();
void xor_b();
void xor_c();
void xor_d();
void xor_e();
void xor_h();
void xor_l();
void xor_hlp();
void xor_a();

void or_b();
void or_c();
void or_d();
void or_e();
void or_h();
void or_l();
void or_hlp();
void or_a();
void cp_b();
void cp_c();
void cp_d();
void cp_e();
void cp_h();
void cp_l();
void cp_hlp();
void cp_a();

void ret_nz();
void pop_bc();
void jp_nz_nn(uint16_t operand);
void jp_nn(uint16_t operand);
void call_nz_nn(uint16_t operand);
void push_bc();
void add_a_n(uint8_t operand);
void rst_00();
void ret_z();
void ret();
void jp_z_nn(uint16_t operand);
void cb(uint8_t cb_opcode);
void call_z_nn(uint16_t operand);
void call_nn(uint16_t operand);
void adc_a_n(uint8_t operand);
void rst_08();

void ret_nc();
void pop_de();
void jp_nc_nn(uint16_t operand);
// Placeholder - No opcode
void call_nc_nn(uint16_t operand);
void push_de();
void sub_n(uint8_t operand);
void rst_10();
void ret_c();
void reti();
void jp_c_nn(uint16_t operand);
// Placeholder - No opcode
void call_c_nn(uint16_t operand);
// Placeholder - No opcode
void sbc_a_n(uint8_t operand);
void rst_18();

void ldh_n_a(uint8_t operand);
void pop_hl();
void ld_ff_c_a();
// Placeholder - No opcode
// Placeholder - No opcode
void push_hl();
void and_n(uint8_t operand);
void rst_20();
void add_sp_n(uint8_t operand);
void jp_hlp();
void ld_nnp_a(uint16_t operand);
// Placeholder - No opcode
// Placeholder - No opcode
// Placeholder - No opcode
void xor_n(uint8_t operand);
void rst_28();

void ldh_a_n(uint8_t operand);
void pop_af();
void ld_ff_a_c();
void di();
// Placeholder - No opcode
void push_af();
void or_n(uint8_t operand);
void rst_30();
void ld_hl_sp_n(uint8_t operand);
void ld_sp_hl();
void ld_a_nnp(uint16_t operand);
void ei();
// Placeholder - No opcode
// Placeholder - No opcode
void cp_n(uint8_t operand);
void rst_38();

#endif
