; opendir.as 
; #include <dirent.h>
; #include <stdlib.h>

; DIR * opendir(char * name)
global _opendir
global ncsv, cret, __flip, _malloc, _free, bDos, _errno

psect text
_opendir:
call ncsv
defw -69
ld hl, 0
add hl,sp
ld (ix-2),l
ld (ix-1),h ; address to store slash-flipped name
ld bc,67
push bc
ld e,(ix+6)
ld d,(ix+7) ; name
push de
push hl
call __flip
pop hl ; address of slash-flipped name
pop bc
pop bc

; "." -> getcwd (default drive)
; "x:." -> getcwd (drive x)
ld b,0
inc hl
ld a,(hl)
cp ':'
jr nz,1f
dec hl
ld a,(hl)
and 0ffh - ('a'-'A')
sub 'A'-1
ld b,a
inc hl
inc hl
inc hl
ld a,(hl)

1:
or a
jr nz,2f
dec hl
ld a,(hl)
cp '.'
jr nz,2f

ld (hl),'\'
inc hl
ex de,hl
ld c,059h ; _GETCD
call bDos

2:

ld hl, 128 ; sizeof( DIR )
push hl
call _malloc
pop bc
push hl
ld a,l
or h
jr z,noram

ld e,(ix-2)
ld d,(ix-1) ; name, flipped 
push de
; we need to treat a root directory in a special way:
call isroot
pop de
ex (sp),ix
ld (ix+14),10h
ld bc, 1640h ; 40h : _FFIRST 16h: also hidden, system and directory
call nz,bDos ; call the bDos only for non-root directories

ld (ix+64),0 ; no entry read so far
ld b,(ix+14) ; attribute
ex (sp),ix
jr nz,error
ld a,b
and 10h
jr z, notdir

pop hl
jp cret

noram:
ld bc, 0DEh ; .NORAM
ld (_errno),bc
jp cret ; return NULL

notdir:
ld a, 0CFh ; .IATTR
error:
ld (_errno),a
xor a
ld (_errno+1),a
call _free ; address of allocated block is already on the stack
ld hl, 0
jp cret ; return NULL

isroot:
; in  - DE: pointer to (flipped) pathname
;     - HL: pointer to res->dir_dir	
; out: * Z: pathname points to a root directory 
;      *NZ: pathname does not point to a root directory
; if pathname points to a root then drive and path are copied to res->dir_dir
; if the path starts with [d:].\ and the cwd is the root dir,
; then the '.' is removed from the string

ld b,0
inc de
ld a,(de)
cp ':'
jr nz, 1f
dec de
ld a,(de)
and 0ffh-('a'-'A')
ld b,a
inc de
inc de
inc de
ld a,(de)
1:
cp '\'
jr z,4f
or a
ret nz
dec de
ld a,(de)
cp '\'
ret nz
ld a,b
or a
jr nz, 2f
push hl
ld c,19h ; _CURDRV
call bDos
add a,'A'
pop hl
2:
ld (hl),a
inc hl
ld (hl),':'
inc hl
ld (hl),'\'
inc hl
xor a
ld (hl),a
ret

4:
; The first component is just one char, check for ".\" with
; the root dir as cwd
dec de
ld a,(de)
cp '.'
ret nz
ld a,b
or a
jr z,5f
sub 'A'-1
ld b,a
5:
push de
push hl
ex de,hl
ld c,59h ; _GETCD
call bDos
pop hl
pop de
ld a,(hl)
or a
ret nz
; DE points to a '.' that we do not want
ld l,e
ld h,d
6:
inc hl
ld a,(hl)
ld (de),a
inc de
cp 1
ret c ; look for '\0', but do not set Z-flag
jr 6b
