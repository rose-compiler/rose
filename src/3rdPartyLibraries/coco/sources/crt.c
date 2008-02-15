#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "crt.h"
#include "cra.h"
#include "crf.h"
#include "collect.h"
#include "set.h"
#include "crs.h"

#define      NODES_SIZE   100     /* Graph Node Table */
#define      NODES_EXTEND 20
Collection   nodes_tab;

#define      TERM_SIZE   100      /* Terminal Table */
#define      TERM_EXTEND 20
Collection   term_tab;

#define      NTERM_SIZE   100     /* Nonterminal Table */
#define      NTERM_EXTEND 20
Collection   nterm_tab;

#define      CLASS_SIZE   20      /* Set Table */
#define      CLASS_EXTEND 10
Collection   class_tab;

#define      SYMSET_SIZE   20     /* Symbol Sets Table */
#define      SYMSET_EXTEND 10
Collection   symset_tab;

#define      PRAGMA_SIZE  5       /* Pragmas Tables */
#define      PRAGMA_EXTEND 2
Collection   pragma_tab;

#define      NAME_SIZE  1         /* Names Tables */
#define      NAME_EXTEND 1
Collection   name_tab;

int          dirty_DFA=0;
struct SemText  global_defs;      /* TRUE => Globals Definitions */
int          first_weak_set = -1; /* First weak set index in symsettab */
int          ignore_case = FALSE; /* TRUE => Ignore case */
FILE         *lstfile;
int          no_sym = 0;
Set          ANY_SET;             /* Set of all characters */
Set          ALL_TERMS;           /* Set of all Terminals */

static Set   FollowVisited;       /* Visited graph Nodes in Comp_Follow */
static Set   FirstVisited;        /* Visited graph Nodes in Comp_First */
static Set   Set1;
static Set   Set2;
static int   CurrentNt;

int          C_option = FALSE;    /* TRUE => Generate Compiler */
int          F_option = FALSE;    /* TRUE => First & Follow */
int          G_option = FALSE;    /* TRUE => Graph listing */
int          L_option = FALSE;    /* TRUE => Generate listing */
int          P_option = FALSE;    /* TRUE => Generate Parser Only */
int          Q_option = FALSE;    /* TRUE => Q editor mode */
int          S_option = FALSE;    /* TRUE => Symbol Table Listing */
int          T_option = FALSE;    /* TRUE => Grammar Test Only */
int          D_option = FALSE;    /* TRUE => Debug #line */
int          Z_option = FALSE;    /* TRUE => Generate .hpp and .cpp files */
int          A_option = FALSE;    /* TRUE => Automata */
int          O_option = FALSE;    /* TRUE => Generate OR only Terminal Conditions */
int          GenCplusplus = FALSE;

char Frames_Path[100] = "";
char c_ext[100] = "c";
char h_ext[100] = "h";

static void PrintTerm(int t);
static void PrintNTerm(int t);
extern int Errors;

typedef void (*Graph_Func) (int gp);
typedef void (*Graph_FuncInt) (int gp, int p);
typedef void (*Graph_FuncSet) (int gp, Set *s);

void upcase(char *s)
{
	while (*s) {
		if (*s >= 'a' && *s <= 'z') *s -= 32;
		s++;
	}
}

/*****************************************************************
***          Graph Construction Functions                      ***
******************************************************************/

/* Create a graph Node and initialize it */
static int MakeGraphNode(int type)
{
	int gp;
	PGraphNode gn;

	gp = Collection_New(&nodes_tab);
	gn = GetGraphP(gp);

	gn->type     = type;
	gn->next     = NIL;
	gn->pointer1 = NIL;
	gn->pointer2 = NIL;
	gn->pointer3 = NIL;
	gn->pointer4 = NIL;
	return gp;
}

/* create a symbol node, Terminal and Nonterminal */
int MakeGraph(int type, int sp)
{
	int gp;
	PGraphNode gn;

	gp = MakeGraphNode(type);
	gn = GetGraphP(gp);
	CR_ASSERT(gn != NULL);
	gn->SLine = S_Line;
	gn->SYMLINK = sp;                  /* set pointer to Table */
	return gp;
}

/* */
void SetGraphLine(int current, int line)
{
	PGraphNode gn;
	gn = GetGraphP(current);
	CR_ASSERT(gn != NULL);
	gn->SLine = line;
}


/* create a semantic code node T_SEM , T_ATTR */
int MakeSemGraph(int type, long filepos, int len, int line, int col)
{
	int gp;
	PGraphNode gn;

	gp = MakeGraphNode(type);
	gn = GetGraphP(gp);
	CR_ASSERT(gn != NULL);
	gn->SEMPOS  = filepos;													/*kws*/
	gn->SEMLEN  = len;
	gn->SEMLINE = line;
	gn->SEMCOL  = col;
	return gp;
}

/* create an operator node with INNER => "link" */
int MakeGraphOp(int type, int link)
{
	int gp;
	PGraphNode gn;

	gp = MakeGraphNode(type);
	gn = GetGraphP(gp);
	CR_ASSERT(gn != NULL);
	gn->INNER = link;
	return gp;
}

/* link two nodes */
int LinkGraph(int current, int next)
{
	PGraphNode gn;

	do {
		gn = GetGraphP(current);
		CR_ASSERT(gn != NULL);
		current = gn->next;
	} while (current!=NIL);
	gn->next = next;
	return next;
}

/* link two alt nodes */
int LinkAltGraph(int current, int nextalt)
{
	PGraphNode gn;

	gn = GetGraphP(current);
	CR_ASSERT(gn != NULL);
	gn->ALT = nextalt;
	return nextalt;
}

