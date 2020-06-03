; Fake overlay


size	equ	05000h		; max overlay size

	global	__ovrbgn, __ovrsize, __ovrstart

	psect	text

__ovrsize:
	defw	size		; this should never be overlaid
__ovrstart:
	defw	__ovrbgn

	psect	bss		; this goes in the uninit bit
__ovrbgn:
	defs	size		; reserve room for overlay

	end
