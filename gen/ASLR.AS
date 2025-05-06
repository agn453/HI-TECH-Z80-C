;	Shift operations - the count is always in B,
;	the quantity to be shifted is in HL, except for the assignment
;	type operations, when it is in the memory location pointed to by
;	HL

	global	aslr	;assign shift logical right
	psect	text
	global	shlr


aslr:
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
	push	hl		;save for the store
	ex	de,hl
	call	shlr
	ex	de,hl
	pop	hl
	ld	(hl),d
	dec	hl
	ld	(hl),e
	ex	de,hl		;return value in hl
	ret
