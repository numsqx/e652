/*
 * E652 -- A 6502 emulator.
 *
 * Copyright (c) 2026 Vincent Yanzee J. Tan
 * This project is licensed under the MIT License.
 * See LICENSE for more info.
 */

#ifndef E652_EXTRAS
#define E652_EXTRAS 1

#include "e652.h"


#define nextpc() (e652_read(E.PC++)) /* Fetch byte then increment pc */

/* addressing modes for cc == 01 */
#define A1_INDX (0) /* (indirect,X) */
#define A1_ZPG (1) /* zeropage */
#define A1_IMM (2) /* immediate */
#define A1_ABS (3) /* absolute */
#define A1_INDY (4) /* (indirect),Y */
#define A1_ZPGX (5) /* zeropage,X */
#define A1_ABSY (6) /* absolute,Y */
#define A1_ABSX (7) /* absolute,X */

/*
 * Resolve the effective address of an immediate.
 */
static inline dword reslv_imm (void)
{
  return E.PC++;
}

/*
 * Resolve the effective address of an index in zeropage.
 */
static inline dword reslv_zpgn (word n)
{
  return ZPAGE + ((e652_read(E.PC++) + n) & 0xff);
}

/*
 * Resolve the effective address of an absolute addr + index.
 */
static inline dword reslv_absn (word n)
{
  word lo, hi;
  lo = e652_read(E.PC++);
  hi = e652_read(E.PC++);
  return ((lo | hi << 8) + n) & MMAX;
}

/*
 * Resolve the effective address of an indirect X-indexed address.
 */
static inline dword reslv_indx (void)
{
  word zp_addr, lo, hi;
  zp_addr = (e652_read(E.PC++) + E.X) & 0xff;
  lo = e652_read(ZPAGE + zp_addr);
  hi = e652_read(ZPAGE + ((zp_addr + 1) & 0xff));
  return (lo | hi << 8) & MMAX;
}

/*
 * Resolve the effective address of an indirect Y-indexed address.
 */
static inline dword reslv_indy (void)
{
  word zp_addr, lo, hi;
  zp_addr = e652_read(E.PC++) & 0xff;
  lo = e652_read(ZPAGE + zp_addr);
  hi = e652_read(ZPAGE + ((zp_addr + 1) & 0xff));
  return ((lo | hi << 8) + E.Y) & MMAX;
}

#endif
