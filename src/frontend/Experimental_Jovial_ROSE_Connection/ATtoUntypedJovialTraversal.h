#ifndef AT_TO_UNTYPED_JOVIAL_TRAVERSAL_H
#define AT_TO_UNTYPED_JOVIAL_TRAVERSAL_H

#include <aterm2.h>
#include "string"
#include "vector"

#ifdef ROSE
#include "FASTNodes.hpp"
#else
class SgSourceFile;
class SgUntypedFile;
class SgUntypedScope;
class SgUntypedExpression;
class SgUntypedDeclarationStatementList;
class SgUntypedStatementList;
class SgUntypedFunctionDeclarationList;
#endif

namespace Jovial {

class ATtoUntypedJovialTraversal
{
 public:
   ATtoUntypedJovialTraversal(SgSourceFile* source);
   virtual ~ATtoUntypedJovialTraversal();

#if ROSE
   SgUntypedFile*        get_file()  { return pUntypedFile; }
   SgUntypedGlobalScope* get_scope() { return pUntypedFile->get_scope(); }

   std::string getCurrentFilename()
      {
         return pSourceFile->get_sourceFileNameWithPath();
      }

 protected:
   SgUntypedFile* pUntypedFile;
   SgSourceFile*  pSourceFile;
#endif

 public:

// 1.1
ATbool traverse_Module(ATerm term);

// 1.2.1 COMPOOL MODULES
ATbool traverse_CompoolModule(ATerm term, SgUntypedScope* scope);

// 1.2.2 PROCEDURE MODULES
ATbool traverse_ProcedureModule(ATerm term, SgUntypedScope* scope);
ATbool traverse_DeclarationList(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 1.2.3 MAIN PROGRAM MODULES
ATbool traverse_MainProgramModule(ATerm term, SgUntypedScope* scope);
ATbool traverse_Name(ATerm term, std::string & name);
ATbool traverse_ProgramBody(ATerm term, SgUntypedScope* scope);
ATbool traverse_NonNestedSubroutineList(ATerm term, SgUntypedScope* scope);
ATbool traverse_SubroutineDefinitionList(ATerm term, SgUntypedFunctionDeclarationList*);

// 2.1 DATA DECLARATIONS
ATbool traverse_DataDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list);

// 2.1.1 ITEM DECLARATION
ATbool traverse_ItemDeclaration(ATerm term, SgUntypedDeclarationStatementList* decl_list);
ATbool traverse_ItemTypeDescription(ATerm term /* TODO - return type */);

// 2.1.1.1 INTEGER TYPE DESCRIPTIONS
ATbool traverse_IntegerItemDescription(ATerm term /* TODO - return type */);
ATbool traverse_OptIntegerSize(ATerm term, bool* has_size /*TODO - return type */);

// 2.1.1.2 FLOATING TYPE DESCRIPTIONS

ATbool traverse_OptRoundOrTruncate(ATerm term, bool* has_round_or_truncate /*TODO - return type */);

// 2.1.5 ALLOCATION OF DATA OBJECTS
ATbool traverse_OptAllocationSpecifier(ATerm term, bool* has_spec, std::string & spec_string);

// 4.0 STATEMENTS
ATbool traverse_Statement(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_StatementList(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_SimpleStatement(ATerm term, SgUntypedStatementList* stmt_list);
ATbool traverse_LabelList(ATerm term, std::vector<std::string> & labels);

// 4.1 ASSIGNMENT STATEMENTS
ATbool traverse_AssignmentStatement(ATerm term, SgUntypedStatementList* stmt_list);

// 5.0 FORMULAS
ATbool traverse_Formula(ATerm term, SgUntypedExpression** expr);

// 5.1 NUMERIC FORMULAS
ATbool traverse_NumericFormula(ATerm term, SgUntypedExpression** expr);

// 5.1.1 INTEGER FORMULAS
ATbool traverse_IntegerFormula(ATerm term, SgUntypedExpression** expr);
ATbool traverse_IntegerPrimary(ATerm term, SgUntypedExpression** expr);
ATbool traverse_OptSign(ATerm term /*TODO - enum variable for sign */);

// 6.1 VARIABLE AND BLOCK REFERENCES
ATbool traverse_Variable(ATerm term, SgUntypedExpression** var);
ATbool traverse_VariableList(ATerm term, std::vector<SgUntypedExpression*> & vars);

}; // class ATtoUntypedJovialTraversal
}  // namespace Jovial

#endif
