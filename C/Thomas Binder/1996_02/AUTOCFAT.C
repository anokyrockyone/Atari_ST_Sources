/*
 * autocfat.c vom 15.01.1996
 *
 * Autor:
 * Thomas Binder
 * (binder@rbg.informatik.th-darmstadt.de)
 *
 * Zweck:
 * Programm f�r den Auto-Ordner, das CheckFat der
 * Reihe nach f�r bestimmte Laufwerke aufruft und
 * bei fehlerhafter FAT einen zweiten Test mit
 * erweiterten Parametern erm�glicht.
 *
 * Vielen Dank auch an meine Betatester (in alpha-
 * betischer Reihenfolge):
 * Alexander Clauss, Dirk Klemmt, Rainer Riedl,
 * Michael Schwingen, Uwe Seimet, Manfred Ssykor
 * und Arno Welzel.
 *
 * History:
 * 29.03.1995: Erstellung
 * 02.04.1995: �nderung der Abbruchbedingung bei
 *             Programmstart: CheckFat wird nicht
 *             aufgerufen, wenn keine Sondertaste
 *             gedr�ckt ist. Man mu� also zum
 *             Test explizit Shift o.�. dr�cken,
 *             weil ein Test bei jedem Warmstart
 *             doch etwas nervig ist. Wer es doch
 *             andersherum m�chte, braucht nur die
 *             anf�ngliche Abfrage zu negieren.
 * 12.04.1995: CheckFat-Pfad ist jetzt c:\bin
 * 21.07.1995: AutoCFat wertet jetzt eine Datei
 *             AUTOCFAT.INF aus, die im Auto-
 *             Ordner oder im Wurzelverzeichnis
 *             des Bootlaufwerks liegen darf. Die
 *             erste Zeile enth�lt den Pfad f�r
 *             CheckFat, die zweite die Laufwerke,
 *             die �berpr�ft werden sollen (als
 *             Zeichenkette, also z.B. CDE).
 * 10.08.1995: Falschen Kommentar ge�ndert.
 * 24.08.1995: Anpassung an MiNT-Library
 * 10.10.1995: Anpassung an neue Version von
 *             CheckFat, die jetzt auch mehrfach
 *             belegte Cluster mit Dateinamen
 *             melden kann.
 * 12.10.1995: AutoCFat merkt sich jetzt zu Beginn
 *             das aktuelle Laufwerk und den
 *             aktuellen Pfad und setzt beides
 *             am Schlu� wieder. Damit klappt das
 *             Pr�fen von C: auch dann, wenn
 *             AutoCFat unter MagiC 3 aus dem
 *             Auto-Ordner von C: gestartet wurde.
 *             (Bisher gab es hier einen Absturz,
 *             weil durch das Dlock von CheckFat
 *             der aktuelle Pfad des Bootlaufwerks
 *             verloren ging, was der Auto-Ordner-
 *             Abarbeitung von MagiC 3 �berhaupt
 *             nicht gef�llt...)
 * 25.10.1995: AutoCFat sollte, wegen �hnlicher
 *             Probleme wie mit MagiC, immer _vor_
 *             MiNT im Auto-Ordner stehen.
 *             Anpassung an neue Option -d von
 *             CheckFat.
 * 29.10.1995: Damit es keine Probleme mehr mit
 *             MiNT und anderen Betriebssystemen,
 *             die Dlock zur Verf�gung stellen,
 *             gibt, ruft AutoCFat CheckFat f�r
 *             das aktuelle Laufwerk mit der neuen
 *             Option -u auf, die die Benutzung
 *             von Dlock abschaltet. Ernsthaftere
 *             Probleme sind dadurch nicht zu
 *             erwarten, da AutoCFat ja im Auto-
 *             Ordner l�uft (bzw. laufen sollte),
 *             und zu der Zeit ist in der Regel
 *             noch kein Multitasking aktiv. Die
 *             beste L�sung ist es ohnehin, das
 *             Programm m�glichst weit an den
 *             Anfang des Auto-Ordners zu setzen.
 * 30.10.1995: In der dritten Zeile der INF-Datei
 *             wird jetzt festgelegt, ob eine der
 *             Umschalttasten AutoCFat aktiviert
 *             oder das Pr�fen verhindert. Bisher
 *             war das nur im Source zu �ndern.
 *             Die alte INF-Datei mu� daher auf
 *             jeden Fall angepa�t werden!
 * 12.12.1995: Die dritte Zeile wurde erweitert.
 *             Es kann jetzt festgelegt werden,
 *             welche Sondertasten gepr�ft werden,
 *             ob diese einzeln oder zusammen
 *             gedr�ckt sein m�ssen und ob
 *             AutoCFat nur bei einem Kaltstart
 *             aktiv werden soll.
 *             Bei Fehlern in der INF-Datei werden
 *             jetzt etwas genauere Fehler
 *             gemeldet.
 * 18.12.1995: Dummerweise war die Auswertung des
 *             TUBS-Cookies in is_cold_boot genau
 *             falsch herum... Jetzt tut's.
 * 19.12.1995: Die Reihenfolge der Abbruchs-
 *             Abfrage vertauscht, damit jetzt auf
 *             jeden Fall auf Kaltstart getestet
 *             wird. Bisher ging AutoCFat n�mlich
 *             von einem Kaltstart aus, wenn es
 *             zuvor immer per Hotkey am Start
 *             gehindert wurde.
 * 15.01.1996: Letzte Vorbereitungen f�r die
 *             Ver�ffentlichung, darunter auch
 *             verbessertes Verhalten, wenn
 *             CheckFat nicht korrekt aufgerufen
 *             werden konnte (z.B. durch falschen
 *             Pfad in der INF-Datei).
 */

