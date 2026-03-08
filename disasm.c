#include <stdio.h>
#include "disasm.h"

struct opinfo {
  const char *M;
  char A;
  char L;
};

struct opinfo defs[256] = {
#define OP(O,M,A,L,C) [O] = (struct opinfo){ #M, A, L },
#include "opcodes.def"
#undef OP
};

void print_operand (byte *code, word addr, struct opinfo *op);


void print_disasm (byte *code, word addr)
{
  struct opinfo op = defs[code[0]];
  printf("%04x: ", addr);
  int pad = 10 - op.L * 3;
  for (int i = 0; i < op.L; i++)
    printf("%02x ", code[i]);
  for (int i = 0; i < pad; i++)
    fputc(' ', stdout);
  printf("%s ", op.M);
  print_operand(code, addr, &op);
  fputc('\n', stdout);
}


void print_operand (byte *code, word addr, struct opinfo *op)
{
  switch (op->A) {
    case A_INDX: printf("($%02X,X)", code[1]); break;
    case A_ZPG:  printf("$%02X", code[1]); break;
    case A_IMM:  printf("#$%02X\t; %d", code[1], code[1]); break;
    case A_ABS:  printf("$%04X", code[1] | code[2] << 8); break;
    case A_INDY: printf("($%02X),Y", code[1]); break;
    case A_ZPGX: printf("$%02X,X", code[1]); break;
    case A_ABSY: printf("$%04X,Y", code[1] | code[2] << 8); break;
    case A_ABSX: printf("$%04X,X", code[1] | code[2] << 8); break;
    case A_REL:  printf("*%c$%02X\t; %04X", (code[1] & 0x80) ? '-' : '+',
                     code[1], addr + op->L + code[1]); break;
    case A_IND:  printf("($%04X)", code[1] | code[2] << 8); break;
    case A_NONE: break;
    case A_ACC:  printf("A"); break;
    case A_ZPGY: printf("$%02X,Y", code[1]); break;
  }
}
