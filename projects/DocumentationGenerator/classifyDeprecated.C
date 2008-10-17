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
               if (!Doxygen::isRecognizedDeclaration(ds)) return;
               if (ds) 
                  {
                    list<DoxygenComment *> *commentList = Doxygen::getCommentList(ds);
                    for (list<DoxygenComment *>::iterator i = commentList->begin(); i != commentList->end(); ++i) 
                       {
                         DoxygenComment *c = *i;
                         if (c->originalFile == NULL)
                            {
                              continue;
                            }
                         if (c->entry.hasDeprecated())
                            {
                              c->attach(c->originalFile, c->originalFile->group("Deprecated functions"));
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
     vector<string> argvList(argv, argv + argc);

     CommandlineProcessing::addCppSourceFileSuffix("docs");
     CommandlineProcessing::addCppSourceFileSuffix("h");

     Doxygen::parseCommandLine(argvList);

     vector<string> newArgv = argvList;
     newArgv.insert(newArgv.begin() + 1, "-rose:collectAllCommentsAndDirectives");

  // Build the AST used by ROSE
     SgProject* sageProject = frontend(newArgv);

     Doxygen::annotate(sageProject);

     OrganizeAllComments oac;
     oac.traverse(sageProject, preorder);

  // Generate source code from AST and call the vendor's compiler
  // sageProject->unparse();
     Doxygen::unparse(sageProject);
   }

