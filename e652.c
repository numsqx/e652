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


int e652_effaddr01 (word opcode)
{
  char cc = opcode & 0x3;
  char bbb = (opcode >> 2) & 0x7;
  if (cc != 1) return -1;
  switch (bbb) {
    case A1_INDX: return reslv_indn(E.X);
    case A1_ZPG:  return reslv_zpgn(0);
    case A1_IMM:  return reslv_imm();
    case A1_ABS:  return reslv_absn(0);
    case A1_INDY: return reslv_indn(E.Y);
    case A1_ZPGX: return reslv_zpgn(E.X);
    case A1_ABSY: return reslv_absn(E.Y);
    case A1_ABSX: return reslv_absn(E.X);
    default: return -1;
  }
}


word e652_read (dword addr)
{
  /* TODO: add hooks */
  return E.m[addr & MMAX];
}


void e652_write (dword addr, word val)
{
  /* TODO: add hooks */
  E.m[addr & MMAX] = val;
}


#define nextinst() *dtab[opcode = nextpc()]


int e652_exec (void)
{
  word opcode;
  static void *dtab[256] = {
    [0 ... 255] = &&unknown,
    #ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Woverride-init"
    #endif

    /* BRK */
    [0x00] = &&I_BRK,

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

    #ifdef __clang__
    #pragma clang diagnostic pop
    #endif
  };

unknown:
  goto nextinst();

I_BRK:
  /* TODO: real implementation */
  return H_DBUG;

I_LDA:
  E.A = e652_read(e652_effaddr01(opcode));
  Pset(EZ, E.A == 0);
  Pset(EN, E.A >> 7);
  goto nextinst();

I_STA:
  e652_write(e652_effaddr01(opcode), E.A);
  goto nextinst();
}
