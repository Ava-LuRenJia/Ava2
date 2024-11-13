#include <stdio.h>
#include <string.h>
#include "biu.h"

uint8_t memory[1024 * 1024]; // 实际定义 memory

// 初始化BIU
void init_biu(BIU *biu) {
    biu->front = 0;
    biu->rear = 0;
    memset(memory, 0, sizeof(memory)); // 初始化内存
}

// 检查队列是否已满，通过计算 rear + 1 与 front 的模值来判断
bool is_queue_full(BIU *biu) {
    return ((biu->rear + 1) % QUEUE_SIZE == biu->front);
}

// 检查队列是否为空
bool is_queue_empty(BIU *biu) {
    return (biu->front == biu->rear);
}

// 向BIU队列中加入指令
bool enqueue_instruction(BIU *biu, const char *instruction) {
    if (is_queue_full(biu)) {
        printf("警告：指令队列已满，当前容量: %d，前指针: %d，后指针: %d\n", QUEUE_SIZE, biu->front, biu->rear);
        return false;
    }

    // 确保指令长度不超过队列中的最大长度
    if (strlen(instruction) >= 50) {
        printf("错误：指令过长，无法加入队列\n");
        return false;
    }

    // 允许重复指令直接加入队列，使用 strcpy 将指令复制到队列的当前位置，然后更新 rear 指针，移动到下一个位置
    printf("Enqueuing instruction: %s\n", instruction); // 添加调试信息
    strcpy(biu->instruction_queue[biu->rear], instruction);
    biu->rear = (biu->rear + 1) % QUEUE_SIZE;
    return true;
}

// 从BIU队列中取出指令
bool dequeue_instruction(BIU *biu, char *instruction) {
    if (is_queue_empty(biu)) {
        printf("警告：指令队列为空，无法取出指令\n");
        return false;
    }
    strcpy(instruction, biu->instruction_queue[biu->front]);
    biu->front = (biu->front + 1) % QUEUE_SIZE;
    printf("Dequeued instruction: %s\n", instruction); // 添加调试信息
    return true;
}

uint16_t find_variable_address(Variable variables[], int var_count, const char *var_name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, var_name) == 0) {
            // 返回变量在数组中的索引作为地址
            return (uint16_t)(i + 0x1000); // 假设从0x1000开始分配地址
        }
    }
    return 0; // 找不到变量返回0
}