#include <mintbind.h>
#include <portab.h>

/* Defines f�r die Sondertasten */
#define RSHIFT  1
#define LSHIFT  2
#define CTRL    4
#define ALT     8
#define CLOCK   16

/*
 * In diesem Char-Array stehen alle zu pr�fenden
 * Laufwerke (wird aus AUTOCFAT.INF gelesen)
 */
char    to_check[256];

/* Pfad von CheckFat, auch aus AUTOCFAT.INF */
char    cfpath[256];

/* Puffer f�r die Zeile zum Hotkey-Verhalten */
char    hotkey[256];

/* Prototypen */
WORD readline(WORD handle, char *buffer);
WORD is_cold_boot(void);
WORD get_cookie(ULONG cookie, ULONG *value);

WORD main(void)
{
    WORD    result,
            handle,
            skip_with_hotkey,
            needed,
            possible,
            only_cold,
            hotkey_ok,
            pos,
            names,
            actdrv,
            cluster = 1,
            crosslinks = 0,
            marked = 0,
            longnames = 0;
    LONG    err;
    char    cline[9],
            *hot,
            *check,
            actpath[256];

/* Versuchen, das INF-File zu �ffnen */
    if (((handle = (WORD)
        Fopen("\\auto\\autocfat.inf", 0)) < 0) &&
        ((handle = (WORD)
        Fopen("\\autocfat.inf", 0)) < 0))
    {
        Cconws("\r\nKann AUTOCFAT.INF nicht "
            "�ffnen!\r\n");
        return(1);
    }
/*
 * INF-Datei ist vorhanden, jetzt die erste Zeile
 * einlesen, die keine Kommentarzeile ist. Gibt es
 * sie nicht, mit einer Fehlermeldung abbrechen.
 */
    if (!readline(handle, cfpath) || !*cfpath)
    {
        Cconws("\r\n1. Zeile von AUTOCFAT.INF "
            "leer!\r\n");
        Fclose(handle);
        return(1);
    }
/*
 * Jetzt die zweite Zeile mit den zu pr�fenden
 * Laufwerken lesen; auch hier abbrechen, wenn sie
 * nicht vorhanden ist.
 */
    if (!readline(handle, to_check) || !*to_check)
    {
        Cconws("\r\n2. Zeile von AUTOCFAT.INF "
            "leer!\r\n");
        Fclose(handle);
        return(1);
    }
/*
 * Jetzt noch die Zeile einlesen, die entscheidet,
 * wann AutoCFat pr�ft oder nicht.
 */
    if (!readline(handle, hotkey))
    {
        Cconws("\r\n3. Zeile von AUTOCFAT.INF "
            "fehlt!\r\n");
        Fclose(handle);
        return(1);
    }
    Fclose(handle);
    only_cold = needed = possible = 0;
    if ((*hotkey != '+') && (*hotkey != '-'))
    {
        Cconws("\r\n3. Zeile von AUTOCFAT.INF "
            "fehlerhaft!\r\n");
        return(1);
    }
    hot = hotkey;
    skip_with_hotkey = (*hot++ == '+');
    for (; *hot; hot++)
    {
        switch (*hot)
        {
            case '+':
                only_cold = 1;
                break;
            case 'a':
                possible |= ALT;
                break;
            case 'A':
                needed |= ALT;
                break;
            case 'c':
                possible |= CTRL;
                break;
            case 'C':
                needed |= CTRL;
                break;
            case 'l':
                possible |= LSHIFT;
                break;
            case 'L':
                needed |= LSHIFT;
                break;
            case 'r':
                possible |= RSHIFT;
                break;
            case 'R':
                needed |= RSHIFT;
                break;
            case 'k':
                possible |= CLOCK;
                break;
            case 'K':
                needed |= CLOCK;
                break;
            default:
                hot[1] = 0;
                break;
        }
        if (only_cold)
        {
            hot++;
            break;
        }
    }
    if ((needed & possible) != 0)
    {
        Cconws("\r\n3. Zeile von AUTOCFAT.INF "
            "fehlerhaft!\r\n");
        return(1);
    }
    if ((needed | possible) == 0)
        possible = 31;
    if (!needed)
        needed = 32;
/*
 * Die INF-Datei wurde erfolgreich gelesen, jetzt
 * das Programm beenden, wenn keine Sondertaste
 * gedr�ckt ist. Ist skip_with_hotkey ungleich
 * Null (dritte Zeile der INF-Datei enth�lt '+'),
 * wird das Verhalten umgedreht, d.h. AutoCFat
 * wird verlassen, wenn eine Sondertaste gedr�ckt
 * ist. Dabei wird nat�rlich beachtet, welche
 * der Sondertasten zwingend n�tig sind und welche
 * auch gedr�ckt sein d�rfen.
 * Ist Pr�fung nur bei Kaltstart gew�nscht, wird
 * AutoCFat bei Warmstart ebenso beendet.
 */
    if ((((Kbshift(-1) & 31) & needed) == needed)
        || (((Kbshift(-1) & 31) & possible) != 0))
    {
        hotkey_ok = 1;
    }
    else
        hotkey_ok = 0;
    if ((!is_cold_boot() && only_cold) ||
        (skip_with_hotkey == hotkey_ok))
    {
        Cconws("\r\nKeine FATs gepr�ft!\r\n");
        return(0);
    }
/*
 * INF-Datei wurde erfolgreich gelesen. Jetzt das
 * aktuelle Laufwerk und den aktuellen Pfad
 * sichern und CheckFat f�r die gew�nschten
 * Laufwerke aufrufen.
 */
    actdrv = Dgetdrv();
    Dgetpath(actpath, actdrv + 1);
    for (check = to_check; *check; check++)
    {
        Cconws("\r\n");
        pos = 1;
        if ((char)(actdrv + 65) == (*check & ~32))
        {
            cline[pos++] = '-';
            cline[pos++] = 'u';
            cline[pos++] = ' ';
        }
        cline[pos] = *check;
        cline[0] = pos;
        cline[++pos] = 0;
        err = Pexec(0, cfpath, cline, 0L);
        if (err < 0L)
        {
            Cconws("Fehler beim Aufruf von "
                "CheckFat!\r\n");
            return(1);
        }
        result = (WORD)err;
/*
 * Liefert CheckFat 3 zur�ck (FAT ist fehlerhaft),
 * einen erneuten Lauf mit erweiterter Ausgabe
 * erm�glichen
 */
        if (result != 3)
            continue;
        Cconws("\r\nFAT ist nicht OK! ");
        Cconws("Mehr Details? [j/n] ");
        if (((WORD)Cconin() & ~32) != 'J')
            continue;
        Cconws("\r\nCluster anzeigen? [j/n] ");
        if (((WORD)Cconin() & ~32) != 'J')
        {
            cluster = 0;
            Cconws("\r\nNamen anzeigen? [j/n] ");
            names = (((WORD)Cconin() & ~32) == 
                'J');
        }
        Cconws("\r\nMehrfach belegte Cluster "
            "ausf�hrlich\r\nmelden? [j/n] ");
        if (((WORD)Cconin() & ~32) == 'J')
        {
            crosslinks = 1;
            Cconws("\r\nMit vollen Pfadnamen? "
                "[j/n] ");
            longnames = (((WORD)Cconin() & ~32) ==
                'J');
        }
        Cconws("\r\nAls defekt markierte Cluster "
            "\r\nmelden? [j/n] ");
        marked = (((WORD)Cconin() & ~32) == 'J');
        Cconws("\r\n\r\n");
        cline[1] = '-';
        cline[2] = 'v';
        pos = 3;
        if (cluster || names)
            cline[pos++] = cluster ? 'c' : 'n';
        if (crosslinks || longnames)
            cline[pos++] = longnames ? 'l' : 'x';
        if (marked)
            cline[pos++] = 'd';
        if ((char)(actdrv + 65) == (*check & ~32))
            cline[pos++] = 'u';
        cline[pos++] = ' ';
        cline[pos] = *check;
        cline[0] = pos;
        cline[++pos] = 0;
        Pexec(0, cfpath, cline, 0L);
        Cconws("\r\nTaste dr�cken!\r\n");
        Cnecin();
    }
/* Laufwerk und Pfad zur�cksetzen */
    Dsetdrv(actdrv);
    Dsetpath(actpath);
    return(0);
}

