; void * memchr (void * s, int c, size_t n)
; added to HiTech's libc.lib by Arnold M

global _memchr
_memchr:
pop af
pop hl ; s
pop de ; c
pop bc ; n
push bc
push de
push hl
push af

ld a,c
or b
jr z,none
ld a,e
cpir
dec hl
ret z
none:
ld hl,0
ret
