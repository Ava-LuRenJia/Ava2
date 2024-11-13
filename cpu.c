#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include "biu.h"

// 初始化CPU寄存器和标志
void cpu_init(CPU *cpu) {
    cpu->AX = 0;
    cpu->BX = 0;
    cpu->CX = 100;
    cpu->DX = 0;
    cpu->SP = 0xFFFE;
    cpu->BP = 0;
    cpu->SI = 0;
    cpu->DI = 0;
    cpu->IP = 0;
    cpu->CS = CODE_SEGMENT_START;
    cpu->DS = DATA_SEGMENT_START;
    cpu->ES = 0;
    cpu->SS = STACK_SEGMENT_START;
    cpu->CF = 0;
    cpu->ZF = 0;
    cpu->SF = 0;
    cpu->OF = 0;
    cpu->PF = 0;
    cpu->DF = 0;
    cpu->IF = 0;
    cpu->FLAGS = 0;
}

// 重置所有标志位
void cpu_reset_flags(CPU *cpu) {
    cpu->FLAGS = 0;
}

// 设置指定的标志位
void cpu_set_flag(CPU *cpu, uint16_t flag, int condition) {
    if (condition) {
        cpu->FLAGS |= flag;  // 设置标志
    } else {
        cpu->FLAGS &= ~flag; // 清除标志
    }
}

// 清除指定的标志位
void cpu_clear_flag(CPU *cpu, uint16_t flag) {
    cpu->FLAGS &= ~flag; // 清除指定标志位
}

// 检查特定标志位状态
int cpu_check_flag(const CPU *cpu, uint16_t flag) {
    return (cpu->FLAGS & flag) ? 1 : 0; // 返回 1 表示标志位被设置，返回 0 表示未设置
}

// 更新算术运算后的标志位
void cpu_update_flags_arithmetic(CPU *cpu, uint32_t result, uint32_t src, uint32_t dest) {
    cpu_set_flag(cpu, FLAG_CF, result > 0xFFFF); // 更新 CF
    cpu_set_flag(cpu, FLAG_ZF, (uint16_t)result == 0); // 更新 ZF
    cpu_set_flag(cpu, FLAG_SF, (int16_t)(result & 0xFFFF) < 0); // 更新 SF

    // 更新 OF
    cpu_set_flag(cpu, FLAG_OF,
                 ((src > 0) && (dest > 0) && (result <= 0)) ||
                 ((src < 0) && (dest < 0) && (result >= 0))
    );

    // 更新 PF（奇偶标志）
    uint8_t parity = __builtin_parity((uint8_t)(result & 0xFF)); // 计算最低字节的奇偶性
    cpu_set_flag(cpu, FLAG_PF, parity == 0);
}

// 更新逻辑运算后的标志位
void cpu_update_flags_logical(CPU *cpu, uint32_t result) {
    cpu_set_flag(cpu, FLAG_ZF, result == 0); // 更新 ZF
    cpu_set_flag(cpu, FLAG_PF, __builtin_parity(result & 0xFF) == 0); // 更新 PF

    // 清除 SF、CF、OF
    cpu_clear_flag(cpu, FLAG_SF | FLAG_CF | FLAG_OF);
}

// 打印CPU状态（用于调试）
void cpu_print_state(const CPU *cpu) {
    printf("通用寄存器:\n");
    printf("AX: 0x%04X\n", cpu->AX);
    printf("BX: 0x%04X  CX: 0x%04X  DX: 0x%04X\n", cpu->BX, cpu->CX, cpu->DX);
    printf("堆栈寄存器:\n");
    printf("SP: 0x%04X  BP: 0x%04X\n", cpu->SP, cpu->BP);
    printf("索引寄存器:\n");
    printf("SI: 0x%04X  DI: 0x%04X\n", cpu->SI, cpu->DI);
    printf("指令指针:\n");
    printf("IP: 0x%04X\n", cpu->IP);
    printf("段寄存器:\n");
    printf("CS: 0x%04X  DS: 0x%04X  ES: 0x%04X  SS: 0x%04X\n", cpu->CS, cpu->DS, cpu->ES, cpu->SS);
    printf("标志寄存器:\n");
    printf("FLAGS: 0x%04X\n", cpu->FLAGS);
    printf("标志状态:\n");
    printf("CF: %d  ZF: %d  SF: %d  OF: %d  PF: %d\n",
           cpu_check_flag(cpu, FLAG_CF),
           cpu_check_flag(cpu, FLAG_ZF),
           cpu_check_flag(cpu, FLAG_SF),
           cpu_check_flag(cpu, FLAG_OF),
           cpu_check_flag(cpu, FLAG_PF));
    printf("DF: %d  IF: %d\n",
           cpu_check_flag(cpu, FLAG_DF),
           cpu_check_flag(cpu, FLAG_IF));
    printf("--------------------------------------------------------\n");
}


// 反转指定标志位
void cpu_toggle_flag(CPU *cpu, uint16_t flag) {
    cpu->FLAGS ^= flag; // 反转指定标志位
}

// 将值压入栈
void cpu_push(CPU *cpu, uint16_t value) {
    if (cpu->SP < 2) {
        printf("错误：栈溢出\n");
        return;
    }
    cpu->SP -= 2; // SP 向下移动
    memory[cpu->SS + cpu->SP] = value & 0xFF;       // 将低字节存入栈
    memory[cpu->SS + cpu->SP + 1] = (value >> 8) & 0xFF;  // 将高字节存入栈
    printf("PUSH executed: Value = 0x%04X\n", value);
}

// 从栈中弹出值
uint16_t cpu_pop(CPU *cpu) {
    uint16_t value = memory[cpu->SS + cpu->SP]; // 从栈中获取值
    cpu->SP += 2; // SP 向上移动
    printf("POP executed: Value = 0x%04X\n", value);
    return value;
}

// 跳转到指定地址
void cpu_jmp(CPU *cpu, uint16_t address) {
    cpu->IP = address; // 更新指令指针
    printf("JMP executed: Address = 0x%04X\n", address);
}

// 获取特定标志的状态
bool cpu_get_flag(CPU *cpu, uint16_t flag) {
    return (cpu->FLAGS & flag) != 0;
}



