#pragma once
#include "z80.h"

#define DEBUG 0
#define BUFFER_SIZE 300

typedef struct GBPlatform {
    struct {
		union {
			struct {
				u8 f;
				u8 a;
			};
			u16 af;
		};
	};

	struct {
		union {
			struct {
				u8 c;
				u8 b;
			};
			u16 bc;
		};
	};

	struct {
		union {
			struct {
				u8 e;
				u8 d;
			};
			u16 de;
		};
	};

	struct {
		union {
			struct {
				u8 l;
				u8 h;
			};
			u16 hl;
		};
	};

	u16 sp;
	u16 pc;
    bool zero;
    bool sub;
    bool half_carry;
    bool carry;
    bool interrupts;

    u8 *mem;
    u8 *rom;
    u8 scanline;
    u8 lcd_state;
} GBPlatform;

u32 get_reg_val(GBPlatform *gb, Type t) {
    u32 ret;
    switch (t) {
        case A: {
            ret = gb->a;
        } break;
        case B: {
            ret = gb->b;
        } break;
        case C: {
            ret = gb->c;
        } break;
        case D: {
            ret = gb->d;
        } break;
        case E: {
            ret = gb->e;
        } break;
        case L: {
            ret = gb->l;
        } break;
        case H: {
            ret = gb->h;
        } break;
        case Hl: {
            ret = gb->hl;
        } break;
        case Bc: {
            ret = gb->bc;
        } break;
        case De: {
            ret = gb->de;
        } break;
        case RelDe: {
            ret = gb->mem[gb->de];
        } break;
        case RelHl: {
            ret = gb->mem[gb->hl];
        } break;
        case HiHl: {
            ret = gb->mem[gb->h];
        } break;
        default: {
            printf("Register Detection Error!\n");
            ret = 0xFFFFFFFF;
        }
    }
    return ret;
}

void gb_load(Instruction inst, GBPlatform *gb) {
    if (!is_not_data_type(inst.type2)) {
        switch (inst.type1) {
            case A: {
                if (inst.type2 == Data_8_Addr) {
                    if (inst.val2 < 0x7F) {
                        switch (inst.val2) {
                            case 0x40: {
                                gb->a = gb->lcd_state;
                            } break;
                            case 0x44: {
                                gb->a = gb->scanline;
                                gb->scanline++;
                            } break;
                            default: {
                                puts("[LOAD-IO] nope");
                            }
                        }
                    } else if (inst.val2 != 0xFF) {
                        gb->a = gb->mem[0xFF00+inst.val2];
                    } else {
                        puts("[LOAD-IO] nope");
                    }
                } else {
                    gb->a = inst.val2;
                }
            } break;
            case B: {
                gb->b = inst.val2;
            } break;
            case C: {
                gb->c = inst.val2;
            } break;
            case D: {
                gb->d = inst.val2;
            } break;
            case E: {
                gb->e = inst.val2;
            } break;
            case Sp: {
                gb->sp = inst.val2;
            } break;
            case Bc: {
                gb->bc = inst.val2;
            } break;
            case H: {
                gb->h = inst.val2;
            } break;
            case L: {
                gb->l = inst.val2;
            } break;
            case Hl: {
                gb->hl = inst.val2;
            } break;
            case De: {
                gb->de = inst.val2;
            } break;
            case LoHl: {
                gb->l = inst.val2;
            } break;
            case RelHl: {
                gb->mem[gb->hl] = inst.val2;
            } break;
            default: {
                puts("[LOAD-data] nope");
            }
        }
    } else {
        switch (inst.type1) {
            case A: {
                gb->a = get_reg_val(gb, inst.type2);
            } break;
            case B: {
                gb->b = get_reg_val(gb, inst.type2);
            } break;
            case C: {
                gb->c = get_reg_val(gb, inst.type2);
            } break;
            case D: {
                gb->d = get_reg_val(gb, inst.type2);
            } break;
            case E: {
                gb->e = get_reg_val(gb, inst.type2);
            } break;
            case Sp: {
                gb->sp = get_reg_val(gb, inst.type2);
            } break;
            case H: {
                gb->h = get_reg_val(gb, inst.type2);
            } break;
            case L: {
                gb->l = get_reg_val(gb, inst.type2);
            } break;
            case Hl: {
                gb->hl = get_reg_val(gb, inst.type2);
            } break;
            case LoHl: {
                gb->l = get_reg_val(gb, inst.type2);
            } break;
            case HiHl: {
                gb->h = get_reg_val(gb, inst.type2);
            } break;
            case Data_8_Addr: {
                gb->mem[0xFF00+inst.val1] = get_reg_val(gb, inst.type2);
            } break;
            case Data_16_Imm: {
                gb->mem[inst.val1] = get_reg_val(gb, inst.type2);
            } break;
            case RelHl: {
                gb->mem[gb->hl] = get_reg_val(gb, inst.type2);
            } break;
            case RelDe: {
                gb->mem[gb->de] = get_reg_val(gb, inst.type2);
            } break;
            case RelC: {
                gb->mem[0xFF00+gb->c] = get_reg_val(gb, inst.type2);
            } break;
            default: {
                puts("[LOAD-reg] nope");
            }
        }
    }
}

