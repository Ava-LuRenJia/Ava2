#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include "biu.h"

// ��ʼ��CPU�Ĵ����ͱ�־
void cpu_init(CPU *cpu) {
    cpu->AX = 0;
    cpu->BX = 0;
    cpu->CX = 0;
    cpu->DX = 0;
    cpu->AL = 0;
    cpu->AH = 0;
    cpu->SP = 0;
    cpu->BP = 0;
    cpu->SI = 0;
    cpu->DI = 0;
    cpu->IP = 0;
    cpu->CS = 0;
    cpu->DS = 0;
    cpu->ES = 0;
    cpu->SS = 0;
    cpu->CF = 0;
    cpu->ZF = 0;
    cpu->SF = 0;
    cpu->OF = 0;
    cpu->PF = 0;
    cpu->DF = 0;
    cpu->IF = 0;
    cpu->FLAGS = 0;
}

// �������б�־λ
void cpu_reset_flags(CPU *cpu) {
    cpu->FLAGS = 0;
}

// ����ָ���ı�־λ
void cpu_set_flag(CPU *cpu, uint16_t flag, int condition) {
    if (condition) {
        cpu->FLAGS |= flag;  // ���ñ�־
    } else {
        cpu->FLAGS &= ~flag; // �����־
    }
}

// ���ָ���ı�־λ
void cpu_clear_flag(CPU *cpu, uint16_t flag) {
    cpu->FLAGS &= ~flag; // ���ָ����־λ
}

// ����ض���־λ״̬
int cpu_check_flag(const CPU *cpu, uint16_t flag) {
    return (cpu->FLAGS & flag) ? 1 : 0; // ���� 1 ��ʾ��־λ�����ã����� 0 ��ʾδ����
}

// �������������ı�־λ
void cpu_update_flags_arithmetic(CPU *cpu, uint32_t result, uint32_t src, uint32_t dest) {
    cpu_set_flag(cpu, FLAG_CF, result > 0xFFFF); // ���� CF
    cpu_set_flag(cpu, FLAG_ZF, (uint16_t)result == 0); // ���� ZF
    cpu_set_flag(cpu, FLAG_SF, (int16_t)(result & 0xFFFF) < 0); // ���� SF

    // ���� OF
    cpu_set_flag(cpu, FLAG_OF,
                 ((src > 0) && (dest > 0) && (result <= 0)) ||
                 ((src < 0) && (dest < 0) && (result >= 0))
    );

    // ���� PF����ż��־��
    uint8_t parity = __builtin_parity((uint8_t)(result & 0xFF)); // ��������ֽڵ���ż��
    cpu_set_flag(cpu, FLAG_PF, parity == 0);
}

// �����߼������ı�־λ
void cpu_update_flags_logical(CPU *cpu, uint32_t result) {
    cpu_set_flag(cpu, FLAG_ZF, result == 0); // ���� ZF
    cpu_set_flag(cpu, FLAG_PF, __builtin_parity(result & 0xFF) == 0); // ���� PF

    // ��� SF��CF��OF
    cpu_clear_flag(cpu, FLAG_SF | FLAG_CF | FLAG_OF);
}

// ��ӡCPU״̬�����ڵ��ԣ�
void cpu_print_state(const CPU *cpu) {
    printf("ͨ�üĴ���:\n");
    printf("AX: 0x%04X  AL: 0x%02X  AH: 0x%02X\n", cpu->AX, cpu->AL, cpu->AH);
    printf("BX: 0x%04X  CX: 0x%04X  DX: 0x%04X\n", cpu->BX, cpu->CX, cpu->DX);
    printf("��ջ�Ĵ���:\n");
    printf("SP: 0x%04X  BP: 0x%04X\n", cpu->SP, cpu->BP);
    printf("�����Ĵ���:\n");
    printf("SI: 0x%04X  DI: 0x%04X\n", cpu->SI, cpu->DI);
    printf("ָ��ָ��:\n");
    printf("IP: 0x%04X\n", cpu->IP);
    printf("�μĴ���:\n");
    printf("CS: 0x%04X  DS: 0x%04X  ES: 0x%04X  SS: 0x%04X\n", cpu->CS, cpu->DS, cpu->ES, cpu->SS);
    printf("��־�Ĵ���:\n");
    printf("FLAGS: 0x%04X\n", cpu->FLAGS);
    printf("��־״̬:\n");
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


// ��תָ����־λ
void cpu_toggle_flag(CPU *cpu, uint16_t flag) {
    cpu->FLAGS ^= flag; // ��תָ����־λ
}

// ��ֵѹ��ջ
void cpu_push(CPU *cpu, uint16_t value) {
    if (cpu->SP < 2) {
        printf("����ջ���\n");
        return;
    }
    cpu->SP -= 2; // SP �����ƶ�
    memory[cpu->SS + cpu->SP] = value; // ��ֵ����ջ
    printf("PUSH executed: Value = 0x%04X\n", value);
}

// ��ջ�е���ֵ
uint16_t cpu_pop(CPU *cpu) {
    uint16_t value = memory[cpu->SS + cpu->SP]; // ��ջ�л�ȡֵ
    cpu->SP += 2; // SP �����ƶ�
    printf("POP executed: Value = 0x%04X\n", value);
    return value;
}

// ��ת��ָ����ַ
void cpu_jmp(CPU *cpu, uint16_t address) {
    cpu->IP = address; // ����ָ��ָ��
    printf("JMP executed: Address = 0x%04X\n", address);
}




