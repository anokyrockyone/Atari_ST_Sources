DEFINITION MODULE Granule;

(*
 * Dieses Modul ist eine alternative Speicherverwaltung zu Storage.
 * Der Vorteil ist, da� sie f�r Bl�cke in 16-Byte-Portionen optimiert
 * ist und fast keinen Verwaltungs-Overhead hat.
 *
 * Sie ist u.U. auch schneller als Storage. Allerdings erkennt sie
 * nicht, welche Bl�cke zusammengeh�ren - das Anwendungsprogramm mu�
 * selbst genau darauf achten, wieviel es alloziert hatte, wenn es einen
 * Bereich wieder freigeben will. DEALLOCATE (x, 0) ist also hier nicht
 * erlaubt - es mu� immer exakt die Gr��e des belegten Bereichs angegeben
 * werden.
 *
 * Granule wurde speziell f�r den GME entwickelt. Der GME verwaltet jede
 * einzelne Zeile �ber dieses Modul in einem eigens allozierten Block.
 *
 * Ob Granule oder Storage f�r Ihre Anwendung besser geeignet ist,
 * probieren Sie am Besten in der Praxis aus.
 * Um die Funktionen 'CreateHeap', 'SetDefaultSize' usw. brauchen Sie sich
 * dabei nicht unbedingt zu k�mmern - sie dienen nur Optimierungszwecken.
 *
 * Hinweis:
 * Die aktuelle Implementation dieses Moduls benutzt bei TT- und Falcon-
 * Rechnern nur das langsamere ST-RAM, also kein TT-RAM.
 * Wen das st�rt, setze sich mit Th. Tempelmann in Verbindung oder siehe
 * die Notizen vom Source des Moduls (die Quelltexte aller Module sind
 * ebenfalls dort zu bekommen). Der Nachteil w�re dann allerdings, da� ca.
 * 100 KB zus�tzlich ungenutzt belegt werden m��ten, falls TT-RAM verf�gbar
 * ist.
 *)

FROM SYSTEM IMPORT  ADDRESS;


PROCEDURE ALLOCATE (VAR adr: ADDRESS; bytes: LONGCARD);
(* Belegt Speicher ab adr mit bytes Bytes. Wenn Dynamic = TRUE
 * (siehe Prozedur Dynamic) wird bei einem Heap�berlauf ein weiteres
 * Speichersegment angefordert und in den Heap integriert. Wenn FALSE
 * oder kein Speicher mehr zu Verf�gung steht, wird adr NIL gesetzt.
 *)

PROCEDURE DEALLOCATE (VAR addr: ADDRESS; bytes: LONGCARD);
(* Gibt den Speicher wieder frei. 'bytes' mu� den selben Wert haben,
 * mit dem der Speicher f�r 'addr' vorher belegt wurde.
 * (werden NEW und DISPOSE verwendet, wird daf�r automatisch Sorge getragen).
 *)

PROCEDURE Shrink (VAR adr: ADDRESS; amount: LONGCARD; VAR size: LONGCARD);
(* Gibt mit allocate belegten Speicher frei.
 * 'amount' ist die Anzahl, um die der Block verkleinert werden soll,
 * 'size' mu� beim Aufruf mit der alten Gr��e besetzt sein und enth�lt
 * hinterher die neue Gr��e.
 * Der freizugebende Teil wird am Ende des Blocks abgezwackt und f�r
 * neue ALLOCATEs zur Verf�gung gestellt.
 *)

PROCEDURE CreateHeap (bytes: LONGCARD): INTEGER;
(* Installiert den Heap mit bytes Byte. Liefert einen Fehlercode mit
 * folgender Bedeutung:
 *
 *  0   = Kein Fehler, Heap wurde installiert
 * -1   = Kein Speicher in ausreichender Gr��e zur Verf�gung
 * -2   = Heap ist bereits installiert
 *
 * Wird kein Heap installiert, wird beim ersten ALLOCATE-Aufruf
 * automatisch ein Heap v. 64KB erzeugt.
 *)
 
PROCEDURE Dynamic (dyn: BOOLEAN);
(* Schaltet die Dynamic-Option des Heaps.
 * TRUE  = ALLOCATE fordert bei Heap�berlauf neues Speichersegment an.
 * FALSE = ALLOCATE liefert bei Heap�berlauf immer NIL
 *)

PROCEDURE SetDefaultSize (size: LONGCARD);
(* Setzt die Standard-Heapgr��e, um die der Heap erweitert wird,
 * wenn er bei "dynamic=TRUE" um ein neues Segment erweitert werden mu�.
 * Der Default ist 64 KB
 *)

PROCEDURE Free (): LONGCARD;
(* Liefert Anzahl der Bytes, die auf dem bereits angelegten Heap noch
 * frei sind.
 *)

PROCEDURE MemAvail (): LONGCARD;
(* liefert die Anzahl aller freien Bytes sowohl im Heap, als auch im
 * noch nicht allozierten Speicher - abz�glich der GEMDOS-Reserve
 * (Konstante GEMReserve, z.Zt. 64KB)
 *)

END Granule.
