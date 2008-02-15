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

#include "slangs.hpp"
#include "slangc.hpp"

#include <iostream>

#define Scan_Ch        Ch
#define Scan_NextCh    NextCh
#define Scan_ComEols   ComEols
#define Scan_CurrLine  CurrLine
#define Scan_CurrCol   CurrCol
#define Scan_LineStart LineStart
#define Scan_BuffPos   BuffPos
#define Scan_NextLen   NextSym.Len

int slangScanner::STATE0[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0,0,0,8,10,0,7,4,9,2,2,2,2,2,2,2,2,2,2,5,3,
                  12,11,13,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int slangScanner::CheckLiteral(int id)
{ char c;
  c =  CurrentCh(NextSym.Pos);
  return id;
}

int slangScanner::Comment()
{ int Level, StartLine, OldCol;
  long OldLineStart;

  Level = 1; StartLine = CurrLine;
  OldLineStart = LineStart; OldCol = CurrCol;
  if (Scan_Ch == '(') { /* 1 */
  	Scan_NextCh();
  	if (Scan_Ch == '*') { /* 2 */
  		Scan_NextCh();
  		while (1) {
  			if (Scan_Ch== '*') { /* 5 */
  				Scan_NextCh();
  				if (Scan_Ch == ')') { /* 6 */
  					Level--; Scan_NextCh(); Scan_ComEols = Scan_CurrLine - StartLine;
  					if(Level == 0) return 1;
  				} /* 6 */ 
  			} else  /* 5 */
  			if (Scan_Ch == '(') {
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


static int counter=0;

int slangScanner::Get() { 
  cout << "GET:" << endl;
  counter++;
  if(inputIteratorStart<inputIteratorEnd) {
    int* inputIteratorCopy=inputIteratorStart;
    cout << "counter:" << counter-1 << ", testdata:" << *inputIteratorStart << ": ptr to: " << inputIteratorStart << endl;
    inputIteratorStart++;
    return *inputIteratorCopy;
  }
  else
    return EOF_Sym;
}


