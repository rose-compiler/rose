/**********************************************************
**   PARSER_C.FRM
**   Coco/R C Support Frames.
**   Author: Frankie Arzu <farzu@uvg.edu.gt>
**
**   Jun 12, 1996   Version 1.06
**      Many fixes and suggestions thanks to
**      Pat Terry <p.terry@ru.ac.za>
**********************************************************/

#include "crs.h"
#include "crp.h"

#include "crt.h"
#include "crf.h"
#include "cra.h"
#include "crp.h"
#include "crs.h"
#include <string.h>
#include <stdlib.h>

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

static void MatchLiteral (int sp)
/* store string either as token or as literal */
{
  PTermNode sn, sn1;
  int matched_sp;

  sn = GetTermP(sp);
  matched_sp = MatchDFA((unsigned char *) sn->name, sp);
  if (matched_sp != 0) {
    sn1 = GetTermP(matched_sp);
    sn1->type = T_CLASSLITTOKEN;
    sn->type  = T_LITTOKEN;
  } else sn->type= T_CLASSTOKEN;
}

static void SetCtx (int gp)
/* set transition code to contextTrans */
{
  PGraphNode gn;
  while (gp > 0) {
    gn = GetGraphP(gp);
    if (gn->type == T_CHAR || gn->type == T_CLASS)
      gn->CONTEXT = T_CONTEXT;
    else
      if (gn->type == T_OPT || gn->type == T_REP)
      SetCtx(gn->INNER);
      else
      if (gn->type == T_ALT) {
      SetCtx(gn->INNER); SetCtx(gn->ALT);
      }
    gp = gn->next;
  }
}

static void StringClass(char *s, Set *items)
{
  s[strlen(s)-1]=0;  s++;  /* Ignore First and Last character */
  while (*s) Set_AddItem(items, *s++);
}

/**************************************************************************/



Error_Func Custom_Error = 0L;

static int Sym;
static int errors = 0;                /*number of detected errors*/
static int ErrDist = MinErrDist;

#define MAXSYM		3


/* Production prototypes */

static void CR(void);
static void Ident(char *s);
static void Declaration(int *startedDFA);
static void Attribs(int *n);
static void SemText(int *n);
static void Expression(int *n);
static void SetDecl(void);
static void TokenDecl(int sec_type);
static void NameDecl(void);
static void TokenExpr(int *n);
static void CompSet(PSet items);
static void SimSet(PSet items);
static void String(char *s);
static void ChrSet(int *n);
static void Term(int *n);
static void Factor(int *n);
static void Symbol(char *name);
static void TokenTerm(int *n);
static void TokenFactor(int *n);


#define NSETBITS        16

