// eu.h
#ifndef EU_H
#define EU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // 需要包含stdint.h
#include "cpu.h"
#include "instruction.h"
#include "biu.h"
#include "address.h"


typedef struct {
    char instructions[MAX_CODE][256];
    int count;
} CodeSegment;

extern CodeSegment code_segment; // 使用 extern 声明


extern int var_count; // 使用 extern 声明

void parse_segment(const char *input, BIU *biu, CPU *cpu, Variable variables[], int *var_count);/*解析代码段*/
void execute_code(CPU *cpu, BIU *biu, Variable variables[], int var_count);/*执行指令*/

#endif // EU_H


