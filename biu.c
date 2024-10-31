#include <stdio.h>
#include <string.h>
#include "biu.h"

uint8_t memory[1024 * 1024]; // ʵ�ʶ��� memory

// ��ʼ��BIU
void init_biu(BIU *biu) {
    biu->front = 0;
    biu->rear = 0;
    memset(memory, 0, sizeof(memory)); // ��ʼ���ڴ�
}

// �������Ƿ�������ͨ������ rear + 1 �� front ��ģֵ���ж�
bool is_queue_full(BIU *biu) {
    return ((biu->rear + 1) % QUEUE_SIZE == biu->front);
}

// �������Ƿ�Ϊ��
bool is_queue_empty(BIU *biu) {
    return (biu->front == biu->rear);
}

// ��BIU�����м���ָ��
bool enqueue_instruction(BIU *biu, const char *instruction) {
    if (is_queue_full(biu)) {
        printf("���棺ָ�������������ǰ����: %d��ǰָ��: %d����ָ��: %d\n", QUEUE_SIZE, biu->front, biu->rear);
        return false;
    }

    // ȷ��ָ��Ȳ����������е���󳤶�
    if (strlen(instruction) >= 50) {
        printf("����ָ��������޷��������\n");
        return false;
    }

    // �����ظ�ָ��ֱ�Ӽ�����У�ʹ�� strcpy ��ָ��Ƶ����еĵ�ǰλ�ã�Ȼ����� rear ָ�룬�ƶ�����һ��λ��
    printf("Enqueuing instruction: %s\n", instruction); // ��ӵ�����Ϣ
    strcpy(biu->instruction_queue[biu->rear], instruction);
    biu->rear = (biu->rear + 1) % QUEUE_SIZE;
    return true;
}

// ��BIU������ȡ��ָ��
bool dequeue_instruction(BIU *biu, char *instruction) {
    if (is_queue_empty(biu)) {
        printf("���棺ָ�����Ϊ�գ��޷�ȡ��ָ��\n");
        return false;
    }
    strcpy(instruction, biu->instruction_queue[biu->front]);
    biu->front = (biu->front + 1) % QUEUE_SIZE;
    printf("Dequeued instruction: %s\n", instruction); // ��ӵ�����Ϣ
    return true;
}

uint16_t find_variable_address(Variable variables[], int var_count, const char *var_name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(variables[i].name, var_name) == 0) {
            // ���ر����������е�������Ϊ��ַ
            return (uint16_t)(i + 0x1000); // �����0x1000��ʼ�����ַ
        }
    }
    return 0; // �Ҳ�����������0
}