void gb_xor(Instruction inst, GBPlatform *gb) {
    switch (inst.type1) {
        case A: {
            gb->a = 0;
            gb->zero = true;
        } break;
        default: {
            puts("[XOR] nope");
        }
    }
    gb->sub = false;
    gb->half_carry = false;
    gb->carry = false;
}

void gb_and(Instruction inst, GBPlatform *gb) {
    switch (inst.type1) {
        case A: {
            gb->a = gb->a & gb->a;

            if (gb->a == 0) {
                gb->zero = true;
            }
        } break;
        case B: {
            gb->a = gb->a & gb->b;

            if (gb->a == 0) {
                gb->zero = true;
            }
        } break;
        case Data_8: {
            gb->a = gb->a & inst.val1;

            if (gb->a == 0) {
                gb->zero = true;
            }
        } break;
        default: {
            puts("[AND] nope");
        }
    }
    gb->sub = false;
    gb->half_carry = true;
    gb->carry = false;
}

void gb_cp(Instruction inst, GBPlatform *gb) {
    u16 tmp = 0;
    if (inst.type1 != Data_8) {
        tmp = get_reg_val(gb, inst.type1);
    } else {
        tmp = inst.val1;
    }

    gb->zero = false;
    gb->half_carry = false;
    if (gb->a == tmp) {
        gb->zero = true;
    } else if (gb->a < tmp) {
        gb->carry = true;
    }
    gb->sub = true;
}

void gb_inc(Instruction inst, GBPlatform *gb) {
    switch (inst.type1) {
        case A: {
            gb->a++;
        } break;
        case B: {
            gb->b++;
        } break;
        case C: {
            gb->c++;
        } break;
        case D: {
            gb->d++;
        } break;
        case E: {
            gb->e++;
        } break;
        case H: {
            gb->h++;
        } break;
        case Hl: {
            gb->hl++;
        } break;
        case De: {
            gb->de++;
        } break;
        default: {
            puts("[INC] nope");
        }
    }
}

void gb_dec(Instruction inst, GBPlatform *gb) {
    switch (inst.type1) {
        case A: {
            gb->a--;
            if (gb->a == 0) {
                gb->zero = true;
            }
        } break;
        case B: {
            gb->b--;
            if (gb->b == 0) {
                gb->zero = true;
            }
        } break;
        case C: {
            gb->c--;
            if (gb->c == 0) {
                gb->zero = true;
            }
        } break;
        case D: {
            gb->d--;
            if (gb->d == 0) {
                gb->zero = true;
            }
        } break;
        case E: {
            gb->e--;
            if (gb->e == 0) {
                gb->zero = true;
            }
        } break;
        case Bc: {
            gb->bc--;
            if (gb->bc == 0) {
                gb->zero = true;
            }
        } break;
        default: {
            puts("[DEC] nope");
        }
    }

    gb->sub = true;
}

