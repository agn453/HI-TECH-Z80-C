	psect	text
	global	_memmove, rcsv, cret

; void * memmove ( void * dest, void * src, size_t n ) 
; memcpy variant that handles overlapping blocks properly
; added to HiTech's libc.lib by Arnold M

_memmove:
	call	rcsv
; dest in HL, src in DE, n in BC
	ld	a,c
	or	b
	jr	z,1f
	sbc	hl,de
	add	hl,de
	jr	c,2f
; dest >= src, copy backward 
	dec	bc
	add	hl,bc
	ex	de,hl
	add	hl,bc
	inc	bc
	lddr
	inc	de
	ex	de,hl
1:
	jp	cret
2:	
; dest < src, copy forward
	push	hl
	ex	de,hl
	ldir
	pop	hl
1:
	jp	cret

