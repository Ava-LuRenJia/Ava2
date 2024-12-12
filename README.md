# Ava2
2024.10.31:  

**河北工大微机原理作业7（有bug版）**

各个部分简要说明：  

**Instruction:**
实现各个指令功能，但是疑似存在bug，导致输出结果有误（比如mov函数实现就有bug）；  

**eu:**
实现解析汇编语言和执行指令；  

**cpu：**
包含各个寄存器和标志位，还包含初始化、设置标志位等功能；  

**biu:**
包含实现指令进入队列，以及从中取指的函数；  

**address:**
包含各个寻址方式，但是疑似存在bug，导致输出结果有误；  

**Chip2164:**
包含各个引脚的状态，包含初始化、写数据、读数据、设置引脚、清除引脚等功能；  


**tips:**

在开始讲解作业之前，请允许我向老师说一声抱歉，因为我这个代码大部分参考了gpt，比如再设计怎么剪除输入汇编程序的注释时，我是按我的想法直接让gap实现了这个功能，对应着呢就是trim_whitespace和parse_segment开头的这部分，具体的设计细节是由gpt完成的；还有一个原因，是因为我们组是在周六的时候才被抽到的，而那时候我们组进度最快的是我，那时的我才做到作业5的一点点内容，再加上我能力有限，所以导致这个作业有明显的赶工痕迹，而且导致了一系列bug；最后一个原因，就是这个代码是存在功能缺陷的，比如Instruction.c中mov的执行代码部分和address这部分存在bug，比如我上一条指令执行的是MOV AL, 15，然后输出没问题，到了下一条MOV [Num1], AL然后输出时发现AL的值变为0了，而且在处理数据段时Num1等值好像没有被定义。因为时间有限，所以我没能处理掉这些bug，导致作业6并没有真正意义上完成，也直接导致了这次的作业7注定不能用汇编语言来验证其输出正确与否。

---------------------------------------------------------------------------------------------------------------------------------------------------------------
2024.11.7:

改进了代码，主要体现在Instruction.c中mov函数的部分和address.c中查找地址的方法，使代码能支持立即寻址、直接寻址、寄存器间接寻址等寻址方式，增加了对MOV AX, DATA的特殊处理。

---------------------------------------------------------------------------------------------------------------------------------------------------------------
2024.11.13:  

哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈，我终于改完bug了(T_T)，比如intruction.c从500多行代码加到1540行代码；然后现在这个代码能执行Test6中的测试用例了。 

不过值得一说的是，有些功能我实现的有点简陋，比如lea的寻址方式我只实现了case DIRECT的情况，如果后面有需要的话可以再添加更多的case； 

还有就是我只改进了Test6中所提到的指令，未被提到的暂时没有进行改进，我认为intruction.c还有很大的改进空间（要加更多的代码）； 

还有就是输入形如MOV SI, OFFSETStr1的指令时，“OFFSETStr1”中间一定不能有空格，否则会识别不了，这也算一个小bug吧； 

在实现堆栈段时，我把作业要求中的堆栈段从后移到前面了；  

对于标签，这个我实现的也比较简陋，只能识别名为“START”的标签，所以说这个也有很大的改进（加代码）空间； 

还有就是从这儿下载我的代码时可以忽略test5.c，因为里边的代码过于老旧了，需要大改才能运行，然而因为我懒，所以还没对这块进行改进- -； 

还有一些问题，就是输入汇编语言时不能加注释，如果加了一定，一定，一定会产生bug！！！
---------------------------------------------------------------------------------------------------------------------------------------------------------------
2024.11.14:  

补一个作业要求和上传一个结果文档txt：  

**作业要求：复现8086CPU的原理及指令集（含伪指令）**

**一、复现8086CPU的原理**

EU（执行单元）与BIU（总线接口单元）

o    EU：负责指令的解释和执行，包括算术逻辑运算单元（ALU）、标志寄存器（FLAGS）、通用寄存器组以及EU控制器。

