#include <rose.h>

class TransformVariableDeclarationTraversal : public SgSimpleProcessing 
   {
     public:
          bool transformed;
  
          TransformVariableDeclarationTraversal() : transformed(false) {};
          void visit(SgNode* astNode);
   };


void TransformVariableDeclarationTraversal::visit(SgNode* node) 
   {
#if 0
     printf ("astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

     if (SgVariableDeclaration* vardecl = isSgVariableDeclaration(node)) 
        {
          SgInitializedNamePtrList& vars = vardecl->get_variables();
          SgInitializedNamePtrList::iterator var;
          for (var = vars.begin(); var != vars.end(); ++var) 
             {
               SgInitializedName* initName = *var;
               if (initName->get_initializer() == NULL) 
                  {
#if 1
                    if (!vardecl->get_declarationModifier().get_storageModifier().isExtern()
                        && !vardecl->get_declarationModifier().get_storageModifier().isStatic()
                        && !isSgClassDefinition(vardecl->get_parent())
                        && !isSgGlobal(vardecl->get_parent())) 
                       {
                      // bool localTransformed = makeDefaultInitExp(initName);
                      // if (localTransformed) 
                            {
                           // transformed = true;

                              printf ("Transformation of SgVariableDeclaration : SgInitializedName : initName = %p = %s \n",initName,initName->get_name().str());

                              std::string filename  = initName->get_file_info()->get_filenameString();
                              int lineNumber   = initName->get_file_info()->get_line();
                              int columnNumber = initName->get_file_info()->get_col();

                              printf ("   --- location: \n");
                              printf ("   --- --- file = %s \n",filename.c_str());
                              printf ("   --- --- line = %d column = %d \n",lineNumber,columnNumber);
#if 1
                              initName->set_initializer(NULL);
#endif
                            }
                       }
#endif
                  }
             }
        }
   }


int main(int argc, char *argv[]) 
   {
  // Initialization
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project!=NULL);
  
     TransformVariableDeclarationTraversal treeTraversal;

  // DQ (12/21/2018): I think this needs to be specified to be post-order.
     treeTraversal.traverseInputFiles(project, preorder);

  // Only output code if there was a transformation that was done.
     return backend(project);

#if 0
     if (treeTraversal.transformed == true)
        {
         return backend(project);
        } 
       else
        {
          printf ("No changes were made, so no results were generated \n");
          return 0;
        }
#endif
   }
