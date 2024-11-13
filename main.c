#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "biu.h"
#include "eu.h"
#include "Chip2164.h"

#define MAX_VARIABLES 100 // 定义最大变量数量
#define MAX_INPUT_SIZE 1024 // 定义输入大小

int main() {
    CPU cpu;
    BIU biu;
    Variable variables[MAX_VARIABLES]; // 变量数组

    cpu_init(&cpu);
    init_biu(&biu);

    printf("请输入汇编指令，每行一条，输入 'END START' 结束输入:\n");

    char input[MAX_INPUT_SIZE] = {0};
    char temp[50];

    // 收集所有输入指令
    while (1) {
        fgets(temp, sizeof(temp), stdin);
        temp[strcspn(temp, "\n")] = '\0'; // 移除换行符

        // 检查结束标志
        if (strcmp(temp, "END START") == 0) {
            break;
        }
        if (strlen(temp) == 0) {
            continue; // 跳过空行
        }

        // 确保不会超出输入大小
        if (strlen(input) + strlen(temp) + 2 < MAX_INPUT_SIZE) {
            snprintf(input + strlen(input), sizeof(input) - strlen(input), "%s\n", temp);
        } else {
            printf("输入过长，无法继续。\n");
            break;
        }
    }

    // 调用 parse_segment 解析输入的汇编代码
    parse_segment(input, &biu, &cpu, variables, &var_count);

    // 执行队列中所有指令
    execute_code(&cpu, &biu, variables, var_count);

    printf("程序执行完毕。\n");

    return 0;
}





//以下主函数是测试Chip2164.c能不能正常运行
/*
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "Chip2164.h"

#define MEMORY_SIZE 65536 // 64K x 1 位的存储容量

int main() {
    Memory2164 memory;
    initMemory2164(&memory);

    // 写入数据示例
    uint16_t address_to_write = 0x01A0; // 要写入的地址
    uint8_t data_to_write = 0xFF;        // 要写入的数据
    write_2164(&memory, address_to_write, data_to_write);
    printf("已写入数据: %02X 到地址: %04X\n", data_to_write, address_to_write);

    // 读取数据示例
    uint16_t address_to_read = 0x01A0; // 要读取的地址
    uint8_t read_data_value = read_2164(&memory, address_to_read);
    printf("从地址: %04X 读取到数据: %02X\n", address_to_read, read_data_value);

    // 尝试读取一个未写入过的地址
    uint16_t new_address_to_read = 0x01A1; // 要读取的地址
    uint8_t new_read_data_value = read_2164(&memory, new_address_to_read);
    printf("从地址: %04X 读取到数据: %02X\n", new_address_to_read, new_read_data_value);

    return 0;
}
*/


/*
#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "biu.h"
#include "eu.h"
#include "Chip2164.h"
#include "test5.h"

#define MAX_VARIABLES 100
#define MAX_INPUT_SIZE 2048

int main() {
    CPU cpu;
    BIU biu;
    Memory2164 memory;
    Variable variables[MAX_VARIABLES];
    int var_count = 0;

    cpu_init(&cpu);
    init_biu(&biu);
    initMemory2164(&memory);

    printf("请输入汇编指令，每行一条，输入 'END START' 结束输入:\n");

    char input[MAX_INPUT_SIZE] = {0};
    char temp[50];

    while (1) {
        fgets(temp, sizeof(temp), stdin);
        temp[strcspn(temp, "\n")] = '\0';

        if (strcmp(temp, "END START") == 0) {
            break;
        }
        if (strlen(temp) == 0) {
            continue;
        }

        if (strlen(input) + strlen(temp) + 2 < MAX_INPUT_SIZE) {
            snprintf(input + strlen(input), sizeof(input) - strlen(input), "%s\n", temp);
        } else {
            printf("输入过长，无法继续。\n");
            break;
        }
    }

    parse_segment(input, &biu, &cpu, variables, &var_count);

    printf("开始执行指令...\n");
    printf("--------------------------------------------------------\n");
    char instruction[50];
    int instruction_count = 0;

    // 执行指令队列
    while (dequeue_instruction(&biu, instruction)) {
        if (strstr(instruction, "READ") != NULL) {
            uint16_t address = 0;
            if (sscanf(instruction, "READ %hu", &address) == 1) {
                // 地址总线发送到 2164，并经过地址译码后传输给行地址和列地址
                uint8_t data = read_2164(&memory, address);
                printf("读取数据: %02X\n", data);
            } else {
                printf("READ指令格式错误: %s\n", instruction);
            }
        } else if (strstr(instruction, "WRITE") != NULL) {
            uint16_t address = 0;
            uint8_t data = 0;
            if (sscanf(instruction, "WRITE %hu %hhu", &address, &data) == 2) {
                // 地址总线发送到 2164，并经过地址译码后传输给行地址和列地址
                write_2164(&memory, address, data);
                printf("写入数据: %02X 到地址 %04X\n", data, address);
            } else {
                printf("WRITE指令格式错误: %s\n", instruction);
            }
        } else {
            // 处理其他指令
            execute_code(&cpu, &biu, variables, var_count);
        }

        printf("已执行指令: %s\n", instruction);
        instruction_count++;
    }

    printf("总共执行了 %d 条指令\n", instruction_count);

    return 0;
}*/