void gb_jump(Instruction inst, GBPlatform *gb) {
    switch (inst.op_k) {
        case Jr: {
            switch (inst.type1) {
                case Nz: {
                    if (!gb->zero) {
                        gb->pc += (i8)inst.val2;
                    }
                } break;
                case Ez: {
                    if (gb->zero) {
                        gb->pc += (i8)inst.val2;
                    }
                } break;
                case Data_8: {
                    gb->pc += (i8)inst.val2;
                } break;
                default: {
                    puts("[JR] nope");
                }
            }
        } break;
        case Jp: {
            gb->pc = inst.val1;
        } break;
        default: {
            puts("[JUMP] nope");
        }
    }
}

void gb_call(Instruction inst, GBPlatform *gb) {
    switch (inst.op_k) {
        case Call: {
            gb->pc += inst.length;

            gb->mem[gb->sp-1] = gb->pc >> 8;
            gb->mem[gb->sp] = (gb->pc << 8) >> 8;

            gb->sp -= 2;

            gb->pc = inst.val1;
        } break;
        case Rst: {
            gb->pc += inst.length;

            gb->mem[gb->sp-1] = gb->pc >> 8;
            gb->mem[gb->sp] = (gb->pc << 8) >> 8;

            gb->sp -= 2;

            u32 pc_base = 0x100;
            switch (inst.type1) {
                case Rst_00: {
                    gb->pc = pc_base + 0x00;
                } break;
                case Rst_38: {
                    gb->pc = pc_base + 0x38;
                } break;
                default: {
                    puts("[RST] nope");
                }
            }
        } break;
        default: {
            puts("[CALL] nope");
        }
    }
}

void gb_push(Instruction inst, GBPlatform *gb) {
    if (inst.type1 == Af) {
        gb->mem[gb->sp-1] = gb->a;
        gb->mem[gb->sp] = gb->zero << 7 | gb->sub << 6 | gb->half_carry << 5 | gb->carry << 4;
    } else {
        gb->mem[gb->sp-1] = ((u16)get_reg_val(gb, inst.type1)) >> 8;
        gb->mem[gb->sp] = (((u16)get_reg_val(gb, inst.type1)) << 8) >> 8;
    }

    gb->sp -= 2;
}

void gb_pop(Instruction inst, GBPlatform *gb) {
    switch (inst.type1) {
        case Bc: {
            gb->sp += 2;

            gb->bc = gb->mem[gb->sp-1] << 8 | gb->mem[gb->sp];
        } break;
        case Af: {
            gb->sp += 2;

            gb->a = gb->mem[gb->sp-1];
            u8 flags = gb->mem[gb->sp];
            gb->zero = (flags << 1) >> 7;
            gb->sub = (flags << 2) >> 6;
            gb->half_carry = (flags << 3) >> 5;
            gb->carry = (flags << 4) >> 4;
        } break;
        default: {
            puts("[POP] nope");
        }
    }
}

void gb_ret(Instruction inst, GBPlatform *gb) {
    if (inst.type1 != None) {
        if (inst.type1 == Ez && gb->zero) {
            gb->sp += 2;
            u16 ret_addr = gb->mem[gb->sp-1] << 8 | gb->mem[gb->sp];
            printf("RET to 0x%x\n", ret_addr);

            gb->pc = ret_addr;
        } else if (inst.type1 == Ez && !gb->zero) {
            gb->sp += 2;
            u16 ret_addr = gb->mem[gb->sp-1] << 8 | gb->mem[gb->sp];
            printf("RET to 0x%x\n", ret_addr);

            gb->pc = ret_addr;
        }
    } else {
        gb->sp += 2;
        u16 ret_addr = gb->mem[gb->sp-1] << 8 | gb->mem[gb->sp];
        printf("RET to 0x%x\n", ret_addr);

        gb->pc = ret_addr;
    }
}

