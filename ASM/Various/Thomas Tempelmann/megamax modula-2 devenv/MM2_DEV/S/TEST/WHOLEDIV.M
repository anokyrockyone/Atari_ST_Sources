
MODULE WholeTest;

FROM SYSTEM IMPORT ASSEMBLER;

CONST
  const65536 = LONGINT(65536);
  
VAR
  var65536   : LONGINT;
  
  varResult  : INTEGER;
  constResult: INTEGER;
  
  d          : LONGINT;

BEGIN
  var65536 := 65536;
  d := -65536;         (* Fehler tritt bei negativen Zahlen d auf. *)
  
  (* Division durch eine Variable, die 65536 enth�lt: *)
  ASSEMBLER BREAK END;
  varResult := SHORT(d DIV var65536);
  (* Klappt ja wunderbar. *)
  
  
  (* Wenn man jetzt die Variable durch eine Konstante ersetzt,
     k�nnte der Compiler doch so schlau sein, das Resultat durch
     ein SWAP zu erhalten... *)
  constResult := SHORT(d DIV const65536);
  (* Zu fr�h gefreut. Man sollte schon froh sein, wenn er's
     �berhaupt berechnen kann. *)

END WholeTest.
