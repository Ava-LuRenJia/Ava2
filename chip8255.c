#include <stdint.h>
#include <stdbool.h>
#include "chip8255.h"

// ��ʼ�� 8255 оƬ
void chip8255_init(Chip8255 *chip) {
    chip->port_a = 0;
    chip->port_b = 0;
    chip->port_c = 0;
    chip->control_register = 0x80; // Ĭ��ģʽ0�����ж˿����
    chip->cs = -1;              // Ĭ��δѡ��
    chip->rd = false;
    chip->wr = false;
}

// д���ݵ� 8255
void chip8255_write(Chip8255 *chip, uint8_t address, uint8_t data) {
    if (chip->cs == -1 || !chip->wr) {
        printf("DEBUG: д�ź�δ��ȷ���� (CS=%d, WR=%d)\n", chip->cs, chip->wr);
        return; // δѡ�л�δ����д�ź�
    }

    switch (address) {
        case PORT_A:
            chip->port_a = data;
            printf("DEBUG: д�� PORT_A ֵ: %d\n", chip->port_a);
            break;
        case PORT_B:
            chip->port_b = data;
            printf("DEBUG: д�� PORT_B ֵ: %d\n", chip->port_b);
            break;
        case PORT_C:
            chip->port_c = data;
            printf("DEBUG: д�� PORT_C ֵ: %d\n", chip->port_c);
            break;
        case CONTROL_REGISTER:
            chip->control_register = data;
            printf("DEBUG: ���ÿ��ƼĴ���: 0x%X\n", chip->control_register);
            break;
        default:
            printf("DEBUG: ��Ч��ַ: %d\n", address);
            break;
    }
}

// �� 8255 ��ȡ����
uint8_t chip8255_read(Chip8255 *chip, uint8_t address) {
    printf("DEBUG: Inside chip8255_read, chip->cs = %d, chip->rd = %d\n", chip->cs, chip->rd);
    if (chip->cs == -1 || !chip->rd) {
        printf("DEBUG: ���ź�δ��ȷ���� (CS=%d, RD=%d)\n", chip->cs, chip->rd);
        return 0; // δѡ�л�δ���Ͷ��ź�
    }

    uint8_t result = 0;
    switch (address) {
        case PORT_A:
            result = chip->port_a;
            printf("DEBUG: ��ȡ PORT_A ֵ: %d\n", result);
            break;
        case PORT_B:
            result = chip->port_b;
            printf("DEBUG: ��ȡ PORT_B ֵ: %d\n", result);
            break;
        case PORT_C:
            result = chip->port_c;
            printf("DEBUG: ��ȡ PORT_C ֵ: %d\n", result);
            break;
        default:
            printf("DEBUG: ��Ч��ַ: %d\n", address);
            break;
    }
    return result;
}


// ���ÿ��ƼĴ���
void chip8255_configure(Chip8255 *chip, uint8_t control) {
    chip->control_register = control;
    // ���ݿ���������ģʽ�ͷ���
    // ��ʱ�򻯴�������ʵ�������������ÿһλ
}