/* prints a graph to the list file */
void ShowGraph(int gp)
{
	PGraphNode gn;

	while (gp > NIL) {
		gn = GetGraphP(gp);
		CR_ASSERT(gn != NULL);
		fprintf(lstfile, "%d^", gp);
		switch(gn->type) {
		case T_T :
		case T_WT:
			{
				PTermNode tn = GetTermP(gn->SYMLINK);
				fprintf(lstfile, "%s", tn->name);
				break;
			}
		case T_NT :
			{
				PNTermNode ntn = GetNTermP(gn->SYMLINK);
				fprintf(lstfile, "%s", ntn->name);
				break;
			}
		case T_OPT:
			fprintf(lstfile, "[");
			ShowGraph(gn->INNER);
			fprintf(lstfile, "]");
			break;
		case T_REP:
			fprintf(lstfile, "{");
			ShowGraph(gn->INNER);
			fprintf(lstfile, "}");
			break;
		case T_ALT:
			{
				PGraphNode gn1;
				fprintf(lstfile, "(");
				ShowGraph(gn->INNER);
				gn1 = gn;
				while (gn1->ALT) {
					fprintf(lstfile, "|");
					gn1 = GetGraphP(gn1->ALT);
					CR_ASSERT(gn1 != NULL);
					ShowGraph(gn1->INNER);
				}
				fprintf(lstfile, ")");
				break;
			}
		case T_SEM:
			fprintf(lstfile, "SEM");
			break;
		case T_ATTR:
			fprintf(lstfile, "ATTR");
			break;
		case T_SYNC:
			fprintf(lstfile, "SYNC");
			break;
		case T_ANY:
			fprintf(lstfile, "ANY");
			break;
		}
		gp = gn->next;
		fprintf(lstfile, " ");
	}
	fprintf(lstfile, "%d^", gp);
}

/*****************************************************************
***           Pragmas management Functions                     ***
*****************************************************************/

/* compare a Pragma Name with "symbol" */
static int CompPragmaName(PPragmaNode tn, PName name)
{
	return !strcmp(tn->name, name);
}

int FindPragma(PName name)
{
	int tp;
	tp = Collection_FirstThat(&pragma_tab,
			(Collection_Comp) CompPragmaName, name);
	return tp;
}

/* install a pragma */
int NewPragma(PName name)
{
	int pp;
	PPragmaNode pn;

	pp = Collection_New(&pragma_tab);
	pn = GetPragmaP(pp);
	CR_ASSERT(pn != NULL);
	strcpy(pn->name, name);
	pn->has_attr = FALSE;
	return pp;
}

void SetPragmaText(int sp, int gp)
{
	PPragmaNode pn;
	PGraphNode gn;

	pn = GetPragmaP(sp);
	CR_ASSERT(pn != NULL);
	gn = GetGraphP(gp);
	CR_ASSERT(gn != NULL);

	pn->has_attr      = TRUE;
	pn->sem_text.pos  = gn->SEMPOS;
	pn->sem_text.len  = gn->SEMLEN;
	pn->sem_text.line = gn->SEMLINE;
	pn->sem_text.col  = gn->SEMCOL;
}

/* add Pragmas to Tokens before generating the Scanner
	No_Sym & MAXT are marks to the last Token and Before first pragma */
void SetupPragmas(void)
{
	PPragmaNode pn;
	PTermNode tn;
	int c, i, tp;

	c = Collection_Count(&pragma_tab);
	for (i = 0; i < c; i++) {
		pn = GetPragmaP(i);
		CR_ASSERT(pn != NULL);
		tp = Collection_New(&term_tab);
		tn = GetTermP(tp);
		CR_ASSERT(tn != NULL);
		tn->type = T_PRAGMATOKEN;
		strcpy(tn->name, pn->name);
		SetTermName(tn);
	}
}

/*****************************************************************
***             Names management Functions                     ***
*****************************************************************/

/* compare a Name "Name" with "symbol" */
static int CompNameName(PNameNode tn, PName name)
{
	return !strcmp(tn->name, name);
}

/* get table index of terminal "symbol" */
int FindName(PName name)
{
	int tp;
	tp = Collection_FirstThat(&name_tab,
			(Collection_Comp) CompNameName, name);
	return tp;
}

/* set the symbolic name of a terminal */
void NewName(PName name, PName user_name)
{
	int np;
	PNameNode nn;

	np = Collection_New(&name_tab);
	nn = GetNameP(np);
	CR_ASSERT(nn != NULL);
	strcpy(nn->name, name);
	strcpy(nn->user_name, user_name);
}

/*****************************************************************
***          Terminal management Functions                     ***
*****************************************************************/

/* compare a terminal name with "symbol" */
static int CompTermName(PTermNode tn, PName name)
{
	return !strcmp(tn->name, name);
}

/* get table index of terminal "symbol" */
int FindTerm(PName name)
{
	int tp;
	tp = Collection_FirstThat(&term_tab,
			(Collection_Comp) CompTermName, name);
	return tp;
}

void SetTermName(PTermNode tn)
{
	char str[MAX_ID_LEN];
	char temp[MAX_ID_LEN];
	char name[MAX_ID_LEN + 100];
	char *t;

	strcpy(temp, tn->name);
	name[0] = '\0';
	t = temp;
	if (*t == '"' || *t == '\'') {
		t++;
		t[strlen(t) - 1] = '\0';
	}
	if (strcmp(temp, "EOF") == 0) strcpy(name, "EOF_");
	else if (strcmp(temp, "not") == 0) strcpy(name, "No_");
	else while (*t) {
		SymCharName(*t, str);
		strcat(name, str);
		t++;
	}
	/* ensure that the name is not longer that MAX_ID_LEN */
	name[MAX_ID_LEN - 4] = '\0';

	strcat(name, "Sym");
	strcpy(tn->gen_name, name);
}

/* install a new terminal */
int NewTerm(PName name)
{
	int tp;
	PTermNode tn;
	char TempName[MAX_STR_LEN];

	strcpy(TempName, name);
	TempName[MAX_ID_LEN - 1] = '\0';
	tp = FindTerm(TempName);
	if (tp != UNDEF) tn = (PTermNode) GetTermP(tp);
	else {
		tp = Collection_New(&term_tab);
		tn = GetTermP(tp);
		CR_ASSERT(tn != NULL);
		strcpy(tn->name, TempName);
		strcpy(tn->gen_name, "");
		tn->type = T_CLASSTOKEN;
	}
	return tp;
}

/* return the symbolic name of a terminal */
void GetTermName(int tp, PName name)
{
	PTermNode tn;
	PNameNode nn;
	int np;

	tn = GetTermP(tp);
	CR_ASSERT(tn != NULL);
	if (tn->gen_name[0] == '\0') { /* no genname */
		np = FindName(tn->name);
		if (np == UNDEF) SetTermName(tn);
		else {
			nn = GetNameP(np);
			strcpy(tn->gen_name, nn->user_name);
		}
	}
	strcpy(name, tn->gen_name);
}

