/*
 * Parsing OpenMP pragma text
by Liao, 9/17/2008
Last Modified: 9/19/2008
*/
#include "rose.h"
#include <iostream>
#include <string>
#include "OmpAttribute.h"
using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace OmpSupport;

/* Don't know a better place for them 
 * Bison does not generate prototypes for them by default
 * */
extern int omp_parse();
extern OmpAttribute* getParsedDirective();
extern void omp_parser_init(SgNode* aNode, const char* str);

class visitorTraversal : public AstSimpleProcessing
{
  protected: 
    virtual void visit(SgNode* n);
};

void visitorTraversal::visit(SgNode* node)
{
  // recognize pragma declaration and call the parser
  // #pragma omp parallel
  // is stored as "omp parallel" for pragma string
  SgPragmaDeclaration* pragmaDecl = isSgPragmaDeclaration (node); 
  if (pragmaDecl)
  {
   // cout<<" Found a pragma declaration at line: " 
   //    << pragmaDecl->get_startOfConstruct()->get_line()
   //    <<endl;
    string pragmaString = pragmaDecl->get_pragma()->get_pragma();  
    istringstream istr(pragmaString);   
    std::string key;
    istr >> key;
    //nextNode provides some context information 
    //Most is the structured block affected by the pragma
    //But not all pragmas have associated block,e.g. threadprivate(xx)
    //SgStatement* nextNode = getnextStatement(pragmaDecl);
    // ROSE_ASSERT(nextNode);
    if (key == "omp")
    {
      //cout<<"Found an OpenMP pragma:"<<pragmaString<<endl;
      
      // Call parser
      omp_parser_init(pragmaDecl,pragmaString.c_str()); 
      omp_parse();
      OmpAttribute* attribute = getParsedDirective();
      attribute->print();//debug only for now
      addOmpAttribute(attribute,pragmaDecl); 
    }  
  } // end if
}

// must have argc and argv here!!
int main(int argc, char * argv[])

{
  SgProject *project = frontend (argc, argv);
  visitorTraversal myvisitor;
  myvisitor.traverseInputFiles(project,preorder);
//  myvisitor.traverse(project,preorder);

// Generate source code from AST and call the vendor's compiler
  return backend(project);
}
