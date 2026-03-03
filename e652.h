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

#define Pset(f,c) (E.P = (E.P & ~(f)) | ((c) ? (f) : 0)) /* Assigns a flag */

#define MLEN (1<<16) /* Length of memory */
#define MMAX (MLEN-1) /* Max possible address (can be used as a wrap mask) */

#define ZPAGE (0x0000) /* Zero page */
#define SPAGE (0x0100) /* Stack page */
#define VPAGE (0xff00) /* Vector page */

#define V_NMI (VPAGE + 0xfa)  /* Non maskable interrupt */
#define V_RES (VPAGE + 0xfc)  /* Reset vector */
#define V_IRQ (VPAGE + 0xfe)  /* Interrupt request */

#define b1(addr) (E.m[(addr) & MMAX]) /* 1B write + wrap-around */
#define b2(addr) (b1((addr)) | b1((addr)+1) << 8) /* 2B write + wrap-around */

#define push(v) (e652_write(SPAGE + E.S--, (v))) /* Stack push */
#define pop() (e652_read(SPAGE + ++E.S)) /* Stack pop */


/*
 * Initialize a 6502 emulation context.
 */
void e652_reset (void);

/*
 * Get the effective memory address. Returns a mem address between 0 to 65536.
 * Returns -1 on error. Works only for opcodes with cc == 01.
 */
int e652_effaddr01 (word opcode);

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
