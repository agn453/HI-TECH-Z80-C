;	This is a set of routines for floating point handling for C

;	The format of a floating point number is as follows:
;
;			------------
;			*   sign   *	1 bit
;			*----------*
;			* exponent *	7 bits
;			*----------*
;			* mantissa *	24 bits, normalized
;			------------
;
;		Note that the number is stored with the mantissa in the
;		low order bytes, i.e. the sign is the most significant
;		bit of the most significant byte.

	global	fpnorm, fladd, flsub, flmul, fldiv, negmant

	psect	text


;	fpnorm	- passed a floating point number in HLDE (sign and exponent
;		in H) - returns with it normalized.
;
;	Points to note:
;		Normalization consists of shifting the mantissa until there
;		is a 1 bit in the MSB of the mantissa.
;

fpnorm:
	ld	a,l		;check for zero mantissa
	or	d
	or	e
	jp	z,fpzero	;make it a clean zero
	push	hl		;save exponent and sign
	pop	bc		;get the exponent into b
	ld	c,b		;copy into c
	res	7,c		;reset the sign bit
2:
	bit	7,l		;test the MSB of the mantissa
	jr	nz,3f		;set, no more shifting required
	dec	c		;decrement exponent
	jp	m,fpovrflw	;underflow - set flag and return 0
	ex	de,hl		;get low word in hl
	add	hl,hl		;shift left
	ex	de,hl		;hi word back again
	adc	hl,hl		;shift bit in
	jr	2b		;loop and test again

3:
	bit	7,b		;test sign
	jr	z,4f		;skip if clear
	set	7,c		;set the new sign bit
4:
	ld	h,c		;put exponent and sign back where it belongs
	ret			;finished

;	Set the floating overflow flag and return zero. Floating execptions
;	may be caught in which case the appropriate routine will be called.

fpovrflw:
	ld	a,1
	ld	(fperr),a
fpzero:
	ld	hl,0
	ld	e,l
	ld	d,h
	ret

;	Negate the mantissa in LDE.

negmant:
	push	hl		;save hi byte and sign
	ld	hl,0
	ld	a,l		;zero a as well
	or	a		;reset carry
	sbc	hl,de		;negate low word
	ex	de,hl		;put back in de
	pop	hl		;restore hi byte
	sbc	a,l		;negate the hi byte
	ld	l,a		;put back
	ret			;and return

;	Floating subtraction. The value on the stack is subtracted from the
;	value in HLDE. To simplify matters, we do it thus:
;
;	A-B == A+-B

flsub:
	pop	bc		;return address
	exx			;get some other regs
	pop	de		;low word
	pop	hl		;hi word
	ld	a,h		;get sign/exponent
	or	a
	jr	z,1f
	xor	80h		;toggle sign
	ld	h,a		;put back
1:
	push	hl		;put back on stack
	push	de
	exx			;get other operand back
	push	bc		;and return address

	;fall through to fladd


