#ifndef CHIP8255_H
#define CHIP8255_H
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// 端口定义
#define PORT_A 0
#define PORT_B 1
#define PORT_C 2
#define CONTROL_REGISTER 3

// 8255芯片结构
typedef struct {
    uint8_t port_a;              // A端口数据
    uint8_t port_b;              // B端口数据
    uint8_t port_c;              // C端口数据
    uint8_t control_register;    // 控制寄存器
    int cs;                     // 片选信号，true代表高地址，False代表低地址
    bool rd;                     // 读信号
    bool wr;                     // 写信号
} Chip8255;

// 初始化 8255 芯片
void chip8255_init(Chip8255 *chip);

// 写数据到 8255
void chip8255_write(Chip8255 *chip, uint8_t address, uint8_t data);

// 从 8255 读取数据
uint8_t chip8255_read(Chip8255 *chip, uint8_t address);

// 配置控制寄存器
void chip8255_configure(Chip8255 *chip, uint8_t control);

#endif // CHIP8255_H

