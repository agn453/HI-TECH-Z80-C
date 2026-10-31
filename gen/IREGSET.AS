;	routines to support the assignment versions of the long operations

	psect	text
	global	iregset, iregstore

iregset:
	ld	e,(hl)	;pick up arg
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	ex	(sp),hl	;save pointer, get return address
	push	bc	;now put the hi word into hl
	ex	(sp),hl	;saving return address on stack again
	pop	bc	;now return address in bc
	exx
	pop	hl	;the pointer
	pop	bc	;the final return address
	pop	de	;now the low word of arg2
	ex	(sp),hl	;get the hi word, save pointer
	push	bc	;this is the return address again
	ex	(sp),hl	;into hl again
	pop	bc	;hi word now in bc
	ex	(sp),hl	;AHA! got return address on stack, ptr in hl
	push	hl	;now its easy, save pointer
	push	bc	;hi word
	push	de	;low word
	exx
	push	bc	;immediate return address
	ret		;finito

iregstore:
	ex	(sp),hl	;get pointer into hl
	pop	bc	;hi word in bc
	ld	(hl),b
	dec	hl
	ld	(hl),c
	dec	hl
	ld	(hl),d
	dec	hl
	ld	(hl),e		;all done
	push	bc	;restore to hl
	pop	hl
	ret		;and return with value in hl
