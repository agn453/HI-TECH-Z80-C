; strrchr(char *s, int c)
; version that can find the closing \0 by Arnold M

	psect	text
	global	rcsv, cret, _strrchr	; also equivalent to _rindex

_strrchr:
	pop	bc
	pop	hl
	pop	de
	push	de
	push	hl
	push	bc

	ld	bc,1 ; the closing nul is considered part of the string here
	jr	5f
6:
	inc	hl
	inc	bc
5:
	ld	a,(hl)
	or	a
	jr	nz,6b
1:
	ld 	a,e
	cpdr
	inc	hl
	ret	z
	ld	l,c
	ld	h,b
	ret
