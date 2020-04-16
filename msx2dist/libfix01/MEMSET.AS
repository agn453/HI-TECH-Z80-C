; memset.as

global _memset, csv, cret

; void * memset (void *s, int c, size_t n)
; fast version with standard argument order by Arnold M

psect	text 
_memset:
	call	csv
	ld	l,(ix+6)
	ld	h,(ix+7)
	ld	c,(ix+10)
	ld	b,(ix+11)
	ld	a,c
	or	b
	jr	z,1f
	ld	e,(ix+8)
	ld	(hl),e
	dec	bc
	ld	a,c
	or	b
	jr	z,1f
	push	hl
	ld	e,l
	ld	d,h
	inc	de
	ldir
	pop	hl
1:
	jp	cret
	
