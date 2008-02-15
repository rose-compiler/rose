#include "crt.h"

/******************************************************************
       Transition Constants, Definitions, and Functions
********************************************************************/

typedef struct {
  int type;               /* transition symbol type T_SET | T_CHAR*/
  int sym;                /* transition symbol */
  int tc;                 /* transiton code T_NORMAL | T_CONTEXT */
  Set to_states;          /* States after transition with sym */
} TransNode;

typedef TransNode *PTransNode;

typedef struct {          /* State of finite automaton */
  Collection  trans_list; /* Transitions */
  int end_of;             /* # of recognized token if state is final */
  int ctx;                /* TRUE => state reached by contextTrans */
  int gen_state_no;
} StateNode;

typedef StateNode *PStateNode;

typedef struct {
  Set set;                /* info about melted states */
  int state;              /* new state */
} MeltedNode;

typedef MeltedNode *PMeltedNode;

/******************************************************************
       Comment Definitions and Functions
********************************************************************/

typedef struct {
  char start_token[3];    /* Begining String */
  char end_token[3];      /* End String */
  int  nested;            /* TRUE => Nested */
} CommentNode;

typedef CommentNode *PCommentNode;

void NewComment(int start_token, int end_token, int nested);
void ShowCommentTab(void);

void ConvertToStates(int gp, int sp);
int  StrToGraph(byte *name);
int  MatchDFA(byte *str, int sp);
int  MakeDeterministic(void);
void MakeScanner(void);
void InitScannerTab(void);
void DoneScannerTab(void);
void ShowDFA(void);


