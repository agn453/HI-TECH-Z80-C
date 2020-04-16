	psect	text
	global	_memcmp, rcsv, cret
; int memcmp(void * s1, void *s1, size_t n)
; fast version using cpi by Arnold M

_memcmp:
	call	rcsv
	ld	a,c
	or	b
	jr	z,3f
1:
	ld	a,(de)
	cpi	; cp (hl), inc hl, dec bc
	jr	nz,2f
	inc	de
	jp	pe,1b
3:
	ld	hl,0
	jp	cret
2:
	dec	hl	; undo cpi
	cp	(hl)	; get carry
	ld	hl,1
	jp	c,cret
	dec	hl
	dec	hl
	jp	cret

