#ifndef FAST_TO_SG_CONVERTER_H
#define FAST_TO_SG_CONVERTER_H

#include "rosePublicConfig.h"
#include "sage3basic.h"

#include "FASTNodes.hpp"

class FASTtoSgConverter : public FAST::Converter
{
 public:
   FASTtoSgConverter();
   virtual ~FASTtoSgConverter();

 protected:
   SgUntypedFile* pUntypedFile;

 public:
   SgUntypedFile*        get_file()  { return pUntypedFile; }
   SgUntypedGlobalScope* get_scope() { return pUntypedFile->get_scope(); }

 public:
   virtual void convert_MainProgram(FAST::MainProgram* main_program);

 public:
   static void convert_scope_lists(FAST::Scope* function_scope, SgUntypedDeclarationStatementList* sg_decls,
                                                                SgUntypedStatementList* sg_stmts,
                                                                SgUntypedFunctionDeclarationList* sg_funcs);

};


#endif
