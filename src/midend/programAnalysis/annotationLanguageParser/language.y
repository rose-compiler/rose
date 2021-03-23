%{
extern int annlex(void);
extern void annerror(const char*);

#include <featureTests.h>
#ifdef ROSE_ENABLE_SOURCE_ANALYSIS

#include "broadway.h"

// DQ (9/12/2011): Added assert to support use in testing suggested by static analysis.
#include "assert.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

extern int annlineno;
extern int line_number_offset;
extern char * anntext;

typedef std::list< enumValueAnn * > enumvalue_list;
typedef std::list< exprAnn * > exprann_list;
typedef std::list< ruleAnn * > rule_list;

// -- Utility functions -- defined in c_breeze/src/main/parser.y

#if 0
declNode::Storage_class merge_sc(declNode::Storage_class sc1,
                                 declNode::Storage_class sc2,
                                 const Coord c = Coord::Unknown);
#endif
struct _TQ merge_tq(struct _TQ ts1,
                    struct _TQ ts2);
#endif

// --- Global state variables ----------------

void annerror(const char * msg)
{
#ifdef ROSE_ENABLE_SOURCE_ANALYSIS
  Annotations::Current->Error(annlineno - line_number_offset, std::string(msg) + " near \"" +
                              std::string(anntext) + "\"");
#endif
}
%}

%union {
#ifdef ROSE_ENABLE_SOURCE_ANALYSIS
  /* --- Broadway --- */

  parserID *                  t_id;
  parserid_list *             t_ids;

  int                         t_tok;
  std::string *               t_string;

  structureTreeAnn *          t_structure;
  structuretree_list *        t_structures;

  //  exprNode *                  t_c_expression;

  enumValueAnn *              t_enumvalue;
  enumvalue_list *            t_enumvalue_list;

  exprAnn *                   t_expr;
  exprann_list *              t_exprs;

  analyzeAnn *                t_analyze;

  ruleAnn *                   t_rule;
  rule_list *                 t_rules;

  reportAnn *                 t_report;
  report_element_list *       t_report_elements;
  reportElementAnn *          t_report_element;

  struct {
    int                              line;
    Broadway::Operator                op;
  } t_property_operator;

  struct {
    int                              line;
    Broadway::FlowSensitivity fs;
  } t_property_sensitivity;

  Direction                   t_direction;
#endif

  /* --- C Code --- */
#if 0
  primNode *          primN;
  sueNode *           sueN;
  enumNode *          enumN;
  exprNode *          exprN;
  constNode *         constN;
  idNode *            idN;
  initializerNode *   initializerN;
  unaryNode *         unaryN;
  binaryNode *        binaryN;
  stmtNode *          stmtN;
  exprstmtNode *      exprstmtN;
  blockNode *         blockN;
  targetNode *        targetN;
  loopNode *          loopN;
  jumpNode *          jumpN;
  declNode *          declN;
  attribNode *        attribN;
  procNode *          procN;

  def_list  * defL;
  stmt_list * stmtL;
  decl_list * declL;
  attrib_list * attribL;
  expr_list * exprL;

  /* type: type nodes must also carry storage class */
  struct _TYPE typeN;

  /* tq: type qualifiers, storage class */
  struct _TQ tq;

  /* tok: token coordinates */
  simpleCoord  tok;
#endif
}

/* ------------------------------------------------------------

      Broadway

   ------------------------------------------------------------
*/

%token <t_id> tokID
%token <t_c_expression> tokCONSTANT

%token <t_tok> tokENABLE tokDISABLE

%token <t_tok> tokMAYPROPERTY
%token <t_tok> tokMUSTPROPERTY
%token <t_tok> tokGLOBAL
%token <t_tok> tokINITIALLY
%token <t_tok> tokDIAGNOSTIC /* TB new */

%token <t_tok> tokPROCEDURE
%token <t_tok> tokACCESS
%token <t_tok> tokMODIFY

%token <t_tok> tokANALYZE
%token <t_tok> tokDEFAULT
%token <t_tok> tokIF

%token <t_tok> tokIS_EXACTLY
%token <t_tok> tokIS_ATLEAST
%token <t_tok> tokCOULD_BE
%token <t_tok> tokIS_ATMOST
%token <t_tok> tokIS_BOTTOM

