// Example ROSE Translator: used for testing ROSE infrastructure

#include "rose.h"
#include <sageDoxygen.h>
#include <list>
#include <commandline_processing.h>
#include <string>

using namespace std;

class OrganizeAllComments : public AstSimpleProcessing 
   {

     public:
          virtual void visit(SgNode *n) 
             {
               SgDeclarationStatement *ds = isSgDeclarationStatement(n);
               SgFunctionDeclaration *fn = isSgFunctionDeclaration(n);
               if (!Doxygen::isRecognizedDeclaration(ds)) return;
               if (fn) 
                  {
                    list<DoxygenComment *> *commentList = Doxygen::getCommentList(fn);
                    bool isVoid = fn->get_type()->get_return_type()->variantT() == V_SgTypeVoid;
                    for (list<DoxygenComment *>::iterator i = commentList->begin(); i != commentList->end(); ++i) 
                       {
                         DoxygenComment *c = *i;
                         if (isVoid) 
                            {
                              c->attach(c->originalFile, c->originalFile->group("Functions returning void"));
                            }
                         else 
                            {
                              c->attach(c->originalFile);
                            }
                       }
                  }
             }
   };

int main( int argc, char * argv[] ) 
   {
     CommandlineProcessing::addCppSourceFileSuffix("docs")
     CommandlineProcessing::addCppSourceFileSuffix("h");

  // Build the AST used by ROSE
     SgProject* sageProject = frontend(argc,argv);

     Doxygen::annotate(sageProject);

     OrganizeAllComments oac;
     oac.traverse(sageProject, preorder);

  // Generate source code from AST and call the vendor's compiler
     sageProject->unparse();
   }

