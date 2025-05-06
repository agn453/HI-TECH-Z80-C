; void setenv(char *name, char * val)
global  csv, cret, bDos
psect text
global _setenv
_setenv:
pop	bc ; return address
pop	hl ; name
pop	de ; val
push	de
push	hl
push	bc
ld	c,6Ch ; _SENV
jp	bDos

