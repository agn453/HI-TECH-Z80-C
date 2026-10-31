	psect	text
	global	iregset, iregstore, asalor, asllor, alor

asalor:
asllor:
	call	iregset
	call	alor
	jp	iregstore
