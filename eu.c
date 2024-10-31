#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "biu.h"
#include "cpu.h"
#include "eu.h"
#include "Chip2164.h"
#define MAX_CODE 256

// 修剪字符串两端的空白
void trim_whitespace(char *str) {
    char *start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    if (*start == 0) {
        str[0] = '\0';
        return;
    }

    memmove(str, start, strlen(start) + 1);
}


void parse_segment(const char *input, BIU *biu, CPU *cpu, Variable variables[], int *var_count) {
    char line[256];
    int in_data_segment = 0, in_code_segment = 0, found_start = 0;

    char *input_copy = strdup(input);
    char *ptr = strtok(input_copy, "\n");

    while (ptr != NULL) {
        strcpy(line, ptr);
        trim_whitespace(line);

        if (line[0] == ';' || strlen(line) == 0) {
            ptr = strtok(NULL, "\n");
            continue;
        }

        char *comment = strchr(line, ';');
        if (comment != NULL) {
            *comment = '\0';
        }

        trim_whitespace(line);

        // 处理段定义
        if (strstr(line, "DATA SEGMENT")) {
            in_data_segment = 1;
            printf("进入数据段。\n");
        } else if (strstr(line, "DATA ENDS")) {
            in_data_segment = 0;
            printf("退出数据段。\n");
        } else if (strstr(line, "CODE SEGMENT")) {
            in_code_segment = 1;
            printf("进入代码段。\n");
        } else if (strstr(line, "CODE ENDS")) {
            in_code_segment = 0;
            printf("退出代码段。\n");
        } else if (strstr(line, "START:")) {
            found_start = 1;
            printf("找到 START 标签\n");
            cpu->CS = CODE_SEGMENT_START;
            cpu->DS = DATA_SEGMENT_START;
            cpu->SS = STACK_SEGMENT_START;
            printf("段寄存器已初始化: CS=%04X, DS=%04X, SS=%04X\n", cpu->CS, cpu->DS, cpu->SS);
        } else if (strstr(line, "PROC") != NULL) {
            // 处理过程定义
            printf("找到过程定义: %s\n", line);
            // 这里可以将过程名称或其标识存储在一个列表中，如果需要后续处理
            // enqueue_instruction(biu, line);  // 如果需要将过程标记入队
        } else if (strstr(line, "ENDP") != NULL) {
            // 处理过程结束
            printf("找到过程结束: %s\n", line);
            // 这里可以进行相应的处理，比如从列表中移除当前过程
        }

        // 解析数据段中的变量
        if (in_data_segment) {
            char var_name[50], var_type[10];
            int value = 0;

            if (sscanf(line, "%s %s %d", var_name, var_type, &value) == 3) {
                if (strcmp(var_type, "DB") == 0 || strcmp(var_type, "DW") == 0) {
                    strcpy(variables[*var_count].name, var_name);
                    if (strcmp(var_type, "DB") == 0) {
                        variables[*var_count].value.byte_value = (uint8_t)value;
                        variables[*var_count].type = 0;
                    } else {
                        variables[*var_count].value.word_value = (uint16_t)value;
                        variables[*var_count].type = 1;
                    }
                    printf("解析变量: 名称=%s, 类型=%s, 值=%d, 地址=%04X\n", var_name, var_type, value, DATA_SEGMENT_START + *var_count);
                    (*var_count)++;
                }
            } else if (strstr(line, "DB") != NULL || strstr(line, "DW") != NULL) {
                char str_value[256];
                if (sscanf(line, "%s %*s %[^\n]", var_name, str_value) == 2) {
                    strcpy(variables[*var_count].name, var_name);
                    if (strstr(line, "DB") != NULL) {
                        variables[*var_count].value.byte_value = (uint8_t)atoi(str_value);
                        variables[*var_count].type = 0;
                    } else {
                        variables[*var_count].value.word_value = (uint16_t)atoi(str_value);
                        variables[*var_count].type = 1;
                    }
                    printf("解析变量: 名称=%s, 值=%d, 地址=%04X\n", var_name,
                           (variables[*var_count].type == 0) ? variables[*var_count].value.byte_value : variables[*var_count].value.word_value,
                           DATA_SEGMENT_START + *var_count);
                    (*var_count)++;
                }
            }
        }

        // 在代码段中入队指令
        if (in_code_segment && found_start) {
            if (strlen(line) > 0) {
                if (!strstr(line, "PROC") && !strstr(line, "ENDP")) {
                    if (enqueue_instruction(biu, line)) {
                        printf("已入队指令: %s\n", line);
                    } else {
                        printf("无法将指令入队: %s\n", line);
                    }
                }
            }
        }

        ptr = strtok(NULL, "\n");
    }

    free(input_copy);
}