/* print each terminal data */
static void Func_ShowTerm(PTermNode tn, int tp)
{
	CR_ASSERT(tn != NULL);
	fprintf(lstfile, "%3d|%s\t=\t", tp, tn->name);
	switch (tn->type) {
	case T_CLASSTOKEN:
		fprintf(lstfile, "Class Token");
		break;
	case T_CLASSLITTOKEN:
		fprintf(lstfile, "Class Literal Token");
		break;
	case T_LITTOKEN:
		fprintf(lstfile, "Literal Token");
		break;
	case T_PRAGMATOKEN:
		fprintf(lstfile, "Pragma Token");
		break;
	}
	fprintf(lstfile, "\n");
}

/* print terminal table to lstfile */
void ShowTermTab(void)
{
	fprintf(lstfile, "\nTERMINALS\n");
	Collection_ForEachPos(&term_tab, (Collection_FuncPos) Func_ShowTerm);
}


/*****************************************************************
***             Class management Functions                     ***
*****************************************************************/

/* find set by name */
static int CompClassName(PClassNode cn, PName name)
{
	return !strcmp(cn->name, name);
}

int FindClass(PName name)
{
	int cp;
	cp = Collection_FirstThat(&class_tab,
			(Collection_Comp) CompClassName, name);
	return cp;
}

/* find set by contents */
static int CompClassData(PClassNode cn, Set *data)
{
	CR_ASSERT(cn != NULL);
	return Set_Equal(&(cn->data), data);
}

int FindClassWithSet(PSet data)
{
	int cp;
	cp = Collection_FirstThat(&class_tab,
			(Collection_Comp) CompClassData, data);
	return cp;
}

/* install a set to the set table */
int NewClass(PName name, PSet set)
{
	int cp;
	PClassNode cn;

	cp = Collection_New(&class_tab);
	cn = GetClassP(cp);
	CR_ASSERT(cn != NULL);
	strcpy(cn->name, name);
	Set_Init(&cn->data);
	Set_Union(&cn->data, set);
	return cp;
}

int GetClassWithName(PName name, PSet set)
{
	int cp;
	PClassNode cn;

	Set_Clean(set);
	if ((cp = FindClass(name)) == UNDEF) return UNDEF;
	cn = GetClassP(cp);
	CR_ASSERT(cn != NULL);
	Set_Union(set, &cn->data);
	return cp;
}

/* print each set info */
static void Func_ShowClass(PClassNode cn, int cp)
{
	CR_ASSERT(cn != NULL);
	if (!cp) return;
	fprintf(lstfile, "%3d|%s\t=\t", cp, cn->name);
	Set_ForEach(&cn->data, (Set_Func) PrintAscii);
	fprintf(lstfile, "\n");
}

/* print set table */
void ShowClassTab(void)
{
	fprintf(lstfile, "\nClass\n");
	Collection_ForEachPos(&class_tab,
			(Collection_FuncPos) Func_ShowClass);
}

/*****************************************************************
***          Symbol Sets management Functions                  ***
*****************************************************************/

/* install a symbol set */
int NewSymSet(PSet set, byte typ)
{
	PSymSetNode sn;
	int sp;

	sp = Collection_New(&symset_tab);
	sn = GetSymSetP(sp);
	CR_ASSERT(sn != NULL);
	sn->type = typ;
	Set_Init(&sn->set);
	Set_Union(&sn->set, set);
	if (typ == T_WT && first_weak_set == -1) first_weak_set = sp;
	return sp;
}

/* install an ANY Terminals set */
int NewANY(void)
{
	return NewSymSet(&ALL_TERMS, T_ANY);
}

/* get symset data by index */
void GetSymSet(int sp, PSet set)
{
	PSymSetNode sn;
	if (sp == NIL) return;
	sn = GetSymSetP(sp);
	Set_Union(set, &sn->set);
}

/* symset union by index */
void IncludeSymSet(int sp, PSet set)
{
	PSymSetNode sn;
	sn = GetSymSetP(sp);
	CR_ASSERT(sn != NULL);
	Set_Union(&sn->set, set);
}

/* symset diference by index */
void ExcludeSymSet(int sp, PSet set)
{
	PSymSetNode sn;
	sn = GetSymSetP(sp);
	CR_ASSERT(sn != NULL);
	Set_Diference(&sn->set, set);
}

/* show symset */
static void Func_ShowSymSet(PSymSetNode sn, int sp)
{
	CR_ASSERT(sn != NULL);
	fprintf(lstfile, "%3d|", sp);
	switch (sn->type) {
	case T_WT  :
		fprintf(lstfile, "WEAK T = \t");
		break;
	case T_ANY :
		fprintf(lstfile, "ANY    = \t");
		break;
	case T_SYNC:
		fprintf(lstfile, "SYNC   = \t");
		break;
	}
	Set_ForEach(&sn->set, (Set_Func) PrintTerm);
	fprintf(lstfile, "\n");
}

void ShowSymSetTab(void)
{
	fprintf(lstfile, "\nSYMBOL SETS\n");
	Collection_ForEachPos(&symset_tab, (Collection_FuncPos) Func_ShowSymSet);
}

/*****************************************************************
***         Nonterminals management Functions                  ***
*****************************************************************/

/* find a nonterminal by name */
static int CompNTerm(PNTermNode ntn, PName name)
{
	return !strcmp(ntn->name, name);
}

int FindNTerm(PName name)
{
	int ntp;
	ntp = Collection_FirstThat(&nterm_tab, (Collection_Comp) CompNTerm, name);
	return ntp;
}

/* install a nonterminal */
int NewNTerm(PName name)
{
	int ntp;
	PNTermNode ntn;

	if ((ntp = FindNTerm(name)) == UNDEF) {
		ntp = Collection_New(&nterm_tab);
		ntn = GetNTermP(ntp);
		CR_ASSERT(ntn != NULL);
		strcpy(ntn->name, name);
		ntn->graph     = NIL;
		ntn->sem       = NIL;
		ntn->nullable  = FALSE;
		ntn->ready     = FALSE;
		ntn->has_attr  = FALSE;
		ntn->reachable = FALSE;
		ntn->attr      = NIL;
		Set_Init(&ntn->first);
		Set_Init(&ntn->follow);
		Set_Init(&ntn->AuxNt);
	}
	return ntp;
}

/* get nonterminal First Set */
static void GetNTermFirst(int ntp, Set *first)
{
	PNTermNode ntn;
	ntn = GetNTermP(ntp);
	CR_ASSERT(ntn != NULL);
	Set_Union(first, &(ntn->first));
}

