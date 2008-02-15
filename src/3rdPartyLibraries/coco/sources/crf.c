/* Frames */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "crt.h"
#include "crf.h"

char compiler_name[MAX_ID_LEN];
char module_prefix[MAX_ID_LEN];
char source_name[100];

static char GEN_PREFIX[100];     /* prefix line string used in GenCode */
static int CurrTab;
static int CurrCol;

int GenParserOptions(FILE *Out, char *option);
int GenScannerOptions(FILE *Out, char *option);
void GenParserErrors(FILE *Out);
void GenScannerTokens(FILE *Out);

#define NAMELEN  20

static char Scan_C_Format[NAMELEN];
static char Scan_H_Format[NAMELEN];
static char Parser_C_Format[NAMELEN];
static char Parser_H_Format[NAMELEN];
static char Compiler_Format[NAMELEN];

void InitFrameVars(void)
{
	if (getenv("CRFRAMES"))
		strcpy(Frames_Path, getenv("CRFRAMES"));

	if (getenv("CRHEXT")) {
		strncpy(h_ext, getenv("CRHEXT"), 5); h_ext[5] = '\0';
	}

	if (getenv("CRCEXT")) {
		strncpy(c_ext, getenv("CRCEXT"), 5); c_ext[5] = '\0';
	}
}

void SetupFrameVars(void)
{
		if (strlen(Frames_Path) > 0) {
#if __MSDOS__ || MSDOS || WIN32 || __WIN32__
			if (GenCplusplus) strcat(Frames_Path, "\\cplus2\\");
			else strcat(Frames_Path, "\\");
#else
			if (GenCplusplus) strcat(Frames_Path, "/cplus2/");
			else strcat(Frames_Path, "/");
#endif
		}

	sprintf(Scan_C_Format, "%%ss.%s", c_ext);
	sprintf(Scan_H_Format, "%%ss.%s", h_ext);
	sprintf(Parser_C_Format, "%%sp.%s", c_ext);
	sprintf(Parser_H_Format, "%%sp.%s", h_ext);
	sprintf(Compiler_Format, "%%s.%s", c_ext);
}

/* get next character from a frame */
static int CopyNextCh(FILE *frame)
{
	int c;

	c = fgetc(frame);
	if (c == 10) strcpy(GEN_PREFIX, "");
	else if (c == '\t') strcat(GEN_PREFIX, "\t");
	else strcat(GEN_PREFIX, " ");
	return c;
}

/* reset line prefix */
static void CopyFrameReset(void)
{
	strcpy(GEN_PREFIX, "");
}

/* copy from "frame" to "out" until an option (-->) is found */

static void CopyFrame(FILE *frame, FILE *out, char *option)
{
	char *s;
	int c;

	s = option;
	while (1) {
		c = CopyNextCh(frame);
		/* find first '-' */
		while (c != EOF && c != '-') {
			fputc(c, out);
			c = CopyNextCh(frame);
		}
		if (c == EOF) break;
		/* find second '-' */
		c = CopyNextCh(frame);
		if (c != '-') {
			fputc('-', out);
			fputc(c, out);
			continue;
		}
		/* find '>' */
		c = CopyNextCh(frame);
		if (c != '>') {
			fputc('-', out);
			fputc('-', out);
			fputc(c, out);
			continue;
		}
		/* found '-->' */

		c = fgetc(frame);
		while (c >= 'A' && c <= 'Z' ||
				c >= 'a' && c <= 'z' ||
				c >= '0' && c <= '9') {
			*s++ = c;
			c = fgetc(frame);
		}
		*s = '\0';
		GEN_PREFIX[strlen(GEN_PREFIX) - 3] = '\0';
		ungetc(c, frame);
		return;
	}
	strcpy(option, "ENDFILE");
}

/* generate a Case Terminal Set Condition */
static void GenCaseTermSet(FILE *out, Set *source)  /* OR only version */
{
	int col = CurrCol, tab = CurrTab;
	int c, i, t, first = 0;

	if (Set_Empty(source)) {
		GenCode(out, "default:");
		return;
	}

	Set_GetRange(source, &i, &c);
	for (; i <= c; i++) {
		if (Set_IsItem(source, i)) {
			if (first) {
				GenCode(out, "$$%I", tab);
				t = col;
				while(t--) putc(' ', out);
			}
			GenCode(out, "case %T: ", i);
			first++;
		}
	}
	Set_Clean(source);
}

