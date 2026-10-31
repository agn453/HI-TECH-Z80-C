	psect	text
	global	_sbrk,__Hbss, _brk, _checksp

;	NB This brk() does not check that the argument is reasonable.

_brk:
	pop	hl	;return address
	pop	de	;argument
	ld	(memtop),de	;store it
	push	de		;adjust stack
	jp	(hl)	;return
_sbrk:
	pop	bc
	pop	de
	push	de
	push	bc
	ld	hl,(memtop)
	ld	a,l
	or	h
	jr	nz,1f
	ld	hl,__Hbss
	ld	(memtop),hl
1:
	add	hl,de
	jr	c,2f		;if overflow, no room
	ld	bc,1024		;allow 1k bytes stack overhead
	add	hl,bc
	jr	c,2f		;if overflow, no room
	sbc	hl,sp
	jr	c,1f
2:
	ld	hl,-1		;no room at the inn
	ret

1:	ld	hl,(memtop)
	push	hl
	add	hl,de
	ld	(memtop),hl
	pop	hl
	ret

_checksp:
	ld	hl,(memtop)
	ld	bc,128
	add	hl,bc
	sbc	hl,sp
	ld	hl,1		;true if ok
	ret	c		;if carry, sp > memtop+128
	dec	hl		;make into 0
	ret

	psect	bss
memtop:	defs	2
