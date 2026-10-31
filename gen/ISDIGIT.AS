	global	_isdigit, _isdig

	psect	text
_isdigit:
_isdig:
	pop	de		;return address
	pop	hl
	push	hl
	push	de
	ld	a,h		;check for a char
	or	a
	jr	nz,nix
	ld	a,l
	cp	'0'
	jr	c,nix
	cp	'9'+1
	jr	nc,nix
	ld	hl,1		;yes
	ret
nix:	ld	hl,0
	ret
