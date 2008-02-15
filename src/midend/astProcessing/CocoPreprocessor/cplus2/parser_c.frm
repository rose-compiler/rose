#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//****************************************************************
//   CPLUS2\PARSER_C.FRM
//   Coco/R C++ Support Frames.
//   Author: Frankie Arzu <farzu@uvg.edu.gt>
//
//   Jun 12, 1996  Version 1.06
//      Many fixes and suggestions thanks to
//      Pat Terry <p.terry@.ru.ac.za>
//   Oct 11, 1997  Version 1.07
//      Eliminated redundant ";" in some places
//****************************************************************

#include -->TokensHeader

-->UserDeclarations

#include -->ParserHeader
#include -->ScanHeader

unsigned short int -->ParserClass::SymSet[][MAXSYM] = {
  -->SymSets
  {0x0}
};

void -->ParserClass::Get()
{ do {
    Sym = Scanner->Get();
    Scanner->NextSym.SetSym(Sym);
    if (Sym <= MAXT) Error->ErrorDist ++;
    else {
      -->Pragmas
      Scanner->NextSym = Scanner->CurrSym;
    }
  } while (Sym > MAXT);
}

void -->ParserClass::ExpectWeak(int n, int follow)
{ if (Sym == n) Get();
  else {
    GenError(n);
    while (!(In(SymSet[follow], Sym) || In(SymSet[0], Sym))) Get();
  }
}

int -->ParserClass::WeakSeparator(int n, int syFol, int repFol)
{ unsigned short int s[MAXSYM];
  int i;

  if (Sym == n) { Get(); return 1; }
  if (In(SymSet[repFol], Sym)) return 0;

  for (i = 0; i < MAXSYM; i++)
    s[i] = SymSet[0][i] | SymSet[syFol][i] | SymSet[repFol][i];
  GenError(n);
  while (!In(s, Sym)) Get();
  return In(SymSet[syFol], Sym);
}

-->Productions

void -->ParserClass::Parse()
{ Scanner->Reset(); Get();
  -->ParseRoot
}


