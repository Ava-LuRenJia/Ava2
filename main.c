/*#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "biu.h"
#include "eu.h"
#include "Chip2164.h"

#define MAX_VARIABLES 100 // ����һ������������
#define MAX_INPUT_SIZE 1024 // ���������С


int main() {
    CPU cpu;
    BIU biu;
    Variable variables[MAX_VARIABLES]; // ��������
    int var_count = 0; // ����������

    cpu_init(&cpu);
    init_biu(&biu);

    printf("��������ָ�ÿ��һ�������� 'END' ��������:\n");

    char input[MAX_INPUT_SIZE] = {0};
    char temp[50];

    // �ռ���������ָ��
    while (1) {
        fgets(temp, sizeof(temp), stdin);
        temp[strcspn(temp, "\n")] = '\0'; // �Ƴ����з�

        // ��������־
        if (strcmp(temp, "END") == 0) {
            break;
        }
        if (strlen(temp) == 0) {
            continue; // ��������
        }

        // ȷ�����ᳬ�������С
        if (strlen(input) + strlen(temp) + 2 < MAX_INPUT_SIZE) {
            snprintf(input + strlen(input), sizeof(input) - strlen(input), "%s\n", temp);
        } else {
            printf("����������޷�������\n");
            break;
        }
    }

    // ���� parse_segment ��������Ļ�����
    parse_segment(input, &biu, &cpu, variables, &var_count);

    // ִ��ָ��
    char instruction[50]; // �洢�Ӷ�����ȡ����ָ��
    int instruction_count = 0; // ͳ��ָ������
    while (dequeue_instruction(&biu, instruction)) {
        execute_code(&cpu, &biu, variables, var_count); // ִ�е���ָ��
        printf("��ִ��ָ��: %s\n", instruction);
        instruction_count++;
    }

    printf("�ܹ�ִ���� %d ��ָ��\n", instruction_count);

    return 0;
}

*/


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
    /*try();*/    //��ҵ5����
    Memory2164 memory;
    Variable variables[MAX_VARIABLES];
    int var_count = 0;

    cpu_init(&cpu);
    init_biu(&biu);
    initMemory2164(&memory);

    printf("��������ָ�ÿ��һ�������� 'END START' ��������:\n");

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
            printf("����������޷�������\n");
            break;
        }
    }

    parse_segment(input, &biu, &cpu, variables, &var_count);

    printf("��ʼִ��ָ��...\n");
    printf("--------------------------------------------------------\n");
    char instruction[50];
    int instruction_count = 0;

    // ִ��ָ�����
    while (dequeue_instruction(&biu, instruction)) {
        if (strstr(instruction, "READ") != NULL) {
            uint16_t address = 0;
            if (sscanf(instruction, "READ %hu", &address) == 1) {
                // ��ַ���߷��͵� 2164����������ַ���������е�ַ���е�ַ
                uint8_t data = read_2164(&memory, address);
                printf("��ȡ����: %02X\n", data);
            } else {
                printf("READָ���ʽ����: %s\n", instruction);
            }
        } else if (strstr(instruction, "WRITE") != NULL) {
            uint16_t address = 0;
            uint8_t data = 0;
            if (sscanf(instruction, "WRITE %hu %hhu", &address, &data) == 2) {
                // ��ַ���߷��͵� 2164����������ַ���������е�ַ���е�ַ
                write_2164(&memory, address, data);
                printf("д������: %02X ����ַ %04X\n", data, address);
            } else {
                printf("WRITEָ���ʽ����: %s\n", instruction);
            }
        } else {
            // ��������ָ��
            execute_code(&cpu, &biu, variables, var_count);
        }

        printf("��ִ��ָ��: %s\n", instruction);
        instruction_count++;
    }

    printf("�ܹ�ִ���� %d ��ָ��\n", instruction_count);

    return 0;
}


//�����������ǲ���Chip2164.c�ܲ�����������
/*
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "Chip2164.h"

#define MEMORY_SIZE 65536 // 64K x 1 λ�Ĵ洢����

int main() {
    Memory2164 memory;
    initMemory2164(&memory);

    // д������ʾ��
    uint16_t address_to_write = 0x01A0; // Ҫд��ĵ�ַ
    uint8_t data_to_write = 0xFF;        // Ҫд�������
    write_2164(&memory, address_to_write, data_to_write);
    printf("��д������: %02X ����ַ: %04X\n", data_to_write, address_to_write);

    // ��ȡ����ʾ��
    uint16_t address_to_read = 0x01A0; // Ҫ��ȡ�ĵ�ַ
    uint8_t read_data_value = read_2164(&memory, address_to_read);
    printf("�ӵ�ַ: %04X ��ȡ������: %02X\n", address_to_read, read_data_value);

    // ���Զ�ȡһ��δд����ĵ�ַ
    uint16_t new_address_to_read = 0x01A1; // Ҫ��ȡ�ĵ�ַ
    uint8_t new_read_data_value = read_2164(&memory, new_address_to_read);
    printf("�ӵ�ַ: %04X ��ȡ������: %02X\n", new_address_to_read, new_read_data_value);

    return 0;
}
*/