o    BIU：负责与外部总线交互，包括地址加法器、段寄存器（CS, DS, SS, ES）、指令指针寄存器（IP）、指令队列和总线控制逻辑。

通用寄存器与内部寄存器

o    通用寄存器：AX, BX, CX, DX，用于存放操作数、结果和地址偏移。

o    指针与变址寄存器：SP, BP, SI, DI，用于栈操作和数组处理。

o    段寄存器：CS, DS, SS, ES，用于定义代码段、数据段、堆栈段和附加段的起始地址。

ALU（算术逻辑运算单元）

o    执行算术和逻辑运算，并更新标志寄存器中的标志位。

标志位

o    FLAGS寄存器包含多个标志位，如CF（进位标志）、ZF（零标志）、SF（符号标志）、OF（溢出标志）等，用于指示运算结果的状态。

**二、指令读取和执行的同步处理模式**

指令队列

o    指令队列位于BIU中，用于暂存从内存中预取的指令。当指令队列不满时，BIU会自动从内存中读取指令填充队列。

同步处理模式

o    8086CPU采用流水线技术，指令的读取和执行在时间上重叠。EU从指令队列中取出指令执行，而BIU则负责从内存中读取指令和存取数据。

**三、各种指令及其寻址方式**

数据传送指令

o    包括MOV、PUSH、POP等，用于数据在寄存器、内存和I/O端口之间的传送。

o    寻址方式：立即寻址、直接寻址、寄存器寻址、寄存器间接寻址、基址变址寻址、相对寻址。

数据和逻辑运算指令

o    包括ADD、SUB、MUL、DIV、AND、OR、XOR、NOT等，用于算术和逻辑运算。

o    运算结果会影响FLAGS寄存器中的标志位。

字符串控制指令

o    包括MOVSB、MOVSW、CMPSB、CMPSW等，用于字符串的传送和比较。

处理器控制指令

o    包括HLT、STC、CLC等，用于控制CPU的状态和行为。

程序控制指令

o    包括JMP、CALL、RET、INT、IRET等，用于实现程序的跳转、调用、返回和中断处理。

**四、汇编语言及其伪指令**

汇编语言：是一种低级编程语言，通过助记符表示机器指令。

伪指令：不是真正的机器指令，但由汇编器解释和执行，用于定义程序结构、数据段、代码段等。

o    段定义伪指令：如.CODE、.DATA、.STACK，用于定义代码段、数据段和堆栈段。

o    数据定义伪指令：如DB、DW、DD，用于定义字节、字和双字数据。

o    过程定义伪指令：如PROC、ENDP，用于定义和结束过程（函数）。

o    标签和注释：使用冒号:定义标签，使用分号;添加注释。

**五、作业内容**

原理复现

o    编写文档或程序，详细阐述8086CPU的EU、BIU、通用寄存器、内部寄存器、ALU和标志位的工作原理。

指令集实现

o    模拟实现8086CPU的部分或全部指令集，包括数据传送指令、数据和逻辑运算指令、字符串控制指令、处理器控制指令和程序控制指令。

o    展示不同寻址方式下指令的执行过程。

指令队列和同步处理

o    模拟指令队列的工作过程，展示指令读取和执行的同步处理模式。

汇编语言编程

o    编写简单的汇编语言程序，使用不同的伪指令和指令，实现特定的功能。

o    展示如何使用段定义伪指令、数据定义伪指令和过程定义伪指令来组织程序结构。

**六、要求**

深入理解8086CPU的原理和指令集，能够准确描述各部分的功能和工作流程。

熟练掌握汇编语言的编程方法和技巧，能够编写简单的汇编语言程序。

提交的作业应包含详细的文档和代码，清晰展示8086CPU的原理、指令集的实现过程以及汇编语言编程的示例。





**附件：测试代码**

; 定义数据段

