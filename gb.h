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
    u8 flags;

    u8 *mem;
} GBPlatform;

void set_zero(GBPlatform *gb) {
    gb->flags = (1 << 8) | gb->flags;
}

bool is_zero_set(GBPlatform *gb) {
    return ((1 << 8) & gb->flags);
}

void set_sub(GBPlatform *gb) {
    gb->flags = (1 << 7) | gb->flags;
}

void clear_sub(GBPlatform *gb) {
    gb->flags = (~(1 << 7)) & gb->flags;
}

void set_half_carry(GBPlatform *gb) {
    gb->flags = (1 << 6) | gb->flags;
}

void set_carry(GBPlatform *gb) {
    gb->flags = (1 << 5) | gb->flags;
}

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
        case Bc: {
            ret = gb->bc;
        } break;
        case De: {
            ret = gb->de;
        } break;
        case RelDe: {
            ret = gb->mem[gb->de];
        } break;
        default: {
            printf("Register Detection Error!");
            ret = 0xFFFFFFFF;
        }
    }
    return ret;
}

void gb_load(Instruction inst, GBPlatform *gb) {
    if (!is_not_data_type(inst.type2)) {
        switch (inst.type1) {
            case A: {
                gb->a = inst.val2;
            } break;
            case B: {
                gb->b = inst.val2;
            } break;
            case C: {
                gb->c = inst.val2;
            } break;
            case Sp: {
                gb->sp = inst.val2;
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
            case Sp: {
                gb->sp = get_reg_val(gb, inst.type2);
            } break;
            case Hl: {
                gb->hl = get_reg_val(gb, inst.type2);
            } break;
            case LoHl: {
                gb->l = get_reg_val(gb, inst.type2);
            } break;
            case Data_8_Addr: {
                gb->mem[0xFF00+inst.val1] = get_reg_val(gb, inst.type2);
            } break;
            case RelHl: {
                gb->mem[gb->hl] = get_reg_val(gb, inst.type2);
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
        } break;
        default: {
            puts("[XOR] nope");
        }
    }
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
        default: {
            puts("[INC] nope");
        }
    }
}

void gb_jump(Instruction inst, GBPlatform *gb) {
    switch (inst.op_k) {
        case Jr: {
            switch (inst.type1) {
                case Nz: {
                    if (is_zero_set(gb)) {
                        gb->pc = inst.val2;
                    }
                } break;
                default: {
                    puts("[JR] nope");
                }
            }
        } break;
        default: {
            puts("[JUMP] nope");
        }
    }
}

void gb_call(Instruction inst, GBPlatform *gb) {
    gb->mem[gb->sp] = gb->pc;
    gb->sp--;

    gb->pc = inst.val1;
}

void gb_push(Instruction inst, GBPlatform *gb) {
    gb->mem[gb->sp] = get_reg_val(gb, inst.type1);
    gb->sp--;
}

void gb_rl(Instruction inst, GBPlatform *gb) {
    
}

// implement this
void gb_pop(Instruction inst, GBPlatform *gb) {
    gb->mem[gb->sp] = gb->pc;
    gb->sp++;
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
        if ((1 << bit) & reg) {
            set_zero(gb);
            clear_sub(gb);
            set_half_carry(gb);
        }
    } else {
        puts("[BIT] nope");
    }
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

int load_and_execute_inst(u8 *rom, GBPlatform *gb) {
    Instruction inst = parse_op(rom, gb->pc, false);
    gb->pc += inst.length;

    u32 ret = 0;
    switch (inst.op_k) {
        case Ld: {
            gb_load(inst, gb);
        } break;
        case Xor: {
            gb_xor(inst, gb);
        } break;
        case Bit: {
            gb_bit(inst, gb);
        } break;
        case Jr: {
            gb_jump(inst, gb);
        } break;
        case Inc: {
            gb_inc(inst, gb);
        } break;
        case Call: {
            gb_call(inst, gb);
        } break;
        case Push: {
            gb_push(inst, gb);
        } break;
        case Rl: {
            gb_rl(inst, gb);
        } break;
        default: {
            puts("UNIMPLEMENTED!");
            ret = 1;
        }
    }
    pretty_print_instruction(inst);

    return ret;
}
