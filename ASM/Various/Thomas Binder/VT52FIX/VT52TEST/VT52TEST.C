/*
Testprogramm zum Feststellen, ob auch das Falcon-TOS > 4.01
den Fehler im VT52-Emulator hat. Geh�rt nicht zum Artikel,
nur zu Testzwecken!
*/


#include <tos.h>


int main(void)
{
	while (Cconis())
		Cnecin();

	Cconws("\033EVT52-Testprogramm f�r Falcon030\r\n");
	Cconws("-------------------------------\r\n\r\n");
	
	Cconws("Fehlerhafte Sequenzen:\r\n");
	Cconws("ESC b/ESC c: \033b");
	Cconout(0);
	Cconws("\033c\377Dieser Text m��te jetzt invertiert sein!\r\n");
	Cconws("\033b\xff\033c");
	Cconout(0);
	Cconws("ESC j/ESC k: \033jNach einem Tastendruck "
			"m��te dies �berschrieben werden!\r\n");
	Cnecin();
	Cconws("\033kUnd dies sollte �ber dem vorherigen Text stehen!\r\n");
	Cconws("\nLetzte Demonstration nach einem Tastendruck!\r\n");
	Cconws("Der Falcon wird falsch scrollen...\r\n\r\n");
	Cconws("Abbruch durch Tastendruck!\r\n");
	Cnecin();
	Cconws("\033v");
	do
	{
		Cconws("Scrolltest ");
	} while (!Cconis());
	return(0);
}
