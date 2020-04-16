#define BUFLEN 67
extern int _flip(char*, char*, int); 

int rename(char *n1, char *n2)
{
  char namebuf[BUFLEN]; /* needed for slash-flipping */

  if (_flip(namebuf, n1, BUFLEN))
  {
#asm
global _errno, cret, bDos
  push ix
  pop hl
  ld de,-BUFLEN
  add hl,de
  ex de,hl
  ld l,(ix+8) ; n2
  ld h,(ix+9)
  ld c,78 ; _RENAME
  call bDos
  ld hl,0
  jp z,cret ; return 0
  ld (_errno),a
  ld a,l
  ld (_errno+1),a
1:
#endasm
  }
  return -1;
}
