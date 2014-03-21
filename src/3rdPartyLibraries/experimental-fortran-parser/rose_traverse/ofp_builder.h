#ifndef OFP_BUILDER_H
#define OFP_BUILDER_H

#include <aterm2.h>

namespace OFP {

typedef struct OFP_Traverse_struct
{
   ATerm   term;
   void *  pre;
   void *  post;
}
   OFP_Traverse, * pOFP_Traverse;

enum DataType
{
   UNKNOWN,
   STRING,
   IDENT,
   LIST,
   OPTION,
   CONST,
   OR
};

ATbool        traverse_init();
ATbool        traverse_finalize();

// symbols
//
ATerm         ofp_getUniqueSymbol (ATermTable table, ATerm symbol);

// aliases
//
ATerm         ofp_getAliasSymbol  (ATerm term);
ATerm         ofp_getAliasName    (ATerm term);
ATerm         ofp_getAliasType    (ATerm term);

ATbool        ofp_isStringType    (ATerm term);
ATbool        ofp_isIdentType     (ATerm term);
enum DataType ofp_getDataType     (ATerm term);
ATbool        ofp_isPrimitiveType (ATerm term);

char *        ofp_getChars        (ATerm term);

ATerm         ofp_getArgName      (ATerm term);
ATerm         ofp_getArgType      (ATerm term);
ATermList     ofp_getArgList         (ATermTable aliasTable, ATermTable symTable, ATermList alist);
ATermList     ofp_getArgListUnique   (ATermTable aliasTable, ATermTable symTable, ATermList alist);
ATermList     ofp_getArgListNotUnique(ATermTable aliasTable, ATermTable symTable, ATermList alist);

ATbool        traverse_Module    (ATerm term, pOFP_Traverse Module);
ATbool        ofp_traverse_Signature (ATerm term, pOFP_Traverse Signature);

// build functions
//
ATermList     ofp_build_production_table         (ATerm term);
ATermList     ofp_coalesceProdTable              (ATermList table);
ATermList     ofp_coalesceAliasTable             (ATermList table);

ATbool        ofp_build_match_args_decl          (FILE * fp, ATermList args);
ATbool        ofp_build_match_terminal           (FILE * fp, ATerm terminal);
ATbool        ofp_build_match_begin              (FILE * fp, ATerm symbol, ATerm constructor, ATermList args, ATermList uargs);
ATbool        ofp_build_match_end                (FILE * fp, ATerm symbol, ATerm constructor);
ATbool        ofp_build_match_sort_option_begin  (FILE * fp, ATerm symbol, ATbool aliased);
ATbool        ofp_build_match_sort_option_end    (FILE * fp, ATerm symbol);
ATbool        ofp_build_match_nonterminal_begin  (FILE * fp, ATerm constructor, ATerm symbol);
ATbool        ofp_build_match_nonterminal_end    (FILE * fp, ATerm constructor, ATerm symbol);

ATbool        ofp_build_traversal_class_decls   (FILE * fp, ATermList prodTable);
ATbool        ofp_build_traversal_class_decl    (FILE * fp, ATerm name);
ATbool        ofp_build_traversal_class_def        (FILE * fp, ATerm name, ATermList cons, ATermList args);
ATbool        ofp_build_traversal_class_destructor (FILE * fp, ATerm name, ATermList cons, ATermList args);

ATbool        ofp_build_traversal_func_header   (FILE * fp, ATerm name);
ATbool        ofp_build_traversal_func_begin    (FILE * fp, ATerm name);
ATbool        ofp_build_traversal_func_end      (FILE * fp, ATerm name, ATbool returnTrue);
ATbool        ofp_build_traversal_production    (FILE * fp, ATermTable aliasTable, ATerm symbol, ATerm constructor, ATermList prod_symbols, ATermList args);
ATbool        ofp_build_traversal_nonterminal   (FILE * fp, ATerm symbol, ATerm prod_symbol, ATerm unique_sym);
ATbool        ofp_build_traversal_list          (FILE * fp, ATerm symbol, ATerm prod_symbol, ATerm unique_sym);

ATbool        ofp_traverse_Constructors         (ATerm term, pOFP_Traverse Constructors);

} // namespace OFP

#endif // OFP_BUILDER_H
