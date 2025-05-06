	global	_islower

	psect	text
_islower:
	pop	de		;return address
	pop	hl
	push	hl
	push	de
	ld	a,h		;check for a char
	or	a
	jr	nz,nix
	ld	a,l
	cp	'a'
	jr	c,nix
	cp	'z'+1
	jr	nc,nix
	ld	hl,1		;yes
	ret
nix:	ld	hl,0
	ret
