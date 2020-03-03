;	setjump, longjump - non local goto

	psect	text
	global	_longjmp, _setjmp

_setjmp:
	pop	bc		;return address
	ex	(sp),iy
	pop	de
	push	iy
	ld	hl,0
	add	hl,sp
	ld	(iy+0),l
	ld	(iy+1),h
	push	ix
	pop	hl
	ld	(iy+2),l
	ld	(iy+3),h
	ld	(iy+4),c
	ld	(iy+5),b
	ld	(iy+6),e
	ld	(iy+7),d
	push	bc
	ld	hl,0		;setjmp returns 0
	push	de		;restore iy
	pop	iy
	ret

_longjmp:
	pop	bc		;return address - junk now
	pop	iy		;argument
	pop	de		;and return value
	ld	l,(iy+0)
	ld	h,(iy+1)
	ld	sp,hl			;stack pointer
	ld	l,(iy+2)
	ld	h,(iy+3)
	push	hl
	pop	ix
	ld	l,(iy+4)
	ld	h,(iy+5)
	ld	c,(iy+6)
	ld	b,(iy+7)
	push	bc
	pop	iy
	push	hl
	ex	de,hl		;get arg into hl
	ret
