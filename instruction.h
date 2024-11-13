
#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include "address.h"
#include "biu.h"
#include "cpu.h"
#include <ctype.h>
// 算术指令函数声明
void add(CPU *cpu, const char *original_instruction, AddressingMode mode);
void sub(CPU *cpu, const char *original_instruction, AddressingMode mode);
void mul(CPU *cpu, uint16_t src);
void imul(CPU *cpu, int16_t src);
void Div(CPU *cpu, uint16_t src);
void idiv(CPU *cpu, int16_t src);
void inc(CPU *cpu, const char *original_instruction);
void dec(CPU *cpu, uint16_t *value);
void adc(CPU *cpu, uint16_t *dest, uint16_t src);
void sbb(CPU *cpu, uint16_t *dest, uint16_t src);

// 逻辑指令函数声明
void and(CPU *cpu, const char *original_instruction, AddressingMode mode);
void or(CPU *cpu, const char *original_instruction, AddressingMode mode);
void xor(CPU *cpu, const char *original_instruction, AddressingMode mode);
void not(CPU *cpu, const char *original_instruction);
void test(CPU *cpu, uint16_t value1, uint16_t value2);

// 数据传送指令函数声明
void mov(CPU *cpu, uint16_t *dest, uint16_t src, AddressingMode mode, const char *instruction,Variable variables[]);
void xchg(uint16_t *dest, uint16_t *src);
void push(CPU *cpu, uint16_t src);
uint16_t pop(CPU *cpu);

// 控制流指令函数声明
void jmp(CPU *cpu, const char *original_instruction, LabelAddress label_table[], int label_count);
void call(CPU *cpu, uint16_t address);
void ret(CPU *cpu);

// 字符串操作指令实现
void movsw(CPU *cpu);
void movsb(CPU *cpu);
void cmpsw(CPU *cpu);
void cmpsb(CPU *cpu);
void scasw(CPU *cpu);
void scasb(CPU *cpu);
void lodsw(CPU *cpu);
void lodsb(CPU *cpu);
void stosw(CPU *cpu);
void stosb(CPU *cpu);
void lea(CPU *cpu, AddressingMode mode , const char *original_instruction,Variable variables[]) ;

// 处理器控制类指令实现
void nop();
void hlt();
void clc(CPU *cpu);
void stc(CPU *cpu);
void cmc(CPU *cpu);
void clcd(CPU *cpu);
void std(CPU *cpu);
void cli(CPU *cpu);
void sti(CPU *cpu);

// 程序控制类指令实现
void interrupt(CPU *cpu, const char *original_instruction);
void iret(CPU *cpu);
void loop(CPU *cpu);
void loopz(CPU *cpu);
void loopnz(CPU *cpu);

void out(CPU *cpu, uint16_t port, uint16_t value);

#endif // INSTRUCTION_H



