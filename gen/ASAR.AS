;	Shift operations - the count is always in B,
;	the quantity to be shifted is in HL, except for the assignment
;	type operations, when it is in the memory location pointed to by
;	HL

	global	asar	;assign shift arithmetic right
	psect	text
	global	shar


asar:
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	push	hl		;save for the store
	ex	de,hl
	call	shar
	ex	de,hl
	pop	hl
	ld	(hl),d
	dec	hl
	ld	(hl),e
	ex	de,hl		;return value in hl
	ret
