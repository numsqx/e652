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

/*
 * Address resolvers. Replace `n` with X, Y, or 0, accordingly.
 */
static inline word reslv_imm (void);
static inline word reslv_zpgn (byte n);
static inline word reslv_absn (byte n);
static inline word reslv_xind (void);
static inline word reslv_indy (void);
static inline word reslv_rel (void);
static inline word reslv_indir (void);


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
    case A_INDX: return reslv_xind();
    case A_ZPG:  return reslv_zpgn(0);
    case A_IMM:  return reslv_imm();
    case A_ABS:  return reslv_absn(0);
    case A_INDY: return reslv_indy();
    case A_ZPGX: return reslv_zpgn(E.X);
    case A_ABSY: return reslv_absn(E.Y);
    case A_ABSX: return reslv_absn(E.X);
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

    #define OP(O,M,A,L,C) [O] = &&I_##M,
    #include "opcodes.def"
    #undef OP

    #ifdef __clang__
    #pragma clang diagnostic pop
    #endif
  };

  goto *dtab[opcode = e652_read(E.PC++)];

illegal:
  return H_ILL;

I_NOP:
  return H_OK;

I_BRK:
  /* TODO: real implementation */
  return H_DBUG;

I_JMP:
  E.PC = (opcode == 0x4C) ? reslv_absn(0) : reslv_indir();
  return H_OK;

I_PHA:
  push(E.A);
  return H_OK;

I_PHP:
  push(E.P);
  return H_OK;

I_PLA:
  E.A = pop();
  return H_OK;

I_PLP:
  E.P = pop();
  return H_OK;

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

I_BCC:
  M = reslv_rel();
  if (!Phas(EC))
    E.PC = M;
  return H_OK;

I_BCS:
  M = reslv_rel();
  if (Phas(EC))
    E.PC = M;
  return H_OK;

I_BEQ:
  M = reslv_rel();
  if (Phas(EZ))
    E.PC = M;
  return H_OK;

I_BNE:
  M = reslv_rel();
  if (!Phas(EZ))
    E.PC = M;
  return H_OK;

I_BMI:
  M = reslv_rel();
  if (Phas(EN))
    E.PC = M;
  return H_OK;

I_BPL:
  M = reslv_rel();
  if (!Phas(EN))
    E.PC = M;
  return H_OK;

I_BVC:
  M = reslv_rel();
  if (!Phas(EV))
    E.PC = M;
  return H_OK;

I_BVS:
  M = reslv_rel();
  if (Phas(EV))
    E.PC = M;
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
_adc_bin:
  R = E.A + M + Phas(EC);
  updateZN(R);
  Pset(EC, (R & 0x100) != 0);
  // if sign of A and M are same, then
  // R's sign must also be same
  Pset(EV, (~(E.A ^ M) & (M ^ R) & 0x80) != 0);
  /* TODO: decimal mode */
  E.A = R;
  return H_OK;

I_SBC:
  M = ~e652_read(e652_effaddr01(opcode)); /* ~M = -M - 1 */
  goto _adc_bin;
}


static inline word reslv_imm (void)
{
  return E.PC++;
}


static inline word reslv_zpgn (byte n)
{
  return ZPAGE | ((e652_read(E.PC++) + n) & 0xff);
}


static inline word reslv_absn (byte n)
{
  byte lo, hi;
  lo = e652_read(E.PC++);
  hi = e652_read(E.PC++);
  return ((lo | hi << 8) + n) & MMAX;
}


static inline word reslv_xind (void)
{
  byte zp_addr, lo, hi;
  zp_addr = e652_read(E.PC++) + E.X;
  lo = e652_read(ZPAGE | ((zp_addr + 0) & 0xff));
  hi = e652_read(ZPAGE | ((zp_addr + 1) & 0xff));
  return (lo | hi << 8) & MMAX;
}


static inline word reslv_indy (void)
{
  byte zp_addr, lo, hi;
  zp_addr = e652_read(E.PC++);
  lo = e652_read(ZPAGE | ((zp_addr + 0) & 0xff));
  hi = e652_read(ZPAGE | ((zp_addr + 1) & 0xff));
  return ((lo | hi << 8) + E.Y) & MMAX;
}


static inline word reslv_rel (void)
{
  int8_t offset = e652_read(E.PC++);
  return E.PC + offset;
}


static inline word reslv_indir (void)
{
  byte rlo, rhi, lo, hi;
  rlo = e652_read(E.PC++);
  rhi = e652_read(E.PC++);
  lo = e652_read((rhi << 8) | ((rlo + 0) & 0xff));
  hi = e652_read((rhi << 8) | ((rlo + 1) & 0xff));
  return (lo | hi << 8) & MMAX;
}