static unsigned short int SymSet[][MAXSYM] = {
  /*EOF_Sym identSym stringSym PRODUCTIONSSym EqualSym ENDSym CHARACTERSSym TOKENSSym NAMESSym PRAGMASSym COMMENTSSym IGNORESym LparenPointSym */
  {0x7EC7,0x4,0x80},
  /*identSym stringSym PointSym ANYSym LparenSym BarSym WEAKSym LbrackSym LbraceSym SYNCSym LparenPointSym */
  {0x106,0xBA80,0x82},
  /*identSym ENDSym */
  {0x202,0x0,0x0},
  /*PointSym RparenSym RbrackSym RbraceSym */
  {0x100,0x4400,0x1},
  /*identSym stringSym PointSym ANYSym LparenSym RparenSym BarSym WEAKSym LbrackSym RbrackSym LbraceSym RbraceSym SYNCSym LparenPointSym */
  {0x106,0xFE80,0x83},
  /*PRODUCTIONSSym PointSym CHARACTERSSym TOKENSSym NAMESSym PRAGMASSym COMMENTSSym TOSym NESTEDSym IGNORESym RparenSym RbrackSym RbraceSym */
  {0x7D40,0x4407,0x1},
  /*identSym stringSym LparenSym LbrackSym LbraceSym */
  {0x6,0xA200,0x0},
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
      if (Sym == OptionsSym) { /*42*/
      	char s[100];
      	  LookAheadString(s, sizeof(s)-1);
      	  SetOptions(s);
      } else
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

static void CR(void)
{
	Name name1;
	int  attr, sem, exp, is_new, sp, type;
	int startedDFA = FALSE;
	PNTermNode sn;
	Expect(COMPILERSym);
	Ident(compiler_name);
	global_defs.pos  = S_NextPos;
	global_defs.line = S_NextLine;
	while (Sym >= identSym && Sym <= COMPILERSym ||
	       Sym >= EqualSym && Sym <= ENDSym ||
	       Sym >= FROMSym && Sym <= NESTEDSym ||
	       Sym >= CASESym && Sym <= No_Sym) {
		Get();
	}
	global_defs.len  =
	  (int) (S_NextPos-global_defs.pos);
	while (Sym >= CHARACTERSSym && Sym <= COMMENTSSym ||
	       Sym == IGNORESym) {
		Declaration(&startedDFA);
	}
	while (!(Sym == EOF_Sym ||
	         Sym == PRODUCTIONSSym)) { GenError(43); Get(); }
	if (Successful()) { /* No Errors so far */
	  if (!MakeDeterministic()) SemError(127);
	};
	Expect(PRODUCTIONSSym);
	while (Sym == identSym) {
		attr = NIL; sem = NIL;
		Ident(name1);
		if ((sp = FindSym(name1, &type)) != UNDEF) {
		  is_new = FALSE;
		  if (type != T_NT) { SemError(108); return; }
		  else {
		    sn = GetNTermP(sp);
		    if (sn->graph) SemError(107);
		    sn->line_dec = S_Line;
		  }
		} else {
		  sp = NewSym(name1, T_NT);
		  sn = GetNTermP(sp); is_new = TRUE;
		  sn->line_dec = S_Line;
		};
		if (Sym == LessSym ||
		    Sym == LessPointSym) {
			Attribs(&attr);
		}
		if (!is_new) {
		  if (sn->has_attr && !attr) SemError(105);
		  if (!sn->has_attr && attr) SemError(105);
		}
		if (attr) {
		  sn->attr = attr; sn->has_attr = TRUE;
		};
		ExpectWeak(EqualSym,1);
		if (Sym == LparenPointSym) {
			SemText(&sem);
		}
		Expression(&exp);
		if (sem) {
		  (void) LinkGraph(sem, exp); exp = sem;
		};
		ExpectWeak(PointSym,2);
		sn = GetNTermP(sp); /* reload */
		sn->graph = exp;
		while (!(Sym >= EOF_Sym && Sym <= identSym ||
		         Sym == ENDSym)) { GenError(44); Get(); }
	}
	Expect(ENDSym);
	Ident(name1);
	if (strcmp(name1, compiler_name)) SemError(117);
	if((sp = FindSym(compiler_name, &type)) != UNDEF) {
	  if (type!=T_NT) SemError(108);
	  else {
	    sn = GetNTermP(sp);
	    if (sn->has_attr) SemError(112);
	    sn->reachable=TRUE;
	  }
	} else SemError(111);
	no_sym = NewSym("not", T_T);
	Expect(PointSym);
	if (dirty_DFA && !MakeDeterministic()) SemError(127);
}

static void Ident(char *s)
{
	Expect(identSym);
	LexString(s, MAX_ID_LEN-1);
}

static void Declaration(int *startedDFA)
{
	Set ignore;
	int n1, n2, nested = FALSE;
	switch (Sym) {
		case CHARACTERSSym:  
			Get();
			while (Sym == identSym) {
				SetDecl();
			}
			break;
		case TOKENSSym:  
			Get();
			while (Sym >= identSym && Sym <= stringSym) {
				TokenDecl(T_T);
			}
			break;
		case NAMESSym:  
			Get();
			while (Sym == identSym) {
				NameDecl();
			}
			break;
		case PRAGMASSym:  
			Get();
			while (Sym >= identSym && Sym <= stringSym) {
				TokenDecl(T_P);
			}
			break;
		case COMMENTSSym:  
			Get();
			Expect(FROMSym);
			TokenExpr(&n1);
			Expect(TOSym);
			TokenExpr(&n2);
			if (Sym == NESTEDSym) {
				Get();
				nested = TRUE;
			}
			if (n1 * n2) NewComment(n1, n2, nested);
			break;
		case IGNORESym:  
			Get();
			if (Sym == CASESym) {
				Get();
				if (*startedDFA) SemError(130);
				ignore_case = TRUE;
			} else if (Sym >= identSym && Sym <= stringSym ||
			           Sym >= ANYSym && Sym <= CHRSym) {
				Set_Init(&ignore);
				CompSet(&ignore);
				AddIgnore(&ignore);
				if (Set_IsItem(&ignore,0)) SemError(119);
				Set_Done(&ignore);
			} else GenError(45);
			break;
		default :GenError(46); break;
	}
	*startedDFA = TRUE;
}

static void Attribs(int *n)
{
	long P;
	int Len, Line, Col;
	if (Sym == LessSym) {
		Get();
		P = S_Pos+1; Line = S_Line; Col = S_Col;
		while (Sym >= identSym && Sym <= LessSym ||
		       Sym >= LessPointSym && Sym <= No_Sym) {
			if (Sym >= identSym && Sym <= stringSym ||
			    Sym >= numberSym && Sym <= LessSym ||
			    Sym >= LessPointSym && Sym <= No_Sym) {
				Get();
			} else if (Sym == badstringSym) {
				Get();
				SemError(102);
			} else GenError(47);
		}
		Expect(GreaterSym);
		Len = (int) (S_Pos - P);
		*n = MakeSemGraph(T_ATTR, P, Len, Line, Col);
	} else if (Sym == LessPointSym) {
		Get();
		P = S_Pos+2; Line = S_Line; Col = S_Col;
		while (Sym >= identSym && Sym <= LessPointSym ||
		       Sym >= LparenPointSym && Sym <= No_Sym) {
			if (Sym >= identSym && Sym <= stringSym ||
			    Sym >= numberSym && Sym <= LessPointSym ||
			    Sym >= LparenPointSym && Sym <= No_Sym) {
				Get();
			} else if (Sym == badstringSym) {
				Get();
				SemError(102);
			} else GenError(48);
		}
		Expect(PointGreaterSym);
		Len = (int) (S_Pos - P);
		*n = MakeSemGraph(T_ATTR, P, Len, Line, Col);
	} else GenError(49);
}

static void SemText(int *n)
{
	long P;
	int Len, Line, Col;
	Expect(LparenPointSym);
	P = S_Pos+2; Line = S_Line; Col = S_Col;
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
		} else GenError(50);
	}
	Expect(PointRparenSym);
	Len = (int) (S_Pos - P);
	*n = MakeSemGraph(T_SEM, P, Len, Line, Col);
}

