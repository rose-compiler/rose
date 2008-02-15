/************************************************
**   Mar 24, 2000  Version 1.15
**      Complex DFA Fix
**      F. Arzu 
**   Sep 8, 2002 Version 1.17
**      Comment checking fix
**      P. Terry
*************************************************/
#include "collect.h"
#include "set.h"
#include "cra.h"
#include "crt.h"
#include "crf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static FILE *fscan, *fhead;

static Collection state_tab;        /* states */
static Collection melted_tab;       /* melted_tab states */
static Collection comment_tab;      /* comment tab */

static int last_sim_state;          /* last simple (non melted_tab state) */
static int last_state;              /* last allocated state */
static int root_state;              /* start state of DFA */
static Set Visited_Nodes;
static Set Stepped_Nodes;

#define GetStateP(p)      (PStateNode) Collection_At(&state_tab, p)
#define GetMeltedP(p)     (PMeltedNode) Collection_At(&melted_tab, p)
#define GetTransP(list,p) (PTransNode) Collection_At(list, p)
#define GetCommentP(p)    (PCommentNode) Collection_At(&comment_tab, p)


extern void SemError(int);

/*****************************************************************
***          Comment management Functions                      ***
*****************************************************************/

/* Convert a Comment Graph to a String */
static int GraphToComment(int gp, char *s)
{
	PGraphNode gn;

	while (gp > 0) {
		gn = GetGraphP(gp);
		CR_ASSERT(gn != NULL);
		if (gn->type == T_CHAR) *s++ = (char) gn->SYMLINK;
		else if (gn->type == T_CLASS) {
			PClassNode sn = GetClassP(gn->SYMLINK);
			int i, c;
			CR_ASSERT(sn != NULL);
			if (Set_Elements(&sn->data) != 1) SemError(126);
			Set_GetRange(&sn->data, &i, &c);
			for (; i <= c; i++)
				if (Set_IsItem(&sn->data, i)) *s++ = (byte) i;
		}
		else SemError(122);
		gp = gn->next;
	}
	*s = '\0';
	return TRUE;
}

/* install a new comment */
void NewComment(int start_token, int end_token, int nested)
{
	int p;
	PCommentNode n;
	char temp[255];

	p = Collection_New(&comment_tab);
	n = GetCommentP(p);
	CR_ASSERT(n != NULL);
	GraphToComment(start_token, temp);
	if (strlen(temp) > 2) SemError(125);
	temp[2] = '\0';
	strcpy(n->start_token, temp);
	GraphToComment(end_token, temp);
	if (strlen(temp) > 2) SemError(125);
	temp[2] = '\0';
	strcpy(n->end_token, temp);
	n->nested = nested;
}

static void Func_ShowComment(PCommentNode n, int p)
{
	fprintf(lstfile, "%3d| FROM %s  TO  %s ", p, n->start_token, n->end_token);
	if (n->nested) fprintf(lstfile, "NESTED\n");
	else fprintf(lstfile, "\n");
}

/* show all comments */
void ShowCommentTab(void)
{
	fprintf(lstfile, "\nCOMMENTS\n");
	Collection_ForEachPos(&comment_tab, (Collection_FuncPos) Func_ShowComment);
}

/* add a new transition to a state */
static void AddTrans(int sp, PTransNode t)
{
	PStateNode sn;
	int n;

	sn = GetStateP(sp);
	CR_ASSERT(sn != NULL);
	n = Collection_New(&sn->trans_list);
	Collection_Put(&sn->trans_list, n, t);
}

/* delete transition from a state */
static void DelTrans(int snr, PTransNode t)
{
	PStateNode sn;
	PTransNode t1;
	int c, i;

	sn = GetStateP(snr);
	CR_ASSERT(sn != NULL);
	c = Collection_Count(&sn->trans_list);
	for (i = 0; i < c; i++) {
		t1 = GetTransP(&sn->trans_list, i);
		CR_ASSERT(t1 != NULL);
		if (t1->type == t->type && t1->sym == t->sym && t1->tc == t->tc
				&& &t1->to_states == &t->to_states) {
			t1->type = T_NONE;
			return;
		}
	}
}

/* find a valid transition with ch from state snr */
static int FindTrans(int snr, byte ch)
{
	PStateNode sn;
	PTransNode t;
	int c, i;

	sn = GetStateP(snr);
	CR_ASSERT(sn != NULL);
	c = Collection_Count(&sn->trans_list);
	for (i = 0; i < c; i++) {
		t = GetTransP(&sn->trans_list, i);
		CR_ASSERT(t != NULL);
		if (t->type == T_CHAR) {
			if (t->sym == ch) return i;
		} else
			if (t->type == T_CLASS) {
				PClassNode cn = GetClassP(t->sym);
				CR_ASSERT(cn != NULL);
				if (Set_IsItem(&cn->data, ch)) return i;
			}
	}
	return UNDEF;
}

