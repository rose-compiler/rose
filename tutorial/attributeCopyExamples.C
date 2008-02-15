#include <rose.h>

using namespace std;

// This is an example of inherited attributes.  Here we prevent the traversal going any
// deeper than 20 levels.
class InheritedAttributeExample : public SgCopyHelp
   {

     static const int maxLevel = 20;
     int level;

     InheritedAttributeExample(int level) : level(level)
     {
     }

     public:

     InheritedAttributeExample() : level(0)
     {
     }

     SgNode *copyAst(const SgNode *n)
        {
          if (level < maxLevel)
             {
               InheritedAttributeExample subCopy(level+1);
               return n->copy(subCopy);
             }
          else
             {
               return const_cast<SgNode *>(n);
             }
        }
   };


// This is an example of synthesized attributes.  Here we keep a list of all the global
// veriable declarations that are created during the copy.
class SynthesizedAttributeExample : public SgCopyHelp
   {

     list<SgVariableDeclaration *> varDecls;

     public:

     SgNode *copyAst(const SgNode *n)
        {
          SynthesizedAttributeExample subCopy;
          SgNode *newN = n->copy(subCopy);
          if (!isSgFunctionDefinition(newN) && !isSgClassDefinition(newN))
             {
               varDecls.insert(varDecls.end(), subCopy.varDecls.begin(), subCopy.varDecls.end());
             }
          if (SgVariableDeclaration *newVarDecl = isSgVariableDeclaration(newN))
             {
               varDecls.push_back(newVarDecl);
             }
          return newN;
        }
   };

// This is an example of accumulator attributes.  Here we count the number of if statements
// found during the copy.
class AccumulatorAttributeExample : public SgCopyHelp
   {

     int ifCount;

     public:

     AccumulatorAttributeExample() : ifCount(0)
     {
     }

     SgNode *copyAst(const SgNode *n)
        {
          if (isSgIfStmt(n))
             {
               ifCount++;
             }
          return n->copy(*this);
        }
   };
