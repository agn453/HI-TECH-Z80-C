	psect	text
	global	__getargs, startup, __argc_

startup:
	jp	__getargs

	psect	bss
__argc_:
	defs	2
