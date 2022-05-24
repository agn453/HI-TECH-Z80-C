;	setjump, longjump - non local goto

	psect	text
	global	_longjmp, _setjmp

_setjmp:
	pop	bc		;return address
	ex	(sp),iy		;jmp_buf ptr to IY
	pop	de		;old IY to DE
	push	de		;keep stack balanced for return
	ld	hl,0
	add	hl,sp
	ld	(iy+0),l	;save SP in jmp_buf
	ld	(iy+1),h
	push	ix
	pop	hl
	ld	(iy+2),l	;save IX (frame ptr) in jmp_buf
	ld	(iy+3),h
	ld	(iy+4),c	;return address to jmp_buf
	ld	(iy+5),b
	ld	(iy+6),e	;save IY in jmp_buf
	ld	(iy+7),d
	ld	hl,0		;setjmp returns 0
	push	de		;restore IY
	pop	iy
	push	bc		;return address -> BC
	ret

_longjmp:
	pop	bc		; return address - junk now
	pop	iy		;jmp_buf ptr to IY
	pop	de		; return val in de as stack will change
	ld	l,(iy+0)	;restore SP from jmp_buf
	ld	h,(iy+1)
	ld	sp,hl
	ld	l,(iy+2)	;restore IX (frame ptr) from jmp_buf
	ld	h,(iy+3)
	push	hl
	pop	ix
	ld	l,(iy+4)	;return address
	ld	h,(iy+5)
	push	hl
	ld	c,(iy+6)	;restore IY from jmp_buf
	ld	b,(iy+7)
	push	bc
	pop	iy
	ex	de,hl		;get arg into hl
	ld	a,l
	or	h
	ret	nz		;not allowed to return 0
	inc	l
	ret
