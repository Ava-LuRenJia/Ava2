#ifndef CHIP8255_H
#define CHIP8255_H
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// �˿ڶ���
#define PORT_A 0
#define PORT_B 1
#define PORT_C 2
#define CONTROL_REGISTER 3

// 8255оƬ�ṹ
typedef struct {
    uint8_t port_a;              // A�˿�����
    uint8_t port_b;              // B�˿�����
    uint8_t port_c;              // C�˿�����
    uint8_t control_register;    // ���ƼĴ���
    int cs;                     // Ƭѡ�źţ�true����ߵ�ַ��False����͵�ַ
    bool rd;                     // ���ź�
    bool wr;                     // д�ź�
} Chip8255;

// ��ʼ�� 8255 оƬ
void chip8255_init(Chip8255 *chip);

// д���ݵ� 8255
void chip8255_write(Chip8255 *chip, uint8_t address, uint8_t data);

// �� 8255 ��ȡ����
uint8_t chip8255_read(Chip8255 *chip, uint8_t address);

// ���ÿ��ƼĴ���
void chip8255_configure(Chip8255 *chip, uint8_t control);

#endif // CHIP8255_H

