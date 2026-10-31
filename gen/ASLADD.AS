	psect	text
	global	iregset, iregstore, asaladd, aslladd, aladd

asaladd:
aslladd:
	call	iregset
	call	aladd
	jp	iregstore
