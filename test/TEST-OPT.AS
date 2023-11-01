; Test input for OPTIMH (Z280 code optimiser)
;
global _instr
n	equ	25
nn	equ	1abch
psect text
_instr:
;
	global csv,cret
	call csv
	jp	cret
;
	global amul,lmul
	call	amul
	call	lmul
;
label:	ld	(hl),c
	inc	hl
	ld	(hl),b
	inc	hl
;
	ld	(hl),c
	inc	hl
	ld	(hl),b
;
	ld	(hl),e
	inc	hl
	ld	(hl),d
;
	ld	c,(hl)
	inc	hl
	ld	b,(hl)
;
	ld	e,(hl)
	inc	hl
	ld	d,(hl)
;
	ld	c,(ix+n)
	ld	b,(ix+n+1)
	ld	c,(iy+n)
	ld	b,(iy+n+1)
;
	ld	e,(ix+n)
	ld	d,(ix+n+1)
;
	ld	l,(ix+n)
	ld	h,(ix+n+1)
;
	ld	(ix+n),c
	ld	(ix+n+1),b
;
	ld	(ix+n),e
	ld	(ix+n+1),d
;
	ld	(ix+n),l
	ld	(ix+n+1),h
;
	or	a
	sbc	hl,bc
;
	or	a
	sbc	hl,de
;
	push	ix
	pop	de
	ld	hl,nn
	add	hl,de
;
	push	ix
	pop	de
;
	push	ix
	pop	hl
;
; Addressing with large offset
;
	push	ix
	pop	de
	ld	hl,nn
	add	hl,de
;
	push	iy
	pop	de
	ld	hl,nn
	add	hl,de
;
