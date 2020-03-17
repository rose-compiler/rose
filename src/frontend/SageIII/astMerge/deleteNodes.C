// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "collectAssociateNodes.h"
#include "test_support.h"
#include "merge_support.h"
using namespace std;

#define ROSE_MERGE_DELETE_VERBOSE 0
#define ROSE_MERGE_DELETE_DETAILS 0

void preDeleteTests ( set<SgNode*> & listToDelete );

struct AncestorSort {
  std::vector<SgNode *> roots;
  std::map< SgNode *, std::vector<SgNode *> > descendants; // direct descendants

  AncestorSort(std::vector<SgNode*> const & nodes) :
    roots(), descendants()
  {
    std::vector<SgNode *>::const_iterator i;
    for (i = nodes.begin(); i != nodes.end(); ++i) {
      SgNode * ni = *i;
      descendants.insert(std::pair< SgNode *, std::vector<SgNode *> >(ni, std::vector<SgNode *>()));
    }
    for (i = nodes.begin(); i != nodes.end(); ++i) {
      SgNode * ni = *i;
      SgNode * pi = ni->get_parent();
      std::map< SgNode *, std::vector<SgNode *> >::iterator d = descendants.find(pi);
      if (d != descendants.end()) {
        d->second.push_back(ni);
      } else {
        roots.push_back(ni);
      }
    }
  }

  void sort(std::vector<SgNode*> & nodes) {
    while (roots.size() > 0) {
      std::vector<SgNode *> next;
      for (std::vector<SgNode *>::const_iterator i = roots.begin(); i != roots.end(); ++i) {
        SgNode * r = *i;

        nodes.push_back(r);

        std::map< SgNode *, std::vector<SgNode *> >::const_iterator d = descendants.find(r);
        if (d != descendants.end()) {
          next.insert(next.end(), d->second.begin(), d->second.end());
        }
      }
      roots.clear();
      roots.insert(roots.end(), next.begin(), next.end());
    }
  }
};

void
deleteNodes ( std::set<SgNode*> & listToDelete )
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     TimingPerformance timer ("Delete the IR nodes that are replaced with shared IR nodes:");

     std::vector<SgNode*> nodes(listToDelete.begin(), listToDelete.end());

#if ROSE_MERGE_DELETE_VERBOSE
     std::clock_t start = std::clock();
     printf ("Enter deleteNodes():\n");
     printf ("  * nodes.size() = %zi\n", nodes.size());
#endif

     AncestorSort sorter(nodes);

#if ROSE_MERGE_DELETE_VERBOSE
     printf ("Before sorting: [%fs]\n", ( std::clock() - start ) / (double) CLOCKS_PER_SEC);
#if ROSE_MERGE_DELETE_DETAILS
     for (std::vector<SgNode *>::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
       printf ("  * %p (%s): %p %p\n", *i, (*i)->class_name().c_str(), (*i)->get_freepointer(), (*i)->get_parent());
     }
     printf("\n");
#endif
#endif

     nodes.clear();
     sorter.sort(nodes);

#if ROSE_MERGE_DELETE_VERBOSE
     printf ("After sorting: [%fs]\n", ( std::clock() - start ) / (double) CLOCKS_PER_SEC);
#if ROSE_MERGE_DELETE_DETAILS
     for (std::vector<SgNode *>::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
       printf ("  * %p (%s): %p %p\n", *i, (*i)->class_name().c_str(), (*i)->get_freepointer(), (*i)->get_parent());
     }
     printf("\n");
#endif
#endif

//   preDeleteTests (listToDelete);

     for (std::vector<SgNode *>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
#if ROSE_MERGE_DELETE_DETAILS
       printf ("> %p: %p\n", *it, (*it)->get_freepointer());
#endif
       if ((*it)->get_freepointer() == AST_FileIO::IS_VALID_POINTER()) {
         delete *it;
       }
     }
#if ROSE_MERGE_DELETE_VERBOSE
     printf ("Leave deleteNodes() [%fs]\n", ( std::clock() - start ) / (double) CLOCKS_PER_SEC);
#endif
   }

