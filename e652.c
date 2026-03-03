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


word *e652_effaddr (word opcode)
{
  word zp_addr, hi, lo; /* declaration only */
  char cc = opcode & 0x3;
  char bbb = (opcode >> 2) & 0x7;
  if (cc == 1) goto cc01;
  if (cc == 2) goto cc10;
  return 0; /* NULL */

cc01:
  switch (bbb) {
    case A1_INDX:
      zp_addr = (E.m[E.PC++] + E.X) & 0xff;
      lo = E.m[ZPAGE + zp_addr];
      hi = E.m[ZPAGE + ((zp_addr + 1) & 0xff)];
      return &E.m[lo | hi << 8];
    case A1_ZPG:
      return &E.m[ZPAGE + E.m[E.PC++]];
    case A1_IMM:
      return &E.m[E.PC++];
    case A1_ABS:
      lo = E.m[E.PC++];
      hi = E.m[E.PC++];
      return &E.m[lo | hi << 8];
    case A1_INDY:
      zp_addr = (E.m[E.PC++] + E.Y) & 0xff;
      lo = E.m[ZPAGE + zp_addr];
      hi = E.m[ZPAGE + ((zp_addr + 1) & 0xff)];
      return &E.m[lo | hi << 8];
    case A1_ZPGX:
      return &E.m[ZPAGE + ((E.m[E.PC++] + E.X) & 0xff)];
    case A1_ABSY:
      lo = E.m[E.PC++];
      hi = E.m[E.PC++];
      return &E.m[((lo | hi << 8) + E.Y) & 0xffff];
    case A1_ABSX:
      lo = E.m[E.PC++];
      hi = E.m[E.PC++];
      return &E.m[((lo | hi << 8) + E.X) & 0xffff];
    default:
      return 0;
  }

cc10:
  switch (bbb) {
    case A2_IMM:
      return &E.m[E.PC++];
    case A2_ZPG:
      return &E.m[ZPAGE + E.m[E.PC++]];
    case A2_ACC:
      return &E.A;
    case A2_ABS:
      lo = E.m[E.PC++];
      hi = E.m[E.PC++];
      return &E.m[lo | hi << 8];
    case A2_ZPGY:
      return &E.m[ZPAGE + ((E.m[E.PC++] + E.Y) & 0xff)];
    case A2_ABSY:
      lo = E.m[E.PC++];
      hi = E.m[E.PC++];
      return &E.m[((lo | hi << 8) + E.Y) & 0xffff];
    case A2_ABSX:
      lo = E.m[E.PC++];
      hi = E.m[E.PC++];
      return &E.m[((lo | hi << 8) + E.X) & 0xffff];
    default:
      return 0;
  }
}


int e652_exec (void)
{
  static void *dtab[256];
  word opcode;

  /* LDA */
  dtab[0xA9] = &&I_LDA;
  dtab[0xA5] = &&I_LDA;
  dtab[0xB5] = &&I_LDA;
  dtab[0xAD] = &&I_LDA;
  dtab[0xBD] = &&I_LDA;
  dtab[0xB9] = &&I_LDA;
  dtab[0xA1] = &&I_LDA;
  dtab[0xB1] = &&I_LDA;
br:
  opcode = nextpc();
  if (dtab[opcode])
    goto *dtab[opcode];
  goto br;

I_LDA:
  E.A = *e652_effaddr(opcode);
  Pset(EZ, E.A == 0);
  Pset(EN, E.A >> 7);
  goto br;
}
