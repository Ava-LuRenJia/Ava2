#include "instruction.h"
#include <stdio.h>
#include <stdint.h>
#include "address.h"

// PUSH ָ��
void push(CPU *cpu, uint16_t src) {
    if (cpu->SP < 2) { // ջ�ĵײ����
        printf("Error: Stack overflow\n");
        return;
    }
    cpu->SP -= 2;
    memory[cpu->SP] = (uint8_t)(src & 0xFF); // �洢��8λ
    memory[cpu->SP + 1] = (uint8_t)((src >> 8) & 0xFF); // �洢��8λ
    printf("PUSH executed: Value = 0x%04X, SP = 0x%04X\n", src, cpu->SP);
}


// POP ָ��
uint16_t pop(CPU *cpu) {
    if (cpu->SP > STACK_TOP - 2) {
        printf("Error: Stack underflow\n");
        return 0; // ����һ��Ĭ��ֵ
    }
    uint16_t value = memory[cpu->SP];
    cpu->SP += 2;
    printf("POP executed: Value = 0x%04X, SP = 0x%04X\n", value, cpu->SP);
    return value;
}

// XCHG ָ�� (�����Ĵ������ڴ�ֵ)
void xchg(uint16_t *dest, uint16_t *src) {
    uint16_t temp = *dest;
    *dest = *src;
    *src = temp;

    printf("XCHG executed: Dest = 0x%04X, Src = 0x%04X\n", *dest, *src);
}

// ADC ָ�� (����λ�ļӷ�)
void adc(CPU *cpu, uint16_t *dest, uint16_t src) {
    uint32_t carry = (cpu->FLAGS & FLAG_CF) ? 1 : 0; // ��ȡ��λ��־
    uint32_t result = (uint32_t)*dest + (uint32_t)src + carry; // ����λ�ļӷ�
    *dest = (uint16_t)result; // ֻ������ 16 λ

    // ���±�־λ
    cpu_update_flags_arithmetic(cpu, result, src, *dest);

    // �����λ��־ CF
    if (result > 0xFFFF) {
        cpu->FLAGS |= FLAG_CF; // ���ý�λ��־
    } else {
        cpu->FLAGS &= ~FLAG_CF; // �����λ��־
    }

    printf("ADC executed: Result = 0x%04X, CF = %d\n", *dest, (cpu->FLAGS & FLAG_CF) ? 1 : 0);
}


// SBB ָ�� (����λ�ļ���)
void sbb(CPU *cpu, uint16_t *dest, uint16_t src) {
    uint32_t carry = (cpu->FLAGS & FLAG_CF) ? 1 : 0; // ��ȡ��λ��־
    int32_t result = (int32_t)*dest - (int32_t)src - carry; // ����λ�ļ���
    *dest = (uint16_t)result;

    // ���±�־λ
    cpu_update_flags_arithmetic(cpu, result, -src, *dest);

    // �����λ��־ CF
    if (result < 0) {
        cpu->FLAGS |= FLAG_CF; // ���ý�λ��־
    } else {
        cpu->FLAGS &= ~FLAG_CF; // �����λ��־
    }

    printf("SBB executed: Result = 0x%04X, CF = %d\n", *dest, (cpu->FLAGS & FLAG_CF) ? 1 : 0);
}


// INC ָ�� (����)
void inc(CPU *cpu, uint16_t *value) {
    uint32_t result = (uint32_t)(*value) + 1;
    *value = (uint16_t)result;

    // ���±�־λ
    cpu_update_flags_arithmetic(cpu, result, 1, *value);

    printf("INC executed: Value = 0x%04X\n", *value);
}

// DEC ָ�� (�Լ�)
void dec(CPU *cpu, uint16_t *value) {
    uint32_t result = (uint32_t)(*value) - 1;
    *value = (uint16_t)result;

    // ���±�־λ
    cpu_update_flags_arithmetic(cpu, result, -1, *value);

    printf("DEC executed: Value = 0x%04X\n", *value);
}

