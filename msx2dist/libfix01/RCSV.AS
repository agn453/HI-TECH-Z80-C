; rcsv, save old iy,ix and get three arguments in hl,de,bc
; return with new framepointer in ix.
; save 2 bytes and 34 T-states compared to HiTech's version - Arnold M.

	global	rcsv
	
ARG	equ	6		;offset of 1st arg
	macro	ldrd reg
	ld	reg,(hl)
	dec	hl
	endm
	psect	text
rcsv:
	ex	(sp),iy		;save iy, get return address
	push	ix
	ld	ix,0
	add	ix,sp		;new frame pointer
	ld	hl,ARG+5
	add	hl,sp		;top of args 
	ldrd	b
	ldrd	c
	ldrd	d
	ldrd	e
	ld	h,(hl)
	ld	l,(ix+ARG+0)
	jp	(iy)
