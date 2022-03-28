	psect	text,global,pure
	psect	data,global
	psect	bss,global

	psect	text
	defs	100h		;Base of CP/M's TPA
	global	start,_main,_exit,__Hdata, __trap, __piped, __exact
start:	ld	hl,(6)		;base address of fdos
	ld	sp,hl		;stack grows downwards
	ld	de,__Hdata	;end of initialized data
	scf
	sbc	hl,de		;size of uninitialized data area
	ld	c,l
	ld	b,h
	dec	bc	
	ld	l,e
	ld	h,d
	inc	de
	ld	(hl),0
	ldir			;clear memory
	ld	hl,-80h		;allow room for args
	add	hl,sp
	ld	sp,hl		;allow space for args
	ld	bc,0		;flag end of args
	push	bc
	ld	hl,80h		;address of argument buffer
	ld	c,(hl)
	ld	b,0
	add	hl,bc
	ld	b,c
	ex	de,hl
	ld	hl,(6)
	ld	c,1
	dec	hl
	ld	(hl),0
	inc	b
	jr	3f

2:	ld	a,(de)
	cp	' '
	dec	de
	jr	nz,1f
	xor	a
	push	hl
	inc	c
1:	dec	hl
	ld	(hl),a
3:
	djnz	2b
	ld	hl,nularg
	push	hl
	ld	hl,0
	add	hl,sp
	push	hl
	push	bc
	ld	hl,__trap
	ld	(39h),hl
	ld	a,0c3h
	ld	(38h),a
	rst	38h
	call	_main
	push	hl
	call	_exit
	jp	p,0

	psect	data
__piped:
        defw    0
__exact:		; Exact file size mode
	defb	'C',0	; 'C' is CP/M 2 (none), 'I' is ISIS, 'D' is DOSPLUS
nularg:	defm	'main'
	defb	0
	end	start
