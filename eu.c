#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "biu.h"
#include "cpu.h"
#include "eu.h"
#include "Chip2164.h"
#define MAX_CODE 256

Variable variables[100];
int var_count = 0; // 当前变量的数量

/**(Variable(*)[100])(variables)用来查询数组情况*/

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

        // 跳过空行或注释行
        if (line[0] == ';' || strlen(line) == 0) {
            ptr = strtok(NULL, "\n");
            continue;
        }

        // 去除行内注释
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
        }

        // 解析数据段中的变量
        if (in_data_segment) {
            char var_name[50], var_type[10];
            int value;
            char str_value[256];

            // 解析单个字节或字变量
            if (sscanf(line, "%s %s %d", var_name, var_type, &value) == 3) {
                strcpy(variables[*var_count].name, var_name);
                if (strcmp(var_type, "DB") == 0) {
                    variables[*var_count].value.byte_value = (uint8_t)value;
                    variables[*var_count].type = 0; // Byte
                } else if (strcmp(var_type, "DW") == 0) {
                    variables[*var_count].value.word_value = (uint16_t)value;
                    variables[*var_count].type = 1; // Word
                }
                printf("解析变量: 名称=%s, 类型=%s, 值=%d, 地址=%04X\n", variables[*var_count].name, var_type, variables[*var_count].value.word_value, DATA_SEGMENT_START + *var_count);
                (*var_count)++;
            }
                // 处理字符串或多个字节初始化
            else if (sscanf(line, "%s %s %[^\n]", var_name, var_type, str_value) == 3) {
                strcpy(variables[*var_count].name, var_name);
                int i = 0;

                if (strcmp(var_type, "DB") == 0) {
                    variables[*var_count].type = 0; // Byte

                    // 检查是否为带引号的字符串
                    if (str_value[0] == '\'') {
                        for (int j = 1; j < strlen(str_value) - 1 && i < 256; j++) {
                            if (str_value[j] != '\'') {
                                variables[*var_count].value.byte_array[i++] = (uint8_t)str_value[j];
                            }
                        }
                    } else { // 解析多个逗号分隔的数值
                        char *token = strtok(str_value, ", ");
                        while (token != NULL && i < 256) {
                            variables[*var_count].value.byte_array[i++] = (uint8_t)atoi(token);
                            token = strtok(NULL, ", ");
                        }
                    }
                    printf("解析变量数组: 名称=%s, 类型=DB, 字节数=%d\n", var_name, i);
                }
                else if (strcmp(var_type, "DW") == 0) {
                    variables[*var_count].type = 1; // Word

                    char *token = strtok(str_value, ", ");
                    while (token != NULL && i < 256) {
                        variables[*var_count].value.word_array[i++] = (uint16_t)atoi(token);
                        token = strtok(NULL, ", ");
                    }
                    printf("解析变量数组: 名称=%s, 类型=DW, 字数=%d\n", var_name, i);
                }
                (*var_count)++;
            }
        }

        // 在代码段中入队指令
        if (in_code_segment && found_start) {
            if (strlen(line) > 0) {
                if (!strstr(line, "PROC") && !strstr(line, "ENDP") && !strstr(line, "START:")) {
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

            // 保留原始指令以便后续使用
            char original_instruction[50];
            strcpy(original_instruction, instruction);

            char *instr_name = strtok(instruction, " ");
            uint16_t dest = 0, src = 0;
            AddressingMode mode;

            if (strcmp(instr_name, "MOV") == 0) {
                mode = parse_instruction(original_instruction, "MOV", cpu, &dest, variables, var_count);
                if (mode != INVALID) {
                    mov(cpu, &dest, src, mode, original_instruction,variables);
                } else {
                    printf("MOV指令解析失败: %s\n", original_instruction);
                }

            } else if (strcmp(instr_name, "LEA") == 0) {
                mode = parse_instruction(original_instruction, "LEA", cpu, &dest, variables, var_count);
                if (mode == MEMORY) {
                    dest = find_variable_address(variables, var_count, strtok(NULL, " "));
                    if (dest != 0) {
                        cpu->SI = dest;  // 将解析到的地址存入 SI 寄存器
                        printf("LEA 执行，地址存入 SI: %u\n", dest);
                    } else {
                        printf("LEA指令格式错误: %s\n", original_instruction);
                    }
                }

            } else if (strcmp(instr_name, "PUSH") == 0) {
                uint16_t value = (strstr(original_instruction, "AX") ? cpu->AX : cpu->BX);
                printf("压入值: 0x%04X\n", value);
                push(cpu, value);

            } else if (strcmp(instr_name, "POP") == 0) {
                uint16_t value = pop(cpu);
                if (strstr(original_instruction, "AX")) cpu->AX = value;
                else if (strstr(original_instruction, "BX")) cpu->BX = value;
                printf("弹出值: 0x%04X\n", value);

            } else if (strcmp(instr_name, "CALL") == 0) {
                uint16_t address = 0;
                if (sscanf(original_instruction, "CALL %hx", &address) == 1) {
                    call(cpu, address);
                } else {
                    printf("CALL指令格式错误: %s\n", original_instruction);
                }

            } else if (strcmp(instr_name, "RET") == 0) {
                ret(cpu);

            } else if (strcmp(instr_name, "INT") == 0) {
                uint8_t interrupt_number = 0;
                if (sscanf(original_instruction, "INT %hhu", &interrupt_number) == 1) {
                    interrupt(cpu, interrupt_number);
                } else {
                    printf("INT指令格式错误: %s\n", original_instruction);
                }

            } else if (strcmp(instr_name, "OUT") == 0) {
                uint16_t port = 0, value = 0;
                if (sscanf(original_instruction, "OUT %hu, %hu", &port, &value) == 2) {
                    out(cpu, port, value);
                } else {
                    printf("OUT指令格式错误: %s\n", original_instruction);
                }

            } else if (strcmp(instr_name, "READ") == 0) {
                uint16_t address = 0;
                if (sscanf(original_instruction, "READ %hu", &address) == 1) {
                    uint8_t data = read_2164(&memory, address);
                    printf("读取数据: %02X\n", data);
                } else {
                    printf("READ指令格式错误: %s\n", original_instruction);
                }

            } else if (strcmp(instr_name, "WRITE") == 0) {
                uint16_t address = 0;
                uint8_t data = 0;
                if (sscanf(original_instruction, "WRITE %hu %hhu", &address, &data) == 2) {
                    write_2164(&memory, address, data);
                    printf("写入数据: %02X 到地址 %04X\n", data, address);
                } else {
                    printf("WRITE指令格式错误: %s\n", original_instruction);
                }

            } else {
                printf("未识别的指令: %s\n", original_instruction);
            }

            cpu_print_state(cpu);
            printf("--------------------------------------------------------\n");
        }
    }
}