/* get nonterminal Follow Set */
static void GetNTermFollow(int ntp, Set *follow)
{
	PNTermNode ntn;
	ntn = GetNTermP(ntp);
	CR_ASSERT(ntn != NULL);
	Set_Union(follow, &(ntn->follow));
}

/* get nonterminal Nullable */
static int IsNTermNullable(int ntp)
{
	PNTermNode ntn;
	ntn = GetNTermP(ntp);
	CR_ASSERT(ntn != NULL);
	return ntn->nullable;
}

/* show nonterminal */
static void Func_ShowNTerm(PNTermNode ntn, int ntp)
{
	if (!ntp) return;
	fprintf(lstfile, "%3d|%s\t:", ntp, ntn->name);
	if (ntn->nullable) fprintf(lstfile, "    (Nullable)");
	if (G_option) {
		fprintf(lstfile, "\n\tGraph: ");
		ShowGraph(ntn->graph);
	}
	if (F_option) {
		fprintf(lstfile, "\n\tStart With :\t");
		Set_ForEach(&ntn->first, (Set_Func) PrintTerm);
		fprintf(lstfile, "\n\tFollow =\t");
		Set_ForEach(&ntn->follow, (Set_Func) PrintTerm);
	}
	fprintf(lstfile, "\n");
}

/* show nonterminal tab */
void ShowNTermTab(void)
{
	fprintf(lstfile, "\nNO TERMINALS\n");
	Collection_ForEachPos(&nterm_tab, (Collection_FuncPos) Func_ShowNTerm);
}

/* apply "fn" to each nonterminal graph */
static void ForEachNTermGraph(Graph_Func fn)
{
	PNTermNode ntn;
	int c, i;
	c = Collection_Count(&nterm_tab);
	for (i = 1; i < c; i++) { /* for each nonterminal */
		CurrentNt = i;
		ntn = GetNTermP(i);
		CR_ASSERT(ntn != NULL);
		(*fn) (ntn->graph);
	}
}

/* apply "fn" to each nonterminal graph pasing an "p" as parameter */
static void ForEachNTermGraph_Int(Graph_FuncInt fn, int p)
{
	PNTermNode ntn;
	int c, i;
	c = Collection_Count(&nterm_tab);
	for (i = 1; i < c; i++) {
		CurrentNt = i;
		ntn = GetNTermP(i);
		CR_ASSERT(ntn != NULL);
		(*fn) (ntn->graph, p);
	}
}

/*****************************************************************
***          First & Follow Functions                          ***
*****************************************************************/

/* compute follow links in a graph */
void CompFollowNode(int gp, int fgp)
{
	PGraphNode gn;
	int next;

	while (gp>NIL) {
		gn = GetGraphP(gp);
		CR_ASSERT(gn != NULL);
		next = (gn->next != NIL) ? gn->next : fgp;
		switch(gn->type) {
		case T_OPT:
			CompFollowNode(gn->INNER, next);
			break;
		case T_REP:
			CompFollowNode(gn->INNER, gp);
			break;
		case T_ALT:
			{
				PGraphNode gn1;
				int gp1 = gp;
				while (gp1 > NIL) {
					gn1 = GetGraphP(gp1);
					CR_ASSERT(gn1 != NULL);
					CompFollowNode(gn1->INNER, next);
					gp1 = gn1->ALT;
				}
				break;
			}
		}
		if (gn->next == NIL) gn->next = -fgp;
		gp = gn->next;
	}
}

/* compute Follow links for all graphs */
static void CompAllFollowNodes(void)
{
	ForEachNTermGraph_Int((Graph_FuncInt) CompFollowNode, 0);
}

/* TRUE => node is Nullable */
static int IsNullableNode(int gp)
{
	PGraphNode gn;
	if (gp <= NIL) return TRUE;
	gn = GetGraphP(gp);
	CR_ASSERT(gn != NULL);
	switch(gn->type) {
	case T_NT :
		return IsNTermNullable(gn->SYMLINK);
	case T_OPT:
	case T_REP:
	case T_SEM:
	case T_SYNC:
		return TRUE;
	case T_ALT:
		if (IsNullableGraph(gn->INNER)) return TRUE;
		if (gn->ALT) return IsNullableNode(gn->ALT);
	}
	return FALSE; /* Terminal, Weak Terminal => Not Nullable */
}

/* TRUE => graph is Nullable */
int IsNullableGraph(int gp)
{
	if (gp <= NIL) return TRUE;
	if (IsNullableNode(gp)) {
		PGraphNode gn = GetGraphP(gp);
		CR_ASSERT(gn != NULL);
		return IsNullableGraph(gn->next);
	}
	return FALSE;
}

/* TRUE => graph is Nullable (use follow links) */
static int IsNullableNextGraph(int gp)
{
	if (gp <= NIL) return TRUE;
	if (IsNullableNode(gp)) {
		PGraphNode gn = GetGraphP(gp);
		CR_ASSERT(gn != NULL);
		return IsNullableNextGraph(abs(gn->next));
	}
	return FALSE;
}

static void CompAllNullable(void)
{
	PNTermNode ntn;
	int i, c, change;

	do { /* Loop until nothing changes */
		change = FALSE;
		c = Collection_Count(&nterm_tab);
		for (i = 1; i < c; i++) { /* for each nonterminal */
			ntn = GetNTermP(i);
			CR_ASSERT(ntn != NULL);
			if (!ntn->nullable)
				if (IsNullableNextGraph(ntn->graph)) {
					ntn->nullable = TRUE;
					ntn->ready    = TRUE;
					change        = TRUE;
				}
		}
	} while (change);
}

/* get First Terminals of a graph
   STRICT = TRUE ==> don't use follow links
*/
static void CompFirst(int gp, Set *first, int STRICT)
{
	PGraphNode gn;

	while (gp > NIL && !Set_IsItem(&FirstVisited, gp)) {
		Set_AddItem(&FirstVisited, gp);
		gn = GetGraphP(gp);
		CR_ASSERT(gn != NULL);
		switch(gn->type) {
		case T_T :
		case T_WT:
			Set_AddItem(first, gn->SYMLINK);
			break;
		case T_NT :
			{
				PNTermNode ntn = GetNTermP(gn->SYMLINK);
				CR_ASSERT(ntn != NULL);
				if (ntn->ready) GetNTermFirst(gn->SYMLINK, first);
				else CompFirst(ntn->graph, first, STRICT);
				break;
			}
		case T_ANY:
			GetSymSet(gn->SETLINK1, first);
			break;
		case T_OPT:
		case T_REP:
		case T_ALT:
			CompFirst(gn->INNER, first, STRICT);
			if (gn->type == T_ALT) CompFirst(gn->ALT, first, STRICT);
			break;
		}
		if (!IsNullableNode(gp)) break; /* Nullable => Continue */
		gp = (STRICT)? gn->next: abs(gn->next);
	}
}

