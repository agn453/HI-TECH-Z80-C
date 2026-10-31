;	Long increment

	psect	text

	global	lainc, llinc, ladec, lldec

gval:
	exx
	pop	hl		;return address
	exx
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	push	de		;lo word
	push	bc		;hi word
	push	bc
	ex	(sp),hl
	exx
	push	hl		;return address
	exx
	ret

lainc:
llinc:
	call	gval
	ld	bc,1
	ex	de,hl
	add	hl,bc
	ex	de,hl
	ld	c,0
	adc	hl,bc

sval:
	ex	(sp),hl
	pop	bc
	ld	(hl),b
	dec	hl
	ld	(hl),c
	dec	hl
	ld	(hl),d
	dec	hl
	ld	(hl),e
	pop	hl		;restore original value
	pop	de
	ret

lldec:
ladec:
	call	gval
	ld	bc,-1
	ex	de,hl
	add	hl,bc
	ex	de,hl
	adc	hl,bc
	jr	sval
