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

#include "slangc.hpp"

#include "prettypr.hpp"
#include "slangs.hpp"

/*--------------------------------------------------------------------------*/



#include "slangp.hpp"
#include "slangs.hpp"

unsigned short int slangParser::SymSet[][MAXSYM] = {
  /*EOF_Sym */
  {0x1},
  
  {0x0}
};

void slangParser::Get()
{ do {
    Sym = Scanner->Get();
    Scanner->NextSym.SetSym(Sym);
    if (Sym <= MAXT) Error->ErrorDist ++;
    else {
      /* Empty Stmt */ ;
      Scanner->NextSym = Scanner->CurrSym;
    }
  } while (Sym > MAXT);
}

void slangParser::ExpectWeak(int n, int follow)
{ if (Sym == n) Get();
  else {
    GenError(n);
    while (!(In(SymSet[follow], Sym) || In(SymSet[0], Sym))) Get();
  }
}

int slangParser::WeakSeparator(int n, int syFol, int repFol)
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

void slangParser::Slang()
{
	Expression();
}

void slangParser::Expression()
{
	SimExpr();
}

void slangParser::SimExpr()
{
	Term();
	while (Sym >= PlusSym && Sym <= MinusSym) {
		AddOp();
		Term();
	}
}

void slangParser::Term()
{
	Factor();
	while (Sym >= StarSym && Sym <= SlashSym) {
		MulOp();
		Factor();
	}
}

void slangParser::AddOp()
{
	if (Sym == PlusSym) {
		Get();
		Append(" + ");
	} else if (Sym == MinusSym) {
		Get();
		Append(" - ");
	} else GenError(7);
}

void slangParser::Factor()
{
	if (Sym == EOF_Sym ||
	    Sym >= StarSym && Sym <= MinusSym) {
		/* Empty */
	} else if (Sym == numberSym) {
		Get();
		char Num[20];
		LexString(Num, 19);
		Append(Num);
		Factor();
	} else GenError(8);
}

void slangParser::MulOp()
{
	if (Sym == StarSym) {
		Get();
		Append(" * ");
	} else if (Sym == SlashSym) {
		Get();
		Append(" / ");
	} else GenError(9);
}



void slangParser::Parse()
{ Scanner->Reset(); Get();
  Slang();
}


