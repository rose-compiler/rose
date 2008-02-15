#include "set.h"
#include "collect.h"
#include "crx.h"
#include "crt.h"
#include "crf.h"
#include "crs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define  MAX_ALT_IF     3
#define  isspacetab(c)  (c == ' ' || c == '\t')

static FILE *fparser, *fherror;
static int ErrorNo, CurrentNt;

static char temp_str[250];

/* add error msg to error file */
static void DeclareError(char *Msg)
{
	fprintf(fherror, "         \"%s\",\n", Msg);
	ErrorNo++;
}

static char *GetFormatLine (char *s, char *line)
{
	char ch;
	int i;
	int col = 1;

	ch = *s;
	if (ch == 0) return NULL;

	while (ch != LF_CHAR && ch != 0) {
		if (ch == TAB_CHAR) {
			i = TAB_SIZE - (col % TAB_SIZE);
			while (i-- >= 0) {*line++ = ' ';col++;}
		} else if (ch != CR_CHAR) {*line++ = ch; col++;}
		s++;
		ch = *s;
	}

	*line = 0;
	if (ch != 0) return s + 1;
		else return s;
}

/* generate semantic action code */
static void GenSemCode(int tab, long pos, int len, int line, int col, int gencomma)
{
	int i;
	char *buff, *s, *sb;
	int Comma = FALSE, firstline = 1;

	if (D_option) fprintf(fparser, "# line %d \"%s\"\n", line, source_name);

	if (pos == -1) { /* a complete null alternative */
		GenCode(fparser, "%I/* Empty */$$", tab);
		return;
	}

	sb = buff = (char *) malloc(len + 2);
	S_GetString(pos, len, buff, len);

	/* remove leading and trailing spaces. PT */
	while (*sb == ' ') { sb++; col++; }
	while (len > 0 && buff[len - 1] == ' ') len--;
	buff[len] = '\0';

	while ((sb = GetFormatLine(sb, temp_str)) != NULL) {
		s = temp_str;
		if (!firstline) {
			GenCode(fparser, "$$");
			i = col - 1;  /* skip spaces (max col + 2) */
			while (i-- && isspacetab(*s)) s++;
		}

		GenCode(fparser, "%I", tab);
		while (*s != 0) {
			GenCode(fparser, "%c", *s);
			if (*s == ';') Comma = FALSE; else
			if (*s != ' ') Comma = TRUE;
			s++;
		}
		firstline = 0;
	}
	if (Comma && gencomma) GenCode(fparser, ";");
	GenCode(fparser, "$$");
	free(buff);
}

/* generate attribute code */
static void GenAttrCode(int tab, int gen_col, long pos, int len, int c)		/*kws*/
{
	char *buff, *s;
	int i;

	s = buff = (char *) malloc(len + 2);
	S_GetString(pos, len, buff, len);
	buff[len] = '\0';

	do {
		while (isspacetab(*s)) s++;
		while (*s != 0 && *s != '\n') {
			if (*s != '\r') GenCode(fparser, "%c", *s);
			s++;
		}
		if (*s == '\n') {
			GenCode(fparser, "$$%I", tab);
			i = gen_col;
			while (i--) GenCode(fparser, " ");
			s++;
		}
	} while (*s);
	free(buff);
}


/* generate function header */
static void GenFuncHeader(PNTermNode n, int header_only)
{
	PGraphNode p;

	if (!header_only && D_option)
		fprintf(fparser, "# line %d \"%s\"\n", n->line_dec, source_name);

	if (GenCplusplus) {
		if (header_only) GenCode(fparser, "void %s(", n->name);
		else GenCode(fparser, "void %sParser::%s(", module_prefix, n->name);
	}
	else GenCode(fparser, "static void %s(", n->name);

	if (n->has_attr) {
		p = GetGraphP(n->attr);
		GenAttrCode(0, strlen(n->name) + 14, p->SEMPOS, p->SEMLEN, p->SEMCOL);
	}
	else if (!GenCplusplus) GenCode(fparser, "void"); ///++++
	if (header_only) {
		GenCode(fparser, ");$$");
		return;
	}
	GenCode(fparser, ")$$");
	if (n->sem != NIL) {
		p = GetGraphP(n->sem);
		GenSemCode(4, p->SEMPOS, p->SEMLEN, p->SEMLINE, p->SEMCOL + 2, TRUE);
	}
	GenCode(fparser, "{$$");
}

