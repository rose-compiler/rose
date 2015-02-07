// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "collectAssociateNodes.h"
#include "test_support.h"
#include "merge_support.h"
#include "deleteOrphanNodes.h"

using namespace std;

void
BuildOrphanListMemoryPoolTraversal::visit ( SgNode* node)
   {
     ROSE_ASSERT(node != NULL);
  // printf ("BuildOrphanListMemoryPoolTraversal::visit: node = %s \n",node->class_name().c_str());

  // All nodes are in the parent list
     parentList.insert(node);

  // Add the SgProject to the child list since we want to keep that IR node in the intersection
     if (isSgProject(node) != NULL)
          childList.insert(node);

     bool nullifyIRchildren = true;

#if 0
  // Error: This causes too many IR nodes to be removed (e.g. SgTypedefSeq IR nodes will be removed)
  // Skip any children of types (since we will not often graph them anyway)
     if (isSgType(node) != NULL)
        {
          nullifyIRchildren = false;
        }
#endif

     if (nullifyIRchildren == true)
        {
       // Only traverse the new part of the AST
          typedef vector<pair<SgNode*,string> > DataMemberMapType;
          DataMemberMapType dataMemberMap = node->returnDataMemberPointers();

          DataMemberMapType::iterator i = dataMemberMap.begin();
          while (i != dataMemberMap.end())
             {
            // Ignore the parent pointer since it will be reset differently if required
               SgNode* childPointer = i->first;
               string  debugString  = i->second;

               if (childPointer != NULL)
                  {
                 // printf ("At node = %p = %s on edge %s found child %p = %s \n",node,node->class_name().c_str(),debugString.c_str(),childPointer,childPointer->class_name().c_str());
                    childList.insert(childPointer);
                  }
               i++;
             }
        }
   }


void
deleteOrphanIRnodesInMemoryPool()
   {
  // Some of these are build by the EDG/Sage translation and need to be fixed.
  // Others are build by the copy mechanism.

#if 1
     printf ("############### Skipping deleteOrphanIRnodesInMemoryPool ###############\n");
     return;
#endif

     BuildOrphanListMemoryPoolTraversal t;
     t.traverseMemoryPool();

     printf ("In deleteOrphanIRnodesInMemoryPool(): t.parentList.size() = %" PRIuPTR " t.childList.size() = %" PRIuPTR " \n",t.parentList.size(),t.childList.size());
     size_t size = t.parentList.size() > t.childList.size() ? t.parentList.size() : t.childList.size();
     std::vector<SgNode*> orphanList(size);
  // orphanList = set_difference(t.parentList.begin(),t.parentList.end(),t.childList.begin(),t.childList.end(),orphanList.begin());
  // set_difference(t.parentList.begin(),t.parentList.end(),t.childList.begin(),t.childList.end(),orphanList.begin());
  // set_difference(t.parentList.begin(),t.parentList.end(),t.childList.begin(),t.childList.end());
     std::vector<SgNode*>::iterator end = set_difference(t.parentList.begin(),t.parentList.end(),t.childList.begin(),t.childList.end(),orphanList.begin());
  // std::vector<SgNode*>::iterator end = set_intersection(t.parentList.begin(),t.parentList.end(),t.childList.begin(),t.childList.end(),orphanList.begin());
  // std::vector<SgNode*>::iterator end = set_symmetric_difference(t.parentList.begin(),t.parentList.end(),t.childList.begin(),t.childList.end(),orphanList.begin());
     orphanList.erase(end,orphanList.end());

#if 0
     std::set<SgNode*>::iterator parent_i = t.parentList.begin();
     while (parent_i != t.parentList.end())
        {
       // if (isSg_File_Info(*parent_i) != NULL)
       //      printf ("parent i = %p = %s \n",*parent_i,(*parent_i)->class_name().c_str());
          printf ("parent i = %p = %s \n",*parent_i,(*parent_i)->class_name().c_str());
          parent_i++;
        }
     std::set<SgNode*>::iterator child_i = t.childList.begin();
     while (child_i != t.childList.end())
        {
       // if (isSg_File_Info(*child_i) != NULL)
       //      printf ("child i = %p = %s \n",*child_i,(*child_i)->class_name().c_str());
          printf ("child i = %p = %s \n",*child_i,(*child_i)->class_name().c_str());
          child_i++;
        }
#endif

     printf ("In deleteOrphanIRnodesInMemoryPool(): deleting orphanList size = %" PRIuPTR " \n",orphanList.size());

     std::vector<SgNode*>::iterator i = orphanList.begin();
     while (i != orphanList.end())
        {
#if 0
          printf ("deleting set_difference i = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
#if 0
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(*i);
          if (classDeclaration != NULL)
             {
               printf ("Deleting classDeclaration = %p = %s \n",classDeclaration,classDeclaration->class_name().c_str());
            // if (classDeclaration == classDeclaration->get_definingDeclaration())
                  {
                    SgClassDefinition* classDefinition = classDeclaration->get_definition();
                    ROSE_ASSERT(classDefinition != NULL);
                    delete classDefinition;
                  }
             }
#endif

          printf ("deleteOrphanIRnodesInMemoryPool(): Deleting node = %p = %s = %s \n",*i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str());

          delete *i;
       // set<SgNode*> setOfIRnodes;
       // DeleteAST deleteTree(setOfIRnodes);
       // deleteTree.traverse(*i,postorder);

          i++;
        }

  // If there were orphans then some IR nodes were deleted and that could have cause more orphans, so rerun recursively.
     unsigned int orphanListSize = orphanList.size();
     if (orphanListSize > 0)
        {
          printf ("Recursive call to buildOrphanListMemoryPoolTraversal() \n");
          deleteOrphanIRnodesInMemoryPool();
        }
   }
