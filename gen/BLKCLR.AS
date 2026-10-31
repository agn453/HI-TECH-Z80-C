;	blkclr(ptr, size)
;	char *	ptr; unsigned short size;

;	Fills memory with size null bytes

	psect	text
	global	_blkclr

_blkclr:
	pop	de	;return address
	pop	hl	;pointer
	pop	bc	;count
	push	bc	;adjust stack
	push	hl
	push	de
	ld	e,0

1:
	ld	a,c	;check for finished
	or	b
	ret	z
	ld	(hl),e
	inc	hl
	dec	bc
	jr	1b
