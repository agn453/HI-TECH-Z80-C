write(fd, buf, nbytes)
unsigned char	fd;
char *	buf;
unsigned short	nbytes;
{
#asm
global _errno, cret, bDos
 ld c,049h ; _WRITE
 ld b,(ix+6) ; fd
 ld e,(ix+8) ; buf
 ld d,(ix+9)
 ld l,(ix+10) ; nbytes
 ld h,(ix+11)
 call bDos
 jp z,cret
 ld (_errno),a
 xor a
 ld (_errno+1),a
#endasm
 /* implicit return hl=number of bytes written */
}