/* generate function trailer */
static void GenFuncTrailer(void)
{
	GenCode(fparser, "}$$$$");
}

/* return number of alternatives in node 'n' */
static int CompAltCount(int n)
{
	PGraphNode p;
	int c = 0;

	while (n) {
		c++;
		p = GetGraphP(n);
		n = p->ALT;
	}
	return c;
}

/* generate function graph */
static void GenFuncGraph(int gp, int tab, Set *chequed)
{
	PGraphNode gn;
	Set s1, Oldchequed;

	Set_Init(&s1);
	Set_Init(&Oldchequed);
	Set_Union(&Oldchequed, chequed);

	while (gp > NIL) {
		gn = GetGraphP(gp);
		if (D_option &&
				gn->type != T_SEM)
			fprintf(fparser, "# line %d \"%s\"\n",
					gn->SLine, source_name);

		switch(gn->type) {
		case T_T :
			if (Set_IsItem(chequed, gn->SYMLINK))
				GenCode(fparser, "%IGet();$$", tab);
			else
				GenCode(fparser, "%IExpect(%T);$$", tab, (int)gn->SYMLINK); /*kws*/
			break;
		case T_WT:
			GenCode(fparser, "%IExpectWeak(%T,%d);$$", tab, (int)gn->SYMLINK,/*kws*/
					(int)gn->SETLINK1 - first_weak_set + 1);				/*kws*/
			break;
		case T_NT :
			GenCode(fparser, "%I%N(", tab, (int)gn->SYMLINK);				/*kws*/
			if (gn->ALT != NIL) {
				PGraphNode gn1 = GetGraphP(gn->ALT);
				if (gn1->type == T_ATTR) {
					PNTermNode ntn = GetNTermP(gn->SYMLINK);
					GenAttrCode(tab, strlen(ntn->name) + 1, gn1->SEMPOS, gn1->SEMLEN, gn1->SEMCOL);
				}
			}
			GenCode(fparser, ");$$");
			break;

		case T_OPT:
			Set_Clean(&s1);
			CompFirstSet(gn->INNER, &s1);
			if (Set_Equal(chequed, &s1)) GenFuncGraph(gn->INNER, tab, &s1);
			else {
				GenCode(fparser, "%Iif (%S) {$$", tab, &s1);
				GenFuncGraph(gn->INNER, tab + 1, &s1);
				GenCode(fparser, "%I}$$", tab);
			}
			break;
		case T_REP:
			{
				PGraphNode gn1 = GetGraphP(gn->INNER);
				if (gn1->type == T_WT) {
					Set_Clean(&s1);
					GenCode(fparser, "%Iwhile (WeakSeparator(%T,%d,%d)) {$$", tab,
							(int)gn1->SYMLINK,								/*kws*/
							gn1->SETLINK1-first_weak_set + 1,
							gn1->SETLINK2-first_weak_set + 1);
					GenFuncGraph(gn1->next, tab + 1, &s1);
				} else {
					Set_Clean(&s1);
					CompFirstSet(gn->INNER, &s1);
					GenCode(fparser, "%Iwhile (%S) {$$", tab, &s1);
					GenFuncGraph(gn->INNER, tab + 1, &s1);
				}
				GenCode(fparser, "%I}$$", tab);
				break;
			}
		case T_ALT:
			{
				Set symdone;
				int gen_err = TRUE;
				int gp1;
				PGraphNode gn1;
				int gen_case;

				Set_Init(&symdone);
				gen_case = CompAltCount(gp) > MAX_ALT_IF;
				if (gen_case) GenCode(fparser, "%Iswitch (Sym) {$$", tab);
				gp1 = gp;
				while (gp1) {
					gn1 = GetGraphP(gp1);
					if (IsNullableGraph(gn1->INNER)) gen_err = FALSE;
					Set_Clean(&s1);
					CompExpected(gn1->INNER, CurrentNt, &s1);
					Set_Diference(&s1, &symdone);
					Set_Union(&symdone, &s1);

					if (gen_case) GenCode(fparser, "%I%X $$", tab + 1, &s1);
					else {
						if (gp1 == gp) GenCode(fparser, "%Iif (%S) {$$", tab, &s1);
						else GenCode(fparser, "%I} else if (%S) {$$", tab, &s1);
					}
					CompExpected(gn1->INNER, CurrentNt, &s1);
					Set_Union(&s1, chequed);
					GenFuncGraph(gn1->INNER, tab + 1 + gen_case, &s1);
					if (gen_case) GenCode(fparser, "%Ibreak;$$", tab + 2);
					gp1 = gn1->ALT;
				}

				if (!gen_err && gn->next <= NIL) gen_err = TRUE;
				if (gen_case) {
					if (gen_err) GenCode(fparser, "%Idefault :GenError(%d); break;$$", tab+1, ErrorNo);
					GenCode(fparser, "%I}$$", tab);
				} else {
					if (gen_err) GenCode(fparser, "%I} else GenError(%d);$$", tab, ErrorNo);
					else GenCode(fparser, "%I}$$", tab);
				}

				if (gen_err) ErrorNo++;
				Set_Done(&symdone);
				break;
			}
		case T_SEM:
			GenSemCode(tab, gn->SEMPOS, gn->SEMLEN, gn->SEMLINE,
				gn->SEMCOL + 2, TRUE);
			break;
		case T_ANY :
			GenCode(fparser, "%IGet();$$", tab);
			break;
		case T_SYNC:
			{
				PSymSetNode ssn = GetSymSetP(gn->SETLINK1);
				GenCode(fparser, "%Iwhile (!(%S)) { GenError(%d); Get(); }$$",
						tab, &ssn->set, ErrorNo);
				ErrorNo++;
				break;
			} /* case */
		} /* switch */
		if (gn->type != T_SEM && gn->type != T_SYNC) Set_Clean(chequed);
		gp = gn->next;
	}

	Set_Clean(chequed);
	Set_Union(chequed, &Oldchequed);
	Set_Done(&Oldchequed);
	Set_Done(&s1);
}

