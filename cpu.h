#ifndef CPU_H
#define CPU_H

#include <stdint.h>

// ���� CPU �ṹ��
typedef struct CPU {
    uint16_t AX, BX, CX, DX;  // ͨ�üĴ���
    uint8_t AL, AH;           // AX �ĵ�λ�͸�λ
    uint16_t SP, BP, SI, DI;  // ��ջ�Ĵ���
    uint16_t IP;              // ָ��ָ��
    uint16_t CS, DS, ES, SS;  // �μĴ���
    uint8_t CF;               // ��λ��־
    uint8_t ZF;               // ���־
    uint8_t SF;               // ���ű�־
    uint8_t OF;               // �����־
    uint8_t PF;               // ��ż��־
    uint8_t DF;               // �����־
    uint8_t IF;               // �жϱ�־
    uint32_t FLAGS;
} CPU;

// ��־λ����
#define FLAG_CF  0x0001 // ��λ��־
#define FLAG_ZF  0x0002 // ���־
#define FLAG_SF  0x0004 // ���ű�־
#define FLAG_OF  0x0008 // �����־
#define FLAG_PF  0x0010 // ��ż��־
#define FLAG_DF 0x0020 // �����־
#define FLAG_IF 0x0040 // �жϱ�־


#define STACK_SIZE 0xFFFF
#define STACK_BOTTOM 0x0000  // ջ��
#define STACK_TOP (STACK_SIZE) // ջ��


// ��������
void cpu_init(CPU *cpu);
void cpu_reset_flags(CPU *cpu);
void cpu_set_flag(CPU *cpu, uint16_t flag, int condition);

void cpu_clear_flag(CPU *cpu, uint16_t flag);
int cpu_check_flag(const CPU *cpu, uint16_t flag);
void cpu_update_flags_arithmetic(CPU *cpu, uint32_t result, uint32_t src, uint32_t dest);
void cpu_print_state(const CPU *cpu);
void cpu_push(CPU *cpu, uint16_t value);
void cpu_update_flags_logical(CPU *cpu, uint32_t result);
void cpu_jmp(CPU *cpu, uint16_t address);
void cpu_toggle_flag(CPU *cpu, uint16_t flag);
uint16_t cpu_pop(CPU *cpu);

#endif // CPU_H