%token <t_tok> tokUNION_SET
%token <t_tok> tokINTERSECT_SET
%token <t_tok> tokUNION_EQUIV
%token <t_tok> tokINTERSECT_EQUIV

%token <t_tok> tokIS_ELEMENT_OF
%token <t_tok> tokIS_EMPTYSET
%token <t_tok> tokADD

%token <t_tok> tokALWAYS
%token <t_tok> tokEVER
%token <t_tok> tokBEFORE
%token <t_tok> tokAFTER
%token <t_tok> tokTRACE
%token <t_tok> tokCONFIDENCE

%token <t_tok> tokFORWARD
%token <t_tok> tokBACKWARD

%token <t_tok> tokIS_ALIASOF
%token <t_tok> tokIS_SAMEAS
%token <t_tok> tokIS_EMPTY

%token <t_tok> tokON_ENTRY
%token <t_tok> tokON_EXIT
%token <t_tok> tokNEW
%token <t_tok> tokARROW
%token <t_tok> tokDELETE

%token <t_tok> tokPATTERN

%token <t_tok> tokANDAND
%token <t_tok> tokOROR

%token <t_tok> tokNOT_EQUALS
%token <t_tok> tokEQUALS_EQUALS
%token <t_tok> tokLESS_EQUALS
%token <t_tok> tokGREATER_EQUALS

%token <t_tok> tokIS_CONSTANT

%token <t_tok> tokASSIGN
%token <t_tok> tokWEAKASSIGN
%token <t_tok> tokEQUALS

%token <t_tok> tokREPORT
%token <t_tok> tokERROR
%token <t_tok> tokAPPEND

%token <t_tok> tokWHEN
%token <t_tok> tokREPLACE
%token <t_tok> tokINLINE

%token <t_tok> tokIO

%token <t_id> tokSTRING

%token <t_tok> ':' '-' ',' ';' '!' '/' '^' '~' '(' ')' '{' '}'
%token <t_tok> '*' '&' '%' '+' '|' '.' '$' '@'

%nonassoc WEAK_REDUCE
%nonassoc LOWER_THAN_EXPRESSION
%left tokANDAND tokOROR
%left tokIS_EXACTLY tokIS_ATLEAST tokCOULD_BE tokIS_ATMOST tokIS_BOTTOM
%left tokEQUALS tokEQUALS_EQUALS tokNOT_EQUALS tokLESS_EQUALS tokGREATER_EQUALS
%left tokASSIGN tokWEAKASSIGN
%left '|'
%left '^'
%left '&'
%left '+' '-'
%left '*' '/' '%'
%nonassoc LOWER_THAN_UNARY
%nonassoc '~' '!'
%nonassoc LOWER_THAN_POSTFIX
%right '@' '$'
%nonassoc '('
%nonassoc PARENTHESIZED

%type <t_structures> structures
%type <t_structure> structure

%type <t_ids> identifier_list optional_diagnostic;
%type <t_id> qualified_identifier

%type <t_property_operator> binary_property_operator
%type <t_property_sensitivity> property_sensitivity

%type <t_analyze> analyze_annotation

%type <t_enumvalue_list> user_defined_class;
%type <t_enumvalue_list> members
%type <t_enumvalue> member;

%type <t_direction> optional_direction;
%type <t_id> optional_default;

%type <t_rules> rules
%type <t_rule> rule
%type <t_exprs> effects
%type <t_expr> condition test effect

%type <t_report> report_annotation
%type <t_report_elements> report_element_list
%type <t_report_element> report_element

/* ------------------------------------------------------------

      C Code

   ------------------------------------------------------------
*/

/* Define terminal tokens */

%token <tok> ctokAND ctokSTAR ctokPLUS ctokMINUS ctokTILDE ctokNOT
%token <tok> ctokLESS ctokGREATER ctokOR ctokHAT ctokPERCENT ctokDIVIDE
%token <tok> ctokLEFTPAREN ctokRIGHTPAREN ctokDOT ctokQUESTION ctokSEMICOLON
%token <tok> ctokLEFTBRACE ctokRIGHTBRACE ctokCOMMA ctokLEFTBRACKET ctokRIGHTBRACKET ctokCOLON

