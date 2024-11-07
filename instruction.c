#include "instruction.h"
#include <stdio.h>
#include <stdint.h>
#include "address.h"

// PUSH 指令
void push(CPU *cpu, uint16_t src) {
    if (cpu->SP < 2) { // 栈的底部检测
        printf("Error: Stack overflow\n");
        return;
    }
    cpu->SP -= 2;
    memory[cpu->SP] = (uint8_t)(src & 0xFF); // 存储低8位
    memory[cpu->SP + 1] = (uint8_t)((src >> 8) & 0xFF); // 存储高8位
    printf("PUSH executed: Value = 0x%04X, SP = 0x%04X\n", src, cpu->SP);
}


// POP 指令
uint16_t pop(CPU *cpu) {
    if (cpu->SP > STACK_TOP - 2) {
        printf("Error: Stack underflow\n");
        return 0; // 返回一个默认值
    }
    uint16_t value = memory[cpu->SP];
    cpu->SP += 2;
    printf("POP executed: Value = 0x%04X, SP = 0x%04X\n", value, cpu->SP);
    return value;
}

// XCHG 指令 (交换寄存器或内存值)
void xchg(uint16_t *dest, uint16_t *src) {
    uint16_t temp = *dest;
    *dest = *src;
    *src = temp;

    printf("XCHG executed: Dest = 0x%04X, Src = 0x%04X\n", *dest, *src);
}

// ADC 指令 (带进位的加法)
void adc(CPU *cpu, uint16_t *dest, uint16_t src) {
    uint32_t carry = (cpu->FLAGS & FLAG_CF) ? 1 : 0; // 读取进位标志
    uint32_t result = (uint32_t)*dest + (uint32_t)src + carry; // 带进位的加法
    *dest = (uint16_t)result; // 只保留低 16 位

    // 更新标志位
    cpu_update_flags_arithmetic(cpu, result, src, *dest);

    // 处理进位标志 CF
    if (result > 0xFFFF) {
        cpu->FLAGS |= FLAG_CF; // 设置进位标志
    } else {
        cpu->FLAGS &= ~FLAG_CF; // 清除进位标志
    }

    printf("ADC executed: Result = 0x%04X, CF = %d\n", *dest, (cpu->FLAGS & FLAG_CF) ? 1 : 0);
}


// SBB 指令 (带借位的减法)
void sbb(CPU *cpu, uint16_t *dest, uint16_t src) {
    uint32_t carry = (cpu->FLAGS & FLAG_CF) ? 1 : 0; // 读取进位标志
    int32_t result = (int32_t)*dest - (int32_t)src - carry; // 带借位的减法
    *dest = (uint16_t)result;

    // 更新标志位
    cpu_update_flags_arithmetic(cpu, result, -src, *dest);

    // 处理借位标志 CF
    if (result < 0) {
        cpu->FLAGS |= FLAG_CF; // 设置借位标志
    } else {
        cpu->FLAGS &= ~FLAG_CF; // 清除借位标志
    }

    printf("SBB executed: Result = 0x%04X, CF = %d\n", *dest, (cpu->FLAGS & FLAG_CF) ? 1 : 0);
}


// INC 指令 (自增)
void inc(CPU *cpu, uint16_t *value) {
    uint32_t result = (uint32_t)(*value) + 1;
    *value = (uint16_t)result;

    // 更新标志位
    cpu_update_flags_arithmetic(cpu, result, 1, *value);

    printf("INC executed: Value = 0x%04X\n", *value);
}

// DEC 指令 (自减)
void dec(CPU *cpu, uint16_t *value) {
    uint32_t result = (uint32_t)(*value) - 1;
    *value = (uint16_t)result;

    // 更新标志位
    cpu_update_flags_arithmetic(cpu, result, -1, *value);

    printf("DEC executed: Value = 0x%04X\n", *value);
}

