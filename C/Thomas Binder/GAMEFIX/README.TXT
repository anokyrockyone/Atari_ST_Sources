Wichtige Informationen zu den einzelnen Dateien:

GAMEFIX.C:    Der Quellcode des GEM-Frontends des Programms GAMEFIX
GAMEFIX.H:    Headerdatei f�r GAMEFIX.C, MINI1.C und MINI2.C
GAMEFIX.RSH:  Die Resourcedaten f�r GAMEFIX.C
GAMEFIX.PRJ:  Pure-C-Projektdatei f�r GAMEFIX.PRG
GAMEFIX.PRG:  Ausf�hrbares Programm zum Steigern der ST(E)-
              Kompatibilit�t des Falcon030
ASSEM.S:      Maschinenspracheteil f�r GAMEFIX.C, MINI1.C und MINI2.C
              Wird im Artikel als Listing 1 bezeichnet
MINI1.C:      Quellcode des aus GAMEFIX heraus speicherbaren 
              Miniprogramms 1 f�r bootende Spiele (im Artikel als 
              Listing 2 bezeichnet)
MINI1.HEX:    Das fertige Miniprogramm 1 als Hexdump, wird von 
              GAMEFIX.C per #include eingebunden
MINI2.C:      Quellcode des aus GAMEFIX heraus speicherbaren 
              Miniprogramms 2 f�r schnelles Einstellen und 
              eventuelles Starten eines Programms (im Artikel Listing 3)
MINI2.HEX:    Wie MINI1.HEX, f�r Miniprogramm 2
FALCON.H:     Die Headerdatei zur Falcon-XBIOS-Library
FALCNLIB.S:   Der Quellcode der Falcon-XBIOS-Library
FALCNLIB.PRJ: Pure-C-Projektdatei f�r FALCNLIB.LIB
FALCNLIB.LIB: Die fertig �bersetzte Falcon-XBIOS-Library f�r Pure C 1.0

