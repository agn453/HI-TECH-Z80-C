isatty(f)
unsigned char	f;
{
#asm
global bDos
  ld c,04Bh ; _IOCTL
  ld b,(ix+6) ; f
  xor a ; 0 => get file handle status
  call bDos
  ld a,080h
  and e
  rlca
  ld l,a
  xor a
  ld h,a
#endasm
  /* implicit return hl; */
}