// AND 指令 (位与)
void and(CPU *cpu, uint16_t *dest, uint16_t src) {
    *dest &= src;

    // 更新标志位
    cpu_update_flags_logical(cpu, *dest);

    printf("AND executed: Result = 0x%04X\n", *dest);
}

// OR 指令 (位或)
void or(CPU *cpu, uint16_t *dest, uint16_t src) {
    *dest |= src;

    // 更新标志位
    cpu_update_flags_logical(cpu, *dest);

    printf("OR executed: Result = 0x%04X\n", *dest);
}

// XOR 指令 (位异或)
void xor(CPU *cpu, uint16_t *dest, uint16_t src) {
    *dest ^= src;

    // 更新标志位
    cpu_update_flags_logical(cpu, *dest);

    printf("XOR executed: Result = 0x%04X\n", *dest);
}

// TEST 指令 (逻辑与，用于更新标志位)
void test(CPU *cpu, uint16_t value1, uint16_t value2) {
    uint16_t result = value1 & value2;

    // 更新标志位
    cpu_update_flags_logical(cpu, result);

    printf("TEST executed: Result = 0x%04X\n", result);
}


// NOP 指令 (无操作)
void nop() {
    printf("NOP executed: No operation performed.\n");
}

// HLT 指令 (停止执行)
void hlt() {
    printf("HLT executed: Halting execution.\n");
    // 这里可以添加代码来停止 CPU 执行
}

// JMP 指令 (无条件跳转)
void jmp(CPU *cpu, uint16_t address) {
    cpu->IP = address; // 更新指令指针到新的地址

    printf("JMP executed: Jumping to address 0x%04X\n", address);
}

// CALL 指令 (调用子程序)
void call(CPU *cpu, uint16_t address) {
    if (cpu->SP < 2) { // 栈的底部检测
        printf("Error: Stack overflow\n");
        return;
    }
    push(cpu, cpu->IP); // 将当前指令指针压入栈
    cpu->IP = address;  // 跳转到目标地址
    printf("CALL executed: Jumping to address 0x%04X\n", address);
}

// RET 指令 (返回子程序)
void ret(CPU *cpu) {
    if (cpu->SP > STACK_TOP - 2) { // 确保有足够空间弹出 IP
        printf("Error: Stack underflow\n");
        return;
    }
    cpu->IP = pop(cpu); // 恢复 IP
    printf("RET executed: Returning to address 0x%04X\n", cpu->IP);
}

// ADD 指令 (无符号加法)
void add(CPU *cpu, uint16_t *dest, uint16_t src) {
    uint32_t result = (uint32_t)*dest + (uint32_t)src;
    *dest = (uint16_t)result;

    // 更新标志位
    cpu_update_flags_arithmetic(cpu, result, src, *dest);

    // 处理进位标志 CF（无符号加法）
    if (result > 0xFFFF) {
        cpu->FLAGS |= FLAG_CF; // 设置进位标志
    } else {
        cpu->FLAGS &= ~FLAG_CF; // 清除进位标志
    }

    printf("ADD executed: Result = 0x%04X\n", *dest);
}

// SUB 指令 (无符号减法)
void sub(CPU *cpu, uint16_t *dest, uint16_t src) {
    uint32_t result = (uint32_t)*dest - (uint32_t)src;
    *dest = (uint16_t)result;

    // 更新标志位
    cpu_update_flags_arithmetic(cpu, result, -src, *dest);

    // 处理借位标志 CF
    if (*dest > src) {
        cpu->FLAGS |= FLAG_CF; // 设置借位标志
    } else {
        cpu->FLAGS &= ~FLAG_CF; // 清除借位标志
    }

    printf("SUB executed: Result = 0x%04X\n", *dest);
}

