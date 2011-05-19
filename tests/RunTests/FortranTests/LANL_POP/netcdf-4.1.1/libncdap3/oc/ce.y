/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

/*The lines down to DO NOT DELETE ... comment are specific to the C Parser.
  They will be commennted out when building a java parser.
*/

%pure-parser
%lex-param {CEparsestate* parsestate}
%parse-param {CEparsestate* parsestate}
%{#include "ceparselex.h"%}

/*DO NOT DELETE THIS LINE*/

%token  SCAN_WORD
%token  SCAN_STRINGCONST
%token  SCAN_NUMBERCONST

%start constraints

%%

constraints:
	  projections {projections(parsestate,$1);}
	| selections  {selections(parsestate,$1);}
	| projections selections
	    {projections(parsestate,$1); selections(parsestate,$2);}
	;

/* %type NClist<NCprojection*> */
projections: projectionlist {$$=$1;}

/* %type NClist<NCselection*> */
selections:  selectionlist {$$=$1;}

/* %type NClist<NCprojection*> */
projectionlist:
	  projection
	    {$$=projectionlist(parsestate,null,$1);}
	| projectionlist ',' projection
	    {$$=projectionlist(parsestate,$1,$3);}
	;

/* %type NCprojection* */
projection:
	  segmentlist
	    {$$=projection(parsestate,$1);}
	;

/* %type NClist<NCsegment> */
segmentlist:
	  segment
	    {$$=segmentlist(parsestate,null,$1);}
	| segmentlist '.' segment
	    {$$=segmentlist(parsestate,$1,$3);}
	;

/* %type NCsegment */
segment:
	  word
	    {$$=segment(parsestate,$1,null);}
	| word array_indices
	    {$$=segment(parsestate,$1,$2);}
	;

/* %type NClist<NCslice*> */
array_indices: /* appends indices to state->segment */
	  array_index
	    {$$=array_indices(parsestate,null,$1);}
        | array_indices array_index
	    {$$=array_indices(parsestate,$1,$2);}
	;

/* %type NCslice* */
array_index: 
	'[' range ']' {$$=$2;}
	;

/* %type NCslice* */
range:
	  index
	    {$$=range(parsestate,$1,null,null);}
	| index ':' index
	    {$$=range(parsestate,$1,null,$3);}
	| index ':' index ':' index
	    {$$=range(parsestate,$1,$3,$5);}
	;

/* %type NClist<NCselection*> */
selectionlist:
	  '&' sel_clause
	    {$$=selectionlist(parsestate,null,$2);}
	| selectionlist sel_clause
	    {$$=selectionlist(parsestate,$1,$2);}
	;

/* %type NCselection* */
sel_clause:
	  selectionvar rel_op '{' value_list '}'
	    {$$=sel_clause(parsestate,1,$1,$2,$4);} /*1,2 distinguish cases*/
	| selectionvar rel_op value
	    {$$=sel_clause(parsestate,2,$1,$2,$3);}
	| function
        ;

/* %type NClist<NCselection*> */
selectionvar:
	selectionpath
	    {$$=$1;}
	;
/* %type NClist<NCselection*> */
selectionpath:
	  ident
	    {$1=selectionpath(parsestate,null,$1);}
	| segment '.' ident
	    {$$=selectionpath(parsestate,$1,$3);}
	;

function:
	  ident '(' ')'
	    {$$=function(parsestate,$1,null);}
	| ident '(' arg_list ')'
	    {$$=function(parsestate,$1,$3);}
	;

arg_list:
	  value
	    {$$=arg_list(parsestate,null,$1);}
	| value_list ',' value
	    {$$=arg_list(parsestate,$1,$3);}
	;

value_list:
	  value
	    {$$=value_list(parsestate,null,$1);}
	| value_list '|' value
	    {$$=value_list(parsestate,$1,$3);}
	;

value:
	  selectionpath /* can be variable or an integer */
	    {$$=value(parsestate,$1,SCAN_WORD);}
	| number
	    {$$=value(parsestate,$1,SCAN_NUMBERCONST);}
	| string
	    {$$=value(parsestate,$1,SCAN_STRINGCONST);}
	;

/* %type SelectionTag */
rel_op:
	  '='     {$$=(Object)ST_EQ;}
	| '>'     {$$=(Object)ST_GT;}
	| '<'     {$$=(Object)ST_LT;}
	| '!' '=' {$$=(Object)ST_NEQ;}
	| '=' '~' {$$=(Object)ST_RE;}
	| '>' '=' {$$=(Object)ST_GE;}
	| '<' '=' {$$=(Object)ST_LE;}
	;

ident:  word
	    {$$ = $1;}
	;

index:  number
	    { unsigned long tmp = 0;
		if(sscanf((char*)$1,"%lu",&tmp) != 1) {
		    yyerror(parsestate,"Index is not an integer");
		}
		$$ = $1;
	    }
	;

word:  SCAN_WORD
	    {$$ = $1;}
	;

number:  SCAN_NUMBERCONST
	    {$$ = $1;}
	;

string: SCAN_STRINGCONST
	    {$$ = $1;}
	;

%%