/* get First Set of a graph */
void CompFirstSet(int gp, PSet first)
{
	Set_Clean(&FirstVisited);
	CompFirst(gp, first, TRUE);
}

/* get First Set of a graph (use follow links) */
static void CompNextFirstSet(int gp, Set *first)
{
	Set_Clean(&FirstVisited);
	CompFirst(gp, first, FALSE);
}

static void CompNTermFirstSet(PNTermNode ntn)
{
	ntn->ready = FALSE;
	CompFirstSet(ntn->graph, &ntn->first);
	ntn->ready = TRUE;
}

/* compute all First Sets of a nonterminal */
static void CompAllFirstSets(void)
{
	Collection_ForEach(&nterm_tab, (Collection_Func) CompNTermFirstSet);
}

/* compute Follow Set of each nonterminal in graph */
static void CompFollowSet(int gp)
{
	PGraphNode gn;

	while (gp > NIL && !Set_IsItem(&FollowVisited, gp)) {
		Set_AddItem(&FollowVisited, gp);
		gn = GetGraphP(gp);
		CR_ASSERT(gn != NULL);
		switch(gn->type) {
		case T_NT:
			{
				PNTermNode ntn = GetNTermP(gn->SYMLINK);
				CR_ASSERT(ntn != NULL);
				CompNextFirstSet(abs(gn->next), &(ntn->follow));
				if (IsNullableNextGraph(abs(gn->next)))
					Set_AddItem(&ntn->AuxNt, CurrentNt);
				break;
			}
		case T_OPT:
		case T_REP:
		case T_ALT:
			CompFollowSet(gn->INNER);
			if (gn->type == T_ALT) CompFollowSet(gn->ALT);
			break;
		}
		gp = gn->next;
	}
}

/* complete follow set by indirect nonterminal links */
static void CompleteFollow(int curr_nt)
{
	int i, c;
	PNTermNode ntn, ntn1;

	if (Set_IsItem(&FollowVisited, curr_nt)) return;
	Set_AddItem(&FollowVisited, curr_nt);
	ntn = GetNTermP(curr_nt);
	CR_ASSERT(ntn != NULL);
	Set_GetRange(&ntn->AuxNt, &i, &c);
	for (; i <= c; i++)
		if (Set_IsItem(&ntn->AuxNt, i)) {
			ntn1 = GetNTermP(i);
			CR_ASSERT(ntn1 != NULL);
			CompleteFollow(i);
			Set_Union(&ntn->follow, &ntn1->follow);
			Set_DelItem(&ntn->AuxNt, i);
		}
}

static void Func_CompFollowSet(int gp)
{
	Set_Clean(&FollowVisited);
	CompFollowSet(gp);
}

static void Func_CompleteFollowSet(int gp)
{
	Set_Clean(&FollowVisited);
	CompleteFollow(CurrentNt);
}

/* compute Follow of all nonterminals */
static void CompAllFollowSets(void)
{
	ForEachNTermGraph((Graph_Func) Func_CompFollowSet);
	ForEachNTermGraph((Graph_Func) Func_CompleteFollowSet);
}

/* return graph node index of a leading ANY in graph */
static int LeadingANY(int gp)
{
	PGraphNode gn;
	int a;

	if (gp <= NIL) return NIL;
	gn = GetGraphP(gp);
	CR_ASSERT(gn != NULL);
	switch (gn->type) {
	case T_ANY:
		return gp;
	case T_OPT:
	case T_REP:
		return LeadingANY(gn->INNER);
	case T_ALT:
		if ((a = LeadingANY(gn->INNER)) != NIL) return a;
		return a = LeadingANY(gn->ALT);
	}
	if (IsNullableNode(gp)) return LeadingANY(gn->next);
	return NIL;
}

/* exclude terminals from ANY set */
static void ExcludeANY(int gp, PSet set)
{
	PGraphNode gn;
	gn = GetGraphP(gp);
	CR_ASSERT(gn != NULL);
	if (gn->SETLINK1 == NIL) gn->SETLINK1 = NewANY();
	ExcludeSymSet(gn->SETLINK1, set);
}

/* compute ANY sets in graph */
static void CompANYSet(int gp)
{
	PGraphNode gn;
	Set set;

	Set_Init(&set);
	while (gp > NIL) {
		gn = GetGraphP(gp);
		CR_ASSERT(gn != NULL);
		switch(gn->type) {
		case T_OPT:
		case T_REP:
			{
				int a;
				CompANYSet(gn->INNER);
				if ((a = LeadingANY(gn->INNER)) != NIL) {
					Set_Clean(&set);
					CompExpected(abs(gn->next), CurrentNt, &set);
					ExcludeANY(a, &set);
				}
				break;
			}
		case T_ALT:
			{
				int gp1 = gp;
				int a;
				PGraphNode gn1;

				Set_Clean(&set);
				while (gp1 != NIL) {
					gn1 = GetGraphP(gp1);
					CR_ASSERT(gn1 != NULL);
					CompANYSet(gn1->INNER);
					if ((a = LeadingANY(gn1->INNER)) != NIL) {
						CompExpected(gn1->ALT, CurrentNt, &set);
						ExcludeANY(a, &set);
					} else CompExpected(gn1->INNER, CurrentNt, &set);
					gp1 = gn1->ALT;
				}
				break;
			} /* case */
		} /* switch */
		gp = gn->next;
	}
	Set_Done(&set);
}

static void Func_CompAnySet(int gp)
{
	Set_Clean(&FollowVisited);
	CompANYSet(gp);
}

/* compute all ANY sets */
static void CompAllANYSets(void)
{
	ForEachNTermGraph((Graph_Func) Func_CompAnySet);
}

