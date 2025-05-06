	global	asfladd, asflsub, asflmul, asfldiv
	global	fladd, flsub, flmul, fldiv
	global 	iregset, iregstore
	psect	text

asfladd:
	call	iregset
	call	fladd
	jp	iregstore
asflsub:
	call	iregset
	call	flsub
	jp	iregstore
asflmul:
	call	iregset
	call	flmul
	jp	iregstore
asfldiv:
	call	iregset
	call	fldiv
	jp	iregstore
