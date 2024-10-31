// eu.h
#ifndef EU_H
#define EU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // ��Ҫ����stdint.h
#include "cpu.h"
#include "instruction.h"
#include "biu.h"
#include "address.h"


typedef struct {
    char instructions[MAX_CODE][256];
    int count;
} CodeSegment;

extern CodeSegment code_segment; // ʹ�� extern ����


extern int var_count; // ʹ�� extern ����

void parse_segment(const char *input, BIU *biu, CPU *cpu, Variable variables[], int *var_count);/*���������*/
void execute_code(CPU *cpu, BIU *biu, Variable variables[], int var_count);/*ִ��ָ��*/

#endif // EU_H


