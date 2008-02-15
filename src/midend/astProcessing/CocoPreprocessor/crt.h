#ifndef CRT_MODULE
#define CRT_MODULE
#include "collect.h"
#include "set.h"
#include <stdio.h>

#if _DEBUG
#define DEBUG 1
#endif

#if DEBUG
#define CR_ASSERT(cond) \
if( !(cond) )\
{fprintf(stderr, "assertion error line %d, file(%s)\n", \
__LINE__, __FILE__);  }
#else
#define CR_ASSERT(cond) ;
#endif

typedef unsigned char byte;
#define MAX_ID_LEN   32
#define MAX_STR_LEN  100

struct SemText{
  long pos;
  int  line,len, col;
};

typedef char     Name[MAX_ID_LEN];
typedef char     *PName;

#define EOFSYM   0       /* EOF Symbol */
/*#define NOSYM    0*/

#define TRUE  1          /* Boolean True */
#define FALSE 0          /* Boolean False */
#define UNDEF -1

#define T_CLASSTOKEN    1
#define T_CLASSLITTOKEN 2
#define T_LITTOKEN      3
#define T_PRAGMATOKEN   4

/******************************************************************
     Graph Node Constants, Definitions and Functions
********************************************************************/

#define NIL       0
#define T_NONE    0
#define T_T       1      /* terminal */
#define T_WT      2      /* weak terminal */
#define T_NT      3      /* no terminal */
#define T_OPT     4      /* optional */
#define T_REP     5      /* repetition */
#define T_ALT     6      /* alternation */
#define T_SEM     7      /* semantic action */
#define T_ATTR    8      /* attribute */
#define T_ANY     9      /* ANY symbol */
#define T_SYNC   10      /* SYNC */
#define T_P      11      /* Pragma */

#define T_CHAR    T_T    /* Simple Char */
#define T_CLASS   T_WT   /* Char Set */
#define T_CONTEXT T_T    /* Context Trans */
#define T_NORMAL  T_NONE /* Normal Trans */

typedef struct {
  int   type;            /* node's type */
  int   next;            /* next node  0 -> none, < 0 follow link */
  long  pointer1;        /* auxiliary pointer, see use below */                         /*kws*/
  int   pointer2;
  int   pointer3;
  int   pointer4;
  int   SLine;
} GraphNode;

typedef GraphNode *PGraphNode;

#define INNER    pointer1    /* T_REP, T_OPT, T_ALT  */
#define ALT      pointer2    /* next T_ALT alternative
                                T_NT link to attributes*/

#define SYMLINK  pointer1    /* T_T, T_WT, T_NT link to table */

#define SETLINK1 pointer2    /* T_W, T_SYNC, T_ANY link to Sets */
#define SETLINK2 pointer3

#define SEMPOS   pointer1    /* T_ATTR, T_SEM */
#define SEMLEN   pointer2
#define SEMLINE  pointer3
#define SEMCOL   pointer4

#define STATE    pointer3    /* NFA graph */
#define CONTEXT  pointer4

int  MakeGraph(int type, int sem_link);
int  MakeSemGraph(int type, long filepos, int len, int line, int col);
int  MakeGraphOp(int type, int link);
int  LinkGraph(int current, int next);
int  LinkAltGraph(int current, int nextalt);
void SetGraphLine(int current, int line);
void ShowGraph(int graph);
void CleanGraphTab(void);

#define GetGraphP(p) (PGraphNode) Collection_At(&nodes_tab, p)

/******************************************************************
     Terminal Definitions and Functions
********************************************************************/

typedef struct {
  Name name;             /* Terminal Name */
  Name gen_name;         /* output name */
  byte type;
} TermNode;

typedef TermNode* PTermNode;

int  FindTerm(PName name);
void SetTermName(PTermNode tn);
int  NewTerm(PName  name);
void GetTermName(int sp, PName name);
void ShowTermTab(void);
#define GetTermP(p)     (PTermNode) Collection_At(&term_tab, p)

/******************************************************************
     Class Definitions and Functions
********************************************************************/

typedef struct {
  Name name;             /* Set Name */
  Set  data;             /* Set of Elements */
} ClassNode;

typedef ClassNode *PClassNode;

int  FindClass(PName name);
int  FindClassWithSet(PSet data);
int  GetClassWithName(PName name, Set *data);
int  GetClassWithData(PSet data);
int  NewClass(PName name, PSet set);
void ShowClassTab(void);
#define GetClassP(p)     (PClassNode) Collection_At(&class_tab, p)