/* generate body of function */
static void GenParseBody(PNTermNode ntn, int ntp)
{
	Set chequed;
	if (ntp == 0) return;
	Set_Init(&chequed);
	CurrentNt = ntp;
	GenFuncHeader(ntn, 0);
	GenFuncGraph(ntn->graph, 1, &chequed);
	GenFuncTrailer();
	Set_Done(&chequed);
}

/* generate function Error Graph */
static void GenFuncErrorGraph(int gp)
{
	PGraphNode gn;
	while (gp > NIL) {
		gn = GetGraphP(gp);
		switch(gn->type) {
		case T_OPT:
			GenFuncErrorGraph(gn->INNER);
			break;
		case T_REP:
			{
			PGraphNode gn1 = GetGraphP(gn->INNER);
			if (gn1->type == T_WT) GenFuncErrorGraph(gn1->next);
			else GenFuncErrorGraph(gn->INNER);
			break;
			}
		case T_ALT:
			{
			int gen_err = TRUE;
			int gp1;
			PGraphNode gn1;
			gp1 = gp;
			while (gp1) {
				gn1 = GetGraphP(gp1);
				if (IsNullableGraph(gn1->INNER)) gen_err = FALSE;
				GenFuncErrorGraph(gn1->INNER);
				gp1 = gn1->ALT;
			}

			if (!gen_err && gn->next <= NIL) gen_err = TRUE;
			if (gen_err) {
				PNTermNode ntn = GetNTermP(CurrentNt);
				sprintf(temp_str, "unexpected symbol in %s", ntn->name);
				DeclareError(temp_str);
			}
			break;
			}
		case T_SYNC:
			{
			PNTermNode ntn = GetNTermP(CurrentNt);
			sprintf(temp_str, "unexpected symbol in %s", ntn->name);
			DeclareError(temp_str);
			break;
			} /* case */
		} /* switch */
		gp = gn->next;
	}
}

static void GenParseError(PNTermNode ntn, int ntp)
{
	if (ntp == 0) return;
	CurrentNt = ntp;
	GenFuncErrorGraph(ntn->graph);
}

/* generate header only */
static void GenParseHeader(PNTermNode ntn, int ntp)
{
	if (ntp == 0) return;
	GenFuncHeader(ntn, 1);
}

/* generate terminal error */
static void GenTermError(PTermNode tn)
{
	char temp[MAX_ID_LEN], temp2[MAX_ID_LEN + 50], temp3[MAX_ID_LEN + 50];
	strcpy(temp, tn->name);
	if (temp[0] == '"' || temp[0] == '\'') {
		int i, j = 2, l = strlen(temp);
		temp2[0] = '\\'; temp2[1] = '\"';
		for (i = 1; i < l-1; i++) {
			if (temp[i] == '\"') { temp2[j] = '\\'; j++; }
			if (temp[i] == '\'') { temp2[j] = '\\'; j++; }
			if (temp[i] == '\\') { temp2[j] = '\\'; j++; }
			temp2[j] = temp[i]; j++;
		}
		temp2[j] = '\\'; temp2[j+1] = '\"'; temp2[j+2] = '\0';
		sprintf(temp3, "%s expected", temp2);
	}
	else sprintf(temp3, "%s expected", temp);
	DeclareError(temp3);
}