// AND ָ�� (λ��)
void and(CPU *cpu, uint16_t *dest, uint16_t src) {
    *dest &= src;

    // ���±�־λ
    cpu_update_flags_logical(cpu, *dest);

    printf("AND executed: Result = 0x%04X\n", *dest);
}

// OR ָ�� (λ��)
void or(CPU *cpu, uint16_t *dest, uint16_t src) {
    *dest |= src;

    // ���±�־λ
    cpu_update_flags_logical(cpu, *dest);

    printf("OR executed: Result = 0x%04X\n", *dest);
}

// XOR ָ�� (λ���)
void xor(CPU *cpu, uint16_t *dest, uint16_t src) {
    *dest ^= src;

    // ���±�־λ
    cpu_update_flags_logical(cpu, *dest);

    printf("XOR executed: Result = 0x%04X\n", *dest);
}

// TEST ָ�� (�߼��룬���ڸ��±�־λ)
void test(CPU *cpu, uint16_t value1, uint16_t value2) {
    uint16_t result = value1 & value2;

    // ���±�־λ
    cpu_update_flags_logical(cpu, result);

    printf("TEST executed: Result = 0x%04X\n", result);
}


// NOP ָ�� (�޲���)
void nop() {
    printf("NOP executed: No operation performed.\n");
}

// HLT ָ�� (ִֹͣ��)
void hlt() {
    printf("HLT executed: Halting execution.\n");
    // ���������Ӵ�����ֹͣ CPU ִ��
}

// JMP ָ�� (��������ת)
void jmp(CPU *cpu, uint16_t address) {
    cpu->IP = address; // ����ָ��ָ�뵽�µĵ�ַ

    printf("JMP executed: Jumping to address 0x%04X\n", address);
}

// CALL ָ�� (�����ӳ���)
void call(CPU *cpu, uint16_t address) {
    if (cpu->SP < 2) { // ջ�ĵײ����
        printf("Error: Stack overflow\n");
        return;
    }
    push(cpu, cpu->IP); // ����ǰָ��ָ��ѹ��ջ
    cpu->IP = address;  // ��ת��Ŀ���ַ
    printf("CALL executed: Jumping to address 0x%04X\n", address);
}

// RET ָ�� (�����ӳ���)
void ret(CPU *cpu) {
    if (cpu->SP > STACK_TOP - 2) { // ȷ�����㹻�ռ䵯�� IP
        printf("Error: Stack underflow\n");
        return;
    }
    cpu->IP = pop(cpu); // �ָ� IP
    printf("RET executed: Returning to address 0x%04X\n", cpu->IP);
}

// ADD ָ�� (�޷��żӷ�)
void add(CPU *cpu, uint16_t *dest, uint16_t src) {
    uint32_t result = (uint32_t)*dest + (uint32_t)src;
    *dest = (uint16_t)result;

    // ���±�־λ
    cpu_update_flags_arithmetic(cpu, result, src, *dest);

    // �����λ��־ CF���޷��żӷ���
    if (result > 0xFFFF) {
        cpu->FLAGS |= FLAG_CF; // ���ý�λ��־
    } else {
        cpu->FLAGS &= ~FLAG_CF; // �����λ��־
    }

    printf("ADD executed: Result = 0x%04X\n", *dest);
}

// SUB ָ�� (�޷��ż���)
void sub(CPU *cpu, uint16_t *dest, uint16_t src) {
    uint32_t result = (uint32_t)*dest - (uint32_t)src;
    *dest = (uint16_t)result;

    // ���±�־λ
    cpu_update_flags_arithmetic(cpu, result, -src, *dest);

    // �����λ��־ CF
    if (*dest > src) {
        cpu->FLAGS |= FLAG_CF; // ���ý�λ��־
    } else {
        cpu->FLAGS &= ~FLAG_CF; // �����λ��־
    }

    printf("SUB executed: Result = 0x%04X\n", *dest);
}

