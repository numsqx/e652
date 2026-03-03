/*
 * E652 -- A 6502 emulator.
 *
 * Copyright (c) 2026 Vincent Yanzee J. Tan
 * This project is licensed under the MIT License.
 * See LICENSE for more info.
 */

#include "e652.h"


/* Static definition of E652 execution context. */
struct e652 E;


void e652_reset (void)
{
  E.PC = b2(V_RES); /* reset vector */
  E.A = 0;
  E.X = 0;
  E.Y = 0;
  E.P = 0x34;
  E.SP = 0xFD;  /* 6502 starts here */
}


int e652_effaddr (word opcode, word n)
{
  word zp_addr, hi, lo; /* declaration only */
  char cc = opcode & 0x3;
  char bbb = (opcode >> 2) & 0x7;
  if (cc == 1) goto cc01;
  if (cc == 2) goto cc10;
  return EFF_INV;

cc01:
  switch (bbb) {
    case A1_INDX: n = E.X; goto indn;
    case A1_ZPG:  n = 0;   goto zpgn;
    case A1_IMM:  /* ~n */ goto imm;
    case A1_ABS:  n = 0;   goto absn;
    case A1_INDY: n = E.Y; goto indn;
    case A1_ZPGX: n = E.X; goto zpgn;
    case A1_ABSY: n = E.Y; goto absn;
    case A1_ABSX: n = E.X; goto absn;
    default: return EFF_INV;
  }

cc10:
  switch (bbb) {
    case A2_IMM:  /* ~n */ goto imm;
    case A2_ZPG:  n = 0;   goto zpgn;
    case A2_ACC:  /* ~n */ goto acc;
    case A2_ABS:  n = 0;   goto absn;
    case A2_ZPGN:          goto zpgn;
    case A2_ABSN:          goto absn;
    default: return EFF_INV;
  }

imm:
  return (E.PC++) & MMAX;
acc:
  return EFF_ACCUM;

/* zeropage addressing */
zpgn:
  return (ZPAGE + ((e652_read(E.PC++) + n) & 0xff)) & MMAX;

/* absolute addressing */
absn:
  lo = e652_read(E.PC++);
  hi = e652_read(E.PC++);
  return ((lo | hi << 8) + n) & MMAX;

/* indirect addressing */
indn:
  zp_addr = (e652_read(E.PC++) + n) & 0xff;
  lo = e652_read(ZPAGE + zp_addr);
  hi = e652_read(ZPAGE + ((zp_addr + 1) & 0xff));
  return (lo | hi << 8) & MMAX;
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

    /* LDA */
    [0xA9] = &&I_LDA,
    [0xA5] = &&I_LDA,
    [0xB5] = &&I_LDA,
    [0xAD] = &&I_LDA,
    [0xBD] = &&I_LDA,
    [0xB9] = &&I_LDA,
    [0xA1] = &&I_LDA,
    [0xB1] = &&I_LDA,

    /* LDX */
    [0xA2] = &&I_LDX,
    [0xA6] = &&I_LDX,
    [0xB6] = &&I_LDX,
    [0xAE] = &&I_LDX,
    [0xBE] = &&I_LDX,

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
  E.A = e652_read(e652_effaddr(opcode, 0));
  Pset(EZ, E.A == 0);
  Pset(EN, E.A >> 7);
  goto nextinst();

I_LDX:
  E.X = e652_read(e652_effaddr(opcode, 0));
  Pset(EZ, E.X == 0);
  Pset(EN, E.X >> 7);
  goto nextinst();
}