static void Expression(int *n)
{
	int n0 = NIL, n1, n2, SX_Line;
	Term(&n1);
	while (WeakSeparator(BarSym,4,3)) {
		if (n0 == NIL)
		  n0 = n1 = MakeGraph(T_ALT, n1);
		SX_Line = S_Line;
		Term(&n2);
		n2 = MakeGraph(T_ALT, n2);
		SetGraphLine(n2,SX_Line);
		n1 = LinkAltGraph(n1, n2);
	}
	*n = (n0 ? n0 : n1);
}

static void SetDecl(void)
{
	Name name;
	Set  items;
	Set_Init(&items);
	Ident(name);
	if (FindClass(name) != UNDEF) SemError(107);
	Expect(EqualSym);
	CompSet(&items);
	Expect(PointSym);
	if (Set_Empty(&items)) SemError(101);
	(void) NewClass(name, &items);
	Set_Done(&items);
}

static void TokenDecl(int sec_type)
{
	char name[MAX_STR_LEN];
	int p = 0, sp, type;
	if (Sym == identSym) {
		Ident(name);
		if ((sp = FindSym(name, &type)) != UNDEF) SemError(107);
		else sp = NewSym(name, sec_type);
		while (!(Sym >= EOF_Sym && Sym <= stringSym ||
		         Sym >= PRODUCTIONSSym && Sym <= EqualSym ||
		         Sym >= CHARACTERSSym && Sym <= COMMENTSSym ||
		         Sym == IGNORESym ||
		         Sym == LparenPointSym)) { GenError(51); Get(); }
		if (Sym == EqualSym) {
			Get();
			TokenExpr(&p);
			if (sec_type == T_T) ConvertToStates(p, sp);
			else  ConvertToStates(p, sp+FIRST_PRAGMA);
			Expect(PointSym);
		} else if (Sym >= identSym && Sym <= stringSym ||
		           Sym == PRODUCTIONSSym ||
		           Sym >= CHARACTERSSym && Sym <= COMMENTSSym ||
		           Sym == IGNORESym ||
		           Sym == LparenPointSym) {
			P_option = TRUE;
		} else GenError(52);
	} else if (Sym == stringSym) {
		String(name);
		P_option = TRUE;
		if ((sp = FindSym(name, &type)) != UNDEF) SemError(107);
		else sp = NewSym(name, sec_type);
	} else GenError(53);
	if (Sym == LparenPointSym) {
		SemText(&p);
		if (sec_type == T_T) SemError(114);
		else SetPragmaText(sp, p);
	}
}

