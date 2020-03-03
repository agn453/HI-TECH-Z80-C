;	Self relocating startup for CP/M

	psect	cpm,global
	psect	text,global,pure
	psect	data,global
	psect	bss,global
	psect	stack,global

	defs	256		;a minimal stack


	psect	cpm
	defs	100h		;Base of CP/M's TPA
	global	start,_main,_exit,__Lbss,__Hstack
reloc:
	ld	hl,(6)		;get base of fdos
	ld	de,__Hstack
	or	a
	sbc	hl,de		;get distance to move
	ld	c,l		;save it in bc
	ld	b,h
1:
	ld	hl,(__Lbss)	;get count
	ld	a,l		;zero?
	or	h
	jr	z,2f
	dec	hl
	ld	(__Lbss),hl
	ld	hl,(addr)	;get address
	ld	e,(hl)		;pick up value
	inc	hl
	ld	d,(hl)
	inc	hl
	ld	(addr),hl	;save for next
	ex	de,hl
	ld	(where),hl	;save it
	ld	de,3f		;dont relocate any of our addresses
	or	a
	sbc	hl,de		;check for range
	jr	c,1b		;skip if less than 3f
	ld	hl,(where)
	ld	e,(hl)		;get value to relocate
	inc	hl
	ld	d,(hl)
	ex	de,hl
	add	hl,bc		;add difference
	ex	de,hl
	ld	(hl),d		;put relocated value back
	dec	hl
	ld	(hl),e
	jr	1b		;loop for more

addr:	defw	__Lbss+2	;start of addresses
where:	defs	2		;temp storage

2:
	ld	hl,__Lbss	;now find how much to move
	ld	de,start
	or	a
	sbc	hl,de
	ld	c,l		;save in bc
	ld	b,h
	ld	hl,__Hstack	;top of memory
	ld	de,__Lbss	;top of code and data
	sbc	hl,de		;room to leave at top of mem
	ex	de,hl
	ld	hl,(6)
	scf			;discount it
	sbc	hl,de
	ex	de,hl		;destination into de
	ld	hl,__Lbss-1	;source in hl - count is already in bc
	ld	a,c		;check for zero
	or	b
	jr	z,3f
	lddr			;move it
3:
	jp	start		;go to it

	psect	text
start:	ld	hl,(6)		;base address of fdos
	ld	sp,hl			;stack grows downwards
	ld	de,__Lbss		;end of initialized data
	scf
	sbc	hl,de			;size of uninitialized data area
	ld	c,l
	ld	b,h
	dec	bc	
	ld	l,e
	ld	h,d
	inc	de
	ld	(hl),0
	ldir			;clear memory
	ld	hl,80h		;arg buf
	ld	c,(hl)		;size of buffer
	ld	b,0
	ld	hl,0
	scf			;one for the road
	sbc	hl,bc		;negate it
	add	hl,sp
	ld	sp,hl			;allow space for args
	ld	de,0		;flag end of args
	push	de
	ld	hl,80h		;address of argument buffer
	add	hl,bc		;bc has size of arg buffer
	ld	b,c
	ex	de,hl
	ld	hl,(6)
	ld	c,1
	dec	hl
	ld	(hl),0
	inc	b
	jr	3f

2:	ld	a,(de)
	cp	' '
	dec	de
	jr	nz,1f
	xor	a
	push	hl
	inc	c
1:	dec	hl
	ld	(hl),a
3:
	djnz	2b
	ld	hl,nularg
	push	hl
	ld	hl,0
	add	hl,sp
	push	hl
	push	bc
	call	_main
	push	hl
	call	_exit
	jp	p,0

	psect	data
nularg:	defm	'main'
	defb	0
	end	reloc
