/* Copyright 2009, UCAR/Unidata and OPeNDAP, Inc.
   See the COPYRIGHT file for more information. */

/*The lines down to DO NOT DELETE ... comment are specific to the C Parser.
  They will be commennted out when building a java parser.
*/
%pure-parser
%lex-param {DAPparsestate* parsestate}
%parse-param {DAPparsestate* parsestate}
%{#include "dapparselex.h"%}

/*DO NOT DELETE THIS LINE*/

%token SCAN_ALIAS 
%token SCAN_ARRAY
%token SCAN_ATTR
%token SCAN_BYTE
%token SCAN_CODE
%token SCAN_DATASET

%token SCAN_ERROR
%token SCAN_FLOAT32
%token SCAN_FLOAT64
%token SCAN_GRID
%token SCAN_INT16
%token SCAN_INT32
%token SCAN_MAPS 
%token SCAN_MESSAGE
%token SCAN_SEQUENCE
%token SCAN_STRING
%token SCAN_STRUCTURE
%token SCAN_UINT16
%token SCAN_UINT32
%token SCAN_URL 
%token SCAN_WORD
/* For errorbody */
%token SCAN_PTYPE
%token SCAN_PROG

%start start

%%

start:
	  SCAN_DATASET datasetbody
	| SCAN_ATTR dassetup attributebody
	| SCAN_ERROR errorbody
        | error
            {$$=unrecognizedresponse(parsestate);}
	;

datasetbody:
	  '{' declarations '}' datasetname ';'
		{$$=datasetbody(parsestate,$4,$2);}
	;

declarations:
	  /* empty */ {$$=declarations(parsestate,null,null);}
        | declarations declaration {$$=declarations(parsestate,$1,$2);}
	;

/* 01/21/08: James says: no dimensions for grids or sequences */
/* 05/08/09: James says: no duplicate map names */
declaration:
	  base_type var_name array_decls ';'
		{$$=makebase(parsestate,$2,$1,$3);}
	| SCAN_STRUCTURE '{' declarations '}' var_name array_decls ';'
	    {if(($$ = makestructure(parsestate,$5,$6,$3))==null) {YYABORT;}}
	| SCAN_SEQUENCE '{' declarations '}' var_name ';'
	    {if(($$ = makesequence(parsestate,$5,$3))==null) {YYABORT;}}
	| SCAN_GRID '{' SCAN_ARRAY ':' declaration SCAN_MAPS ':'
          declarations '}' var_name ';'
	    {if(($$ = makegrid(parsestate,$10,$5,$8))==null) {YYABORT;}}
        | error 
            {daperror(parsestate,"Unrecognized type"); YYABORT;}
	;
 

base_type:
	  SCAN_BYTE {$$=(Object)SCAN_BYTE;}
	| SCAN_INT16 {$$=(Object)SCAN_INT16;}
	| SCAN_UINT16 {$$=(Object)SCAN_UINT16;}
	| SCAN_INT32 {$$=(Object)SCAN_INT32;}
	| SCAN_UINT32 {$$=(Object)SCAN_UINT32;}
	| SCAN_FLOAT32 {$$=(Object)SCAN_FLOAT32;}
	| SCAN_FLOAT64 {$$=(Object)SCAN_FLOAT64;}
	| SCAN_URL {$$=(Object)SCAN_URL;}
	| SCAN_STRING {$$=(Object)SCAN_STRING;}
	;

array_decls:
	  /* empty */ {$$=arraydecls(parsestate,null,null);}
	| array_decls array_decl {$$=arraydecls(parsestate,$1,$2);}
	;

array_decl:
	   '[' SCAN_WORD ']' {$$=arraydecl(parsestate,null,$2);}
	 | '[' name '=' SCAN_WORD ']' {$$=arraydecl(parsestate,$2,$4);}
	 | error
	    {daperror(parsestate,"Illegal dimension declaration"); YYABORT;}
	;

datasetname:
	  var_name {$$=$1;}
        | error
	    {daperror(parsestate,"Illegal dataset declaration"); YYABORT;}
	;

var_name: name {$$=$1;};

dassetup: {dassetup(parsestate);}

attributebody:
	  '{' attr_list '}' {$$=attributebody(parsestate,$2);}
	| error
            {daperror(parsestate,"Illegal DAS body"); YYABORT;}
	;

attr_list:
	  /* empty */ {$$=attrlist(parsestate,null,null);}
	| attr_list attribute {$$=attrlist(parsestate,$1,$2);}
	;

attribute:
	  alias ';' {$$=null;} /* ignored */ 
        | SCAN_BYTE name bytes ';'
	    {$$=attribute(parsestate,$2,$3,(Object)SCAN_BYTE);}
	| SCAN_INT16 name int16 ';'
	    {$$=attribute(parsestate,$2,$3,(Object)SCAN_INT16);}
	| SCAN_UINT16 name uint16 ';'
	    {$$=attribute(parsestate,$2,$3,(Object)SCAN_UINT16);}
	| SCAN_INT32 name int32 ';'
	    {$$=attribute(parsestate,$2,$3,(Object)SCAN_INT32);}
	| SCAN_UINT32 name uint32 ';'
	    {$$=attribute(parsestate,$2,$3,(Object)SCAN_UINT32);}
	| SCAN_FLOAT32 name float32 ';'
	    {$$=attribute(parsestate,$2,$3,(Object)SCAN_FLOAT32);}
	| SCAN_FLOAT64 name float64 ';'
	    {$$=attribute(parsestate,$2,$3,(Object)SCAN_FLOAT64);}
	| SCAN_STRING name strs ';'
	    {$$=attribute(parsestate,$2,$3,(Object)SCAN_STRING);}
	| SCAN_URL name urls ';'
	    {$$=attribute(parsestate,$2,$3,(Object)SCAN_URL);}
	| name '{' attr_list '}' {$$=attrset(parsestate,$1,$3);}
	| error 
            {daperror(parsestate,"Illegal attribute"); YYABORT;}
	;

bytes:
	  SCAN_WORD {$$=attrvalue(parsestate,null,$1,(Object)SCAN_BYTE);}
	| bytes ',' SCAN_WORD
		{$$=attrvalue(parsestate,$1,$3,(Object)SCAN_BYTE);}
	;
int16:
	  SCAN_WORD {$$=attrvalue(parsestate,null,$1,(Object)SCAN_INT16);}
	| int16 ',' SCAN_WORD
		{$$=attrvalue(parsestate,$1,$3,(Object)SCAN_INT16);}
	;
uint16:
	  SCAN_WORD {$$=attrvalue(parsestate,null,$1,(Object)SCAN_UINT16);}
	| uint16 ',' SCAN_WORD
		{$$=attrvalue(parsestate,$1,$3,(Object)SCAN_UINT16);}
	;
int32:
	  SCAN_WORD {$$=attrvalue(parsestate,null,$1,(Object)SCAN_INT32);}
	| int32 ',' SCAN_WORD
		{$$=attrvalue(parsestate,$1,$3,(Object)SCAN_INT32);}
	;
uint32:
	  SCAN_WORD {$$=attrvalue(parsestate,null,$1,(Object)SCAN_UINT32);}
	| uint32 ',' SCAN_WORD  {$$=attrvalue(parsestate,$1,$3,(Object)SCAN_UINT32);}
	;
float32:
	  SCAN_WORD {$$=attrvalue(parsestate,null,$1,(Object)SCAN_FLOAT32);}
	| float32 ',' SCAN_WORD  {$$=attrvalue(parsestate,$1,$3,(Object)SCAN_FLOAT32);}
	;
float64:
	  SCAN_WORD {$$=attrvalue(parsestate,null,$1,(Object)SCAN_FLOAT64);}
	| float64 ',' SCAN_WORD  {$$=attrvalue(parsestate,$1,$3,(Object)SCAN_FLOAT64);}
	;
strs:
	  str_or_id {$$=attrvalue(parsestate,null,$1,(Object)SCAN_STRING);}
	| strs ',' str_or_id {$$=attrvalue(parsestate,$1,$3,(Object)SCAN_STRING);}
	;

urls:
	  url {$$=attrvalue(parsestate,null,$1,(Object)SCAN_URL);}
	| urls ',' url {$$=attrvalue(parsestate,$1,$3,(Object)SCAN_URL);}
	;

url:
	SCAN_WORD {$$=$1;}
	;

str_or_id:
	SCAN_WORD {$$=$1;}
	;

/* Not used
float_or_int:
	SCAN_WORD {$$=$1;}
	;
*/

alias:
	SCAN_ALIAS SCAN_WORD SCAN_WORD {$$=$2; $$=$3; $$=null;} /* Alias is ignored */
	;

errorbody:
	'{' errorcode errormsg errorptype errorprog '}' ';'
		{$$=errorbody(parsestate,$2,$3,$4,$5);}
	;

errorcode:  /*empty*/ {$$=null;} | SCAN_CODE    '=' SCAN_WORD ';' {$$=$3;}
errormsg:   /*empty*/ {$$=null;} | SCAN_MESSAGE '=' SCAN_WORD ';' {$$=$3;}
errorptype: /*empty*/ {$$=null;} | SCAN_PTYPE   '=' SCAN_WORD ';' {$$=$3;}
errorprog : /*empty*/ {$$=null;} | SCAN_PROG    '=' SCAN_WORD ';' {$$=$3;}

/* Note that variable names like "byte" are legal names
   and are disambiguated by context
*/
name:
          SCAN_WORD      {$$=$1;}
	| SCAN_ALIAS     {$$=strdup("alias");}
	| SCAN_ARRAY     {$$=strdup("array");}
	| SCAN_ATTR      {$$=strdup("attributes");}
	| SCAN_BYTE      {$$=strdup("byte");}
	| SCAN_DATASET   {$$=strdup("dataset");}
	| SCAN_ERROR     {$$=strdup("error");}
	| SCAN_FLOAT32   {$$=strdup("float32");}
	| SCAN_FLOAT64   {$$=strdup("float64");}
	| SCAN_GRID      {$$=strdup("grid");}
	| SCAN_INT16     {$$=strdup("int16");}
	| SCAN_INT32     {$$=strdup("int32");}
	| SCAN_MAPS      {$$=strdup("maps");}
	| SCAN_SEQUENCE  {$$=strdup("sequence");}
	| SCAN_STRING    {$$=strdup("string");}
	| SCAN_STRUCTURE {$$=strdup("structure");}
	| SCAN_UINT16    {$$=strdup("uint16");}
	| SCAN_UINT32    {$$=strdup("uint32");}
	| SCAN_URL       {$$=strdup("url");}
	| SCAN_CODE      {$$=strdup("code");}
	| SCAN_MESSAGE   {$$=strdup("message");}
	;

%%
