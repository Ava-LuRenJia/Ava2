#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "Chip2164.h"

#define MEMORY_SIZE 65536 // 64K x 1 位的存储容量


// 初始化 2164 芯片
void initMemory2164(Memory2164 *chip) {
    memset(chip->memory, 0, MEMORY_SIZE);//把模拟存储单元清零
    memset(chip->pin_states, 0, sizeof(chip->pin_states));//把引脚状态清零
}

// 设置引脚高电平
void set_pin(Memory2164 *chip, uint8_t pin) {
    chip->pin_states[pin] = true;
}

// 设置引脚低电平
void clear_pin(Memory2164 *chip, uint8_t pin) {
    chip->pin_states[pin] = false;
}

// 读取引脚状态
uint8_t read_pin(Memory2164 *chip, uint8_t pin) {
    return chip->pin_states[pin] ? 1 : 0;
}

// 发送地址
void send_address(Memory2164 *chip, uint8_t address) {
    for (uint8_t i = 0; i < 8; i++) {
        if (address & (1 << i)) {//使用位操作 (1 << i) 来检查 address 中的第 i 位是否为 1。
            set_pin(chip, A0_PIN + i);
        } else {
            clear_pin(chip, A0_PIN + i);
        }
        /*如果第 i 位为 1，调用 set_pin 函数将对应的引脚（A0_PIN + i）设置为高电平；如果为 0，则调用 clear_pin 函数将对应的引脚设置为低电平。*/
    }
}

// 写入数据到存储单元
void write_data(Memory2164 *chip, uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        if (data & (1 << i)) {
            set_pin(chip, Din_PIN + i); // 高电平
        } else {
            clear_pin(chip, Din_PIN + i); // 低电平
        }/*Din_PIN的引脚存在冲突问题，有待优化*/
    }
    chip->memory[chip->pin_states[A0_PIN] | (chip->pin_states[A7_PIN] << 7)] = data;
}

// 读取数据
uint8_t read_data(Memory2164 *chip) {
    uint8_t data = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (read_pin(chip, Dout_PIN + i)) {
            data |= (1 << i);
        }
    }
    return data;
}

// 读取操作
uint8_t read_2164(Memory2164 *mem, uint16_t address) {
    uint8_t row_address = (address >> 8) & 0xFF;
    uint8_t col_address = address & 0xFF;

    clear_pin(mem, RAS_PIN);
    send_address(mem, row_address); // 发送行地址
    set_pin(mem, RAS_PIN);          // 锁存行地址
    /*clear_pin(mem, RAS_PIN)将RAS引脚设置为低电平，为发送行地址做准备。
    send_address(mem, row_address)发送行地址到芯片.
    set_pin(mem, RAS_PIN)将RAS引脚设置为高电平，锁存行地址。*/

    clear_pin(mem, CAS_PIN);
    send_address(mem, col_address); // 发送列地址
    set_pin(mem, CAS_PIN);          // 锁存列地址

    set_pin(mem, WE_PIN);           // 设置为读操作
    return read_data(mem);          // 读取数据
}

// 写入操作
void write_2164(Memory2164 *mem, uint16_t address, uint8_t data) {
    uint8_t row_address = (address >> 8) & 0xFF;
    uint8_t col_address = address & 0xFF;
/*这两行代码将16位地址分解为8位的行地址和8位的列地址。address >> 8将地址右移8位，得到行地址的高8位（在2164芯片中，通常地址的高位用于选择行，低位用于选择列）*/

    clear_pin(mem, RAS_PIN);
    send_address(mem, row_address); // 发送行地址
    set_pin(mem, RAS_PIN);          // 锁存行地址

    clear_pin(mem, CAS_PIN);
    send_address(mem, col_address); // 发送列地址
    set_pin(mem, CAS_PIN);          // 锁存列地址

    clear_pin(mem, WE_PIN);         // 设置为写操作
    write_data(mem, data);          // 写入数据
    set_pin(mem, WE_PIN);           // 写入完成，复位 WE
}






