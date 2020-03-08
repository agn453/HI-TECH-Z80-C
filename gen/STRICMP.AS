; int strcasecmp(const char *s1, const char *s2)
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

global _strcasecmp
_strcasecmp:

pop bc
pop hl ; s1
pop de ; s2
push de
push hl
push bc

1:
ld a,(de) ;*s2
tolower
ld c,a
ld a,(hl) ;*s1 
tolower
cp c
jr nz,2f
inc hl
inc de
or a
jp nz,1b
ld h,a
ld l,a
ret

2:
sbc hl,hl
ret c
inc hl
ret