void
preDeleteTests ( std::set<SgNode*> & listToDelete )
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     TimingPerformance timer ("Delete the IR nodes that are replaced with shared IR nodes:");

  // displaySet(listToDelete,"Called from preDeleteTests");

     std::set<SgNode*>::iterator i = listToDelete.begin();
     while ( i != listToDelete.end())
        {
#if ROSE_MERGE_DELETE_TRACE
          Sg_File_Info* fileInfo = (*i)->get_file_info();
          printf ("In preDeleteTests(): listToDelete result: i = %p = %s = %s at file = %s \n",
               *i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str(),(fileInfo != NULL) ? fileInfo->get_raw_filename().c_str() : "NULL");
#endif
#if 0
       // DQ (2/8/2007): Make sure that for a class declaration the type is in the delete list if we remove the associated declaration!
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(*i);
          if (classDeclaration != NULL)
             {
               SgType* type = classDeclaration->get_type();

               SgClassType* classType = isSgClassType(type);
               ROSE_ASSERT(classType != NULL);
#if 0
               if ( (listToDelete.find(type) != listToDelete.end()) || (listToDelete.find(classType->get_declaration()) != listToDelete.end()) )
                  {
                    printf ("Error preDeleteTests(): looking for type = %p = %s from classDeclaration = %p = %s = %s \n",
                         type,type->class_name().c_str(),classDeclaration,classDeclaration->class_name().c_str(),SageInterface::get_name(classDeclaration).c_str());
#if 0
                    classDeclaration->get_file_info()->display("Error deleteNodes(): classDeclaration: debug");
#endif
                    printf ("Error preDeleteTests(): classType->get_declaration() = %p = %s \n",classType->get_declaration(),classType->get_declaration()->class_name().c_str());
                  }
#endif
            // ROSE_ASSERT(listToDelete.find(type) != listToDelete.end());
            // ROSE_ASSERT(listToDelete.find(classType->get_declaration()) != listToDelete.end());
#if 0
               if (listToDelete.find(type) != listToDelete.end())
                  {
                    printf ("WARNING Make sure that for a class declaration the type is in the delete list if we remove the associated declaration: type = %p = %s found in listToDelete \n",type,type->class_name().c_str());
                  }
#endif
            // DQ (7/4/2010): Make this a warning since it happens for an odd corner of C++ (un-named PADDING_VARIABLE declarations in classes).
            // ROSE_ASSERT(listToDelete.find(type) == listToDelete.end());
#if 0
               if (listToDelete.find(classType->get_declaration()) != listToDelete.end())
                  {
                    printf ("WARNING Make sure that for a class declaration the type is in the delete list if we remove the associated declaration: classType->get_declaration() = %p = %s found in listToDelete \n",classType->get_declaration(),classType->get_declaration()->class_name().c_str());
                  }
#endif
            // DQ (7/4/2010): Make this a warning since it happens for an odd corner of C++ (un-named PADDING_VARIABLE declarations in classes).
            // ROSE_ASSERT(listToDelete.find(classType->get_declaration()) == listToDelete.end());
            }
#endif

          i++;
        }
   }


void
deleteSetErrorCheck( SgProject* project, const std::set<SgNode*> & listToDelete )
   {
  // DQ (2/15/2007): Error checking on the finalDeleteSet

     if (SgProject::get_verbose() > 0)
          printf ("In deleteSetErrorCheck(): calling computeSetIntersection() \n");

  // set<SgNode*> intersectionSet = computeSetIntersection(project->get_file(0).get_globalScope()->get_symbol_table()->get_symbols(),listToDelete);
  // set<SgNode*> intersectionSet = computeSetIntersection(project->get_fileList()[0]->get_globalScope()->get_symbol_table()->get_symbols(),listToDelete);
     SgSourceFile* sourceFile = isSgSourceFile(project->get_fileList()[0]);
     ROSE_ASSERT(sourceFile != NULL);
     ROSE_ASSERT(sourceFile->get_globalScope() != NULL);
     ROSE_ASSERT(sourceFile->get_globalScope()->get_symbol_table() != NULL);

     set<SgNode*> intersectionSet = computeSetIntersection(sourceFile->get_globalScope()->get_symbol_table()->get_symbols(),listToDelete);

     if (SgProject::get_verbose() > 0)
        {
          printf ("In deleteSetErrorCheck(): calling computeSetIntersection(): DONE \n");
          printf ("In deleteSetErrorCheck(): intersectionSet.size() = %" PRIuPTR " \n",intersectionSet.size());
          displaySet(intersectionSet,"intersectionSet");
        }

     std::set<SgNode*>::iterator i = intersectionSet.begin();
     while ( i != intersectionSet.end())
        {
#if 1
          if (SgProject::get_verbose() > 0)
             {
               Sg_File_Info* fileInfo = (*i)->get_file_info();
               printf ("In deleteSetErrorCheck(): intersectionSet result: i = %p = %s = %s at file = %s \n",
                    *i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str(),(fileInfo != NULL) ? fileInfo->get_raw_filename().c_str() : "NULL");
               switch((*i)->variantT())
                  {
                    case V_SgClassSymbol:
                       {
                         SgClassSymbol* classSymbol = isSgClassSymbol(*i);
                         SgClassDeclaration* classDeclaration =  classSymbol->get_declaration();
                         printf ("asociated classDeclaration                                    = %p = %s \n",classDeclaration,classDeclaration->get_name().str());
                         printf ("asociated classDeclaration->get_definingDeclaration()         = %p \n",classDeclaration->get_definingDeclaration());
                         printf ("asociated classDeclaration->get_firstNondefiningDeclaration() = %p \n",classDeclaration->get_firstNondefiningDeclaration());
                         classDeclaration->get_file_info()->display("problem location");
                       }

                    default:
                       {
                      // ignore these cases
                       }
                  }
             }
#endif
          i++;
        }

  // ROSE_ASSERT(intersectionSet.empty() == true);
   }
