**********************************************************************
************** INTRODUCTION DEMO ATROCITY - FLI BY RAZORBACK *********
************** MAIN CODE BY ZANAK - (C) 1994 ATROCITY Corp.  *********
**********************************************************************  

	SECTION TEXT
	
**********************************************************************
************** MEMOIRE - ECRANS **************************************
**********************************************************************

	MOVE.L	4(SP),A0
	
	MOVE.L	12(A0),D0	; TAILLE SECTION TEXT	
	ADD.L	20(A0),D0	; TAILLE SECTION DATA
	ADD.L	28(A0),D0	; TAILLE SECTION BSS
	ADDI.L	#256,D0	; TAILLE BASEPAGE
	ADDI.L	#256,D0	; TAILLE ERROR

	MOVE.L	D0,-(SP)	; TAILLE BLOCK ENTIER
	MOVE.L	A0,-(SP)	; ADRESSE BLOCK
	MOVE.W	#0,-(SP)
	MOVE.W	#$4A,-(SP)	; MSHRINK
	TRAP	#1	; CALL GEMDOS
	ADD.L	#12,SP
	
	TST.L	D0	; TESTE SI ERREUR ...
	BMI	FIN	; SI OUI FIN SINON
			; ON CONTINUE ...

**********************************************************************

	CLR.L	-(SP)
	MOVE.W	#$20,-(SP)	; SUPERVISEUR
	TRAP	#1
	ADDQ.L	#6,SP

	CLR.B	$FFFF8900.W		; IT DMA OFF
	CLR.B	$FFFF8901.W		; DMA STOP

************** CLS ***************************************************

	PEA	TEXT(PC)
	MOVE.W	#$9,-(SP)
	TRAP	#1
	ADDQ.L	#6,SP

**********************************************************************
	
	MOVE.L	D0,-(SP)	; OLD REZ 

********* OVERSCAN PREPA ****************************************

;	MOVE.W	$FFFF8288.W,D0
;	MOVE.W	$FFFF828A.W,D1
;	MOVE.W	D0,HBB(a6)
;	MOVE.W	D1,HBL(a6)
	
**********************************************************************
	
	MOVE.W	#2,-(SP)	; OU EST L'ECRAN PHYSIQUE ?
	TRAP	#14
	ADDQ.L	#2,SP
	MOVE.L	D0,PSCR	; IL EST LA ...
	
	MOVE.W	#3,-(SP)	; OU EST L'ECRAN LOGIQUE ?
	TRAP	#14
	ADDQ.L	#2,SP
	MOVE.L	D0,LSCR	; IL EST LA AUSSI ...
	
	MOVE.W	#-1,-(SP)	; ET LA RESOLUTION ?
	MOVE.W	#88,-(SP)
	TRAP	#14
	ADDQ.L	#4,SP
	MOVE.W	D0,REZ	; ELLE EST LA ...

**********************************************************************

	MOVE.L	#SIZE1,D0		; TAILLE 1
	ADDI.L	#256,D0
	CLR.B	D0
	MOVE.L	D0,ECRAN1	; CONFIG ECRAN1
	
	MOVE.L	#SIZE2,D0		; TAILLE 2
	ADDI.L	#256,D0
	CLR.B	D0
	MOVE.L	D0,ECRAN2	; CONFIG ECRAN2
	
**********************************************************************

	MOVE.W	#%100100,-(SP)	
	MOVE.W	#3,-(SP)
	MOVE.L	ECRAN1,-(SP)
	MOVE.L	ECRAN2,-(SP)
	MOVE.W	#5,-(SP)	; VSETMODE
	TRAP	#14
	ADD.L	#14,SP
	
**********************************************************************

	MOVE.B	$484.W,KEYBOARD	; COUPE LE CLAVIER
	CLR.B	$484.W
	
	DC.W	$A00A
	MOVE.B	#$12,$FFFFFC02.W	; COUPE LA SOURIS
	
**********************************************************************
*********** ROUTINE PRINCIPALE ***************************************
**********************************************************************
	
* Recopie l'image sur l'�cran
	lea.l	IMAGE+18,a0
	movea.l	ECRAN1,a1
	movea.l	ECRAN2,a2
	move.w	#320*200-1,d7
Copy_Img	moveq.l	#0,d0
	move.w	(a0)+,d1
	move.w	d1,d2
	move.w	d1,d3
	and.w	#$001f,d1
	lsr.w	#5,d2

	and.w	#$001f,d2		; Vert
	lsr.w	#8,d3
	lsr.w	#2,d3
	and.w	#$001f,d3

	or.b	d1,d0		; Rouge
	lsl.w	#6,d0

	lsl.w	#1,d2		; Vert
	or.b	d2,d0
	lsl.w	#5,d0

	or.b	d3,d0		; Bleu

	move.w	d0,(a1)+
	move.w	d0,(a2)+
	dbra	d7,Copy_Img

**********************************************************************	
*********** FIN DE LA DEMO *******************************************
**********************************************************************

FIN	CMP.B	#$39+$80,$FFFFFC02.W
	BNE	FIN
		
	DC.W	$A009
	MOVE.B	#8,$FFFFFC02.W
	MOVE.B	KEYBOARD,$484.W

	MOVE.W	REZ,-(SP)
	MOVE.W	#3,-(SP)
	MOVE.L	PSCR,-(SP)	; REMET ECRAN PHYSIQUE
	MOVE.L	LSCR,-(SP)	; REMET ECRAN LOGIQUE
	MOVE.W	#5,-(SP)
	TRAP	#14
	ADD.L	#14,SP
		
;	MOVE.W	HBB(a6),D0
;	MOVE.W	HBL(a6),D1
;	MOVE.W	D0,$FFFF8288.W
;	MOVE.W	D1,$FFFF828A.W

	CLR.L	-(SP)
	MOVE.W	#32,-(SP)	; USER	
	TRAP	#1
	ADDQ.L	#6,SP

	CLR	-(SP)	; RETOUR AU BUREAU
	TRAP	#1
	
**********************************************************************
*********** SOUS-ROUTINES ********************************************
**********************************************************************
	
************** LA VBL*************************************************

VBL0	MOVE.L	$466.W,D7
VBL1	CMP.L	$466.W,D7
	BEQ.B	VBL1
	RTS

WAIT	BSR	VBL0
	DBF	D0,WAIT
	RTS
	
**********************************************************************
	SECTION DATA
**********************************************************************

TEXT	DC.B	$1B,"E"
	DC.B	0

IMAGE	INCBIN	E:\SPHERE.TGA
	EVEN
				
**********************************************************************
	SECTION BSS
**********************************************************************

TEXTE	DS.W	2
SIZE1	DS.W	320*200+128
SIZE2	DS.W	320*200+128
PSCR	DS.W	2
LSCR	DS.W	2
REZ	DS.W	1
ECRAN1	DS.W	2
ECRAN2	DS.W	2
HBB	DS.W	1
HBL	DS.W	1
KEYBOARD	DS.B	1

	END