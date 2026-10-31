	global	rcsv

ARG	equ	6		;offset of 1st arg
	psect	text
rcsv:
	ex	(sp),iy		;save iy, get return address
	push	ix
	ld	ix,0
	add	ix,sp		;new frame pointer
	ld	l,(ix+ARG+0)
	ld	h,(ix+ARG+1)
	ld	e,(ix+ARG+2)
	ld	d,(ix+ARG+3)
	ld	c,(ix+ARG+4)
	ld	b,(ix+ARG+5)
	jp	(iy)
