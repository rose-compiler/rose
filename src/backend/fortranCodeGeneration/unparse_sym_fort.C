/* unparse_sym_fort.C
 *
 * Code to unparse Sage/Fortran symbol nodes.
 * 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <rose.h>
#include "unparser_fort.h"


//----------------------------------------------------------------------------
//  UnparserFort::unparseSymbol
//
//  General unparse function for symbols. Routes work to the
//  appropriate helper function.
//----------------------------------------------------------------------------

void 
UnparserFort::unparseSymbol(SgSymbol* sym, SgUnparse_Info& info) 
{
  ROSE_ASSERT(sym != NULL);
    
  switch (sym->variantT()) {
    
  case V_SgVariableSymbol:       unparseVarSymbol(sym, info);       break;
  case V_SgFunctionSymbol:       unparseFunctionSymbol(sym, info);  break;
  case V_SgMemberFunctionSymbol: unparseMFunctionSymbol(sym, info); break;
  case V_SgIntrinsicSymbol:      unparseIntrinsicSymbol(sym, info); break;
  case V_SgFunctionTypeSymbol:   unparseFuncTypeSymbol(sym, info);  break;
  case V_SgClassSymbol:          unparseClassSymbol(sym, info);     break;
  case V_SgEnumSymbol:           unparseEnumSymbol(sym, info);      break;
  case V_SgEnumFieldSymbol:      unparseFieldSymbol(sym, info);     break;
  case V_SgTypedefSymbol:        unparseTypedefSymbol(sym, info);   break;
  case V_SgLabelSymbol:          unparseLabelSymbol(sym, info);     break;
    
  default: 
    printf("UnparserFort::unparseSymbol: Error: No handler for %s (variant: %d)\n",
	   sym->sage_class_name(), sym->variantT());
    ROSE_ASSERT(false);
    break;
  }
}


//----------------------------------------------------------------------------
//  UnparserFort::<>
//----------------------------------------------------------------------------

void 
UnparserFort::unparseVarSymbol(SgSymbol* sym, SgUnparse_Info& info)
{
}

void 
UnparserFort::unparseFunctionSymbol(SgSymbol* sym, SgUnparse_Info& info)
{
}

void 
UnparserFort::unparseMFunctionSymbol(SgSymbol* sym, SgUnparse_Info& info)
{
}


void 
UnparserFort::unparseIntrinsicSymbol(SgSymbol* sym, SgUnparse_Info& info)
{
}

void 
UnparserFort::unparseFuncTypeSymbol(SgSymbol* sym, SgUnparse_Info& info)
{
}

void 
UnparserFort::unparseClassSymbol(SgSymbol* sym, SgUnparse_Info& info)
{
}

void 
UnparserFort::unparseEnumSymbol(SgSymbol* sym, SgUnparse_Info& info)
{
}

void 
UnparserFort::unparseFieldSymbol(SgSymbol* sym, SgUnparse_Info& info)
{
}

void 
UnparserFort::unparseTypedefSymbol(SgSymbol* sym, SgUnparse_Info& info)
{
}

void 
UnparserFort::unparseLabelSymbol(SgSymbol* sym, SgUnparse_Info& info)
{
}
