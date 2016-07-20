// Example ROSE Translator: used within ROSE/tutorial

#include "rose.h"

using namespace std;

int main( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

  // Build a list of functions within the AST
     Rose_STL_Container<SgNode*> templateInstantiationDeclList = 
          NodeQuery::querySubTree (project,V_SgTemplateInstantiationDecl);

     int classTemplateCounter = 0;
     for (Rose_STL_Container<SgNode*>::iterator i = templateInstantiationDeclList.begin(); 
          i != templateInstantiationDeclList.end(); i++)
        {
          SgTemplateInstantiationDecl* instantiatedTemplateClass = isSgTemplateInstantiationDecl(*i);
          ROSE_ASSERT(instantiatedTemplateClass != NULL);

       // output the function number and the name of the function
          printf ("Class name #%d is %s \n",
               classTemplateCounter++,
               instantiatedTemplateClass->get_templateName().str());

          const SgTemplateArgumentPtrList& templateParameterList = instantiatedTemplateClass->get_templateArguments();
          int parameterCounter = 0;
          for (SgTemplateArgumentPtrList::const_iterator j = templateParameterList.begin(); 
               j != templateParameterList.end(); j++)
             {
               printf ("   TemplateArgument #%d = %s \n",parameterCounter++,(*j)->unparseToString().c_str());
             }
        }

     return 0;
   }