/* change an old transition with a new character set */
static void ChangeTrans(PTransNode t, Set *set)
{
	if (Set_Elements(set) == 1) {
		t->type = T_CHAR;
		t->sym = Set_MinIndex(set);
	} else {
		int n = FindClassWithSet(set);
		if (n == UNDEF) n = NewClass("##", set);
		t->type = T_CLASS;
		t->sym = n;
	}
}

/* combine two transitions; combine target states and Context */
static void CombineTrans(PTransNode a, PTransNode b)
{
	CR_ASSERT(a != NULL);
	CR_ASSERT(b != NULL);
	Set_Union(&a->to_states, &b->to_states);
	if (b->tc == T_CONTEXT) a->tc = T_CONTEXT;
}

/* check if two transitions overlap; Transitions with common elements */
static int OverlapTrans(PTransNode a, PTransNode b)
{
	PClassNode cna, cnb;
	int result = 0;
	CR_ASSERT(a != NULL);
	CR_ASSERT(b != NULL);
	if (a->type == T_CHAR) {
		if (b->type == T_CHAR) result = a->sym == b->sym;
		else
			if (b->type == T_CLASS) { /* Char with Class */
				cnb = GetClassP(b->sym);
				CR_ASSERT(cnb != NULL);
				result = Set_IsItem(&cnb->data, a->sym);
			}
	} else
		if (a->type == T_CLASS) {
			if (b->type == T_CHAR) { /* Char with Class */
				cna = GetClassP(a->sym);
				CR_ASSERT(cna != NULL);
				result = Set_IsItem(&cna->data, b->sym);
			} else
				if (b->type == T_CLASS) { /* Class with Class */
					cna = GetClassP(a->sym);
					cnb = GetClassP(b->sym);
					CR_ASSERT(cna != NULL);
					CR_ASSERT(cnb != NULL);
					result = ! Set_Diferent(&cna->data, &cnb->data);
				}
		}
	return result;
}


/* create a new melted state (Join 2 or more old state) */
static int NewMelt(Set *set, int s)
{
	int n;
	PMeltedNode mn;

	n = Collection_New(&melted_tab);
	mn = GetMeltedP(n);
	CR_ASSERT(mn != NULL);
	Set_Init(&mn->set);
	Set_Union(&mn->set, set);
	mn->state = s;
	return n;
}

/* create a new state */
static int NewState(void)
{
	PStateNode sn;
	last_state = Collection_New(&state_tab);
	sn = GetStateP(last_state);
	CR_ASSERT(sn != NULL);
	sn->end_of = 0;
	sn->ctx = T_NONE;
	sn->gen_state_no = -1;
	Collection_Init(&sn->trans_list, sizeof(TransNode), 1, 1);
	return last_state;
}

static void Func_DoneTrans(PTransNode tn)
{
	Set_Done(&tn->to_states);
}

static void DoneState(PStateNode s)
{
	s->end_of = 0;
	s->ctx = 0;
	Collection_ForEach(&s->trans_list, (Collection_Func) Func_DoneTrans);
	Collection_Done(&s->trans_list);
}

/* generate transition (g.state, g.sym) --> tostate */
static void NewTransition(int from_state, PGraphNode gn, int to_state)
{
	TransNode t;
	if (to_state == root_state) SemError(121);
	t.type = gn->type;
	t.sym  = gn->SYMLINK;
	t.tc   = gn->CONTEXT;
	Set_Init(&t.to_states);
	Set_AddItem(&t.to_states, to_state);
	AddTrans(from_state, &t);
}

/* find a transition from state s with char ch */
static int GetTransition(int s, byte ch)
{
	int tn;
	PTransNode t;
	PStateNode sn;

	tn = FindTrans(s, ch);
	if (tn == UNDEF) return UNDEF;
	sn = GetStateP(s);
	CR_ASSERT(sn != NULL);
	t  = GetTransP(&sn->trans_list, tn);
	CR_ASSERT(t != NULL);
	tn = Set_MinIndex(&t->to_states);
	return tn;
}

/* get transition characters */
static void GetTransChars(PTransNode t, Set *set)
{
	Set_Clean(set);
	if (t->type == T_CHAR) Set_AddItem(set, t->sym);
	else
		if (t->type == T_CLASS) {
			PClassNode cn = GetClassP(t->sym);
			CR_ASSERT(cn != NULL);
			Set_Union(set, &cn->data);
		}
}

/* combine transitions to the same state */
static void CombineShifts(void)
{
	int s, i, j, c;
	PStateNode sn;
	PTransNode a, b;
	Collection *trans_tab;
	Set seta, setb;

	Set_Init(&seta);
	Set_Init(&setb);

	for(s = root_state; s <= last_state; s++) {
		sn = GetStateP(s);
		CR_ASSERT(sn != NULL);
		trans_tab = &sn->trans_list;
		c = Collection_Count(trans_tab);
		for (i = 0; i < c; i++) {
			a = GetTransP(trans_tab, i);
			CR_ASSERT(a != NULL);
			if (a->type == T_NONE) continue;  /* Trans Deleted */
			for(j = i + 1; j < c; j++) {
				b = GetTransP(trans_tab, j);
				CR_ASSERT(b != NULL);
				if (b->type == T_NONE) continue;  /* Trans Deleted */
				if (Set_Equal(&a->to_states, &b->to_states) && a->tc == b->tc) {
					GetTransChars(a, &seta);
					GetTransChars(b, &setb);
					Set_Union(&seta, &setb);
					ChangeTrans(a, &seta);
					DelTrans(s, b);
				}
			}
		}
	}
	Set_Done(&seta);
	Set_Done(&setb);
}