DATA SEGMENT

    ; 数据定义伪指令

    Num1 DB 5               ; 定义一个字节变量Num1, 值为5

    Num2 DW 10              ; 定义一个字变量Num2, 值为10

    Str1 DB 'Hello, ', 0    ; 定义一个以0结尾的字符串

    Str2 DB 'World!', 0

DATA ENDS

 

; 定义代码段

CODE SEGMENT

    ASSUME CS:CODE, DS:DATA, SS:STACK

 

START:

    ; 初始化数据段寄存器

    MOV AX, DATA

    MOV DS, AX

 

    ; 数据传送指令

    MOV AL, 15              ; 立即寻址

    MOV [Num1], AL          ; 直接寻址

    MOV BX, Num2            ; 寄存器间接寻址

    MOV CX, [BX]            ; 通过BX指向内存地址的内容加载到CX

    LEA SI, Str1            ; 将Str1的地址加载到SI (基址变址寻址)

    PUSH CX                 ; 将CX压入堆栈

    POP DX                  ; 将堆栈顶部值弹出到DX

 

    ; 数据和逻辑运算指令

    ADD AL, 3               ; 加法

    SUB AL, 2               ; 减法

    AND AL, 0Fh             ; 逻辑与

    OR AL, 01h              ; 逻辑或

    XOR AL, 02h             ; 逻辑异或

    NOT AL                  ; 逻辑非

 

    ; 字符串控制指令

    MOV SI, OFFSET Str1     ; 设置源地址

    MOV DI, OFFSET Str2     ; 设置目标地址

    MOVSB                   ; 传送一个字节从SI到DI

    INC SI

    INC DI

    CMPSB                   ; 比较SI和DI指向的字节

 

    ; 处理器控制指令

    STC                     ; 设置进位标志

    CLC                     ; 清除进位标志

 

    ; 程序控制指令

    JMP START               ; 无条件跳转到START标签

    ; CALL 和 RET 可以用来调用子程序和返回

    ; INT 和 IRET 用于处理中断

 

    ; 结束程序

    MOV AH, 4Ch             ; DOS终止功能号

    INT 21h                 ; 调用DOS中断

CODE ENDS

 

; 定义堆栈段

STACK SEGMENT STACK

    DW 128 DUP(0)           ; 为堆栈分配空间

STACK ENDS

 

END START  

---------------------------------------------------------------------------------------------------------------------------------------------------------------

2024/12/12  
**期末大作业**  
这是我自定的题目：  
1. 环境光感应
使用一个环境光传感器（如光敏电阻或光传感器模块），实时监测周围环境的光照强度。
将传感器的输出信号（模拟信号）通过一个ADC接口（如使用8255）转化为数字信号供8086处理。
CPU根据当前光照强度来判断是否需要打开照明设备。
2. 自动开关控制
当光照强度低于设定阈值时（如晚上或光线较暗的环境），系统自动开启灯光。
当光照强度超过设定阈值时（如白天或灯光充足时），系统自动关闭灯光。
你可以设定一个亮度阈值（比如500 lux以下时开灯，500 lux以上时关灯）。
3. 用户控制
使用按键或开关（连接到8255接口）允许用户手动控制灯光的开关。用户可以选择自动模式或手动模式。
在手动模式下，用户可以通过按键控制灯光的开关，或调节灯光亮度（如使用PWM调光）。
硬件组件
8086微处理器：作为主控单元，负责所有的运算与控制。
环境光传感器：用于检测周围的光照强度。
ADC接口芯片（8255）：将模拟光照信号转换为数字信号，供CPU处理。
定时器（8253）：用于定时控制灯光开关。
按键（8255接口）：用于手动控制灯光的开关或设置定时器。
LED指示灯：显示灯光开关状态。
数码管显示模块：显示环境光照强度和灯光状态。
**程序设计**

输入汇编指令解析  

光照强度读取与控制  


