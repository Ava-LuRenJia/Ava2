#include "address.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


AddressingMode parse_instruction(char *instruction, const char *op, CPU *cpu, uint16_t *addr, Variable variables[], int var_count) {
    char dest[50], src[50];
    uint16_t value;

    // ȥ��ע��
    char *comment = strchr(instruction, ';');
    if (comment) {
        *comment = '\0';
    }

    // ȥ��ǰ��Ŀո�
    char *trimmed_instruction = instruction;
    while (*trimmed_instruction == ' ') trimmed_instruction++;

    printf("����ָ��: %s\n", trimmed_instruction);

    // ���ָ���Ƿ����������
    if (strstr(trimmed_instruction, op) != NULL) {
        // ����ָ���ʽ: MOV dest, src
        if (sscanf(trimmed_instruction, "%*s %49[^,], %49s", dest, src) == 2) {
            // ȥ�� src ��ǰ���ո�
            char *src_ptr = src;
            while (*src_ptr == ' ') src_ptr++;

            // ����ε�ַ
            if (strcmp(src_ptr, "DATA") == 0) {
                *addr = 0x2000;  // ���� DATA ���ڵ�ַ 0x2000
                return DIRECT;
            }

            // ����Ĵ���Ѱַ
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
                        return IMMEDIATE; // ������Ϊ����������
                    }
                }
            }

            // ����ֱ��Ѱַ: MOV dest, [Num1]
            if (sscanf(src_ptr, "[%[^]]]", dest) == 1) {
                for (int i = 0; i < var_count; i++) {
                    if (strcmp(dest, variables[i].name) == 0) {
                        *addr = variables[i].value.word_value;
                        return DIRECT;
                    }
                }
            }

            // ����ֱ�ӵ�ַģʽ: MOV dest, [1234h]
            if (sscanf(src_ptr, "[%hx]", &value) == 1) {
                *addr = value;
                return DIRECT;
            }

            // �����ַ�ӱ�ַģʽ: MOV dest, [BX+SI]
            if (sscanf(src_ptr, "[%[^+]+%hx]", dest, &value) == 2) {
                *addr = cpu->BX + value;
                return BASE_INDEX;
            }

            // �������Ѱַģʽ: JMP LABEL
            if (strstr(trimmed_instruction, "JMP") != NULL) {
                char label[50];
                if (sscanf(trimmed_instruction, "JMP %s", label) == 1) {
                    *addr = atoi(label);
                    return RELATIVE;
                }
            }

            // ����Ĵ������Ĵ�����ת��: MOV DS, AX
            if (strcmp(dest, "DS") == 0 && strcmp(src_ptr, "AX") == 0) {
                cpu->DS = cpu->AX; // ���� DS
                return REGISTER;
            }

            // ���������Ĵ������Ĵ�����ת��: MOV AX, BX
            if (strcmp(dest, "AX") == 0 && strcmp(src_ptr, "BX") == 0) {
                cpu->AX = cpu->BX; // AX = BX
                return REGISTER;
            }
        }
    }

    printf("δʶ���ָ��: %s\n", trimmed_instruction);
    return INVALID; // ����ƥ�����
}