/* ANSI keywords, extensions below */
%token <tok> ctokAUTO            ctokDOUBLE          ctokINT             ctokSTRUCT
%token <tok> ctokBREAK           ctokELSE            ctokLONG            ctokSWITCH
%token <tok> ctokCASE            ctokENUM            ctokREGISTER        ctokTYPEDEF
%token <tok> ctokCHAR            ctokEXTERN          ctokRETURN          ctokUNION
%token <tok> ctokCONST           ctokFLOAT           ctokSHORT           ctokUNSIGNED
%token <tok> ctokCONTINUE        ctokFOR             ctokSIGNED          ctokVOID
%token <tok> ctokDEFAULT         ctokGOTO            ctokSIZEOF          ctokVOLATILE
%token <tok> ctokDO              ctokIF              ctokSTATIC          ctokWHILE

/* unary op ctokens added by Eric Brewer */

%token <tok> ctokUPLUS ctokUMINUS ctokINDIR ctokADDRESS ctokPOSTINC ctokPOSTDEC ctokPREINC ctokPREDEC ctokBOGUS


/* ANSI Grammar suggestions */
%token <idN>    IDENTIFIER
%token <constN> STRINGliteral
%token <constN> FLOATINGconstant
%token <constN> INTEGERconstant OCTALconstant HEXconstant WIDECHARconstant
%token <constN> CHARACTERconstant

/* New Lexical element, whereas ANSI suggested non-terminal */

/*
   Lexer distinguishes this from an identifier.
   An identifier that is CURRENTLY in scope as a typedef name is provided
   to the parser as a TYPEDEFname
*/
%token <idN> TYPEDEFname

/* Multi-Character operators */
%token <tok>  ctokARROW            /*    ->                              */
%token <tok>  ctokICR ctokDECR         /*    ++      --                      */
%token <tok>  ctokLS ctokRS            /*    <<      >>                      */
%token <tok>  ctokLE ctokGE ctokEQ ctokNE      /*    <=      >=      ==      !=      */
%token <tok>  ctokANDAND ctokOROR      /*    &&      ||                      */
%token <tok>  ctokELLIPSIS         /*    ...                             */

/* modifying assignment operators */
%token <tok> ctokEQUALS
%token <tok> ctokMULTassign  ctokDIVassign    ctokMODassign   /*   *=      /=      %=      */
%token <tok> ctokPLUSassign  ctokMINUSassign              /*   +=      -=              */
%token <tok> ctokLSassign    ctokRSassign                 /*   <<=     >>=             */
%token <tok> ctokANDassign   ctokERassign     ctokORassign    /*   &=      ^=      |=      */

/* GCC extensions */
%token <tok> ctokINLINE
%token <tok> ctokATTRIBUTE

/* -- OLD Meta programming -- */

%token <tok> ctokMETA_TYPE_EXPR
%token <tok> ctokMETA_TYPE_STMT

%token <idN> ctokMETA_EXPR
%token <idN> ctokMETA_STMT

/* -- Generated list -- */

/* %type <programN> prog.start */

/* ------------------------------------------------------------

      C patterns

   ------------------------------------------------------------
*/

%token <t_id>  metatokID
%token <t_tok> metatokEXPR
%token <t_tok> metatokSTMT
%token <t_tok> metatokTYPE

%%

annotation_file:
    annotation
  | annotation_file annotation
  ;

annotation:
    global
  | property
  | tokENABLE tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
          Annotations::Enabled_properties.push_back($2->name());
      #endif
      }
  | tokDISABLE tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
          Annotations::Disabled_properties.push_back($2->name());
      #endif
      }
  | procedure
  | pattern
  | analyze_annotation
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        Annotations::Current->init()->add_analysis($1);
      #endif
      }
  ;

/* --- Global variables ----------------------- */

global:
    tokGLOBAL '{' structures '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        Annotations::Current->add_globals( $3 );
        delete $3;
      #endif
      }

  | tokGLOBAL structure
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        structuretree_list * temp = new structuretree_list();
        temp->push_back( $2);
        Annotations::Current->add_globals( temp );
      #endif
      }
  ;

/* --- Property declarations ------------------ */

