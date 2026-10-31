; file: getcwd.as
; author: Arnold Metselaar
;
; implements 
; char * getcwd(int drive)
; drive: 
;	0 - current
;	1 - A:
;	2 - B: etc.
;
global _getcwd, bDos

psect text
_getcwd:
pop hl
pop de ; get arg
push de
push hl ; restore stack
ld a,e
or a
jr nz, 1f
ld c,19h ; get default drive
call bDos
inc a
1:
ld b,a
add a,'A'-1
ld hl,cwdbuf
ld (hl),a
inc hl
ld (hl),':'
inc hl
ld (hl),'\'
inc hl
ex de,hl
ld c,59h ; _GETCD, get current directory
call bDos
ld hl,cwdbuf
ret

psect bss
cwdbuf: defs 67