/* compute SYNC sets in a graph */
static void CompSYNCSet(int gp)
{
	PGraphNode gn;
	Set set;

	Set_Init(&set);
	while (gp > NIL) {
		gn = GetGraphP(gp);
		CR_ASSERT(gn != NULL);
		switch(gn->type) {
		case T_SYNC:
			Set_Clean(&set);
			CompExpected(abs(gn->next), CurrentNt, &set);
			Set_AddItem(&set, EOFSYM);
			IncludeSymSet(ALL_SYNCS, &set);
			gn->SETLINK1 = NewSymSet(&set, T_SYNC);
			break;
		case T_OPT:
		case T_REP:
		case T_ALT:
			CompSYNCSet(gn->INNER);
			if (gn->type == T_ALT) CompSYNCSet(gn->ALT);
			break;
		}
		gp = gn->next;
	}
	Set_Done(&set);
}

/* compute all SYNC sets */
static void CompAllSYNCSets(void)
{
	Set set;

	Set_Init(&set);
	Set_AddItem(&set, EOFSYM);
	IncludeSymSet(ALL_SYNCS, &set);
	ForEachNTermGraph((Graph_Func) CompSYNCSet);
}

/* compute WEAK Terminal sets in a graph */
static void CompWEAKSet(int gp)
{
	PGraphNode gn, gn1;
	Set set;

	Set_Init(&set);
	while (gp > NIL) {
		gn = GetGraphP(gp);
		CR_ASSERT(gn != NULL);
		switch(gn->type) {
		case T_WT:
			Set_Clean(&set);
			CompExpected(abs(gn->next), CurrentNt, &set);  /* Follow WEAK TERMINAL */
			gn->SETLINK1 = NewSymSet(&set, T_WT);
			break;
		case T_REP:
			gn1 = GetGraphP(gn->INNER);
			CR_ASSERT(gn1 != NULL);
			if (gn1->type == T_WT) {
				Set_Clean(&set);
				CompExpected(abs(gn->next), CurrentNt, &set); /* Follow LOOP */
				gn1->SETLINK2 = NewSymSet(&set, T_WT);
			}
			CompWEAKSet(gn->INNER);
			break;
		case T_OPT:
		case T_ALT:
			CompWEAKSet(gn->INNER);
			if (gn->type == T_ALT) CompWEAKSet(gn->ALT);
			break;
		}
		gp = gn->next;
	}
	Set_Done(&set);
}

/* compute all WEAK sets */
static void CompAllWEAKSets(void)
{
	ForEachNTermGraph((Graph_Func) CompWEAKSet);
}

/* compute set of all terminals except pragmas */
static void CompALLTermsSet(void)
{
	Set_Clean(&ALL_TERMS);
	Set_AddRange(&ALL_TERMS, 1, Collection_Count(&term_tab) - 1);
}

static void Func_NTermDeclared(PNTermNode ntn, int ntp)
{
	if (ntp == 0) return;
	if (ntn->graph == NIL) {
		if (Q_option) {
			fprintf(lstfile, "(%s)  (%d, %d )",
			        source_name, ntn->line_use, 0);
			fprintf(lstfile, " %s Not Declared\n", ntn->name);
		}
		else {
			fprintf(lstfile, "\"%s\", Line %d, Col %d :",
			        source_name, ntn->line_use, 0);
			fprintf(lstfile, "**** %s Not Declared\n", ntn->name);
		}
		Errors++;
	} else
	if (!ntn->reachable) {
		if (Q_option) {
			fprintf(lstfile, "(%s)  (%d, %d )",
			        source_name, ntn->line_dec, 0);
			fprintf(lstfile, " %s Declared but not used\n", ntn->name);
		}
		else {
			fprintf(lstfile, "\"%s\", Line %d, Col %d :",
			        source_name, ntn->line_dec, 0);
			fprintf(lstfile, "**** %s Declared but not used\n", ntn->name);
		}
		Errors++;
	}
}

/* check if a nonterminal was use but not defined, or defined and not used */
static void CheckNTermDeclared(void)
{
	Collection_ForEachPos(&nterm_tab, (Collection_FuncPos) Func_NTermDeclared);
}

static void Func_NTermNullable(PNTermNode ntn, int ntp)
{
	if (! ntn->nullable || ntp == 0) return;
	if (Q_option) {
		fprintf(lstfile, "(%s)  (%d, %d )",
		        source_name, S_Line, 0);
		fprintf(lstfile, " Warning : %s is nullable\n", ntn->name);
	}
	else {
		fprintf(lstfile, "\"%s\", Line %d, Col %d :",
		        source_name, S_Line, 0);
	fprintf(lstfile, "**** Warning : %s is nullable\n", ntn->name);
	}
}

/* check if a nonterminal is nullable */
static void CheckNTermNullable(void)
{
	Collection_ForEachPos(&nterm_tab, (Collection_FuncPos) Func_NTermNullable);
}

/* check for nonterminal circular derivations */
static int CompCircularGraph(int gp)
{
	PGraphNode gn;
	int t;

	while (gp > NIL) {
		gn = GetGraphP(gp);
		CR_ASSERT(gn != NULL);
		switch(gn->type) {
		case T_T :
		case T_WT:
			return FALSE;
		case T_NT :
			{
				PNTermNode ntn = GetNTermP(gn->SYMLINK);
				CR_ASSERT(ntn != NULL);
				if (gn->SYMLINK == CurrentNt) return CurrentNt;
				if (Set_IsItem(&Set1, gn->SYMLINK)) return FALSE;
				Set_AddItem(&Set1, gn->SYMLINK);
				if (CompCircularGraph(ntn->graph)) return gn->SYMLINK;
				break;
			}
		case T_OPT:
		case T_REP:
		case T_ALT:
			if ((t = CompCircularGraph(gn->INNER)) != FALSE) return t;
			if (gn->type == T_ALT)
				if ((t = CompCircularGraph(gn->ALT)) != FALSE) return t;
			break;
		}
		if (!IsNullableNode(gp)) break; /* Nullable => Continue */
		gp = gn->next;
	}
	return FALSE;
}

static void Func_CheckCircular(PNTermNode ntn, int ntp)
{
	int t;
	Set_Clean(&Set1);
	CurrentNt = ntp;
	if ((t = CompCircularGraph(ntn->graph)) != FALSE) {
		PNTermNode ntn1 = GetNTermP(t);
		if (Q_option)
			fprintf(lstfile, " Circular Derivation %s -> %s\n", ntn->name, ntn1->name);
		else
			fprintf(lstfile, "**** Circular Derivation %s -> %s\n", ntn->name, ntn1->name);
		Errors++;
	}
}

