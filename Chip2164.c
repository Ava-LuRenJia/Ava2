#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "Chip2164.h"

#define MEMORY_SIZE 65536 // 64K x 1 λ�Ĵ洢����


// ��ʼ�� 2164 оƬ
void initMemory2164(Memory2164 *chip) {
    memset(chip->memory, 0, MEMORY_SIZE);//��ģ��洢��Ԫ����
    memset(chip->pin_states, 0, sizeof(chip->pin_states));//������״̬����
}

// �������Ÿߵ�ƽ
void set_pin(Memory2164 *chip, uint8_t pin) {
    chip->pin_states[pin] = true;
}

// �������ŵ͵�ƽ
void clear_pin(Memory2164 *chip, uint8_t pin) {
    chip->pin_states[pin] = false;
}

// ��ȡ����״̬
uint8_t read_pin(Memory2164 *chip, uint8_t pin) {
    return chip->pin_states[pin] ? 1 : 0;
}

// ���͵�ַ
void send_address(Memory2164 *chip, uint8_t address) {
    for (uint8_t i = 0; i < 8; i++) {
        if (address & (1 << i)) {//ʹ��λ���� (1 << i) ����� address �еĵ� i λ�Ƿ�Ϊ 1��
            set_pin(chip, A0_PIN + i);
        } else {
            clear_pin(chip, A0_PIN + i);
        }
        /*����� i λΪ 1������ set_pin ��������Ӧ�����ţ�A0_PIN + i������Ϊ�ߵ�ƽ�����Ϊ 0������� clear_pin ��������Ӧ����������Ϊ�͵�ƽ��*/
    }
}

// д�����ݵ��洢��Ԫ
void write_data(Memory2164 *chip, uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        if (data & (1 << i)) {
            set_pin(chip, Din_PIN + i); // �ߵ�ƽ
        } else {
            clear_pin(chip, Din_PIN + i); // �͵�ƽ
        }/*Din_PIN�����Ŵ��ڳ�ͻ���⣬�д��Ż�*/
    }
    chip->memory[chip->pin_states[A0_PIN] | (chip->pin_states[A7_PIN] << 7)] = data;
}

// ��ȡ����
uint8_t read_data(Memory2164 *chip) {
    uint8_t data = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (read_pin(chip, Dout_PIN + i)) {
            data |= (1 << i);
        }
    }
    return data;
}

// ��ȡ����
uint8_t read_2164(Memory2164 *mem, uint16_t address) {
    uint8_t row_address = (address >> 8) & 0xFF;
    uint8_t col_address = address & 0xFF;

    clear_pin(mem, RAS_PIN);
    send_address(mem, row_address); // �����е�ַ
    set_pin(mem, RAS_PIN);          // �����е�ַ
    /*clear_pin(mem, RAS_PIN)��RAS��������Ϊ�͵�ƽ��Ϊ�����е�ַ��׼����
    send_address(mem, row_address)�����е�ַ��оƬ.
    set_pin(mem, RAS_PIN)��RAS��������Ϊ�ߵ�ƽ�������е�ַ��*/

    clear_pin(mem, CAS_PIN);
    send_address(mem, col_address); // �����е�ַ
    set_pin(mem, CAS_PIN);          // �����е�ַ

    set_pin(mem, WE_PIN);           // ����Ϊ������
    return read_data(mem);          // ��ȡ����
}

// д�����
void write_2164(Memory2164 *mem, uint16_t address, uint8_t data) {
    uint8_t row_address = (address >> 8) & 0xFF;
    uint8_t col_address = address & 0xFF;
/*�����д��뽫16λ��ַ�ֽ�Ϊ8λ���е�ַ��8λ���е�ַ��address >> 8����ַ����8λ���õ��е�ַ�ĸ�8λ����2164оƬ�У�ͨ����ַ�ĸ�λ����ѡ���У���λ����ѡ���У�*/

    clear_pin(mem, RAS_PIN);
    send_address(mem, row_address); // �����е�ַ
    set_pin(mem, RAS_PIN);          // �����е�ַ

    clear_pin(mem, CAS_PIN);
    send_address(mem, col_address); // �����е�ַ
    set_pin(mem, CAS_PIN);          // �����е�ַ

    clear_pin(mem, WE_PIN);         // ����Ϊд����
    write_data(mem, data);          // д������
    set_pin(mem, WE_PIN);           // д����ɣ���λ WE
}