/* return the automata state associated with a syntax graph node */
static int TheState(int gp, int sp)
{
	int s;
	PGraphNode gn;
	PStateNode sn;

	if (gp == 0) {
		s = NewState();
		sn = GetStateP(s);
		CR_ASSERT(sn != NULL);
		sn->end_of = sp;
		return s;
	}
	gn = GetGraphP(gp);
	CR_ASSERT(gn != NULL);
	return gn->STATE;
}

/* walk through the syntax graph to create the automata */
static void Step(int from, int gp, int sp)
{
	int next;
	PGraphNode gn;
	if (gp == 0) return;
	Set_AddItem(&Stepped_Nodes, gp);
	gn = GetGraphP(gp);
	CR_ASSERT(gn != NULL);
	switch (gn->type) {
	case T_CHAR:
	case T_CLASS:
		NewTransition(from, gn, TheState(abs(gn->next), sp));
		break;
	case T_ALT:
		Step(from, gn->INNER, sp);
		Step(from, gn->ALT, sp);
		break;
	case T_OPT:
	case T_REP:
		next = abs(gn->next);
		if (!Set_IsItem(&Stepped_Nodes, next)) Step(from, next, sp);
		Step(from, gn->INNER, sp);
		break;
	}
}

/* walk through the syntax graph to create the automata accepting sp */
static void MakeTrans(int p, int start, int sp)
{
	PGraphNode gn;

	if (p == 0 || Set_IsItem(&Visited_Nodes,p)) return; /* end of Graph */
	Set_AddItem(&Visited_Nodes,p);

	gn = GetGraphP(p);
	CR_ASSERT(gn != NULL);
	if (start) {
		Set_Clean(&Stepped_Nodes);
		Step(gn->STATE, p, sp);
	}
	switch (gn->type) {
	case T_CHAR:
	case T_CLASS:
		MakeTrans(abs(gn->next), TRUE, sp);
		break;
	case T_ALT:
		MakeTrans(gn->INNER, FALSE, sp);
		MakeTrans(gn->ALT, FALSE, sp);
		break;
	case T_OPT:
		MakeTrans(abs(gn->next),TRUE, sp);
		MakeTrans(gn->INNER, FALSE, sp);
		break;
	case T_REP:
		MakeTrans(abs(gn->next), FALSE, sp);
		MakeTrans(gn->INNER, FALSE, sp);
		break;
	}
}

static void NumberNodes(int p, int state, int sp)
{
	PGraphNode gn;

	if (p == 0) return;         /* end of Graph */
	gn = GetGraphP(p);
	CR_ASSERT(gn != NULL);
	if (gn->STATE >= 0) return; /* already visited */
	if (state==-1) state = NewState();
	gn->STATE = state;
	if (IsNullableGraph(p)) {
		PStateNode sn = GetStateP(state);
		CR_ASSERT(sn != NULL);
		sn->end_of = sp;
	}
	switch (gn->type) {
	case T_CHAR:
	case T_CLASS:
		NumberNodes(abs(gn->next), -1, sp);
		break;
	case T_ALT:
		NumberNodes(gn->INNER, state, sp);
		NumberNodes(gn->ALT, state, sp);
		break;
	case T_OPT:
		NumberNodes(abs(gn->next), -1, sp);
		NumberNodes(gn->INNER, state, sp);
		break;
	case T_REP:
		NumberNodes(abs(gn->next), state, sp);
		NumberNodes(gn->INNER, state, sp);
		break;
	}
}

static void InitGraphState(PGraphNode gn)
{
	gn->STATE = -1;
}

/* convert syntax graph to automata */
void ConvertToStates(int gp, int sp)
{
	Collection_ForEach(&nodes_tab, (Collection_Func) InitGraphState);
	CompFollowNode(gp, 0);
	NumberNodes(gp, root_state, sp);
	Set_Init(&Visited_Nodes);
	Set_Init(&Stepped_Nodes);
	MakeTrans(gp, TRUE, sp);
	CleanGraphTab();
	Set_Done(&Visited_Nodes);
	Set_Done(&Stepped_Nodes);
}

