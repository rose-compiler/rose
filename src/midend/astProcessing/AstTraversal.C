

// Author: Markus Schordan
// $Id: AstTraversal.C,v 1.3 2006/04/24 00:21:32 dquinlan Exp $

#ifndef ASTRESTRUCTURE_C
#define ASTRESTRUCTURE_C

#include "sage3basic.h"
#include "AstTraversal.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

void AstPreOrderTraversal::preOrderVisit(SgNode* node) {
  //cout << "Visiting : " << node->sage_class_name() << endl;
}

void AstPreOrderTraversal::setChildrenContainer(SgNode* node, std::vector<SgNode*>& c) {
  ROSE_ASSERT(node!=0);
  AstSuccessorsSelectors::selectDefaultSuccessors(node,c);
#if 0
  ostringstream ss;
  ss << "Successors of node " << node->sage_class_name() << " selected : " << c.size();
  cout << ss.str() << endl;
  cout << "Successors=(";
  
  int count=0;
  for (std::vector<SgNode*>::iterator i=c.begin(); i!=c.end(); i++) {
    ROSE_ASSERT(count<c.size());
    count++;
    cout << *i << " ";
  }
  cout << ")" << endl;
#endif
}

void AstPrePostOrderTraversal::setChildrenContainer(SgNode* node, std::vector<SgNode*>& c) {
  ROSE_ASSERT(node!=0);
  AstSuccessorsSelectors::selectDefaultSuccessors(node,c);
}

//! Determines whether the given sequence l of nodes extended by node creates a cycle.
//! The found cycle is returned. If no cycle is found, the returned list is empty.
list<SgNode*> AstCycleTest::determineCycle(list<SgNode*>& l, SgNode* node)
   {
     list<SgNode*> noCycle;
     list<SgNode*> cycle;
     cycle.push_front(node);
#if 0
     printf ("In AstCycleTest::determineCycle(l,node = %p = %s) l.size() = %" PRIuPTR " \n",node,node->class_name().c_str(),l.size());
#endif
     for (list<SgNode*>::reverse_iterator i = l.rbegin(); i != l.rend(); i++)
        {
#if 0
          printf ("node = %p = %s *i = %p = %s \n",node,node->class_name().c_str(),*i,(*i)->class_name().c_str());
#endif
          cycle.push_front(*i);
          if (node == *i)
             {
#if 0
               printf ("Found a match, return the cycle \n");
#endif
               return cycle;
             }
        }

     return noCycle;
   }

void AstCycleTest::preOrderVisit(SgNode* node) {
  activeNodes.push_back(node);
}

//! In case of a cycle the traversal does not continue to prevent an infinite recursion
//! of the traversal.
void 
AstCycleTest::setChildrenContainer(SgNode* node, std::vector<SgNode*>& c)
   {
     AstSuccessorsSelectors::selectDefaultSuccessors(node,c);
     modifyChildrenContainer(node,c);
#if 0
     for(list<SgNode*>::iterator i=activeNodes.begin();i!=activeNodes.end();i++)
        {
          if(*i!=0)
               cout << (*i)->sage_class_name() << " ";
            else
               cout << "null ";
        }
     cout << endl;
#endif

#if 0
     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     if (locatedNode != NULL)
        {
          Sg_File_Info* fileInfo = locatedNode->get_file_info();
          if (fileInfo != NULL)
             {
               printf ("In AstCycleTest::setChildrenContainer(): location = %s line = %d column = %d \n",
                    fileInfo->get_filename(),fileInfo->get_line(),fileInfo->get_col());
             }
            else
             {
               printf ("In AstCycleTest::setChildrenContainer(): fileInfo == NULL \n");
             }
        }
#endif
  
     for (std::vector<SgNode*>::iterator i=c.begin();i!=c.end();i++)
        {
#if 0
          printf ("for node = %p = %s list c.size() = %" PRIuPTR " \n",node,node->class_name().c_str(),c.size());
#endif
          if (*i != NULL)
             {
#if 0
               printf ("Calling determineCycle(activeNodes = %" PRIuPTR ", *i = %p = %s) \n",activeNodes.size(),*i,(*i)->class_name().c_str());
#endif
               list<SgNode*> cycle = determineCycle(activeNodes,*i);
               if (cycle.size() > 0)
                  {
                 // cycle found
                    cout << "CYCLE FOUND:";
                    for(list<SgNode*>::iterator j=cycle.begin();j!=cycle.end();j++)
                       {
                         string name = "default name";
                         SgInitializedName* initializedName = isSgInitializedName(*j);
                         if ( initializedName != NULL)
                              name = initializedName->get_name().str();
                         cout << (*j)->sage_class_name() << "(" << *j << "," << name << ") ";
                       }
                    cout << endl;
                  }
               ROSE_ASSERT(cycle.size() == 0);
             }
        }
   }

void AstCycleTest::postOrderVisit(SgNode* node) {
  activeNodes.pop_back();
}

#endif
