	global	_isspace

	psect	text
_isspace:
	pop	de		;return address
	pop	hl
	push	hl
	push	de
	ld	a,h		;check for a char
	or	a
	jr	nz,nix
	ld	a,l
	cp	' '
	jr	z,yes
	cp	12		;newline
	jr	z,yes
	cp	9		;tab
	jr	nz,nix
yes:
	ld	hl,1		;yes
	ret
nix:	ld	hl,0
	ret
