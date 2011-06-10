// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "fixupDeclarations.h"
void
fixupDeclarations( SgNode* node )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Testing declarations (no side-effects to AST):");

#if 1
  // DQ (10/20/2007): This clears all the pointers to SgClassDefinition object from non-defining SgClassDeclaration objects.
  // This fixes a bug in the copy mechanism so that deep copies are not made of the SgClassDefinition objects (always a mistake).
  // This approach uses the memory pools.
     class ResetDefinitionsInNonDefiningClassDeclarationsOnMemoryPool : public ROSE_VisitTraversal
        {
          public:
            // Required traversal function
               void visit (SgNode* node)
                  {
                 // For now we just do this for the SgClassDeclaration
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(node);
                 // if ( (classDeclaration != NULL) && (classDeclaration->get_definingDeclaration() != NULL) && (classDeclaration != classDeclaration->get_definingDeclaration()) )
                    if (classDeclaration != NULL)
                       {
                      // Note that classDeclaration->get_definingDeclaration() can be NULL, this is OK.
                         if ( (classDeclaration != classDeclaration->get_definingDeclaration()) )
                            {
                           // This is a non-defining class declaration
                              if (classDeclaration->get_definition() != NULL)
                                 {
                                   classDeclaration->set_definition(NULL);
                                 }
                            }
                       }
                  }

            // This avoids a warning by g++
               virtual ~ResetDefinitionsInNonDefiningClassDeclarationsOnMemoryPool() {};         
        };

     // PC (10/26/2009): This deletes dangling class definitions associated with classes that are declared but not defined.
     // Their existence interferes with the AST merge mechanism.
     class DeleteUnreferencedClassDefinitionsInMemoryPool : public ROSE_VisitTraversal
        {
          public:
            // Required traversal function
               void visit (SgNode* node)
                  {
                    SgClassDefinition* classDefinition = isSgClassDefinition(node);
                    if (classDefinition != NULL)
                       {
                         SgClassDeclaration* classDeclaration = isSgClassDeclaration(classDefinition->get_parent());
                         if (classDeclaration != NULL)
                            {
                              if (classDeclaration->get_definition() == NULL)
                                 {
                                   if (classDefinition->get_members().size() > 0)
                                      {
                                        std::cerr << "Warning: about to delete a dangling class definition with " << classDefinition->get_members().size() << " members!" << std::endl;
                                      }
                                   delete classDefinition;
                                 }
                            }
                       }
                  }

               virtual ~DeleteUnreferencedClassDefinitionsInMemoryPool() {};         
        };

#if 1
  // This will traverse the whole memory pool
     ResetDefinitionsInNonDefiningClassDeclarationsOnMemoryPool traversal;
     traversal.traverseMemoryPool();

     DeleteUnreferencedClassDefinitionsInMemoryPool deleteTraversal;
     SgClassDefinition::traverseMemoryPoolNodes(deleteTraversal);
#else
  // DQ (10/20/2007): We can alternatively just traverse the types of IR nodes that are relavant, 
  // instead of the whole memory pool.  This is a cool feature of the memory pool traversal in this
  // case this make the traversal about 30% faster, since we visit such a small subset of IR nodes.
     ResetTemplateNamesOnMemoryPool traversal;

     SgClassDeclaration::traverseMemoryPoolNodes(traversal);
     SgFunctionDeclaration::traverseMemoryPoolNodes(traversal);
     SgMemberFunctionDeclaration::traverseMemoryPoolNodes(traversal);
     SgTemplateInstantiationDecl::traverseMemoryPoolNodes(traversal);
     SgTemplateInstantiationFunctionDecl::traverseMemoryPoolNodes(traversal);
     SgTemplateInstantiationMemberFunctionDecl::traverseMemoryPoolNodes(traversal);
#endif
#endif

  // This simplifies how the traversal is called!
     FixupDeclarations declarationFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     declarationFixupTraversal.traverse(node,preorder);

   }

