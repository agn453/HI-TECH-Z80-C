/* Duplicate file descriptor */

dup(fd)
int	fd;
{
#asm
global _errno, bDos
  ld c,047h ; _DUP
  ld b,(ix+6) ; fd
  call bDos
  jr z,1f
  ld (_errno), a
  xor a
  ld (_errno+1),a
1:
  ld l,b
  rl b
  sbc a,a
  ld h,a
#endasm
/* implicit return hl; */
}

