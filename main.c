#include <stdio.h>
#include <string.h>
#include "e652.h"

/*
 * Execution loop.
 */
int exec_loop (void);

/*
 * Just load the ROM anywhere in memory.
 */
int loadrom (byte *mem, int offset, char *path);

/*
 * Print the last value of registers.
 */
void dump_state (void);

/*
 * Memory.
 */
static byte emu_mem[MLEN];



int main (int argc, char **argv)
{
  if (argc < 2) {
    fprintf(stderr, "usage: %s ROM\n", argv[0]);
    return 1;
  }
  memset(emu_mem, 0, MLEN);
  if (loadrom(emu_mem, 0, argv[1]) != 0)
    return -1;
  E.m = emu_mem;
  e652_reset();
  E.PC = 0x400;
  exec_loop();
  return 0;
}

int exec_loop (void)
{
  int v;
  for (;;) {
    printf("%04X: %02x: A=%02X, X=%02X, Y=%02X, S=%02X, P=%02X\n",
        E.PC, E.m[E.PC], E.A, E.X, E.Y, E.S, E.P);
    v = e652_execnext();
    if (v == H_DBUG) {
      dump_state();
      return v;
    }
  }
}


int loadrom (byte *mem, int offset, char *path)
{
  FILE *fp = fopen(path, "rb");
  if (!fp) {
    perror("e652: failed to open ROM");
    return -1;
  }
  fread(mem + offset, 1, MLEN - offset, fp);
  fclose(fp);
  return 0;
}


void dump_state (void)
{
  printf("[state dump]\n");
  printf("  PC      0x%04X    %d\n", E.PC, E.PC);
  printf("  A       0x%02X      %d\n", E.A, E.A);
  printf("  X       0x%02X      %d\n", E.X, E.X);
  printf("  Y       0x%02X      %d\n", E.Y, E.Y);
  printf("  S       0x%02X      %d\n", E.S, E.S);
  printf("  P       0x%02X      %d\n", E.P, E.P);
  /* print flags */
  printf("  flags  ");
  if (E.P == 0) printf(" NOFLAGS");
  if (E.P & EN) printf(" NEG");
  if (E.P & EV) printf(" OVF");
  if (E.P & EB) printf(" BRK");
  if (E.P & ED) printf(" DEC");
  if (E.P & EI) printf(" ITD");
  if (E.P & EZ) printf(" ZRO");
  if (E.P & EC) printf(" CAR");
  fputc('\n', stdout);
}
