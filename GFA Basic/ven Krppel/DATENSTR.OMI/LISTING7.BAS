 '*********************************************
 '*               Listing 7                   *
 '*                                           *
 '*      RADIXSORT zur String-Sortierung      *
 '*         Sortierung rein nach ASCII        *
 '* Version 2  -  nur Sortierprozeduren       *
 '* Sven Kr�ppel, (c) MAXON Computer 1991     *
 '*********************************************

 DEF PROC Radixsort(R S_Anfang%,R S_Ende%)
   'Radix-Sortierung der Schlange S

   LOCAL Char_Nr%' Nummer des Zeichens, das als
                 ' Schl�ssel benutzt wird.
   Laengen_Sort(S_Anfang%,S_Ende%)
   FOR Char_Nr%=Max_Laenge% TO 1 STEP -1
     IF L_Anfang%(Char_Nr%)<>L_Ende%(Char_Nr%) THEN
        'nur sortieren, wenn mehr als ein Wort
        'in der Liste steht
        Binsort(L_Anfang%(Char_Nr%),L_Ende%(Char_Nr%),Char_Nr%)
     ENDIF
     Ll_Verbinden(L_Anfang%(Char_Nr%-1),L_Ende%(Char_Nr%-1),L_Anfang%(Char_Nr%),L_Ende%(Char_Nr%))
   NEXT Char_Nr%
   S_Anfang%=L_Anfang%(0)
   S_Ende%=L_Ende%(0)
 RETURN 'Radixsort

 DEF PROC Laengen_Sort(R S_Anfang%,R S_Ende%)
   'Die Prozedur sortiert die Schlange S in die
   'Beh�lter L(). Der Schl�ssel ist die Wort-
   'l�nge. Die Beh�lter speichern nach dem Auf-
   'ruf die Schlangen der entsprechenden Wort-
   'l�nge. Sie werden nicht aufgel�st.
   'S_Anfang% und S_Ende% sind nach dem Aufruf
   'undefiniert, da die Schlange S nicht mehr
   'existiert.

   LOCAL L%' Wortl�nge
   LOCAL I%' Laufvariable
   LOCAL P%' Hilfscursor

   'Beh�lter L l�schen
   FOR I%=0 TO Max_Laenge%
     L_Anfang%(I%)=0
     L_Ende%(I%)=0
   NEXT I%

   'S in Beh�lter sortieren
   WHILE S_Anfang%<>0
     P%=Wl_Next%(S_Anfang%)' Cursor auf das
               ' zweite Listenelement retten
     L%= LEN(Wl_Wort$(S_Anfang%))
     Ll_Anhaengen(L_Anfang%(L%),L_Ende%(L%),S_Anfang%)
     S_Anfang%=P%
   WEND
 RETURN 'Laengen_Sort


 DEF PROC Binsort(R S_Anfang%,R S_Ende%,Char_Nr%)
   'Die Prozedur sortiert die Strings der
   'Schlange S in die Beh�lter.
   'Char_Nr% : Nummer des Zeichens im String,
   '           das als Schl�ssel benutzt wird

   LOCAL Wort$' Wort, das einsortiert wird
   LOCAL Ch%' ASCII-Code des Zeichens, das
            ' einsortiert wird.
   LOCAL I%'  Laufvariable

   'Beh�lter l�schen
   FOR I%=Min_Ascii% TO Max_Ascii%
     B_Anfang%(I%)=0
     B_Ende%(I%)=0
   NEXT I%

   'S in Beh�lter sortieren
   WHILE S_Anfang%<>0
     P%=Wl_Next%(S_Anfang%)' Cursor auf das
               ' zweite Listenelement retten
     Wort$=Wl_Wort$(S_Anfang%)
     Ch%= ASC( MID$(Wort$,Char_Nr%,1))
     Ll_Anhaengen(B_Anfang%(Ch%),B_Ende%(Ch%),S_Anfang%)
     S_Anfang%=P%
   WEND

   'Beh�lter aufl�sen und sortierte Liste
   'generieren
   S_Anfang%=0
   S_Ende%=0
   FOR I%=Min_Ascii% TO Max_Ascii%
     IF B_Anfang%(I%)<>0 THEN 'Schlange in
        'Beh�lter B(I%) nicht leer => verbinden
        Ll_Verbinden(S_Anfang%,S_Ende%,B_Anfang%(I%),B_Ende%(I%))
     ENDIF
   NEXT I%

 RETURN 'Binsort



