	psect	text
	global	iregset, iregstore, asaland, aslland, aland

asaland:
aslland:
	call	iregset
	call	aland
	jp	iregstore
