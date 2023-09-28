#include <stdio.h>
#include "shell.h"

//获取instruction的指定字段，其中inst代表32位指令




uint32_t get_op(uint32_t inst) { return inst >> 26; }
uint32_t get_rs(uint32_t inst) { return (inst >> 21) & 0x1f; }
uint32_t get_rt(uint32_t inst) { return (inst >> 16) & 0x1f; }
uint32_t get_rd(uint32_t inst) { return (inst >> 11) & 0x1f; }
uint32_t get_targetadr(uint32_t inst) { return inst & 0x3ffffff; }//获取跳转目标地址-26位
uint32_t get_imm(uint32_t inst) { return inst & 0xffff; }//获取立即数字段-16位
uint32_t get_shamt(uint32_t inst) { return (inst >> 6) & 0x1f; }
uint32_t get_func(uint32_t inst) { return inst & 0x3f; }

// 将16位立即数符号扩展到32位
uint32_t sign_ext(uint32_t imm) {
    int32_t signed_imm = *((int16_t*)&imm); // 将16位值视为有符号数
    uint32_t extended_imm = *((uint32_t*)&signed_imm); // 转换为32位数
    return extended_imm;
}

// 将字节（8位）符号扩展到32位
uint32_t sign_ext_byte(uint8_t imm) {
    int32_t signed_imm = *((int8_t*)&imm); // 将8位值视为有符号数
    uint32_t extended_imm = *((uint32_t*)&signed_imm); // 转换为32位数
    return extended_imm;
}

// 将半字（16位）符号扩展到32位，实现与16位立即数的符号扩展相同
uint32_t sign_ext_half(uint16_t imm) {
    int32_t signed_imm = *((int16_t*)&imm); // 将16位值视为有符号数
    uint32_t extended_imm = *((uint32_t*)&signed_imm); // 转换为32位数
    return extended_imm;
}

//将32位立即数零扩展（实际上，它保持不变，只需要转一下类型就可以了），
uint32_t zero_ext(uint32_t imm) { 
    return imm; // 32位值不变
}

// 将字节（8位）零扩展到32位，实现类似zero_ext
uint32_t zero_ext_byte(uint8_t imm) { 
    return imm; // 在C中，8位无符号值会自动零扩展到32位
}

// 将半字（16位）零扩展到32位，实现类似zero_ext
uint32_t zero_ext_half(uint16_t imm) { 
    return imm; // 在C中，16位无符号值会自动零扩展到32位
}

/* execute one instruction here. You should use CURRENT_STATE and modify
* values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
* access memory. */