property:
    tokMAYPROPERTY tokID ':' optional_direction user_defined_class optional_diagnostic optional_default
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        Annotations::Current->add_enum_property(new enumPropertyAnn($2, $4, true, $7, $5, $6));
        delete $2;
        delete $5;
        delete $6;
      #endif
      }

  | tokMUSTPROPERTY tokID ':' optional_direction user_defined_class optional_diagnostic optional_default
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        Annotations::Current->add_enum_property(new enumPropertyAnn($2, $4, false, $7, $5, $6));
        delete $2;
        delete $5;
        delete $6;
      #endif
      }

  | tokMAYPROPERTY tokID ':' tokUNION_SET
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        Annotations::Current->add_set_property(new setPropertyAnn($2, Forward,
                                                                  setPropertyAnn::Set,
                                                                  setPropertyAnn::Union));
        delete $2;
      #endif
      }

  | tokMUSTPROPERTY tokID ':' tokINTERSECT_SET
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        Annotations::Current->add_set_property(new setPropertyAnn($2, Forward,
                                                                  setPropertyAnn::Set,
                                                                  setPropertyAnn::Intersect));
        delete $2;
      #endif
      }

  | tokMAYPROPERTY tokID ':' tokUNION_EQUIV
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        Annotations::Current->add_set_property(new setPropertyAnn($2, Forward,
                                                                  setPropertyAnn::Equivalence,
                                                                  setPropertyAnn::Union));
        delete $2;
      #endif
      }

  | tokMUSTPROPERTY tokID ':' tokINTERSECT_EQUIV
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        Annotations::Current->add_set_property(new setPropertyAnn($2, Forward,
                                                                  setPropertyAnn::Equivalence,
                                                                  setPropertyAnn::Intersect));
        delete $2;
      #endif
      }

  ;

optional_direction:
    {
    #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
      $$ = Forward;
    #endif
    }

  | tokFORWARD
    {
    #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
      $$ = Forward;
    #endif
    }

  | tokBACKWARD
    {
    #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
      $$ = Backward;
    #endif
    }
;

optional_default:

      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = (parserID *)0;
      #endif
      }

  | tokINITIALLY tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = $2;
      #endif
      }
  ;

optional_diagnostic: /* TB new */
    {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = NULL;
      #endif
    }
  | tokDIAGNOSTIC '{' identifier_list '}'
    {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = $3;
      #endif
    }
  ;

user_defined_class:
    '{' members '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = $2;
      #endif
      }
  ;

members:
    member
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new enumvalue_list();
        $$->push_back($1);
      #endif
      }

  | members ',' member
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = $1;
        $$->push_back($3);
      #endif
      }
  ;

member:
    tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new enumValueAnn($1, (enumvalue_list *) 0);
        delete $1;
      #endif
      }

  | tokID user_defined_class
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new enumValueAnn($1, $2);
        delete $1;
        delete $2;
      #endif
      }
  ;

/* --- Procedure annotations ------------------ */

procedure:
    procedure_declaration '{' procedure_annotations '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        assert(procedureAnn::Current != NULL);
        Annotations::Current->add_procedure(procedureAnn::Current);
        procedureAnn::Current = 0;
      #endif
      }

  | procedure_declaration '{' '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        assert(procedureAnn::Current != NULL);
        Annotations::Current->add_procedure(procedureAnn::Current);
        procedureAnn::Current = 0;
      #endif
      }
  ;

procedure_declaration:
    tokPROCEDURE tokID '(' identifier_list ')'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        procedureAnn::Current = new procedureAnn($2, $4, Annotations::Current, $1);
        delete $2;
      #endif
      }

  | tokPROCEDURE tokID '(' ')'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        procedureAnn::Current = new procedureAnn($2, (parserid_list *)0, Annotations::Current, $1);
        delete $2;
      #endif
      }
  ;

procedure_annotations:
    procedure_annotation
  | procedure_annotations procedure_annotation
  ;

procedure_annotation:
    structure_annotation
      {
      }

  | behavior_annotation
      {
      }

  | analyze_annotation
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        procedureAnn::Current->add_analysis($1);
      #endif
      }

  | report_annotation
      {
      }

  ;

/* --- Structure declarations ----------------- */

structure_annotation:
    tokON_ENTRY '{' structures '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
        procedureAnn::Current->add_on_entry($3);
      #endif
      }

  | tokON_EXIT  '{' structures '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
        procedureAnn::Current->add_on_exit(new pointerRuleAnn((exprAnn *)0, $3, $1));
      #endif
      }

  | tokON_EXIT '{' pointer_rule_list '}'
      {
      }

  | tokON_ENTRY '{' '}'
      {
      }

  | tokON_EXIT '{' '}'
      {
      }
  ;

