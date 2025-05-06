	global	_isalpha

	psect	text
_isalpha:
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
	cp	'z'+1
	jr	nc,nix
	cp	'a'
	cp	'Z'+1
	jr	c,yes
	cp	'a'
	jr	c,nix
yes:
	ld	hl,1		;yes
	ret
nix:	ld	hl,0
	ret
