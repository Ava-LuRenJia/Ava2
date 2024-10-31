#include "test5.h"

void try() {
    CPU cpu;
    cpu_init(&cpu);

    // 测试 ADD 指令
    printf("---- Testing ADD ----\n");
    cpu.AX = 5;
    add(&cpu, &cpu.AX, 10); // AX = 5 + 10
    cpu_print_state(&cpu); // 预期 AX = 15

    // 测试 SUB 指令
    printf("---- Testing SUB ----\n");
    cpu.AX = 20;
    sub(&cpu, &cpu.AX, 5); // AX = 20 - 5
    cpu_print_state(&cpu); // 预期 AX = 15

    // 测试 MOV 指令


    // 测试 PUSH 指令
    printf("---- Testing PUSH ----\n");
    cpu_push(&cpu, cpu.AX); // 将 AX 压入栈
    cpu_print_state(&cpu); // 预期 SP 减少

    // 测试 POP 指令
    printf("---- Testing POP ----\n");
    uint16_t popped_value = cpu_pop(&cpu); // 从栈弹出
    printf("Popped value: %u\n", popped_value); // 预期 100
    cpu_print_state(&cpu); // 预期 SP 增加

    // 测试 XCHG 指令
    printf("---- Testing XCHG ----\n");
    cpu.BX = 0x1234;
    cpu.CX = 0x5678;
    printf("Before xchg: BX = 0x%04X, CX = 0x%04X\n", cpu.BX, cpu.CX);
    xchg(&cpu.BX, &cpu.CX); // 交换 BX 和 CX
    printf("After xchg: BX = 0x%04X, CX = 0x%04X\n", cpu.BX, cpu.CX);

    // 测试 ADC 指令
    printf("---- Testing ADC ----\n");
    cpu.AX = 5;
    cpu_set_flag(&cpu, FLAG_CF, 1); // 设置进位标志为1
    adc(&cpu, &cpu.AX, 10); // AX = 5 + 10 + 1 (CF)
    cpu_print_state(&cpu); // 预期 AX = 16

    // 测试 SBB 指令
    printf("---- Testing SBB ----\n");
    cpu.AX = 20;
    cpu_set_flag(&cpu, FLAG_CF, 1); // 设置进位标志为1
    sbb(&cpu, &cpu.AX, 5); // AX = 20 - 5 - 1 (CF)
    cpu_print_state(&cpu); // 预期 AX = 14

    // 调用 inc 和 dec
    printf("---- Testing INC and DEC ----\n");
    inc(&cpu, &cpu.AX); // 只传递 CPU 的指针和 AX 的地址
    dec(&cpu, &cpu.AX); // 只传递 CPU 的指针和 AX 的地址

    // 测试 AND 指令
    printf("---- Testing AND ----\n");
    cpu.AX = 0b1100; // 12
    and(&cpu, &cpu.AX, 0b1010); // AX = 12 AND 10
    cpu_print_state(&cpu); // 预期 AX = 8 (0b1000)

    // 测试 OR 指令
    printf("---- Testing OR ----\n");
    cpu.AX = 0b1100; // 12
    or(&cpu, &cpu.AX, 0b0010); // AX = 12 OR 2
    cpu_print_state(&cpu); // 预期 AX = 14 (0b1110)

    // 测试 XOR 指令
    printf("---- Testing XOR ----\n");
    cpu.AX = 0b1100; // 12
    xor(&cpu, &cpu.AX, 0b1010); // AX = 12 XOR 10
    cpu_print_state(&cpu); // 预期 AX = 6 (0b0110)

    // 测试 TEST 指令
    printf("---- Testing TEST ----\n");
    cpu.AX = 0b1100; // 12
    test(&cpu, cpu.AX, 0b1010); // AX = 12 AND 10 (只设置标志)
    cpu_print_state(&cpu); // ZF 和 SF 标志会被更新

    // 测试 movsb
    printf("---- Testing MOVSB ----\n");
    // 先写入一个值到 memory[0]
    memory[0] = 0x55; // 从内存地址 0 复制数据
    cpu.SI = 0; // 设置源索引为 0
    cpu.DI = 1; // 设置目的索引为 1
    printf("Before movsb: memory[0] = 0x%02X, memory[1] = 0x%02X\n", memory[0], memory[1]);
    movsb(&cpu, cpu.DI, cpu.SI); // 从内存 SI 复制到 DI
    printf("After movsb: memory[0] = 0x%02X, memory[1] = 0x%02X\n", memory[0], memory[1]);

    // 测试 NOP 指令
    printf("---- Testing NOP ----\n");
    nop(&cpu); // 执行 NOP
    cpu_print_state(&cpu); // 状态应该保持不变

    // 测试 HLT 指令
    printf("---- Testing HLT ----\n");
    hlt(&cpu); // 执行 HLT
    // 这里假设 HLT 指令将 CPU 状态设置为停止状态，不进行状态输出

    // 测试 JMP 指令
    printf("---- Testing JMP ----\n");
    cpu.IP = 0; // 假设 IP 指向0
    jmp(&cpu, 10); // JMP 到地址10
    printf("IP after JMP: %u\n", cpu.IP); // 预期 IP = 10

    // 测试 CALL 和 RET 指令
    printf("---- Testing CALL and RET ----\n");
    cpu.IP = 0;
    cpu_push(&cpu, cpu.IP); // 将返回地址压入栈
    call(&cpu, 20); // CALL 到地址20
    printf("IP after CALL: %u\n", cpu.IP); // 预期 IP = 20
    // 调用 ret 函数
    ret(&cpu); // 调用 ret 函数，不需要赋值

    // 测试 DIV 指令
    printf("---- Testing DIV ----\n");
    cpu.AX = 20; // 被除数
    cpu.BX = 4; // 除数
    Div(&cpu, cpu.BX); // AX = AX / BX
    cpu_print_state(&cpu);

    // 测试 IDIV 指令
    printf("---- Testing IDIV ----\n");
    cpu.AX = -20; // 被除数 (有符号数)
    cpu.BX = -4; // 除数 (有符号数)
    idiv(&cpu, cpu.BX); // AX = AX / BX
    cpu_print_state(&cpu); // 预期 AX = 5, DX = 0

    // 测试 MUL 指令
    printf("---- Testing MUL ----\n");
    cpu.AX = 5;   // 被乘数
    cpu.BX = 4;   // 乘数
    mul(&cpu, cpu.BX); // AX = AX * BX
    cpu_print_state(&cpu); // 预期 AX = 20, DX = 0

    // 测试 IMUL 指令
    printf("---- Testing IMUL ----\n");
    cpu.AX = -5;  // 被乘数 (有符号数)
    cpu.BX = -4;  // 乘数 (有符号数)
    imul(&cpu, cpu.BX); // AX = AX * BX
    cpu_print_state(&cpu); // 预期 AX = 20, DX = 0

    // 测试 IMUL 指令（带符号扩展）
    printf("---- Testing IMUL with signed extension ----\n");
    cpu.AX = 5;   // 被乘数
    cpu.BX = -4;  // 乘数 (有符号数)
    imul(&cpu, cpu.BX); // AX = AX * BX
    cpu_print_state(&cpu); // 预期 AX = -20


    // 测试 MOVSW 指令
    printf("---- Testing MOVSW ----\n");
    memory[0] = 0x1234; // 源
    cpu.SI = 0; // 设置源索引为 0
    cpu.DI = 2; // 设置目的索引为 2
    movsw(&cpu); // 从 memory[0] 复制到 memory[2]
    printf("memory[2] = 0x%04X\n", memory[2]); // 预期 0x1234

    // 测试 CMPSB 指令
    printf("---- Testing CMPSB ----\n");
    memory[0] = 0x12; // 比较值
    memory[1] = 0x12; // 被比较值
    cpu.SI = 0; // 源索引
    cpu.DI = 1; // 目的索引
    cmpsb(&cpu); // 比较 memory[SI] 和 memory[DI]
    cpu_print_state(&cpu); // 标志状态应更新

    // 测试 CMPSW 指令
    printf("---- Testing CMPSW ----\n");
    memory[0] = 0x1234; // 比较值
    memory[2] = 0x1234; // 被比较值
    cpu.SI = 0; // 源索引
    cpu.DI = 2; // 目的索引
    cmpsw(&cpu); // 比较 memory[SI] 和 memory[DI]
    cpu_print_state(&cpu); // 标志状态应更新

    // 测试 SCASB 指令
    printf("---- Testing SCASB ----\n");
    cpu.AX = 0x12; // 被查找的值
    memory[1] = 0x12; // 要查找的值
    cpu.DI = 1; // 目的索引
    scasb(&cpu); // 查找 memory[DI]
    cpu_print_state(&cpu); // 标志状态应更新

    // 测试 SCASW 指令
    printf("---- Testing SCASW ----\n");
    cpu.AX = 0x1234; // 被查找的值
    memory[2] = 0x1234; // 要查找的值
    cpu.DI = 2; // 目的索引
    scasw(&cpu); // 查找 memory[DI]
    cpu_print_state(&cpu); // 标志状态应更新

    // 测试 LODSB 指令
    printf("---- Testing LODSB ----\n");
    memory[0] = 0x34; // 源值
    cpu.SI = 0; // 源索引
    lodsb(&cpu); // 从 memory[SI] 加载到 AL
    printf("AL after LODSB: 0x%02X\n", cpu.AL); // 预期 0x34

    // 测试 LODSW 指令
    printf("---- Testing LODSW ----\n");
    memory[0] = 0x1234; // 源值
    cpu.SI = 0; // 源索引
    lodsw(&cpu); // 从 memory[SI] 加载到 AX
    printf("AX after LODSW: 0x%04X\n", cpu.AX); // 预期 0x1234

    // 测试 STOSB 指令
    printf("---- Testing STOSB ----\n");
    cpu.AL = 0x56; // 要存储的值
    cpu.DI = 1; // 目的索引
    stosb(&cpu); // 将 AL 存储到 memory[DI]
    printf("memory[1] after STOSB: 0x%02X\n", memory[1]); // 预期 0x56

    // 测试 STOSW 指令
    printf("---- Testing STOSW ----\n");
    cpu.AX = 0x789A; // 要存储的值
    cpu.DI = 2; // 目的索引
    stosw(&cpu); // 将 AX 存储到 memory[DI]
    printf("memory[2] after STOSW: 0x%04X\n", memory[2]); // 预期 0x789A

    // 测试 CLC 指令
    printf("---- Testing CLC ----\n");
    cpu_set_flag(&cpu, FLAG_CF, 1); // 设置进位标志
    clc(&cpu); // 清除进位标志
    printf("CF after CLC: %d\n", cpu.CF); // 预期 0

    // 测试 STC 指令
    printf("---- Testing STC ----\n");
    stc(&cpu); // 设置进位标志
    printf("CF after STC: %d\n", cpu.CF); // 预期 1

    // 测试 CMC 指令
    printf("---- Testing CMC ----\n");
    cpu_set_flag(&cpu, FLAG_CF, 1); // 设置进位标志
    cmc(&cpu); // 反转进位标志
    printf("CF after CMC: %d\n", cpu.CF); // 预期 0

    // 测试 CLCD 指令
    // 这个指令需要根据实现进行测试，假设清除方向标志
    printf("---- Testing CLCD ----\n");
    cpu.DF = 1; // 设置方向标志
    clcd(&cpu); // 清除方向标志
    printf("DF after CLCD: %d\n", cpu.DF); // 预期 0

    // 测试 STD 指令
    printf("---- Testing STD ----\n");
    std(&cpu); // 设置方向标志
    printf("DF after STD: %d\n", cpu.DF); // 预期 1

    // 测试 CLI 指令
    printf("---- Testing CLI ----\n");
    cli(&cpu); // 禁用中断
    printf("IF after CLI: %d\n", cpu.IF); // 预期 0

    // 测试 STI 指令
    printf("---- Testing STI ----\n");
    sti(&cpu); // 启用中断
    printf("IF after STI: %d\n", cpu.IF); // 预期 1

    // 测试 JMP 指令
    printf("---- Testing JMP ----\n");
    cpu.IP = 0; // 假设 IP 指向0
    jmp(&cpu, 10); // JMP 到地址10
    printf("IP after JMP: %u\n", cpu.IP); // 预期 IP = 10

    // 测试 CALL 和 RET 指令
    printf("---- Testing CALL and RET ----\n");
    cpu.IP = 0;
    cpu_push(&cpu, cpu.IP); // 将返回地址压入栈
    call(&cpu, 20); // CALL 到地址20
    printf("IP after CALL: %u\n", cpu.IP); // 预期 IP = 20
    // 调用 ret 函数
    ret(&cpu); // 调用 ret 函数，不需要赋值
    printf("IP after RET: %u\n", cpu.IP); // 预期 IP = 0

    // 测试 INT 指令
    printf("---- Testing INT ----\n");
    int interrupt_number = 0x21; // 假设中断号
    interrupt(&cpu, interrupt_number); // 调用中断处理
    // 中断处理后的状态验证可以根据实现添加

    // 测试 IRET 指令
    printf("---- Testing IRET ----\n");
    iret(&cpu); // 返回中断
    // 中断返回后的状态验证可以根据实现添加

    // 测试 LOOP 指令
    printf("---- Testing LOOP ----\n");
    cpu.CX = 5; // 循环计数器
    loop(&cpu); // LOOP 到地址10
    printf("CX after LOOP: %u\n", cpu.CX); // 预期 CX = 4

    // 测试 LOOPZ 指令
    printf("---- Testing LOOPZ ----\n");
    cpu.CX = 1; // 循环计数器
    cpu.ZF = 1; // 设置零标志
    loopz(&cpu); // LOOPZ 到地址10
    printf("CX after LOOPZ: %u\n", cpu.CX); // 预期 CX = 0

    // 测试 LOOPNZ 指令
    printf("---- Testing LOOPNZ ----\n");
    cpu.CX = 1; // 循环计数器
    cpu.ZF = 0; // 清除零标志
    loopnz(&cpu); // LOOPNZ 到地址10
    printf("CX after LOOPNZ: %u\n", cpu.CX); // 预期 CX = 0
}