/*
 * readline
 *
 * Liest eine Zeile aus einer GEMDOS-Datei ein,
 * die wahlweise mit CRLF oder nur LF enden darf.
 * Beginnt sie mit einem '#', wird gleich die
 * n�chste Zeile eingelesen.
 *
 * Eingabe:
 * handle: Zu benutzendes GEMDOS-Handle
 * buffer: Zeiger auf Zeilenpuffer
 *
 * R�ckgabe:
 * 0: Fehler beim Lesen (oder: Zeile zu lang)
 * 1: Alles OK
 */
WORD readline(WORD handle, char *buffer)
{
    WORD    count;
    char    input;

    for (;;)
    {
        count = 0;
        for (;;)
        {
            if (Fread(handle, 1L, &input) != 1L)
                return(0);
            if (input == '\n')
                break;
            if (count == 255)
                return(0);
            buffer[count++] = input;
        }
        if (count)
        {
            if (buffer[count - 1] == '\r')
                count--;
        }
        buffer[count] = 0;
        if (*buffer != '#')
            break;
    }
    return(1);
}

/*
 * is_cold_boot
 *
 * Pr�ft, ob der Rechner gerade eingeschaltet
 * wurde.
 *
 * R�ckgabe:
 * 1: Rechner wurde "kaltgestartet"
 * 0: Rechner wurde schon mindestens einmal
 *    "warmgestartet"
 */