//MOV
void mov(CPU *cpu, uint16_t *dest, uint16_t src, AddressingMode mode, const char *instruction) {
    // ����Դ������
    if (sscanf(instruction, "MOV %*s, %hu", &src) != 1) {
        // ���û��������������Ĵ���
        if (strstr(instruction, "AX")) {
            cpu->AX =*dest;
        } else if (strstr(instruction, "AL")) {
            cpu->AL = *dest;
        } else if (strstr(instruction, "AH")) {
            cpu->AH = *dest;
        } else if (strstr(instruction, "BX")) {
            cpu->BX = *dest;
        } else if (strstr(instruction, "CX")) {
            cpu->CX = *dest;
        } else if (strstr(instruction, "DX")) {
            cpu->DX = *dest;
        }
    }

    switch (mode) {
        case IMMEDIATE: // ������Ѱַ
            src = *dest; // �� src ����Ŀ��
            break;

        case REGISTER: // �Ĵ���Ѱַ
            if (strstr(instruction, "AX")) {
                cpu->AX =*dest; // ���� AX
            } else if (strstr(instruction, "AL")) {
                cpu->AL = *dest; // ���� AL
            } else if (strstr(instruction, "AH")) {
                cpu->AH = *dest; // ���� AH
            } else if (strstr(instruction, "BX")) {
                cpu->BX = *dest; // ���� BX
            } else if (strstr(instruction, "CX")) {
                cpu->CX = *dest; // ���� CX
            } else if (strstr(instruction, "DX")) {
                cpu->DX = *dest; // ���� DX
            }
            break;

        case DIRECT: // ֱ��Ѱַ
            memory[*dest] = src; // �� src �����ڴ�
            break;

        case INDIRECT: // �Ĵ������Ѱַ
            memory[src] = *dest; // �� src ����ͨ���Ĵ���Ѱַ���ڴ�λ��
            break;

        case BASE_INDEX: // ��ַ�ӱ�ַѰַ
            memory[*dest + cpu->SI] = src; // �� src �����ַ�ӱ�ַ���ڴ�
            break;

        case RELATIVE: // ���Ѱַ
            memory[cpu->IP + src] = *dest; // �� src ������Ե�ַ
            break;

        default:
            printf("����: δʶ���Ѱַģʽ��\n");
            break;
    }

    printf("MOV ִ�����: Dest = 0x%04X, Src = 0x%04X, Mode = %d\n", *dest, src, mode);
}



// �޷��ų˷�ʵ��
void mul(CPU *cpu, uint16_t src) {
    // ���浱ǰ AX ��ֵ
    uint32_t result = (uint32_t)cpu->AX * (uint32_t)src;

    // ����Ƿ����
    if (result > 0xFFFF) {
        cpu_set_flag(cpu, FLAG_CF, 1); // ���ý�λ��־
        ; // ���ý�λ��־
        result &= 0xFFFF; // ������ 16 λ
    } else {
        cpu_clear_flag(cpu, FLAG_CF); // �����λ��־
    }

    cpu->AX = (uint16_t)result; // �������� AX

    // ����������־λ
    cpu_update_flags_arithmetic(cpu, result, src, cpu->AX);

    printf("MUL executed: AX = 0x%04X, src = 0x%04X\n", cpu->AX, src);
}

void imul(CPU *cpu, int16_t src) {
    // ���浱ǰ AX ��ֵ
    int32_t result = (int32_t)cpu->AX * (int32_t)src;

    // ����Ƿ����
    if (result > INT16_MAX || result < INT16_MIN) {
        cpu_set_flag(cpu, FLAG_OF, 1); // ���������־
        ; // ���������־
        result = result > INT16_MAX ? INT16_MAX : INT16_MIN; // ���ƽ������Ч��Χ
    } else {
        cpu_clear_flag(cpu, FLAG_OF); // ��������־
    }

    cpu->AX = (int16_t)result; // �������� AX

    // ����������־λ
    cpu_update_flags_arithmetic(cpu, result, src, cpu->AX);

    printf("IMUL executed: AX = 0x%04X, src = %d\n", cpu->AX, src);
}

void Div(CPU *cpu, uint16_t divisor) {
    if (divisor == 0) {
        // ������������
        printf("Error: Division by zero\n");
        return;
    }

    // ������������ AX ���Գ������̴���� AX����������� DX
    uint32_t dividend = (uint32_t)cpu->AX; // ȷ�����������
    cpu->AX = dividend / divisor; // ��
    cpu->DX = dividend % divisor; // ����
}


