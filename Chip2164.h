#ifndef CHIP2164_H
#define CHIP2164_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "biu.h"
#include "eu.h"

#define MEMORY_SIZE 256  // 假设 256 x 8 位的存储容量

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

#define MEMORY_SIZE 65536 // 模拟64K存储容量

// 2164 存储芯片类
typedef struct {
    uint8_t memory[MEMORY_SIZE]; // 模拟存储单元
    bool pin_states[13];         // 引脚状态
} Memory2164;

// 初始化 2164 存储芯片
void initMemory2164(Memory2164 *chip);

// 写入数据到 2164 存储芯片
void write_2164(Memory2164 *mem, uint16_t address, uint8_t data);

// 从 2164 存储芯片读取数据
uint8_t read_2164(Memory2164 *mem, uint16_t address);
void set_pin(Memory2164 *chip, uint8_t pin);
void clear_pin(Memory2164 *chip, uint8_t pin);
uint8_t read_pin(Memory2164 *chip, uint8_t pin);
void send_address(Memory2164 *chip, uint8_t address);
void write_data(Memory2164 *chip, uint8_t data);
uint8_t read_data(Memory2164 *chip);
uint8_t read_2164(Memory2164 *mem, uint16_t address);

#endif // CHIP2164_H
