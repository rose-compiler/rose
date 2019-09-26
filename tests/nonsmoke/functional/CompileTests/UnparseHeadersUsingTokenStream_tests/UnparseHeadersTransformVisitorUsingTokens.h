#include <string>

using namespace std;

class UnparseHeadersTransformVisitor : public AstSimpleProcessing 
   {
     private:
          static const string matchEnding;
          static const size_t matchEndingSize;
          static const string renameEnding;

     protected:
          void visit(SgNode* astNode);
   };
