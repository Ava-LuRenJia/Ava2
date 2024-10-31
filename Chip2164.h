#ifndef CHIP2164_H
#define CHIP2164_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "biu.h"
#include "eu.h"

#define MEMORY_SIZE 256  // ���� 256 x 8 λ�Ĵ洢����

#define RAS_PIN  0
#define CAS_PIN  1
#define WE_PIN   2
#define Din_PIN  3
#define Dout_PIN 4
#define A0_PIN 5
#define A1_PIN 6
#define A2_PIN 7
#define A3_PIN 8
#define A4_PIN 9
#define A5_PIN 10
#define A6_PIN 11
#define A7_PIN 12

#define MEMORY_SIZE 65536 // ģ��64K�洢����

// 2164 �洢оƬ��
typedef struct {
    uint8_t memory[MEMORY_SIZE]; // ģ��洢��Ԫ
    bool pin_states[13];         // ����״̬
} Memory2164;

// ��ʼ�� 2164 �洢оƬ
void initMemory2164(Memory2164 *chip);

// д�����ݵ� 2164 �洢оƬ
void write_2164(Memory2164 *mem, uint16_t address, uint8_t data);

// �� 2164 �洢оƬ��ȡ����
uint8_t read_2164(Memory2164 *mem, uint16_t address);

#endif // CHIP2164_H