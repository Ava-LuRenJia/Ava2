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

// 标签表
LabelAddress label_table[100];
int label_count = 0;  // 标签表中的标签数量

// 解析堆栈段
int parse_stack_segment(const char *line, CPU *cpu) {
    // 解析堆栈段格式，如 "STACK SEGMENT" 和 "DW 128 DUP(0)"
    if (strstr(line, "STACK SEGMENT")) {
        printf("\n");
        printf("进入堆栈段。\n");
        cpu->SS = 0x1000; // 假设堆栈段开始地址为0x1000
    } else if (strstr(line, "STACK ENDS")) {
        printf("退出堆栈段。\n");
        printf("\n");
    } else {
        // 解析堆栈分配的内存，例如 "DW 128 DUP(0)"
        char var_name[50], var_type[10];
        int value, dup_value = 0;

        // 解析 "DW 128 DUP(0)"
        if (sscanf(line, "%s %s %d", var_name, var_type, &value) == 3) {
            // 查找 DUP(0) 部分
            char *dup_start = strstr(line, "DUP(");
            if (dup_start != NULL) {
                // 提取 DUP() 中的数字
                if (sscanf(dup_start, "DUP(%d)", &dup_value) == 1) {
                    printf("堆栈分配: 名称=%s, 类型=%s, 数量=%d\n", var_name, var_type, value);
                    return dup_value; // 返回堆栈分配的大小
                } else {
                    printf("无法解析 DUP 中的数字\n");
                }
            }
        }
    }
    return 0;
}

