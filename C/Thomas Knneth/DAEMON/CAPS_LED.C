/*
	CAPS_LED

	-------------------------------------------------
	- Begleitlisting zum Artikel in der ST-Computer -
	-  Demonstration von MiNT Hintergrundprozessen  -
	-------------------------------------------------
	
	geschrieben von:
	Thomas K�nneth
	Friedrich-Neuper-Str. 2b
	90552 R�thenbach

	Compiler:	Pure C 1.1
	Datum:		7.7.1994
	Tabweite:	4
*/

/* Headerdateien */
#include <stdlib.h>
#include <tos.h>
#include <aes.h>
#include <vdi.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

/* ab PureC 1.1 kann diese Zeile entfallen. �letre PureC-Versionen
	ben�tigen diese datei. Sie liegt der Sourcecode-Distribution von
	MiNT bei */
#include <mintbind.h>

typedef enum { FALSE,TRUE } BOOLEAN ;

/* Makros */
#define	CAPSLED1	"\n\r\033p CAPS_LED \033q"
#define	CAPSLED2	"\n\r\xBD Thomas K�nneth, R�thenbach"
#define	ERMEM		"\n\rFehler: nicht genug Speicher f�r den Child-Proze�"
#define	NOMINT		"\n\rFehler: MiNT nicht installiert - CAPSLED nicht lauff�hig"

#define	ENSMEM	-39		/* insufficient memory */

/* Funktionsprototypen */
void output(void);
void cdecl signal_handler(long which_signal);
void cdecl simple_handler(long which_signal);
BOOLEAN get_cookie(long cookie,long *value);

/* globale Variablen */
int		my_handle,
		work_out[57];		/* wird f�r v_opnvwk /vq_extnd ben�tigt */
BOOLEAN	abbruch;
long	MiNT_version;

void main(void)
{
	int 	work_in[11],
			aes_handle,
			apid,				/* AES-Kennung */
			i,					/* Schleifenvariablen */
			dummy;
	/* ersten 2 Zeilen des Begr��ungstextes ausgeben */
	Cconws(CAPSLED1);
	Cconws(CAPSLED2);
	/* ohne MiNT gebootet? - Fehlermeldung und Schlu� */
	if (get_cookie('MiNT',&MiNT_version) == FALSE) {
		Cconws(NOMINT);
		exit(1);
	}
	/* eine Kopie des Prozesses anlegen */
	switch (Pvfork()) {
		case 0:
			/* so, wir sind also der Kind-Proze� -> Elternproze� killen */
			Pkill(Pgetppid(),SIGKILL);
			break;
		case ENSMEM:
			/* nicht genung Speicher f�r den Child-Proze� */
			Cconws(ERMEM);
			exit(1);
		default:
			/* i'm the parent - wating to be killed by my child :-( */
			while (TRUE) Syield();
	}
	/* jetzt m�ssen wir solange warten, bis die AES da sind */
	Psignal(SIGALRM,simple_handler);
	for (;;) {
		apid = appl_init();
		if (_GemParBlk.global[0] != 0)
			break;
		/* wir warten 4 Sekunden und hoffen, da� die AES beim n�chsten
			Versuch da sind */
		Talarm(4);
		Pause();
	}
	/* hierher kommen wir erst, wenn die AES da sind oder ein Fehler
		aufgetreten ist */
	if (apid < 0)
		exit(1);	/* da hat was nicht geklappt */
	/* Handle der physik. Bildschirm-Workstation erfragen */
	aes_handle = graf_handle(&dummy,&dummy,&dummy,&dummy);
	/* bei den AES verabschieden */
	appl_exit();
	/* �ffnen einer virtuellen Bildschirm-Workstation */
	for(i = 0; i < 10; work_in[i++] = 1) ;
	work_in[10] = 2;
	my_handle = aes_handle;
	v_opnvwk(work_in,&my_handle,work_out);
	if (!my_handle)
		exit(1);
	/* Schreibmodus auf REPLACE */
	vswr_mode(my_handle,MD_REPLACE);
	/* F�lltyp deckend */
	vsf_interior(my_handle,FIS_SOLID);
	/* keine Umrandung */
	vsf_perimeter(my_handle,0);
	/* Handler f�r verschiedene Signale installieren */
	Psignal(SIGALRM,signal_handler);
	Psignal(SIGTERM,signal_handler);
	Psignal(SIGABRT,signal_handler);
	abbruch = FALSE;
	/* die Programm-Hauptschleife */
	while (abbruch != TRUE) {
		output();	/* die stilisierte LED malen */
		Talarm(1);	/* Timer auf 1 Sekunde stellen */
		Pause();	/* hier wird der Proze� schlafen gelegt */
	}
	/* die Workstation schlie�en */
	v_clsvwk(my_handle);
	/* und ordnungsgem�� beenden */
	exit(0);
}

/* Signal-Handler: sorgt z.B. f�r den Anzeigewechsel und das Neuzeichnen */
void cdecl signal_handler(long which_signal)
{
	switch ((int)which_signal) {
		case SIGALRM:
			/* unser mit Talarm() gesetzter Timer ist abgelaufen; wir k�nnen
				hier ggf. ein paar Aktionen t�tigen. (X)BIOS- und GEMDOS-
				Calls sind m�glich, AES- und VDI-Aufrufe sind zu unterlassen
			*/
			break;
		case SIGABRT:
		case SIGTERM:
			/* und tsch�s... */
			abbruch = TRUE;
			break;
	}
}

void output(void)
{
	int	x_pos,
		pxyarray[8];
	vq_extnd(my_handle,0,work_out);
	x_pos = work_out[0];
	/* Unrandung der CapsLock - Anzeige */
	vsl_color(my_handle,1);
	/* F�llfarbe */
	vsf_color(my_handle,0);
	pxyarray[0] = (x_pos - 16);
	pxyarray[1] = 0;
	pxyarray[2] = x_pos;
	pxyarray[3] = 10;
	v_bar(my_handle,pxyarray);
	/* Abfrage, welche Sondertaste gedr�ckt wurde. Wenn in >main< das
		appl_exit() erst kurz vor Programm-Ende gemacht wird kann und
		sollte statt dem BIOS lieber das AES befragt werden */
	if (Kbshift(-1) & 16) {
		/* F�llfarbe rot */
		vsf_color(my_handle,2);
		v_circle(my_handle,(x_pos - 4),4,4);
		/* F�llfarbe */
		vsf_color(my_handle,0);
	}
	v_arc(my_handle,(x_pos - 4),4,4,0,3600);
}

void cdecl simple_handler(long which_signal)
{
	/* die Warning >>Parameter 'which_signal' is never used in function
		simple_handler<< kann selbstverst�ndlich ignoriert werden */
}

BOOLEAN get_cookie(long cookie,long *value)
{
	long oldstack = Super(0L);
	long *cookiejar = *((long **)0x5a0L);
	Super((void *)oldstack);
	if (cookiejar != NULL)
		do {
			if (cookiejar[0] == cookie) {
				if (value)
					*value = cookiejar[1];
				return (TRUE);
			}
			else
				cookiejar = &(cookiejar[2]);
		}
		while (cookiejar[-2]);
	return (FALSE);
}