/* check the DFA if can match a string */
int MatchDFA(byte *str, int sp)
{
	int matched_sp, s, s1, to, i, len, t;
	int weak_match;
	GraphNode gn;
	PStateNode state;
	PTransNode tp;

	s = root_state;
	i = 1;
	weak_match = 0;
	len = strlen((char *) str) - 1;
	while (1) {
		if (i == len) break;
		s1 = GetTransition(s, str[i]);
		if (s1 == -1) break;
		t  = FindTrans(s,str[i]);
		state = GetStateP(s);
		tp = GetTransP(&state->trans_list, t);
		if (tp->type == T_CLASS) weak_match = 1;
		s = s1;
		i++;
	}

	if (weak_match && (i != len || state->end_of == 0)) {  // Fri  08-30-02
		s = root_state; i=1; dirty_DFA=1;
	}

	while (i < len) {  /* make new DFA from str[i..len-1] */
		to = NewState();
		gn.type = T_CHAR;
		gn.SYMLINK = str[i];
		gn.CONTEXT = T_NORMAL;
		NewTransition(s, &gn, to);
		if (weak_match) {
			weak_match = 0;
			t = FindTrans(s,str[i]);
			state = GetStateP(s);
			tp = GetTransP(&state->trans_list, t);
		}
		s = to;
		i++;
	}

	state = GetStateP(s);
	CR_ASSERT(state != NULL);
	matched_sp = state->end_of;
	if (matched_sp == 0) state->end_of = sp;
	return matched_sp;
}

static void PrintTrans(PTransNode a)
{
	Set set;
	Set_Init(&set);
	if (a->type == T_CHAR) Set_AddItem(&set, a->sym);
	else
		if (a->type == T_CLASS) {
			PClassNode cn = GetClassP(a->sym);
			CR_ASSERT(cn != NULL);
			Set_Union(&set, &cn->data);
		}
	printf("Trans=(Ch=");
	Set_PrintChar(&set);
	printf(",States=");
	Set_PrintInt(&a->to_states);
	printf(")");
	Set_Done(&set);
}

/* generate unique transitions from two overlapping transitions */
static void SplitTrans(int s, PTransNode a, PTransNode b)
{
	TransNode c;
	Set seta, setb, setc;
	c.tc = T_NONE;

	Set_Init(&seta);
	Set_Init(&setb);
	Set_Init(&setc);

	GetTransChars(a, &seta);
	GetTransChars(b, &setb);

#if DEBUG
	printf("SplitTrans(%d,",s);
	PrintTrans(a);
	printf(",");
	PrintTrans(b);
	printf("):\n");
#endif
	
	if (Set_Equal(&seta, &setb)) {
		CombineTrans(a, b);
		DelTrans(s, b);
	} else
		if (Set_Includes(&seta, &setb)) {
			Set_Copy(&setc, &seta);
			Set_Diference(&setc, &setb);
			CombineTrans(b, a);
			ChangeTrans(a, &setc);
		}
		else
			if (Set_Includes(&setb, &seta)) {
				Set_Copy(&setc, &setb);
				Set_Diference(&setc, &seta);
				CombineTrans(a, b);
				ChangeTrans(b, &setc);
			}
			else {
				Set_Copy(&setc, &seta);
				Set_Intersect(&setc, &setb);
				Set_Diference(&seta, &setc);
				Set_Diference(&setb, &setc);
				ChangeTrans(a, &seta);
				ChangeTrans(b, &setb);

				Set_Init(&c.to_states);
				c.tc = T_NONE;
				CombineTrans(&c, a);
				CombineTrans(&c, b);
				ChangeTrans(&c, &setc);
				AddTrans(s, &c);
			}

#if DEBUG
	printf("Result = (");
	PrintTrans(a);

	if (b->tc != T_NONE) {
		printf(",");
		PrintTrans(b);
	}

	if (c.tc != T_NONE) {
		printf(",");
		PrintTrans(&c);
	}
	printf(")\n");
#endif

	Set_Done(&seta);
	Set_Done(&setb);
	Set_Done(&setc);
}

/* make all transitions in the state unique */
static int MakeUnique(int s)
{
	PStateNode state;
	Collection *trans_tab;
	PTransNode a, b;
	int i, j, c, changed = 0;

	state = GetStateP(s);
	CR_ASSERT(state != NULL);
	trans_tab = &(state->trans_list);
	c = Collection_Count(trans_tab);
	for (i = 0; i < c; i++) {
		trans_tab = &(state->trans_list);
		c = Collection_Count(trans_tab);

		a = GetTransP(trans_tab, i);
		CR_ASSERT(a != NULL);
		if (a->type == T_NONE) continue;  /* Trans Deleted??? */
		for(j = i + 1; j < c; j++) {
			trans_tab = &(state->trans_list);
			c = Collection_Count(trans_tab);

			b = GetTransP(trans_tab, j);
			CR_ASSERT(b != NULL);
			if (b->type == T_NONE) continue;  /* Trans Deleted??? */
			if (OverlapTrans(a, b)) {
				SplitTrans(s, a, b);
				changed = 1;
			}
		}
	}
	return changed;
}

/* return a melted state if known */
static int KnownMelt(Set *set)
{
	PMeltedNode melt;
	int i, c = Collection_Count(&melted_tab);

	for (i = 0; i < c; i++) {
		melt = GetMeltedP(i);
		CR_ASSERT(melt != NULL);
		if (Set_Equal(set, &melt->set)) return i;
	}
	return -1;
}

/* add the melted states numbers to 'set' */
static void AddMeltSet(int s, Set *set)
{
	PMeltedNode melt;
	int i, c = Collection_Count(&melted_tab);

	for (i = 0; i < c; i++) {
		melt = GetMeltedP(i);
		CR_ASSERT(melt != NULL);
		if (melt->state==s) {
			 Set_Union(set, &melt->set);
			 break;
		}
	}
}

