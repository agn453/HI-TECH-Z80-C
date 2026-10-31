;	_swap(width, s1, s2)

;	exchange the two byte arrays

	psect	text
	global	__swap, rcsv, cret
__swap:
	call	rcsv
	push	bc
	ex	(sp),hl
	pop	bc
	jr	1f
2:
	ld	a,(hl)
	ex	af,af'
	ld	a,(de)
	ld	(hl),a
	ex	af,af'	
	ld	(de),a
	inc	hl
	inc	de
	dec	bc
1:
	ld	a,b
	or	c
	jr	nz,2b
	jp	cret
