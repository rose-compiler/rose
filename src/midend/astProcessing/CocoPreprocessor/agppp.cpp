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

#include "agppc.hpp"

using namespace std;

#include "crt.h"

#include "agppp.hpp"
#include "agpps.hpp"

#include <stdlib.h>
#include <iostream>
#include <list>
#include <string>

int global_len;
int global_pos;

list<string> terminalsList;

#define semActionIndent "      "
#define MAXSEMLEN 2000

bool isTerminal(string s) {
  for(list<string>::iterator i=terminalsList.begin();
      i!=terminalsList.end();
      i++) {
    if(s==*i) return true;
  }
  return false;
}

string
generateTerminalInit(string terminalName) {
  string s;
  s="(. "+terminalName+"* astNode = dynamic_cast<class "+terminalName+"*>(dynamic_cast<slangScanner*>(Scanner)->getCurrentAstToken().node); .)\n";
  return s;
}

string
generateLock(string restructureObjectName) {
  string s;
  s="(. "+restructureObjectName+"->lock(astNode); .)\n";
  return s;
}

string
generateUnlock(string restructureObjectName) {
  string s;
  s="(. "+restructureObjectName+"->unlock(astNode) .)\n";
  return s;
}

/*
static void FixString(char *name)
{
  int i, j, len, spaces, start;
  len = strlen(name);
  if (len == 2) { SemError(129); return; }
  if (ignore_case) upcase(name);
  spaces = FALSE; start = name[0];
  for (i = 1; i <= len-2; i++) {
    if (name[i] > 0 && name[i] <= ' ') spaces = TRUE;
    if (name[i] == '\\') {
      if (name[i+1] == '\\' || name[i+1] == '\'' || name[i+1] == '\"') {
        for (j = i; j < len; j++) name[j] = name[j+1]; len--;
      }
    }
  }
  if (spaces) SemError(124);
}
*/
/**************************************************************************/



#include "agppp.hpp"
#include "agpps.hpp"

unsigned short int agppParser::SymSet[][MAXSYM] = {
  /*EOF_Sym PRODUCTIONSSym */
  {0x41,0x0},
  
  {0x0}
};

void agppParser::Get()
{ do {
    Sym = Scanner->Get();
    Scanner->NextSym.SetSym(Sym);
    if (Sym <= MAXT) Error->ErrorDist ++;
    else {
      if (Sym == OptionsSym) { /*23*/
      	char s[100];
      	  LookAheadString(s, sizeof(s)-1);
      	  cout << s << "\n";
      } else
      /* Empty Stmt */ ;
      Scanner->NextSym = Scanner->CurrSym;
    }
  } while (Sym > MAXT);
}

void agppParser::ExpectWeak(int n, int follow)
{ if (Sym == n) Get();
  else {
    GenError(n);
    while (!(In(SymSet[follow], Sym) || In(SymSet[0], Sym))) Get();
  }
}

int agppParser::WeakSeparator(int n, int syFol, int repFol)
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

void agppParser::AGPP()
{
	Name name1, compiler_name;
	long P; char semstring[MAXSEMLEN];
	int Len;
	Expect(COMPILERSym);
	P = LexPos()/*+8*/;
	Ident(compiler_name);
	global_pos  = LookAheadPos();
	
	while (Sym >= identSym && Sym <= COMPILERSym ||
	       Sym >= ENDSym && Sym <= RbrackSym ||
	       Sym >= LessSym && Sym <= No_Sym) {
		Get();
	}
	global_len  =
	  (int) (LookAheadPos()-global_pos);
	Len = (int) (LookAheadPos() - P); GetString(P,semstring,Len); cout << semstring << "\n\n";
	while (Sym == TOKENSSym) {
		Declaration();
	}
	while (!(Sym == EOF_Sym ||
	         Sym == PRODUCTIONSSym)) { GenError(24); Get(); }
	Expect(PRODUCTIONSSym);
	cout << "\nPRODUCTIONS\n";
	while (Sym == identSym) {
		Rule();
	}
	Expect(ENDSym);
	Ident(name1);
	Expect(PointSym);
	cout << "END " << name1 << " .\n";
}

void agppParser::Ident(char *s)
{
	Expect(identSym);
	LexString(s, MAX_ID_LEN-1);
}

void agppParser::Declaration()
{
	Expect(TOKENSSym);
	cout << "TOKENS\n";
	while (Sym == identSym) {
		TokenDecl();
	}
}

void agppParser::Rule()
{
	Name name1; int attr, sem,exp,n1;
	Ident(name1);
	cout << name1 << " ";
	if (Sym == LessSym ||
	    Sym == LessPointSym) {
		Attribs(&attr);
	}
	Expect(EqualSym);
	cout << "= ";
	Rhs();
	while (Sym == BarSym) {
		Get();
		cout << "| ";
		Rhs();
	}
	Expect(PointSym);
	cout << ".\n";
}