void
FixupDeclarations::visit (SgNode* node)
   {
     ROSE_ASSERT(node != NULL);

     SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
     if (declaration != NULL)
        {
#if 0
          printf ("Found a SgDeclarationStatement = %s \n",declaration->sage_class_name());
          switch(declaration->variantT())
             {
               case V_SgFunctionDeclaration:
                    printf ("Found a function declaration name = %s \n",isSgFunctionDeclaration(declaration)->get_name().str());
                    declaration->get_file_info()->display("declaration found in FixupDeclarations::visit()");
                    break;
               case V_SgMemberFunctionDeclaration:
                    printf ("Found a function declaration name = %s \n",isSgFunctionDeclaration(declaration)->get_name().str());
                    break;

               default:
                  {
                    printf ("Other sort for declaration found in FixupDeclarations::visit() ... \n");
                    break;
                  }
             }
#endif

#if 0
       // DQ (10/22/2007): This test is repeated at the end of this function, use that version instead.

       // DQ (10/16/2007): Make sure that the defining and nondefining declarations are the same sorts of IR nodes!
          if (declaration != NULL && declaration->get_definingDeclaration() != NULL && declaration->get_firstNondefiningDeclaration() != NULL)
             {
               if (declaration->get_definingDeclaration()->variantT() != declaration->get_firstNondefiningDeclaration()->variantT())
                  {
                    printf ("variants are different: declaration                 = %p = %s \n",declaration,declaration->class_name().c_str());
                    printf ("     declaration->get_definingDeclaration()         = %p = %s \n",declaration->get_definingDeclaration(),declaration->get_definingDeclaration()->class_name().c_str());
                    printf ("     declaration->get_firstNondefiningDeclaration() = %p = %s \n",declaration->get_firstNondefiningDeclaration(),declaration->get_firstNondefiningDeclaration()->class_name().c_str());
                  }

            // ROSE_ASSERT(declaration->get_definingDeclaration()->variantT() == declaration->get_firstNondefiningDeclaration()->variantT());
               if (isSgFunctionDeclaration(declaration->get_firstNondefiningDeclaration()) != NULL && isSgTemplateInstantiationFunctionDecl(declaration->get_definingDeclaration()) != NULL)
                  {
                 // printf ("This case is close enough for now! \n");
                    if (declaration->get_definingDeclaration()->variantT() != declaration->get_firstNondefiningDeclaration()->variantT())
                       {
                         printf ("Different types of IR nodes used for defining vs. firstNondefining declarations \n");
                       }
                  }
                 else
                  {
                    ROSE_ASSERT(declaration->get_definingDeclaration()->variantT() == declaration->get_firstNondefiningDeclaration()->variantT());
                  }
             }
#endif

          SgDeclarationStatement* firstDeclaration = NULL;
#if 0
          printf ("(before possible reset) declaration = %p declaration->get_firstNondefiningDeclaration() = %p \n",
               declaration,declaration->get_firstNondefiningDeclaration());
#endif

#if 0
       // DQ (3/15/2006): The SgFunctionParameterList is a special case since we should consider setting 
       // the declaration that contains any defalut arguments to be the defining declaration and that could
       // be different than the defining declaration of the function associated with the SgFunctionParameterList.
          if (isSgFunctionParameterList(declaration) != NULL)
             {
               printf ("(before posible reset of SgFunctionParameterList) declaration = %p declaration->get_firstNondefiningDeclaration() = %p declaration->get_definingDeclaration() = %p \n",
                    declaration,declaration->get_firstNondefiningDeclaration(),declaration->get_definingDeclaration());
             }
#endif

          if (declaration->get_firstNondefiningDeclaration() == NULL)
             {
            // If firstNondefiningDeclaration is not defined then make it a self reference
            // printf ("In fixupDeclarations set_firstNondefiningDeclaration of %p = %s to %p \n",declaration,declaration->class_name().c_str(),declaration);
#if 1
               if (declaration->get_definingDeclaration() == declaration) {
                   std::cerr << "warning: setting a defining declaration as the firstNondefiningDeclaration" << std::endl;
               }
#endif
               declaration->set_firstNondefiningDeclaration(declaration);
               firstDeclaration = declaration;
             }
            else
             {
            // printf ("Setting the firstDeclaration from declaration = %p using declaration->get_firstNondefiningDeclaration() = %p \n",declaration,declaration->get_firstNondefiningDeclaration());
               firstDeclaration = declaration->get_firstNondefiningDeclaration();
             }

          SgDeclarationStatement* definingDeclaration = NULL;
          if (declaration->get_definingDeclaration() == NULL)
             {
            // This is the case for any forward declaration appearing after the first non-defining 
            // declaration and before the defining declaration
               definingDeclaration = firstDeclaration->get_definingDeclaration();
               if (definingDeclaration != NULL)
                  {
                 // printf ("In fixupDeclarations set_definingDeclaration of %p = %s to %p \n",declaration,declaration->class_name().c_str(),definingDeclaration);
                    declaration->set_definingDeclaration(definingDeclaration);
                  }
             }
            else
             {
               definingDeclaration = declaration->get_definingDeclaration();
             }

       // DQ (2/6/2007): This is not true, since a forward declaration may appear after the 
       // defining declaration for a class or template declaration.
       // The first nondefining declaration should exist and will be the same as the defining 
       // declaration if only the defining declaration exists. Is this correct behavior???
          ROSE_ASSERT(firstDeclaration != NULL);
          if (firstDeclaration->get_firstNondefiningDeclaration() != firstDeclaration)
             {
               printf ("firstDeclaration = %p = %s = %s \n",firstDeclaration,firstDeclaration->class_name().c_str(),SageInterface::get_name(firstDeclaration).c_str());
               printf ("firstDeclaration->get_firstNondefiningDeclaration() = %p \n",firstDeclaration->get_firstNondefiningDeclaration());
               if (firstDeclaration->get_firstNondefiningDeclaration() != NULL)
                    printf ("firstDeclaration->get_firstNondefiningDeclaration() = %s \n",firstDeclaration->get_firstNondefiningDeclaration()->class_name().c_str());
             }
          ROSE_ASSERT(firstDeclaration->get_firstNondefiningDeclaration() == firstDeclaration);

       // No first non-defining declaration exists if the first appearence includes a definition (class or function)
          if (firstDeclaration != NULL)
             {
               ROSE_ASSERT(declaration->get_firstNondefiningDeclaration() == firstDeclaration);
               ROSE_ASSERT(firstDeclaration->get_firstNondefiningDeclaration() == firstDeclaration);
             }

       // Defining declaration not valid if no definition (class or function) is present
          if (definingDeclaration != NULL)
             {
               ROSE_ASSERT(declaration->get_firstNondefiningDeclaration() == firstDeclaration);
#if 0
               if (definingDeclaration->get_firstNondefiningDeclaration() != firstDeclaration)
                  {
                    printf ("declaration->get_firstNondefiningDeclaration()         = %p \n",declaration->get_firstNondefiningDeclaration());
                    printf ("definingDeclaration                                    = %p \n",definingDeclaration);
                    printf ("firstDeclaration                                       = %p \n",firstDeclaration);
                    printf ("definingDeclaration->get_firstNondefiningDeclaration() = %p \n",definingDeclaration->get_firstNondefiningDeclaration());
                  }
               ROSE_ASSERT(definingDeclaration->get_firstNondefiningDeclaration() == firstDeclaration);
#endif
#if 1
               ROSE_ASSERT(definingDeclaration != NULL);
               if (definingDeclaration->get_definingDeclaration() != definingDeclaration)
                  {
                    printf ("Error: definingDeclaration->get_definingDeclaration() != definingDeclaration \n");
                    printf ("declaration->get_definingDeclaration()                 = %p \n",declaration->get_definingDeclaration());
                    printf ("definingDeclaration                                    = %p \n",definingDeclaration);
                    printf ("firstDeclaration                                       = %p \n",firstDeclaration);
                 // printf ("definingDeclaration->get_firstNondefiningDeclaration() = %p \n",definingDeclaration->get_firstNondefiningDeclaration());
                    definingDeclaration->get_file_info()->display("definingDeclaration");
                    ROSE_ASSERT(definingDeclaration->get_definingDeclaration() != NULL);
                    ROSE_ASSERT(definingDeclaration->get_definingDeclaration()->get_file_info() != NULL);
                    definingDeclaration->get_definingDeclaration()->get_file_info()->display("definingDeclaration");
                  }
#endif
               ROSE_ASSERT(definingDeclaration->get_definingDeclaration() == definingDeclaration);
             }

       // error checking possible if both are defined
          if ( (firstDeclaration != NULL) && (definingDeclaration != NULL) )
             {
               if (firstDeclaration->get_definingDeclaration() != definingDeclaration)
                  {
                    SgSourceFile* firstDeclarationSourceFile    = TransformationSupport::getSourceFile(firstDeclaration);
                    SgSourceFile* definingDeclarationSourceFile = TransformationSupport::getSourceFile(definingDeclaration);
                    printf ("Error: firstDeclaration            = %p = %s \n",firstDeclaration,firstDeclaration->class_name().c_str());
                    printf ("Error: firstDeclaration in file    = %p = %s \n",firstDeclarationSourceFile,firstDeclarationSourceFile->getFileName().c_str());
                    printf ("Error: definingDeclaration         = %p = %s \n",definingDeclaration,definingDeclaration->class_name().c_str());
                    printf ("Error: definingDeclaration in file = %p = %s \n",definingDeclarationSourceFile,definingDeclarationSourceFile->getFileName().c_str());

                 // DQ (2/26/2009): For moreTest3.cpp outlining test, these are in different file and to be file consistant 
                    if (firstDeclarationSourceFile == definingDeclarationSourceFile)
                       {
                         ROSE_ASSERT(firstDeclaration->get_definingDeclaration() != NULL);
                         printf ("Error: firstDeclaration->get_definingDeclaration() = %p = %s \n",firstDeclaration->get_definingDeclaration(),firstDeclaration->get_definingDeclaration()->class_name().c_str());
                         printf ("Error: firstDeclaration->get_definingDeclaration() != definingDeclaration \n");
                         firstDeclaration->get_file_info()->display("location of firstDeclaration");
                         definingDeclaration->get_file_info()->display("location of definingDeclaration");
                         firstDeclaration->get_definingDeclaration()->get_file_info()->display("location of firstDeclaration->get_definingDeclaration()");
                       }
                      else
                       {
                      // DQ (3/4/2009): Only for outlining to a separate file is this true, in the copyAST_tests/copytest2007_23.C this fails (as it should). So disable the assertion.
                      // DQ (2/26/2009): If the defining declaration and the "firstDeclaration" are in a different files then they can't reference each other.
                      // ROSE_ASSERT(firstDeclaration->get_definingDeclaration() == NULL);
                         if (firstDeclaration->get_definingDeclaration() != NULL)
                            {
                           // This message will be output in the processing of copyAST_tests/copytest2007_23.C.
                              printf ("Warning: Note that firstDeclarationSourceFile != definingDeclarationSourceFile and firstDeclaration->get_definingDeclaration() != NULL \n");
                            }
                       }
                  }

            // DQ (2/21/2009): Comment out while we test the outlining.
               if (firstDeclaration->get_definingDeclaration() != definingDeclaration)
                  {
                    printf ("Warning: firstDeclaration->get_definingDeclaration() != definingDeclaration \n");
                    printf ("#############  Commented out assertion (temporary)  ################## \n");
                  }
            // ROSE_ASSERT(firstDeclaration->get_definingDeclaration() == definingDeclaration);
             }

       // DQ (10/16/2007): Make sure that the defining and nondefining declarations are the same sorts of IR nodes!
          if (declaration != NULL && declaration->get_definingDeclaration() != NULL && declaration->get_firstNondefiningDeclaration() != NULL)
             {
#if 0
            // DQ (10/22/2007): See if we can turn this back on now! Not Yet, test2007_90.C fails.
               if (declaration->get_definingDeclaration()->variantT() != declaration->get_firstNondefiningDeclaration()->variantT())
                  {
                    printf ("declaration                                         = %p = %s \n",declaration,declaration->class_name().c_str());
                    printf ("     declaration->get_definingDeclaration()         = %p = %s \n",declaration->get_definingDeclaration(),declaration->get_definingDeclaration()->class_name().c_str());
                    printf ("     declaration->get_firstNondefiningDeclaration() = %p = %s \n",declaration->get_firstNondefiningDeclaration(),declaration->get_firstNondefiningDeclaration()->class_name().c_str());
                  }

            // DQ (10/22/2007): See if we can turn this back on now! Not Yet, test2007_90.C fails.
               ROSE_ASSERT(declaration->get_definingDeclaration()->variantT() == declaration->get_firstNondefiningDeclaration()->variantT());
#else
            // DQ (10/22/2007): Use this more relaxed test for now!
               if ( (isSgFunctionDeclaration(declaration->get_firstNondefiningDeclaration()) != NULL && isSgTemplateInstantiationFunctionDecl(declaration->get_definingDeclaration()) != NULL) ||
                    (isSgFunctionDeclaration(declaration->get_definingDeclaration()) != NULL && isSgTemplateInstantiationFunctionDecl(declaration->get_firstNondefiningDeclaration()) != NULL) )
                  {
#if PRINT_DEVELOPER_WARNINGS
                    printf ("Warning inconsistent use of SgTemplateInstantiationFunctionDecl: found a declaration = %p = %s which has a firstNondefiningDeclaration which is a SgFunctionDeclaration and which has a defining declaration which is a SgTemplateInstantiationFunctionDecl \n",
                         declaration,declaration->class_name().c_str());
                 // declaration->get_startOfConstruct()->display("Warning inconsitant use of SgTemplateInstantiationFunctionDecl: found a declaration has a firstNondefiningDeclaration which is a SgFunctionDeclaration and a defining declaration which is a SgTemplateInstantiationFunctionDecl");
#endif
                  }
                 else
                  { // defining and nondefining pointers point to something of different types
                    if (declaration->get_definingDeclaration()->variantT() != declaration->get_firstNondefiningDeclaration()->variantT())
                       {
                         printf ("Error: declaration                                  = %p = %s \n",declaration,declaration->class_name().c_str());
                         printf ("     declaration->get_definingDeclaration()         = %p = %s \n",declaration->get_definingDeclaration(),declaration->get_definingDeclaration()->class_name().c_str());
                         printf ("     declaration->get_firstNondefiningDeclaration() = %p = %s \n",declaration->get_firstNondefiningDeclaration(),declaration->get_firstNondefiningDeclaration()->class_name().c_str());
                       }
                    ROSE_ASSERT(declaration->get_definingDeclaration()->variantT() == declaration->get_firstNondefiningDeclaration()->variantT());
                  }
#endif
             }
        }
   }