static void CheckCircular(void)
{
	Collection_ForEachPos(&nterm_tab, (Collection_FuncPos) Func_CheckCircular);
}

static int IsTerminalGraph(int gp)
{
	PGraphNode gn;
	while (gp > NIL) {
		gn = GetGraphP(gp);
		CR_ASSERT(gn != NULL);
		switch(gn->type) {
		case T_NT:
			if (!Set_IsItem(&Set1, gn->SYMLINK)) return FALSE;
			break;
		case T_ALT:
			if (!IsTerminalGraph(gn->INNER) &&
					((gn->ALT == NIL) || !IsTerminalGraph(gn->ALT))) return FALSE;
			break;
		}
		gp = gn->next;
	}
	return TRUE;
}

/* check if a nonterminal can derive to terminals */
static void CheckDeriveTerminals(void)
{
	PNTermNode ntn;
	int c, i, change;

	Set_Clean(&Set1);
	c = Collection_Count(&nterm_tab);
	do {
		change = FALSE;
		for (i = 1; i < c; i++) /* for each Nonterminal */
			if (!Set_IsItem(&Set1, i)) {
				ntn = GetNTermP(i);
				CR_ASSERT(ntn != NULL);
				if (IsTerminalGraph(ntn->graph)) {
					Set_AddItem(&Set1, i);
					change = TRUE;
				}
			}
	} while (change);

	for (i = 1; i < c; i++) /* for each nonterminal */
		if (!Set_IsItem(&Set1, i)) {
			ntn = GetNTermP(i);
			CR_ASSERT(ntn != NULL);
			if (Q_option)
				fprintf(lstfile, " %s Can't derive to Terminals\n", ntn->name);
			else
				fprintf(lstfile, "**** %s Can't derive to Terminals\n", ntn->name);
			Errors++;
		}
}

/* Generate LL(1) errors */
static void LL1Error(int c, int i)
{
	PNTermNode ntn;
	PTermNode tn;

	ntn = GetNTermP(CurrentNt);
	CR_ASSERT(ntn != NULL);
	if (Q_option) {
		fprintf(lstfile, "(%s)  (%d, %d )",
		        source_name, S_Line, 0);
		fprintf(lstfile, " LL(1) Error in %s: ", ntn->name);
	}
	else {
		fprintf(lstfile, "\"%s\", Line %d, Col %d :",
		        source_name, S_Line, 0);
		fprintf(lstfile, "**** LL(1) Error in %s: ", ntn->name);
	}
	if (i > 0) {
		tn = GetTermP(i);
		fprintf(lstfile, "%s is the ", tn->name);
	}
	switch (c) {
	case 1:
		fprintf(lstfile, "start of several alternatives.\n");
		break;
	case 2:
		fprintf(lstfile, "start & successor of nullable structures.\n");
		break;
	case 3:
		fprintf(lstfile, "an ANY node matches no symbol.\n");
		break;
	}
}

/* check if two sets have items in common */
static void CheckSets(int cond, Set *s1, Set *s2)
{
	int i, c;
	Set_GetRange(s1, &i, &c);
	for (; i <= c; i++)
		if (Set_IsItem(s1, i) && Set_IsItem(s2, i)) LL1Error(cond, i);
}

/* perform LL(1) checks in graph */
static void Func_CheckLL1Graph(int gp)
{
	PGraphNode gn;
	while (gp > NIL) {
		gn = GetGraphP(gp);
		CR_ASSERT(gn != NULL);
		switch(gn->type) {
		case T_ALT:
			{
				PGraphNode gn1;
				int gp1;
				gp1 = gp;
				Set_Clean(&Set1);
				while (gp1 > NIL) {
					gn1 = GetGraphP(gp1);
					CR_ASSERT(gn1 != NULL);
					Set_Clean(&Set2);
					CompExpected(gn1->INNER, CurrentNt, &Set2);
					CheckSets(1, &Set1, &Set2);
					Set_Union(&Set1, &Set2);
					gp1 = gn1->ALT;
				}
				gp1 = gp;
				while (gp1 > NIL) {
					gn1 = GetGraphP(gp1);
					CR_ASSERT(gn1 != NULL);
					Func_CheckLL1Graph(gn1->INNER);
					gp1 = gn1->ALT;
				}
				break;
			}
		case T_OPT:
		case T_REP:
			Set_Clean(&Set1);
			Set_Clean(&Set2);
			CompExpected(gn->INNER, CurrentNt, &Set1);
			CompExpected(abs(gn->next), CurrentNt, &Set2);
			CheckSets(2, &Set1, &Set2);
			Func_CheckLL1Graph(gn->INNER);
			break;
		case T_ANY:
			Set_Clean(&Set1);
			GetSymSet(gn->SETLINK1, &Set1);
			if (Set_Empty(&Set1)) LL1Error(3, 0);
			break;
		}
		gp = gn->next;
	}
}

static void CheckLL1(void)
{
	ForEachNTermGraph((Graph_Func) Func_CheckLL1Graph);
}

/* compute all symbol sets needed */
void CompSymbolSets(void)
{
	CheckNTermDeclared();
	if (Errors) return;
	CheckDeriveTerminals();
	if (Errors) return;
	CompALLTermsSet();
	SetupPragmas();
	CompAllFollowNodes();
	CompAllNullable();
	CheckNTermNullable();
	CheckCircular();
	if (Errors) return;

	CompAllFirstSets();
	CompAllFollowSets();
	CompAllANYSets();

	CompAllFirstSets();    /* Second Time: In very few cases */
	CompAllFollowSets();   /* ANY sets can add information to first */
	CompAllANYSets();      /* and Follow */

	CompAllSYNCSets();
	CompAllWEAKSets();
	CheckLL1();
}

/* get expected next symbols */
void CompExpected(int gp, int nterm, Set *set)
{
	CompNextFirstSet(gp, set);
	if (IsNullableNextGraph(gp)) GetNTermFollow(nterm, set);
}

/*****************************************************************
***          General purpose Functions                         ***
*****************************************************************/

/* add ignore sets */
void AddIgnore(Set *set)
{
	int cp;
	PClassNode cn;

	cp = FindClass("@ignore_chars");
	if (cp != UNDEF) {
		cn = GetClassP(cp);
		Set_Union(&cn->data, set);
	}
}