pointer_rule_list:
    pointer_rule
  | pointer_rule_list pointer_rule
  ;

pointer_rule:
    tokIF '(' condition ')' '{' structures '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
        procedureAnn::Current->add_on_exit(new pointerRuleAnn($3, $6, $1));
      #endif
      }

  | tokIF '(' condition ')' structure
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        structuretree_list * temp = new structuretree_list();
        temp->push_back($5);

     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
        procedureAnn::Current->add_on_exit(new pointerRuleAnn($3, temp, $1));
      #endif
      }

  | tokDEFAULT '{' structures '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
        procedureAnn::Current->add_on_exit(new pointerRuleAnn((exprAnn *)0, $3, $1));
      #endif
      }

  | tokDEFAULT structure
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        structuretree_list * temp = new structuretree_list();
        temp->push_back($2);

     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
        procedureAnn::Current->add_on_exit(new pointerRuleAnn((exprAnn *)0, temp, $1));
      #endif
      }

  ;

structures:
    structure
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new structuretree_list();
        $$->push_back($1);
      #endif
      }

  | structures structure
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = $1;
        $$->push_back($2);
      #endif
      }

  | structures ',' structure
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = $1;
        $$->push_back($3);
      #endif
      }

  | tokDELETE qualified_identifier
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
        procedureAnn::Current->add_delete($2);
        delete $2;

        /* "delete" doesn't actually add anything to the structure tree, so
           we just pass an empty list back. Similarly for the next two
           productions. */

        $$ = new structuretree_list();
      #endif
      }

  | structures tokDELETE qualified_identifier
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
        procedureAnn::Current->add_delete($3);
        delete $3;
        $$ = $1;
      #endif
      }

  | structures ',' tokDELETE qualified_identifier
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
        procedureAnn::Current->add_delete($4);
        delete $4;
        $$ = $1;
      #endif
      }
  ;

structure:
    qualified_identifier
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new structureTreeAnn($1, (structuretree_list *)0,
                                  structureTreeAnn::None, false);
        delete $1;
      #endif
      }

  | tokID tokARROW structure
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new structureTreeAnn($1, $3, structureTreeAnn::Arrow, false);
        delete $1;
      #endif
      }

  | tokID tokARROW tokNEW structure
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new structureTreeAnn($1, $4, structureTreeAnn::Arrow, true);
        delete $1;
      #endif
      }

  | tokID '{' structures '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new structureTreeAnn($1, $3, structureTreeAnn::Dot, false);
        delete $1;
      #endif
      }
  ;

/* --- Behavior ------------------------------- */

behavior_annotation:

    tokACCESS '{' identifier_list '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
        procedureAnn::Current->add_uses( $3 );
        delete $3;
      #endif
      }

  | tokACCESS tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        parserid_list temp;
        temp.push_back( * $2);
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
        procedureAnn::Current->add_uses( & temp );
        delete $2;
      #endif
      }

  | tokMODIFY '{' identifier_list '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
        procedureAnn::Current->add_defs( $3 );
        delete $3;
      #endif
      }

  | tokMODIFY tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        parserid_list temp;
        temp.push_back( * $2);
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
        procedureAnn::Current->add_defs( & temp );
        delete $2;
      #endif
      }

  ;

/* --- Analyzer rules ------------------------- */

analyze_annotation:
    tokANALYZE tokID '{' rules '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new analyzeAnn(Annotations::Current, $2, $4, $1);
        delete $2;
      #endif
      }

  | tokANALYZE tokID '{' effects '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        rule_list * temp_rules = new rule_list();
        temp_rules->push_back(new ruleAnn((exprAnn *)0, $4, $3));
        $$ = new analyzeAnn(Annotations::Current, $2, temp_rules, $1);
        delete $2;
      #endif
      }
  ;

rules:
    rule
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new rule_list();
        $$->push_back($1);
      #endif
      }

  | rules rule
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = $1;
        $$->push_back($2);
      #endif
      }
  ;

rule:
    tokIF '(' condition ')' '{' effects '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new ruleAnn($3, $6, $1);
      #endif
      }

  | tokIF '(' condition ')' effect
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        exprann_list * temp = new exprann_list();
        temp->push_back($5);
        $$ = new ruleAnn($3, temp, $1);
      #endif
      }

  | tokDEFAULT '{' effects '}'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new ruleAnn((exprAnn *)0, $3, $1);
      #endif
      }

  | tokDEFAULT effect
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        exprann_list * temp = new exprann_list();
        temp->push_back($2);
        $$ = new ruleAnn((exprAnn *)0, temp, $1);
      #endif
      }

  ;

