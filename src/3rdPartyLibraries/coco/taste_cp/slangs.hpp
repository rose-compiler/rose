//****************************************************************
//   CPLUS2\SCAN_H.FRM
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

#ifndef prettyScanner_INCLUDE
#define prettyScanner_INCLUDE

#include "cr_scan.hpp"

#if __MSDOS__ || MSDOS
#       define SCANNER  CRDosScanner
#else
#       define SCANNER  CRScanner
#endif

class slangScanner : public SCANNER
{
public:
  slangScanner(int* SrcFile, int ignoreCase) { 
    inputIteratorStart=SrcFile;
    inputIteratorEnd=SrcFile+5;
  };
  virtual void Reset() {}
protected:
  static int STATE0[];
  int CheckLiteral(int id);
  int LeftContext(char *s);
  int Comment();
  int Get();
private:
  int* inputIteratorStart; // to be used as Iterator
  int* inputIteratorEnd;
};

#endif /* prettyScanner_INCLUDE */


