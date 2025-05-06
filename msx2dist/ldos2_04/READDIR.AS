; readdir.as

; struct dirent * readdir (DIR * dir)

psect text
global _readdir
global csv, cret, _errno, bDos
_readdir:
call csv
ld e,(ix+6)
ld d,(ix+7)
ld hl,64
add hl,de
ld bc, 1640h ; 40h : _FFIRST 16h: also hidden, system and directory
ld a,(hl)
or a
jr z, first
inc c ; 41h : _FNEXT

first:
push hl
ex (sp),ix
ld hl,nulstr
 psect data
nulstr:
 defb 0
 psect text
call bDos
ex (sp),ix
pop hl
jp z,cret

ld l,a
ld h,0
ld (_errno),hl
ld l,h
jp cret

