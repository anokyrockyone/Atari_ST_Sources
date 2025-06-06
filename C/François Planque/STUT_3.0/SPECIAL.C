/*
 * special.c
 *
 * Purpose:
 * --------
 * Fonctions sp�ciales d'int�r�t g�n�ral
 * c'est-�-dire qui auraient du figurer dans les librairies 
 * standard...
 *
 * History:
 * --------
 * 1993: fplanque: Created
 */

 
 	#include "!OPTIONS.H"				/* Options de compilation */         
	#define	THIS_FILE	"SPECIAL.C v2.00 - 03.95"
          

/*
 * System headers:
 */
	#include	<stdio.h>			/* Pour PUTCHAR */
	#include <aes.h>
   #include	<string.h>

/*
 * Custom headers:
 */
	#include	"SPEC_PU.H"	
	#include "S_MALLOC.H"

/*
 * ------------------------ PROTOTYPES -------------------------
 */



/*
 * ------------------------ VARIABLES -------------------------
 */
    
    
/*
 * Public variables: 
 */
	char	*	G_empty_string = "";			/* Chaine vide */
	char	*	G_crlf = "\r\n";				/* Retour � la ligne */
	void	*	G_null = NULL;

	char	*	G_tmp_buffer;					/* Peut contenir un texte long de 511 signes + '\0' */
	char	*	G_2nd_buffer;					/* 2�me buffer */


/*
 * ------------------------ FUNCTIONS -------------------------
 */

/*
 * ctrl_IntRange(-)
 *
 * Purpose:
 * --------
 * inits
 *
 * History:
 * --------
 * 14.01.95: fplanque: Created
 */
void	Special_Init( void )
{
	
	/*
	 * Tampon interm�diaire: 
	 */
	G_tmp_buffer = (char *) MALLOC( TMP_BUFFER_SIZE+1 );
	G_2nd_buffer = (char *) MALLOC( SECOND_BUFFER_SIZE+1 );

}



/*
 * ctrl_IntRange(-)
 *
 * Purpose:
 * --------
 * Contr�le qu'un INT est bien compris dans l'intervale d�sir�
 *
 * History:
 * --------
 * 12.03.94: fplanque: Created
 */
int	ctrl_IntRange(				/* Out: Valeur ds intervale */
			int	n_Value,			/* In:  Valeur d'origine */
			int	n_Minimum,		/* In:  Valeur Minimale autoris�e */
			int	n_Maximum )		/* In:  Valeur Maximale autoris�e */
{
	if( n_Value < n_Minimum )
	{
		return	n_Minimum;
	}
	else if( n_Value > n_Maximum )
	{
		return	n_Maximum;
	}

	return	n_Value;
}


/*
 * wait(-)
 *
 * Purpose:
 * --------
 * attend un certain nbre de 100�mes de seconde
 *
 * History:
 * --------
 * 30.09.94: fplanque: Created
 */
void	wait(
			clock_t	delay )	/* In: d�lai d'attente en 100�mes de seconde */
{
	clock_t	ct_end = clock() + delay * CLK_TCK / 100;

	while( clock() < ct_end );
}


/*
 * min(-)
 *
 * Purpose:
 * --------
 * Minimum de deux INT
 *
 * History:
 * --------
 * fplanque: Created
 */
int	min(			/* Out: Minimum des deux INT */
			int a,	/* In:  INT #1 */
			int b)	/* In:  INT #2 */
{
	return( a<b?a:b );
}


/*
 * l_min(-)
 *
 * Purpose:
 * --------
 * Minimum de deux LONG INT
 *
 * History:
 * --------
 * fplanque: Created
 */
long	l_min(		/* Out: Minimum des deux LONG INT */
			long a,	/* In:  LONG INT #1 */
			long b)	/* In:  LONG INT #2 */
{
	return( a<b?a:b );
}

/*
 * max(-)
 *
 * Purpose:
 * --------
 * Maximum de deux INT
 *
 * History:
 * --------
 * fplanque: Created
 */