/* get information about states_set */
static int GetStateSet(Set *state_set, Set *set, int *end_of, int *ctx)
{
	int f, s;
	int correct = TRUE;
	PStateNode state;
	char err[100];

	Set_Clean(set);
	*end_of = 0;
	*ctx = T_NONE;
	Set_GetRange(state_set, &s, &f);
	for ( ;s <= f; s++)
		if (Set_IsItem(state_set, s)) {
			if (s <= last_sim_state) Set_AddItem(set, s);
			else AddMeltSet(s, set);
			state = GetStateP(s);
			CR_ASSERT(state != NULL);
			if (state->end_of != 0) {
				if (*end_of == 0 || *end_of == state->end_of) {
					*end_of = state->end_of;
				} else {
					PTermNode tn1 = GetTermP(*end_of),
					tn2 = GetTermP(state->end_of);
					CR_ASSERT(tn1 != NULL);
					CR_ASSERT(tn2 != NULL);
					sprintf(err, "Tokens %s (%d) and %s (%d) cannot be distinguished.",
					tn1->name, *end_of, tn2->name, state->end_of);
					fprintf(lstfile, "%s\n", err);
					correct = FALSE;
				}
			}
			if (state->ctx != T_NONE) {
				*ctx = T_CONTEXT;
/* Tue  08-27-02 removed following test
				if (state->end_of != 0) {
					PTermNode tn1 = GetTermP(*end_of),
					tn2 = GetTermP(state->end_of);
					CR_ASSERT(tn1 != NULL);
					CR_ASSERT(tn2 != NULL);
					sprintf(err, "Ambiguous CONTEXT clause. Tokens %s (%d) and %s (%d)",
					tn1->name, *end_of, tn2->name, state->end_of);
					fprintf(lstfile, "%s\n", err);
					correct = FALSE;
				}
*/
			}
		}
	return correct;
}

/* copy all the transitions from state_set states to the state 'sn' */
static void FillWithTrans(int sn, Set *state_set)
{
	PTransNode t;
	TransNode t1;
	PStateNode state;
	Collection *trans_tab;
	int i, c, s, f;

	Set_GetRange(state_set, &s, &f);
	for (; s <= f ; s++)
		if (Set_IsItem(state_set, s)) {
			state = GetStateP(s);
			CR_ASSERT(state != NULL);
			trans_tab = &(state->trans_list);
			c = Collection_Count(trans_tab);
			for(i = 0; i < c; i++) {
				t = GetTransP(trans_tab, i);
				CR_ASSERT(t != NULL);
				Collection_Get(trans_tab, i, &t1);
				if (t->type != T_NONE) {
					Set_Init(&t1.to_states);
					Set_Copy(&t1.to_states, &t->to_states);
					AddTrans(sn, &t1);
				}
			}
		}
}

/* melt state_tab appearing with a shift of the same symbol */
static int MeltStates(int s)
{
	int s1, i, c, m, end_of, ctx, change, correct = TRUE;
	PStateNode state, state1;
	PMeltedNode melt;
	Collection *trans_tab;
	Set set;
	PTransNode a;

	Set_Init(&set);
	state = GetStateP(s);
	CR_ASSERT(state != NULL);
	trans_tab = &(state->trans_list);
	c = Collection_Count(trans_tab);
	for (i = 0; i < c; i++) {
		a = GetTransP(trans_tab, i);
		CR_ASSERT(a != NULL);
		if (a->type == T_NONE) continue;  /* Trans Deleted??? */
		if (Set_Elements(&a->to_states) <= 1) continue;  /* Trans to 1 state */
		/* Trans to more than 1 state => melt */
		if (!GetStateSet(&a->to_states, &set, &end_of, &ctx)) correct = FALSE;
		m = KnownMelt(&set);
		if (m == -1) {
			s1 = NewState();
			state1 = GetStateP(s1);
			CR_ASSERT(state1 != NULL);
			state1->end_of = end_of;
			state1->ctx = ctx;
			FillWithTrans(s1, &a->to_states);
			do {
				change = MakeUnique(s1);
			} while (change);
			m = NewMelt(&set, s1);
		}
		melt = GetMeltedP(m);
		CR_ASSERT(melt != NULL);
		Set_Clean(&a->to_states);
		Set_AddItem(&a->to_states, melt->state);
	}
	Set_Done(&set);
	return correct;
}

static void FindCtxStates(void)
{
	PStateNode state, state1;
	PTransNode t;
	Collection *trans_tab;
	int i, c, s, s1;

	for (s = root_state; s <= last_state; s++) {
		state = GetStateP(s);
		CR_ASSERT(state != NULL);
		trans_tab = &(state->trans_list);
		c = Collection_Count(trans_tab);
		for(i = 0; i < c; i++) {
			t = GetTransP(trans_tab, i);
			CR_ASSERT(t != NULL);
			if (t->type == T_NONE) continue;
			if (t->tc == T_CONTEXT) {
				s1 = Set_MinIndex(&t->to_states);
				state1 = GetStateP(s1);
				CR_ASSERT(state1 != NULL);
				state1->ctx = TRUE;
			}
		}
	}
}