/* generate pragma code */
static void GenPragma(PPragmaNode pn, int pp)
{
	if (pn->has_attr == 0) return;
	GenCode(fparser, "if (Sym == %T) { /*%d*/$$", pp + no_sym + 1, pp + no_sym + 1);
	GenSemCode(1, (long) pn->sem_text.pos, pn->sem_text.len,
			pn->sem_text.line, pn->sem_text.col, TRUE);
	GenCode(fparser, "} else$$");
}

static int GetSymMax(void)
{
	return Set_MaxIndex(&ALL_TERMS) / SET_NBITS + 1;
}

/* generate symbol sets code */
static void GenSymSet(PSymSetNode ssn, int ssp)
{
	int i, c, max;
	if (ssp> 0 && ssn->type != T_WT) return;
	max = GetSymMax();

	GenCode(fparser, "/*");
	Set_GetRange(&ssn->set, &i, &c);
	for (; i <= c; i++)
		if (Set_IsItem(&ssn->set, i)) GenCode(fparser, "%T ", i);
	GenCode(fparser, "*/$$");

	GenCode(fparser, "{");
	c = ssn->set.size;
	for (i = 0; i < c; i++) {
		GenCode(fparser, "0x%x", ssn->set.data[i]);
		if (i < max - 1) GenCode(fparser, ",");
	}
	for (; i < max; i++) {
		GenCode(fparser, "0x0");
		if (i < max - 1) GenCode(fparser, ",");
	}
	GenCode(fparser, "},$$");
}

void GenParserErrors(FILE *Out)
{
	fherror = Out;
	ErrorNo = 0;
	Collection_ForEach(&term_tab, (Collection_Func) GenTermError);
	ErrorNo = Collection_Count(&term_tab);
	Collection_ForEachPos(&nterm_tab, (Collection_FuncPos) GenParseError);
}

/* generate parser */
int GenParserOptions(FILE *Out, char *option)
{
	fparser = Out;

	if (!stricmp(option, "UserDeclarations")) {
		if (global_defs.pos>0) {
			GenSemCode(0, global_defs.pos,
			global_defs.len,
			global_defs.line, 1, FALSE);
		}
	} else
	if (!stricmp(option, "Declarations")) {
		GenCode(fparser, "#define MAXSYM\t\t%d$$", GetSymMax());
	} else
	if (!stricmp(option, "MaxSymSets")) {
		GenCode(fparser, "%d", GetSymMax());
	} else
	if (!stricmp(option, "MaxErr")) {
		GenCode(fparser, "%d", ErrorNo);
	} else

	if (!stricmp(option, "SymSets")) {
		Collection_ForEachPos(&symset_tab, (Collection_FuncPos) GenSymSet);
	} else
	if (!stricmp(option, "Pragmas")) {
		Collection_ForEachPos(&pragma_tab, (Collection_FuncPos) GenPragma);
		GenCode(fparser, "/* Empty Stmt */ ;");
	} else
	if (!stricmp(option, "ProdHeaders")) {
		Collection_ForEachPos(&nterm_tab, (Collection_FuncPos) GenParseHeader);
	} else
	if (!stricmp(option, "Productions")) {
		ErrorNo = Collection_Count(&term_tab);
		Collection_ForEachPos(&nterm_tab, (Collection_FuncPos) GenParseBody);
	} else
	if (!stricmp(option, "ParseRoot")) {
		GenCode(fparser, "%s();", compiler_name);
	} else return 0;

	return 1;
}

static void downshift(char *s)
{
	while (*s) {
		if (*s >= 'A' && *s <= 'Z') *s += 32;
		s++;
	}
}

/* make parser */
void MakeParser(void)
{
	PNTermNode nt;
	int i;

	strcpy(module_prefix, compiler_name);
	module_prefix[6] = '\0';
	downshift(module_prefix);
	i = FindNTerm(compiler_name);
	nt = GetNTermP(i);
	Set_AddItem(&nt->follow, 0); /* Follow(StartSymbol) = EOF */
	CompSymbolSets();
}



