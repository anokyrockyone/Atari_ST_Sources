ystart: equ -$1199
xstart:	equ -$1b33
yinc:	equ $16
xinc:	equ $e
	move #2,-(sp)
	trap #14
	addq #2,sp
	movea.l d0,a4
	lea (a4),a0
	move #$2000,d0
q0:	clr.l (a0)+
	dbf d0,q0
	lea (a4),a0
	move #400,d2
	movea #ystart,a3
q1:	movea #xstart,a2
	move #39,d1
q2:	move #15,d4
q3:	clr d5
	clr d6
	lea.l $0,a5
	lea.l $0,a6
	move #30,d3
q4:	sub.l a5,a6
	move.l a6,d7
	asl.l #4,d7
	swap d7
	add a2,d7
	muls d5,d6
	asl.l #5,d6
	swap d6
	add a3,d6
	move d7,d5
	muls d7,d7
	move.l d7,a6
	move d6,d0
	muls d0,d0
	move.l d0,a5
	add.l d0,d7
	cmpi.l #$4000000,d7
	bhi q5
	dbf d3,q4
q5:	andi #1,d3
	asl (a0)
	add d3,(a0)
	adda #xinc,a2
	dbf d4,q3
	��O�D��G�p�	��ω�A�D�p�	��O� �A�Dpp�	��ω���Fpp�   �O�@A�C��pp�����Fpp�	��O�D���p���pp