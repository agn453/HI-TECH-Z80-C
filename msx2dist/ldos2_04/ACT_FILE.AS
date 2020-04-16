global	__act_file
global	ncsv, cret, indir, bDos
global	__flip
global	amod
global	_errno, cret
psect	text
; int _act_file(char * name, short mode, short bc)
__act_file:
call ncsv
defw -67
ld	hl,67
push	hl
ld	l,(ix+6)
ld	h,(ix+7)
push	hl
push	ix
pop	de
ld	hl,-67
add	hl,de
push	hl
call	__flip
pop	bc
pop	bc
pop	bc
ld	a,l
or	h
jp	nz,l2
L2:
ld	hl,-1
jp	cret
l2:
; #asm
global _errno, cret
push	ix
pop	hl
ld	de,-67
add	hl,de
ex	de,hl 
; ld	a, (ix+8)
ld	c, (ix+8)
ld	b,0 ; 8 bits is enough
ld	hl, modtab
add	hl,bc
ld	a,(hl)
psect	data
modtab:
defb	1,2,0,3,5,6,4,7, 10h ; translate mode to dos2 conventions
psect	text
ld	c, (ix+10) ; bdos function code
ld	b, (ix+11) ; attributes (for create) 
ld	l,b ; attributes (for chmod)
call	bDos
jr	z,1f
ld	(_errno),a
xor	a
ld	(_errno+1),a
ld	hl,-1
jp	cret ; return -1
1:
ld	hl,0
ld	a,(ix+10) ; function code
cp	45h
jp	nc,cret ; return 0 (success) if called by chmod, unlink or rmdir  
ld	l,b ; file handle/descriptor, called by open, creat or mkdir
; #endasm
jp	cret
