/* changes position in file */

long lseek(int fd, long offs, int wh)
{
#asm
  global bDos
  ld c,04Ah ; _SEEK
  ld b,(ix+6) ; fd
  ld l,(ix+8) ; offs
  ld h,(ix+9)
  ld e,(ix+10)
  ld d,(ix+11)
  ld a,(ix+12) ; wh
  call bDos ; new pointer in de:hl
  ex de,hl ; Hitech's calling convention requires long in hl:de
#endasm
  /* implicit return hl:de; */
} 
