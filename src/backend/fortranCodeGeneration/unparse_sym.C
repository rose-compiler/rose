/* unparse_sym.C
 * This C file contains the general function to unparse symbols as well as
 * functions to unparse every kind of symbol.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rose.h"
#include "unparser.h"

//-----------------------------------------------------------------------------------
//  void Unparser::unparseSymbol
//
//  General function that gets called when unparsing a symbol. Then it routes
//  to the appropriate function to unparse each kind of symbol.
//-----------------------------------------------------------------------------------
void Unparser::unparseSymbol(SgSymbol* sym, SgUnparse_Info& info) {
  ROSE_ASSERT(sym != NULL);
    
  switch(sym->variant()) {
  case VARIABLE_NAME: { unparseVarSymbol(sym, info); break; }
  case FUNCTION_NAME: { unparseFunctionSymbol(sym, info); break; }
  case FUNCTYPE_NAME: { unparseFuncTypeSymbol(sym, info); break; }
  case R_CLASS_NAME: { unparseClassSymbol(sym, info); break; }
#ifndef USE_SAGE3
  case UNION_NAME: { unparseUnionSymbol(sym, info); break; }
  case STRUCT_NAME: { unparseStructSymbol(sym, info); break; }
#endif
  case ENUM_NAME: { unparseEnumSymbol(sym, info); break; }
  case FIELD_NAME: { unparseFieldSymbol(sym, info); break; } 
  case TYPEDEF_NAME: { unparseTypedefSymbol(sym, info); break; }
  case MEMBER_FUNC_NAME: { unparseMFunctionSymbol(sym, info); break; }
  case LABEL_NAME: { unparseLabelSymbol(sym, info); break; } 
#ifndef USE_SAGE3
  case CONSTRUCT_NAME: { unparseConstructSymbol(sym, info); break; }
#endif
  default: {
    printDebugInfo("Abort: Unknown symbol", TRUE);
    ROSE_ABORT();
    break;
  }
  
  }
}

void Unparser::unparseVarSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparser::unparseFunctionSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparser::unparseFuncTypeSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparser::unparseClassSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparser::unparseUnionSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparser::unparseStructSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparser::unparseEnumSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparser::unparseFieldSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparser::unparseTypedefSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparser::unparseMFunctionSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparser::unparseLabelSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
void Unparser::unparseConstructSymbol(SgSymbol* sym, SgUnparse_Info& info) {}
