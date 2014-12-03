#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#include "OFPUnparser.hpp"
#include <assert.h>

#define UNPARSER_AVAILABLE 0

void OFP::FortranTextUnparser::unparseNode(SgUntypedNode * node)
{
   if (!node) return;

   switch (node->variantT())
      {
         case V_SgUntypedFile:
            {
               SgUntypedFile * file = isSgUntypedFile(node);  assert(file);
               SgUntypedGlobalScope* scope = file->get_scope();

               ROSE_ASSERT(scope != NULL);
               ROSE_ASSERT(scope->get_declaration_list() != NULL);
               SgUntypedDeclarationStatementPtrList programUnitList = scope->get_declaration_list()->get_decl_list();
               for (int i = 0; i < programUnitList.size(); i++) {
                  SgUntypedDeclarationStatement * decl = isSgUntypedDeclarationStatement(programUnitList.at(i));  assert(decl);
#if UNPARSER_AVAILABLE
                  decl->unparse(oss);
#endif
               }
               break;
            }
        default:
#if UNPARSER_AVAILABLE
           if (node->get_has_unparse()) {
              node->unparse(oss);
           }
           else {
              std::cerr << "OFPUnparser::unparseNode::::::::::::::::: virtual method unparse NEEDS IMPLEMENTATION!\n";
              assert(0);
           }
#else
           printf ("Exiting if UNPARSER_AVAILABLE == 0 \n");
           ROSE_ASSERT(false);
#endif
      }
}

//static void OFP::FortranTextUnparser::unparseOpEnum(SgToken::ROSE_Fortran_Operators e)
static void unparseOpEnum(std::ostream & oss, SgToken::ROSE_Fortran_Operators e)
{
   switch (e)
      {
         case SgToken::FORTRAN_INTRINSIC_PLUS:    oss << " + ";      break;
         case SgToken::FORTRAN_INTRINSIC_MINUS:   oss << " - ";      break;
         case SgToken::FORTRAN_INTRINSIC_POWER:   oss << " ** ";     break;
         case SgToken::FORTRAN_INTRINSIC_CONCAT:  oss << " // ";     break;
         case SgToken::FORTRAN_INTRINSIC_TIMES:   oss << " * ";      break;
         case SgToken::FORTRAN_INTRINSIC_DIVIDE:  oss << " / ";      break;
         case SgToken::FORTRAN_INTRINSIC_AND:     oss << " .AND. ";  break;
         case SgToken::FORTRAN_INTRINSIC_OR:      oss << " .OR.  ";  break;
         case SgToken::FORTRAN_INTRINSIC_EQV:     oss << " .EQV. ";  break;
         case SgToken::FORTRAN_INTRINSIC_NEQV:    oss << " .NEQV. "; break;
         case SgToken::FORTRAN_INTRINSIC_EQ:      oss << " = ";      break;
         case SgToken::FORTRAN_INTRINSIC_NE:      oss << " /= ";     break;
         case SgToken::FORTRAN_INTRINSIC_GE:      oss << " >= ";     break;
         case SgToken::FORTRAN_INTRINSIC_LE:      oss << " <= ";     break;
         case SgToken::FORTRAN_INTRINSIC_LT:      oss << " < ";      break;
         case SgToken::FORTRAN_INTRINSIC_GT:      oss << " > ";      break;
         case SgToken::FORTRAN_INTRINSIC_NOT:     oss << " .NOT. ";  break;
         case SgToken::FORTRAN_INTRINSIC_OLDEQ:   oss << " .EQ. ";   break;
         case SgToken::FORTRAN_INTRINSIC_OLDNE:   oss << " .NE. ";   break;
         case SgToken::FORTRAN_INTRINSIC_OLDGE:   oss << " .GE. ";   break;
         case SgToken::FORTRAN_INTRINSIC_OLDLE:   oss << " .LE. ";   break;
         case SgToken::FORTRAN_INTRINSIC_OLDLT:   oss << " .LT. ";   break;
         case SgToken::FORTRAN_INTRINSIC_OLDGT:   oss << " .GT. ";   break;
      }
}

static void unparseLabel(std::ostream & oss, std::string label)
{
   if (label.length() > 0) {
      oss << label << " ";
   }
}

