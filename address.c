#include "address.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


AddressingMode parse_instruction(char *instruction, const char *op, CPU *cpu, uint16_t *addr, Variable variables[], int var_count) {
    char dest[50], src[50];
    uint16_t value;

    // 去除注释
    char *comment = strchr(instruction, ';');
    if (comment) {
        *comment = '\0';
    }

    // 去除前后的空格
    char *trimmed_instruction = instruction;
    while (*trimmed_instruction == ' ') trimmed_instruction++;

    printf("处理指令: %s\n", trimmed_instruction);

    // 检查指令是否包含操作码
    if (strstr(trimmed_instruction, op) != NULL) {
        // 处理指令格式: MOV dest, src
        if (sscanf(trimmed_instruction, "%*s %49[^,], %49s", dest, src) == 2) {
            // 去除 src 的前导空格
            char *src_ptr = src;
            while (*src_ptr == ' ') src_ptr++;
            if (strcmp(src, "DATA") == 0) {
                return IMMEDIATE; // 立即数寻址模式
            }

            // 处理目标是内存地址的情况: MOV [Num1], AL
            if (strstr(dest, "[") != NULL && strstr(dest, "]") != NULL) {
                // 处理目标是内存地址（即带中括号）
                if (sscanf(dest, "[%[^]]]", dest) == 1) {
                    // 查找变量名
                    for (int i = 0; i < var_count; i++) {
                        if (strcmp(dest, variables[i].name) == 0) {
                            *addr = variables[i].value.word_value;
                            return DIRECT; // 直接寻址模式
                        }
                    }
                }
            }

            // 处理源是内存地址的情况: MOV AL, [Num1]
            if (strstr(src_ptr, "[") != NULL && strstr(src_ptr, "]") != NULL) {
                // 如果 src 是内存地址
                if (sscanf(src_ptr, "[%[^]]]", dest) == 1) {
                    for (int i = 0; i < var_count; i++) {
                        if (strcmp(dest, variables[i].name) == 0) {
                            *addr = variables[i].value.word_value;
                            return DIRECT; // 直接寻址模式
                        }
                    }
                }
            }

            // 处理寄存器间接寻址：MOV [BX], AX
            if (dest[0] == '[' && dest[strlen(dest) - 1] == ']') {
                char reg_name[10];
                sscanf(dest, "[%9[^]]]", reg_name);  // 获取括号内的内容
                // 确保 BX 是在这里识别为寄存器
                if (strcmp(reg_name, "BX") == 0) {
                    *addr = cpu->BX;
                    return INDIRECT;  // 寄存器间接寻址模式
                } else if(strcmp(reg_name, "AX") == 0){
                    *addr = cpu->AX;
                    return INDIRECT;
                } else if(strcmp(reg_name, "CX") == 0){
                    *addr = cpu->CX;
                    return INDIRECT;
                } else if(strcmp(reg_name, "DX") == 0){
                    *addr = cpu->DX;
                    return INDIRECT;
                } else if (strcmp(reg_name, "SI") == 0) {
                    *addr = cpu->SI;
                    return INDIRECT;
                } else if (strcmp(reg_name, "DI") == 0) {
                    *addr = cpu->DI;
                    return INDIRECT;
                } else if (strcmp(reg_name, "BP") == 0) {
                    *addr = cpu->BP;
                    return INDIRECT;
                } else {
                    printf("错误: 未识别的间接寻址寄存器 '%s'。\n", reg_name);
                    return INVALID;
                }
            }

            // 处理寄存器寻址
            if (strcmp(src_ptr, "AX") == 0) {
                *addr = cpu->AX;
                return REGISTER;
            } else if (strcmp(src_ptr, "BX") == 0) {
                *addr = cpu->BX;
                return REGISTER;
            } else if (strcmp(src_ptr, "CX") == 0) {
                *addr = cpu->CX;
                return REGISTER;
            } else if (strcmp(src_ptr, "DX") == 0) {
                *addr = cpu->DX;
                return REGISTER;
            } else if (strcmp(src_ptr, "AL") == 0) {
                *addr = cpu->AX & 0xFF;
                return REGISTER;
            } else if (strcmp(src_ptr, "AH") == 0) {
                *addr = (cpu->AX >> 8) & 0xFF;
                return REGISTER;
            } else if (isdigit(src_ptr[0]) || (src_ptr[0] == '-' && isdigit(src_ptr[1]))) {
                *addr = (uint16_t)atoi(src_ptr);
                return IMMEDIATE;
            } else {
                for (int i = 0; i < var_count; i++) {
                    if (strcmp(src_ptr, variables[i].name) == 0) {
                        *addr = variables[i].value.word_value;
                        return IMMEDIATE; // 将其作为立即数处理
                    }
                }
            }

            // 处理直接地址模式: MOV dest, [1234h]
            if (sscanf(src_ptr, "[%[^]]]", dest) == 1) {
                // 检查是否为寄存器间接寻址（例如 [BX]）
                if (strcmp(dest, "BX") == 0 || strcmp(dest, "SI") == 0 || strcmp(dest, "DI") == 0 ||
                    strcmp(dest, "BP") == 0 || strcmp(dest, "CX") == 0 || strcmp(dest, "AX") == 0 ||
                    strcmp(dest, "DX") == 0) {
                    // 寄存器间接寻址
                    *addr = cpu->BX;  // 这里可以根据寄存器动态决定
                    return INDIRECT;
                }
            }
            // 处理基址加变址模式: MOV dest, [BX+SI]
            if (sscanf(src_ptr, "[%[^+]+%hx]", dest, &value) == 2) {
                *addr = cpu->BX + value;
                return BASE_INDEX;
            }

            // 处理相对寻址模式: JMP LABEL
            if (strstr(trimmed_instruction, "JMP") != NULL) {
                char label[50];
                if (sscanf(trimmed_instruction, "JMP %s", label) == 1) {
                    *addr = atoi(label);
                    return RELATIVE;
                }
            }

            // 处理寄存器到寄存器的转移: MOV DS, AX
            if (strcmp(dest, "DS") == 0 && strcmp(src_ptr, "AX") == 0) {
                cpu->DS = cpu->AX; // 更新 DS
                return REGISTER;
            }

            // 处理其他寄存器到寄存器的转移: MOV AX, BX
            if (strcmp(dest, "AX") == 0 && strcmp(src_ptr, "BX") == 0) {
                cpu->AX = cpu->BX; // AX = BX
                return REGISTER;
            }
        }
    }

    printf("未识别的指令: %s\n", trimmed_instruction);
    return INVALID; // 处理不匹配情况
}










