	psect	text
	global	iregset, iregstore, asalmul, asllmul, almul

asalmul:
asllmul:
	call	iregset
	call	almul
	jp	iregstore