/* mark the state reachable from valid transitions */
static int MarkToStates(PCollection trans_tab)
{
	int s, i, c, change;
	PTransNode t;
	PStateNode state;

	change = FALSE;
	c = Collection_Count(trans_tab);
	for(i = 0; i < c ; i++) {
		t = GetTransP(trans_tab, i);
		CR_ASSERT(t != NULL);
		if (t->type == T_NONE) continue;
		s = Set_MinIndex(&t->to_states);
		state = GetStateP(s);
		CR_ASSERT(state != NULL);
		if (state->gen_state_no == -1) change = TRUE;
		state->gen_state_no = 1;
	}
	return change;
}


/* mark all states not reachable from state 0 */
static void DeleteRedundantStates(void)
{
	PStateNode state;
	int s, change;

	state = GetStateP(0);
	CR_ASSERT(state != NULL);
	state->gen_state_no = 0;
	do {
		change = FALSE;
		for (s = root_state; s <= last_state; s++) {
			state = GetStateP(s);
			CR_ASSERT(state != NULL);
			if (state->gen_state_no == -1) continue;
			if (MarkToStates(&state->trans_list)) change = TRUE;
		}
	} while (change);
}

int MakeDeterministic(void)
{
	int s, correct, changed;

	last_sim_state = last_state;

	if (last_state == 0) return TRUE;

	FindCtxStates();
	for (s = root_state; s <= last_state; s++) {
		do {
			changed = MakeUnique(s);
		} while (changed);
	}

	correct = TRUE;
	for (s = root_state; s <= last_state; s++) {
		if (!MeltStates(s)) correct = FALSE;
	}

	CombineShifts();

	return correct;
}

int StrToGraph(byte *name)
{
	int next = 0, i, len, gp, first = 0;
	len = strlen((char *) name);
	for (i = 1; i <= len - 2; i++) {
		gp = MakeGraph(T_CHAR, name[i]);
		if (next) next = LinkGraph(next, gp);
		else first = next = gp;
	}
	return first;
}

/* generate Ignore Chars */
static void GenIgnore(void)
{
	int p;
	PClassNode cn;

	p = FindClass("@ignore_chars");
	cn = GetClassP(p);
	GenCode(fscan, "while (%C) Scan_NextCh();", &cn->data);
}

/* generate comment part in GET*/
static void GenCommentPart(void)
{
	int p;
	PCommentNode n;
	Set set;

	Set_Init(&set);
	p = 0;
	while (p < Collection_Count(&comment_tab)) {
		n = (PCommentNode) Collection_At(&comment_tab, p);
		Set_AddItem(&set, n->start_token[0]);
		p++;
	}
	if (!Set_Empty(&set))
		GenCode(fscan, "if ((%C) && Comment()) goto start;", &set);
	Set_Done(&set);
}

/* generate State0 vector */
static void GenState0(void)
{
	int i, n;

	for (i = 0; i <= 254; i++) {
		if (i % 30 == 0 && i / 30 > 0) GenCode(fscan, "$$");
		n = GetTransition(0, (byte) i);
		if (n < 0) n = 0;
		GenCode(fscan, "%d,", n);
	}
	n = GetTransition(0, 255);
	if (n < 0) n = 0;
	GenCode(fscan, "%d", n);
}

/* generate literal class search */

static void fixname(char *s, char *d)
{
	char temp[MAX_ID_LEN];
	strcpy(temp, s); s = temp;
	s[strlen(s) - 1] = '\0'; s++;
	*d++ = '"';
	while (*s) {
		if (*s == '\\') *d++ = '\\';
		if (*s == '"') *d++ = '\\';
		*d++ = *s++;
	}
	*d++ = '"';
	*d = '\0';
}

static void GenLiterals(void)
{
	char token[MAX_ID_LEN], name[MAX_ID_LEN];
	int i, p, start;
	PTermNode n;

	for (i = 33; i < 127; i++) {
		p = 0;
		start = 0;
		while (p < term_tab.el_free) {
			n = GetTermP(p);
			CR_ASSERT(n != NULL);
			if (n->type == T_LITTOKEN && n->name[1] == i) {
				GetTermName(p, token);
				if (start == 0) {
					if (i != '\\') GenCode(fscan, "$1case '%c':$$", i);
					else GenCode(fscan, "$1case %d:$$", i);
				}
				fixname(n->name, name);
				GenCode(fscan, "$2if (EqualStr(%s)) return %s;$$", name, token);
				start = 1;
			}
			p++;
		}
		if (start) GenCode(fscan, "$2break;$$");
	}
}

