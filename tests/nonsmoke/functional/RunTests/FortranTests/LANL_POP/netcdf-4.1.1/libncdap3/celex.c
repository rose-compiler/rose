/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

#include "ceparselex.h"

/* Forward */
static void dumptoken(CElexstate* lexstate);
static int tohex(int c);
static void ceaddyytext(CElexstate* lex, int c);

/****************************************************/
static char* worddelims =
  "{}[]:;=,";

/* Define 1 and > 1st legal characters */
static char* wordchars1 =
  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-+_/%\\";
static char* wordcharsn =
  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-+_/%\\";

/* Number characters */
static char* numchars1="+-0123456789";
static char* numcharsn="Ee.+-0123456789";

/**************************************************/

int
celex(YYSTYPE* lvalp, CEparsestate* state)
{
    CElexstate* lexstate = state->lexstate;
    int token;
    int c;
    char* p=lexstate->next;
    token = 0;
    ncbytesclear(lexstate->yytext);
    p=lexstate->next;
    while(token == 0 && (c=*p)) {
	if(c <= ' ' || c >= '\177') {p++; continue;}
	if(c == '"') {
	    int more = 1;
	    /* We have a SCAN_STRINGCONST */
	    while(more && (c=*(++p))) {
		switch (c) {
		case '"': p++; more=0; break;
		case '\\':
		    c=*(++p);
		    switch (c) {
		    case 'r': c = '\r'; break;
		    case 'n': c = '\n'; break;
		    case 'f': c = '\f'; break;
		    case 't': c = '\t'; break;
		    case 'x': {
			int d1,d2;
			c = '?';
			++p;
		        d1 = tohex(*p++);
			if(d1 < 0) {
			    ceerror(state,"Illegal \\xDD in SCAN_STRING");
			} else {
			    d2 = tohex(*p++);
			    if(d2 < 0) {
			        ceerror(state,"Illegal \\xDD in SCAN_STRING");
			    } else {
				c=(((unsigned int)d1)<<4) | (unsigned int)d2;
			    }
			}
		    } break;
		    default: break;
		    }
		    break;
		default: break;
		}
		ceaddyytext(lexstate,c);
	    }
	    token=SCAN_STRINGCONST;
	} else if(strchr(numchars1,c) != NULL) {
	    /* we might have a SCAN_NUMBERCONST */
	    int isnumber = 0;
	    double number;
	    char* yytext;
	    ceaddyytext(lexstate,c);
	    for(p++;(c=*p);p++) {
		if(strchr(numcharsn,c) == NULL) break;
	        ceaddyytext(lexstate,c);
	    }
	    /* See if this is a number */
	    yytext = ncbytescontents(lexstate->yytext);
	    if(sscanf(yytext,"%lg",&number) == 1
	       || sscanf(yytext,"%lG",&number) == 1)
		isnumber = 1; /* maybe */
	    /* A number followed by an id char is assumed to just be
		a funny id */	       
	    if(isnumber && (*p == '\0' || strchr(wordcharsn,*p) == NULL))  {
	        token = SCAN_NUMBERCONST;
	    } else {
		/* Now, if the funny word has a "." in it,
		   we have to back up to that dot */
		char* dotpoint = strchr(yytext,'.');
		if(dotpoint != NULL) {
		    p = dotpoint;
		    *dotpoint = '\0';
		}
		token = SCAN_WORD;
	    }
	} else if(strchr(wordchars1,c) != NULL) {
	    /* we have a SCAN_WORD */
	    ceaddyytext(lexstate,c);
	    for(p++;(c=*p);p++) {
		if(strchr(wordcharsn,c) == NULL) break;
	        ceaddyytext(lexstate,c);
	    }
	    token=SCAN_WORD;
	} else {
	    /* we have a single char token */
	    token = c;
	    p++;
	}
    }
    lexstate->next = p;
    strncpy(lexstate->lasttokentext,ncbytescontents(lexstate->yytext),MAX_TOKEN_LENGTH);
    lexstate->lasttoken = token;
    if(cedebug) dumptoken(lexstate);

    /*Put return value onto Bison stack*/

    if(ncbyteslength(lexstate->yytext) == 0)
        *lvalp = NULL;
    else {
        *lvalp = ncbytesdup(lexstate->yytext);
	nclistpush(lexstate->reclaim,(ncelem)*lvalp);
    }

    return token;
}

static void
ceaddyytext(CElexstate* lex, int c)
{
    ncbytesappend(lex->yytext,(char)c);
}

static int
tohex(int c)
{
    if(c >= 'a' && c <= 'f') return (c - 'a') + 0xa;
    if(c >= 'A' && c <= 'F') return (c - 'A') + 0xa;
    if(c >= '0' && c <= '9') return (c - '0');
    return -1;
}

static void
dumptoken(CElexstate* lexstate)
{
    switch (lexstate->lasttoken) {
    case SCAN_STRINGCONST:
        fprintf(stderr,"TOKEN = |\"%s\"|\n",lexstate->lasttokentext);
	break;
    case SCAN_WORD:
    case SCAN_NUMBERCONST:
    default:
        fprintf(stderr,"TOKEN = |%s|\n",lexstate->lasttokentext);
	break;
    }
}

void
celexinit(char* input, CElexstate** lexstatep)
{
    CElexstate* lexstate = (CElexstate*)malloc(sizeof(CElexstate));
    if(lexstatep) *lexstatep = lexstate;
    if(lexstate == NULL) return;
    memset((void*)lexstate,0,sizeof(CElexstate));
    lexstate->input = strdup(input);
    lexstate->next = lexstate->input;
    lexstate->yytext = ncbytesnew();
    lexstate->reclaim = nclistnew();
}

void
celexcleanup(CElexstate** lexstatep)
{
    unsigned int i;
    CElexstate* lexstate = *lexstatep;
    if(lexstate == NULL) return;
    if(lexstate->input != NULL) free(lexstate->input);
    if(lexstate->reclaim != NULL) {
	while(nclistlength(lexstate->reclaim) > 0) {
	    char* word = (char*)nclistpop(lexstate->reclaim);
	    if(word) free(word);
	}
	nclistfree(lexstate->reclaim);
    }
    ncbytesfree(lexstate->yytext);
    free(lexstate);
    *lexstatep = NULL;
}

