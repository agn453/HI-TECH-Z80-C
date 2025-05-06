	global	_isupper

	psect	text
_isupper:
	pop	de		;return address
	pop	hl
	push	hl
	push	de
	ld	a,h		;check for a char
	or	a
	jr	nz,nix
	ld	a,l
	cp	'A'
	jr	c,nix
	cp	'Z'+1
	jr	nc,nix
	ld	hl,1		;yes
	ret
nix:	ld	hl,0
	ret
