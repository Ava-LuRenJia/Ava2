#include <stdint.h>
#include <stdbool.h>
#include "chip8255.h"

// 初始化 8255 芯片
void chip8255_init(Chip8255 *chip) {
    chip->port_a = 0;
    chip->port_b = 0;
    chip->port_c = 0;
    chip->control_register = 0x80; // 默认模式0，所有端口输出
    chip->cs = -1;              // 默认未选中
    chip->rd = false;
    chip->wr = false;
}

// 写数据到 8255
void chip8255_write(Chip8255 *chip, uint8_t address, uint8_t data) {
    if (chip->cs == -1 || !chip->wr) {
        printf("DEBUG: 写信号未正确设置 (CS=%d, WR=%d)\n", chip->cs, chip->wr);
        return; // 未选中或未发送写信号
    }

    switch (address) {
        case PORT_A:
            chip->port_a = data;
            printf("DEBUG: 写入 PORT_A 值: %d\n", chip->port_a);
            break;
        case PORT_B:
            chip->port_b = data;
            printf("DEBUG: 写入 PORT_B 值: %d\n", chip->port_b);
            break;
        case PORT_C:
            chip->port_c = data;
            printf("DEBUG: 写入 PORT_C 值: %d\n", chip->port_c);
            break;
        case CONTROL_REGISTER:
            chip->control_register = data;
            printf("DEBUG: 配置控制寄存器: 0x%X\n", chip->control_register);
            break;
        default:
            printf("DEBUG: 无效地址: %d\n", address);
            break;
    }
}

// 从 8255 读取数据
uint8_t chip8255_read(Chip8255 *chip, uint8_t address) {
    printf("DEBUG: Inside chip8255_read, chip->cs = %d, chip->rd = %d\n", chip->cs, chip->rd);
    if (chip->cs == -1 || !chip->rd) {
        printf("DEBUG: 读信号未正确设置 (CS=%d, RD=%d)\n", chip->cs, chip->rd);
        return 0; // 未选中或未发送读信号
    }

    uint8_t result = 0;
    switch (address) {
        case PORT_A:
            result = chip->port_a;
            printf("DEBUG: 读取 PORT_A 值: %d\n", result);
            break;
        case PORT_B:
            result = chip->port_b;
            printf("DEBUG: 读取 PORT_B 值: %d\n", result);
            break;
        case PORT_C:
            result = chip->port_c;
            printf("DEBUG: 读取 PORT_C 值: %d\n", result);
            break;
        default:
            printf("DEBUG: 无效地址: %d\n", address);
            break;
    }
    return result;
}


// 配置控制寄存器
void chip8255_configure(Chip8255 *chip, uint8_t control) {
    chip->control_register = control;
    // 根据控制字设置模式和方向
    // 暂时简化处理，完整实现需解析控制字每一位
}