/* install a symbol */
int NewSym(PName name, int typ)
{
	int sp = 0;

	switch (typ) {
	case T_T:
	case T_WT:
		sp = NewTerm(name);
		break;
	case T_P:
		sp = NewPragma(name);
		break;
	case T_NT:
		sp = NewNTerm(name);
		break;
	}
	return sp;
}

/* find symbol */
int FindSym(PName name, int *typ)
{
	int sp;

	sp = FindClass(name);
	if (sp > UNDEF) {
		*typ = T_CLASS;
		return sp;
	}

	sp = FindTerm(name);
	if (sp > UNDEF) {
		*typ = T_T;
		return sp;
	}

	sp = FindPragma(name);
	if (sp > UNDEF) {
		*typ = T_P;
		return sp;
	}

	sp = FindNTerm(name);
	if (sp > UNDEF) {
		*typ = T_NT;
		return sp;
	}

	return UNDEF;
}

/* print ascii char */
void PrintAscii(int s)
{
	if (s <= 32) fprintf(lstfile, " #%d ", s);
	else fprintf(lstfile, "%c", s);
}

/* print integer */
void PrintInt(int s)
{
	fprintf(lstfile, "%d ", s);
}

/* print terminal */
static void PrintTerm(int tp)
{
	PTermNode tn;
	tn = GetTermP(tp);
	fprintf(lstfile, "%s ", tn->name);
}

/* print nonterminal */
static void PrintNTerm(int ntp)
{
	PNTermNode ntn;
	ntn = GetNTermP(ntp);
	CR_ASSERT(ntn != NULL);
	fprintf(lstfile, "%s ", ntn->name);
}


/* create & initialize all module variables */
void InitTab(void)
{
	Collection_Init(&nterm_tab, sizeof(NTermNode), NTERM_SIZE, NTERM_EXTEND);
	Collection_Init(&term_tab, sizeof(TermNode), TERM_SIZE, TERM_EXTEND);
	Collection_Init(&nodes_tab, sizeof(GraphNode), NODES_SIZE, NODES_EXTEND);
	Collection_Init(&class_tab, sizeof(ClassNode), CLASS_SIZE, CLASS_EXTEND);
	Collection_Init(&symset_tab, sizeof(SymSetNode), SYMSET_SIZE, SYMSET_EXTEND);
	Collection_Init(&pragma_tab, sizeof(PragmaNode), PRAGMA_SIZE, PRAGMA_EXTEND);
	Collection_Init(&name_tab, sizeof(NameNode), NAME_SIZE, NAME_EXTEND);

	Set_Init(&ANY_SET);
	Set_Init(&ALL_TERMS);
	Set_Init(&FirstVisited);
	Set_Init(&FollowVisited);
	Set_Init(&Set1);
	Set_Init(&Set2);

	Set_Clean(&ANY_SET);
	(void) NewSymSet(&ANY_SET, T_SYNC);           /* ALL SYNCS symset 0 */

	Set_AddItem(&ANY_SET, ' ');
	(void) NewClass("@ignore_chars", &ANY_SET);  /* IGNORE SET 0 */

	(void) NewSym("EOF", T_T);                   /* EOF_Sym term 0 */
	(void) NewSym("@NTERM", T_NT);               /* No Terminal 0 */

	(void) MakeGraphNode(0);                     /* graph Node 0 */
	Set_AddRange(&ANY_SET, 1, 255);              /* ANY SET */
}

void CleanGraphTab(void)
{
	Collection_Clean(&nodes_tab);
	(void) MakeGraphNode(0);                     /* graph Node 0 */
}

/* destroy all module variables */
void DoneTab(void)
{
	Collection_Done(&nterm_tab);
	Collection_Done(&term_tab);
	Collection_Done(&nodes_tab);
	Collection_Done(&class_tab);
	Collection_Done(&symset_tab);
	Collection_Done(&pragma_tab);
	Collection_Done(&name_tab);
	Set_Done(&ANY_SET);
	Set_Done(&ALL_TERMS);
	Set_Done(&FirstVisited);
	Set_Done(&FollowVisited);
	Set_Done(&Set1);
	Set_Done(&Set2);
}

char *SetVar(char *s)
{
	char name[200], value[200];
	char *n = name, *v = value;

	/* Get the variable name */
	while (*s && *s != '=') *n++ = *s++;
	*n='\0';

	if (*s == '=') s++;

	/* Get the variable Value */
	while (*s) *v++ = *s++;
	*v = '\0';

/*	printf("Variable %s = %s \n", name, value); */

	if (stricmp(name, "CRHEXT") == 0) strcpy(h_ext, value);
	if (stricmp(name, "CRCEXT") == 0) strcpy(c_ext, value);
	if (stricmp(name, "CRFRAMES") == 0) strcpy(Frames_Path, value);

	return s;
}

void SetOptions(char *s)
{
	while (*s) {
		switch (*s) {
		case 'A' :
		case 'a' :
			L_option = TRUE;
			A_option = TRUE;
			S_option = TRUE;
			break;
		case 'C' :
		case 'c' :
			C_option = TRUE;
			break;
		case 'F' :
		case 'f' :
			L_option = TRUE;
			F_option = TRUE;
			S_option = TRUE;
			break;
		case 'G' :
		case 'g' :
			L_option = TRUE;
			G_option = TRUE;
			S_option = TRUE;
			break;
		case 'L' :
		case 'l' :
			L_option = TRUE;
			break;
		case 'O' :
		case 'o' :
			O_option = TRUE;
			break;
		case 'P' :
		case 'p' :
			P_option = TRUE;
			break;
		case 'Q' :
		case 'q' :
			Q_option = TRUE;
			break;
		case 'S' :
		case 's' :
			L_option = TRUE;
			S_option = TRUE;
			break;
		case 'T' :
		case 't' :
			T_option = TRUE;
			break;
		case 'D' :
		case 'd' :
			if (s[1] == '\0')
				D_option = TRUE;
			else {
				s++;
				s=SetVar(s);
				return;
			}
			break;
		case 'X' :
		case 'x' :
			GenCplusplus = TRUE;
			strcpy(c_ext, "cpp");
			strcpy(h_ext, "hpp");
			break;
		case 'Z' :
		case 'z' :
			Z_option = TRUE;
			strcpy(c_ext, "cpp");
			strcpy(h_ext, "hpp");
			break;
		}
		s++;
	}
}


