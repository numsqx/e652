/*
 * E652 -- A 6502 emulator.
 *
 * Copyright (c) 2026 Vincent Yanzee J. Tan
 * This project is licensed under the MIT License.
 * See LICENSE for more info.
 */

#include "e652.h"
#include "e652_extras.h"


/* Static definition of E652 execution context. */
struct e652 E;


void e652_reset (void)
{
  E.PC = b2(V_RES); /* reset vector */
  E.A = 0;
  E.X = 0;
  E.Y = 0;
  E.P = 0x34;
  E.S = 0xFD;  /* 6502 starts here */
}


int e652_effaddr01 (byte opcode)
{
  char cc = opcode & 0x3;
  char bbb = (opcode >> 2) & 0x7;
  if (cc != 1) return -1;
  switch (bbb) {
    case A1_INDX: return reslv_xind();
    case A1_ZPG:  return reslv_zpgn(0);
    case A1_IMM:  return reslv_imm();
    case A1_ABS:  return reslv_absn(0);
    case A1_INDY: return reslv_indy();
    case A1_ZPGX: return reslv_zpgn(E.X);
    case A1_ABSY: return reslv_absn(E.Y);
    case A1_ABSX: return reslv_absn(E.X);
    default: return -1;
  }
}


byte e652_read (word addr)
{
  /* TODO: add hooks */
  return E.m[addr & MMAX];
}


void e652_write (word addr, byte val)
{
  /* TODO: add hooks */
  E.m[addr & MMAX] = val;
}


#define updateZN(v) do { \
    Pset(EZ, ((v) & 0xff) == 0); \
    Pset(EN, ((v) & 0x80) != 0); \
  } while (0)


