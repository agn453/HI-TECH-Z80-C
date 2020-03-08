; int strncasecmp(const char *s1, const char *s2, size_t n)
; return a value less than 0, 0 or greater than 0 if s1 is found, respectively, 
;   to be less than equal or greater than s2 ignoring case
; by Arnold M

psect text

macro tolower
  cp 'A'
  jr c, 10f
  cp 'Z'+1
  jr nc, 10f
  add  a, 'a'-'A'
10: 
endm

global _strncasecmp, rcsv, cret

_strncasecmp:
 call rcsv ; hl=s1, de=s2, bc=n
 ld b,c ; keep low part of n in reg b,  reg c is used as scratch
 ld a,b
 or a
 jr z,bzero
loop:
 ld a,(de) ;*s2
 tolower
 ld c,a
 ld a,(hl) ;*s1 
 tolower
 cp c
 jr nz,diff
 or a
 jr z, equal
 inc hl
 inc de
 djnz loop
bzero:
 dec (ix+6+5) ; n / 0x100
 jp p,loop ; dec m does not affect the carry flag
 ld a,(ix+6+5)
 inc a
 jr nz,loop

equal:
 ld h,a
 ld l,a
 jp cret

diff:
 sbc hl,hl
 jp c,cret
 inc hl
 jp cret
