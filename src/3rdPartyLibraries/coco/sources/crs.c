/**********************************************************
**   SCAN_C.FRM
**   Coco/R C Support Frames.
**   Author: Frankie Arzu <farzu@uvg.edu.gt>
**
**   Jun 12, 1996   Version 1.06
**      Many fixes and suggestions thanks to
**      Pat Terry <p.terry@ru.ac.za>
**   Mar 13, 1998   Version 1.08
**      Support for MS VC++ 5.0
**   Oct 31, 1999   Version 1.14
**      LeftContext Support
**   Mar 24, 2000   Version 1.15
**      LeftContext no longer needed
**********************************************************/

#include "crs.h"
#include <stdio.h>
#include <stdlib.h>
#if __MSDOS__ || MSDOS
#  include <io.h>
#  define  SEGMENTED_BUFF   1  /* Segmented Scanner Buffer */
#else
#  if WIN32 || __WIN32__
#    include <io.h>
#  else
#    include <unistd.h>
#  endif
#  define  SEGMENTED_BUFF   0  /* Linear Scanner Buffer */
#endif

#ifdef NEXTSTEP   /* NextStep */
#  include <sys/file.h>
#  include <sys/types.h>
#  include <sys/uio.h>
#  define L_SET     0  /* set the seek pointer */
#  define L_INCR    1  /* increment the seek pointer */
#  define L_XTND    2  /* extend the file size */
#endif

#include <string.h>

static int STATE0[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                       0,0,0,0,18,0,17,0,0,19,27,28,0,24,0,25,23,0,16,16,16,16,16,16,16,16,16,16,0,0,
                       34,22,35,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                       1,30,0,31,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                       1,1,1,32,29,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int  S_src;                 /* source file */
int  S_Line, S_Col;         /* line and column of current symbol */
int  S_Len;                 /* length of current symbol */
long S_Pos;                 /* file position of current symbol */
int  S_NextLine;            /* line of lookahead symbol */
int  S_NextCol;             /* column of lookahead symbol */
int  S_NextLen;             /* length of lookahead symbol */
long S_NextPos;             /* file position of lookahead symbol */
static unsigned char S_ch;  /* current input character */
int  S_CurrLine;            /* current input line (may be higher than line) */
long S_LineStart;           /* start position of current line */


#define Scan_Ch         S_ch
#define Scan_NextCh     S_NextCh
#define Scan_ComEols    S_ComEols
#define Scan_CurrLine   S_CurrLine
#define Scan_LineStart  S_LineStart
#define Scan_BuffPos    S_BuffPos
#define Scan_CurrCol    S_CurrCol
#define Scan_NextLen    S_NextLen

#if SEGMENTED_BUFF          /* Segmented Buffer for small (MS-DOS) systems */
#  define  BLKSIZE  16384
   static  unsigned char  *Buffer[32]; /* source buffer for low-level access */
#  define CURRENT_CH(Pos) (Buffer[(int) (Pos / BLKSIZE)])[(int) (Pos % BLKSIZE)]
#else
   static unsigned char *Buffer;
#  define CURRENT_CH(Pos) Buffer[Pos]
#endif

static int  S_ComEols;   /* number of EOLs in a comment */
static long S_BuffPos;   /* current position in buf */
static int  S_CurrCol;   /* current Column Number */
static long S_InputLen;  /* source file size */

unsigned char S_CurrentCh(long pos);

#define IGNORE_CASE  0
#define UPCASE(c)    ((c >= 'a' && c <= 'z') ? c-32 : c)

unsigned char S_CurrentCh(long pos)
{ return (unsigned char) CURRENT_CH(pos); }

#if IGNORE_CASE
#define S_NextCh() { S_BuffPos++;\
		     S_ch = CURRENT_CH(S_BuffPos);\
		     S_ch = UPCASE(S_ch);\
		     if (S_ch == TAB_CHAR) S_CurrCol += TAB_SIZE - (S_CurrCol % TAB_SIZE); else \
		     if (S_ch == LF_CHAR) {\
		       S_CurrLine++; S_CurrCol = 0;\
		       S_LineStart = S_BuffPos + 1;}\
		     S_CurrCol++;\
		   }
#else
#define S_NextCh() { S_BuffPos++;\
		     S_ch = CURRENT_CH(S_BuffPos);\
		     if (S_ch == TAB_CHAR) S_CurrCol += TAB_SIZE - (S_CurrCol % TAB_SIZE); else \
		     if (S_ch == LF_CHAR) {\
		       S_CurrLine++; S_CurrCol = 0;\
		       S_LineStart = S_BuffPos + 1;}\
		     S_CurrCol++;\
		   }
#endif

static int EqualStr (char *s)
{ long q; char c;
  if (S_NextLen != (int) strlen(s)) return 0;
  q = S_NextPos;
  while (*s) {
    c = CURRENT_CH(q); q++;
#if IGNORE_CASE
    c = UPCASE(c);
#endif
    if (c != *s++) return 0;
  }
  return 1;
}

