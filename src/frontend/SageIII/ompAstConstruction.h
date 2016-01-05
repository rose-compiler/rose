#ifndef _OMP_AST_CONSTRUCTION
#define _OMP_AST_CONSTRUCTION

#include "AstSimpleProcessing.h"

namespace OmpSupport
{
    class SgVarRefExpVisitor : public AstSimpleProcessing {
        private:
            std::vector<SgExpression*> expressions;
            
        public:
            SgVarRefExpVisitor( );
            std::vector<SgExpression*> get_expressions();
            void visit( SgNode* node );
    };
    
  void processOpenMP(SgSourceFile* sageFilePtr);

  // Special handling to wrap statements in between "target begin" and "target end" OmpAttribute into a block.
  // "target end" attribute will later be skipped when creating dedicate OMP statement.  
  void  postParsingProcessing (SgSourceFile* sageFilePtr); 
} // end of the namespace 
#endif
