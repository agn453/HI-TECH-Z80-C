#include	<stdio.h>

extern	int	errno;
extern	char *	sys_err[];
extern	int	sys_ner;

static
ps(s)
char *	s;
{
  while(*s)
    putc(*s++, stderr);
}

void perror(char * s)
{
  char err_text[64];
  int my_errno;
  
  my_errno=errno;
  ps(s);
  putc(':', stderr);
  
#asm
global bDos
  push ix
  pop hl
  ld de,-64
  add hl,de
  ex de,hl
  ld c,066h ; _EXPLAIN
  ld b,(ix+-66) ; my_errno
  call bDos
#endasm
  ps(err_text);
}
