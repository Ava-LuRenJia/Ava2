#include "test5.h"

void try() {
    CPU cpu;
    cpu_init(&cpu);

    // ���� ADD ָ��
    printf("---- Testing ADD ----\n");
    cpu.AX = 5;
    add(&cpu, &cpu.AX, 10); // AX = 5 + 10
    cpu_print_state(&cpu); // Ԥ�� AX = 15

    // ���� SUB ָ��
    printf("---- Testing SUB ----\n");
    cpu.AX = 20;
    sub(&cpu, &cpu.AX, 5); // AX = 20 - 5
    cpu_print_state(&cpu); // Ԥ�� AX = 15

    // ���� MOV ָ��


    // ���� PUSH ָ��
    printf("---- Testing PUSH ----\n");
    cpu_push(&cpu, cpu.AX); // �� AX ѹ��ջ
    cpu_print_state(&cpu); // Ԥ�� SP ����

    // ���� POP ָ��
    printf("---- Testing POP ----\n");
    uint16_t popped_value = cpu_pop(&cpu); // ��ջ����
    printf("Popped value: %u\n", popped_value); // Ԥ�� 100
    cpu_print_state(&cpu); // Ԥ�� SP ����

    // ���� XCHG ָ��
    printf("---- Testing XCHG ----\n");
    cpu.BX = 0x1234;
    cpu.CX = 0x5678;
    printf("Before xchg: BX = 0x%04X, CX = 0x%04X\n", cpu.BX, cpu.CX);
    xchg(&cpu.BX, &cpu.CX); // ���� BX �� CX
    printf("After xchg: BX = 0x%04X, CX = 0x%04X\n", cpu.BX, cpu.CX);

    // ���� ADC ָ��
    printf("---- Testing ADC ----\n");
    cpu.AX = 5;
    cpu_set_flag(&cpu, FLAG_CF, 1); // ���ý�λ��־Ϊ1
    adc(&cpu, &cpu.AX, 10); // AX = 5 + 10 + 1 (CF)
    cpu_print_state(&cpu); // Ԥ�� AX = 16

    // ���� SBB ָ��
    printf("---- Testing SBB ----\n");
    cpu.AX = 20;
    cpu_set_flag(&cpu, FLAG_CF, 1); // ���ý�λ��־Ϊ1
    sbb(&cpu, &cpu.AX, 5); // AX = 20 - 5 - 1 (CF)
    cpu_print_state(&cpu); // Ԥ�� AX = 14

    // ���� inc �� dec
    printf("---- Testing INC and DEC ----\n");
    inc(&cpu, &cpu.AX); // ֻ���� CPU ��ָ��� AX �ĵ�ַ
    dec(&cpu, &cpu.AX); // ֻ���� CPU ��ָ��� AX �ĵ�ַ

    // ���� AND ָ��
    printf("---- Testing AND ----\n");
    cpu.AX = 0b1100; // 12
    and(&cpu, &cpu.AX, 0b1010); // AX = 12 AND 10
    cpu_print_state(&cpu); // Ԥ�� AX = 8 (0b1000)

    // ���� OR ָ��
    printf("---- Testing OR ----\n");
    cpu.AX = 0b1100; // 12
    or(&cpu, &cpu.AX, 0b0010); // AX = 12 OR 2
    cpu_print_state(&cpu); // Ԥ�� AX = 14 (0b1110)

    // ���� XOR ָ��
    printf("---- Testing XOR ----\n");
    cpu.AX = 0b1100; // 12
    xor(&cpu, &cpu.AX, 0b1010); // AX = 12 XOR 10
    cpu_print_state(&cpu); // Ԥ�� AX = 6 (0b0110)

    // ���� TEST ָ��
    printf("---- Testing TEST ----\n");
    cpu.AX = 0b1100; // 12
    test(&cpu, cpu.AX, 0b1010); // AX = 12 AND 10 (ֻ���ñ�־)
    cpu_print_state(&cpu); // ZF �� SF ��־�ᱻ����

    // ���� movsb
    printf("---- Testing MOVSB ----\n");
    // ��д��һ��ֵ�� memory[0]
    memory[0] = 0x55; // ���ڴ��ַ 0 ��������
    cpu.SI = 0; // ����Դ����Ϊ 0
    cpu.DI = 1; // ����Ŀ������Ϊ 1
    printf("Before movsb: memory[0] = 0x%02X, memory[1] = 0x%02X\n", memory[0], memory[1]);
    movsb(&cpu, cpu.DI, cpu.SI); // ���ڴ� SI ���Ƶ� DI
    printf("After movsb: memory[0] = 0x%02X, memory[1] = 0x%02X\n", memory[0], memory[1]);

    // ���� NOP ָ��
    printf("---- Testing NOP ----\n");
    nop(&cpu); // ִ�� NOP
    cpu_print_state(&cpu); // ״̬Ӧ�ñ��ֲ���

    // ���� HLT ָ��
    printf("---- Testing HLT ----\n");
    hlt(&cpu); // ִ�� HLT
    // ������� HLT ָ� CPU ״̬����Ϊֹͣ״̬��������״̬���

    // ���� JMP ָ��
    printf("---- Testing JMP ----\n");
    cpu.IP = 0; // ���� IP ָ��0
    jmp(&cpu, 10); // JMP ����ַ10
    printf("IP after JMP: %u\n", cpu.IP); // Ԥ�� IP = 10

    // ���� CALL �� RET ָ��
    printf("---- Testing CALL and RET ----\n");
    cpu.IP = 0;
    cpu_push(&cpu, cpu.IP); // �����ص�ַѹ��ջ
    call(&cpu, 20); // CALL ����ַ20
    printf("IP after CALL: %u\n", cpu.IP); // Ԥ�� IP = 20
    // ���� ret ����
    ret(&cpu); // ���� ret ����������Ҫ��ֵ

    // ���� DIV ָ��
    printf("---- Testing DIV ----\n");
    cpu.AX = 20; // ������
    cpu.BX = 4; // ����
    Div(&cpu, cpu.BX); // AX = AX / BX
    cpu_print_state(&cpu);

    // ���� IDIV ָ��
    printf("---- Testing IDIV ----\n");
    cpu.AX = -20; // ������ (�з�����)
    cpu.BX = -4; // ���� (�з�����)
    idiv(&cpu, cpu.BX); // AX = AX / BX
    cpu_print_state(&cpu); // Ԥ�� AX = 5, DX = 0

    // ���� MUL ָ��
    printf("---- Testing MUL ----\n");
    cpu.AX = 5;   // ������
    cpu.BX = 4;   // ����
    mul(&cpu, cpu.BX); // AX = AX * BX
    cpu_print_state(&cpu); // Ԥ�� AX = 20, DX = 0

    // ���� IMUL ָ��
    printf("---- Testing IMUL ----\n");
    cpu.AX = -5;  // ������ (�з�����)
    cpu.BX = -4;  // ���� (�з�����)
    imul(&cpu, cpu.BX); // AX = AX * BX
    cpu_print_state(&cpu); // Ԥ�� AX = 20, DX = 0

    // ���� IMUL ָ���������չ��
    printf("---- Testing IMUL with signed extension ----\n");
    cpu.AX = 5;   // ������
    cpu.BX = -4;  // ���� (�з�����)
    imul(&cpu, cpu.BX); // AX = AX * BX
    cpu_print_state(&cpu); // Ԥ�� AX = -20


    // ���� MOVSW ָ��
    printf("---- Testing MOVSW ----\n");
    memory[0] = 0x1234; // Դ
    cpu.SI = 0; // ����Դ����Ϊ 0
    cpu.DI = 2; // ����Ŀ������Ϊ 2
    movsw(&cpu); // �� memory[0] ���Ƶ� memory[2]
    printf("memory[2] = 0x%04X\n", memory[2]); // Ԥ�� 0x1234

    // ���� CMPSB ָ��
    printf("---- Testing CMPSB ----\n");
    memory[0] = 0x12; // �Ƚ�ֵ
    memory[1] = 0x12; // ���Ƚ�ֵ
    cpu.SI = 0; // Դ����
    cpu.DI = 1; // Ŀ������
    cmpsb(&cpu); // �Ƚ� memory[SI] �� memory[DI]
    cpu_print_state(&cpu); // ��־״̬Ӧ����

    // ���� CMPSW ָ��
    printf("---- Testing CMPSW ----\n");
    memory[0] = 0x1234; // �Ƚ�ֵ
    memory[2] = 0x1234; // ���Ƚ�ֵ
    cpu.SI = 0; // Դ����
    cpu.DI = 2; // Ŀ������
    cmpsw(&cpu); // �Ƚ� memory[SI] �� memory[DI]
    cpu_print_state(&cpu); // ��־״̬Ӧ����

    // ���� SCASB ָ��
    printf("---- Testing SCASB ----\n");
    cpu.AX = 0x12; // �����ҵ�ֵ
    memory[1] = 0x12; // Ҫ���ҵ�ֵ
    cpu.DI = 1; // Ŀ������
    scasb(&cpu); // ���� memory[DI]
    cpu_print_state(&cpu); // ��־״̬Ӧ����

    // ���� SCASW ָ��
    printf("---- Testing SCASW ----\n");
    cpu.AX = 0x1234; // �����ҵ�ֵ
    memory[2] = 0x1234; // Ҫ���ҵ�ֵ
    cpu.DI = 2; // Ŀ������
    scasw(&cpu); // ���� memory[DI]
    cpu_print_state(&cpu); // ��־״̬Ӧ����

    // ���� LODSB ָ��
    printf("---- Testing LODSB ----\n");
    memory[0] = 0x34; // Դֵ
    cpu.SI = 0; // Դ����
    lodsb(&cpu); // �� memory[SI] ���ص� AL
    printf("AL after LODSB: 0x%02X\n", cpu.AL); // Ԥ�� 0x34

    // ���� LODSW ָ��
    printf("---- Testing LODSW ----\n");
    memory[0] = 0x1234; // Դֵ
    cpu.SI = 0; // Դ����
    lodsw(&cpu); // �� memory[SI] ���ص� AX
    printf("AX after LODSW: 0x%04X\n", cpu.AX); // Ԥ�� 0x1234

    // ���� STOSB ָ��
    printf("---- Testing STOSB ----\n");
    cpu.AL = 0x56; // Ҫ�洢��ֵ
    cpu.DI = 1; // Ŀ������
    stosb(&cpu); // �� AL �洢�� memory[DI]
    printf("memory[1] after STOSB: 0x%02X\n", memory[1]); // Ԥ�� 0x56

    // ���� STOSW ָ��
    printf("---- Testing STOSW ----\n");
    cpu.AX = 0x789A; // Ҫ�洢��ֵ
    cpu.DI = 2; // Ŀ������
    stosw(&cpu); // �� AX �洢�� memory[DI]
    printf("memory[2] after STOSW: 0x%04X\n", memory[2]); // Ԥ�� 0x789A

    // ���� CLC ָ��
    printf("---- Testing CLC ----\n");
    cpu_set_flag(&cpu, FLAG_CF, 1); // ���ý�λ��־
    clc(&cpu); // �����λ��־
    printf("CF after CLC: %d\n", cpu.CF); // Ԥ�� 0

    // ���� STC ָ��
    printf("---- Testing STC ----\n");
    stc(&cpu); // ���ý�λ��־
    printf("CF after STC: %d\n", cpu.CF); // Ԥ�� 1

    // ���� CMC ָ��
    printf("---- Testing CMC ----\n");
    cpu_set_flag(&cpu, FLAG_CF, 1); // ���ý�λ��־
    cmc(&cpu); // ��ת��λ��־
    printf("CF after CMC: %d\n", cpu.CF); // Ԥ�� 0

    // ���� CLCD ָ��
    // ���ָ����Ҫ����ʵ�ֽ��в��ԣ�������������־
    printf("---- Testing CLCD ----\n");
    cpu.DF = 1; // ���÷����־
    clcd(&cpu); // ��������־
    printf("DF after CLCD: %d\n", cpu.DF); // Ԥ�� 0

    // ���� STD ָ��
    printf("---- Testing STD ----\n");
    std(&cpu); // ���÷����־
    printf("DF after STD: %d\n", cpu.DF); // Ԥ�� 1

    // ���� CLI ָ��
    printf("---- Testing CLI ----\n");
    cli(&cpu); // �����ж�
    printf("IF after CLI: %d\n", cpu.IF); // Ԥ�� 0

    // ���� STI ָ��
    printf("---- Testing STI ----\n");
    sti(&cpu); // �����ж�
    printf("IF after STI: %d\n", cpu.IF); // Ԥ�� 1

    // ���� JMP ָ��
    printf("---- Testing JMP ----\n");
    cpu.IP = 0; // ���� IP ָ��0
    jmp(&cpu, 10); // JMP ����ַ10
    printf("IP after JMP: %u\n", cpu.IP); // Ԥ�� IP = 10

    // ���� CALL �� RET ָ��
    printf("---- Testing CALL and RET ----\n");
    cpu.IP = 0;
    cpu_push(&cpu, cpu.IP); // �����ص�ַѹ��ջ
    call(&cpu, 20); // CALL ����ַ20
    printf("IP after CALL: %u\n", cpu.IP); // Ԥ�� IP = 20
    // ���� ret ����
    ret(&cpu); // ���� ret ����������Ҫ��ֵ
    printf("IP after RET: %u\n", cpu.IP); // Ԥ�� IP = 0

    // ���� INT ָ��
    printf("---- Testing INT ----\n");
    int interrupt_number = 0x21; // �����жϺ�
    interrupt(&cpu, interrupt_number); // �����жϴ���
    // �жϴ�����״̬��֤���Ը���ʵ�����

    // ���� IRET ָ��
    printf("---- Testing IRET ----\n");
    iret(&cpu); // �����ж�
    // �жϷ��غ��״̬��֤���Ը���ʵ�����

    // ���� LOOP ָ��
    printf("---- Testing LOOP ----\n");
    cpu.CX = 5; // ѭ��������
    loop(&cpu); // LOOP ����ַ10
    printf("CX after LOOP: %u\n", cpu.CX); // Ԥ�� CX = 4

    // ���� LOOPZ ָ��
    printf("---- Testing LOOPZ ----\n");
    cpu.CX = 1; // ѭ��������
    cpu.ZF = 1; // �������־
    loopz(&cpu); // LOOPZ ����ַ10
    printf("CX after LOOPZ: %u\n", cpu.CX); // Ԥ�� CX = 0

    // ���� LOOPNZ ָ��
    printf("---- Testing LOOPNZ ----\n");
    cpu.CX = 1; // ѭ��������
    cpu.ZF = 0; // ������־
    loopnz(&cpu); // LOOPNZ ����ַ10
    printf("CX after LOOPNZ: %u\n", cpu.CX); // Ԥ�� CX = 0
}
