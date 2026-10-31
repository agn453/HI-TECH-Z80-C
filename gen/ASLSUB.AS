	psect	text
	global	iregset, iregstore, asalsub, asllsub, alsub

asalsub:
asllsub:
	call	iregset
	call	alsub
	jp	iregstore
