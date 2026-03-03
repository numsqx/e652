#include <stdio.h>
#include "e652.h"

/*
 * Just load the ROM anywhere in memory.
 */
int loadrom (word *mem, int offset, char *path);

/* Memory. */
static word emu_mem[MLEN];


int main (int argc, char **argv)
{
  if (argc < 2) {
    fprintf(stderr, "usage: %s ROM\n", argv[0]);
    return 1;
  }
  if (loadrom(emu_mem, 0x8000, argv[1]) != 0)
    return -1;
  E.m = emu_mem;
  e652_reset();
  e652_exec();
}


int loadrom (word *mem, int offset, char *path)
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
