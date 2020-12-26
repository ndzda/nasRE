#include "../io.cpp"

namespace vm
{
    const long long fill8byte = 0xffffffffffffffffLL;

    class nasVM
    {
        long long RZero = 0;
        long long R0 = 0, R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0;
        long long L0 = 0, L1 = 0, L2 = 0, L3 = 0, L4 = 0, L5 = 0, L6 = 0, L7 = 0;
        long long PC = 0;
        unsigned char *code, *heap, *stack;
        long long stackP = 0, stackSize = 0;
        long long *const reg[256] = {
            &RZero,                                         // 0x00 Zero register (0)零寄存器
            &PC,                                            // 0x01 Operating position register (PC)运行位置寄存器
            &stackP,                                        // 0x02 Stack pointer register 栈指针寄存器
            0, 0, 0, 0, 0,                                  // 0x03-0x07
            0, 0, 0, 0, 0, 0, 0, 0,                         // 0x08-0x0f
            &R0, &R1, &R2, &R3, &R4, &R5, &R6, &R7,         // 0x10-0x17
            &L0, &L1, &L2, &L3, &L4, &L5, &L6, &L7,         // 0x18-0x1f
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x2X
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x3X
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x4X
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x5X
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x6X
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x7X
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x8X
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x9X
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xaX
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xbX
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xcX
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xdX
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0xeX
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 0xfX
        };

// Get virtual register by number 通过编号获取虚拟寄存器
#define R(i) (*(reg[i]))
// Get double-precision virtual register by number 通过编号获取双精度浮点数虚拟寄存器
#define getDoubleR(i) (*((double *)(reg(i))))