void gb_rl(Instruction inst, GBPlatform *gb) {
    if (inst.op_k == Rla) {
        if ((gb->a >> 7) == 0) {
            gb->carry = false;
        } else {
            gb->carry = true;
        }
        gb->a = (gb->a << 1) | gb->carry;
    } else {
        switch (inst.type1) {
            case A: {
                if ((gb->a >> 7) == 0) {
                    gb->carry = false;
                } else {
                    gb->carry = true;
                }
                gb->a = (gb->a << 1) | gb->carry;
            } break;
            case C: {
                    if ((gb->c >> 7) == 0) {
                        gb->carry = false;
                    } else {
                        gb->carry = true;
                    }
                    gb->c = (gb->c << 1) | gb->carry;
            } break;
            default: {
                puts("[RL] nope");
            }
        }
    }
}

void gb_srl(Instruction inst, GBPlatform *gb) {
    switch (inst.type1) {
        case A: {
            gb->carry = !(1 & gb->a);
            gb->a = gb->a << inst.val1;
            if (gb->a == 0) {
                gb->zero = true;
            }
        } break;
        default: {
            puts("[SRL] nope");
        }
    }
    gb->sub = false;
    gb->half_carry = false;
}

void gb_bit(Instruction inst, GBPlatform *gb) {
    u8 bit = 8;
    switch (inst.type1) {
        case Zero: {
            bit = 0;
        } break;
        case One: {
            bit = 1;
        } break;
        case Two: {
            bit = 2;
        } break;
        case Three: {
            bit = 3;
        } break;
        case Four: {
            bit = 4;
        } break;
        case Five: {
            bit = 5;
        } break;
        case Six: {
            bit = 6;
        } break;
        case Seven: {
            bit = 7;
        } break;
        default: {
            printf("[BIT-ERR] %s\n", type_string(inst.type1));
        }
    }

    u8 reg = 0;
    if (bit < 8) {
        switch (inst.type2) {
            case H: {
                reg = gb->h;
            } break;
            default: {
                puts("[BIT] nope");
                return;
            }
        }
        if (!((1 << bit) & reg)) {
            gb->zero = true;
            gb->sub = false;
            gb->half_carry = true;
        }
    } else {
        puts("[BIT] nope");
    }
}

void gb_res(Instruction inst, GBPlatform *gb) {
    u8 bit = 8;
    switch (inst.type1) {
        case Zero: {
            bit = 0;
        } break;
        case One: {
            bit = 1;
        } break;
        case Two: {
            bit = 2;
        } break;
        case Three: {
            bit = 3;
        } break;
        case Four: {
            bit = 4;
        } break;
        case Five: {
            bit = 5;
        } break;
        case Six: {
            bit = 6;
        } break;
        case Seven: {
            bit = 7;
        } break;
        default: {
            printf("[RES-ERR] %s\n", type_string(inst.type1));
        }
    }

    if (bit < 8) {
        switch (inst.type2) {
            case A: {
                gb->a = gb->a & ~(1 << bit);
            } break;
            case H: {
                gb->h = gb->h & ~(1 << bit);
            } break;
            default: {
                puts("[RES] nope");
                return;
            }
        }
    } else {
        puts("[RES] nope");
    }
}

void gb_sub(Instruction inst, GBPlatform *gb) {
    switch (inst.type1) {
        case B: {
            gb->a = gb->a - gb->b;
            if (gb->a == 0) {
                gb->zero = true;
            }
        } break;
        case Data_8: {
            gb->a = gb->a - inst.val1;
            if (gb->a == 0) {
                gb->zero = true;
            }
        } break;
        default: {
            puts("[SUB] nope");
        }
    }
    gb->sub = true;
}

void gb_add(Instruction inst, GBPlatform *gb) {
    if (!is_not_data_type(inst.type2)) {
        switch (inst.type1) {
            case B: {
                gb->a = gb->a - gb->b;
                if (gb->a == 0) {
                    gb->zero = true;
                }
            } break;
            case Data_8: {
                gb->a = gb->a - inst.val1;
                if (gb->a == 0) {
                    gb->zero = true;
                }
            } break;
            default: {
                puts("[ADD] nope");
            }
        }
    } else {
        switch (inst.type1) {
            case B: {
                gb->a = gb->a + gb->b;
                if (gb->a == 0) {
                    gb->zero = true;
                }
            } break;
            case C: {
                gb->a = gb->a + gb->c;
                if (gb->a == 0) {
                    gb->zero = true;
                }
            } break;
            case D: {
                gb->a = gb->a + gb->d;
                if (gb->a == 0) {
                    gb->zero = true;
                }
            } break;
            case Hl: {
                gb->hl = gb->hl + (u16)get_reg_val(gb, inst.type2);
                if (gb->hl == 0) {
                    gb->zero = true;
                }
            } break;
            default: {
                puts("[ADD] nope");
            }
        }
    }

    gb->sub = false;
    gb->half_carry = false;
}