WORD is_cold_boot(void)
{
    ULONG   tubs,
            proc_lives;
    LONG    old_stack;
    WORD    cold;

/*
 * Zun�chst wird nach dem TUBS-Cookie gesucht. Ist
 * dieser vorhanden, kann das gew�nschte Ergebnis
 * hier ausgelesen werden (wenn Bit 0 gesetzt ist,
 * lag ein Warmstart vor).
 */
    if (get_cookie('TUBS', &tubs))
        return((WORD)((tubs & 1) ^ 1));
/*
 * Wenn der Cookie nicht vorhanden war, mu� eine
 * etwas unsichere Methode benutzt werden (die
 * TCKJ aus dem TUBS-Paket letztlich auch benutzt,
 * allerdings ist sie deswegen zuverl�ssiger, weil
 * das Programm einen Resethandler installiert,
 * was AutoCFat nat�rlich nicht machen soll):
 * Es wird gepr�ft, ob in proc_lives (0x380) ein
 * bestimmter Wert steht. Wenn ja, ist es ein
 * Warmstart, ansonsten wird von einem Kaltstart
 * ausgegangen und der magische Wert 'CFAT' nach
 * 0x380 geschrieben, um beim n�chsten Mal den
 * Warmstart zu erkennen.
 */
    if (Super((void *)1L) == 0L)
        old_stack = Super(0L);
    else
        old_stack = 0L;
    proc_lives = *(ULONG *)0x380;
/*
 * Es wird mit 'CFAT', 'TCKJ' und 0x1234578UL
 * verglichen, da prinzipiell keiner dieser Werte
 * bei einem Kaltstart vorzufinden sein d�rfte
 * (0x1234578UL wird bei einer Bomben-Exception
 * nach proc_lives geschrieben).
 */
    if ((proc_lives == 'CFAT') || (proc_lives ==
        'TCKJ') || (proc_lives == 0x12345678UL))
    {
        cold = 0;
    }
    else
    {
        cold = 1;
        *(ULONG *)0x380 = 'CFAT';
    }
    if (old_stack)
        Super((void *)old_stack);
    return(cold);
}