;	Floating addition:
;		Add the value in HLDE to the value on the stack (under the
;		return address, and return with the argument removed from
;		the stack.

fladd:
	ld	a,l		;check 1st operand for zero
	or	d
	or	e		;only need to check mantissa
	exx			;get some spare registers
	pop	bc		;return address
	pop	de		;low word of 2nd operand
	pop	hl		;hi word
	push	bc		;put return address back on stack
	ret	z		;if 1st operand 0, just return 2nd
	ld	a,l		;check for zero 2nd arg
	or	d
	or	e		;if zero, just return the 1st operand
	ld	a,h		;put exponent in a
	exx			;restore 1st operand
	ret	z
	res	7,a		;clear sign
	ld	c,h		;get exponent
	res	7,c		;and clear sign
	sub	c		;find difference
	jr	nc,1f		;if negative,
	exx			; switch operands
	neg			;and make it positive
1:
	cp	24		;if less than 24 bits difference,
	jr	c,2f		;we can do the add
	exx			;otherwise just return the larger value
	ret
2:
	or	a		;check for zero difference
	call	nz,fpadjust	;adjust till equal
	ld	c,h		;save exponent of result
	bit	7,h		;test sign, do we need to negate?
	ld	h,0		;zero fill in case +ve
	jr	z,1f		;no
	call	negmant		;yes
	ld	h,0ffh		;1 fill top byte
1:
	push	de		;get low word
	exx			;select other bank
	bit	7,h		;test sign, do we need to negate?
	ld	h,0		;zero fill in case +ve
	jr	z,1f		;no
	call	negmant		;yes
	ld	h,0ffh		;1 fill top byte
1:
	pop	bc		;get low word of other operand
	ex	de,hl		;exchange hi/low
	add	hl,bc
	ex	de,hl		;restore
	exx			;get other bank again
	push	hl		;and hi word
	ld	a,c		;and exponent
	exx
	pop	bc
	adc	hl,bc		;add it in
	res	7,a		;clear sign from exponent
	jr	c,hm3
	inc	h
	dec	h		;zero in h?
	jr	z,hm2
hm3:
	sra	h		;now shift down 1 bit to compensate
	rr	l		;propogate the shift
	rr	d
	rr	e
	inc	a		;increment to compensate for shift above
hm2:
	push	af		;save carry flag
	ld	c,a		;save exponent
	ld	a,h
	and	80h		;mask off low bits
	or	c		;or in exponent
	ld	h,a		;now have it!
	call	m,negmant	;restore mantissa to positive if required
	pop	af		;restore carry flag
	call	c,round		;round up if necessary
	jp	fpnorm		;normalize and return!!

;	Round the number in HLDE up by one, because of a shift of bits out
;	earlier

round:
	ld	bc,1		;add in 1 extra bit
	ex	de,hl
	add	hl,bc
	ex	de,hl
	push	hl		;save exponent/sign
	ld	h,0
	ld	c,h
	adc	hl,bc		;add in carry
	pop	bc		;get exponent/sign back
	bit	0,h		;did it cause carry out of l?
	jr	z,2f
	srl	h
	rr	l
	rr	d
	rr	e
	ld	a,b		;get exponent/sign
	and	7fh		;get exponent only
	inc	a		;add one
	ld	c,a
	ld	a,b
	and	80h
	or	c		;now exponent and sign again
	ld	b,a
2:
	ld	h,b		;restore sign/exponent
	ret

;	Adjust the floating number in HLDE by increasing the exponent by the
;	contents of A. The mantissa must be shifted right to compensate.

fpadjust:
	and	31		;mask of hi bits - irrelevant
	ld	b,a		;put in a suitable register for loop count
1:
	srl	l
	rr	d
	rr	e
	inc	h		;increment exponent - it will not overflow
	djnz	1b		;loop if more
	ret			;finito

;	Get the right operand into HLDE', leave the left operand
;	where it is in HLDE, but make both of them +ve. The original
;	exponents/signs are left in C and B, left and right operands
;	respectively.

fsetup:
	pop	bc		;top return address
	exx
	pop	bc		;outer return address
	pop	de		;low word
	pop	hl		;hi word of right operand
	push	bc		;put return address back
	ld	c,h		;get exponent
	res	7,h		;clear sign
	ld	a,c		;exponent again
	exx
	push	bc		;inner return address
	ld	b,a		;other exponent
	ld	c,h		;this exponent
	res	7,h		;make positive
	ret

;	Floating multiplication. The number in HLDE is multiplied by the
;	number on the stack under the return address. The stack is cleaned
;	up and the result returned in HLDE.

flmul:
	call	fsetup		;get operands, make them +ve.
	push	bc		;save exponents etc.
	ld	h,0		;zero top byte
	push	hl		;push hi word
	ld	hl,0		;zero product
	exx
	ld	h,0		;zero top byte
	push	hl		;push hi word
	pop	bc		;put it into bc
	pop	hl		;hi word of multplicand
	ex	de,hl		;get it into de
	push	hl		;low word of multiplier
	ld	hl,0		;zero product
	exx
	pop	bc		;low word of multiplier
	ld	a,c		;get low 8 bits of multiplier
	ld	c,b		;save next 8 bits
	call	mult26		;do 8 bits of multiply
	ld	a,c
	call	mult8		;next 8 bits
	exx
	ld	a,c		;next 8 bits
	exx
	call	mult8		;do next chunk
	exx			;get hi words
	push	hl		;product hi word
	exx
	pop	de
	ex	de,hl		;hi word in hl, lo in de
	ld	a,h		;get hi byte
	ld	h,0
	ld	c,h		;zero lower byte
	jr	1f		;skip forward
2:
	srl	a
	rr	l
	rr	d
	rr	e
	rr	c		;save carry bit in c
	inc	h
1:
	or	a		;hi byte zero yet?
	jr	nz,2b		;no, keep shifting down
	ex	af,af'
	ld	a,c		;copy shifted-out bits
	ex	af,af'
	pop	bc		;get exponents
	bit	7,l		;check for zero mantissa
	jp	z,fpzero	;return a clean zero if so
	ld	a,c
	res	7,a		;mask off sign
	sub	41h		;remove bias, allow one bit shift
	add	a,h		;add in shift count
	sub	6		;compensate for shift up earlier
	ld	h,b		;the other
	res	7,h		;mask off signs
	add	a,h		;add them together
	jp	m,fpovrflw	;overflow in exponent
	ld	h,a		;put exponent in
	ld	a,c		;now check signs
	xor	b
	jp	p,1f
	set	7,h		;set sign flag
1:
	ex	af,af'
	rla			;shift top bit out
	ret	nc		;return if no carry
	jp	round		;round it

mult26:
	ld	b,6
3:
	srl	a		;shift LSB of multiplier into carry
	jp	nc,1f
	add	hl,de
	exx
	adc	hl,de
	exx
1:
	ex	de,hl
	add	hl,hl
	ex	de,hl
	exx
	ex	de,hl
	adc	hl,hl
	ex	de,hl
	exx			;shift multiplicand up one bit
	djnz	3b		;more?
	ld	b,2		;do remaining two bits
	jr	4f

mult8:
	ld	b,8
3:
	exx
	srl	h
	rr	l		;shift product down 1 bit
	exx
	rr	h
	rr	l
4:
	srl	a		;shift LSB into carry
	jp	nc,1f
	add	hl,de
	exx
	adc	hl,de
	exx
1:
	djnz	3b		;more?
	ret			;no, return as is


;	Floating division. The number in HLDE is divided by the
;	number on the stack under the return address. The stack is cleaned
;	up and the result returned in HLDE.

fldiv:
	call	fsetup		;get operands, make them +ve.
	push	bc		;save exponents etc.
	ld	h,0		;zero top byte of dividend
	push	de		;push lo word
	ld	bc,0		;zero quotient
	exx
	ld	h,0		;zero top byte of divisor
	ex	(sp),hl		;get lo word of dividend into hl
	ld	bc,0		;zero low word of quotient
	exx
	pop	de		;hi word of divisor
	ld	a,24+6		;number of bits in dividend and then some

3:
	push	hl		;save dividend
	exx
	push	hl		;low word
	or	a		;reset carry
	sbc	hl,de		;try a subtraction
	exx
	sbc	hl,de		;now the hi word
	exx
	jr	nc,4f		;skip if no carry
	pop	hl		;restore dividend
	exx
	pop	hl
	exx
	jr	5f

4:
	inc	sp		;unjunk stack
	inc	sp
	inc	sp
	inc	sp

5:
	ccf			;complement carry bit
	rl	c		;shift into quotient
	rl	b
	exx
	rl	c
	rl	b
	exx			;low words again
	add	hl,hl		;shift dividend left
	exx
	adc	hl,hl		;upper shift
	dec	a		;decrement loop count
	jr	nz,3b

	exx
	push	bc		;get low word of quotient
	exx
	pop	de
	push	bc		;hi word
	pop	hl
	ld	a,h		;get hi byte
	ld	h,0
	ld	c,h		;zero lower byte
	jr	1f		;skip forward
2:
	srl	a
	rr	l
	rr	d
	rr	e
	rr	c		;save carry bit in c
	inc	h
1:
	or	a		;hi byte zero yet?
	jr	nz,2b		;no, keep shifting down
	ex	af,af'
	ld	a,c		;copy shifted-out bits
	ex	af,af'
	pop	bc		;restore exponents
	push	bc		;save signs
	ld	a,c
	res	7,a
	res	7,b
	sub	b
	add	a,41h-6		;compensate
	add	a,h
	ld	h,a
	pop	bc
	ld	a,c
	xor	b		;get sign
	jp	p,1f
	set	7,h
1:	
	ex	af,af'		;get shifted out bit back again
	rla
	call	c,round		;round if necessary
	jp	fpnorm		;normalize it and return

	psect	bss

fperr:
	defs	1		;floating over/underflow flag