void parse_segment(const char *input, BIU *biu, CPU *cpu, Variable variables[], int *var_count) {
    char line[256];
    int in_data_segment = 0, in_code_segment = 0, found_start = 0;
    int current_data_address = DATA_SEGMENT_START;  // 初始化数据段地址

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
            char label[50];
            sscanf(line, "%49[^:]", label);  // 提取标签
            // 假设标签的地址在代码段中是递增的
            label_table[label_count].label = strdup(label);
            label_table[label_count].address = cpu->IP;  // 当前 IP 地址作为标签地址
            label_count++;
            printf("找到标签: %s, 地址: 0x%04X\n", label, cpu->IP);
            cpu->CS = CODE_SEGMENT_START;
            cpu->DS = DATA_SEGMENT_START;
            cpu->SS = STACK_SEGMENT_START;
            printf("段寄存器已初始化: CS=%04X, DS=%04X, SS=%04X\n", cpu->CS, cpu->DS, cpu->SS);
        }

        // 处理堆栈段
        int stack_size = parse_stack_segment(line, cpu);
        if (stack_size > 0) {
            printf("堆栈段已分配，大小: %d\n", stack_size);
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

                // 设置变量的地址
                variables[*var_count].address = current_data_address;

                // 打印变量信息
                printf("解析变量: 名称=%s, 类型=%s, 值=%d, 地址=%04X\n", variables[*var_count].name, var_type, variables[*var_count].value.word_value, variables[*var_count].address);

                // 更新数据段地址，按大小递增
                if (variables[*var_count].type == 0) {
                    current_data_address += 1;  // 如果是byte类型，地址加1
                } else if (variables[*var_count].type == 1) {
                    current_data_address += 2;  // 如果是word类型，地址加2
                }

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
                        int len = strlen(str_value) - 1; // 获取字符串长度（不包括引号）

                        // 解析字符串并加入字节数组
                        for (int j = 1; j < len; j++) { // 不包括引号
                            if (i < 256) {
                                variables[*var_count].value.byte_array[i++] = (uint8_t)str_value[j];
                            }
                        }

                        // 添加终止符 '\0'
                        if (i < 256) {
                            variables[*var_count].value.byte_array[i++] = 0; // 添加终止符
                        }

                        // 输出字节数：包括终止符
                        printf("解析变量数组: 名称=%s, 类型=DB, 字节数=%d, 地址=%04X\n", var_name, i, current_data_address);
                    } else { // 解析多个逗号分隔的数值
                        char *token = strtok(str_value, ", ");
                        while (token != NULL && i < 256) {
                            variables[*var_count].value.byte_array[i++] = (uint8_t)atoi(token);
                            token = strtok(NULL, ", ");
                        }

                        // 输出字节数：数值的个数
                        printf("解析变量数组: 名称=%s, 类型=DB, 字节数=%d, 地址=%04X\n", var_name, i, current_data_address);
                    }
                }

                else if (strcmp(var_type, "DW") == 0) {
                    variables[*var_count].type = 1; // Word

                    char *token = strtok(str_value, ", ");
                    while (token != NULL && i < 256) {
                        variables[*var_count].value.word_array[i++] = (uint16_t)atoi(token);
                        token = strtok(NULL, ", ");
                    }
                    printf("解析变量数组: 名称=%s, 类型=DW, 字数=%d, 地址=%04X\n", var_name, i, current_data_address);
                }

                // 设置变量的地址
                variables[*var_count].address = current_data_address;

                // 更新数据段地址，按大小递增
                if (variables[*var_count].type == 0) {
                    current_data_address += i;  // 字节数组的地址按数组长度增加
                } else if (variables[*var_count].type == 1) {
                    current_data_address += 2 * i;  // 字数组的地址按每个元素2字节增加
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
            uint16_t ip_increment = 0;  // 用于保存指令字节数

            if (strcmp(instr_name, "MOV") == 0) {
                mode = parse_instruction(original_instruction, "MOV", cpu, &dest, variables, var_count);
                if (mode != INVALID) {
                    mov(cpu, &dest, src, mode, original_instruction, variables);
                    ip_increment = (mode == IMMEDIATE || mode == DIRECT || mode == RELATIVE || mode == BASE_INDEX) ? 3 : 2;
                } else {
                    printf("MOV指令解析失败: %s\n", original_instruction);
                }

            } else if (strcmp(instr_name, "ADD") == 0) {
                // 解析 ADD 指令
                mode = parse_instruction(original_instruction, "ADD", cpu, &dest, variables, var_count);
                if (mode != INVALID) {
                    // 将 mode 和原指令传递给 add 函数进行处理
                    add(cpu , original_instruction , mode);
                    ip_increment = (mode == IMMEDIATE) ? 3 : 2; // 假设 ADD 指令为 2 字节或 3 字节
                } else {
                    printf("ADD指令解析失败: %s\n", original_instruction);
                }
            }else if (strcmp(instr_name, "SUB") == 0) {
                // 解析 ADD 指令
                mode = parse_instruction(original_instruction, "SUB", cpu, &dest, variables, var_count);
                if (mode != INVALID) {
                    // 将 mode 和原指令传递给 add 函数进行处理
                    sub(cpu , original_instruction , mode);
                    ip_increment = (mode == IMMEDIATE) ? 3 : 2; // 假设 ADD 指令为 2 字节或 3 字节
                } else {
                    printf("SUB指令解析失败: %s\n", original_instruction);
                }
            }else if (strcmp(instr_name, "AND") == 0) {
                // 解析 ADD 指令
                mode = parse_instruction(original_instruction, "AND", cpu, &dest, variables, var_count);
                if (mode != INVALID) {
                    // 将 mode 和原指令传递给 add 函数进行处理
                    and(cpu , original_instruction , mode);
                    ip_increment = (mode == IMMEDIATE) ? 3 : 2; // 假设 ADD 指令为 2 字节或 3 字节
                } else {
                    printf("AND指令解析失败: %s\n", original_instruction);
                }
            }else if (strcmp(instr_name, "OR") == 0) {
                // 解析 ADD 指令
                mode = parse_instruction(original_instruction, "OR", cpu, &dest, variables, var_count);
                if (mode != INVALID) {
                    // 将 mode 和原指令传递给 add 函数进行处理
                    or(cpu , original_instruction , mode);
                    ip_increment = (mode == IMMEDIATE) ? 3 : 2; // 假设 ADD 指令为 2 字节或 3 字节
                } else {
                    printf("OR指令解析失败: %s\n", original_instruction);
                }
            }else if (strcmp(instr_name, "XOR") == 0) {
                // 解析 ADD 指令
                mode = parse_instruction(original_instruction, "XOR", cpu, &dest, variables, var_count);
                if (mode != INVALID) {
                    // 将 mode 和原指令传递给 add 函数进行处理
                    xor(cpu , original_instruction , mode);
                    ip_increment = (mode == IMMEDIATE) ? 3 : 2; // 假设 ADD 指令为 2 字节或 3 字节
                } else {
                    printf("XOR指令解析失败: %s\n", original_instruction);
                }
            }else if (strcmp(instr_name, "NOT") == 0) {
                not(cpu , original_instruction);
            }else if (strcmp(instr_name, "MOVSB") == 0) {
                movsb(cpu);
            }else if (strcmp(instr_name, "INC") == 0) {
                inc(cpu , original_instruction);
            }else if (strcmp(instr_name, "CMPSB") == 0) {
                cmpsb(cpu);
            }else if (strcmp(instr_name, "STC") == 0) {
                stc(cpu);
            }else if (strcmp(instr_name, "CLC") == 0) {
                clc(cpu);
            }else if (strcmp(instr_name, "JMP") == 0) {
                // 调用 jmp 函数处理跳转
                jmp(cpu, original_instruction, label_table, label_count);
                ip_increment = 0;  // 跳转时不递增 IP
            }else if (strcmp(instr_name, "INT") == 0) {
                interrupt(cpu , original_instruction);
            }

            else if (strcmp(instr_name, "LEA") == 0) {
                char original_instruction2[50];
                strcpy(original_instruction2, original_instruction);
                mode = parse_instruction(original_instruction2, "LEA", cpu, &dest, variables, var_count);
                lea(cpu , mode ,original_instruction , variables);

            } else if (strcmp(instr_name, "PUSH") == 0) {
                uint16_t value = 0;
                if (strstr(original_instruction, "AX")) value = cpu->AX;
                else if (strstr(original_instruction, "BX")) value = cpu->BX;
                else if (strstr(original_instruction, "CX")) value = cpu->CX;
                else if (strstr(original_instruction, "DX")) value = cpu->DX;
                else {
                    printf("Error: Unsupported register in PUSH\n");
                    return;
                }

                printf("压入值: 0x%04X\n", value);
                push(cpu, value);
                ip_increment = 1;

            } else if (strcmp(instr_name, "POP") == 0) {
                uint16_t value = pop(cpu);
                if (strstr(original_instruction, "AX")) cpu->AX = value;
                else if (strstr(original_instruction, "BX")) cpu->BX = value;
                else if (strstr(original_instruction, "CX")) cpu->CX = value;
                else if (strstr(original_instruction, "DX")) cpu->DX = value;
                else {
                    printf("Error: Unsupported register in POP\n");
                    return;
                }

                printf("弹出值: 0x%04X\n", value);
                ip_increment = 1;

            } else if (strcmp(instr_name, "CALL") == 0) {
                uint16_t address = 0;
                if (sscanf(original_instruction, "CALL %hx", &address) == 1) {
                    call(cpu, address);
                    ip_increment = 3;
                } else {
                    printf("CALL指令格式错误: %s\n", original_instruction);
                }

            } else if (strcmp(instr_name, "RET") == 0) {
                ret(cpu);
                ip_increment = 1;

            } else if (strcmp(instr_name, "OUT") == 0) {
                uint16_t port = 0, value = 0;
                if (sscanf(original_instruction, "OUT %hu, %hu", &port, &value) == 2) {
                    out(cpu, port, value);
                    ip_increment = 3;
                } else {
                    printf("OUT指令格式错误: %s\n", original_instruction);
                }

            } else if (strcmp(instr_name, "READ") == 0) {
                uint16_t address = 0;
                if (sscanf(original_instruction, "READ %hu", &address) == 1) {
                    uint8_t data = read_2164(&memory, address);
                    printf("读取数据: %02X\n", data);
                    ip_increment = 3;
                } else {
                    printf("READ指令格式错误: %s\n", original_instruction);
                }

            } else if (strcmp(instr_name, "WRITE") == 0) {
                uint16_t address = 0;
                uint8_t data = 0;
                if (sscanf(original_instruction, "WRITE %hu %hhu", &address, &data) == 2) {
                    write_2164(&memory, address, data);
                    printf("写入数据: %02X 到地址 %04X\n", data, address);
                    ip_increment = 3;
                } else {
                    printf("WRITE指令格式错误: %s\n", original_instruction);
                }

            } else {
                printf("未识别的指令: %s\n", original_instruction);
            }

            // 更新 IP 值
            cpu->IP += ip_increment;
            cpu_print_state(cpu);
            printf("--------------------------------------------------------\n");
        }
    }
}









