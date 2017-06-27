#ifndef FAST_TO_SG_CONVERTER_H
#define FAST_TO_SG_CONVERTER_H

#include "FASTNodes.hpp"

class FASTtoSgConverter : public FAST::Converter
{
 public:
   FASTtoSgConverter(SgSourceFile* source);
   virtual ~FASTtoSgConverter();

   SgUntypedFile*        get_file()  { return pUntypedFile; }
   SgUntypedGlobalScope* get_scope() { return pUntypedFile->get_scope(); }

   virtual void convert_MainProgram(FAST::MainProgram* main_program);

   void setSourcePosition( SgLocatedNode* locatedNode, FAST::PosInfo & pos );
   void setSourcePosition( SgLocatedNode* locatedNode, FAST::PosInfo & start, FAST::PosInfo & end );

   std::string getCurrentFilename()
      {
         return pSourceFile->get_sourceFileNameWithPath();
      }

 private:
   void convert_scope_lists(FAST::Scope* function_scope, SgUntypedDeclarationStatementList* sg_decls,
                                                         SgUntypedStatementList* sg_stmts,
                                                         SgUntypedFunctionDeclarationList* sg_funcs);

 protected:
   SgUntypedFile* pUntypedFile;
   SgSourceFile*  pSourceFile;

};


#endif
