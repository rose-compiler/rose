#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//****************************************************************
//   CPLUS2\SCAN_C.FRM
//   Coco/R C++ Support Frames.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//   Oct 31, 1999  Version 1.14
//      LeftContext Support
//   Mar 24, 2000  Version 1.15
//      LeftContext Support no longer needed
//****************************************************************

#include "agppc.hpp"
#include "agpps.hpp"

#define Scan_Ch        Ch
#define Scan_NextCh    NextCh
#define Scan_ComEols   ComEols
#define Scan_CurrLine  CurrLine
#define Scan_CurrCol   CurrCol
#define Scan_LineStart LineStart
#define Scan_BuffPos   BuffPos
#define Scan_NextLen   NextSym.Len

int agppScanner::STATE0[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,18,0,17,0,0,19,33,0,0,0,0,0,22,0,16,16,16,16,16,16,16,16,16,16,0,0,
                  29,23,30,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,27,0,28,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,25,24,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int agppScanner::CheckLiteral(int id)
{ char c;
  c =  CurrentCh(NextSym.Pos);
  if (IgnoreCase) c = Upcase(c);
  switch (c) {
  	case 'C':
  		if (EqualStr("COMPILER")) return COMPILERSym;
  		break;
  	case 'E':
  		if (EqualStr("END")) return ENDSym;
  		break;
  	case 'P':
  		if (EqualStr("PRODUCTIONS")) return PRODUCTIONSSym;
  		break;
  	case 'T':
  		if (EqualStr("TOKENS")) return TOKENSSym;
  		break;
  
  }
  return id;
}

int agppScanner::Comment()
{ int Level, StartLine, OldCol;
  long OldLineStart;

  Level = 1; StartLine = CurrLine;
  OldLineStart = LineStart; OldCol = CurrCol;
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

int agppScanner::Get()
{ int state, ctx;

  start:
    while (Scan_Ch >= 9 && Scan_Ch <= 10 ||
           Scan_Ch == 13 ||
           Scan_Ch == ' ') Scan_NextCh();
    if ((Scan_Ch == '/') && Comment()) goto start;

    CurrSym = NextSym;
    NextSym.Init(0, CurrLine, CurrCol - 1, BuffPos, 0);
    NextSym.Len  = 0; ctx = 0;

    if (Ch == EOF_CHAR) return EOF_Sym;
    state = STATE0[Ch];
    while(1) {
      Scan_NextCh(); NextSym.Len++;
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
      	if (Scan_Ch == '.') state = 22; else
      	if (Scan_Ch == '=') state = 23; else
      	if (Scan_Ch == '|') state = 24; else
      	if (Scan_Ch == '{') state = 25; else
      	if (Scan_Ch == '}') state = 26; else
      	if (Scan_Ch == '[') state = 27; else
      	if (Scan_Ch == ']') state = 28; else
      	if (Scan_Ch == '<') state = 29; else
      	if (Scan_Ch == '>') state = 30; else
      	if (Scan_Ch == '(') state = 33; else
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
      	if (Scan_Ch == '>') state = 32; else
      	if (Scan_Ch == ')') state = 35; else
      	return PointSym;
      	break;
      case 23:
      	return EqualSym;
      case 24:
      	return BarSym;
      case 25:
      	return LbraceSym;
      case 26:
      	return RbraceSym;
      case 27:
      	return LbrackSym;
      case 28:
      	return RbrackSym;
      case 29:
      	if (Scan_Ch == '.') state = 31; else
      	return LessSym;
      	break;
      case 30:
      	return GreaterSym;
      case 31:
      	return LessPointSym;
      case 32:
      	return PointGreaterSym;
      case 33:
      	if (Scan_Ch == '.') state = 34; else
      	return No_Sym;
      	break;
      case 34:
      	return LparenPointSym;
      case 35:
      	return PointRparenSym;
      
      default: return No_Sym; /* Scan_NextCh already done */
      }
    }
}