u8 *load_gb_boot(const char *filename) {
	FILE *rom_file = fopen(filename, "rb");

	if (!rom_file) {
		puts("Error opening BIOS!");
		return NULL;
	}

	fseek(rom_file, 0, SEEK_END);
	u64 length = ftell(rom_file);
	fseek(rom_file, 0, SEEK_SET);
	u8 *rom = (u8 *)malloc(length + 1);
	rom[length] = 0;

	fread(rom, 1, length, rom_file);

    return rom;
}

u8 *load_gb_rom(const char *filename) {
	FILE *rom_file = fopen(filename, "rb");

	if (!rom_file) {
		puts("Error opening ROM!");
		return NULL;
	}

	fseek(rom_file, 0, SEEK_END);
	u64 length = ftell(rom_file);
	fseek(rom_file, 0, SEEK_SET);
	u8 *rom = (u8 *)malloc(length + 0x100);
	rom[length] = 0;

	fread(rom, 1, length, rom_file);

    return rom;
}

void print_state(GBPlatform *gb) {
    printf("a: 0x%x, bc: 0x%x, de: 0x%x, hl: 0x%x, sp: 0x%x, pc: 0x%x\n", gb->a, gb->bc, gb->de, gb->hl, gb->sp, gb->pc);
    printf("zero: %d, sub: %d, half carry: %d, carry: %d\n", gb->zero, gb->sub, gb->half_carry, gb->carry);
    puts("--------------------------------------------------");
}

int load_and_execute_inst(GBPlatform *gb) {
    Instruction inst = parse_op(gb->rom, gb->pc, false);
    pretty_print_instruction(gb->pc, inst);
    //print_state(gb);
    //print_instruction(gb->pc, inst);
    bool no_inc = false;

    u32 ret = 0;
    switch (inst.op_k) {
        case Ld: {
            gb_load(inst, gb);
        } break;
        case Sub: {
            gb_sub(inst, gb);
        } break;
        case Add: {
            gb_add(inst, gb);
        } break;
        case Xor: {
            gb_xor(inst, gb);
        } break;
        case And: {
            gb_and(inst, gb);
        } break;
        case Bit: {
            gb_bit(inst, gb);
        } break;
        case Res: {
            gb_res(inst, gb);
        } break;
        case Jr: {
            gb_jump(inst, gb);
        } break;
        case Jp: {
            gb_jump(inst, gb);
            no_inc = true;
        } break;
        case Inc: {
            gb_inc(inst, gb);
        } break;
        case Dec: {
            gb_dec(inst, gb);
        } break;
        case Call: {
            gb_call(inst, gb);
            no_inc = true;
        } break;
        case Push: {
            gb_push(inst, gb);
        } break;
        case Pop: {
            gb_pop(inst, gb);
        } break;
        case Rl: {
            gb_rl(inst, gb);
        } break;
        case Rla: {
            gb_rl(inst, gb);
        } break;
        case Srl: {
            gb_srl(inst, gb);
        } break;
        case Cp: {
            gb_cp(inst, gb);
        } break;
        case Ret: {
            gb_ret(inst, gb);
            no_inc = true;
        } break;
        case Rst: {
            gb_call(inst, gb);
            no_inc = true;
        } break;
        case Di: { gb->interrupts = false; } break;
        case Nop: { } break;
        default: {
            puts("UNIMPLEMENTED!");
            ret = 1;
        }
    }

    if (!no_inc) {
        gb->pc += inst.length;
    }
    return ret;
}
