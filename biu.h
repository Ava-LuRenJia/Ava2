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
#define QUEUE_SIZE 256 // 设置队列大小
#define CODE_SEGMENT_START 0x1000 // 示例地址
#define DATA_SEGMENT_START 0x2000 // 示例地址
#define STACK_SEGMENT_START 0x3000 // 示例地址
#define DATA DATA_SEGMENT_START


typedef struct {
    char name[50];               // 变量名称
    int type;                    // 0 = byte (DB), 1 = word (DW)
    uint16_t address;            // 变量的内存地址
    union {
        uint8_t byte_value;      // 单字节值 (DB)
        uint16_t word_value;     // 双字节值 (DW)
        uint8_t byte_array[256]; // 字节数组 (支持 DB 初始化的多个字节)
        uint16_t word_array[256];// 字数组 (支持 DW 初始化的多个字)
    } value;
} Variable;


typedef struct {
    const char *label;   // 标签名称
    uint16_t address;    // 标签对应的地址
} LabelAddress;



extern int var_count;

extern Variable variables[MAX_VARS]; // 使用 extern 声明

extern uint8_t memory[MEMORY_SIZE]; // 声明 memory 为外部变量

// BIU的指令队列结构
typedef struct {
    char instruction_queue[QUEUE_SIZE][50]; // 每个指令最大长度为50字符
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





