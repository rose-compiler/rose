// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "collectAssociateNodes.h"
#include "test_support.h"
#include "merge_support.h"
using namespace std;

void preDeleteTests ( set<SgNode*> & listToDelete );

void
deleteNodes ( set<SgNode*> & listToDelete )
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     TimingPerformance timer ("Delete the IR nodes that are replaced with shared IR nodes:");

     preDeleteTests (listToDelete);

     set<SgNode*>::iterator i = listToDelete.begin();
     while ( i != listToDelete.end())
        {
#if 0
          Sg_File_Info* fileInfo = (*i)->get_file_info();
          printf ("In deleteNodes(): listToDelete result: i = %p = %s = %s at file = %s \n",
               *i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str(),(fileInfo != NULL) ? fileInfo->get_raw_filename().c_str() : "NULL");
#endif

#if 1
         delete *i;
#endif
          i++;
        }
   }

void
preDeleteTests ( set<SgNode*> & listToDelete )
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     TimingPerformance timer ("Delete the IR nodes that are replaced with shared IR nodes:");

  // displaySet(listToDelete,"Called from preDeleteTests");

     set<SgNode*>::iterator i = listToDelete.begin();
     while ( i != listToDelete.end())
        {
#if 0
          Sg_File_Info* fileInfo = (*i)->get_file_info();
          printf ("In preDeleteTests(): listToDelete result: i = %p = %s = %s at file = %s \n",
               *i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str(),(fileInfo != NULL) ? fileInfo->get_raw_filename().c_str() : "NULL");
#endif

       // DQ (2/8/2007): Make sure that for a class declaration the type is in the delete list if we remove the associated declaration!
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(*i);
          if (classDeclaration != NULL)
             {
               SgType* type = classDeclaration->get_type();

               SgClassType* classType = isSgClassType(type);
               ROSE_ASSERT(classType != NULL);
#if 1
               if ( (listToDelete.find(type) != listToDelete.end()) || (listToDelete.find(classType->get_declaration()) != listToDelete.end()) )
                  {
                    printf ("Error preDeleteTests(): looking for type = %p = %s from classDeclaration = %p = %s = %s \n",
                         type,type->class_name().c_str(),classDeclaration,classDeclaration->class_name().c_str(),SageInterface::get_name(classDeclaration).c_str());
                    classDeclaration->get_file_info()->display("Error deleteNodes(): classDeclaration: debug");
                    printf ("Error preDeleteTests(): classType->get_declaration() = %p = %s \n",classType->get_declaration(),classType->get_declaration()->class_name().c_str());
                  }
#endif
            // ROSE_ASSERT(listToDelete.find(type) != listToDelete.end());
            // ROSE_ASSERT(listToDelete.find(classType->get_declaration()) != listToDelete.end());

               if (listToDelete.find(type) != listToDelete.end())
                  {
                    printf ("WARNING Make sure that for a class declaration the type is in the delete list if we remove the associated declaration: type = %p = %s found in listToDelete \n",type,type->class_name().c_str());
                  }
            // DQ (7/4/2010): Make this a warning since it happens for an odd corner of C++ (un-named PADDING_VARIABLE declarations in classes).
            // ROSE_ASSERT(listToDelete.find(type) == listToDelete.end());

               if (listToDelete.find(classType->get_declaration()) != listToDelete.end())
                  {
                    printf ("WARNING Make sure that for a class declaration the type is in the delete list if we remove the associated declaration: classType->get_declaration() = %p = %s found in listToDelete \n",classType->get_declaration(),classType->get_declaration()->class_name().c_str());
                  }
            // DQ (7/4/2010): Make this a warning since it happens for an odd corner of C++ (un-named PADDING_VARIABLE declarations in classes).
            // ROSE_ASSERT(listToDelete.find(classType->get_declaration()) == listToDelete.end());
            }

          i++;
        }
   }


void
deleteSetErrorCheck( SgProject* project, const set<SgNode*> & listToDelete )
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
          printf ("In deleteSetErrorCheck(): intersectionSet.size() = %zu \n",intersectionSet.size());
          displaySet(intersectionSet,"intersectionSet");
        }

     set<SgNode*>::iterator i = intersectionSet.begin();
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