void execute_code(CPU *cpu, BIU *biu, Variable variables[], int var_count) {
    char instruction[50];
    printf("开始执行指令...\n");
    printf("--------------------------------------------------------\n");

    while (!is_queue_empty(biu)) {
        if (dequeue_instruction(biu, instruction)) {
            if (instruction[0] == ';' || strlen(instruction) == 0) {
                continue; // 跳过注释或空指令
            }
            uint16_t dest = 0, src = 0;

            // 处理 MOV 指令
            if (strstr(instruction, "MOV") != NULL) {
                AddressingMode mode = parse_instruction(instruction, "MOV", cpu, &dest, variables, var_count);
                // 执行 MOV 操作
                mov(cpu, &dest, src, mode, instruction);
            } else if (strstr(instruction, "LEA") != NULL) {
                uint16_t addr = 0;
                AddressingMode mode = parse_instruction(instruction, "LEA", cpu, &addr, variables, var_count);
                if (mode == MEMORY) {
                    // 如果是内存寻址，确保地址有效
                    addr = find_variable_address(variables, var_count, "Str1");
                    if (addr != 0) {
                        cpu->SI = addr;  // 将解析到的地址存入 SI 寄存器
                    }
                }
                printf("LEA 解析，地址: %u\n", addr);
            } else if (strstr(instruction, "PUSH") != NULL) {
                uint16_t value = (strstr(instruction, "AX") ? cpu->AX : cpu->BX);
                printf("压入值: 0x%04X\n", value);
                push(cpu, value); // 实际的 PUSH 逻辑
            } else if (strstr(instruction, "POP") != NULL) {
                uint16_t value = pop(cpu); // 从栈中弹出值
                if (strstr(instruction, "AX")) cpu->AX = value;
                else if (strstr(instruction, "BX")) cpu->BX = value;
                printf("弹出值: 0x%04X\n", value);
            } else if (strstr(instruction, "CALL") != NULL) {
                uint16_t address = 0; // 解析调用的地址
                sscanf(instruction, "CALL %hx", &address); // 从指令中提取地址
                call(cpu, address); // 调用 CALL 函数
            } else if (strstr(instruction, "RET") != NULL) {
                ret(cpu); // 调用 RET 函数
            } else if (strstr(instruction, "INT") != NULL) {
                uint8_t interrupt_number = 0; // 解析中断号
                sscanf(instruction, "INT %hhu", &interrupt_number); // 从指令中提取中断号
                interrupt(cpu, interrupt_number); // 调用中断处理函数
            } else if (strstr(instruction, "OUT") != NULL) {
                uint16_t port = 0, value = 0; // 解析端口和输出值
                sscanf(instruction, "OUT %hu, %hu", &port, &value); // 从指令中提取端口和输出值
                out(cpu, port, value); // 调用输出函数
            } else if (strstr(instruction, "READ") != NULL) {
                uint16_t address = 0;
                if (sscanf(instruction, "READ %hu", &address) == 1) {
                    // 通过 read_2164 函数读取数据
                    uint8_t data = read_2164(&memory, address);
                    printf("读取数据: %02X\n", data);
                } else {
                    printf("READ指令格式错误: %s\n", instruction);
                }
            } else if (strstr(instruction, "WRITE") != NULL) {
                uint16_t address = 0;
                uint8_t data = 0;
                if (sscanf(instruction, "WRITE %hu %hhu", &address, &data) == 2) {
                    // 通过 write_2164 函数写入数据
                    write_2164(&memory, address, data);
                    printf("写入数据: %02X 到地址 %04X\n", data, address);
                } else {
                    printf("WRITE指令格式错误: %s\n", instruction);
                }
            } else {
                printf("未识别的指令: %s\n", instruction);
            }

            cpu_print_state(cpu);
            printf("--------------------------------------------------------\n");
        }
    }
}









