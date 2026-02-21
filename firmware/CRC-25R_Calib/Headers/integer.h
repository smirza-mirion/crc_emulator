/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _INTEGER
#define _INTEGER

//#ifdef _WIN32	/* FatFs development platform */

//#include <windows.h>
//#include <tchar.h>

//#else			/* Embedded platform */

/* These types must be 16-bit, 32-bit or larger integer */
typedef int				INT;
typedef unsigned int	UINT;

/* These types must be 8-bit integer */
typedef char			CHAR;
typedef unsigned char	UCHAR;
#ifndef _BYTE_
#define _BYTE_
typedef unsigned char	BYTE;
#endif

/* These types must be 16-bit integer */
typedef short			SHORT;
typedef unsigned short	USHORT;
#ifndef _WORD_
#define _WORD_
typedef unsigned short	WORD;
#endif

typedef unsigned short	WCHAR;

/* These types must be 32-bit integer */
typedef long			LONG;
typedef unsigned long	ULONG;

#ifndef _DWORD_
#define _DWORD_
typedef unsigned long	DWORD;
#endif

//#endif

#endif