void agppParser::Attribs(int *n)
{
	long P; char semstring[MAXSEMLEN];
	int Len, Line, Col;
	if (Sym == LessSym) {
		Get();
		P = LexPos();
		while (Sym >= identSym && Sym <= LessSym ||
		       Sym >= LessPointSym && Sym <= No_Sym) {
			if (Sym >= identSym && Sym <= stringSym ||
			    Sym >= numberSym && Sym <= LessSym ||
			    Sym >= LessPointSym && Sym <= No_Sym) {
				Get();
			} else if (Sym == badstringSym) {
				Get();

			} else GenError(25);
		}
		Expect(GreaterSym);
		Len = (int) (LexPos() - P);
		GetString(P,semstring,Len); cout << semstring << "> ";
	} else if (Sym == LessPointSym) {
		Get();
		P = LexPos();
		while (Sym >= identSym && Sym <= LessPointSym ||
		       Sym >= LparenPointSym && Sym <= No_Sym) {
			if (Sym >= identSym && Sym <= stringSym ||
			    Sym >= numberSym && Sym <= LessPointSym ||
			    Sym >= LparenPointSym && Sym <= No_Sym) {
				Get();
			} else if (Sym == badstringSym) {
				Get();

			} else GenError(26);
		}
		Expect(PointGreaterSym);
		Len = (int) (LexPos() - P);
		GetString(P,semstring,Len); cout << semstring << ".> ";
	} else GenError(27);
}

void agppParser::Rhs()
{
	Name name1; int attr; int n1;
	if (Sym == LparenPointSym) {
		SemTextList(&n1);
	}
	Ident(name1);
	cout << name1 << " ";
	if(isTerminal(name1)) {
	string t=generateTerminalInit(name1);
	string l=generateLock("subst");
	cout << "(. { .)"+t+l << endl;
	};
	if (Sym == LessSym ||
	    Sym == LessPointSym) {
		Attribs(&attr);
	}
	if (Sym == LparenPointSym) {
		SemTextList(&n1);
	}
	if (Sym == stringSym) {
		Get();
		cout << "\"(\" ";
		if (Sym == identSym ||
		    Sym == LbraceSym ||
		    Sym == LbrackSym ||
		    Sym == LparenPointSym) {
			if (Sym == LbraceSym) {
				Get();
				cout << "{ ";
				if (Sym == LparenPointSym) {
					SemTextList(&n1);
				}
				NonTerminalExp();
				Expect(RbraceSym);
				cout << "}";
			} else if (Sym == identSym ||
			           Sym == LbrackSym ||
			           Sym == LparenPointSym) {
				NonTerminalExpList();
				while (Sym == BarSym) {
					Get();
					cout << "\n| ";
					NonTerminalExpList();
				}
			} else GenError(28);
		}
		Expect(stringSym);
		cout << "\")\"";
		if (Sym == LparenPointSym) {
			SemTextList(&n1);
		}
	}
	if(isTerminal(name1)) { 
	  string ul=generateUnlock("subst");
	  cout << ul+"(. } .)"; 
	} ;
	cout << "\n";
}

void agppParser::SemTextList(int *n)
{
	SemText(n);
	while (Sym == LparenPointSym) {
		SemText(n);
	}
}

void agppParser::NonTerminalExp()
{
	Name name; int n1;
	if (Sym == LbrackSym) {
		Get();
		cout << "[ ";
		if (Sym == LparenPointSym) {
			SemTextList(&n1);
		}
		NonTerminal();
		Expect(RbrackSym);
		cout << "]";
		if (Sym == LparenPointSym) {
			SemTextList(&n1);
		}
	} else if (Sym == identSym) {
		NonTerminal();
	} else GenError(29);
}

void agppParser::NonTerminalExpList()
{
	int n1;
	if (Sym == LparenPointSym) {
		SemTextList(&n1);
	}
	NonTerminalExp();
	while (Sym == identSym ||
	       Sym == LbrackSym) {
		NonTerminalExp();
	}
}

void agppParser::NonTerminal()
{
	int attr,n1; Name name1;
	Ident(name1);
	cout << name1 << " ";
	if (Sym == LessSym ||
	    Sym == LessPointSym) {
		Attribs(&attr);
	}
	if (Sym == LparenPointSym) {
		SemTextList(&n1);
	}
}

void agppParser::TokenDecl()
{
	char name[MAX_STR_LEN];
	Ident(name);
	cout << name << "\n"; terminalsList.push_back(name);
}

void agppParser::SemText(int *n)
{
	long P; char semstring[MAXSEMLEN];
	int Len, Line, Col;
	Expect(LparenPointSym);
	
	P = LexPos()/*+2*/;
	while (Sym >= identSym && Sym <= LparenPointSym ||
	       Sym == No_Sym) {
		if (Sym >= identSym && Sym <= stringSym ||
		    Sym >= numberSym && Sym <= PointGreaterSym ||
		    Sym == No_Sym) {
			Get();
		} else if (Sym == badstringSym) {
			Get();
			SemError(102);
		} else if (Sym == LparenPointSym) {
			Get();
			SemError(109);
		} else GenError(30);
	}
	Expect(PointRparenSym);
	Len = (int) (LexPos() - P);
	GetString(P,semstring,Len); cout << semstring << ".)\n";
}



void agppParser::Parse()
{ Scanner->Reset(); Get();
  AGPP();
}


