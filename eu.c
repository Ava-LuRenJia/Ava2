#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "biu.h"
#include "cpu.h"
#include "eu.h"
#include "Chip2164.h"
#define MAX_CODE 256

// �޼��ַ������˵Ŀհ�
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

        // ����ζ���
        if (strstr(line, "DATA SEGMENT")) {
            in_data_segment = 1;
            printf("�������ݶΡ�\n");
        } else if (strstr(line, "DATA ENDS")) {
            in_data_segment = 0;
            printf("�˳����ݶΡ�\n");
        } else if (strstr(line, "CODE SEGMENT")) {
            in_code_segment = 1;
            printf("�������Ρ�\n");
        } else if (strstr(line, "CODE ENDS")) {
            in_code_segment = 0;
            printf("�˳�����Ρ�\n");
        } else if (strstr(line, "START:")) {
            found_start = 1;
            printf("�ҵ� START ��ǩ\n");
            cpu->CS = CODE_SEGMENT_START;
            cpu->DS = DATA_SEGMENT_START;
            cpu->SS = STACK_SEGMENT_START;
            printf("�μĴ����ѳ�ʼ��: CS=%04X, DS=%04X, SS=%04X\n", cpu->CS, cpu->DS, cpu->SS);
        } else if (strstr(line, "PROC") != NULL) {
            // ������̶���
            printf("�ҵ����̶���: %s\n", line);
            // ������Խ��������ƻ����ʶ�洢��һ���б��У������Ҫ��������
            // enqueue_instruction(biu, line);  // �����Ҫ�����̱�����
        } else if (strstr(line, "ENDP") != NULL) {
            // ������̽���
            printf("�ҵ����̽���: %s\n", line);
            // ������Խ�����Ӧ�Ĵ���������б����Ƴ���ǰ����
        }

        // �������ݶ��еı���
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
                    printf("��������: ����=%s, ����=%s, ֵ=%d, ��ַ=%04X\n", var_name, var_type, value, DATA_SEGMENT_START + *var_count);
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
                    printf("��������: ����=%s, ֵ=%d, ��ַ=%04X\n", var_name,
                           (variables[*var_count].type == 0) ? variables[*var_count].value.byte_value : variables[*var_count].value.word_value,
                           DATA_SEGMENT_START + *var_count);
                    (*var_count)++;
                }
            }
        }

        // �ڴ���������ָ��
        if (in_code_segment && found_start) {
            if (strlen(line) > 0) {
                if (!strstr(line, "PROC") && !strstr(line, "ENDP")) {
                    if (enqueue_instruction(biu, line)) {
                        printf("�����ָ��: %s\n", line);
                    } else {
                        printf("�޷���ָ�����: %s\n", line);
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
    printf("��ʼִ��ָ��...\n");
    printf("--------------------------------------------------------\n");

    while (!is_queue_empty(biu)) {
        if (dequeue_instruction(biu, instruction)) {
            if (instruction[0] == ';' || strlen(instruction) == 0) {
                continue; // ����ע�ͻ��ָ��
            }
            uint16_t dest = 0, src = 0;

            // ���� MOV ָ��
            if (strstr(instruction, "MOV") != NULL) {
                AddressingMode mode = parse_instruction(instruction, "MOV", cpu, &dest, variables, var_count);
                // ִ�� MOV ����
                mov(cpu, &dest, src, mode, instruction);
            } else if (strstr(instruction, "LEA") != NULL) {
                uint16_t addr = 0;
                AddressingMode mode = parse_instruction(instruction, "LEA", cpu, &addr, variables, var_count);
                if (mode == MEMORY) {
                    // ������ڴ�Ѱַ��ȷ����ַ��Ч
                    addr = find_variable_address(variables, var_count, "Str1");
                    if (addr != 0) {
                        cpu->SI = addr;  // ���������ĵ�ַ���� SI �Ĵ���
                    }
                }
                printf("LEA ��������ַ: %u\n", addr);
            } else if (strstr(instruction, "PUSH") != NULL) {
                uint16_t value = (strstr(instruction, "AX") ? cpu->AX : cpu->BX);
                printf("ѹ��ֵ: 0x%04X\n", value);
                push(cpu, value); // ʵ�ʵ� PUSH �߼�
            } else if (strstr(instruction, "POP") != NULL) {
                uint16_t value = pop(cpu); // ��ջ�е���ֵ
                if (strstr(instruction, "AX")) cpu->AX = value;
                else if (strstr(instruction, "BX")) cpu->BX = value;
                printf("����ֵ: 0x%04X\n", value);
            } else if (strstr(instruction, "CALL") != NULL) {
                uint16_t address = 0; // �������õĵ�ַ
                sscanf(instruction, "CALL %hx", &address); // ��ָ������ȡ��ַ
                call(cpu, address); // ���� CALL ����
            } else if (strstr(instruction, "RET") != NULL) {
                ret(cpu); // ���� RET ����
            } else if (strstr(instruction, "INT") != NULL) {
                uint8_t interrupt_number = 0; // �����жϺ�
                sscanf(instruction, "INT %hhu", &interrupt_number); // ��ָ������ȡ�жϺ�
                interrupt(cpu, interrupt_number); // �����жϴ�����
            } else if (strstr(instruction, "OUT") != NULL) {
                uint16_t port = 0, value = 0; // �����˿ں����ֵ
                sscanf(instruction, "OUT %hu, %hu", &port, &value); // ��ָ������ȡ�˿ں����ֵ
                out(cpu, port, value); // �����������
            } else if (strstr(instruction, "READ") != NULL) {
                uint16_t address = 0;
                if (sscanf(instruction, "READ %hu", &address) == 1) {
                    // ͨ�� read_2164 ������ȡ����
                    uint8_t data = read_2164(&memory, address);
                    printf("��ȡ����: %02X\n", data);
                } else {
                    printf("READָ���ʽ����: %s\n", instruction);
                }
            } else if (strstr(instruction, "WRITE") != NULL) {
                uint16_t address = 0;
                uint8_t data = 0;
                if (sscanf(instruction, "WRITE %hu %hhu", &address, &data) == 2) {
                    // ͨ�� write_2164 ����д������
                    write_2164(&memory, address, data);
                    printf("д������: %02X ����ַ %04X\n", data, address);
                } else {
                    printf("WRITEָ���ʽ����: %s\n", instruction);
                }
            } else {
                printf("δʶ���ָ��: %s\n", instruction);
            }

            cpu_print_state(cpu);
            printf("--------------------------------------------------------\n");
        }
    }
}









