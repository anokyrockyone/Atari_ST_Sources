
          TEXT

          INCLUDE   "INIT_TOS.L"
          INCLUDE   "MACROS.L"


          SETBLOCK

          PRINTLINE ATTENTION

          WAIT
          CMPI.B    #'F',d0
          BEQ       FORMAT
          CMPI.B    #'f',d0
          BEQ       FORMAT
          TERM

FORMAT    SUPER

          clr.l     d0

BOUCLE    movem.l   d0,-(sp)
          move.w    #$e5e5,-(sp)
          move.l    #$87654321,-(sp)
          move.w    #1,-(sp)
          move.w    #0,-(sp)
          move.w    d0,-(sp)
          move.w    #9,-(sp)
          move.w    #0,-(sp)
          clr.l     -(sp)
          pea       TAMPON
          move.w    #$a,-(sp)
          trap      #14
          add.l     #26,sp
          tst       d0
          bmi       ERROR
          movem.l   (sp)+,d0
          addi.b    #1,d0
          cmpi.b    #80,d0
          bne       BOUCLE

          PRINTLINE OK
GO        WAIT
          USER
          TERM

ERROR     move      d0,d5
          PRINTLINE PROBLEME
          neg       d5



          divu      #10,d5
          add       #$30,d5
          CCONOUT   d5
          swap      d5

          add       #$30,d5
          CCONOUT   d5
          jmp       GO


          DATA

ATTENTION DC.B      27,'E','DANGER ! Ce programme FORMATE la disquette, inserez'
          DC.B      ' une disquette VIERGE puis',10,13,'pressez [F] pour FORMATER la'
          DC.B      ' disquette... (Ou une autre touche pour QUITTER!)',7,0

OK        DC.B      27,'E','Pas d',39,'erreur : OK...',0

PROBLEME  DC.B      27,'E',7,'ERREUR de code :-',0


          BSS

          DS.B      20000    ;tampon en ammont de la nouvelle PILE
PILE      DS.B      1        ;le formatage n�cessite beaucoup de place...
          ALIGN.W            ;SPECIFIQUE � PROFIMAT !!!!!
TAMPON    DS.B      10000    ;tampon pour le formatage
SAUV_SP   DS.L      1        ;tampon pour SUPER et USER

          END


 