static int CheckLiteral(int id)
{ char c;
  c = CURRENT_CH(S_NextPos);
#if IGNORE_CASE
  c = UPCASE(c);
#endif
  switch (c) {
  	case 'A':
  		if (EqualStr("ANY")) return ANYSym;
  		break;
  	case 'C':
  		if (EqualStr("COMPILER")) return COMPILERSym;
  		if (EqualStr("CHARACTERS")) return CHARACTERSSym;
  		if (EqualStr("COMMENTS")) return COMMENTSSym;
  		if (EqualStr("CASE")) return CASESym;
  		if (EqualStr("CHR")) return CHRSym;
  		if (EqualStr("CONTEXT")) return CONTEXTSym;
  		break;
  	case 'E':
  		if (EqualStr("END")) return ENDSym;
  		break;
  	case 'F':
  		if (EqualStr("FROM")) return FROMSym;
  		break;
  	case 'I':
  		if (EqualStr("IGNORE")) return IGNORESym;
  		break;
  	case 'N':
  		if (EqualStr("NAMES")) return NAMESSym;
  		if (EqualStr("NESTED")) return NESTEDSym;
  		break;
  	case 'P':
  		if (EqualStr("PRODUCTIONS")) return PRODUCTIONSSym;
  		if (EqualStr("PRAGMAS")) return PRAGMASSym;
  		break;
  	case 'S':
  		if (EqualStr("SYNC")) return SYNCSym;
  		break;
  	case 'T':
  		if (EqualStr("TOKENS")) return TOKENSSym;
  		if (EqualStr("TO")) return TOSym;
  		break;
  	case 'W':
  		if (EqualStr("WEAK")) return WEAKSym;
  		break;
  
  }
  return id;
}

static int Comment(void)
{ int Level, StartLine, OldCol;
  long OldLineStart;

  Level = 1; StartLine = S_CurrLine;
  OldLineStart = S_LineStart; OldCol = S_CurrCol;
  if (Scan_Ch == '/') { /* 1 */
  	Scan_NextCh();
  	if (Scan_Ch == '*') { /* 2 */
  		Scan_NextCh();
  		while (1) {
  			if (Scan_Ch== '*') { /* 5 */
  				Scan_NextCh();
  				if (Scan_Ch == '/') { /* 6 */
  					Level--; Scan_NextCh(); Scan_ComEols = Scan_CurrLine - StartLine;
  					if(Level == 0) return 1;
  				} /* 6 */ 
  			} else  /* 5 */
  			if (Scan_Ch == '/') {
  				Scan_NextCh();
  				if (Scan_Ch == '*') { Level++; Scan_NextCh(); }
  			} else /* 5 */
  			if (Scan_Ch == EOF_CHAR) return 0;
  			else Scan_NextCh();
  		} /* while */
  	} else { /* 2 */
  		if (Scan_Ch == LF_CHAR) { Scan_CurrLine--; Scan_LineStart = OldLineStart; }
  		Scan_BuffPos -= 2; Scan_CurrCol = OldCol - 1; Scan_NextCh();
  	} /* 2 */
  } /* 1*/
  
  return 0;
}

