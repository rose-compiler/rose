/* unparse_sym.C
 * This C file contains the general function to unparse symbols as well as
 * functions to unparse every kind of symbol.
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparser.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

//-----------------------------------------------------------------------------------
//  void Unparser::unparseSymbol
//
//  General function that gets called when unparsing a symbol. Then it routes
//  to the appropriate function to unparse each kind of symbol.
//-----------------------------------------------------------------------------------
void
Unparse_Sym::unparseSymbol(SgSymbol* sym, SgUnparse_Info& info)
   {
     ASSERT_not_null(sym);
    
     switch(sym->variant())
        {
          case VARIABLE_NAME:    { unparseVarSymbol(sym, info); break; }
          case FUNCTION_NAME:    { unparseFunctionSymbol(sym, info); break; }
          case FUNCTYPE_NAME:    { unparseFuncTypeSymbol(sym, info); break; }
          case CLASS_NAME:       { unparseClassSymbol(sym, info); break; }
          case ENUM_NAME:        { unparseEnumSymbol(sym, info); break; }
          case FIELD_NAME:       { unparseFieldSymbol(sym, info); break; }
          case TYPEDEF_NAME:     { unparseTypedefSymbol(sym, info); break; }
          case MEMBER_FUNC_NAME: { unparseMFunctionSymbol(sym, info); break; }
          case LABEL_NAME:       { unparseLabelSymbol(sym, info); break; }
          case TEMPLATE_NAME:    { unparseTemplateSymbol(sym, info); break; }
          case NAMESPACE_NAME:   { unparseNamespaceSymbol(sym, info); break; }
          default:
             {
               printf ("Default reached in Unparse_Sym::unparseSymbol(sym = %p = %s,info) \n",sym,sym->class_name().c_str());
               unp->u_debug->printDebugInfo("Abort: Unknown symbol", true);
               ROSE_ABORT();
               break;
             }
        }
   }

void Unparse_Sym::unparseVarSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparse_Sym::unparseFunctionSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparse_Sym::unparseFuncTypeSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparse_Sym::unparseClassSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
// void Unparse_Sym::unparseUnionSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
// void Unparse_Sym::unparseStructSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparse_Sym::unparseEnumSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparse_Sym::unparseFieldSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparse_Sym::unparseTypedefSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparse_Sym::unparseMFunctionSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparse_Sym::unparseLabelSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
// void Unparse_Sym::unparseConstructSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparse_Sym::unparseTemplateSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparse_Sym::unparseNamespaceSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
