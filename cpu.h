#ifndef CPU_H
#define CPU_H

#include <stdint.h>

// 定义 CPU 结构体
typedef struct CPU {
    uint16_t AX, BX, CX, DX;  // 通用寄存器
    uint8_t AL, AH;           // AX 的低位和高位
    uint16_t SP, BP, SI, DI;  // 堆栈寄存器
    uint16_t IP;              // 指令指针
    uint16_t CS, DS, ES, SS;  // 段寄存器
    uint8_t CF;               // 进位标志
    uint8_t ZF;               // 零标志
    uint8_t SF;               // 符号标志
    uint8_t OF;               // 溢出标志
    uint8_t PF;               // 奇偶标志
    uint8_t DF;               // 方向标志
    uint8_t IF;               // 中断标志
    uint32_t FLAGS;
} CPU;

// 标志位定义
#define FLAG_CF  0x0001 // 进位标志
#define FLAG_ZF  0x0002 // 零标志
#define FLAG_SF  0x0004 // 符号标志
#define FLAG_OF  0x0008 // 溢出标志
#define FLAG_PF  0x0010 // 奇偶标志
#define FLAG_DF 0x0020 // 方向标志
#define FLAG_IF 0x0040 // 中断标志


#define STACK_SIZE 0xFFFF
#define STACK_BOTTOM 0x0000  // 栈底
#define STACK_TOP (STACK_SIZE) // 栈顶


// 声明函数
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