//MOV
void mov(CPU *cpu, uint16_t *dest, uint16_t src, AddressingMode mode, const char *instruction,Variable variables[]) {
    char instruction_copy[20];
    char *dest_reg, *src_reg;
    uint16_t src_value = 0; // 用于存储源寄存器或内存的值
    uint16_t *addr = NULL; // 指向目标地址的指针
    char destination_str[50]; // 目标操作数
    char source_str[50];

    // 执行 MOV 操作
    switch (mode) {
        case IMMEDIATE: // 立即数寻址
        {
            uint16_t src = 0;
            if (strcmp(instruction, "MOV AX, DATA") == 0) {
                src = DATA_SEGMENT_START;  // 如果是 DATA，赋值为 DATA_SEGMENT_START 的值
                cpu->AX = (uint16_t)src;  // 将立即数存入 AX
                break;
            }
            // 检查是否是数字（立即数）
            if (sscanf(instruction, "MOV %*[^,], %hu", &src) == 1) {
                // 将立即数存入目标寄存器
                if (strstr(instruction, "AL")) {
                    cpu->AX = (cpu->AX & 0xFF00) | (src & 0xFF);  // 仅更新 AX 的低8位 (AL)
                } else if (strstr(instruction, "AH")) {
                    cpu->AX = (cpu->AX & 0x00FF) | ((src & 0xFF) << 8);  // 仅更新 AX 的高8位 (AH)
                } else if (strstr(instruction, "AX")) {
                    cpu->AX = (uint16_t)src;  // 将立即数存入 AX
                } else if (strstr(instruction, "BX")) {
                    cpu->BX = (uint16_t)src;  // 将立即数存入 BX
                } else if (strstr(instruction, "CX")) {
                    cpu->CX = (uint16_t)src;  // 将立即数存入 CX
                } else if (strstr(instruction, "DX")) {
                    cpu->DX = (uint16_t)src;  // 将立即数存入 DX
                } else {
                    printf("错误: 无效的寄存器名称。\n");
                    return;
                }

                // 更新标志位，基于AX寄存器的值更新
                cpu->ZF = (cpu->AX == 0);  // 零标志，检查AX是否为0
                cpu->SF = (cpu->AX & 0x8000) != 0;  // 符号标志，检查AX的符号位
            }
                // 处理变量名（例如 Num2）
            else if (strstr(instruction, "MOV") && strchr(instruction, ',') && sscanf(instruction, "MOV %*[^,], %s", dest) == 1) {
                for (int i = 0; i < var_count; i++) {
                    if (strcmp(dest, variables[i].name) == 0) {
                        // 将变量值存入目标寄存器
                        if (strstr(instruction, "BX")) {
                            cpu->BX = variables[i].value.word_value;
                        } else {
                            printf("错误: 未识别的寄存器名称。\n");
                            return;
                        }
                        break;
                    }
                }
            } else {
                printf("错误: 无效的立即数格式。\n");
            }

            break;
        }



        case REGISTER: // 寄存器寻址
            // 复制指令字符串，以避免修改原始指令
            strncpy(instruction_copy, instruction, sizeof(instruction_copy) - 1);
            instruction_copy[sizeof(instruction_copy) - 1] = '\0';

            // 使用 strtok 分割指令字符串，提取目标和源寄存器
            strtok(instruction_copy, " ");          // 跳过 "MOV"
            dest_reg = strtok(NULL, ", ");          // 获取目标寄存器
            src_reg = strtok(NULL, ", ");           // 获取源寄存器

            // 检查是否正确提取了寄存器
            if (dest_reg == NULL || src_reg == NULL) {
                printf("错误: 无效的寄存器格式。\n");
                return;
            }

            // 获取源寄存器的值
            if (strcmp(src_reg, "AL") == 0) {
                src = cpu->AX & 0xFF;  // 获取AX的低8位
            } else if (strcmp(src_reg, "AH") == 0) {
                src = (cpu->AX >> 8) & 0xFF;  // 获取AX的高8位
            } else if (strcmp(src_reg, "AX") == 0) {
                src = cpu->AX;  // 直接获取AX寄存器的值
            } else if (strcmp(src_reg, "BX") == 0) {
                src = cpu->BX;
            } else if (strcmp(src_reg, "CX") == 0) {
                src = cpu->CX;
            } else if (strcmp(src_reg, "DX") == 0) {
                src = cpu->DX;
            } else {
                printf("错误: 未识别的源寄存器 %s。\n", src_reg);
                return;
            }

            // 将 src 的值赋给目标寄存器
            if (strcmp(dest_reg, "AL") == 0) {
                cpu->AX = (cpu->AX & 0xFF00) | (src & 0xFF);  // 修改AX的低8位
            } else if (strcmp(dest_reg, "AH") == 0) {
                cpu->AX = (cpu->AX & 0x00FF) | ((src & 0xFF) << 8);  // 修改AX的高8位
            } else if (strcmp(dest_reg, "AX") == 0) {
                cpu->AX = src;  // 直接赋值给AX寄存器
            } else if (strcmp(dest_reg, "BX") == 0) {
                cpu->BX = src;
            } else if (strcmp(dest_reg, "CX") == 0) {
                cpu->CX = src;
            } else if (strcmp(dest_reg, "DX") == 0) {
                cpu->DX = src;
            } else if (strcmp(dest_reg, "DS") == 0) {
                cpu->DS = src;
            }else {
                printf("错误: 未识别的目标寄存器 %s。\n", dest_reg);
                return;
            }

            // 更新标志位
            cpu->ZF = (cpu->AX == 0);  // 零标志，根据AX值判断
            cpu->SF = (cpu->AX & 0x8000) != 0;  // 符号标志，根据AX值判断
            break;



        case DIRECT: // 直接寻址   于11月6日对立即数寻址和寄存器寻址做了进一步改进
            if (sscanf(instruction, "MOV %49[^,], %49s", destination_str, source_str) == 2) {
                // 处理目标 destination，检查是否是内存地址
                if (strstr(destination_str, "[") != NULL && strstr(destination_str, "]") != NULL) {
                    // 目标是内存地址
                    char var_name[50];
                    sscanf(destination_str, "[%49[^]]]", var_name); // 提取变量名
                    memset(var_name + strlen(var_name), 0, sizeof(var_name) - strlen(var_name)); // 清空后续字符

                    bool found = false;
                    for (int i = 0; i < var_count; i++) {
                        // 确保比较时变量名不会有多余的字符
                        memset(variables[i].name + strlen(variables[i].name), 0, sizeof(variables[i].name) - strlen(variables[i].name));
                        if (strncmp(var_name, variables[i].name, sizeof(var_name)) == 0) {
                            addr = &variables[i].value.word_value; // 存储变量地址
                            found = true;
                            break; // 找到后跳出循环
                        }
                    }
                    if (!found) {
                        printf("错误: 未找到变量 '%s'\n", var_name);
                        return; // 返回错误
                    }
                } else {
                    // 目标是寄存器
                    if (strcmp(destination_str, "AL") == 0) {
                        addr = &cpu->AX; // 指向 AL 的地址
                    } else if (strcmp(destination_str, "AX") == 0) {
                        addr = &cpu->AX; // 指向 AX 的地址
                    } else {
                        printf("错误: 无效的目标操作数 '%s'。\n", destination_str);
                        return; // 返回错误
                    }
                }

                // 处理源操作数
                if (strstr(source_str, "[") != NULL && strstr(source_str, "]") != NULL) {
                    // 源操作数是内存地址
                    char var_name[50];
                    sscanf(source_str, "[%49[^]]]", var_name); // 提取变量名
                    memset(var_name + strlen(var_name), 0, sizeof(var_name) - strlen(var_name)); // 清空后续字符

                    bool found = false;
                    for (int i = 0; i < var_count; i++) {
                        // 确保比较时变量名不会有多余的字符
                        memset(variables[i].name + strlen(variables[i].name), 0, sizeof(variables[i].name) - strlen(variables[i].name));
                        if (strncmp(var_name, variables[i].name, sizeof(var_name)) == 0) {
                            src_value = variables[i].value.word_value; // 获取变量值
                            found = true;
                            break; // 找到后跳出循环
                        }
                    }
                    if (!found) {
                        printf("错误: 未找到源操作数 '%s'\n", source_str);
                        return; // 返回错误
                    }
                } else {
                    // 源操作数是寄存器或立即数
                    if (strcmp(source_str, "AL") == 0) {
                        src_value = cpu->AX & 0xFF; // 获取 AL 的值
                    } else if (strcmp(source_str, "AH") == 0) {
                        src_value = (cpu->AX >> 8) & 0xFF; // 获取 AH 的值
                    } else if (strcmp(source_str, "AX") == 0) {
                        src_value = cpu->AX; // 获取 AX 的值
                    } else {
                        // 立即数
                        if (isdigit(source_str[0]) || (source_str[0] == '-' && isdigit(source_str[1]))) {
                            src_value = (uint16_t)atoi(source_str); // 立即数
                        } else {
                            bool found = false; // 重置找到标志
                            for (int i = 0; i < var_count; i++) {
                                if (strcmp(source_str, variables[i].name) == 0) {
                                    src_value = variables[i].value.word_value; // 获取变量值
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                printf("错误: 未找到源操作数 '%s'\n", source_str);
                                return; // 返回错误
                            }
                        }
                    }
                }

                // 将 src_value 存入指定的内存地址
                if (addr != NULL) {
                    *addr = src_value; // 使用 addr 存储值
                } else {
                    printf("错误: addr 未被初始化。\n");
                    return; // 返回错误
                }

                // 更新标志位
                cpu->ZF = (src_value == 0); // 零标志
                cpu->SF = (src_value & 0x8000) != 0; // 符号标志
                cpu->PF = (__builtin_popcount(src_value) % 2) == 0; // 奇偶标志

                printf("直接寻址: 写入内存[0x%04X] = 0x%04X\n", *addr, src_value);
            } else {
                printf("错误: 无法解析操作数。\n");
                return; // 返回错误
            }
            break;


        case INDIRECT: // 寄存器间接寻址
        {
            char *trimmed_instruction = instruction;
            while (*trimmed_instruction == ' ') trimmed_instruction++;
            char dest[50], src[50];

            // 解析指令，获取目标寄存器和源操作数
            if (sscanf(trimmed_instruction, "%*s %49[^,], %49s", dest, src) == 2) {
                // 去除 src 的前导空格
                char *src_ptr = src;
                while (*src_ptr == ' ') src_ptr++;  // 跳过前导空格
                char temp_src[50];
                int j = 0;
                for (int i = 0; src_ptr[i] != '\0'; i++) {
                    if (src_ptr[i] != '[' && src_ptr[i] != ']') {
                        temp_src[j++] = src_ptr[i];
                    }
                }
                temp_src[j] = '\0';  // 添加字符串结束符
                strcpy(src_ptr, temp_src);  // 将处理后的字符串放回 src_ptr

                // 解析间接地址寄存器（保留不变）
                uint16_t address = 0;
                if (strcmp(src_ptr, "BX") == 0) {
                    address = cpu->BX;
                } else if (strcmp(src_ptr, "AX") == 0) {
                    address = cpu->AX;
                } else if (strcmp(src_ptr, "CX") == 0) {
                    address = cpu->CX;
                } else if (strcmp(src_ptr, "AL") == 0) {
                    address = cpu->AX & 0xFF;
                } else if (strcmp(src_ptr, "AH") == 0) {
                    address = (cpu->AX >> 8) & 0xFF;
                } else if (strcmp(src_ptr, "DX") == 0) {
                    address = cpu->CX;  // 使用 SI 作为内存地址
                } else if (strcmp(src_ptr, "SI") == 0) {
                    address = cpu->SI;  // 使用 SI 作为内存地址
                } else if (strcmp(src_ptr, "DI") == 0) {
                    address = cpu->DI;  // 使用 DI 作为内存地址
                } else if (strcmp(src_ptr, "BP") == 0) {
                    address = cpu->BP;  // 使用 BP 作为内存地址
                } else {
                    printf("错误: 未识别的源寄存器 '%s'。\n", src_ptr);
                    return;
                }

                // 处理目标操作数
                uint8_t dest_value = 0;
                if (strcmp(dest, "CX") == 0) {
                    dest_value = cpu->CX;
                } else if (strcmp(dest, "AX") == 0) {
                    dest_value = cpu->AX;
                } else if (strcmp(dest, "BX") == 0) {
                    dest_value = cpu->BX;
                } else if (strcmp(dest, "DX") == 0) {
                    dest_value = cpu->DX;
                } else {
                    printf("错误: 未识别的目标寄存器 '%s'。\n", dest);
                    return;
                }

                // 写入内存地址
                memory[address] = dest_value;
                printf("寄存器间接寻址: 写入内存[0x%04X] = 0x%02X\n", address, dest_value);

                // 更新标志位
                cpu->ZF = (dest_value == 0); // 零标志
                cpu->SF = (dest_value & 0x80) != 0; // 符号标志
                cpu->PF = (__builtin_popcount(dest_value) % 2) == 0; // 奇偶标志
            } else {
                printf("错误: 无效的寄存器间接寻址格式。\n");
            }
        }
            break;

        case BASE_INDEX: // 基址加变址寻址
            if (sscanf(instruction, "MOV %*[^,], %hu", &src) != 1) {
                printf("错误: 无效的基址加变址寻址格式。\n");
                return;
            }
            // 将 src 存入 BX 基址加 SI 变址位置的内存
            memory[cpu->BX + cpu->SI] = src;
            // 更新标志位
            cpu->ZF = (src == 0); // 零标志
            cpu->SF = (src & 0x8000) != 0; // 符号标志
            break;

        case RELATIVE: // 相对寻址
            if (sscanf(instruction, "MOV %*[^,], %hu", &src) != 1) {
                printf("错误: 无效的相对寻址格式。\n");
                return;
            }
            // 将 src 存入相对 IP 偏移的内存位置
            memory[cpu->IP + *dest] = src;
            // 更新标志位
            cpu->ZF = (src == 0); // 零标志
            cpu->SF = (src & 0x8000) != 0; // 符号标志
            break;

        default:
            printf("错误: 未识别的寻址模式。\n");
            return;
    }

}



// 无符号乘法实现
void mul(CPU *cpu, uint16_t src) {
    // 保存当前 AX 的值
    uint32_t result = (uint32_t)cpu->AX * (uint32_t)src;

    // 检查是否溢出
    if (result > 0xFFFF) {
        cpu_set_flag(cpu, FLAG_CF, 1); // 设置进位标志
        ; // 设置进位标志
        result &= 0xFFFF; // 保留低 16 位
    } else {
        cpu_clear_flag(cpu, FLAG_CF); // 清除进位标志
    }

    cpu->AX = (uint16_t)result; // 将结果存回 AX

    // 更新其他标志位
    cpu_update_flags_arithmetic(cpu, result, src, cpu->AX);

    printf("MUL executed: AX = 0x%04X, src = 0x%04X\n", cpu->AX, src);
}

void imul(CPU *cpu, int16_t src) {
    // 保存当前 AX 的值
    int32_t result = (int32_t)cpu->AX * (int32_t)src;

    // 检查是否溢出
    if (result > INT16_MAX || result < INT16_MIN) {
        cpu_set_flag(cpu, FLAG_OF, 1); // 设置溢出标志
        ; // 设置溢出标志
        result = result > INT16_MAX ? INT16_MAX : INT16_MIN; // 限制结果在有效范围
    } else {
        cpu_clear_flag(cpu, FLAG_OF); // 清除溢出标志
    }

    cpu->AX = (int16_t)result; // 将结果存回 AX

    // 更新其他标志位
    cpu_update_flags_arithmetic(cpu, result, src, cpu->AX);

    printf("IMUL executed: AX = 0x%04X, src = %d\n", cpu->AX, src);
}

void Div(CPU *cpu, uint16_t divisor) {
    if (divisor == 0) {
        // 处理除以零错误
        printf("Error: Division by zero\n");
        return;
    }

    // 除法操作：将 AX 除以除数，商存放在 AX，余数存放在 DX
    uint32_t dividend = (uint32_t)cpu->AX; // 确保不出现溢出
    cpu->AX = dividend / divisor; // 商
    cpu->DX = dividend % divisor; // 余数
}


void idiv(CPU *cpu, int16_t divisor) {
    if (divisor == 0) {
        // 处理除以零错误
        printf("Error: Division by zero\n");
        return;
    }

    // 计算商和余数
    int32_t dividend = (int32_t)cpu->AX; // 确保AX是有符号的
    cpu->AX = dividend / divisor; // 商
    cpu->DX = dividend % divisor; // 余数
}

// 字符串操作指令实现
void movsw(CPU *cpu) {
    uint16_t value = *(uint16_t *)&memory[cpu->SI]; // 从内存读取字
    *(uint16_t *)&memory[cpu->DI] = value;          // 写入到目的内存
    cpu->SI += 2;                                   // 更新 SI
    cpu->DI += 2;                                   // 更新 DI
}

// MOVSB 指令 (字节移动)
void movsb(CPU *cpu, uint16_t dest, uint16_t src) {
    memory[dest] = memory[src]; // 将字节从 src 移动到 dest

    printf("MOVSB executed: Src = 0x%04X, Dest = 0x%04X\n", src, dest);
}

void cmpsw(CPU *cpu) {
    uint16_t left = *(uint16_t *)&memory[cpu->SI];
    uint16_t right = *(uint16_t *)&memory[cpu->DI];
    cpu_set_flag(cpu, FLAG_ZF, left == right);
    cpu_set_flag(cpu, FLAG_CF, left < right);
    cpu_set_flag(cpu, FLAG_SF, (left - right) >> 15); // 设置符号标志
}

void cmpsb(CPU *cpu) {
    uint8_t left = memory[cpu->SI];
    uint8_t right = memory[cpu->DI];
    cpu_set_flag(cpu, FLAG_ZF, left == right);
    cpu_set_flag(cpu, FLAG_CF, left < right);
    cpu_set_flag(cpu, FLAG_SF, (left - right) & 0x80); // 设置符号标志
}

void scasw(CPU *cpu) {
    uint16_t right = *(uint16_t *)&memory[cpu->DI];
    cpu_set_flag(cpu, FLAG_ZF, cpu->AX == right);
    cpu_set_flag(cpu, FLAG_CF, cpu->AX < right);
    cpu_set_flag(cpu, FLAG_SF, (cpu->AX - right) >> 15); // 设置符号标志
}

void scasb(CPU *cpu) {
    uint8_t right = memory[cpu->DI];
    cpu_set_flag(cpu, FLAG_ZF, cpu->AL == right);
    cpu_set_flag(cpu, FLAG_CF, cpu->AL < right);
    cpu_set_flag(cpu, FLAG_SF, (cpu->AL - right) & 0x80); // 设置符号标志
}

void lodsw(CPU *cpu) {
    cpu->AX = *(uint16_t *)&memory[cpu->SI]; // 从内存加载字
    cpu->SI += 2;                            // 更新 SI
}

void lodsb(CPU *cpu) {
    cpu->AL = memory[cpu->SI];              // 从内存加载字节
    cpu->SI += 1;                            // 更新 SI
}

void stosw(CPU *cpu) {
    *(uint16_t *)&memory[cpu->DI] = cpu->AX; // 存储到内存
    cpu->DI += 2;                            // 更新 DI
}

void stosb(CPU *cpu) {
    memory[cpu->DI] = cpu->AL;               // 存储到内存
    cpu->DI += 1;                            // 更新 DI
}

// 处理器控制类指令实现
void clc(CPU *cpu) {
    cpu_clear_flag(cpu, FLAG_CF); // 清除进位标志
}

void stc(CPU *cpu) {
    cpu_set_flag(cpu, FLAG_CF, 1); // 设置进位标志
}

void cmc(CPU *cpu) {
    cpu_toggle_flag(cpu, FLAG_CF); // 反转进位标志
}

void clcd(CPU *cpu) {
    cpu_clear_flag(cpu, FLAG_DF); // 清除方向标志
}

void std(CPU *cpu) {
    cpu_set_flag(cpu, FLAG_DF, 1); // 设置方向标志
}

void cli(CPU *cpu) {
    cpu_clear_flag(cpu, FLAG_IF); // 清除中断标志
}

void sti(CPU *cpu) {
    cpu_set_flag(cpu, FLAG_IF, 1); // 设置中断标志
}

// 程序控制类指令实现

void interrupt(CPU *cpu, uint8_t interrupt_number) {
    if (cpu->SP < 4) { // 确保有足够的空间存储 IP 和 FLAGS
        printf("Error: Stack overflow\n");
        return;
    }
    push(cpu, cpu->IP);    // 保存 IP
    push(cpu, cpu->FLAGS); // 保存标志寄存器
    uint16_t vector_address = memory[interrupt_number * 4] | (memory[interrupt_number * 4 + 1] << 8);
    cpu->IP = vector_address;
    printf("INT %d executed: Jumping to vector address 0x%04X\n", interrupt_number, vector_address);
}


void iret(CPU *cpu) {
    if (cpu->SP > STACK_TOP - 4) { // 确保有足够空间弹出 FLAGS 和 IP
        printf("Error: Stack underflow\n");
        return;
    }
    cpu->FLAGS = pop(cpu); // 恢复标志寄存器
    cpu->IP = pop(cpu); // 恢复 IP
}

void loop(CPU *cpu) {
    if (cpu->CX != 0) {
        cpu->CX--; // 减少计数
        cpu->IP++; // 跳转到下一个指令
    }
}

void loopz(CPU *cpu) {
    if (cpu->CX != 0 && cpu->ZF) {
        cpu->CX--; // 减少计数
        cpu->IP++; // 跳转到下一个指令
    }
}

void loopnz(CPU *cpu) {
    if (cpu->CX != 0 && !cpu->ZF) {
        cpu->CX--; // 减少计数
        cpu->IP++; // 跳转到下一个指令
    }
}

// LEA 指令的实现
void lea(CPU *cpu, uint16_t reg, uint16_t addr, AddressingMode mode) {
    uint16_t effective_address;
    switch (mode) {
        case IMMEDIATE:
            effective_address = addr; // 直接使用立即数
            break;
        case REGISTER:
            // 这里假设addr是寄存器编号，实际需要从寄存器中读取地址
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
            effective_address = addr; // 使用直接地址
            break;
        case INDIRECT:
            // 假设 addr 是存放地址的寄存器
            effective_address = memory[cpu->BX]; // 使用 BX 寄存器作为指针
            break;
        case BASE_INDEX:
            effective_address = cpu->BX + cpu->SI; // 基址 + 变址
            break;
        case RELATIVE:
            effective_address = cpu->CS + addr; // 使用 CS 寄存器进行相对寻址
            break;
        default:
            printf("Invalid addressing mode specified for LEA.\n");
            return;
    }
}


void out(CPU *cpu, uint16_t port, uint16_t value) {
    // 输出到指定端口的逻辑
    printf("输出到端口 %u: 值: 0x%04X\n", port, value);
    // 实际的硬件交互逻辑应在此处实现
}
