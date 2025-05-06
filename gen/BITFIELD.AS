;	Functions to implement bitfields:

;	the width and offset (in bits) of the bitfield concerned
;	appear in the code immediately after the call, packed into one byte
;	like this:

;		bit	|7    4|3   0|
;			|offset|width|

;	bfext: called with a value in HL, returns with extracted
;	bitfield value in HL

	psect	text
	global	bfext, bfins
bfext:
	pop	bc		;get return address
	ld	a,(bc)		;pick up width and offset
	inc	bc
	push	bc
	push	af
	rra
	rra
	rra
	rra
	and	0Fh		;get offset
	jr	z,1f		;skip if zero
	ld	b,a
2:
	srl	h
	rr	l
	djnz	2b
1:
	pop	af
	push	de		;don't touch de
	ld	de,1		;start with a 1
	and	0Fh		;get width
	ld	b,a		;put in counter
2:
	sll	e
	rl	d		;shift up one bit
	djnz	2b
	dec	de		;decrement by one
	ld	a,l
	and	e		;mask appropriately
	ld	l,a
	ld	a,h
	and	d
	ld	h,a		;all done!
	pop	de
	ret

;	Insert the data in de into the data at (hl). Size and offset
;	are in the following byte as usual

bfins:
	pop	bc		;get return address
	ld	a,(bc)		;get info byte
	inc	bc
	push	bc
	push	hl		;save address
	push	af		;save for ron
	ld	hl,1
	and	0Fh
	ld	b,a
2:
	sll	l
	rl	h		;shift up one bit
	djnz	2b
	dec	hl		;convert to a mask
	ld	a,e
	and	l
	ld	e,a
	ld	a,d
	and	h
	ld	d,a		;masked it
	pop	af		;get byte again
	push	de		;save value
	rra
	rra
	rra
	rra
	and	0Fh		;mask out offset
	jr	z,1f		;skip if zero
	ld	b,a
2:
	sll	l		;shift it up
	rl	h
	sll	e
	rl	d
	djnz	2b		;loop for more
1:
	ex	(sp),hl		;save mask, get value
	pop	bc		;mask in bc
	ex	(sp),hl		;address in hl, save value
	ld	a,c		;complement mask
	cpl
	and	(hl)		;and with memory data
	or	e		;or with shifted value
	ld	(hl),a		;put back
	inc	hl
	ld	a,b
	cpl
	and	(hl)
	or	d
	ld	(hl),a
	pop	hl		;get value back
	ret			;finito!
