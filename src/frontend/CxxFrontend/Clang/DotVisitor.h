
#include "clang/AST/RecursiveASTVisitor.h"

class DotVisitor : public clang::RecursiveASTVisitor<DotVisitor> {
     protected:

     public:
         DotVisitor();
         virtual ~DotVisitor();

};