void process_instruction()
{
    uint32_t inst = mem_read_32(CURRENT_STATE.PC);//获取到instruction

    uint32_t op = get_op(inst);
    uint32_t rs = get_rs(inst);
    uint32_t rt = get_rt(inst);
    uint32_t imm = get_imm(inst);
    uint32_t rd = get_rd(inst);
    uint32_t shamt = get_shamt(inst);
    uint32_t func = get_func(inst);
    uint32_t targetadr=get_targetadr(inst);

    switch (op) {
        case 0x0: {//R型指令
            switch (func) {
                case 0x0: {
                    // SLL逻辑左移
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x2: {
                    // SRL逻辑右移
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x3: {
                    // SRA算术右移
                    int32_t val = *((int32_t*)&CURRENT_STATE.REGS[rt]);
                    val = val >> shamt;
                    NEXT_STATE.REGS[rd] = val;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x4: {
                    // SLLV变量逻辑左移
                    uint32_t shamt = CURRENT_STATE.REGS[rs] & 0x1f;
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x6: {
                    // SRLV变量逻辑右移
                    uint32_t shamt = CURRENT_STATE.REGS[rs] & 0x1f;
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x7: {
                    // SRAV变量算术右移
                    int32_t val = *((int32_t*)&CURRENT_STATE.REGS[rt]);
                    uint32_t shamt = CURRENT_STATE.REGS[rs] & 0x1f;
                    val = val >> shamt;
                    NEXT_STATE.REGS[rd] = val;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x8: {
                    // JR无条件跳转
                    NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
                    break;
                }
                case 0x9: {
                    // JALR跳转并链接
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 4;
                    NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
                    break;
                }
                case 0xc: {
                    // SYSCALL系统调用，这里是按照实验手册的限制性定义进行编写
                    if (CURRENT_STATE.REGS[2] == 0x0a) {//v0寄存器就是reg[2]
                        RUN_BIT = FALSE;
                    } 
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    
                    break;
                }
                case 0x10: {
                    // MFHI从HI寄存器移动值
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x11: {
                    // MTHI将值移动到 HI 寄存器
                    NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x12: {
                    // MFLO从 LO 寄存器移动值
                    NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x13: {
                    // MTLO 将值移动到 LO 寄存器
                    NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x18: {
                    // MULT乘法
                    int64_t lhs = *((int32_t*)&CURRENT_STATE.REGS[rs]);
                    int64_t rhs = *((int32_t*)&CURRENT_STATE.REGS[rt]);
                    int64_t product = lhs * rhs;
                    uint64_t uint_product = *((uint32_t*)&product);
                    NEXT_STATE.HI =
                        (uint32_t)((uint_product >> 32) & 0xffffffff);
                    NEXT_STATE.LO = (uint32_t)(uint_product & 0xffffffff);
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x19: {
                    // MULTH无符号乘法
                    uint64_t lhs = CURRENT_STATE.REGS[rs];
                    uint64_t rhs = CURRENT_STATE.REGS[rt];
                    uint64_t product = lhs * rhs;

                    NEXT_STATE.HI = (uint32_t)((product >> 32) & 0xffffffff);
                    NEXT_STATE.LO = (uint32_t)(product & 0xffffffff);
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x1a: {
                    // DIV 除法
                    int32_t lhs = *((int32_t*)&CURRENT_STATE.REGS[rs]);
                    int32_t rhs = *((int32_t*)&CURRENT_STATE.REGS[rt]);
                    NEXT_STATE.LO = lhs / rhs;
                    NEXT_STATE.HI = lhs % rhs;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x1b: {
                    // DIVU无符号除法
                    uint32_t lhs = CURRENT_STATE.REGS[rs];
                    uint32_t rhs = CURRENT_STATE.REGS[rt];
                    NEXT_STATE.LO = lhs / rhs;
                    NEXT_STATE.HI = lhs % rhs;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x20: {
                    // ADD加法
                    NEXT_STATE.REGS[rd] =
                        CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x21: {
                    // ADDU无符号加法
                    NEXT_STATE.REGS[rd] =
                        CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x22: {
                    // SUB减法
                    NEXT_STATE.REGS[rd] =
                        CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x23: {
                    // SUBU无符号减法
                    NEXT_STATE.REGS[rd] =
                        CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x24: {
                    // AND位与
                    NEXT_STATE.REGS[rd] =
                        CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x25: {
                    // OR 位或
                    NEXT_STATE.REGS[rd] =
                        CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x26: {
                    // XOR位异或
                    NEXT_STATE.REGS[rd] =
                        CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x27: {
                    // NOR位非或
                    NEXT_STATE.REGS[rd] =
                        ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x2a: {
                    // SLT设置小于
                    int32_t lhs = *((int32_t*)&CURRENT_STATE.REGS[rs]);
                    int32_t rhs = *((int32_t*)&CURRENT_STATE.REGS[rt]);
                    NEXT_STATE.REGS[rd] = (lhs < rhs) ? 1 : 0;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                case 0x2b: {
                    // SLTU无符号设置小于
                    NEXT_STATE.REGS[rd] =
                        CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt] ? 1 : 0;
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    break;
                }
                default: {
                    printf("Unknown instruction: 0x%x\n", inst);
                    break;
                }
            }
            break;
        }
        case 0x8: {
            // ADDI立即数加法
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + sign_ext(imm);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        case 0x9: {
            // ADDIU无符号立即数加法
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + sign_ext(imm);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        case 0xc: {
            // ANDI立即数位与
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & zero_ext(imm);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        case 0xd: {
            // ORI立即数位或
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | zero_ext(imm);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        case 0xe: {
            // XORI立即数位异或
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ zero_ext(imm);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        case 0x4: {
            // BEQ相等跳转

            uint32_t offset = sign_ext(imm) << 2;

            if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]) {
                NEXT_STATE.PC = CURRENT_STATE.PC + offset + 4;
            } else {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;
        }
        case 0x1: {
            uint32_t offset = sign_ext(imm) << 2;

            switch (rt) {
                case 0x0: {
                    // BLTZ小于0分支
                    if ((CURRENT_STATE.REGS[rs] & 0x80000000) != 0) {
                        NEXT_STATE.PC = CURRENT_STATE.PC + offset + 4;
                    } else {
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    }
                    break;
                }
                case 0x10: {
                    // BLTZAL小于0跳转并链接
                    NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                    if ((CURRENT_STATE.REGS[rs] & 0x80000000) != 0) {
                        NEXT_STATE.PC = CURRENT_STATE.PC + offset + 4;
                    } else {
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    }
                    break;
                }
                case 0x1: {
                    // BGEZ大于等于0分支
                    if ((CURRENT_STATE.REGS[rs] & 0x80000000) == 0) {
                        NEXT_STATE.PC = CURRENT_STATE.PC + offset + 4;
                    } else {
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    }
                    break;
                }
                case 0x11: {
                    // BGEZAL大于等于0跳转并链接
                    NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                    if ((CURRENT_STATE.REGS[rs] & 0x80000000) == 0) {
                        NEXT_STATE.PC = CURRENT_STATE.PC + offset + 4;
                    } else {
                        NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                    }
                    break;
                }
            }
            break;
        }
        case 0x5: {
            // BNE不相等分支

            uint32_t offset = sign_ext(imm) << 2;

            printf("BNE: offset: %d, rs: %d, rt: %d\n", offset, rs, rt);

            printf("rs: 0x%08x\n", CURRENT_STATE.REGS[rs]);
            printf("rt: 0x%08x\n", CURRENT_STATE.REGS[rt]);

            if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]) {
                NEXT_STATE.PC = CURRENT_STATE.PC + offset + 4;
            } else {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            }
            break;
        }
        case 0x6: {
            // BLEZ小于等于0分支

            uint32_t offset = sign_ext(imm) << 2;

            if (rt == 0) {
                if ((CURRENT_STATE.REGS[rs] & 0x80000000) != 0 ||
                    CURRENT_STATE.REGS[rs] == 0) {
                    NEXT_STATE.PC = CURRENT_STATE.PC + offset + 4;
                } else {
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                }
            } else {
                // Illegal instruction
                printf("Illegal rt in BLEZ.\n");
            }
            break;
        }
        case 0x7: {
            // BGTZ大于0分支
            uint32_t offset = sign_ext(imm) << 2;

            printf("BGTZ: offset: 0x%08x, rs: %d, rt: %d, pc: 0x%08x\n", offset,
                   rs, rt, CURRENT_STATE.PC);

            if (rt == 0) {
                if ((CURRENT_STATE.REGS[rs] & 0x80000000) == 0 &&
                    CURRENT_STATE.REGS[rs] != 0) {
                    NEXT_STATE.PC = CURRENT_STATE.PC + offset + (uint32_t)4;
                    printf("PC: 0x%08x\n", NEXT_STATE.PC);
                } else {
                    NEXT_STATE.PC = CURRENT_STATE.PC + 4;
                }
            } else {
                // Illegal instruction
                printf("Illegal rt in BGTZ.\n");
            }
            break;
        }
        case 0x2: {
            // J无条件跳转
            NEXT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) | (targetadr << 2);
            break;
        }
        case 0x3: {
            // JAL跳转并链接
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
            NEXT_STATE.PC = (CURRENT_STATE.PC & 0xf0000000) | (targetadr << 2);
            break;
        }
        case 0xf: {
            // LUI将立即数加载到寄存器高16位

            if (rs == 0) {
                NEXT_STATE.REGS[rt] = imm << 16;
                NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            } else {
                // Illegal instruction
            }
            break;
        }
        case 0x20: {
            // LB从内存加载字节

            uint32_t addr = sign_ext(imm) + CURRENT_STATE.REGS[rs];

            uint8_t byte = mem_read_32(addr) & 0xff;

            NEXT_STATE.REGS[rt] = sign_ext_byte(byte);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        case 0x24: {
            // LBU从内存加载无符号字节

            uint32_t addr = sign_ext(imm) + CURRENT_STATE.REGS[rs];

            uint8_t byte = mem_read_32(addr) & 0xff;

            NEXT_STATE.REGS[rt] = zero_ext_byte(byte);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        case 0x21: {
            // LH从内存加载半字

            uint32_t addr = sign_ext(imm) + CURRENT_STATE.REGS[rs];

            uint16_t half = mem_read_32(addr) & 0xffff;

            NEXT_STATE.REGS[rt] = sign_ext_half(half);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        case 0x25: {
            // LHU从内存加载无符号半字

            uint32_t addr = sign_ext(imm) + CURRENT_STATE.REGS[rs];

            uint16_t half = mem_read_32(addr) & 0xffff;

            NEXT_STATE.REGS[rt] = zero_ext_half(half);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        case 0x23: {
            // LW从内存加载字

            uint32_t addr = sign_ext(imm) + CURRENT_STATE.REGS[rs];

            NEXT_STATE.REGS[rt] = mem_read_32(addr);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        case 0x28: {
            // SB将字节存储到内存

            uint32_t addr = sign_ext(imm) + CURRENT_STATE.REGS[rs];

            uint32_t val = (mem_read_32(addr) & 0xffffff00) |
                           (CURRENT_STATE.REGS[rt] & 0xff);

            mem_write_32(addr, val);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        case 0x29: {
            // SH将半字存储到内存

            uint32_t addr = sign_ext(imm) + CURRENT_STATE.REGS[rs];

            uint32_t val = (mem_read_32(addr) & 0xffff0000) |
                           (CURRENT_STATE.REGS[rt] & 0xffff);
            mem_write_32(addr, val);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        case 0x2b: {
            // SW 将字存储到内存
            uint32_t addr = sign_ext(imm) + CURRENT_STATE.REGS[rs];

            mem_write_32(addr, CURRENT_STATE.REGS[rt]);
            NEXT_STATE.PC = CURRENT_STATE.PC + 4;
            break;
        }
        default: {
            printf("wrong inst！at： 0x%08x\n", inst);
            break;
        }
    }
}    



