#include	"cpm.h"

close(fd)
int	fd;
{
#asm
global _errno, cret, bDos
  ld c,045h
  ld b,(ix+6) ; fd
  call bDos
  ld hl,0
  jp z, cret
  dec hl
  ld (_errno),a
  xor a
  ld (_errno+1),a
#endasm
  /* implicit return hl; */
}

