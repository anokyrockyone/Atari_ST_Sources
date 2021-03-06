 ;
 ;  MOUSER.S -- Display the Mouse Position on the Screen
 ;                by S. M. Baugh
 ;
 TEXT
meminit:
  move.l    4(sp),a6         ; do memory shrink
  move.l    $18(a6),d0
  add.l     $1C(a6),d0
  and.l     #-2,d0           ; make even address
  move.l    d0,d2
  add.l     #$2000,d0        ; stack size
  move.l    d0,sp

  move.l    a6,d1
  sub.l     d1,d2
  move.l    d2,-(sp)
  move.l    a6,-(sp)
  move.w    #0,-(sp)
  move.w    #$4A,-(sp)
  trap      #1
  add.l     #12,sp

init:
  dc.w      $A000            ; initialize Line A
  move.l    a0,lineavar      ; save variable address
  dc.w      $A00A            ; hide Mr. Mouse

  move.w    #$1B,d0          ; Clear screen
  bsr       showit
  move.w    #'E',d0
  bsr       showit

  move.l    #$C000C,oldx     ; current position of mouse

init2:
  move.l    lineavar,a0      ; transform Mr. Mouse to Mr. Moose
  move.l    8(a0),a6         ; intin array
  clr.w     (a6)             ;   intin(0) x pos of hot spot
  clr.w     2(a6)            ;   intin(1) y pos of hot spot
  move.w    #1,6(a6)         ;   intin(3) mask color value
  move.w    #2,8(a6)         ;   intin(4) data color
  add.l     #10,a6           ; --> intin(5)
  lea       mausdat,a4       ; data for Mr. Moose
  move.w    #16,d7           ; counter
\LP:
  move.l    (a4)+,(a6)+
  dbra      d7,\LP
  dc.w      $A00B            ; transform it!

  dc.w      $A009            ; show mouse


  move.w    #34,-(sp)        ; get and save mouse handler vector
  trap      #$E              ; kbdvbase XBIOS call
  addq.l    #2,sp
  move.l    d0,a0
  move.l    d0,tabsav
  move.l    $10(a0),vecsav
  move.l    #myvec,$10(a0)

  move.l    #0,vecbuf        ; clear mouse vector buffer

main:
  lea       vecbuf,a6
  cmp.b     #$F9,(a6)        ; mouse handler fills w/ absolute mouse
  beq       restore          ;   right button pressed ... exit

  clr.l     d0
  move.w    $26E0,d0         ; read mouse vector
  move.w    $26E2,d1
  cmp.w     oldx,d0
  bne       update           ; not equal then display new position
  cmp.w     oldy,d1
  beq       main             ; equal then mouse has not moved

update:
  move.w    d0,oldx          ; save new parameters
  move.w    d1,oldy
  lea       oldx,a0          ; input integer
  lea       xpos,a1          ; output ASCII area
  bsr       BINHEX_W         ; convert int to ASCII
  lea       oldy,a0          ; same for y
  lea       ypos,a1
  bsr       BINHEX_W
  move.w    #$1B,d0          ; reset cursor ...
  bsr       showit
  move.w    #'Y',d0
  bsr       showit
  move.w    #$20,d0          ; ... to 0,0 (+ required offset of $20)
  bsr       showit
  move.w    #$20,d0
  bsr       showit

  lea       xpos,a6          ; show x & y
  move.l    #3,d7            ; counter of ascii characters in number -1
\LP1:
  clr       d0
  move.b    (a6)+,d0
  bsr       showit
  dbra      d7,\LP1

  move.w    #' ',d0          ; space
  bsr       showit

  lea       ypos,a6          ; same for y position
  move.l    #3,d7
\LP2:
  clr       d0
  move.b    (a6)+,d0
  bsr       showit
  dbra      d7,\LP2
  bra       main             ; & continue process

restore:
  move.l    tabsav,a0
  move.l    vecsav,$10(a0)   ; restore mouse handler

exit:
  clr.w     -(sp)
  trap      #1

 ; ***** Subroutines *****

showit:                      ; write character to screen
  move.w    d0,-(sp)
  move.l    #$30002,-(sp)
  trap      #$D
  addq.l    #6,sp
  rts

 ; converts word size binary # to hex ASCII string
 ;   input: a0 -> input integer  a1 -> output ASCII buffer
BINHEX_W:
         clr.l     d0
         move.l    #1,d1               ; loop counter - 1 rep
\1:
         move.b    (a0),d0             ; high nibble of char
         andi.l    #$F0,d0             ; clears low & 2 high nibbles
         ror       #4,d0               ; shift high nibble to low for offset
         cmpi.b    #10,d0              ; d0 < $A ?
         blt.s     \2                  ; yes, skip next line
         add.b     #'A'-'0'-10,d0      ; add "A-F" minus next line + offset
\2:
         add.b     #'0',d0             ; add ASCII "0" to offset
         move.b    d0,(a1)+            ; save to ouput area
         move.b    (a0)+,d0            ; repeat for low nibble
         andi.l    #$0F,d0             ; clear high nibble
         cmpi.b    #10,d0
         blt.s     \3
         add.b     #'A'-'0'-10,d0
\3:
         add.b     #'0',d0
         move.b    d0,(a1)+
         dbra      d1,\1               ; loop 1 more time
BHWFINIS:                              ; global label resets local labels
         rts

 ; Mouse handler

myvec:                        ; move a byte from mouse report
  move.b    (a0),vecbuf       ;   without changing a0
  move.l    vecsav,-(sp)      ; put OS handler addr on stack and
  rts                         ;   jmp to let OS continue processing

 DATA
 ALIGN

mausdat:      dc.w $C413,$CC33,$FDB7,$FFFF,$3FF8,$1FF8,$1FF8,$1FF8
              dc.w $3FFC,$7FFE,$7FFE,$7FFE,$3FFC,$3FF8,$0FF0,$0000

daten:
  dc.w      $0,0,0,0,0,$660,$660,0,0,0,0,$630,$630,0,0,0,0,0

 BSS
 ALIGN
lineavar    ds.l  1
xpos:       ds.l  1
ypos:       ds.l  1
oldx:       ds.w  1
oldy:       ds.w  1
tabsav:     ds.l  1
vecsav:     ds.l  1
vecbuf:     ds.w  3

 END
 