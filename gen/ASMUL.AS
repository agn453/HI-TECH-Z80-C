;	Assign versions of multiply

	global	asamul, aslmul, amul

	psect	text

asamul:
aslmul:
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
	push	bc
	ex	(sp),hl
	call	amul
	ex	(sp),hl
	pop	de
	ld	(hl),d
	dec	hl
	ld	(hl),e
	ex	de,hl		;return value in hl
	ret

