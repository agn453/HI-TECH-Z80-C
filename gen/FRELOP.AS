;	floating relational operation - returns flags as though
;	a floating subtract was done.

	psect	text
	global	frelop

frelop:
	exx			;select	alternate reg set
	pop	hl		;return	address
	exx			;get other set again
	pop	bc		;low word of 2nd arg
	ex	de,hl		;put hi	word of	1st in de
	ex	(sp),hl		;get hi	word of	2nd in hl
	ex	de,hl		;hi word of 1st	back in	hl
	ld	a,h		;test for differing signs
	xor	d
	jp	p,2f		;the same, so ok
	ld	a,h		;get the sign of the LHS
	or	1		;ensure zero flag is reset, set sign flag
	pop	bc		;unjunk stack
	jr	1f		;return	with sign of LHS
2:
	ld	a,d		;preserve sign flag
	res	7,d		;clear sign flag
	res	7,h		;and the other
	and	80h		;mask out sign flag
	sbc	hl,de		;set the flags
	pop	hl		;low word of 1st into hl again
	jr	nz,togs		;go fixup sign flag if different
	sbc	hl,bc		;now set flags on basis	of low word
	jr	z,1f		;if zero, all ok
togs:
	rr	h		;rotate	carry into sign
	xor	h		;toggle sign if necessary
	or	1		;reset zero flag
1:
	exx			;get return address
	jp	(hl)		;and return with stack clean