/* generate a Terminal Set Condition */
static void GenTermSetOR(FILE *out, Set *source)  /* OR only version */
{
	int col = CurrCol, tab = CurrTab;
	int c, i, first, t;

	if (Set_Empty(source)) {
		GenCode(out, "1");
		return;
	}

	first = 1;
	Set_GetRange(source, &i, &c);
	for (; i <= c; i++) {
		if (Set_IsItem(source, i)) {
			if (!first) {
				GenCode(out, " ||$$%I", tab);
				t = col;
				while(t--) putc(' ', out);
			}
			GenCode(out, "Sym == %T", i);
			first = 0;
		}
	}
	Set_Clean(source);
}

/* generate a terminal set condition */
static void GenTermSet(FILE *out, Set *source)
{
	int col = CurrCol, tab = CurrTab;
	int c, i, j, t, first;

	if (Set_Empty(source)) {
		GenCode(out, "1");
		return;
	}

	first = 1;
	Set_GetRange(source, &i, &c);
	do {
		for (; i <= c; i++) if (Set_IsItem(source, i)) break;
		if (i > c) break;
		for (j = i; j <= c && Set_IsItem(source, j); j++);
		j--;

		if (!first) {
			GenCode(out, " ||$$%I", tab);
			t = col;
			while(t--) putc(' ', out);
		}
		if (i == j) GenCode(out, "Sym == %T", i);
		else GenCode(out, "Sym >= %T && Sym <= %T", i, j);
		i = j + 1;
		first = 0;
	} while (1);
}

/* generate a Caracter Set condition */
static void GenCharSet(FILE *out, Set *set)
{
	int col = CurrCol, tab = CurrTab;
	int i, c, j, t, count;

	Set_GetRange(set, &i, &c);
	count = 0;
	do {
		for (; i <= c; i++) if (Set_IsItem(set, i)) break;
		if (i>c) break;
		for (j = i; j <= c && Set_IsItem(set, j); j++);
		j--;

		if (count) {
			GenCode(out, " ||$$%I", tab);
			t = col;
			while(t--) putc(' ', out);
		}
		if (i == j) GenCode(out, "Scan_Ch == %#", i);
		else GenCode(out, "Scan_Ch >= %# && Scan_Ch <= %#", i, j);
		i = j + 1;
		count++;
	} while (1);
}

/* Gen_Code: (Same as fprintf)
		$$ ==> New Line
		$n ==> tabs(n)
		%T ==> Terminal Name
		%N ==> No Terminal Name
		%# ==> char
		%S ==> Terminals Set (Sym == X ..)
		%X ==> Terminals Set (Case X: ..)
		%C ==> Character Set (S_ch == X ...)
*/
void GenCode(FILE *out, char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	for ( ; *fmt; fmt++)
		if (*fmt == '%')
			switch (*++fmt) {
			case 'd':
				fprintf(out, "%d", va_arg(ap, int));
				break;
			case 'x':
				fprintf(out, "%X", va_arg(ap, int));
				break;
			case 'c':
				fprintf(out, "%c", va_arg(ap, int));
				break;
			case 's':
				fputs(va_arg(ap, char *), out);
				break;
			case '#':
				{
					int n = va_arg(ap, int);
					if (n < 32 ||
							n == '\\' ||
							n == '\'' ||
							n > 126) fprintf(out, "%d", n);
					else fprintf(out, "'%c'", n);
					break;
				}
			case 'T':
				{  /* Terminal */
					Name s;
					GetTermName(va_arg(ap, int), s);
					fprintf(out, "%s", s);
					break;
				}
			case 'N':
				{ /* NonTerminal */
					PNTermNode ntn = GetNTermP(va_arg(ap, int));
					fprintf(out, "%s", ntn->name);
					break;
				}
			case 'S':
				{ /* NonTerminal Set */
					Set *s = va_arg(ap, Set *);
					if (O_option) GenTermSetOR(out, s);
					else GenTermSet(out, s);
					break;
				}
			case 'X':
				{ /* NonTerminal Set */
					Set *s = va_arg(ap, Set *);
					GenCaseTermSet(out, s);
					break;
				}
			case 'C':
				{ /* Character Set */
					Set *s = va_arg(ap, Set *);
					GenCharSet(out, s);
					break;
				}
			case 'I':
				{ /*indent */
					int n = va_arg(ap, int);
					CurrTab = n;
					while (n-- > 0) putc('\t', out);
					break;
				}
			default:
				{
					putc('%', out);
					putc(*fmt, out);
					break;
				}
			}
		else
			if (*fmt == '$') {
				switch (*++fmt) {
				case '$' :
					fprintf(out, "\n%s", GEN_PREFIX);
					CurrCol = 0;
					break;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					{
						int n = *fmt - '0';
						while (n-- > 0) putc('\t', out);
						break;
					}
				default  :
					fprintf(out, "$%c", *fmt);
				}
			}
			else {
				putc(*fmt, out);
				CurrCol++;
			}
	va_end(ap);
}

