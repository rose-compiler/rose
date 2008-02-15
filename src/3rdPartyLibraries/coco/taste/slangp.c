/**********************************************************
**   PARSER_C.FRM
**   Coco/R C Support Frames.
**   Author: Frankie Arzu <farzu@uvg.edu.gt>
**
**   Jun 12, 1996   Version 1.06
**      Many fixes and suggestions thanks to
**      Pat Terry <p.terry@ru.ac.za>
**********************************************************/

#include "slangs.h"
#include "slangp.h"

#include <stdio.h>
#include "prettypr.h"
#include "slangs.h"

void WriteIdent()
{
    char IdentName[20];
    LexString(IdentName, sizeof(IdentName)-1);
    Append(IdentName);
}

void WriteNumber()
{
    char IdentName[20];
    LexString(IdentName, sizeof(IdentName)-1);
    Append(IdentName);
}

/*--------------------------------------------------------------------------*/



Error_Func Custom_Error = 0L;

static int Sym;
static int errors = 0;                /*number of detected errors*/
static int ErrDist = MinErrDist;

#define MAXSYM		1


/* Production prototypes */

static void Slang(void);
static void Expression(void);
static void SimExpr(void);
static void Term(void);
static void AddOp(void);
static void Factor(void);
static void MulOp(void);


#define NSETBITS        16

static unsigned short int SymSet[][MAXSYM] = {
  /*EOF_Sym */
  {0x1},
  {0x0}
};

void GenError(int errno)
{ if (ErrDist >= MinErrDist) {
    if (Custom_Error != 0L)
      (*Custom_Error) (errno, S_NextLine, S_NextCol, S_NextPos);
    errors++;
  }
  ErrDist = 0;
}

void SynError(int errno)
{ if (errno <= MAXERROR) errno = MAXERROR;
  if (ErrDist >= MinErrDist) {
    if (Custom_Error != 0L)
      (*Custom_Error) (errno, S_NextLine, S_NextCol, S_NextPos);
    errors++;
  }
  ErrDist = 0;
}

void SemError(int errno)
{ if (errno <= MAXERROR) errno = MAXERROR;
  if (ErrDist >= MinErrDist) {
    if (Custom_Error != 0L)
      (*Custom_Error) (errno, S_Line, S_Col, S_Pos);
    errors++;
  }
  ErrDist = 0;
}

static void Get(void)
{ do {
    Sym = S_Get();
    if (Sym <= MAXT) ErrDist ++;
    else {
      /* Empty Stmt */ ;
      S_NextPos  = S_Pos;  S_NextCol = S_Col;
      S_NextLine = S_Line; S_NextLen = S_Len;
    }
  } while (Sym > MAXT);
}

static int In (unsigned short int *SymbolSet, int i)
{ return SymbolSet[i / NSETBITS] & (1 << (i % NSETBITS)); }

static void Expect (int n)
{ if (Sym == n) Get(); else GenError(n); }

static void ExpectWeak (int n, int follow)
{ if (Sym == n) Get();
  else {
    GenError(n);
    while (!(In(SymSet[follow], Sym) || In(SymSet[0], Sym))) Get();
  }
}

static int WeakSeparator (int n, int syFol, int repFol)
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

int Successful(void)
{ return errors == 0; }

/* Productions */

static void Slang(void)
{
	Expression();
}

static void Expression(void)
{
	SimExpr();
}

static void SimExpr(void)
{
	Term();
	while (Sym >= PlusSym && Sym <= MinusSym) {
		AddOp();
		Term();
	}
}

static void Term(void)
{
	Factor();
	while (Sym >= StarSym && Sym <= SlashSym) {
		MulOp();
		Factor();
	}
}

static void AddOp(void)
{
	if (Sym == PlusSym) {
		Get();
		Append(" + ");
	} else if (Sym == MinusSym) {
		Get();
		Append(" - ");
	} else GenError(7);
}

static void Factor(void)
{
	if (Sym == EOF_Sym ||
	    Sym >= StarSym && Sym <= MinusSym) {
		/* Empty */
	} else if (Sym == numberSym) {
		Get();
		WriteNumber();
		Factor();
	} else GenError(8);
}

static void MulOp(void)
{
	if (Sym == StarSym) {
		Get();
		Append(" * ");
	} else if (Sym == SlashSym) {
		Get();
		Append(" / ");
	} else GenError(9);
}



void Parse(void)
{ S_Reset(); Get();
  Slang();
}


