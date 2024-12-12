#include <stdio.h>
#include <string.h>
#include "chip8255.h"
#include "cpu.h"
#include "biu.h"
#include "eu.h" // 包含 eu.h 头文件

#define MAX_INPUT_SIZE 1024 // 定义输入大小
#define MAX_INSTRUCTION_COUNT 100

// 定义指令结构
typedef struct {
    char instruction[50]; // 存储指令（如 MOV AL, 10）
} AssemblyInstruction;

// 模拟汇编指令执行的函数
void simulate_assembly(Chip8255 *chip1, Chip8255 *chip2,CPU *cpu, BIU *biu, Variable variables[], int var_count) {
    printf("请输入汇编指令，每行一条，输入 'END START' 结束输入（输入AH代表自动选择高地址，AL代表自动选择低地址）:\n");

    char input[MAX_INPUT_SIZE] = {0};  // 存储汇编指令
    char temp[50];  // 临时存储输入的一行指令
    AssemblyInstruction instructions[MAX_INSTRUCTION_COUNT];
    int sum = 0;

    // 收集所有输入指令
    while (1) {
        // 使用 fgets 读取一行输入
        if (fgets(temp, sizeof(temp), stdin) == NULL) {
            break;
        }

        // 去除换行符
        temp[strcspn(temp, "\n")] = '\0'; // 移除换行符

        // 去除前后的空格
        char *start = temp;
        while (*start == ' ' || *start == '\t') {
            start++;  // 跳过前导空格
        }

        char *end = start + strlen(start) - 1;
        while (end > start && (*end == ' ' || *end == '\t')) {
            end--;  // 跳过尾部空格
        }
        *(end + 1) = '\0';  // 终止符添加在尾部

        // 检查结束标志
        if (strcmp(start, "END START") == 0) {
            break;
        }
        if (strlen(start) == 0) {
            continue; // 跳过空行
        }

        // 确保不会超出输入大小
        if (strlen(input) + strlen(start) + 2 < MAX_INPUT_SIZE) {
            snprintf(input + strlen(input), sizeof(input) - strlen(input), "%s\n", start);
        } else {
            printf("输入过长，无法继续。\n");
            break;
        }

        // 检查 MOV 指令
        if (strncmp(start, "MOV", 3) == 0) {
            char reg[3]; // 存储寄存器名
            int data;

            // 使用 sscanf 解析 MOV 指令后的寄存器和数据
            if (sscanf(start + 4, "%2s, %d", reg, &data) == 2) {
                // 如果解析成功（即找到有效的寄存器和数据）
                strcpy(instructions[sum].instruction, start);  // 存储指令
                sum++;  // 增加指令计数
            }
        }
    }

    parse_segment(input, biu, cpu, variables, &var_count);
    execute_code(cpu, biu, variables, var_count);

    // 处理 PORT_A 和 PORT_B 的指令
// 修改 simulate_assembly 中处理 MOV 指令部分
    for (int i = 0; i < sum; i++) {
        printf("处理指令: %s\n", instructions[i].instruction);

        if (strncmp(instructions[i].instruction, "MOV", 3) == 0) {
            char reg[3];
            int data;
            sscanf(instructions[i].instruction + 4, "%2s, %d", reg, &data);

            if (strcmp(reg, "AH") == 0) {
                chip2->wr = true;  // 写操作
                chip2->cs = 0000;
                chip8255_write(chip2, PORT_A, (uint8_t)data);
                printf("控制 高地址芯片的PORT_A: 写入 %d\n", data);
                chip2->wr = false;  // 写操作
            } else if (strcmp(reg, "BH") == 0) {
                chip2->wr = true;  // 写操作
                chip2->cs = 0000;
                chip8255_write(chip2, PORT_B, (uint8_t)data);
                printf("控制 高地址芯片的PORT_B: 写入 %d\n", data);
                chip2->wr = false;  // 写操作
            }

            if (strcmp(reg, "AL") == 0) {
                chip1->wr = true;  // 写操作
                chip1->cs = 0001;
                chip8255_write(chip1, PORT_A, (uint8_t)data);
                printf("控制 低地址芯片的PORT_A: 写入 %d\n", data);
                chip1->wr = false;  // 写操作
            } else if (strcmp(reg, "BL") == 0) {
                chip1->wr = true;  // 写操作
                chip1->cs = 0001;
                chip8255_write(chip1, PORT_B, (uint8_t)data);
                printf("控制 低地址芯片的PORT_B: 写入 %d\n", data);
                chip1->wr = false;  // 写操作
            }

        }
    }
    printf("请输入小灯状态，1代表打开，0代表关闭: \n");
    int light_on;
    scanf("%d", &light_on);

if(light_on == 0){
        printf("灯已关闭\n");
    }
if(chip1->cs == 0001 && light_on == 1) {
    chip1->rd = true;  // 读操作
    int light_intensity_low = chip8255_read(chip1, PORT_A) + chip8255_read(chip1, PORT_B);
    printf("当前是低地址芯片\n");
    printf("灯已打开，当前光强: %d\n", light_intensity_low);
    chip1->rd = false;  // 读操作
}
if (chip2->cs == 0000 && light_on == 1) {
        chip2->rd = true;  // 读操作
        int light_intensity_high = chip8255_read(chip2, PORT_A) + chip8255_read(chip2, PORT_B);
        printf("当前是高地址芯片\n");
        printf("灯已打开，当前光强: %d\n", light_intensity_high);
        chip2->rd = false;  // 读操作
    }
    printf("程序执行完毕。\n");
}

// 主函数
int main() {
    Chip8255 chip1;
    Chip8255 chip2;
    CPU cpu = {0};
    BIU biu = {0};
    chip8255_init(&chip1);
    chip8255_init(&chip2);
    Variable variables[100];
    int var_count = 0;

    simulate_assembly(&chip1, &chip2,&cpu, &biu, variables, var_count);

    return 0;
}