int	max(			/* Out: Maximum des deux INT */
			int a,	/* In:  INT #1 */
			int b)	/* In:  INT #2 */
{
	return( a<b?b:a );
}


/*
 * l_max(-)
 *
 * Purpose:
 * --------
 * Maximum de deux LONG INT
 *
 * History:
 * --------
 * fplanque: Created
 */
long	l_max(		/* Out: Maximum des deux LONG INT */
			long a,	/* In:  LONG INT #1 */
			long b)	/* In:  LONG INT #2 */
{
	return( a<b?b:a );
}




/*
 * rcintersect(-)
 *
 * Purpose:
 * --------
 * Calcule le rectangle resultant de l'intersection de deux
 * rectangles
 *
 * Algorythm:
 * ----------  
 * Calcule bas-droite intersection
 * Calcule haut-gauche intersection
 * Calcule largeur/hauteur intersection
 * V�rifie qu'il y a eu intersection
 *
 * Notes:
 * ------
 * Toutes coordonn�es transmises par variables s�par�es
 * Une autre fonction plus efficace effectue une transmission par GRECT
 *
 * History:
 * --------
 *     1993: fplanque: Created             
 * 29.04.94: fplanque: Modifi� v�rification intersection
 */
int	rcintersect( 		/* Out: !=0 s'il y a eu intersection */
			int area_x,    /* In: Premier rectangle */
			int area_y, 
			int area_w, 
			int area_h,
			int *box_x, 	/* In/Out: Deuxi�me rectangle/Rectangle intersection */
			int *box_y, 
			int *box_w, 
			int *box_h )
{
	int	tw, th;
	
	/*
	 * Calcule coordonn�es inf�rieures droites de l'intersection
	 */
	tw= min( *box_x+*box_w, area_x+area_w );
	th= min( *box_y+*box_h, area_y+area_h );

	/*
	 * Calcule coordonn�es sup�rieurs gauches de l'intersection
	 */
	*box_x= max( *box_x, area_x );
	*box_y= max( *box_y, area_y );
	
	/*
	 * Calcule dimensions de l'intersection
	 */
	*box_w= tw - *box_x;
	*box_h= th - *box_y;
	
	/*
	 * D�termine s'il y a intersection
	 */
	return ( (*box_w > 0) && (*box_h > 0) );
}


/*
 * grcintersect(-)
 *
 * Purpose:
 * --------
 * Calcule le rectangle resultant de l'intersection de deux
 * rectangles
 *
 * Algorythm:
 * ----------  
 * Calcule bas-droite intersection
 * Calcule haut-gauche intersection
 * Calcule largeur/hauteur intersection
 * V�rifie qu'il y a eu intersection
 *
 * History:
 * --------
 *     1993: fplanque: Created             
 * 29.04.94: fplanque: Modifi� v�rification intersection
 */
int	grcintersect(		/* Out: !=0 s'il y a eu intersection */
			GRECT *area,   /* In: Premier rectangle */
			GRECT *box )   /* In/Out: Deuxi�me rectangle/Rectangle intersection */
{
	int	tw, th;
	
	/*
	 * Calcule coordonn�es inf�rieures droites de l'intersection
	 */
	tw= min( box->g_x+box->g_w, area->g_x+area->g_w );
	th= min( box->g_y+box->g_h, area->g_y+area->g_h );

	/*
	 * Calcule coordonn�es sup�rieurs gauches de l'intersection
	 */
	box->g_x= max( box->g_x, area->g_x );
	box->g_y= max( box->g_y, area->g_y );
	/*
	 * Calcule dimensions de l'intersection
	 */
	box->g_w= tw - box->g_x;
	box->g_h= th - box->g_y;
	
	/*
	 * D�termine s'il y a intersection
	 */
	return ( (box->g_w > 0) && (box->g_h > 0) );
}
                                  
                                  
/*
 * grecttoarray(-)
 *
 * Purpose:
 * --------
 * Remplit un (morceau de) tableau d'int
 * avec les champs d'un GRECT
 *
 * History:
 * --------
 * fplanque: Created
 */
