	global	__getargs, __argc_
	psect	text

__getargs:
	pop	hl		;return address
	exx
	pop	hl
	pop	hl		;unjunk stack
	ld	a,(80h)		;get buffer length
	inc	a		;allow for null byte
	neg
	ld	l,a
	ld	h,-1
	add	hl,sp
	ld	sp,hl			;allow space for args
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
	push	hl
	inc	c
4:	ld	a,(de)		;remove extra spaces
	cp	' '
	jr	nz,5f
	dec	de
	jr	4b
5:
	xor	a
1:	dec	hl
	ld	(hl),a
3:
	djnz	2b
	ld	(__argc_),bc		;store argcount
	ld	hl,nularg
	push	hl
	ld	hl,0
	add	hl,sp
	exx
	push	de		;junk the stack again
	push	de
	push	hl		;return address
	exx
	ret

	psect	data
nularg:	defb	0
