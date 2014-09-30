// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "collectAssociateNodes.h"
#include "test_support.h"
#include "merge_support.h"

// #include "colorTraversal.h"

using namespace std;

// set<SgNode*> firstAST_globalVar;

#if 0
// To test the merge process we make a copy of the AST built from the input file and 
// try to merge the copied AST with the original AST.
SgNode*
copyAST ( SgNode* node )
   {
     ROSE_ASSERT(node != NULL);

  // This is a better implementation using a derived class from SgCopyHelp to control the 
  // copying process (skipping the copy of any function definition).  This is a variable 
  // declaration with an explicitly declared class type.
     class RestrictedCopyType : public SgCopyHelp
        {
       // DQ (9/26/2005): This class demonstrates the use of the copy mechanism 
       // within Sage III (originally designed and implemented by Qing Yi).
       // One problem with it is that there is no context information permitted.

          public:
               virtual SgNode *copyAst(const SgNode *n)
                  {
                    SgNode *returnValue = n->copy(*this);
                    return returnValue;
                  }
        } restrictedCopyType;

     SgNode* copyOfNode = node->copy(restrictedCopyType);

     ROSE_ASSERT(copyOfNode != NULL);
     return copyOfNode;
   }
#endif


void
displaySet ( const set<SgNode*> & inputSet, const std::string & label )
   {
     printf ("In displaySet(inputSet.size() = %" PRIuPTR ", label = %s) \n",inputSet.size(),label.c_str());
     set<SgNode*>::const_iterator i = inputSet.begin();
     while ( i != inputSet.end() )
        {
          Sg_File_Info* fileInfo = (*i)->get_file_info();
          printf ("set element: i = %p = %s = %s = %s from file = %s \n",*i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str(),SageInterface::generateUniqueName(*i,false).c_str(),(fileInfo != NULL) ? fileInfo->get_filenameString().c_str() : "fileInfo == NULL");
          i++;
        }
   }


set<SgNode*>
generateNodeListFromAST ( SgNode* node )
   {
  // Generate a list of the SgNode* in the AST (subset of all IR nodes in the memory pool)
     class NodeListTraversal : public SgSimpleProcessing
        {
          public:
              set<SgNode*> nodeList;
              void visit (SgNode* n )
                 {
#ifndef USE_ROSE
                // DQ (2/8/2010): This code demonstrates a bug in the unparser for ROSE when run using tests/testCodeGeneration
                   ROSE_ASSERT(n != NULL);
                // printf ("In generateNodeListFromAST building nodeList n = %p = %s \n",n,n->class_name().c_str());
                   nodeList.insert(n);
#endif
                 }
        };

     NodeListTraversal t;
     t.traverse(node,preorder);

     return t.nodeList;
   }

set<SgNode*>
generateNodeListFromMemoryPool()
   {
  // Generate a list of the SgNode* in the memory pool (all IR nodes)
     class NodeListTraversal : public ROSE_VisitTraversal
        {
          public:
              set<SgNode*> nodeList;
              void visit (SgNode* n ) { nodeList.insert(n); }
              virtual ~NodeListTraversal() {}
        };

     NodeListTraversal t;
     t.traverseMemoryPool();

     return t.nodeList;
   }


set<SgNode*>
getSetOfSharedNodes()
   {
  // Generate a subset of the SgNode* in the memory pool (all IR nodes)
     class NodeListTraversal : public ROSE_VisitTraversal
        {
          public:
              set<SgNode*> nodeList;
              void visit (SgNode* n )
                 {
                   Sg_File_Info* fileInfo = NULL;
                   fileInfo = n->get_file_info();
                   if (fileInfo != NULL)
                      {
                        if (fileInfo->isShared() == true)
                           nodeList.insert(n);
                      }
                     else
                      {
                        fileInfo = isSg_File_Info(n);
                        if (fileInfo != NULL)
                           {
                             if (fileInfo->isShared() == true)
                                  nodeList.insert(n);
                           }
                      }
                 }

              virtual ~NodeListTraversal() {}
        };

     NodeListTraversal t;
     t.traverseMemoryPool();

     return t.nodeList;
   }