void idiv(CPU *cpu, int16_t divisor) {
    if (divisor == 0) {
        // ������������
        printf("Error: Division by zero\n");
        return;
    }

    // �����̺�����
    int32_t dividend = (int32_t)cpu->AX; // ȷ��AX���з��ŵ�
    cpu->AX = dividend / divisor; // ��
    cpu->DX = dividend % divisor; // ����
}

// �ַ�������ָ��ʵ��
void movsw(CPU *cpu) {
    uint16_t value = *(uint16_t *)&memory[cpu->SI]; // ���ڴ��ȡ��
    *(uint16_t *)&memory[cpu->DI] = value;          // д�뵽Ŀ���ڴ�
    cpu->SI += 2;                                   // ���� SI
    cpu->DI += 2;                                   // ���� DI
}

// MOVSB ָ�� (�ֽ��ƶ�)
void movsb(CPU *cpu, uint16_t dest, uint16_t src) {
    memory[dest] = memory[src]; // ���ֽڴ� src �ƶ��� dest

    printf("MOVSB executed: Src = 0x%04X, Dest = 0x%04X\n", src, dest);
}

void cmpsw(CPU *cpu) {
    uint16_t left = *(uint16_t *)&memory[cpu->SI];
    uint16_t right = *(uint16_t *)&memory[cpu->DI];
    cpu_set_flag(cpu, FLAG_ZF, left == right);
    cpu_set_flag(cpu, FLAG_CF, left < right);
    cpu_set_flag(cpu, FLAG_SF, (left - right) >> 15); // ���÷��ű�־
}

void cmpsb(CPU *cpu) {
    uint8_t left = memory[cpu->SI];
    uint8_t right = memory[cpu->DI];
    cpu_set_flag(cpu, FLAG_ZF, left == right);
    cpu_set_flag(cpu, FLAG_CF, left < right);
    cpu_set_flag(cpu, FLAG_SF, (left - right) & 0x80); // ���÷��ű�־
}

void scasw(CPU *cpu) {
    uint16_t right = *(uint16_t *)&memory[cpu->DI];
    cpu_set_flag(cpu, FLAG_ZF, cpu->AX == right);
    cpu_set_flag(cpu, FLAG_CF, cpu->AX < right);
    cpu_set_flag(cpu, FLAG_SF, (cpu->AX - right) >> 15); // ���÷��ű�־
}

void scasb(CPU *cpu) {
    uint8_t right = memory[cpu->DI];
    cpu_set_flag(cpu, FLAG_ZF, cpu->AL == right);
    cpu_set_flag(cpu, FLAG_CF, cpu->AL < right);
    cpu_set_flag(cpu, FLAG_SF, (cpu->AL - right) & 0x80); // ���÷��ű�־
}

void lodsw(CPU *cpu) {
    cpu->AX = *(uint16_t *)&memory[cpu->SI]; // ���ڴ������
    cpu->SI += 2;                            // ���� SI
}

void lodsb(CPU *cpu) {
    cpu->AL = memory[cpu->SI];              // ���ڴ�����ֽ�
    cpu->SI += 1;                            // ���� SI
}

void stosw(CPU *cpu) {
    *(uint16_t *)&memory[cpu->DI] = cpu->AX; // �洢���ڴ�
    cpu->DI += 2;                            // ���� DI
}

void stosb(CPU *cpu) {
    memory[cpu->DI] = cpu->AL;               // �洢���ڴ�
    cpu->DI += 1;                            // ���� DI
}

// ������������ָ��ʵ��
void clc(CPU *cpu) {
    cpu_clear_flag(cpu, FLAG_CF); // �����λ��־
}

void stc(CPU *cpu) {
    cpu_set_flag(cpu, FLAG_CF, 1); // ���ý�λ��־
}

void cmc(CPU *cpu) {
    cpu_toggle_flag(cpu, FLAG_CF); // ��ת��λ��־
}

