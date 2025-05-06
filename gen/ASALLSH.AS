;	ASsign Arithmetic Long Left  SHift
;	ASsign Logical Long Left SHift

	psect	text
	global	asallsh, aslllsh, allsh, iregstore

asallsh:
aslllsh:
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
	call	allsh		;do the shift
	jp	iregstore	;go store the value and return