int e652_execnext (void)
{
  byte opcode;
  int M, R;
  static void *dtab[256] = {
    [0 ... 255] = &&illegal,
    #ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Woverride-init"
    #endif

    /* BRK */
    [0x00] = &&I_BRK,

    /* NOP: cc=10 */
    [0xEA] = &&I_NOP, /* NOP */

    /* status flag stuff */
    [0x38] = &&I_SEC,
    [0xF8] = &&I_SED,
    [0x78] = &&I_SEI,
    [0x18] = &&I_CLC,
    [0xD8] = &&I_CLD,
    [0x58] = &&I_CLI,
    [0xB8] = &&I_CLV,

    /* LDA: cc=01 */
    [0xA9] = &&I_LDA, /* IMM */
    [0xAD] = &&I_LDA, /* ABS */
    [0xA5] = &&I_LDA, /* ZPG */
    [0xA1] = &&I_LDA, /* INDX */
    [0xB1] = &&I_LDA, /* INDY */
    [0xB5] = &&I_LDA, /* ZPGX */
    [0xBD] = &&I_LDA, /* ABSX */
    [0xB9] = &&I_LDA, /* ABSY */

    /* STA: cc=01 */
    [0x8D] = &&I_STA, /* ABS */
    [0x85] = &&I_STA, /* ZPG */
    [0x81] = &&I_STA, /* INDX */
    [0x91] = &&I_STA, /* INDY */
    [0x95] = &&I_STA, /* ZPGX */
    [0x9D] = &&I_STA, /* ABSX */
    [0x99] = &&I_STA, /* ABSY */

    /* AND: cc=01 */
    [0x29] = &&I_AND, /* IMM */
    [0x2D] = &&I_AND, /* ABS */
    [0x25] = &&I_AND, /* ZPG */
    [0x21] = &&I_AND, /* INDX */
    [0x31] = &&I_AND, /* INDY */
    [0x35] = &&I_AND, /* ZPGX */
    [0x3D] = &&I_AND, /* ABSX */
    [0x39] = &&I_AND, /* ABSY */

    /* ORA: cc=01 */
    [0x09] = &&I_ORA, /* IMM */
    [0x0D] = &&I_ORA, /* ABS */
    [0x05] = &&I_ORA, /* ZPG */
    [0x01] = &&I_ORA, /* INDX */
    [0x11] = &&I_ORA, /* INDY */
    [0x15] = &&I_ORA, /* ZPGX */
    [0x1D] = &&I_ORA, /* ABSX */
    [0x19] = &&I_ORA, /* ABSY */

    /* EOR: cc=01 */
    [0x49] = &&I_EOR, /* IMM */
    [0x4D] = &&I_EOR, /* ABS */
    [0x45] = &&I_EOR, /* ZPG */
    [0x41] = &&I_EOR, /* INDX */
    [0x51] = &&I_EOR, /* INDY */
    [0x55] = &&I_EOR, /* ZPGX */
    [0x5D] = &&I_EOR, /* ABSX */
    [0x59] = &&I_EOR, /* ABSY */

    /* CMP: cc=01 */
    [0xC9] = &&I_CMP, /* IMM */
    [0xCD] = &&I_CMP, /* ABS */
    [0xC5] = &&I_CMP, /* ZPG */
    [0xC1] = &&I_CMP, /* INDX */
    [0xD1] = &&I_CMP, /* INDY */
    [0xD5] = &&I_CMP, /* ZPGX */
    [0xDD] = &&I_CMP, /* ABSX */
    [0xD9] = &&I_CMP, /* ABSY */

    /* ADC: cc=01 */
    [0x69] = &&I_ADC, /* IMM */
    [0x6D] = &&I_ADC, /* ABS */
    [0x65] = &&I_ADC, /* ZPG */
    [0x61] = &&I_ADC, /* INDX */
    [0x71] = &&I_ADC, /* INDY */
    [0x75] = &&I_ADC, /* ZPGX */
    [0x7D] = &&I_ADC, /* ABSX */
    [0x79] = &&I_ADC, /* ABSY */

    #ifdef __clang__
    #pragma clang diagnostic pop
    #endif
  };

  goto *dtab[opcode = nextpc()];

illegal:
  return H_ILL;

I_NOP:
  return H_OK;

I_BRK:
  /* TODO: real implementation */
  return H_DBUG;

I_SEC:
  Pset(EC, 1);
  return H_OK;

I_SED:
  Pset(ED, 1);
  return H_OK;

I_SEI:
  Pset(EI, 1);
  return H_OK;

I_CLC:
  Pset(EC, 0);
  return H_OK;

I_CLD:
  Pset(ED, 0);
  return H_OK;

I_CLI:
  Pset(EI, 0);
  return H_OK;

I_CLV:
  Pset(EV, 0);
  return H_OK;

I_LDA:
  E.A = e652_read(e652_effaddr01(opcode));
  updateZN(E.A);
  return H_OK;

I_STA:
  e652_write(e652_effaddr01(opcode), E.A);
  return H_OK;

I_AND:
  E.A &= e652_read(e652_effaddr01(opcode));
  updateZN(E.A);
  return H_OK;

I_ORA:
  E.A |= e652_read(e652_effaddr01(opcode));
  updateZN(E.A);
  return H_OK;

I_EOR:
  E.A ^= e652_read(e652_effaddr01(opcode));
  updateZN(E.A);
  return H_OK;

I_CMP:
  M = e652_read(e652_effaddr01(opcode));
  R = E.A - M;
  updateZN(R);
  Pset(EC, E.A >= M);
  return H_OK;

I_ADC:
  M = e652_read(e652_effaddr01(opcode));
  R = E.A + M + ((E.P & EC) ? 1 : 0);
  updateZN(R);
  Pset(EC, (R & 0x100) != 0);
  // if sign of A and M are same, then
  // R's sign must also be same
  Pset(EV, (~(E.A ^ M) & (M ^ R) & 0x80) != 0);
  /* TODO: decimal mode */
  E.A = R;
  return H_OK;
}