/* return symbolic character name */
void SymCharName(unsigned char c, char *asciiname)
{
	switch (c) {
	case 0   :
		strcpy(asciiname, "Nul");
		return;
	case 8   :
		strcpy(asciiname, "BS");
		return;
	case 9   :
		strcpy(asciiname, "HT");
		return;
	case 10  :
		strcpy(asciiname, "LF");
		return;
	case 12  :
		strcpy(asciiname, "FF");
		return;
	case 13  :
		strcpy(asciiname, "CR");
		return;
	case 27  :
		strcpy(asciiname, "ESC");
		return;
	case ' ' :
		strcpy(asciiname, "_"); /* SP */
		return;
	case '!' :
		strcpy(asciiname, "Bang");
		return;
	case '"' :
		strcpy(asciiname, "Dquote");
		return;
	case '#' :
		strcpy(asciiname, "Hash");
		return;
	case '$' :
		strcpy(asciiname, "Dollar");
		return;
	case '%' :
		strcpy(asciiname, "Percent");
		return;
	case '&' :
		strcpy(asciiname, "And");
		return;
	case '\'':
		strcpy(asciiname, "Squote");
		return;
	case '(' :
		strcpy(asciiname, "Lparen");
		return;
	case ')' :
		strcpy(asciiname, "Rparen");
		return;
	case '*' :
		strcpy(asciiname, "Star");
		return;
	case '+' :
		strcpy(asciiname, "Plus");
		return;
	case ',' :
		strcpy(asciiname, "Comma");
		return;
	case '-' :
		strcpy(asciiname, "Minus");
		return;
	case '.' :
		strcpy(asciiname, "Point");
		return;
	case '/' :
		strcpy(asciiname, "Slash");
		return;
	case ':' :
		strcpy(asciiname, "Colon");
		return;
	case ';' :
		strcpy(asciiname, "Semicolon");
		return;
	case '<' :
		strcpy(asciiname, "Less");
		return;
	case '=' :
		strcpy(asciiname, "Equal");
		return;
	case '>' :
		strcpy(asciiname, "Greater");
		return;
	case '?' :
		strcpy(asciiname, "Query");
		return;
	case '@' :
		strcpy(asciiname, "At");
		return;
	case '[' :
		strcpy(asciiname, "Lbrack");
		return;
	case '\\':
		strcpy(asciiname, "Backslash");
		return;
	case ']' :
		strcpy(asciiname, "Rbrack");
		return;
	case '^' :
		strcpy(asciiname, "Uparrow");
		return;
	case '_' :
		strcpy(asciiname, "Underscore");
		return;
	case '`' :
		strcpy(asciiname, "Accent");
		return;
	case '{' :
		strcpy(asciiname, "Lbrace");
		return;
	case '|' :
		strcpy(asciiname, "Bar");
		return;
	case '}' :
		strcpy(asciiname, "Rbrace");
		return;
	case '~' :
		strcpy(asciiname, "Tilde");
		return;
	case '0' :
	case '1' :
	case '2' : 
	case '3' :
	case '4' :
	case '5' :
	case '6' :
	case '7' :
	case '8' :
	case '9' :
		sprintf(asciiname, "D%c", c);
		return;
	default :
		strcpy(asciiname, " ");
		if (c >= '0' && c <= '9' ||
				c >= 'A' && c <= 'Z' ||
				c >= 'a' && c <= 'z') asciiname[0] = c;
		else sprintf(asciiname, "Asc%d", c);
		return;
	}
}

