	psect	text
	global	alsub, llsub

alsub:
llsub:
	exx
	pop	hl
	exx
	pop	bc
	ex	de,hl
	or	a
	sbc	hl,bc
	ex	de,hl
	pop	bc
	sbc	hl,bc
	exx
	push	hl
	exx
	ret
