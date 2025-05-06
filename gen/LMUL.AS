;	Long multiplication for Z80

;	Called with 1st arg in HLDE, 2nd arg on stack. Returns with
;	result in HLDE, other argument removed from stack

	global	almul, llmul

	psect	text
almul:
llmul:
	ex	de,hl
	ex	(sp),hl		;return address now in hl
	exx
	pop	de		;low word in de
	pop	bc		;low word of multiplier in bc
	exx
	pop	bc		;hi word of multiplier
	push	hl		;restore return address
	ld	hl,0		;initialize product
	exx			;get lo words back
	ld	hl,0
	ld	a,c
	ld	c,b
	call	mult8b
	ld	a,c
	call	mult8b
	exx
	ld	a,c
	exx
	call	mult8b
	exx
	ld	a,b
	exx
	call	mult8b
	push	hl		;low word
	exx
	pop	de
	ret

mult8b:	ld	b,8
3:
	srl	a
	jp	nc,1f
	add	hl,de
	exx
	adc	hl,de
	exx
1:	ex	de,hl
	add	hl,hl
	ex	de,hl
	exx
	ex	de,hl
	adc	hl,hl
	ex	de,hl
	exx
	djnz	3b
	ret
