#ifndef _OMP_AST_CONSTRUCTION
#define _OMP_AST_CONSTRUCTION

#include "AstSimpleProcessing.h"

namespace OmpSupport
{
    class SgVarRefExpVisitor : public AstSimpleProcessing {
        private:
            std::vector<SgVarRefExp*> expressions;
            
        public:
            SgVarRefExpVisitor( );
            std::vector<SgVarRefExp*> get_expressions();
            void visit( SgNode* node );
    };
    
  void processOpenMP(SgSourceFile* sageFilePtr);
} // end of the namespace 
#endif
