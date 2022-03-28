;	Self relocating startup for CP/M

	psect	cpm,global
	psect	text,global,pure
	psect	data,global
	psect	bss,global
	psect	stack,global

	defs	256		;a minimal stack


	psect	cpm
	defs	100h		;Base of CP/M's TPA
	global	start,_main,_exit,__Lbss,__Hstack, __z3env
	global	__piped,__initrsx,__exact


reloc:		; DOS Protection	8080/Z80	x86
				;	---------  --------------------
	defb	0EBh,04h	;	EX DE,HL   JMPS LABE
				;	INC B

	ex	de,hl		;	EX DE,HL
	defb	0C3h		;	JP BEGIN
	defw	begin		; (We don't want the assembler optimising
				;  this to a relative branch)

labe:	defb	0B8h,0,09h	;		   MOV AX,9 ; print string

	defb	0BAh		;		   MOV DX,OFFSET BVMES
	defw	bvmes

	defb	0CDh,021h	;		   INT 21h  ; call DOS

	defb	0B8h,01h,4Ch	;		   MOV AX,4C01h ; terminate
				;		   ; with CTRL-BREAK code (1)

	defb	0CDh,021h	;		   INT 21h  ; call DOS
						   ; we never return here

	defb	13
	defm	'Compiled with Hi-Tech C [Relocatable]'
	defb	13,10,26

	psect	data
z3ecst:	defm	'Z3ENV'
bvmes:	defm	'This CP/M program requires a Z80 CPU.'
	defb	13,10,'$'
memmes:	defm	'Not enough memory to run.'
	defb	13,10,'$'
	psect	cpm

begin:	sub	a		;Stop 8080 processors running this.
	jp	po,okver
;
	ld	de,bvmes
	ld	c,9		;Print error message
	jp	5

okver:	
	push	hl		;HL holds possible Z3ENV address
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
	pop	hl
3:
	jp	start		;go to it

	psect	text
start:	ld	l,0		;Start of the page
	ld	a,(hl)
	cp	0c3h		;Must start with a jump
	jr	nz,noz3
	ld	l,1bh
	ld	a,(hl)		;Must refer to itself
	or	a
	jr	nz,noz3
	inc	hl
	ld	a,(hl)
	cp	h
	jr	nz,noz3
	ld	l,3		;Must contain the string "Z3ENV"
	ld	de,z3ecst
	ld	b,5		;Check 5 characters
z3ecmp:	ld	a,(de)
	cp	(hl)
	jr	nz,noz3
	inc	hl
	inc	de
	djnz	z3ecmp
	ld	l,0
	defb	3eh		;Swallow the LD HL
noz3:	ld	hl,0
yesz3:	ld	(__z3env),hl
;
memok:	
 
	ld	c,12
 	call    5		;Check CP/M version
 	ld	a,l
 	cp	30h
 	jr	nc,iscpm3
 	xor	a		; CP/M 2.2 has no RSX
 	jr	norsx

    ;If the PIPEMGR RSX is loaded, initialise it.
iscpm3:
	ld	c,3Ch
	ld	de,rsxpb
	call	5
	ld	a,h
	inc	l		;HL=00FFh if no PIPEMGR present.
	or	l
norsx:
	ld	(__piped),a
	call	nz,__initrsx	;use PIPEMGR for stdin, stdout and stderr
	ld	hl,(6)		;base address of fdos

	ld	sp,hl		;stack grows downwards
	ld	de,__Lbss	;end of initialized data
	scf
	sbc	hl,de		;size of uninitialized data area
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
	ld	sp,hl		;allow space for args
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
__z3env:
	defw	0
__piped:
        defw    0
__exact:		; Exact file size mode
	defb	'C',0	; 'C' is CP/M 2 (none), 'I' is ISIS, 'D' is DOSPLUS
rsxpb:	defb	79h,1
	defw	pipesgn
pipesgn:
	defm	'PIPEMGR '

	end	reloc
