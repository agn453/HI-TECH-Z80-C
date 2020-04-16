	psect	text
	global	_memcpy, rcsv, cret
; int memcpy(void * s1, void *s1, size_t n)
; fast version using ldir by Arnold M

_memcpy:
	call	rcsv

	ld	a,c
	or	b
	jr	z,1f
	push	hl
	ex	de,hl
	ldir
	pop	hl
1:
	jp	cret
