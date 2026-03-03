/*
 * E652 -- A 6502 emulator.
 *
 * Copyright (c) 2026 Vincent Yanzee J. Tan
 * This project is licensed under the MIT License.
 * See LICENSE for more info.
 */

#ifndef E652
#define E652 1

#include <stdint.h>


typedef uint8_t word;
typedef uint16_t dword;

/*
 * e652 emulation context.
 */
struct e652 {
  word *m;        /* 64KB */
  dword PC;       /* Program counter. */
  word A;         /* Accumulator register. */
  word X, Y;      /* Index (general) regs. */
  word P;         /* Status register. */
  word S;         /* Stack pointer. */
};

extern struct e652 E;


#define EN (1<<7)  /* Negative */
#define EV (1<<6)  /* Overflow */
#define EB (1<<4)  /* Break */
#define ED (1<<3)  /* Decimal mode */
#define EI (1<<2)  /* Interrupt disable */
#define EZ (1<<1)  /* Zero */
#define EC (1<<0)  /* Carry */

#define MLEN (1<<16)
#define MMAX (MLEN-1)

#define ZPAGE (0x0000) /* Zero page */
#define SPAGE (0x0100) /* Stack page */
#define VPAGE (0xff00) /* Vector page */

#define V_NMI (VPAGE + 0xfa)  /* Non maskable interrupt */
#define V_RES (VPAGE + 0xfc)  /* Reset vector */
#define V_IRQ (VPAGE + 0xfe)  /* Interrupt request */

/* addressing modes for cc == 01 */
#define A1_INDX (0) /* (indirect,X) */
#define A1_ZPG (1) /* zeropage */
#define A1_IMM (2) /* immediate */
#define A1_ABS (3) /* absolute */
#define A1_INDY (4) /* (indirect),Y */
#define A1_ZPGX (5) /* zeropage,X */
#define A1_ABSY (6) /* absolute,Y */
#define A1_ABSX (7) /* absolute,X */

/* addressing modes for cc == 02 */
#define A2_IMM (0) /* immediate */
#define A2_ZPG (1) /* zeropage */
#define A2_ACC (2) /* accumulator */
#define A2_ABS (3) /* absolute */
#define A2_ZPGN (5) /* zeropage,X or Y */
#define A2_ABSN (7) /* absolute,X or Y */

/* useful macros */
#define b1(addr) (E.m[(addr) & MMAX])
#define b2(addr) (b1((addr)) | b1((addr)+1) << 8)

#define Pset(f,c) (E.P = (E.P & ~(f)) | ((c) ? (f) : 0))

#define push(v) (e652_write(SPAGE + E.S--, (v)))
#define pop() (e652_read(SPAGE + ++E.S))

#define nextpc() (e652_read(E.PC++))


/*
 * Initialize a 6502 emulation context.
 */
void e652_reset (void);

/*
 * Get the effective memory address. Returns either a mem address between
 * 0 to 65536, EFF_ACCUM for the accumulator register, or EFF_INV. n would
 * be the value of the indexing register to use if cc == 10.
 */
int e652_effaddr (word opcode, word n);
#define EFF_INV   (-1)
#define EFF_ACCUM (-2)

/*
 * Read byte from memory.
 */
word e652_read (dword addr);

/*
 * Write byte onto memory.
 */
void e652_write (dword addr, word val);

/*
 * Execute instructions. Returns the halt reason.
 */
int e652_exec (void);
#define H_UNK     (0)   /* Unknown */
#define H_DBUG    (1)   /* Debug */

#endif
