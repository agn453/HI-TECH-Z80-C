;	Assign versions of divide

	global	asadiv, asldiv, adiv, ldiv

	psect	text

asadiv:
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	push	bc
	ex	(sp),hl
	call	adiv
	ex	(sp),hl
	pop	de
	ld	(hl),d
	dec	hl
	ld	(hl),e
	ex	de,hl		;return value in hl
	ret

asldiv:
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	push	bc
	ex	(sp),hl
	call	ldiv
	ex	(sp),hl
	pop	de
	ld	(hl),d
	dec	hl
	ld	(hl),e
	ex	de,hl		;return value in hl
	ret

