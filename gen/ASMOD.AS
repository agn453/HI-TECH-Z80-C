;	Assign versions of modulus

	global	asamod, aslmod, amod, lmod

	psect	text

asamod:
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	push	bc
	ex	(sp),hl
	call	amod
	ex	(sp),hl
	pop	de
	ld	(hl),d
	dec	hl
	ld	(hl),e
	ex	de,hl		;return value in hl
	ret

aslmod:
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	push	bc
	ex	(sp),hl
	call	lmod
	ex	(sp),hl
	pop	de
	ld	(hl),d
	dec	hl
	ld	(hl),e
	ex	de,hl		;return value in hl
	ret

