#include "sage3basic.h"
#include "checkPhysicalSourcePosition.h"
#include "stringify.h"

using namespace Rose;

// documented in header file
size_t
checkPhysicalSourcePosition(SgNode *ast)
   {
     struct T1: public AstSimpleProcessing 
        {
          size_t nviolations;
          T1(): nviolations(0) {}

          void visit(SgNode *node)
             {
               SgLocatedNode *located = isSgLocatedNode(node);
               if (located)
                  {
                    check(located, located->get_file_info());
                    check(located, located->generateMatchingFileInfo());
                    check(located, located->get_startOfConstruct());
                    check(located, located->get_endOfConstruct());
                  }
             }

       // Mark node as compiler generated and emit a warning if it wasn't already so marked.
          void check(SgNode *node, Sg_File_Info *finfo)
             {
               if (finfo != NULL)
                  {
                    if (finfo->get_file_id() >= 0 && finfo->get_physical_file_id() < 0)
                       {
                         SgNode* parent = finfo->get_parent();
                         ROSE_ASSERT(parent != NULL);
                         printf ("Detected inconsistant physical source position information: %p parent = %p = %s \n",finfo,parent,parent->class_name().c_str());
                         finfo->display("checkPhysicalSourcePosition()");

                         ROSE_ASSERT(false);

                         ++nviolations;
                       }
                  }
             }
        } t1;

     t1.traverse(ast, preorder);
     return t1.nviolations;
   }

                