condition:
    test
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = $1;
      #endif
      }

  | condition tokOROR condition
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new connectiveExprAnn(Broadway::Or, $1, $3, $2);
      #endif
      }

  | condition tokANDAND condition
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new connectiveExprAnn(Broadway::And, $1, $3, $2);
      #endif
      }

  | '!' condition
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new connectiveExprAnn(Broadway::Not, $2, (exprAnn *)0, $1);
      #endif
      }

  | '(' condition ')'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = $2;
      #endif
      }
  ;

test:
    /* --- Enum property tests --- */

    tokID ':' tokID property_sensitivity tokIS_BOTTOM
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new enumPropertyExprAnn($4.fs, $1, $3, Broadway::Is_Bottom,
                                     (const parserID *)0, $4.line);
        delete $1;
        delete $3;
      #endif
      }

  | tokID property_sensitivity tokIS_BOTTOM
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new enumPropertyExprAnn($2.fs, (const parserID *)0, $1, Broadway::Is_Bottom,
                                     (const parserID *)0, $2.line);
        delete $1;
      #endif
      }

  | tokID ':' tokID property_sensitivity binary_property_operator tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new enumPropertyExprAnn($4.fs, $1, $3, $5.op, $6, $5.line);
        delete $1;
        delete $3;
        delete $6;
      #endif
      }

  | tokID property_sensitivity binary_property_operator tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new enumPropertyExprAnn($2.fs, (const parserID *)0, $1, $3.op, $4, $3.line);
        delete $1;
        delete $4;
      #endif
      }

    /* --- Set property tests --- */

  | tokID tokID tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new setPropertyExprAnn(Broadway::Before, $2, $1, Broadway::Is_Equivalent, $3, $1->line());
        delete $1;
        delete $2;
        delete $3;
      #endif
      }

  | tokID tokIS_ELEMENT_OF tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new setPropertyExprAnn(Broadway::Before, $3, $1, Broadway::Is_Element_Of,
                                    (const parserID *)0, $2);
        delete $1;
        delete $3;
      #endif
      }

  | tokID tokIS_EMPTYSET
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new setPropertyExprAnn(Broadway::Before, $1, (const parserID *)0,
                                    Broadway::Is_EmptySet, (const parserID *)0, $2);

        delete $1;
      #endif
      }

    /* --- Numeric tests --- */

  | tokID tokIS_CONSTANT
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new numericExprAnn($1, annlineno);
      #endif
      }

    /* --- Pointer tests --- */

  | tokID tokIS_ALIASOF tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new pointerExprAnn(Broadway::Is_AliasOf, $1, $3);
        delete $1;
        delete $3;
      #endif
      }

  | tokID tokIS_SAMEAS tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new pointerExprAnn(Broadway::Is_SameAs, $1, $3);
        delete $1;
        delete $3;
      #endif
      }

  | tokID tokIS_EMPTY
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new pointerExprAnn(Broadway::Is_Empty, $1, (const parserID *)0);
        delete $1;
      #endif
      }

  ;

effects:
    effect
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new exprann_list();
        $$->push_back($1);
      #endif
      }

  | effects effect
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = $1;
        $$->push_back($2);
      #endif
      }
  ;

effect:

  tokID tokASSIGN tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new enumPropertyExprAnn(Broadway::None,
                                     (const parserID *)0, $1, Broadway::Assign,
                                     $3, $2);
        delete $1;
        delete $3;
      #endif
      }

  | tokID tokWEAKASSIGN tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        enumPropertyExprAnn * tmp = new enumPropertyExprAnn(Broadway::None,
                                                            (const parserID *)0, $1, Broadway::Assign,
                                                            $3, $2);
        tmp->set_weak();
        $$ = tmp;
        delete $1;
        delete $3;
      #endif
      }

  | tokADD tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new setPropertyExprAnn(Broadway::None,
                                    (const parserID *)0, $2, Broadway::Add_Elements,
                                    (const parserID *)0, $1);
        delete $2;
      #endif
      }

  | tokID tokID tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new setPropertyExprAnn(Broadway::None,
                                    $2, $1, Broadway::Add_Equivalences, $3, $1->line());
        delete $1;
        delete $2;
        delete $3;
      #endif
      }
  ;

