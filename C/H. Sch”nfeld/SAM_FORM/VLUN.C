/* VLUN Minimal-Rahmenprogramm               */
/* Modulname:  VLUN.C                        */
/* (c) MAXON Computer 1994                   */
/* Autoren:    H. Sch�nfeld, B. Spellenberg  */

# include "vlun.h"

/* Struktur mit Zeigern auf s�mtliche zur
   Wandlung eines Formats n�tigen Routinen   */
typedef struct 
{
   HEAD_FUNC      *TstHead;
   READ_FUNC      *ToStd;
   WRITE_FUNC     *FromStd;
   WRT_HEAD_FUNC  *WrtHead;
   FINISH_FUNC    *Finish;
   char           *FmtName;
} FMT_FUNCTION;

/* Liste mit den Wandelfunktionen f�r alle
   unterst�tzen Formate                      */
FMT_FUNCTION CnvFuncs[] =
{
   { AvrTstHead, AllToStd, AvrFromStd,
     AvrWrtHead, AvrFinish, "AVR-Format" },
   { SndTstHead, AllToStd, SndFromStd,
     SndWrtHead, SndFinish, "SND-Format" },
   { DvsTstHead, DvsToStd, DvsFromStd,
     DvsWrtHead, 0L, "DVSM-Format" },
   { WavTstHead, WavToStd, WavFromStd,
     WavWrtHead, WavFinish, "WAVE-Format" },
   { RawSetHead, AllToStd, RawFromStd,
     0L, 0L, "Raw-Format" },
   { 0L, 0L, 0L, 0L, 0L, 0L}
};

   
SAMPLE_SPEC InSmp;   /* Input-File Format    */
OUT_FORMAT  OutSmp;  /* Export-File Format   */

/* Minimal Beispiel Rahmenprogramm ohne
   Benutzerinterface zur Demonstration der
   Anwendung der Wandel-Routinen             */
int main(void)
{
/* Komplette Pfade f�r Input und Export File
   mit Extension                             */
   char  InFileName[128],
         OutFileName[128];
/* Std. C Filehandles f�r Input/Export Files */
   FILE  *InFileH,
         *OutFileH;
/* Resultat des Header-Tests f�r Format Fmt  */
   int   TstRes,
/* Indexz�hler f�r Liste der Wandelfunktionen*/
         Fmt;
/* Gr��e des Wandelbuffers in Byte           */
   long  StdBufLen,
/* Gr��e der gelesenen Daten im Std. Format  */
         Read;
/* Zeiger auf Wandelbuffer                   */
   char  *StdBuf;
   
/* Festlegen des Input/Export Filenamens und
   des Exportformats.
   In eigenen Progs. z.B. Eingabe per Dialog
   oder Kommandozeile.                       */

   strcpy(InFileName,"in.avr");
   strcpy(OutFileName,"out.wav");
   OutSmp.Typ=WAVHEADER;
   OutSmp.Format=STEREO|SIGNED|BIT16;

/* Input-File �ffnen                         */ 
   InFileH=fopen(InFileName,"rb");
   if(!InFileH)
   {
      puts("Kann Inputfile nicht �ffnen");
      exit(1);
   }

/* Export-File �ffnen                        */
   OutFileH=fopen(OutFileName,"wb");
   if(!OutFileH)
   {
      puts("Kann Outputfile nicht �ffnen");
      fclose(InFileH);
      exit(1);
   }

/* F�r alle Formate in der Wandelliste den
   Header-Test aufrufen bis Format erkannt
   wird                                      */
   for(Fmt=0;CnvFuncs[Fmt].TstHead;Fmt++)
   {
/* Vorm Header Test zum Fileanfang gehen     */
      fseek(InFileH,0L,0);
/* Testfunktion des aktuellen Format aufrufen*/
      TstRes=(*CnvFuncs[Fmt].TstHead)
               (InFileH,&InSmp);

/* Abbruch da Format erkannt, aber in dieser
   Form nicht unterst�tzt                    */                   
      if(NOT_SUPPORTED==TstRes)
      {
         printf ("Unbekanntes %s\n",
                  *CnvFuncs[Fmt].FmtName);
         continue;
      }

/* Weiter zum n�chsten Format-Test           */
      if(UNKNOWN==TstRes)
         continue;

/* Ansonsten Format erkannt...               */

/* L�nge eines Blocks im Standardformat
   berechnen                                 */
      StdBufLen=InSmp.BufLen*InSmp.SizeFac;

/* Daf�r n�tigen Buffer allozieren           */
      if(!(StdBuf=malloc(StdBufLen)))
      {
         fclose(OutFileH);
         fclose(InFileH);
         puts("Nicht gen�gend Speicher");
         exit(2);
      }

/* Export-File-Format Header schreiben falls
   vorhanden                                 */
      if(CnvFuncs[OutSmp.Typ].WrtHead)
         (*CnvFuncs[OutSmp.Typ].WrtHead)
            (OutFileH,&InSmp,&OutSmp);

/* Solange n�tig Input-File blockweise laden
   und ins Standard-Format wandeln. Dann Block
   ins Export-Format wandeln und schreiben   */
      while(Read=(*CnvFuncs[Fmt].ToStd)
                  (InFileH,&InSmp,
                  StdBuf,StdBufLen))
      {
         (*CnvFuncs[OutSmp.Typ].FromStd)
            (OutFileH,&OutSmp,Read,StdBuf);
      }

/* Zusatzinformationen in Export-File schreiben
   wenn im Format vorgesehen                 */
      if(CnvFuncs[OutSmp.Typ].Finish)
         (*CnvFuncs[OutSmp.Typ].Finish)
            (OutFileH,&OutSmp);

/* Wandeln beendet                           */
      break;
   }

   free(StdBuf);
   fclose(InFileH);
   fclose(OutFileH);
   return(0);
}