/* generate comment function */
static void GenCommentBody(PCommentNode n)
{
	CR_ASSERT(n != NULL);
	GenCode(fscan, "$2while (1) {$$"
			"$3if (Scan_Ch== %#) { /* 5 */$$", n->end_token[0]);
	if (n->end_token[1] == 0) {
		GenCode(fscan, "$4Level--; Scan_NextCh(); Scan_ComEols = Scan_CurrLine - StartLine;$$"
				"$4if(Level == 0) return 1;$$");
	} else {
		GenCode(fscan, "$4Scan_NextCh();$$"
				"$4if (Scan_Ch == %#) { /* 6 */$$", n->end_token[1]);
		GenCode(fscan, "$5Level--; Scan_NextCh(); Scan_ComEols = Scan_CurrLine - StartLine;$$"
				"$5if(Level == 0) return 1;$$"
				"$4} /* 6 */ $$");
	}
	if (n->nested) {
		GenCode(fscan, "$3} else  /* 5 */$$");
		GenCode(fscan, "$3if (Scan_Ch == %#) {$$", n->start_token[0]);
		if (n->start_token[1] == 0) {
			GenCode(fscan, "$4Level++; Scan_NextCh();$$");
		} else {
			GenCode(fscan, "$4Scan_NextCh();$$");
			GenCode(fscan, "$4if (Scan_Ch == %#) { Level++; Scan_NextCh(); }$$", n->start_token[1]);
		}
	}
	GenCode(fscan, "$3} else /* 5 */$$");
	GenCode(fscan, "$3if (Scan_Ch == EOF_CHAR) return 0;$$$3else Scan_NextCh();$$");
	GenCode(fscan, "$2} /* while */$$");
}

/* generate comment */
static void GenComment(PCommentNode n)
{
	CR_ASSERT(n != NULL);
	GenCode(fscan, "if (Scan_Ch == %#) { /* 1 */$$", n->start_token[0]);
	if (n->start_token[1] == 0) {
		GenCode(fscan, "$1Scan_NextCh();$$");
		GenCommentBody(n);
	} else {
		GenCode(fscan, "$1Scan_NextCh();$$"
				"$1if (Scan_Ch == %#) { /* 2 */$$"
				"$2Scan_NextCh();$$", n->start_token[1]);
		GenCommentBody(n);
		GenCode(fscan, "$1} else { /* 2 */$$");
		GenCode(fscan, "$2if (Scan_Ch == LF_CHAR) { Scan_CurrLine--; Scan_LineStart = OldLineStart; }$$");
		GenCode(fscan, "$2Scan_BuffPos -= 2; Scan_CurrCol = OldCol - 1; Scan_NextCh();$$");
		GenCode(fscan, "$1} /* 2 */$$");
	}
	GenCode(fscan, "} /* 1*/$$");
}

/* make the scanner */

/* generate State Transition */
static void GenStateTrans(int sn, PTransNode t, int is_context)
{
	int to;
	Set set1;

	CR_ASSERT(t != NULL);
	Set_Init(&set1);
	if (t->type == T_CHAR) Set_AddItem(&set1, t->sym);
	if (t->type == T_CLASS) {
		PClassNode cn = GetClassP(t->sym);
		Set_Copy(&set1, &cn->data);
	}
	to = Set_MinIndex(&t->to_states);

	GenCode(fscan, "$1if (%C)",&set1);

	if (t->tc == T_CONTEXT) { /* Transition With Context */
		if (to != sn)  GenCode(fscan, " { ctx++; state = %d;} else$$", to);
		else GenCode(fscan, " ctx++; else$$");
	} else {
		if (is_context) { /* Normal Transition in Context State => Reset Context*/
			if (to != sn)  GenCode(fscan, " {ctx = 0; state = %d;} else$$", to);
			else GenCode(fscan, " ctx = 0; else$$");
		} else {  /* Normal Transition */
			if (to != sn)  GenCode(fscan, " state = %d; else$$", to);
			else GenCode(fscan, " /*same state*/; else$$");
		}
	}
	Set_Done(&set1);
}

/* generate accepting token for a state */
static void GenStateAccept(int token, int is_context)
{
	PTermNode tn;

	if (is_context)
		GenCode(fscan, "$1{$$"
				"$2Scan_NextLen -= ctx; Scan_BuffPos -= ctx+1; Scan_CurrCol -= ctx+1; Scan_NextCh(); $$");

	tn = GetTermP(token);
	CR_ASSERT(tn != NULL);
	if (tn->type == T_CLASSLITTOKEN)
		GenCode(fscan, "%Ireturn CheckLiteral(%T);$$", 1 + is_context, token);
	else {
		if (token == 0) GenCode(fscan, "%Ireturn No_Sym;$$", 1 + is_context);
		else GenCode(fscan, "%Ireturn %T;$$", 1 + is_context, token);
	}

	if (is_context) GenCode(fscan, "$1}$$");
}