void	grecttoarray( 
			GRECT *rect,  	/* In: GRECT � transcrire */
			int *array )	/* Buffer: Tableau dans lequel il faut �crire */
{
	array[0] = rect -> g_x;
	array[1] = rect -> g_y;
	array[2] = rect -> g_x + rect -> g_w -1;
	array[3] = rect -> g_y + rect -> g_h -1;
	
}


/*
 * ping(-)
 *
 * Purpose:
 * --------
 * Fait ping/beeep/tut selon l'ordinateur
 *
 * Algorythm:
 * ----------  
 * Imprime un caract�re de code ASCII 7 sur la console
 *
 * Notes:
 * ------
 * Utilis� pour signaler les erreurs i-e les situations impr�vues
 * dans le code. Ne devrait pratiquement jamais se produire en temps
 * normal. Pour les bips normaux, on devrait utilis� quelque chose
 * de plus sophistiqu�!
 *
 * History:
 * --------
 * fplanque: Created
 */
void	ping ( void )
{
	putchar('\a');
}






/*
 * Sstrcmp(-)
 *
 * Compare deux chaines mais s'arr�te sur un caract�re sp�cial:
 *
 * 12.01.95: fplanque: created
 */
int	Sstrcmp(
			const	char * cpsz_1,
			const	char * cpsz_2,
			char			 c_spec )	/* In: Caract�re de fin */
{
	int	difference;
	
	for( ; ; )
	{
		if( *cpsz_1 == c_spec || *cpsz_1 == '\0' )
		{	
			if( *cpsz_2 == c_spec || *cpsz_2 == '\0' )
			{	/* 1 = 2 */
				return	0;
			}
			
			/* 1 < 2 */
			return	-1;
		}
			
		
		if( *cpsz_2 == c_spec || *cpsz_2 == '\0' )
		{	/* 1 > 2 */
			return 1;
		}

		if( (difference = *(cpsz_1++) - *(cpsz_2++)) !=0 )
		{
			return	difference;
		}
	}
}







/*
 * strBchr(-)
 *
 * Purpose:
 * --------
 * String BACK-char
 * Cherche un caract�re depuis la fin de la chaine/
 *
 * Algorythm:
 * ----------  
 * En fait il parcourt la chaine � l'endroit mais jusqu'au bout!
 * Car pour commencer par la fin, il faudrait d�j� chercher la fin!
 *
 * Notes:
 * ------
 *
 * History:
 * --------
 * 1993: fplanque: Created
 */
char	*strBchr(
			char *string, 	/* In: Cha�ne dans laquelle il faut chercher */
			char chr ) 		/* In: Caract�re � rechercher */
{
	char	*found_pos = NULL;	/* Pour l'instant, on n'a rien trouv�! */

	do
	{
		if ( *string == chr )
		{ /*
			* Si on a trouv� une it�ration du CHR recherch�: 
			*/
			found_pos = string;				/* On la note */
		}
		
	} while( *(string++) != '\0' );		/* Tant qu'on a pas atteint le '\0' final */ 

	return	found_pos;	/* Retourne pos de la derni�re it�ration trouv�e */
}


/*
 * replace_extension(-)
 *
 * Purpose:
 * --------
 * Remplace l'extension d'un nom de fichier par celle voulue
 *
 * Notes:
 * ------
 * Pas d'alloc m�moire: le nom sur lequel on op�re doit comporter la place n�cessaire � l'op�ration
 *
 * History:
 * --------
 * 12.11.94: fplanque: Created
 */
void	replace_extension(
			char * 			pBsz_FileName,		/* In: Buffer contenant Nom sur lequel on op�re */
			const char *	cpsz_Extension )	/* In: Extension � utiliser */
{			
	char * piBsz_Point = strBchr( pBsz_FileName, '.' );

	if( piBsz_Point == NULL )
	{	/*
		 * Pas trouv�: ahoute extension:
		 */
		strcat( pBsz_FileName, cpsz_Extension );
	}
	else
	{	/*
		 * Trouv�: remplace par nlle extension:
		 */
		strcpy( piBsz_Point, cpsz_Extension ); 
	}

}

/* 
 * ----------------------- Gestion de chaines ------------------------
 */