int S_Get(void)
{ int state, ctx;
  start:
  while (Scan_Ch >= 9 && Scan_Ch <= 10 ||
         Scan_Ch == 13 ||
         Scan_Ch == ' ') Scan_NextCh();
  if ((Scan_Ch == '/') && Comment()) goto start;

  S_Pos  = S_NextPos;   S_NextPos  = S_BuffPos;
  S_Col  = S_NextCol;   S_NextCol  = S_CurrCol - 1;
  S_Line = S_NextLine;  S_NextLine = S_CurrLine;
  S_Len  = S_NextLen;   S_NextLen  = 0; ctx = 0;

  if (S_ch == EOF_CHAR) return EOF_Sym;
  state = STATE0[S_ch];
  while(1) {
    S_NextCh(); S_NextLen++;
    switch (state) {
     /* State 0; valid STATE0 Table
    case 0:
    	if (Scan_Ch >= 'A' && Scan_Ch <= 'Z' ||
        Scan_Ch == '_' ||
        Scan_Ch >= 'a' && Scan_Ch <= 'z') state = 1; else
    	if (Scan_Ch == '"') state = 18; else
    	if (Scan_Ch == 39) state = 19; else
    	if (Scan_Ch >= '0' && Scan_Ch <= '9') state = 16; else
    	if (Scan_Ch == '$') state = 17; else
    	if (Scan_Ch == '=') state = 22; else
    	if (Scan_Ch == '.') state = 23; else
    	if (Scan_Ch == '+') state = 24; else
    	if (Scan_Ch == '-') state = 25; else
    	if (Scan_Ch == '(') state = 27; else
    	if (Scan_Ch == ')') state = 28; else
    	if (Scan_Ch == '|') state = 29; else
    	if (Scan_Ch == '[') state = 30; else
    	if (Scan_Ch == ']') state = 31; else
    	if (Scan_Ch == '{') state = 32; else
    	if (Scan_Ch == '}') state = 33; else
    	if (Scan_Ch == '<') state = 34; else
    	if (Scan_Ch == '>') state = 35; else
    	return No_Sym;
    	break;
     --------- End State0 --------- */
    case 1:
    	if (Scan_Ch >= '0' && Scan_Ch <= '9' ||
    	    Scan_Ch >= 'A' && Scan_Ch <= 'Z' ||
    	    Scan_Ch == '_' ||
    	    Scan_Ch >= 'a' && Scan_Ch <= 'z') /*same state*/; else
    	return CheckLiteral(identSym);
    	break;
    case 6:
    	return stringSym;
    case 7:
    	return stringSym;
    case 12:
    	return badstringSym;
    case 13:
    	return badstringSym;
    case 14:
    	return badstringSym;
    case 15:
    	return badstringSym;
    case 16:
    	if (Scan_Ch >= '0' && Scan_Ch <= '9') /*same state*/; else
    	return numberSym;
    	break;
    case 17:
    	if (Scan_Ch >= 'A' && Scan_Ch <= 'Z' ||
    	    Scan_Ch == '_' ||
    	    Scan_Ch >= 'a' && Scan_Ch <= 'z') /*same state*/; else
    	return OptionsSym;
    	break;
    case 18:
    	if (Scan_Ch == '"') state = 6; else
    	if (Scan_Ch >= ' ' && Scan_Ch <= '!' ||
    	    Scan_Ch >= '#' && Scan_Ch <= '[' ||
    	    Scan_Ch >= ']' && Scan_Ch <= 255) /*same state*/; else
    	if (Scan_Ch == 92) state = 20; else
    	if (Scan_Ch == 13) state = 12; else
    	if (Scan_Ch == 10) state = 13; else
    	return No_Sym;
    	break;
    case 19:
    	if (Scan_Ch == 39) state = 7; else
    	if (Scan_Ch >= ' ' && Scan_Ch <= '&' ||
    	    Scan_Ch >= '(' && Scan_Ch <= '[' ||
    	    Scan_Ch >= ']' && Scan_Ch <= 255) /*same state*/; else
    	if (Scan_Ch == 92) state = 21; else
    	if (Scan_Ch == 13) state = 14; else
    	if (Scan_Ch == 10) state = 15; else
    	return No_Sym;
    	break;
    case 20:
    	if (Scan_Ch >= ' ' && Scan_Ch <= 255) state = 18; else
    	return No_Sym;
    	break;
    case 21:
    	if (Scan_Ch >= ' ' && Scan_Ch <= 255) state = 19; else
    	return No_Sym;
    	break;
    case 22:
    	return EqualSym;
    case 23:
    	if (Scan_Ch == '.') state = 26; else
    	if (Scan_Ch == '>') state = 37; else
    	if (Scan_Ch == ')') state = 39; else
    	return PointSym;
    	break;
    case 24:
    	return PlusSym;
    case 25:
    	return MinusSym;
    case 26:
    	return Range;
    case 27:
    	if (Scan_Ch == '.') state = 38; else
    	return LparenSym;
    	break;
    case 28:
    	return RparenSym;
    case 29:
    	return BarSym;
    case 30:
    	return LbrackSym;
    case 31:
    	return RbrackSym;
    case 32:
    	return LbraceSym;
    case 33:
    	return RbraceSym;
    case 34:
    	if (Scan_Ch == '.') state = 36; else
    	return LessSym;
    	break;
    case 35:
    	return GreaterSym;
    case 36:
    	return LessPointSym;
    case 37:
    	return PointGreaterSym;
    case 38:
    	return LparenPointSym;
    case 39:
    	return PointRparenSym;
    
    default: return No_Sym; /* S_NextCh already done */
    }
  }
}

void S_GetString (long pos, int len, char *s, int max)
{ while (len-- && max--) { *s++ = CURRENT_CH(pos); pos++; }
  *s = '\0';
}

void S_GetName (long pos, int len, char *s, int max)
{ while (len-- && max--) {
    *s = CURRENT_CH(pos);
#if IGNORE_CASE
    *s = UPCASE(*s);
#endif
    s++; pos++;
  }
  *s = '\0';
}

void S_Reset(void)
/* assert: S_src has been opened */
{ long len;
  int  i = 0, n;

  S_InputLen = len = lseek(S_src, 0L, SEEK_END);
  (void) lseek(S_src, 0L, SEEK_SET);

#if SEGMENTED_BUFF
  while (len > BLKSIZE) {
    Buffer[i] = (unsigned char *) malloc(BLKSIZE);
    n = read(S_src, (void *) Buffer[i], (unsigned) BLKSIZE);
    len -= n;
    i++;
  }
  Buffer[i] = (unsigned char *) malloc((unsigned) len + 1);
  n = read(S_src, (void *) Buffer[i], (unsigned) len);
  (Buffer[i])[n] = EOF_CHAR;
#else 
  Buffer = (unsigned char *) malloc(len+1);
  n = read(S_src, (void *) Buffer, len);
  Buffer[n] = EOF_CHAR;
#endif

  S_CurrLine = 1; S_LineStart = 0; S_BuffPos = -1; S_CurrCol = 0;
  S_NextPos  = 0; S_NextCol = 0; S_NextLine = 0; S_NextLen = 0;
  S_ComEols = 0;
  S_NextCh();
}


