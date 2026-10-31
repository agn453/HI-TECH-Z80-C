;	ASsign Logical Long Right  SHift

	psect	text
	global	asllrsh, llrsh, iregstore

asllrsh:
	push	bc		;save the count
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ex	(sp),hl		;save pointer, restore count
	push	bc		;hi word
	ex	(sp),hl
	pop	bc
	call	llrsh		;do the shift
	jp	iregstore	;go store the value and return
