
#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include "address.h"
#include "biu.h"
#include "cpu.h"


// ����ָ�������
void add(CPU *cpu, uint16_t *dest, uint16_t src);
void sub(CPU *cpu, uint16_t *dest, uint16_t src);
void mul(CPU *cpu, uint16_t src);
void imul(CPU *cpu, int16_t src);
void Div(CPU *cpu, uint16_t src);
void idiv(CPU *cpu, int16_t src);
void inc(CPU *cpu, uint16_t *value);
void dec(CPU *cpu, uint16_t *value);
void adc(CPU *cpu, uint16_t *dest, uint16_t src);
void sbb(CPU *cpu, uint16_t *dest, uint16_t src);

// �߼�ָ�������
void and(CPU *cpu, uint16_t *dest, uint16_t src);
void or(CPU *cpu, uint16_t *dest, uint16_t src);
void xor(CPU *cpu, uint16_t *dest, uint16_t src);
void test(CPU *cpu, uint16_t value1, uint16_t value2);

// ���ݴ���ָ�������
void mov(CPU *cpu, uint16_t *dest, uint16_t src, AddressingMode mode, const char *instruction);
void xchg(uint16_t *dest, uint16_t *src);
void push(CPU *cpu, uint16_t src);
uint16_t pop(CPU *cpu);

// ������ָ�������
void jmp(CPU *cpu, uint16_t address);
void call(CPU *cpu, uint16_t address);
void ret(CPU *cpu);

// �ַ�������ָ��ʵ��
void movsw(CPU *cpu);
void movsb(CPU *cpu, uint16_t dest, uint16_t src);
void cmpsw(CPU *cpu);
void cmpsb(CPU *cpu);
void scasw(CPU *cpu);
void scasb(CPU *cpu);
void lodsw(CPU *cpu);
void lodsb(CPU *cpu);
void stosw(CPU *cpu);
void stosb(CPU *cpu);
void lea(CPU *cpu, uint16_t reg, uint16_t addr, AddressingMode mode);

// ������������ָ��ʵ��
void nop();
void hlt();
void clc(CPU *cpu);
void stc(CPU *cpu);
void cmc(CPU *cpu);
void clcd(CPU *cpu);
void std(CPU *cpu);
void cli(CPU *cpu);
void sti(CPU *cpu);

// ���������ָ��ʵ��
void interrupt(CPU *cpu, uint8_t interrupt_number);
void iret(CPU *cpu);
void loop(CPU *cpu);
void loopz(CPU *cpu);
void loopnz(CPU *cpu);

void out(CPU *cpu, uint16_t port, uint16_t value);

#endif // INSTRUCTION_H