static void NameDecl(void)
{
	Name username, name;
	Ident(username);
	Expect(EqualSym);
	if (Sym == identSym) {
		Ident(name);
	} else if (Sym == stringSym) {
		String(name);
	} else GenError(54);
	Expect(PointSym);
	NewName(name, username);
}

static void TokenExpr(int *n)
{
	int n0 = NIL, n1, n2;
	TokenTerm(&n1);
	while (WeakSeparator(BarSym,6,5)) {
		if (n0 == NIL)
		n0 = n1 = MakeGraph(T_ALT, n1);
		TokenTerm(&n2);
		n2 = MakeGraph(T_ALT, n2);
		n1 = LinkAltGraph(n1, n2);
	}
	*n = (n0 ? n0 : n1);
}

static void CompSet(PSet items)
{
	Set set1, set2;
	Set_Init(&set1); Set_Init(&set2);
	SimSet(&set1);
	while (Sym >= PlusSym && Sym <= MinusSym) {
		if (Sym == PlusSym) {
			Get();
			SimSet(&set2);
			Set_Union(&set1, &set2);
		} else if (Sym == MinusSym) {
			Get();
			SimSet(&set2);
			Set_Diference(&set1, &set2);
		} else GenError(55);
		Set_Clean(&set2);
	}
	Set_Union(items, &set1);
	Set_Done(&set1); Set_Done(&set2);
}

static void SimSet(PSet items)
{
	Name name;
	char str[MAX_STR_LEN];
	int n1, n2;
	switch (Sym) {
		case identSym:  
			Ident(name);
			if (FindClass(name) == UNDEF) SemError(115);
			GetClassWithName(name, items);
			break;
		case stringSym:  
			String(str);
			StringClass(str, items);
			break;
		case CHRSym:  
			ChrSet(&n1);
			if (Sym == Range) {
				Get();
				ChrSet(&n2);
				Set_AddRange(items, n1, n2);
			} else if (Sym == PRODUCTIONSSym ||
			           Sym == PointSym ||
			           Sym >= CHARACTERSSym && Sym <= COMMENTSSym ||
			           Sym == IGNORESym ||
			           Sym >= PlusSym && Sym <= MinusSym) {
				Set_AddItem(items, n1);
			} else GenError(56);
			break;
		case ANYSym:  
			Get();
			Set_Union(items, &ANY_SET);
			break;
		default :GenError(57); break;
	}
}

static void String(char *s)
{
	Expect(stringSym);
	LexString(s, MAX_STR_LEN-1);
	FixString(s);
}

static void ChrSet(int *n)
{
	char str[5]; int x;
	Expect(CHRSym);
	Expect(LparenSym);
	if (Sym == numberSym) {
		Get();
		LexString(str, sizeof(str)-1);
		x = atoi(str);
		if (x > 255) { SemError(118); x = 0; }
		*n = x;
	} else if (Sym == stringSym) {
		Get();
		LexString(str, sizeof(str)-1);
		if (strlen(str) != 3) SemError(118);
		*n = str[1];
	} else GenError(58);
	Expect(RparenSym);
}

static void Term(int *n)
{
	int n0 = NIL, n1, n2;
	if (Sym >= identSym && Sym <= stringSym ||
	    Sym == ANYSym ||
	    Sym == LparenSym ||
	    Sym >= WEAKSym && Sym <= LbrackSym ||
	    Sym == LbraceSym ||
	    Sym == SYNCSym ||
	    Sym == LparenPointSym) {
		Factor(&n1);
		n0 = n1;
		while (Sym >= identSym && Sym <= stringSym ||
		       Sym == ANYSym ||
		       Sym == LparenSym ||
		       Sym >= WEAKSym && Sym <= LbrackSym ||
		       Sym == LbraceSym ||
		       Sym == SYNCSym ||
		       Sym == LparenPointSym) {
			Factor(&n2);
			n1 = LinkGraph(n1, n2);
		}
	} else if (Sym == PointSym ||
	           Sym >= RparenSym && Sym <= BarSym ||
	           Sym == RbrackSym ||
	           Sym == RbraceSym) {
		n0 = MakeSemGraph(T_SEM, -1, 0, S_Line, S_Col);
	}
	*n = n0;
}

