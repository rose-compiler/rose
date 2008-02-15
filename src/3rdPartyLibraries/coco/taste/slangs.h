/**********************************************************
**   SCAN_H.FRM
**   Coco/R C Support Frames.
**   Author: Frankie Arzu <farzu@uvg.edu.gt>
**
**   Jun 12, 1996   Version 1.06
**      Many fixes and suggestions thanks to
**      Pat Terry <p.terry@ru.ac.za>
**********************************************************/

#ifndef SCAN_HEADER_INCLUDE
#define SCAN_HEADER_INCLUDE

#include "slangc.h"

#define  TAB_CHAR  9
#define  LF_CHAR   10
#define  CR_CHAR   13
#define  EOF_CHAR  0

#define  TAB_SIZE  8

extern int  S_src;         /* source file */
extern int  S_Line, S_Col; /* line and column of current symbol */
extern int  S_Len;         /* length of current symbol */
extern long S_Pos;         /* file position of current symbol */
extern int  S_NextLine;    /* line of lookahead symbol */
extern int  S_NextCol;     /* column of lookahead symbol */
extern int  S_NextLen;     /* length of lookahead symbol */
extern long S_NextPos;     /* file position of lookahead symbol */
extern int  S_CurrLine;    /* current input line (may be higher than line) */
extern long S_lineStart;   /* start position of current line */

int S_Get(void);
/* Gets next symbol from source file */

void S_Reset(void);
/* Reads and stores source file internally */
/* Assert: S_src has been opened */

void S_GetString(long pos, int len, char *s, int max);
/* Retrieves exact string of max length len at position pos in source file */

void S_GetName(long pos, int len, char *s, int max);
/* Retrieves an string of max length len at position pos in source file.  Each
   character in the string will be capitalized if IGNORE CASE is specified */

unsigned char S_CurrentCh(long pos);
/* Returns current character at specified file position */

#endif /* SCAN_HEADER_INCLUDE */