/******************************************************************
     Symbol Sets Definitions and Functions
********************************************************************/

#define ALL_SYNCS   0

typedef struct {
  Set  set;              /* Symbols Set */
  byte type;             /* Type Of Set (T_ANY, T_SYNC, T_WEEK) */
} SymSetNode;

typedef SymSetNode *PSymSetNode;

int  NewSymSet(PSet set, byte typ);
int  NewANY(void);
void GetSymSet(int index, PSet set);
void IncludeSymSet(int index, PSet set);
void ExcludeSymSet(int index, PSet set);
void ShowSymSetTab(void);
#define GetSymSetP(p)     (PSymSetNode) Collection_At(&symset_tab, p)

/******************************************************************
     Non-Terminal Definitions and Functions
********************************************************************/

typedef struct {
  Name name;             /* No Terminal Name */
  int  graph;            /* EBNF graph */
  int  has_attr;         /* TRUE => Attributes */
  int  attr;
  int  sem;              /* > 0 => Semantic */
  int  line_dec;         /* Line Number (for Error reporting) */
  int  line_use;
  int  nullable;         /* TRUE => Nullable */
  int  ready;            /* TRUE => First Set Ready */
  int  reachable;        /* TRUE => Reachable (Used in RHS of a production) */
  Set  first;            /* First Set */
  Set  follow;           /* Follow Set */
  Set  AuxNt;            /* Auxiliary No Terminal Set */
} NTermNode;

typedef NTermNode *PNTermNode;

int FindNTerm(PName name);
int NewNTerm(PName  name);
void ShowNTermTab(void);

#define GetNTermP(p)     (PNTermNode) Collection_At(&nterm_tab, p)

/******************************************************************
     Pragma Definitions and Functions
********************************************************************/

typedef struct {
  Name name;             /* Terminal Name */
  int has_attr;
  struct SemText sem_text;
} PragmaNode;

typedef PragmaNode *PPragmaNode;
int  FindPragma(PName name);
int  NewPragma(PName name);
void SetPragmaText(int sp, int gp);
void SetupPragmas(void);

#define GetPragmaP(p)     (PPragmaNode) Collection_At(&pragma_tab, p)
#define FIRST_PRAGMA      1024

/******************************************************************
     Names Definitions and Functions
********************************************************************/

typedef struct {
  Name name;             /* Terminal Name */
  Name user_name;
} NameNode;

typedef NameNode *PNameNode;
int  FindName(PName name);
void NewName(PName name, PName user_name);
#define GetNameP(p)     (PNameNode) Collection_At(&name_tab, p)

/******************************************************************
     General purpose Definitions and Functions
********************************************************************/

void upcase(char *s);
void AddIgnore(PSet set);
int  NewSym(PName name, int typ);
int  FindSym(PName name, int *typ);

void InitTab(void);
void DoneTab(void);

int  IsNullableGraph(int gp);
void CompFirstSet(int gp, PSet first);
void CompExpected(int gp, int nt, PSet set);
void CompFollowNode(int gp, int fgp);
void CompSymbolSets(void);

void PrintAscii(int s);
void PrintInt(int i);

void SetOptions(char *s);
extern void GenSemError(int nr); /* crp.c */

extern Collection nterm_tab;
extern Collection term_tab;
extern Collection nodes_tab;
extern Collection class_tab;
extern Collection symset_tab;
extern Collection pragma_tab;
extern Collection name_tab; /* ++++ */

extern struct SemText global_defs;
extern int dirty_DFA;
extern int ignore_case;
extern int first_weak_set;
extern int no_sym;
extern FILE *lstfile;
extern Set  ANY_SET;
extern Set  ALL_TERMS;

extern int C_option;     /* TRUE => Generate Compiler */
extern int F_option;     /* TRUE => First & Follow */
extern int G_option;     /* TRUE => graph listing */
extern int L_option;     /* TRUE => Generate listing */
extern int P_option;     /* TRUE => Generate Parser Only */
extern int Q_option;     /* TRUE => Q Editor mode */
extern int O_option;     /* TRUE => Generate OR only Terminal conditions */
extern int S_option;     /* TRUE => Symbol Table Listing */
extern int T_option;     /* TRUE => Gramar Test Only */
extern int A_option;     /* TRUE => Trace Automata */
extern int D_option;     /* TRUE => Debug #line */
extern int Z_option;     /* TRUE => generate .hpp and .cpp files */
extern int GenCplusplus;

extern char Frames_Path[];
extern char c_ext[];
extern char h_ext[];


#endif  /* CRT_MODULE */