static void Factor(int *n)
{
	char name1[MAX_STR_LEN];
	int weak = 0, SX_Line;
	int n1, n2 = NIL;
	int sp, is_new, type;
	PNTermNode snt;
	switch (Sym) {
		case identSym: 
		case stringSym: 
		case WEAKSym:  
			if (Sym == WEAKSym) {
				Get();
				weak = 1;
			}
			Symbol(name1);
			sp = FindSym(name1, &type);
			if (type == T_CLASS) SemError(104);
			if (weak && type == T_T) type = T_WT;
			if (weak && type == T_NT) SemError(123);
			n1 = MakeGraph(type, sp);
			if (type == T_NT) {
			  snt = GetNTermP(sp);
			  is_new = snt->graph == 0;
			  snt->line_use = S_Line;
			  snt->reachable = TRUE;
			};
			if (Sym == LessSym ||
			    Sym == LessPointSym) {
				Attribs(&n2);
				(void) LinkAltGraph(n1, n2);
				if (type != T_NT) SemError(103);
				else {
				  if(!is_new && !snt->has_attr) SemError(105);
				  if (is_new) snt->has_attr = TRUE;
				};
			} else if (Sym >= identSym && Sym <= stringSym ||
			           Sym == PointSym ||
			           Sym == ANYSym ||
			           Sym >= LparenSym && Sym <= SYNCSym ||
			           Sym == LparenPointSym) {
				if (type == T_NT)
				if (!is_new && snt->has_attr) SemError(105);
			} else GenError(59);
			break;
		case LparenSym:  
			Get();
			Expression(&n1);
			Expect(RparenSym);
			break;
		case LbrackSym:  
			Get();
			SX_Line = S_Line;
			Expression(&n1);
			Expect(RbrackSym);
			n1 = MakeGraph(T_OPT, n1);
			SetGraphLine(n1,SX_Line);
			break;
		case LbraceSym:  
			Get();
			SX_Line = S_Line;
			Expression(&n1);
			Expect(RbraceSym);
			n1 = MakeGraph(T_REP, n1);
			SetGraphLine(n1,SX_Line);
			break;
		case LparenPointSym:  
			SemText(&n1);
			break;
		case ANYSym:  
			Get();
			n1 = MakeGraph(T_ANY, 0);
			break;
		case SYNCSym:  
			Get();
			n1 = MakeGraph(T_SYNC, 0);
			break;
		default :GenError(60); break;
	}
	*n = n1;
}

static void Symbol(char *name)
{
	int sp, type;
	if (Sym == identSym) {
		Ident(name);
		sp = FindSym(name, &type);
		if (sp == UNDEF) sp = NewSym(name, T_NT);
	} else if (Sym == stringSym) {
		String(name);
		sp = FindSym(name, &type);
		if (sp == UNDEF) {
		  sp = NewSym(name, T_T);
		  MatchLiteral(sp);
		};
	} else GenError(61);
}

static void TokenTerm(int *n)
{
	int n0 = NIL, n1, n2;
	TokenFactor(&n1);
	n0 = n1;
	while (Sym >= identSym && Sym <= stringSym ||
	       Sym == LparenSym ||
	       Sym == LbrackSym ||
	       Sym == LbraceSym) {
		TokenFactor(&n2);
		n1 = LinkGraph(n1, n2);
	}
	if (Sym == CONTEXTSym) {
		Get();
		Expect(LparenSym);
		TokenExpr(&n2);
		Expect(RparenSym);
		SetCtx(n2); n1 = LinkGraph(n1, n2);
	}
	*n = n0;
}

static void TokenFactor(int *n)
{
	char name[MAX_STR_LEN];
	int p = 0;
	switch (Sym) {
		case identSym:  
			Ident(name);
			if ((p = FindClass(name)) == UNDEF) {
			  /* Just Create a valid node */
			  p = MakeGraph(T_CHAR, 0);
			  SemError(115);
			} else p = MakeGraph(T_CLASS, p);
			break;
		case stringSym:  
			String(name);
			p = StrToGraph((unsigned char *) name);
			break;
		case LparenSym:  
			Get();
			TokenExpr(&p);
			Expect(RparenSym);
			break;
		case LbrackSym:  
			Get();
			TokenExpr(&p);
			Expect(RbrackSym);
			p = MakeGraphOp(T_OPT, p);
			break;
		case LbraceSym:  
			Get();
			TokenExpr(&p);
			Expect(RbraceSym);
			p = MakeGraphOp(T_REP, p);
			break;
		default :GenError(62); break;
	}
	*n = p;
}



void Parse(void)
{ S_Reset(); Get();
  CR();
}