binary_property_operator:

     tokIS_EXACTLY
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
          $$.line = $1;
          $$.op = Broadway::Is_Exactly;
      #endif
      }

  |  tokIS_ATLEAST
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
          $$.line = $1;
          $$.op = Broadway::Is_AtLeast;
      #endif
      }

  |  tokCOULD_BE
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
          $$.line = $1;
          $$.op = Broadway::Could_Be;
      #endif
      }

  |  tokIS_ATMOST
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
          $$.line = $1;
          $$.op = Broadway::Is_AtMost;
      #endif
      }
  ;

property_sensitivity:

     tokBEFORE
       {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
           $$.line = $1;
           $$.fs = Broadway::Before;
      #endif
       }

  |  tokAFTER
       {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
           $$.line = $1;
           $$.fs = Broadway::After;
      #endif
       }

  |  tokALWAYS
       {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
           $$.line = $1;
           $$.fs = Broadway::Always;
      #endif
       }

  |  tokEVER
       {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
           $$.line = $1;
           $$.fs = Broadway::Ever;
      #endif
       }

  |  tokTRACE
       {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
           $$.line = $1;
           $$.fs = Broadway::Trace;
      #endif
       }

  |  tokCONFIDENCE
       {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
           $$.line = $1;
           $$.fs = Broadway::Confidence;
      #endif
       }

  |
       {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
           $$.line =  0;
           $$.fs = Broadway::Before;
      #endif
       }
  ;

/* --- Report --------------------------------- */

report_annotation:

     tokREPORT report_element_list ';'
       {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
         procedureAnn::Current->add_report(new reportAnn((exprAnn *)0, false, $2, $1));
      #endif
       }

  |  tokREPORT tokIF '(' condition ')' report_element_list ';'
       {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
         procedureAnn::Current->add_report(new reportAnn($4, false, $6, $1));
      #endif
       }
  |  tokERROR report_element_list ';'
       {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
         procedureAnn::Current->add_report(new reportAnn((exprAnn *)0, true, $2, $1));
      #endif
       }

  |  tokERROR tokIF '(' condition ')' report_element_list ';'
       {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
     // DQ (9/12/2011): Static analysis reports that this could be NULL, check it explicitly.
        assert(procedureAnn::Current != NULL);
         procedureAnn::Current->add_report(new reportAnn($4, true, $6, $1));
      #endif
       }
  ;

report_element_list:

     report_element
       {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
         $$ = new report_element_list();
         $$->push_back( $1 );
      #endif
       }

  |  report_element_list tokAPPEND report_element
       {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
         $$ = $1;
         $$->push_back($3);
      #endif
       }

  ;

report_element:

     tokSTRING
       {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
         $$ = new literalReportElementAnn($1);
         delete $1;
      #endif
       }

  | tokID ':' tokID property_sensitivity
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new expressionReportElementAnn($4.fs,
                                            new enumPropertyExprAnn($4.fs, $1, $3, Broadway::Report,
                                                                (const parserID *)0, $1->line()),
                                            annlineno);
        delete $1;
        delete $3;
      #endif
      }

  | '@' tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new locationReportElementAnn($2);
      #endif
      }

  | '[' tokID ']'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new bindingReportElementAnn($2, false);
      #endif
      }

  | '|' tokID '|'
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
        $$ = new bindingReportElementAnn($2, true);
      #endif
      }
  ;

/* --- Actions -------------------------- */

/* --- Pattern -------------------------------- */

pattern:
    tokPATTERN
      {
      }
  ;

/* --- Miscellaneous -------------------------- */

identifier_list:
    qualified_identifier
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
          $$ = new parserid_list();
          $$->push_back( * $1 );
          delete $1;
      #endif
      }

  | identifier_list ',' qualified_identifier
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
          $$ = $1;
          $$->push_back( * $3);
          delete $3;
      #endif
      }
  ;

qualified_identifier:
    tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
          $$ = $1;
      #endif
      }

  | tokIO tokID
      {
      #ifdef ROSE_ENABLE_SOURCE_ANALYSIS
          $$ = $2;
          $$->set_io();
      #endif
      }
  ;

/* --- C constant expression ------------------ */

%%
