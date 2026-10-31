	psect	text
	global	aladd, lladd

aladd:
lladd:
	exx
	pop	hl
	exx
	pop	bc
	ex	de,hl
	add	hl,bc
	ex	de,hl
	pop	bc
	adc	hl,bc
	exx
	push	hl
	exx
	ret