#if UNPARSER_AVAILABLE
void SgUntypedProgramHeaderDeclaration::unparse(std::ostream & oss)
{
   SgUntypedFunctionScope * scope = get_scope();

   SgUntypedDeclarationStatementPtrList specList = scope->get_declaration_list()->get_decl_list();
   SgUntypedStatementPtrList            execList = scope->get_statement_list()->get_stmt_list();
   SgUntypedFunctionDeclarationPtrList  isubList = scope->get_function_list()->get_func_list();

   unparseLabel(oss, get_label_string());

   //TODO-DQ-2014.3.21 add has_program_statement member variable
   if (get_name().size() > 0) {
      oss << "PROGRAM" << " " << get_name() << "\n";
   }

   for (int i = 0; i < specList.size(); i++) {
      specList.at(i)->unparse(oss);
   }
   for (int i = 0; i < execList.size(); i++) {
      execList.at(i)->unparse(oss);
   }
   for (int i = 0; i < isubList.size(); i++) {
      isubList.at(i)->unparse(oss);
   }

   get_end_statement()->unparse(oss);
}

void SgUntypedModuleDeclaration::unparse(std::ostream & oss)
{
   SgUntypedModuleScope * scope = get_scope();

   SgUntypedDeclarationStatementPtrList specList = scope->get_declaration_list()->get_decl_list();
   SgUntypedStatementPtrList            stmtList = scope->get_statement_list()->get_stmt_list();
   SgUntypedFunctionDeclarationPtrList  msubList = scope->get_function_list()->get_func_list();
                 
   //TODO-CER-2014.4.12 - needs prefix, args, suffix, ...

   unparseLabel(oss, get_label_string());

   oss << "MODULE" << " " << get_name() << "\n";

   for (int i = 0; i < specList.size(); i++) {
      specList.at(i)->unparse(oss);
   }
   for (int i = 0; i < stmtList.size(); i++) {
      stmtList.at(i)->unparse(oss);
   }
   for (int i = 0; i < msubList.size(); i++) {
      msubList.at(i)->unparse(oss);
   }
   
   get_end_statement()->unparse(oss);
}

void SgUntypedSubroutineDeclaration::unparse(std::ostream & oss)
{
   SgUntypedFunctionScope * scope = get_scope();

   SgUntypedDeclarationStatementPtrList specList = scope->get_declaration_list()->get_decl_list();
   SgUntypedStatementPtrList            execList = scope->get_statement_list()->get_stmt_list();
   SgUntypedFunctionDeclarationPtrList  isubList = scope->get_function_list()->get_func_list();
                 
   //TODO-CER-2014.4.12 - needs prefix, args, suffix, ...

   unparseLabel(oss, get_label_string());

   oss << "SUBROUTINE" << " " << get_name() << "\n";

   for (int i = 0; i < specList.size(); i++) {
      specList.at(i)->unparse(oss);
   }
   for (int i = 0; i < execList.size(); i++) {
      execList.at(i)->unparse(oss);
   }
   for (int i = 0; i < isubList.size(); i++) {
      isubList.at(i)->unparse(oss);
   }
   
   get_end_statement()->unparse(oss);
}

void SgUntypedMpSubprogramDeclaration::unparse(std::ostream & oss)
{
   SgUntypedFunctionScope * scope = get_scope();

   SgUntypedDeclarationStatementPtrList specList = scope->get_declaration_list()->get_decl_list();
   SgUntypedStatementPtrList            execList = scope->get_statement_list()->get_stmt_list();
   SgUntypedFunctionDeclarationPtrList  isubList = scope->get_function_list()->get_func_list();
                 
   unparseLabel(oss, get_label_string());

   oss << "MODULE PROCEDURE" << " " << get_name() << "\n";

   for (int i = 0; i < specList.size(); i++) {
      specList.at(i)->unparse(oss);
   }
   for (int i = 0; i < execList.size(); i++) {
      execList.at(i)->unparse(oss);
   }
   for (int i = 0; i < isubList.size(); i++) {
      isubList.at(i)->unparse(oss);
   }
   
   get_end_statement()->unparse(oss);
}