/*
 * get_cookie
 *
 * Pr�ft, ob ein bestimmter Cookie vorhanden ist
 * und liefert, wenn gew�nscht, dessen Wert.
 *
 * Eingabe:
 * cookie: Zu suchender Cookie (z.B. 'MiNT')
 * value: Zeiger auf einen vorzeichenlosen Long,
 *        in den der Wert des Cookies geschrieben
 *        werden soll. Ist dies nicht gew�nscht/
 *        erforderlich, einen Nullzeiger �ber-
 *        geben.
 *
 * R�ckgabe:
 * 0: Cookie nicht vorhanden, value unbeeinflu�t
 * 1: Cookie vorhanden, Wert steht in value (wenn
 *    value kein Nullpointer ist)
 */
WORD get_cookie(ULONG cookie, ULONG *value)
{
    LONG    *jar,
            old_stack;
    
    /*
     * Den Zeiger auf den Cookie-Jar ermitteln,
     * dabei ggf. in den Supervisor-Modus
     * wechseln.
     */
    if (Super((void *)1L) == 0L)
    {
        old_stack = Super(0L);
        jar = *((LONG **)0x5a0L);
        Super((void *)old_stack);
    }
    else
        jar = *(LONG **)0x5a0;
    
    /*
     * Ist die "Keksdose" leer, gleich Null zu-
     * r�ckliefern, da ja gar kein Cookie
     * vorhanden ist.
     */
    if (jar == 0L)
        return(0);
    
    /*
     * Sonst den Cookie-Jar bis zum Ende durch-
     * suchen und im Erfolgsfall 1 zur�ckliefern.
     * Falls value kein Nullpointer war, vorher
     * den Wert des Cookies dort eintragen.
     */
    while (jar[0])
    {
        if (jar[0] == cookie)
        {
            if (value != 0L)
                *value = jar[1];
            
            return(1);
        }
        
        jar += 2;
    }
    /*
     * Bis zum Ende gesucht und nichts gefunden,
     * also 0 zur�ckgeben.
     */
    return(0);
}

/* EOF */
