// biu.h
#ifndef BIU_H
#define BIU_H

#include "cpu.h"
#include <stdint.h>
#include <stdbool.h>

#define MEMORY_SIZE (1024 * 1024) // 1MB
#define MAX_VARS 100
#define MAX_CODE 256
#define MAX_INPUT_SIZE 1024
#define QUEUE_SIZE 256 // ���ö��д�С
#define CODE_SEGMENT_START 0x1000 // ʾ����ַ
#define DATA_SEGMENT_START 0x2000 // ʾ����ַ
#define STACK_SEGMENT_START 0x3000 // ʾ����ַ
#define DATA DATA_SEGMENT_START

typedef struct {
    char name[50];       // ��������
    uint16_t address;    // �������ڴ��ַ
    uint8_t type;        // �������ͣ�0��ʾ�ֽڣ�DB����1��ʾ�֣�DW��
    union {
        uint8_t byte_value;   // �洢�ֽ����ͱ�����DB��
        uint16_t word_value;  // �洢�����ͱ�����DW��
    } value;
} Variable;


extern Variable variables[MAX_VARS]; // ʹ�� extern ����

extern uint8_t memory[MEMORY_SIZE]; // ���� memory Ϊ�ⲿ����

// BIU��ָ����нṹ
typedef struct {
    char instruction_queue[QUEUE_SIZE][50]; // ÿ��ָ����󳤶�Ϊ50�ַ�
    int front;
    int rear;
} BIU;

void init_biu(BIU *biu);
bool is_queue_full(BIU *biu);
bool is_queue_empty(BIU *biu);
bool enqueue_instruction(BIU *biu, const char *instruction);
bool dequeue_instruction(BIU *biu, char *instruction);
uint16_t find_variable_address(Variable variables[], int var_count, const char *var_name);
#endif // BIU_H