/*
 * dup_String(-)
 *
 * Purpose:
 * --------
 * Duplique une chaine de caract�res
 *
 * Algorythm:
 * ----------  
 * Tient compte du cas ou le pointeur est 
 * NULL ou encore egal � G_empty_string
 *
 * Notes:
 * ------
 *
 *
 * History:
 * --------
 * 21.09.94: fplanque: Created
 * 14.11.94: parametre est maintenant const
 * 04.12.94: vorrection bug: && -> ||
 */
char * dup_String( 							/* Out: Copie */
				const char * psz_Src )		/* In: Source */
{
	if( psz_Src == NULL || psz_Src == G_empty_string )
	{
		/* printf( "\nduplication string vide" ); */
		return	(char*) psz_Src;		/* unCONST !! */
	}
	else
	{
		/* printf( "\nduplication de [%s]", psz_Src ); */
		return	STRDUP( psz_Src );
	}

}


/*
 * free_String(-)
 *
 * Purpose:
 * --------
 * Efface une chaine dont on fournit un pointeur
 *
 * Algorythm:
 * ----------  
 * Tient compte du cas ou le pointeur est 
 * NULL ou encore egal � G_empty_string
 *
 * Notes:
 * ------
 *
 *
 * History:
 * --------
 * 24.05.94: fplanque: Created
 */
void	free_String(
			char	* pString )	/* In: ptr sur chaine � effacer */
{
	if( pString != NULL && pString != G_empty_string )
	{
		FREE( pString );
	}
}


/*
 * len_String(-)
 *
 * Purpose:
 * --------
 * Donne lg d'une chaine dont on fournit un pointeur
 *
 * Algorythm:
 * ----------  
 * Tient compte du cas ou le pointeur est NULL
 *
 * History:
 * --------
 * 24.03.95: fplanque: Created
 */
size_t	len_String(
				const char	* pString )	/* In: ptr sur chaine � effacer */
{
	if( pString != NULL )
	{
		return strlen( pString );
	}
	
	return 0;
}


/*
 * copy_String(-)
 *
 * Purpose:
 * --------
 * Copie une chaine dont on fournit un pointeur
 *
 * Algorythm:
 * ----------  
 * Tient compte du cas ou le pointeur est NULL
 *
 * History:
 * --------
 * 29.03.95: fplanque: Created
 */
void	copy_String(
				char			* pString_Dest,	/* In: ptr sur buffer destination */
				const char	* cpString_Srce )	/* In: ptr sur chaine � effacer */
{
	if( cpString_Srce != NULL )
	{
		strcpy( pString_Dest, cpString_Srce );
	}
	else
	{
		pString_Dest[0]='\0';	
	}
}


/*
 * freeOld_dupString(-)
 *
 * Purpose:
 * --------
 * Duplique une chaine et sauve le ptr � un emplacement
 * que l'on prend soin de vider au pr�alable
 * il s'agit d'un REMPLACEMENT
 *
 * Algorythm:
 * ----------  
 * Tient compte du cas ou le pointeur est 
 * NULL ou encore egal � G_empty_string
 *
 *
 * History:
 * --------
 * 14.11.94: fplanque: Created
 */
void	freeOld_dupString(
			char		* * ppsz_Dest,		/* In/Out: Adr du Ptr destination */
			const char * psz_Srce )		/* In: Chaine � dupliquer */
{
	free_String( * ppsz_Dest );
	* ppsz_Dest = dup_String( psz_Srce );
}


/*
 * realloc_String(-)
 *
 * Reallocation d'une chaine avec une nouvelle taille
 * Gestion du cas ou la source est NULL
 *
 * 10.07.95: fplanque: Created
 */
char * realloc_String(				/* Out: Nouvelle chaine */
			const char * pMsz_src,	/* In:  Chaine source */
			size_t		 size_new )	/* In:  Nouvelle longueur */
{
	if( pMsz_src == NULL )
	{
		return	(char*)MALLOC( size_new );
	}
	else
	{
		return	(char*)REALLOC( pMsz_src, size_new ); 
	}
}