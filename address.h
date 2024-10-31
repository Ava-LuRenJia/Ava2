#ifndef ADDRESS_H
#define ADDRESS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "biu.h"

#define MAX_VARS 100
#define MAX_CODE 256

// Ѱַ��ʽö��
typedef enum {
    IMMEDIATE, // ������Ѱַ
    REGISTER,  // �Ĵ���Ѱַ
    DIRECT,    // ֱ��Ѱַ
    INDIRECT,  // �Ĵ������Ѱַ
    BASE_INDEX, // ��ַ�ӱ�ַѰַ
    RELATIVE,  // ���Ѱַ
    INVALID,
    MEMORY,
} AddressingMode;

AddressingMode parse_instruction(char *instruction, const char *op, CPU *cpu, uint16_t *addr, Variable variables[], int var_count);


#endif