/* generate state */
static void GenState(PStateNode state, int sn)
{
	int i, c, is_context, t_count;
	Collection *trans_tab;
	PTransNode t;

	CR_ASSERT(state != NULL);
	if (state->gen_state_no == -1) return; /* unused stated */
	is_context = (state->ctx == T_CONTEXT);

	if (sn == 0) GenCode(fscan, " /* State 0; valid STATE0 Table$$");
	GenCode(fscan, "case %d:$$", sn);
	trans_tab = &state->trans_list;
	c = Collection_Count(trans_tab);
	t_count = 0;
	for (i = 0; i < c; i++) {
		t = GetTransP(trans_tab, i);
		CR_ASSERT(t != NULL);
		if (t->type == T_NONE) continue;
		GenStateTrans(sn, t, is_context);
		t_count++;
	}
	is_context = (state->ctx == T_CONTEXT && t_count == 0);
	i = state->end_of;
	if (i >= FIRST_PRAGMA) i = (i - FIRST_PRAGMA) + no_sym + 1;
	GenStateAccept(i, is_context);
	if (t_count) GenCode(fscan, "$1break;$$");
	if (sn == 0) GenCode(fscan, " --------- End State0 --------- */$$");
}

void MakeScanner(void)
{
	DeleteRedundantStates();
}

/* generate scanner */
int GenScannerOptions(FILE *Out, char *option)
{
	fscan = Out;
	if (!stricmp(option, "IgnoreCase")) {
		GenCode(fscan, "%d", ignore_case);
	} else
	if (!stricmp(option, "State0")) {
		GenState0();
	} else
	if (!stricmp(option, "Literals")) {
		GenLiterals();
	} else
	if (!stricmp(option, "Comment")) {
		Collection_ForEach(&comment_tab, (Collection_Func) GenComment);
	} else
	if (!stricmp(option, "GetDFA")) {
		Collection_ForEachPos(&state_tab, (Collection_FuncPos) GenState);
	} else
	if (!stricmp(option, "GetIgnore")) {
		GenIgnore();
	} else
	if (!stricmp(option, "GetComment")) {
		GenCommentPart();
	} else return 0;

	return 1;
}

/* generate header file */
static void Func_GenHeaderItem(PTermNode n, int p)
{
	char s[MAX_ID_LEN];
	GetTermName(p, s);
	fprintf(fhead, "#define %s\t%d\t/* %s */\n", s, p, n->name);
}

void GenScannerTokens(FILE *Out)
{
	fhead = Out;
	Collection_ForEachPos(&term_tab, (Collection_FuncPos) Func_GenHeaderItem);
	fprintf(fhead, "#define %s\t%s\t/* %s */\n", "MAXT", "No_Sym", "Max Terminals");
}

/* initialize scanner generator tables */
void InitScannerTab(void)
{
	Collection_Init(&state_tab, sizeof(StateNode), 50, 10);
	Collection_Init(&melted_tab, sizeof(MeltedNode), 10, 5);
	Collection_Init(&comment_tab, sizeof(CommentNode), 10, 5);
	last_state = -1;
	root_state = NewState();
}

/* destroy scanner generator tables */
static void Func_DoneState(PStateNode sn)
{
	DoneState(sn);
}

static void Func_DoneMelt(PMeltedNode sn)
{
	Set_Done(&sn->set);
}

void DoneScannerTab(void)
{
	Collection_ForEach(&state_tab, (Collection_Func) Func_DoneState);
	Collection_ForEach(&melted_tab, (Collection_Func) Func_DoneMelt);
	Collection_Done(&state_tab);
	Collection_Done(&melted_tab);
	Collection_Done(&comment_tab);
}

/* dump state information to list file */
static void ShowState(PStateNode state, int sn)
{
	Name name;
	int i, c, tok;
	Collection *trans_tab;
	PTransNode t;
	Set set1;

	if (state->gen_state_no == -1) return;
	fprintf(lstfile, "\n\nState %d: ", sn);
	if (state->ctx) fprintf(lstfile, " Context State");
	fprintf(lstfile, "\n");
	Set_Init(&set1);
	trans_tab = &state->trans_list;
	c = Collection_Count(trans_tab);
	for (i = 0; i < c; i++) {
		t = GetTransP(trans_tab, i);
		CR_ASSERT(t != NULL);
		if (t->type == T_NONE) continue;
		Set_Clean(&set1);
		if (t->type == T_CHAR) Set_AddItem(&set1, t->sym);
		if (t->type == T_CLASS) {
			PClassNode cn = GetClassP(t->sym);
			Set_Copy(&set1, &cn->data);
		}

		fprintf(lstfile, "\t");
		Set_ForEach(&set1, (Set_Func) PrintAscii);
		fprintf(lstfile, " -> ");
		Set_ForEach(&t->to_states, (Set_Func) PrintInt);

		if (t->tc == T_CONTEXT) fprintf(lstfile, "  CONTEXT Trans");
		fprintf(lstfile, "\n");
	}

	tok = state->end_of;
	if (tok >= FIRST_PRAGMA) tok = (tok - FIRST_PRAGMA) + no_sym + 1;
	if (tok) GetTermName(tok, name);
	else strcpy(name, "No_Sym");
	fprintf(lstfile, "\tAccept Token:  %s (%d)\n", name, tok);
	Set_Done(&set1);
}

void ShowDFA(void)
{
	fprintf(lstfile, "\n\n\nDFA:\n");
	Collection_ForEachPos(&state_tab, (Collection_FuncPos) ShowState);
}