void clcd(CPU *cpu) {
    cpu_clear_flag(cpu, FLAG_DF); // ��������־
}

void std(CPU *cpu) {
    cpu_set_flag(cpu, FLAG_DF, 1); // ���÷����־
}

void cli(CPU *cpu) {
    cpu_clear_flag(cpu, FLAG_IF); // ����жϱ�־
}

void sti(CPU *cpu) {
    cpu_set_flag(cpu, FLAG_IF, 1); // �����жϱ�־
}

// ���������ָ��ʵ��

void interrupt(CPU *cpu, uint8_t interrupt_number) {
    if (cpu->SP < 4) { // ȷ�����㹻�Ŀռ�洢 IP �� FLAGS
        printf("Error: Stack overflow\n");
        return;
    }
    push(cpu, cpu->IP);    // ���� IP
    push(cpu, cpu->FLAGS); // �����־�Ĵ���
    uint16_t vector_address = memory[interrupt_number * 4] | (memory[interrupt_number * 4 + 1] << 8);
    cpu->IP = vector_address;
    printf("INT %d executed: Jumping to vector address 0x%04X\n", interrupt_number, vector_address);
}


void iret(CPU *cpu) {
    if (cpu->SP > STACK_TOP - 4) { // ȷ�����㹻�ռ䵯�� FLAGS �� IP
        printf("Error: Stack underflow\n");
        return;
    }
    cpu->FLAGS = pop(cpu); // �ָ���־�Ĵ���
    cpu->IP = pop(cpu); // �ָ� IP
}

void loop(CPU *cpu) {
    if (cpu->CX != 0) {
        cpu->CX--; // ���ټ���
        cpu->IP++; // ��ת����һ��ָ��
    }
}

void loopz(CPU *cpu) {
    if (cpu->CX != 0 && cpu->ZF) {
        cpu->CX--; // ���ټ���
        cpu->IP++; // ��ת����һ��ָ��
    }
}

void loopnz(CPU *cpu) {
    if (cpu->CX != 0 && !cpu->ZF) {
        cpu->CX--; // ���ټ���
        cpu->IP++; // ��ת����һ��ָ��
    }
}

// LEA ָ���ʵ��
void lea(CPU *cpu, uint16_t reg, uint16_t addr, AddressingMode mode) {
    uint16_t effective_address;
    switch (mode) {
        case IMMEDIATE:
            effective_address = addr; // ֱ��ʹ��������
            break;
        case REGISTER:
            // �������addr�ǼĴ�����ţ�ʵ����Ҫ�ӼĴ����ж�ȡ��ַ
            switch (addr) {
                case 0:
                    effective_address = cpu->AX;
                    break;
                case 1:
                    effective_address = cpu->BX;
                    break;
                case 2:
                    effective_address = cpu->CX;
                    break;
                case 3:
                    effective_address = cpu->DX;
                    break;
                case 4:
                    effective_address = cpu->SI;
                    break;
                case 5:
                    effective_address = cpu->DI;
                    break;
                default:
                    printf("Invalid register specified for REGISTER addressing: %d\n", addr);
                    return;
            }
            break;
        case DIRECT:
            effective_address = addr; // ʹ��ֱ�ӵ�ַ
            break;
        case INDIRECT:
            // ���� addr �Ǵ�ŵ�ַ�ļĴ���
            effective_address = memory[cpu->BX]; // ʹ�� BX �Ĵ�����Ϊָ��
            break;
        case BASE_INDEX:
            effective_address = cpu->BX + cpu->SI; // ��ַ + ��ַ
            break;
        case RELATIVE:
            effective_address = cpu->CS + addr; // ʹ�� CS �Ĵ����������Ѱַ
            break;
        default:
            printf("Invalid addressing mode specified for LEA.\n");
            return;
    }
}


void out(CPU *cpu, uint16_t port, uint16_t value) {
    // �����ָ���˿ڵ��߼�
    printf("������˿� %u: ֵ: 0x%04X\n", port, value);
    // ʵ�ʵ�Ӳ�������߼�Ӧ�ڴ˴�ʵ��
}