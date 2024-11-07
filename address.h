#ifndef ADDRESS_H
#define ADDRESS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "biu.h"

#define MAX_VARS 100
#define MAX_CODE 256

// 寻址方式枚举
typedef enum {
    IMMEDIATE, // 立即数寻址
    REGISTER,  // 寄存器寻址
    DIRECT,    // 直接寻址
    INDIRECT,  // 寄存器间接寻址
    BASE_INDEX, // 基址加变址寻址
    RELATIVE,  // 相对寻址
    INVALID,
    MEMORY,
} AddressingMode;

parse_instruction(char *instruction, const char *op, CPU *cpu, uint16_t *addr, Variable variables[], int var_count);


#endif
