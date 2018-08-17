; file as.as

.entry LENGTH
.extern W
MAIN:	mov	r3,LENGTH
LOOP:	jmp	L1(#-1,r6)
	prnn	 #-5
L1:	inc	K
.entry LOOP
	bne	LOOP(K,W)
END:	stop
LENGTH:	.data	6
K:	.data 	4
.extern L3