        char loop()
        {
            register char flag;
            long long tmp;
            while (1)
            {
                switch (code[PC])
                {
                // 0x0x
                // do nothing 空指令
                case 0x00: // doNothing()
                    PC++;
                    break;
                // Copy one register to another 将一个寄存器复制到另一个寄存器
                case 0x01: // R[a]=R[b]
                    PC += 3;
                    R(code[PC - 2]) = R(code[PC - 1]);
                    break;
                // If the register is not 0, skip the next instruction(2byte) 如果寄存器不是0则跳过下一条指令(2byte)
                case 0x02: // if(R[a])
                    if (R(code[PC + 1]))
                        PC += 4;
                    else
                        PC += 2;
                    break;
                // Set the PC register to the value of the specified register 设置PC寄存器为指定寄存器(跳转指令)
                case 0x03: // goto(R[a])  (R[PC]=R[a])
                    PC = R(code[PC + 1]);
                    break;
                // Clear specified register 清除指定寄存器(设为0)
                case 0x04: // clearRegister(a)
                    flag = code[PC + 1];
                    if (flag & (1 << 0)) // RZero=0
                        RZero = 0;
                    if (flag & (1 << 1)) // Rx=0
                        R0 = 0, R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0;
                    if (flag & (1 << 2)) // Lx=0
                        L0 = 0, L1 = 0, L2 = 0, L3 = 0, L4 = 0, L5 = 0, L6 = 0, L7 = 0;
                    PC += 2;
                    break;
                // Set the register to the specified constant(8byte) 设置寄存器为指定常量
                case 0x05: // R[a]=(8byte)(b)
                    R(code[PC + 1]) = *((long long *)(code + (PC + 2)));
                    PC += 10;
                    break;
                // Set the register offset by a constant(1byte) 设置寄存器偏移一个常量
                case 0x06: // R[a]+=(1byte)(b)
                    R(code[PC + 1]) += (char)(code[PC + 2]);
                    PC += 3;
                    break;
                // Clear the specified byte of the specified register (8 bytes per register) 清除指定寄存器的指定字节(每个寄存器8个字节)
                // Each bit of variable b represents each byte. True is reserved, false is cleared 变量b的每一位代表每一个字节 真为保留 假为清除
                case 0x07: // clearRegisterByte(a,b)
                    tmp = 0;
                    flag = code[PC + 2];
                    if (flag & (1 << 0))
                        tmp |= 0xffLL << (0 * 8);
                    if (flag & (1 << 1))
                        tmp |= 0xffLL << (1 * 8);
                    if (flag & (1 << 2))
                        tmp |= 0xffLL << (2 * 8);
                    if (flag & (1 << 3))
                        tmp |= 0xffLL << (3 * 8);
                    if (flag & (1 << 4))
                        tmp |= 0xffLL << (4 * 8);
                    if (flag & (1 << 5))
                        tmp |= 0xffLL << (5 * 8);
                    if (flag & (1 << 6))
                        tmp |= 0xffLL << (6 * 8);
                    if (flag & (1 << 7))
                        tmp |= 0xffLL << (7 * 8);
                    R(code[PC + 1]) &= tmp;
                    PC += 3;
                    break;

                // 0x1x
                // Heap copy to registers 堆复制到寄存器
                // R[a]=heap[R[b]]
                case 0x10: // 1byte char
                    R(code[PC + 1]) = *((char *)(heap + R(code[PC + 2])));
                    PC += 3;
                    break;
                case 0x11: // 2byte short
                    R(code[PC + 1]) = *((short *)(heap + R(code[PC + 2])));
                    PC += 3;
                    break;
                case 0x12: // 4byte int
                    R(code[PC + 1]) = *((int *)(heap + R(code[PC + 2])));
                    PC += 3;
                    break;
                case 0x13: // 8byte long (long long)
                    R(code[PC + 1]) = *((long long *)(heap + R(code[PC + 2])));
                    PC += 3;
                    break;
                // Copy register to heap 寄存器复制到堆
                // heap[R[b]]=R[a]
                case 0x14: // 1byte char
                    *((char *)(heap + R(code[PC + 2]))) = (char)R(code[PC + 1]);
                    PC += 3;
                    break;
                case 0x15: // 2byte short
                    *((short *)(heap + R(code[PC + 2]))) = (short)R(code[PC + 1]);
                    PC += 3;
                    break;
                case 0x16: // 4byte int
                    *((int *)(heap + R(code[PC + 2]))) = (int)R(code[PC + 1]);
                    PC += 3;
                    break;
                case 0x17: // 8byte long (long long)
                    *((long long *)(heap + R(code[PC + 2]))) = (long long)R(code[PC + 1]);
                    PC += 3;
                    break;
                // Stack copy to registers 栈复制到寄存器
                // R[a]=stack[R[b]]
                case 0x18: // 1byte char
                    R(code[PC + 1]) = *((char *)(stack + stackP + R(code[PC + 2])));
                    PC += 3;
                    break;
                case 0x19: // 2byte short
                    R(code[PC + 1]) = *((short *)(stack + stackP + R(code[PC + 2])));
                    PC += 3;
                    break;
                case 0x1a: // 4byte int
                    R(code[PC + 1]) = *((int *)(stack + stackP + R(code[PC + 2])));
                    PC += 3;
                    break;
                case 0x1b: // 8byte long (long long)
                    R(code[PC + 1]) = *((long long *)(stack + stackP + R(code[PC + 2])));
                    PC += 3;
                    break;
                // Copy register to stack 寄存器复制到栈
                // stack[R[b]]=R[a]
                case 0x1c: // 1byte char
                    *((char *)(stack + stackP + R(code[PC + 2]))) = (char)R(code[PC + 1]);
                    PC += 3;
                    break;
                case 0x1d: // 2byte short
                    *((short *)(stack + stackP + R(code[PC + 2]))) = (short)R(code[PC + 1]);
                    PC += 3;
                    break;
                case 0x1e: // 4byte int
                    *((int *)(stack + stackP + R(code[PC + 2]))) = (int)R(code[PC + 1]);
                    PC += 3;
                    break;
                case 0x1f: // 8byte long (long long)
                    *((long long *)(stack + stackP + R(code[PC + 2]))) = (long long)R(code[PC + 1]);
                    PC += 3;
                    break;
                // 0x2x
                // 0x3x
                // ADD 加
                case 0x30: // R[a]=R[b]+R[c]
                    R(code[PC + 1]) = R(code[PC + 2]) + R(code[PC + 3]);
                    PC += 4;
                    break;
                // SUB 减
                case 0x31: // R[a]=R[b]-R[c]
                    R(code[PC + 1]) = R(code[PC + 2]) - R(code[PC + 3]);
                    PC += 4;
                    break;
                // MUL 乘
                case 0x32: // R[a]=R[b]*R[c]
                    R(code[PC + 1]) = R(code[PC + 2]) * R(code[PC + 3]);
                    PC += 4;
                    break;
                // DIV 除
                case 0x33: // R[a]=R[b]/R[c]
                    R(code[PC + 1]) = R(code[PC + 2]) / R(code[PC + 3]);
                    PC += 4;
                    break;
                // MOD 求余
                case 0x34: // R[a]=R[b]%R[c]
                    R(code[PC + 1]) = R(code[PC + 2]) % R(code[PC + 3]);
                    PC += 4;
                    break;
                // POW 求幂
                case 0x35: // R[a]=pow(R[b],R[c])
                    // R(code[PC+1])=pow((double)R(code[PC+2]),(int)R(code[PC+3]));
                    PC += 4;
                    break;
                // UNM 求相反数(乘-1)
                case 0x36: // R[a]=R[b]*-1
                    R(code[PC + 1]) = R(code[PC + 2]) * -1;
                    PC += 3;
                    break;
                // is 0 是0
                case 0x3a: // R[a]=(R[b]!=0)
                    R(code[PC + 1]) = (bool)R(code[PC + 2]);
                    PC += 3;
                    break;
                // not 0 不是0
                case 0x3b: // R[a]=(R[b]!=0)
                    break;
                // 0xEx
                // Apply for binary code area memory 申请字节码区内存
                // The parameters are the starting position and length in the slave heap memory, and the return value is the binary code memory number
                // 参数为从堆内存中的起始位置与长度,返回二进制码内存编号
                case 0xe0: // R[a]=binCode_memory(R[b],R[c])
                    break;
                // Create a new thread 新建一个线程
                case 0xe3: // thread_start(R[a])
                    break;
                // 0xFx
                // Exit the virtual machine (exit the thread) 退出虚拟机(退出线程)
                case 0xff: // exit(a)
                    flag = code[PC + 1];
                    goto endOfLoop;
                    break;
                }
            }
        endOfLoop:
            return flag;
        }
    };
} // namespace vm