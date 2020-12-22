#include "io.cpp"

class nasVM
{
    long long RZero = 0;
    long long R0 = 0, R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0;
    long long L0 = 0, L1 = 0, L2 = 0, L3 = 0, L4 = 0, L5 = 0, L6 = 0, L7 = 0;
    long long PC = 0;
    char *code, *heap, *stack;
    long long stackP = 0, stackSize = 0;

    long long inline &R(char r) // Get virtual register by number 通过编号获取虚拟寄存器
    {
        switch (r)
        {
        case 0x00: // Zero register (0)零寄存器
            return RZero;
        case 0x01: // Operating position register (PC)运行位置寄存器
            return PC;
        case 0x02:
            return stackP; // Top of Stack Pointer Register 栈顶指针寄存器

        // 16 main registers 16个主寄存器
        case 0x10:
            return R0;
        case 0x11:
            return R1;
        case 0x12:
            return R2;
        case 0x13:
            return R3;
        case 0x14:
            return R4;
        case 0x15:
            return R5;
        case 0x16:
            return R6;
        case 0x17:
            return R7;
        case 0x20:
            return L0;
        case 0x21:
            return L1;
        case 0x22:
            return L2;
        case 0x23:
            return L3;
        case 0x24:
            return L4;
        case 0x25:
            return L5;
        case 0x26:
            return L6;
        case 0x27:
            return L7;
        }
        return RZero;
    }

    int loop()
    {
        char flag;
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
                PC+=2;
                break;
            // 0x1x
            // Heap copy to registers 堆复制到寄存器
            // R[a]=heap[R[b]]
            case 0x10: // 1byte char
                R(code[PC+1])=*((char*)(heap+R(code[PC+2])));
                PC+=3;
                break;
            case 0x11: // 2byte short
                R(code[PC+1])=*((short*)(heap+R(code[PC+2])));
                PC+=3;
                break;
            case 0x12: // 4byte int
                R(code[PC+1])=*((int*)(heap+R(code[PC+2])));
                PC+=3;
                break;
            case 0x13: // 8byte long (long long)
                R(code[PC+1])=*((long long*)(heap+R(code[PC+2])));
                PC+=3;
                break;
            // Copy register to heap 寄存器复制到堆
            // heap[R[b]]=R[a]
            case 0x14: // 1byte char
                *((char*)(heap+R(code[PC+2])))=(char)R(code[PC+1]);
                PC+=3;
                break;
            case 0x15: // 2byte short
                *((short*)(heap+R(code[PC+2])))=(short)R(code[PC+1]);
                PC+=3;
                break;
            case 0x16: // 4byte int
                *((int*)(heap+R(code[PC+2])))=(int)R(code[PC+1]);
                PC+=3;
                break;
            case 0x17: // 8byte long (long long)
                *((long long*)(heap+R(code[PC+2])))=(long long)R(code[PC+1]);
                PC+=3;
                break;
            // Stack copy to registers 栈复制到寄存器
            // R[a]=stack[R[b]]
            case 0x18: // 1byte char
                R(code[PC+1])=*((char*)(stack+stackP+R(code[PC+2])));
                PC+=3;
                break;
            case 0x19: // 2byte short
                R(code[PC+1])=*((short*)(stack+stackP+R(code[PC+2])));
                PC+=3;
                break;
            case 0x1a: // 4byte int
                R(code[PC+1])=*((int*)(stack+stackP+R(code[PC+2])));
                PC+=3;
                break;
            case 0x1b: // 8byte long (long long)
                R(code[PC+1])=*((long long*)(stack+stackP+R(code[PC+2])));
                PC+=3;
                break;
            // Copy register to stack 寄存器复制到栈
            // stack[R[b]]=R[a]
            case 0x1c: // 1byte char
                *((char*)(stack+stackP+R(code[PC+2])))=(char)R(code[PC+1]);
                PC+=3;
                break;
            case 0x1d: // 2byte short
                *((short*)(stack+stackP+R(code[PC+2])))=(short)R(code[PC+1]);
                PC+=3;
                break;
            case 0x1e: // 4byte int
                *((int*)(stack+stackP+R(code[PC+2])))=(int)R(code[PC+1]);
                PC+=3;
                break;
            case 0x1f: // 8byte long (long long)
                *((long long*)(stack+stackP+R(code[PC+2])))=(long long)R(code[PC+1]);
                PC+=3;
                break;
            // 0x2x
            //ADD 加
            case 0x20:// R[a]=R[b]+R[c]
                R(code[PC+1])=R(code[PC+2])+R(code[PC+3]);
                PC+=4;
            break;
            //SUB 减
            case 0x21:// R[a]=R[b]-R[c]
                R(code[PC+1])=R(code[PC+2])-R(code[PC+3]);
                PC+=4;
            break;
            //MUL 乘
            case 0x22:// R[a]=R[b]*R[c]
                R(code[PC+1])=R(code[PC+2])*R(code[PC+3]);
                PC+=4;
            break;
            //DIV 除
            case 0x23:// R[a]=R[b]/R[c]
                R(code[PC+1])=R(code[PC+2])/R(code[PC+3]);
                PC+=4;
            break;
            //MOD 求余
            case 0x24:// R[a]=R[b]%R[c]
                R(code[PC+1])=R(code[PC+2])%R(code[PC+3]);
                PC+=4;
            break;
            //POW 求幂
            case 0x25:// R[a]=pow(R[b],R[c])
                //R(code[PC+1])=pow((double)R(code[PC+2]),(int)R(code[PC+3]));
                PC+=4;
            break;
            //UNM 求相反数(乘-1)
            case 0x26:// R[a]=R[b]*-1
                R(code[PC+1])=R(code[PC+2])*-1;
                PC+=3;
            break;
            //is 0 是0
            case 0x2a:// R[a]=(R[b]==0)
            break;
            //not 0 不是0
            case 0x2b:// R[a]=(R[b]!=0)
            break;
            }
        }
    }
};