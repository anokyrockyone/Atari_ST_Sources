/*
	Tabulatorweite: 3
	Kommentare ab: Spalte 60											*Spalte 60*
*/

#ifndef __2B_UNIVERSAL_TYPES__										/* schon vorhanden? */
#define __2B_UNIVERSAL_TYPES__

/*----------------------------------------------------------------------------------------*/
/* Systemumgebung																									*/
/*----------------------------------------------------------------------------------------*/
#define	LITTLE_ENDIAN	0												/* 0: Motorola-Format, 1: Intel-Format */
#define	INTEL	0															/* Intel-CPU */
#define	USE_INLINE	0													/* ggf. Inline-Assembler benutzen */

#define	IS_MACOS		0													/* MacOS */
#define	IS_WINOS		0													/* Windows */
#define	IS_ATARI		1													/* MagiC */

#if ( defined( FALSE ))
	#if ( FALSE != 0 )
		#error	FALSE bereits anderslautend definiert
	#endif
#else
	#define	FALSE	0
#endif

#if ( defined( TRUE ))
	#if ( TRUE != 1 )
		#error	TRUE bereits anderslautend definiert
	#endif
#else
	#define	TRUE	1
#endif

/*----------------------------------------------------------------------------------------*/
/* Standarddatentypen f�r 8, 16, 32 und 64 Bit															*/
/*----------------------------------------------------------------------------------------*/
typedef signed char		int8;											/* 8 Bit vorzeichenbehaftet */
typedef unsigned char	uint8;										/* 8 Bit vorzeichenlos */

typedef signed short		int16;										/* 16 Bit vorzeichenbehaftet */
typedef unsigned short	uint16;										/* 16 Bit vorzeichenlos */

typedef signed long		int32;										/* 32 Bit vorzeichenbehaftet */
typedef unsigned long	uint32;										/* 32 Bit vorzeichenlos */

typedef struct
{
	int32		hi;
	uint32	lo;
} int64;																		/* 64 Bit vorzeichenbehaftet */

typedef struct
{
	uint32	hi;
	uint32	lo;
} uint64;																	/* 64 Bit vorzeichenlos */

/*----------------------------------------------------------------------------------------*/
/* Diverse Abk�mmlinge																							*/
/*----------------------------------------------------------------------------------------*/
typedef uint8	boolean;													/* aus Kompatibilit�tsgr�nden - besser int16 oder int32 verwenden */
typedef int32	fixed;													/* 16.16 vorzeichenbehaftet */
typedef int64	fixed64;													/* 32.32 vorzeichenbehaftet */

/*----------------------------------------------------------------------------------------*/
/* Wandelmakros f�r Little-Endian (Intel)																	*/
/*----------------------------------------------------------------------------------------*/
#define	REVERSE16( a ) 	(int16) (((uint16)(a) >> 8 ) | ((uint16)(a) << 8 ))
#define	REVERSEu16( a )	(((uint16)(a) >> 8 ) | ((uint16)(a) << 8 ))

#define	REVERSE32( a )		(int32) (((uint32)(a) << 24 ) | (((uint32)(a) & 0xff00L ) << 8 ) | \
											  (((uint32)(a) >> 8 ) & 0xff00L ) | ((uint32)(a) >> 24 ))

#define	REVERSEu32( a )	(((uint32)(a) << 24 ) | (((uint32)(a) & 0xff00L ) << 8 ) | \
									(((uint32)(a) >> 8 ) & 0xff00L ) | ((uint32)(a) >> 24 ))

#if INTEL																	/* Intel-CPU? */
#if USE_INLINE																/* Inline-Assembler benutzen? */
#undef	REVERSE16
#undef	REVERSEu16
#undef	REVERSE32
#undef	REVERSEu32

#if 0
	Die untenstehende Inline-Funktionen in Intel-Assembler benutzen die folgenden
	MSVC Eigenschaften:
	
	__declspec( naked ):	Die Funktion enth�lt nur die angegebenen Befehle - keinen Header.
	__fastcall:				Die Variable wird in ecx und nicht auf dem Stack �bergeben.
	
	Integer-Werte werden in eax bzw. ax zur�ckgeliefert.
#endif

static int16 __fastcall REVERSE16( int16 value );
static uint16 __fastcall REVERSEu16( uint16 value );
static int32 __fastcall REVERSE32( int32 value );
static uint32 __fastcall REVERSEu32( uint32 value );

__declspec( naked ) int16 __fastcall REVERSE16( int16 value )
{
	__asm	ror	cx,8
	__asm	mov	ax,cx
	__asm	ret
}
__declspec( naked ) uint16 __fastcall REVERSEu16( uint16 value )
{
	__asm	ror	cx,8
	__asm	mov	ax,cx
	__asm	ret
}

__declspec( naked ) int32 __fastcall REVERSE32( int32 value )
{
	__asm	bswap	ecx
	__asm	mov	eax,ecx
	__asm	ret
}
__declspec( naked ) uint32 __fastcall REVERSEu32( uint32 value )
{
	__asm	bswap	ecx
	__asm	mov	eax,ecx
	__asm	ret
}

#endif
#endif

/*----------------------------------------------------------------------------------------*/
/* Pr�ffunktion																									*/
/*----------------------------------------------------------------------------------------*/
#if ENABLE_2BTYPES_CHECK

extern boolean	are_2Btypes_ok( void );

/*----------------------------------------------------------------------------------------*/
/* Gr��e der Standarddatentypen �berpr�fen																*/
/* Funktionsresultat:	FALSE: Gr��enfehler TRUE: alles in Ordnung								*/
/*----------------------------------------------------------------------------------------*/
boolean	are_2Btypes_ok( void )
{
	if ( sizeof( int8 ) != 1 )
		return( FALSE );

	if ( sizeof( uint8 ) != 1 )
		return( FALSE );
	
	if ( sizeof( int16 ) != 2 )
		return( FALSE );

	if ( sizeof( uint16 ) != 2 )
		return( FALSE );

	if ( sizeof( int32 ) != 4 )
		return( FALSE );

	if ( sizeof( uint32 ) != 4 )
		return( FALSE );

	if ( sizeof( int64 ) != 8 )
		return( FALSE );

	if ( sizeof( uint64 ) != 8 )
		return( FALSE );

	return( TRUE );														/* alle Datentypen haben die korrekte L�nge */
}

#endif
#endif																		/* __2B_UNIVERSAL_TYPES__ */