FILE *OpenFile(char *name, char *op, int GenError)
{
	FILE *F;
	if((F = fopen(name, op)) == NULL) {
	if (GenError) {
		fprintf(stderr, "Unable to open file %s\n", name);
		exit(1);
	}
	}
	return F;
}

#if 0
#if !( __MSDOS__ || MSDOS || WIN32 || __WIN32__)
int stricmp(char *s1, char *s2)
{
	char c1, c2;
	int c;

	if (strlen(s1) > strlen(s2)) c = strlen(s2);
	else c = strlen(s1);

	while (c--) {
		c1 = (*s1 >= 'a' && *s1 <= 'z') ? *s1 - 32 : *s1;
		c2 = (*s2 >= 'a' && *s2 <= 'z') ? *s2 - 32 : *s2;
		if (c1 < c2) return -1;
		if (c1 > c2) return 1;
		s1++;
		s2++;
	}
	return 0;
}
#endif
#endif

static void ProcessFrame(FILE *In, FILE *Out, char *Prefix)
{
	char option[100];

	CopyFrameReset();
	while (1) {
		CopyFrame(In, Out, option);
		if (!stricmp(option, "ErrorFile")) {
			fprintf(Out, "\"%se.%s\"", Prefix, h_ext);
		} else
		if (!stricmp(option, "ScanHeader")) {
		    fprintf(Out, "\"%ss.%s\"", Prefix, h_ext);
		} else
		if (!stricmp(option, "ParserHeader")) {
		    fprintf(Out, "\"%sp.%s\"", Prefix, h_ext);
		} else
		if (!stricmp(option, "ErrorHeader")) {
			fprintf(Out, "\"%se.%s\"", Prefix, h_ext);
		} else
		if (!stricmp(option, "TokensHeader")) {
			fprintf(Out, "\"%sc.%s\"", Prefix, h_ext);
		} else
		if (!stricmp(option, "ScanClass")) {
			fprintf(Out, "%sScanner", Prefix);
		} else
		if (!stricmp(option, "ParserClass")) {
			fprintf(Out, "%sParser", Prefix);
		} else
		if (GenParserOptions(Out, option)) {
		} else
		if (GenScannerOptions(Out, option)) {
		} else
		if (!stricmp(option, "ENDFILE")) break;
	}
}

static void GenFrame(char *Prefix, char *InName, char *OutName)
{
	FILE *InFile, *OutFile;
	char FileName[100];

	sprintf(FileName, "%s%s", Frames_Path, InName);
	InFile = OpenFile(FileName, "r", 1);

	sprintf(FileName, OutName, Prefix);
	OutFile = OpenFile(FileName, "w", 1);

	ProcessFrame(InFile, OutFile, Prefix);

	fclose(InFile);
	fclose(OutFile);
}

/* generate compiler */
void GenCompiler()
{
	FILE *InFile, *OutFile;
	char FileName[100];

	sprintf(FileName, Compiler_Format, module_prefix);
	OutFile = OpenFile(FileName, "w", 1);

	sprintf(FileName, "%s.frm", module_prefix);
	InFile = OpenFile(FileName, "r", 0);
	if (InFile == NULL) {
		sprintf(FileName, "%scompiler.frm", Frames_Path);
		InFile = OpenFile(FileName, "r", 1);
	}

	ProcessFrame(InFile, OutFile, module_prefix);

	fclose(InFile);
	fclose(OutFile);
}

/* generate compiler */
void GenHeaders()
{
	FILE *OutFile;
	char FileName[100];

	sprintf(FileName, "%se.%s", module_prefix, h_ext);
	OutFile = OpenFile(FileName, "w", 1);
	GenParserErrors(OutFile);
	fclose(OutFile);

	sprintf(FileName, "%sc.%s", module_prefix, h_ext);
	OutFile = OpenFile(FileName, "w", 1);
	GenScannerTokens(OutFile);
	fclose(OutFile);
}

void GenParser(void)
{
	GenFrame(module_prefix, "parser_c.frm", Parser_C_Format);
	GenFrame(module_prefix, "parser_h.frm", Parser_H_Format);
}

void GenScanner(void)
{
	GenFrame(module_prefix, "scan_c.frm", Scan_C_Format);
	GenFrame(module_prefix, "scan_h.frm", Scan_H_Format);
}



