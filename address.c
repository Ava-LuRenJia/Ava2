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

            // 处理段地址
            if (strcmp(src_ptr, "DATA") == 0) {
                *addr = 0x2000;  // 假设 DATA 段在地址 0x2000
                return DIRECT;
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

            // 处理直接寻址: MOV dest, [Num1]
            if (sscanf(src_ptr, "[%[^]]]", dest) == 1) {
                for (int i = 0; i < var_count; i++) {
                    if (strcmp(dest, variables[i].name) == 0) {
                        *addr = variables[i].value.word_value;
                        return DIRECT;
                    }
                }
            }

            // 处理直接地址模式: MOV dest, [1234h]
            if (sscanf(src_ptr, "[%hx]", &value) == 1) {
                *addr = value;
                return DIRECT;
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