void SgUntypedFunctionDeclaration::unparse(std::ostream & oss)
{
   SgUntypedFunctionScope * scope = get_scope();

   SgUntypedDeclarationStatementPtrList specList = scope->get_declaration_list()->get_decl_list();
   SgUntypedStatementPtrList            execList = scope->get_statement_list()->get_stmt_list();
   SgUntypedFunctionDeclarationPtrList  isubList = scope->get_function_list()->get_func_list();

   //TODO-CER-2014.4.12 - needs prefix, args, suffix, ...

   unparseLabel(oss, get_label_string());

   oss << "FUNCTION " << get_name() << "()" << "\n";

   for (int i = 0; i < specList.size(); i++) {
      specList.at(i)->unparse(oss);
   }
   for (int i = 0; i < execList.size(); i++) {
      execList.at(i)->unparse(oss);
   }
   for (int i = 0; i < isubList.size(); i++) {
      isubList.at(i)->unparse(oss);
   }
   
   get_end_statement()->unparse(oss);
}

void SgUntypedImplicitDeclaration::unparse(std::ostream & oss)
{
   unparseLabel(oss, get_label_string());
   oss << "IMPLICIT";
   if (get_statement_enum() == SgToken::FORTRAN_IMPLICIT_NONE) oss << " NONE";
   else                                                        oss << " TODO";
   oss << "\n";
}

// Statements
//

void SgUntypedNamedStatement::unparse(std::ostream & oss)
{
   switch (get_statement_enum())
      {
        case SgToken::FORTRAN_END_PROGRAM:
           {
              unparseLabel(oss, get_label_string());
              oss << "END PROGRAM ";
              oss << get_statement_name();
              oss << "\n";
              break;
           }
        case SgToken::FORTRAN_END_MODULE:
           {
              unparseLabel(oss, get_label_string());
              oss << "END MODULE ";
              oss << get_statement_name();
              oss << "\n";
              break;
           }
        case SgToken::FORTRAN_END_SUBROUTINE:
           {
              unparseLabel(oss, get_label_string());
              oss << "END SUBROUTINE ";
              oss << get_statement_name();
              oss << "\n";
              break;
           }
        case SgToken::FORTRAN_END_FUNCTION:
           {
              unparseLabel(oss, get_label_string());
              oss << "END FUNCTION ";
              oss << get_statement_name();
              oss << "\n";
              break;
           }
        case SgToken::FORTRAN_END_MP_SUBPROGRAM:
           {
              unparseLabel(oss, get_label_string());
              oss << "END PROCEDURE ";
              oss << get_statement_name();
              oss << "\n";
              break;
           }

        default:
           oss << "FortranTextUnparser::UnparseSgUntypedNamedStatement:::::::::::::::::::::::: " << get_statement_enum() << "\n";
      }
}

void SgUntypedVariableDeclaration::unparse(std::ostream & oss)
{
   SgUntypedInitializedNamePtrList names = get_parameters()->get_name_list();
   assert(names.size() > 0);
   oss << names.at(0)->get_type()->get_type_name() << " ";
   for (int i = 0; i < names.size(); i++) {
      oss << names.at(i)->get_name();
      if (i < names.size() - 1) oss << ", ";
   }
   oss << "\n";
}

// Statements
//

void SgUntypedStatement::unparse(std::ostream & oss)
{
   switch (get_statement_enum()) {
      case SgToken::FORTRAN_CONTAINS:   oss << "CONTAINS" << "\n";   break;
      default:
         oss << "FortranTextUnparser::UnparseUnknownStmt enum::::::::::::::::::: "
             << get_statement_enum() << "\n";
   }
}

void SgUntypedAssignmentStatement::unparse(std::ostream & oss)
{
   get_lhs_operand()->unparse(oss);
   oss << " = ";
   get_rhs_operand()->unparse(oss);
   oss << "\n";
}

void SgUntypedReferenceExpression::unparse(std::ostream & oss)
{
   oss << get_name();
}

void SgUntypedValueExpression::unparse(std::ostream & oss)
{
   oss << get_value_string();
   if (get_type() && get_type()->get_has_kind()) {
      // TODO kind should be an expression
      oss << "_";
      get_type()->get_type_kind()->unparse(oss);
   }
}

void SgUntypedBinaryOperator::unparse(std::ostream & oss)
{
   get_lhs_operand()->unparse(oss);
   unparseOpEnum (oss, get_operator_enum());
   get_rhs_operand()->unparse(oss);
}
#endif
