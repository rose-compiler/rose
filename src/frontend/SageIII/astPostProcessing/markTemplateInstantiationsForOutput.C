// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "rose_config.h"

#include "markForOutputInCodeGeneration.h"
#include "markTemplateInstantiationsForOutput.h"

#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
#include "lowLevelRewriteInterface.h"
#endif

#ifdef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
   #include "transformationSupport.h"
#endif

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

set<SgDeclarationStatement*>
MarkTemplateInstantiationsForOutput::BuildSetOfRequiredTemplateDeclarations ( SgNode* node, SgSourceFile* file )
   {
     ROSE_ASSERT(file != NULL);

  // This simplifies how the traversal is called!
     MarkTemplateInstantiationsForOutputSupport declarationFixupTraversal(file);

  // This inherited attribute is used for all traversals (within the iterative approach we define)
     MarkTemplateInstantiationsForOutputSupportInheritedAttribute inheritedAttribute;

  // An alternative is to save the list of required declaration and then zero out the list 
  // in the declarationFixupTraversal traversal and then re-execute the traversal on each 
  // declaration that is compiler generated (i.e. not from the current file).  We define 
  // an iterative process where we always search the new declarations on the assembled 
  // list and merge the lists until finally no list is generated (then we stop).

  // List of lists of declarations, each list is obtained from an iteration of the 
  // prelink process, multiple iterations are required because each new declaration 
  // found to be required may cascade into other declarations being required.  All
  // of these could generate required instantiations and trigger the output of 
  // template instantiations.
     vector<list<SgDeclarationStatement*> > listOfListsOfDeclarations;

  // First call to traverse is a traversal of the whole AST
     declarationFixupTraversal.traverse(node,inheritedAttribute);

#if 0
     printf ("Declarations collected form first phase of processing (size = %" PRIuPTR "): \n",declarationFixupTraversal.listOfTemplateDeclarationsToOutput.size());
     list<SgDeclarationStatement*>::iterator i = declarationFixupTraversal.listOfTemplateDeclarationsToOutput.begin();
     while ( i != declarationFixupTraversal.listOfTemplateDeclarationsToOutput.end())
        {
          printf ("  -- declaration = %p = %s \n",*i,(*i)->class_name().c_str());
          i++;
        }
#endif

#if 0
     printf ("Through with first call to traverse AST! \n");
     printf ("declarationFixupTraversal.listOfTemplateDeclarationsToOutput.size() = %" PRIuPTR " \n",declarationFixupTraversal.listOfTemplateDeclarationsToOutput.size());
#endif

     list<SgDeclarationStatement*> accumulatedList = declarationFixupTraversal.listOfTemplateDeclarationsToOutput;

  // create a shorter name for the list where we accumulare required declarations
     list<SgDeclarationStatement*> & currentList = declarationFixupTraversal.listOfTemplateDeclarationsToOutput;

     int prelinkIterationCounter = 0;
  // while ( declarationFixupTraversal.listOfTemplateDeclarationsToOutput.empty() == false )
     while ( currentList.empty() == false )
        {
       // For each identified template instantiation, traverse it to identify if it includes an instantiation that would ???
#if 0
       // Add any new elements from currentList into the accumulatedList (so that we can use the accumulatedList to remove elements from the currentList at the end of the loop).
          list<SgDeclarationStatement*>::iterator l = currentList.begin();
          while (l != currentList.end())
             {

#error "DEAD CODE!"

            // Check is this is a previously seen declaration.
               if (find(accumulatedList.begin(),accumulatedList.end(),*l) == accumulatedList.end())
                  {
                    accumulatedList.push_back(*l);
                  }
               l++;
             }
#endif
#if 0
          printf ("   --- AFTER accumulation: accumulatedList.size() = %" PRIuPTR " \n",accumulatedList.size());
          list<SgDeclarationStatement*>::iterator a = accumulatedList.begin();
          unsigned int b = 0;
          while (a != accumulatedList.end())
             {
               printf ("   ---   ---  accumulatedList[%u] = %p \n",b,*a);
               a++;
               b++;
             }
#endif

       // DQ (3/31/2013): This might not be required if we use the accumulatedList to build the setOfRequiredDeclarations (function return value).
       // append the newest list to the back of the list of lists of required declarations
          listOfListsOfDeclarations.push_back(currentList);
#if 0
          printf ("   --- Number of lists of required declarations = %" PRIuPTR " \n",listOfListsOfDeclarations.size());
#endif
#if 0
          for (unsigned int n = 0; n < listOfListsOfDeclarations.size(); n++)
             {
            // printf (" -- Size of listOfListsOfDeclarations[%u] = %" PRIuPTR " \n",n,listOfListsOfDeclarations[n].size());
               printf ("   ---   --- Size of listOfListsOfDeclarations[%u] = %" PRIuPTR " \n",n,listOfListsOfDeclarations[n].size());
             }
#endif
#if 0
          printf ("   --- currentList.size() = %" PRIuPTR " \n",currentList.size());
          list<SgDeclarationStatement*>::iterator j = currentList.begin();
          unsigned int m = 0;
          while (j != currentList.end())
             {
               printf ("   ---   ---  currentList[%u] = %p \n",m,*j);
               j++;
               m++;
             }
#endif

#if 0
       // DQ (3/31/2013): We need to erase any previously seen element, not just those from last iteration.
       // remove the elements from the current list
       // currentList.erase(currentList.begin(),currentList.end());

#error "DEAD CODE!"

       // currentList.erase(accumulatedList.begin(),accumulatedList.end());
#if 0
          printf ("REMOVING any previously seen elements from the list (1st time): \n");
#endif
          list<SgDeclarationStatement*>::iterator c = accumulatedList.begin();
          unsigned int d = 0;
          while (c != accumulatedList.end())
             {
               if (find(currentList.begin(),currentList.end(),*c) != currentList.end())
                  {
                    printf ("   ---   ---  erasing (remove) accumulatedList[%u] = %p from currentList \n",d,*c);
                    currentList.remove(*c);
                  }

#error "DEAD CODE!"

               c++;
               d++;
             }
#if 0
          printf ("   --- AFTER ERASE: currentList.size() = %" PRIuPTR " \n",currentList.size());
          list<SgDeclarationStatement*>::iterator k = currentList.begin();
          unsigned int o = 0;
          while (k != currentList.end())
             {
               printf ("   ---   ---  currentList[%u] = %p \n",o,*k);
               k++;
               o++;
             }

          printf ("   --- declarationFixupTraversal.listOfTemplateDeclarationsToOutput.size() = %" PRIuPTR " \n",declarationFixupTraversal.listOfTemplateDeclarationsToOutput.size());
#endif

#error "DEAD CODE!"

#if 0
          printf ("   --- After erase of currentList: Number of lists of required declarations = %" PRIuPTR " \n",listOfListsOfDeclarations.size());
          for (unsigned int n = 0; n < listOfListsOfDeclarations.size(); n++)
             {
               printf ("   ---   --- Size of listOfListsOfDeclarations[%u] = %" PRIuPTR " \n",n,listOfListsOfDeclarations[n].size());

               list<SgDeclarationStatement*>::iterator i = listOfListsOfDeclarations[n].begin();

               unsigned int m = 0;
               while (i != listOfListsOfDeclarations[n].end())
                  {
                    printf ("   ---   --- listOfListsOfDeclarations[%u][%u] = %p \n",n,m,*i);
                    i++;
                    m++;
                  }
             }
#endif
#endif
          list<SgDeclarationStatement*>::iterator i = listOfListsOfDeclarations[prelinkIterationCounter].begin();
          while ( i != listOfListsOfDeclarations[prelinkIterationCounter].end() )
             {
            // Iterate thorugh the list a look for compiler generated declarations

               ROSE_ASSERT((*i)->get_file_info() != NULL);
               if ( (*i)->get_file_info()->isCompilerGenerated() == true )
                  {
                 // Look into this subtrees for any required declarations (which would not have been caught last iteration!
#if 0
                    printf ("   --- START: Nested traversal: declarationFixupTraversal.listOfTemplateDeclarationsToOutput.size() = %" PRIuPTR " \n",
                         declarationFixupTraversal.listOfTemplateDeclarationsToOutput.size());
#endif
                    declarationFixupTraversal.traverse ( *i , inheritedAttribute );
#if 0
                    printf ("   --- END: Nested traversal: declarationFixupTraversal.listOfTemplateDeclarationsToOutput.size() = %" PRIuPTR " \n",
                         declarationFixupTraversal.listOfTemplateDeclarationsToOutput.size());
#endif
                  }

               i++;
             }

       // DQ (3/31/2013): Added debugging code.
          if (listOfListsOfDeclarations.size() > 2)
             {
               printf ("Exiting as a test: listOfListsOfDeclarations.size() > 2 prelinkIterationCounter = %u \n",prelinkIterationCounter);
               ROSE_ASSERT(false);
             }

       // DQ (3/31/2013): This is some sort of set operation (union into accumulatedList) and removing the insersection 
       // from the currentList.  Note sure if there is a simpler expression for this.

       // DQ (3/31/2013): Remove any previously handled declarations (list elements).
#if 0
          printf ("SCRUB THE LIST: REMOVING any previously seen elements from the list (2nd time): \n");
#endif
          list<SgDeclarationStatement*>::iterator e = accumulatedList.begin();
          unsigned int f = 0;
          while (e != accumulatedList.end())
             {
               if (find(currentList.begin(),currentList.end(),*e) != currentList.end())
                  {
#if 0
                    printf ("   ---   ---  erasing (remove) accumulatedList[%u] = %p from currentList \n",f,*e);
#endif
                    currentList.remove(*e);
                  }

               e++;
               f++;
             }

       // Add any new elements from currentList into the accumulatedList (so that we can use the accumulatedList to remove elements from the currentList at the end of the loop).
          list<SgDeclarationStatement*>::iterator l = currentList.begin();
          unsigned int g = 0;
          while (l != currentList.end())
             {
            // Check is this is a previously seen declaration.
               if (find(accumulatedList.begin(),accumulatedList.end(),*l) == accumulatedList.end())
                  {
#if 0
                    printf ("   ---   ---  adding new ellements from currentList[%u] = %p to accumulatedList \n",g,*l);
#endif
                    accumulatedList.push_back(*l);
                  }
               l++;
               g++;
             }

#if 0
          printf ("   --- @@@@@ At end of prelink loop prelinkIterationCounter = %u currentList.size() = %lu \n",prelinkIterationCounter, (unsigned long) currentList.size());
#endif
#if 0
          printf ("Exiting after first prelink iteration \n");
          ROSE_ASSERT(false);
#endif
          prelinkIterationCounter++;

        }

#if 0
     printf ("Number of lists of required declarations = %" PRIuPTR " \n",listOfListsOfDeclarations.size());
#endif

  // Convert the vector of lists to a set!
     set<SgDeclarationStatement*> setOfRequiredDeclarations;
#if 0
     for (unsigned int n = 0; n < listOfListsOfDeclarations.size(); n++)
        {
#if 0
          printf (" -- Size of listOfListsOfDeclarations[%d] = %" PRIuPTR " \n",n,listOfListsOfDeclarations[n].size());
#endif
          list<SgDeclarationStatement*>::iterator i = listOfListsOfDeclarations[n].begin();
          while ( i != listOfListsOfDeclarations[n].end() )
             {
               setOfRequiredDeclarations.insert(*i);
               i++;
             }
        }
#else
  // DQ (3/31/2013): Use this simpler implementation.
     list<SgDeclarationStatement*>::iterator c = accumulatedList.begin();
     while (c != accumulatedList.end())
        {
          setOfRequiredDeclarations.insert(*c);
          c++;
        }
#endif

     return setOfRequiredDeclarations;
   }


void
MarkTemplateInstantiationsForOutput::
ProcessMemberFunctionTemplateDeclarations ( set<SgDeclarationStatement*> setOfRequiredDeclarations, SgSourceFile* input_file )
   {
     ROSE_ASSERT(input_file != NULL);

     for ( set<SgDeclarationStatement*>::iterator i = setOfRequiredDeclarations.begin(); i != setOfRequiredDeclarations.end(); i++ )
        {
       // Iterate through the functions recorded as required for compilation of this file!

       // ROSE_ASSERT((*i)->get_definingDeclaration() != NULL);
       // ROSE_ASSERT((*i)->get_firstNondefiningDeclaration() != NULL);

          SgDeclarationStatement* definingDeclaration         = (*i)->get_definingDeclaration();
          SgDeclarationStatement* firstNondefiningDeclaration = (*i)->get_firstNondefiningDeclaration();

          bool isDefiningDeclaration         = definingDeclaration         == NULL ? false : (*i == definingDeclaration);
          bool isfirstNondefiningDeclaration = firstNondefiningDeclaration == NULL ? false : (*i == firstNondefiningDeclaration);
       // first debug the member functions!
#if 0
          printf ("In ProcessMemberFunctionTemplateDeclarations() set element = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
       // We only have to worry about member functions (since that is what this function is handling).
          SgTemplateInstantiationMemberFunctionDecl* memberFunctionInstantiation = isSgTemplateInstantiationMemberFunctionDecl(*i);
          if (memberFunctionInstantiation != NULL)
             {
#if 0
               printf ("\n\n");
               printf ("*************************************** \n");
               printf ("      Required declaration = %p = %s = %s (is defining decl = %s) (is first non-defining decl = %s) \n",
                                   *i,(*i)->sage_class_name(),SageInterface::get_name(*i).c_str(),
                                   isDefiningDeclaration ? "true" : "false", 
                                   isfirstNondefiningDeclaration ? "true" : "false");
               printf ("(*i)->get_file_info() = %p \n",(*i)->get_file_info());
               (*i)->get_file_info()->display("required declaration: debug");
#endif
            // At least one of these should be true!
               ROSE_ASSERT ( isfirstNondefiningDeclaration == true || isDefiningDeclaration == true );

            // If the template declaration is in the current file then we need not output the instantiation (skip marking instantiation for output!)
               SgDeclarationStatement* templateDeclaration = memberFunctionInstantiation->get_templateDeclaration();

            // DQ (1/21/2013): This is a problem for specialized templates containing declarations (e.g. member functions) 
            // that are not in the associated template class.  These member function instantiations, don't always have an 
            // associated member function template declaration.  So we can't inforce this.
            // ROSE_ASSERT(templateDeclaration != NULL);
               if (templateDeclaration != NULL)
                  {
                 // BEGIN: Indentation issue in this source code.

#if 0
            // printf ("templateDeclaration = %p = %s \n",templateDeclaration,templateDeclaration->get_name().str());
               printf ("templateDeclaration = %p = %s \n",templateDeclaration,templateDeclaration->class_name().c_str());
               printf ("templateDeclaration->get_firstNondefiningDeclaration() = %p \n",templateDeclaration->get_firstNondefiningDeclaration());
               printf ("templateDeclaration->get_definingDeclaration()         = %p \n",templateDeclaration->get_definingDeclaration());
#endif
            // string currentFilename = TransformationSupport::getFile(templateDeclaration)->getFileName();
               SgSourceFile* file = TransformationSupport::getSourceFile(memberFunctionInstantiation);
               ROSE_ASSERT(file != NULL);
               string currentFilename = (file != NULL) ? file->getFileName() : "";
               string filenameOfTemplateDeclaration = templateDeclaration->get_file_info()->get_filename();

               ROSE_ASSERT(templateDeclaration->get_scope() != NULL);
               SgTemplateInstantiationDefn* memberFunctionScopeTemplateInstantiationDefinition = isSgTemplateInstantiationDefn(templateDeclaration->get_scope());
               if (memberFunctionScopeTemplateInstantiationDefinition != NULL)
                  {
                 // This is a SgTemplateInstantiationMemberFunctionDecl in a templated class declaration 
                 // (so it might be a templated member function or a non-templated member function).
                    SgTemplateInstantiationDecl* memberFunctionScopeTemplateInstantiationDeclaration = isSgTemplateInstantiationDecl(memberFunctionScopeTemplateInstantiationDefinition->get_declaration());
                    ROSE_ASSERT(memberFunctionScopeTemplateInstantiationDeclaration != NULL);
                    SgDeclarationStatement* memberFunctionScopeTemplateDeclaration = memberFunctionScopeTemplateInstantiationDeclaration->get_templateDeclaration();
                    ROSE_ASSERT(memberFunctionScopeTemplateDeclaration != NULL);
#if 0
                    printf ("Warning the member function might be specified in a templated class which is nested in another templated class! \n");
#endif
                    filenameOfTemplateDeclaration = memberFunctionScopeTemplateDeclaration->get_file_info()->get_filename();
                  }

#if 0
               templateDeclaration->get_file_info()->display("templateDeclaration file name problem: debug");
               printf ("currentFilename               = %s \n",currentFilename.c_str());
               printf ("filenameOfTemplateDeclaration = %s \n",filenameOfTemplateDeclaration.c_str());
#endif

            // We only want to fixup template details in the current file, since we only unparse the current file.
               if (filenameOfTemplateDeclaration == currentFilename)
                  {
                 // This template declaration is in the current file so let the vendor compiler instantiate it, 
                 // there are a few rules here:
                 //    1) if it is a specialization then we should output it (since it is used), or
                 //    2) if it is defined in the class and the class is a templated class then 
                 //       EDG will not list the source for the member function in the class template
                 //       declaration (independent of the setting og TEMPLATES_IN_IL within EDG.
                    bool isSpecialization = memberFunctionInstantiation->isSpecialization();

                 // DQ (5/2/2012): Although I included that case for handling "isDefinedInClass"
                 // below, I think it should always be false for this handling of specializations.
                 // bool isDefinedInClass = memberFunctionInstantiation->isDefinedInClass();
                 // bool isDefinedInClass = false;
                    bool isDefinedInClass = memberFunctionInstantiation->isDefinedInClass();
#if 0
                    printf ("MarkTemplateInstantiationsForOutput::ProcessMemberFunctionTemplateDeclarations(): isSpecialization = %s \n",isSpecialization ? "true" : "false");
                    printf ("MarkTemplateInstantiationsForOutput::ProcessMemberFunctionTemplateDeclarations(): isDefinedInClass = %s \n",isDefinedInClass ? "true" : "false");
#endif
                    SgDeclarationStatement* templateDeclaration = memberFunctionInstantiation->get_templateDeclaration();
                 // bool templateDeclarationIsDeclaredInClass = (templateDeclaration->get_parent() == templateDeclaration->get_scope());
                    SgNode* parentOfTemplateDeclaration = templateDeclaration->get_parent();
                 // Later this test will have to be for "isTemplateDefinition(parentOfTemplateDeclaration));"
                 // Make this a little more general since a member function might appear in a non-templated class.
                 // bool templateDeclarationIsDeclaredInClass = (isSgTemplateInstantiationDefn(parentOfTemplateDeclaration) != NULL);
                    bool templateDeclarationIsDeclaredInClass = (isSgClassDefinition(parentOfTemplateDeclaration) != NULL);
#if 0
                    printf ("templateDeclaration->get_parent()    = %p = %s \n",templateDeclaration->get_parent(),templateDeclaration->get_parent()->class_name().c_str());
                    printf ("templateDeclaration->get_scope()     = %p = %s \n",templateDeclaration->get_scope(),templateDeclaration->get_scope()->class_name().c_str());
                    printf ("templateDeclarationIsDeclaredInClass = %s \n",templateDeclarationIsDeclaredInClass ? "true" : "false");
                 // printf ("templateDeclaration: get_name() = %s get_string() = %s \n",
                 //           templateDeclaration->get_name().str(),templateDeclaration->get_string().str());
                 // printf ("templateDeclaration: get_string() = %s \n",templateDeclaration->get_string().str());

                    printf ("memberFunctionInstantiation->get_definition() = %p \n",memberFunctionInstantiation->get_definition());
                 // printf ("memberFunctionInstantiation->get_parent()     = %p = %s \n",
                 //      memberFunctionInstantiation->get_parent(),memberFunctionInstantiation->get_parent()->class_name().c_str());
                 // printf ("memberFunctionInstantiation->get_scope()      = %p = %s \n",
                 //      memberFunctionInstantiation->get_scope(),memberFunctionInstantiation->get_scope()->class_name().c_str());
#endif

                 // DQ (5/2/2012): We have to check if it is defined in the class since then the template 
                 // string (still used for unparsing) will not have the function definitions.
                 // Check if this is a specialization in which case we have to put it out!
                 // if ( (isSpecialization == true) || (isDefinedInClass == true) )
                 // if ( isSpecialization == true )
                    if ( (isSpecialization == true) || (isDefinedInClass == true) )
                       {
#if 0
                         printf ("Found a specialization in the current file, mark the specialization for output (on line = %d) \n",templateDeclaration->get_file_info()->get_line());
#endif
                      // I assume this is a definition if we are marking it for output!
                      // ROSE_ASSERT(memberFunctionInstantiation->get_definition() != NULL);
#if 0
                         printf ("Calling markForOutputInCodeGeneration() for memberFunctionInstantiation = %p = %s \n",memberFunctionInstantiation,memberFunctionInstantiation->class_name().c_str());
#endif
                      // Mark this for output later when we generate code!
                         markForOutputInCodeGeneration(memberFunctionInstantiation);
                       }
                      else
                       {
#if 0
                         printf ("templateDeclarationIsDeclaredInClass = %s \n",templateDeclarationIsDeclaredInClass ? "true" : "false");
#endif
                         if (templateDeclarationIsDeclaredInClass == true)
                            {
                           // If it is not a specialization it might have been that the template declaration 
                           // appeared in the class in which case EDG has deleted the defining template 
                           // declaration and we only have the opportunity to output the generated template 
                           // instantiation (not a specialization, but a simple instantiation of the member 
                           // function).  In this case we have to mark the defining template instantiation 
                           // for output and if the instantiation of the class is not output we have to 
                           // output a member function prototype for the instantiated member function (since 
                           // it will be output at the end of the file (as an inlined function)).  I'm not
                           // clear if it is an issue that as an inlined function it is used (referenced)
                           // before it is defined (but it seems to work just fine, at least with some older 
                           // versions of the g++ compiler).
#if 0
                              printf ("Found special case of template defined in class and not output in class template definition (mark it for output) \n");
                              printf ("     member function qualified name = %s \n",memberFunctionInstantiation->get_qualified_name().str());
#endif

                           // DQ (8/8/2012): Output this declaration only if it is part of the class being output.
                           // DQ (8/26/2005): Suppress prototypes of constuctors (see test2005_147.C), not clear why these can't be output!
                           // bool processMemberFunction = true;
                              SgClassDefinition* parentClassDefinition = isSgClassDefinition(memberFunctionInstantiation->get_scope());
                              ROSE_ASSERT(parentClassDefinition != NULL);
                              SgClassDeclaration* parentClassDeclaration = parentClassDefinition->get_declaration();
                              ROSE_ASSERT(parentClassDeclaration != NULL);

                              bool processMemberFunction = (parentClassDeclaration->get_file_info()->isOutputInCodeGeneration() == true);

                           // DQ (8/27/2005): skipping constructors appears to be required for both g++ 3.3.x and 3.4.x
                           // special handling for non-defining constructor declarations
#if 0
                              printf ("memberFunctionInstantiation->get_specialFunctionModifier().isConstructor() = %s \n",memberFunctionInstantiation->get_specialFunctionModifier().isConstructor() ? "true" : "false");
#endif
                              if ( isDefiningDeclaration == false && memberFunctionInstantiation->get_specialFunctionModifier().isConstructor() == true )
                                 {
                                // printf ("Warning: Skipping output of constructor prototypes since their specialization is a problem (bug) in some versions of g++ (I think 3.4.x) \n");
                                   processMemberFunction = false;
                                 }

                           // printf ("processMemberFunction = %s \n",processMemberFunction ? "true" : "false");
                              if (processMemberFunction == true)
                                 {
                                // This is not a constructor prototype so it is OK to output the prototype
#if 0
                                   printf ("Calling markForOutputInCodeGeneration(memberFunctionInstantiation = %p) \n",memberFunctionInstantiation);
#endif
                                // Mark this for output later when we generate code!
                                // This marks the defining and non-defining declarations for output
                                // (which for the case of a constructor we fixup below).
                                   markForOutputInCodeGeneration(memberFunctionInstantiation);

                                // DQ (8/28/2005): It is a bug in g++ if we output the forward declaration
                                // of a member function specialization (for either a template or non-template 
                                // member function).
                                   if ( isDefiningDeclaration == true && memberFunctionInstantiation->get_specialFunctionModifier().isConstructor() == true )
                                      {
#if 0
                                        SgDeclarationStatement* temp = memberFunctionInstantiation->get_firstNondefiningDeclaration();
                                        printf ("Note: need to unmark non-defining constructor so that it will not be output (g++ bug) temp = %p = %s \n",temp,(temp != NULL) ? temp->class_name().c_str() : "NULL");
#endif
#if 0
                                     // DQ (11/5/2007): This is not required for the 4.1.2 compiler, and is even an error, but it might be required for the 3.x compilers.
                                        if (temp != NULL)
                                           temp->get_file_info()->unsetOutputInCodeGeneration();
#endif
                                      }

                                // DQ (8/26/2005): It still might be that the non-defining declaration is not in the global 
                                // scope so that marking it for output is still insufficient.

                                // test if the declaration appears in global scope

                                // DQ (10/11/2007): Note that even if the defining declaration of the template specialization 
                                // appears in global scope (which is OK) g++ 3.4.x and 4.x place additional constraints on 
                                // where the prototypes can be placed; so we cannot just put them into global scope after the 
                                // class declaration.  If the class declaration appears in a namespace, then it must go into
                                // the name space (and after the class declaration).

#if 0
                                   ROSE_ASSERT(memberFunctionInstantiation != NULL);
                                   printf ("test if the declaration appears in global scope memberFunctionInstantiation = %p = %s = %s \n",
                                        memberFunctionInstantiation,memberFunctionInstantiation->class_name().c_str(),SageInterface::get_name(memberFunctionInstantiation).c_str());
#endif
                                   SgGlobal* globalScope = file->get_globalScope();
                                   ROSE_ASSERT(globalScope != NULL);

                                // printf ("#####################  globalScope = %p \n",globalScope);

                                   SgDeclarationStatementPtrList & declarationList = globalScope->get_declarations();
                                   SgDeclarationStatementPtrList::iterator location = find(declarationList.begin(),declarationList.end(),*i);
                                // printf ("declarationList.begin() = %p  declarationList.end() = %p \n",*declarationList.begin(),*declarationList.end());
                                // printf ("location in enumeration of declarations in global scope = %p \n",*location);
#if 0
                                   printf ("location != declarationList.end() = %s memberFunctionInstantiation = %p globalScope = %p \n",location != declarationList.end() ? "true " : "false",memberFunctionInstantiation,globalScope);
#endif
                                   SgTemplateInstantiationMemberFunctionDecl* nondefiningMemberFunctionInstantiation = isSgTemplateInstantiationMemberFunctionDecl(memberFunctionInstantiation->get_firstNondefiningDeclaration());
                                   SgDeclarationStatementPtrList::iterator locationOfNondefiningDeclaration = find(declarationList.begin(),declarationList.end(),nondefiningMemberFunctionInstantiation);

                                   ROSE_ASSERT(nondefiningMemberFunctionInstantiation->get_templateDeclaration() == memberFunctionInstantiation->get_templateDeclaration());
                                // printf ("memberFunctionInstantiation->get_templateDeclaration() = %p \n",memberFunctionInstantiation->get_templateDeclaration());

                                // DQ (11/5/2007): Check for existance of both the defining and non-defining, since then we want to move the non-defining declaration (the TRUE case).
                                // But if the non-defining declaration does not exist then we want to build it (the FALSE case).
                                // if (location != declarationList.end())
                                   if (location != declarationList.end() && (locationOfNondefiningDeclaration != declarationList.end()) )
                                      {
                                     // Found the declaration in global scope
#if 0
                                        printf ("Found the declaration in global scope \n");
#endif
                                     // DQ (10/11/2007): Added support to move prototypes of template specializations into the correct namespace (required by g++ version later than 3.3.x)
#if 0
                                        printf ("isDefiningDeclaration = %s \n",isDefiningDeclaration ? "true" : "false");
#endif
                                     // DQ (11/4/2007): If there is a defining declaration then wait until we process that declaration, if there is not one then move the trigger 
                                     // this transformation on the basis of the non-defining declaration (since the defiing declaration might not exist). The point is to only do 
                                     // this relocation once (however, I thinkwe may have to also do it for the defining declaration as well, not just for the non-defining declaration.
                                     // In this case we would have to do the relocation each time we see the declaration (defining or non-defiing).  This initial implementatio has 
                                     // only handle the non-defining declaration.
                                     // Older comment: Wait until we see the defining declaration and then move the nondefining declaration if it was declared in a class defined in a namespace.
                                     // printf ("Check if there is a defining declaration memberFunctionInstantiation->get_definingDeclaration() = %p \n",memberFunctionInstantiation->get_definingDeclaration());
                                     // if (isDefiningDeclaration == true)
                                        if (isDefiningDeclaration == true || memberFunctionInstantiation->get_definingDeclaration() == NULL)
                                           {
#if 0
                                             printf ("Found a defining declaration in global scope, if the non-defining declaration is from a class in a namespace, then we have to move the non-defining declaration! \n");
#endif
                                          // Refactored some code to the SageInterface
                                             SgNamespaceDefinitionStatement* classNamespaceScope = SageInterface::enclosingNamespaceScope(memberFunctionInstantiation);
#if 0
                                             printf ("classNamespaceScope = %p \n",classNamespaceScope);
#endif
                                          // This will invalidate any outstanding iterator defined on declarationList
                                             SgTemplateInstantiationMemberFunctionDecl* nondefiningMemberFunctionInstantiation = isSgTemplateInstantiationMemberFunctionDecl(memberFunctionInstantiation->get_firstNondefiningDeclaration());
                                             SgDeclarationStatementPtrList::iterator locationOfNondefiningDeclaration = find(declarationList.begin(),declarationList.end(),nondefiningMemberFunctionInstantiation);
                                          // ROSE_ASSERT(locationOfNondefiningDeclaration != declarationList.end());
#if 0
                                             printf ("locationOfNondefiningDeclaration != declarationList.end() = %s \n",locationOfNondefiningDeclaration != declarationList.end() ? "true" : "false");
#endif
                                             if ( (classNamespaceScope != NULL ) && (locationOfNondefiningDeclaration != declarationList.end()) )
                                          // if ( (classNamespaceScope != NULL ) ) // && (locationOfNondefiningDeclaration != declarationList.end()) )
                                                {
                                               // We have to move the prototype of the specialized template to the namespace (after the declaration)
#if 0
                                                  printf ("We have to move the prototype %p of the specialized template to the namespace (after the declaration) erase it from where it is currently ...\n",nondefiningMemberFunctionInstantiation);
#endif
                                                  declarationList.erase(locationOfNondefiningDeclaration);

                                               // Refactored some code to the SageInterface
                                               // DQ (11/4/2007): Get the associated declaration, if it is part of a template instantiation then get the template declaration
                                                  SgDeclarationStatement* parentDeclaration = SageInterface::getNonInstantiatonDeclarationForClass(memberFunctionInstantiation);
#if 0
                                                  printf ("parentDeclaration              = %p = %s \n",parentDeclaration,parentDeclaration->class_name().c_str());
                                                  printf ("parentDeclaration->get_scope() = %p = %s \n",parentDeclaration->get_scope(),parentDeclaration->get_scope()->class_name().c_str());
#endif
                                                  bool inFront = false;
                                                  classNamespaceScope->insert_statement(parentDeclaration,nondefiningMemberFunctionInstantiation,inFront);
#if 0
                                                  printf ("DONE with repositioning of prototype from global scope to namespace scope \n");
#endif
                                                }
                                               else
                                                {
#if 0
                                                  printf ("For the non-defining declaration, no namespace was found in chain of scopes back to global scope! (so we do NOT have to relocate the template function prototype) \n");
#endif
#if 0
                                               // If it was a defining declaration but the non-defining declaration doesn't exist then we have to build one 
                                                  printf ("nondefiningMemberFunctionInstantiation = %p \n",nondefiningMemberFunctionInstantiation);

                                                  printf ("If it was a defining declaration but the non-defining declaration doesn't exist then we have to build one \n");
                                               // ROSE_ASSERT(false);
#endif
                                                }
                                           }
                                          else
                                           {
#if 0
                                             printf ("We will move the nondefining declaration when we find the defining declaration (deferred relocation of prototype to later). \n");
#endif
                                           }
                                      }
                                     else
                                      {
                                     // Could not find the declaration in global scope, so add it after the class declaration.
#if 0
                                        printf ("Could not find the declaration in global scope so add it after the class declaration (for now) \n");
                                        printf ("Before building prototype: memberFunctionInstantiation = %p defining = %p nondefining = %p \n",memberFunctionInstantiation,
                                             memberFunctionInstantiation->get_firstNondefiningDeclaration(),memberFunctionInstantiation->get_definingDeclaration());
#endif

                                        ROSE_ASSERT(memberFunctionInstantiation->get_class_scope() != NULL);

                                        SgDeclarationStatement* parentDeclaration = memberFunctionInstantiation->get_associatedClassDeclaration();
                                        ROSE_ASSERT(parentDeclaration != NULL);
#if 0
                                        printf ("parentDeclaration = %p = %s \n",parentDeclaration,parentDeclaration->class_name().c_str());
                                        printf ("parentDeclaration->get_parent() = %p = %s \n",parentDeclaration->get_parent(),parentDeclaration->get_parent()->class_name().c_str());
                                        printf ("parentDeclaration->get_scope() = %p = %s \n",parentDeclaration->get_scope(),parentDeclaration->get_scope()->class_name().c_str());

                                     // DQ (10/11/2007): I would like to uncomment this assertion.
                                     // DQ (12/22/2006): This is a reasonable case that we need to allow!
                                     // DQ (8/30/2005): I don't think this is a problem! (see test2005_153.C)
                                        if (parentDeclaration->get_scope() != globalScope)
                                           {
                                             printf ("########## parentDeclaration->get_scope() is not global scope it is a %s ########## \n",parentDeclaration->get_scope()->class_name().c_str());
                                             printf ("memberFunctionInstantiation = %p defining = %p nondefining = %p \n",memberFunctionInstantiation,
                                                  memberFunctionInstantiation->get_firstNondefiningDeclaration(),memberFunctionInstantiation->get_definingDeclaration());
                                           }
                                     // ROSE_ASSERT(parentDeclaration->get_scope() == globalScope);
#endif

//                                      SgClassDeclaration* parentClassDeclaration = isSgClassDeclaration(parentDeclaration);
//                                      SgNonrealDecl* parentNonrealDecl = isSgNonrealDecl(parentDeclaration);

                                     // DQ (11/4/2007): This looks for a forward declaration of matching name exists in the specificed scope (starting as "parentClassDeclaration").
                                        bool foundExistingPrototype = SageInterface::isPrototypeInScope ( globalScope, memberFunctionInstantiation, parentDeclaration );

                                     // DQ (11/4/2007): If not found in global scope then check if it has already been moved to a namespace.
                                        SgNamespaceDefinitionStatement* classNamespaceScope = SageInterface::enclosingNamespaceScope(memberFunctionInstantiation);
#if 0
                                        printf ("classNamespaceScope = %p \n",classNamespaceScope);
#endif
                                        if (foundExistingPrototype == false)
                                           {
                                          // Find the correct namespace
                                          // SgNamespaceDefinitionStatement* classNamespaceScope = SageInterface::enclosingNamespaceScope(memberFunctionInstantiation);

                                             if (classNamespaceScope != NULL)
                                                {
                                                  SgTemplateInstantiationMemberFunctionDecl* nondefiningMemberFunctionInstantiation = isSgTemplateInstantiationMemberFunctionDecl(memberFunctionInstantiation->get_firstNondefiningDeclaration());
                                                  ROSE_ASSERT(nondefiningMemberFunctionInstantiation != NULL);

                                                  foundExistingPrototype = SageInterface::isPrototypeInScope ( classNamespaceScope, memberFunctionInstantiation, nondefiningMemberFunctionInstantiation );
                                                }
                                           }
#if 0
                                        printf ("foundExistingPrototype = %s \n",foundExistingPrototype ? "true" : "false");
#endif
                                     // DQ (11/3/2007): Use the new test result to avoid redundantly adding prototypes.
                                        if (foundExistingPrototype == false)
                                           {
                                          // DQ (11/3/2007): Only build the new member function prototype declaration if we are going to insert it into place.
                                             SgDeclarationStatementPtrList::iterator parentLocation = find(declarationList.begin(),declarationList.end(),parentDeclaration);
                                             if (parentLocation != declarationList.end())
                                                {
#if 0
                                                  printf ("Building a new template member function prototype \n");
#endif
                                               // Use the version of the function located in the SageInterface
                                                  SgTemplateInstantiationMemberFunctionDecl* nondefiningMemberFunctionInstantiation = isSgTemplateInstantiationMemberFunctionDecl(memberFunctionInstantiation->get_firstNondefiningDeclaration());
                                               // SgTemplateInstantiationMemberFunctionDecl* copyOfMemberFunction = SageInterface::buildForwardFunctionDeclaration(memberFunctionInstantiation);
                                                  ROSE_ASSERT(nondefiningMemberFunctionInstantiation != NULL);
                                                  SgTemplateInstantiationMemberFunctionDecl* copyOfMemberFunction = SageInterface::buildForwardFunctionDeclaration(nondefiningMemberFunctionInstantiation);

                                               // DQ (8/28/2005): Mark it as extern "C++" since it might be placed
                                               // in an extern "C" portion of the source code!
                                               // This happends in swig generated code (polyhedralcmiswig.cc in KULL).
                                                  copyOfMemberFunction->get_declarationModifier().get_storageModifier().setExtern();
                                                  copyOfMemberFunction->set_linkage("C++");
#if 0
                                                  printf ("Inserting after class declaration: copyOfMemberFunction = %p \n",copyOfMemberFunction);
#endif
#if 0
                                                  printf ("Before INSERT: memberFunctionInstantiation = %p scope = %p = %s \n",
                                                       memberFunctionInstantiation,
                                                       memberFunctionInstantiation->get_scope(),
                                                       memberFunctionInstantiation->get_scope()->class_name().c_str());
#endif
                                               // This is important to copytest2007_64.C
                                                  bool inFront = false;
                                               // SgTemplateInstantiationMemberFunctionDecl* nondefiningMemberFunctionInstantiation = isSgTemplateInstantiationMemberFunctionDecl(memberFunctionInstantiation->get_firstNondefiningDeclaration());

                                               // I think we can assert this!
                                               // ROSE_ASSERT(memberFunctionInstantiation == nondefiningMemberFunctionInstantiation);

                                               // SgDeclarationStatementPtrList::iterator locationOfNondefiningDeclaration = find(declarationList.begin(),declarationList.end(),nondefiningMemberFunctionInstantiation);
                                               // if ( (classNamespaceScope != NULL ) && (locationOfNondefiningDeclaration != declarationList.end()) )
                                                  if ( (classNamespaceScope != NULL ) )
                                                     {
                                                    // We have to move the prototype of the specialized template to the namespace (after the declaration)
#if 0
                                                       printf ("We have make a copy of the prototype %p of the specialized template and insert it into the namespace (after the declaration) ...\n",copyOfMemberFunction);
#endif
                                                       SgDeclarationStatement* parentDeclaration = SageInterface::getNonInstantiatonDeclarationForClass(memberFunctionInstantiation);
#if 0
                                                       printf ("Insert prototype into classNamespaceScope = %p = %s \n",classNamespaceScope,classNamespaceScope->class_name().c_str());
                                                       printf ("parentDeclaration->get_scope()            = %p = %s \n",parentDeclaration->get_scope(),parentDeclaration->get_scope()->class_name().c_str());
#endif
                                                       bool inFront = false;
                                                       classNamespaceScope->insert_statement(parentDeclaration,copyOfMemberFunction,inFront);

                                                       ROSE_ASSERT(copyOfMemberFunction->get_parent() == classNamespaceScope);
                                                     }
                                                    else
                                                     {
                                                    // printf ("classNamespaceScope == NULL || (locationOfNondefiningDeclaration == declarationList.end()): Insert prototype into globalScope \n");
#if 0
                                                       printf ("classNamespaceScope == NULL: Insert prototype into globalScope \n");
#endif
                                                       globalScope->insert_statement(parentDeclaration,copyOfMemberFunction,inFront);

                                                       ROSE_ASSERT(copyOfMemberFunction->get_parent() == globalScope);
                                                     }
                                               // DQ (10/12/2007): Reset the parent to reflect the structural change.
                                               // ROSE_ASSERT(copyOfMemberFunction->get_parent() == globalScope);
                                                }
                                               else
                                                {
                                               // parent class not found in global scope!
#if 0
                                               // DQ (11/3/2007): Make this an error!
                                                  printf ("Warning: parent class of template instantiation member function not found in global scope! (Can't insert forward declaration of template member function specialization, but not required here, I think) \n");
                                               // ROSE_ASSERT(false);
#endif
                                                }
                                           }
                                          else
                                           {
#if 0
                                             printf ("A prototype for this member function either already existed or was previously added. memberFunctionInstantiation = %p = %s = %s \n",
                                                  memberFunctionInstantiation,memberFunctionInstantiation->class_name().c_str(),SageInterface::get_name(memberFunctionInstantiation).c_str());
#endif
                                           }
                                     // end of case "Could not find the declaration in global scope, so add it after the class declaration."
                                      }
                                 }
                                else
                                 {
#if 0
                                   printf ("Skipped marking for output! (likely a constructor, which are a g++ error to output as prototype specialization) \n");
#endif
                                 }
                            }
                           else
                            {
#if 0
                              printf ("Not a specialization, so skip call to markForOutputInCodeGeneration for template *i = %p = %s instantated from line %d \n",
                                   memberFunctionInstantiation,memberFunctionInstantiation->class_name().c_str(),
                                   templateDeclaration->get_file_info()->get_line());
#endif
                            }
                      // end of else case for "if ( isSpecialization == true )"
                       }
                  }
                 else
                  {
                 // Since the template declaration is in another file it will be seen when we the vendor 
                 // compiler processes the current file (using the same includes) and so we don't have to 
                 // worry about the output of this instantiated template.
#if 0
                    printf ("Skipping call to markForOutputInCodeGeneration on memberFunctionInstantiation = %p = %s \n",
                         memberFunctionInstantiation,memberFunctionInstantiation->class_name().c_str());
#endif
                  }

                 // END: Indentation issue in this source code.
                  }
                 else
                  {
                 // DQ (1/21/2013): This case is part of supporting template instnations than don't have an associated template declaration.
#if 0
                    printf ("In ProcessMemberFunctionTemplateDeclarations(): Note that some template instantiations might not have an associated template declaration (see test2013_08.C, and BoxLib test2013_15.C) \n");
#endif
                  }
               
#if 0
               printf ("*************************************** \n");
#endif
             } // if (memberFunctionInstantiation != NULL)
        } // for loop over set of needed declarations
   }


void
MarkTemplateInstantiationsForOutput::ProcessFunctionTemplateDeclarations ( set<SgDeclarationStatement*> setOfRequiredDeclarations, SgSourceFile* file )
   {
     ROSE_ASSERT(file != NULL);

     for ( set<SgDeclarationStatement*>::iterator i = setOfRequiredDeclarations.begin(); i != setOfRequiredDeclarations.end(); i++ )
        {
       // Iterate through the function recorded as required for compilation of this file!

       // ROSE_ASSERT((*i)->get_definingDeclaration() != NULL);
       // ROSE_ASSERT((*i)->get_firstNondefiningDeclaration() != NULL);

          SgDeclarationStatement* definingDeclaration         = (*i)->get_definingDeclaration();
          SgDeclarationStatement* firstNondefiningDeclaration = (*i)->get_firstNondefiningDeclaration();

          bool isDefiningDeclaration         = definingDeclaration         == NULL ? false : (*i == definingDeclaration);
          bool isfirstNondefiningDeclaration = firstNondefiningDeclaration == NULL ? false : (*i == firstNondefiningDeclaration);
       // first debug the member functions!
#if 0
          printf ("In ProcessFunctionTemplateDeclarations() set element = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
          SgTemplateInstantiationFunctionDecl* functionInstantiation = isSgTemplateInstantiationFunctionDecl(*i);
          if (functionInstantiation != NULL)
             {
#if 0
               printf ("\n\n");
               printf ("*************************************** \n");
               printf ("      Required declaration = %p = %s = %s (is defining decl = %s) (is first non-defining decl = %s) \n",
                                   *i,(*i)->sage_class_name(),SageInterface::get_name(*i).c_str(),
                                   isDefiningDeclaration ? "true" : "false", 
                                   isfirstNondefiningDeclaration ? "true" : "false");
               printf ("(*i)->get_file_info() = %p \n",(*i)->get_file_info());
               (*i)->get_file_info()->display("required declaration: debug");
#endif
            // At least one of these should be true!
               ROSE_ASSERT ( isfirstNondefiningDeclaration == true || isDefiningDeclaration == true );

#if 0
               printf ("functionInstantiation->get_templateDeclaration() = %p \n",functionInstantiation->get_templateDeclaration());
#endif

            // If the template declaration is in the current file then we need not output the instantiation (skip marking instantiation for output!)
               SgDeclarationStatement* templateDeclaration = functionInstantiation->get_templateDeclaration();

            // DQ (11/3/2012): Changed this assertion to a conditional.
            // Looking at the code for where the template declaration is set, it seems reasonable that it might 
            // not be found since p->assoc_template == NULL. However it seems that there are other ways to find 
            // it.  This is written about in a note in setTemplateOrTemplateInstantiationFunctionGeneration().
#if 0
               printf ("In ProcessFunctionTemplateDeclarations(): COMMENTED OUT ASSERTION (templateDeclaration != NULL) to generate AST graph \n");
#endif
            // ROSE_ASSERT(templateDeclaration != NULL);
#if 1
               if (templateDeclaration != NULL)
                  {
#if 0
            // printf ("templateDeclaration = %p = %s \n",templateDeclaration,templateDeclaration->get_name().str());
               printf ("templateDeclaration = %p = %s \n",templateDeclaration,templateDeclaration->class_name().c_str());
               printf ("templateDeclaration->get_firstNondefiningDeclaration() = %p \n",templateDeclaration->get_firstNondefiningDeclaration());
               printf ("templateDeclaration->get_definingDeclaration()         = %p \n",templateDeclaration->get_definingDeclaration());
#endif
               string currentFilename = (file != NULL) ? file->getFileName() : "";
               string filenameOfTemplateDeclaration = templateDeclaration->get_file_info()->get_filename();
#if 0
               templateDeclaration->get_file_info()->display("templateDeclaration file name problem: debug");
               printf ("currentFilename               = %s \n",currentFilename.c_str());
               printf ("filenameOfTemplateDeclaration = %s \n",filenameOfTemplateDeclaration.c_str());
#endif
               if (filenameOfTemplateDeclaration == currentFilename)
                  {
                    bool isSpecialization = functionInstantiation->isSpecialization();
#if 0
                    printf ("In MarkTemplateInstantiationsForOutput::ProcessFunctionTemplateDeclarations(): isSpecialization = %s \n",isSpecialization ? "true" : "false");
#endif
                    if (isSpecialization == true)
                       {
#if 0
                         printf ("Calling markForOutputInCodeGeneration on functionInstantiation = %p = %s \n",functionInstantiation,functionInstantiation->class_name().c_str());
#endif
                         markForOutputInCodeGeneration (functionInstantiation);
                       }
                  }
                  }
#else
               printf ("In ProcessFunctionTemplateDeclarations(): COMMENTED OUT ASSERTION (templateDeclaration != NULL) to generate AST graph \n");
#endif
             }
        }
   }

void
MarkTemplateInstantiationsForOutput::ProcessClassTemplateDeclarations ( set<SgDeclarationStatement*> setOfRequiredDeclarations, SgSourceFile* file )
   {
     ROSE_ASSERT(file != NULL);

     for ( set<SgDeclarationStatement*>::iterator i = setOfRequiredDeclarations.begin(); i != setOfRequiredDeclarations.end(); i++ )
        {
       // Iterate through the function recorded as required for compilation of this file!

       // ROSE_ASSERT((*i)->get_definingDeclaration() != NULL);
       // ROSE_ASSERT((*i)->get_firstNondefiningDeclaration() != NULL);

          SgDeclarationStatement* definingDeclaration         = (*i)->get_definingDeclaration();
          SgDeclarationStatement* firstNondefiningDeclaration = (*i)->get_firstNondefiningDeclaration();

          bool isDefiningDeclaration         = definingDeclaration         == NULL ? false : (*i == definingDeclaration);
          bool isfirstNondefiningDeclaration = firstNondefiningDeclaration == NULL ? false : (*i == firstNondefiningDeclaration);
       // first debug the member functions!

#if 0
          printf ("In ProcessClassTemplateDeclarations() set element = %p = %s \n",*i,(*i)->class_name().c_str());
#endif

          SgTemplateInstantiationDecl* classInstantiation = isSgTemplateInstantiationDecl(*i);
          if (classInstantiation != NULL)
             {
#if 0
               printf ("\n\n");
               printf ("*************************************** \n");
               printf ("      Required declaration = %p = %s = %s (is defining decl = %s) (is first non-defining decl = %s) \n",
                                   *i,(*i)->sage_class_name(),SageInterface::get_name(*i).c_str(),
                                   isDefiningDeclaration ? "true" : "false", 
                                   isfirstNondefiningDeclaration ? "true" : "false");
               printf ("(*i)->get_file_info() = %p \n",(*i)->get_file_info());
               (*i)->get_file_info()->display("required declaration: debug");
#endif
            // At least one of these should be true!
               ROSE_ASSERT ( isfirstNondefiningDeclaration == true || isDefiningDeclaration == true );

            // If the template declaration is in the current file then we need not output the instantiation (skip marking instantiation for output!)
               SgDeclarationStatement* templateDeclaration = classInstantiation->get_templateDeclaration();
               ROSE_ASSERT(templateDeclaration != NULL);
#if 0
            // printf ("templateDeclaration = %p = %s \n",templateDeclaration,templateDeclaration->get_name().str());
               printf ("templateDeclaration = %p = %s \n",templateDeclaration,templateDeclaration->class_name().c_str());
               printf ("templateDeclaration->get_firstNondefiningDeclaration() = %p \n",templateDeclaration->get_firstNondefiningDeclaration());
               printf ("templateDeclaration->get_definingDeclaration()         = %p \n",templateDeclaration->get_definingDeclaration());
#endif
               string currentFilename = (file != NULL) ? file->getFileName() : "";
               string filenameOfTemplateDeclaration = templateDeclaration->get_file_info()->get_filename();
#if 0
               templateDeclaration->get_file_info()->display("templateDeclaration file name problem: debug");
               printf ("currentFilename               = %s \n",currentFilename.c_str());
               printf ("filenameOfTemplateDeclaration = %s \n",filenameOfTemplateDeclaration.c_str());
#endif
               if (filenameOfTemplateDeclaration == currentFilename)
                  {
                    bool isSpecialization = classInstantiation->isSpecialization();
#if 0
                    printf ("MarkTemplateInstantiationsForOutput::ProcessClassTemplateDeclarations(): isSpecialization = %s \n",isSpecialization ? "true" : "false");
#endif
#if 0
                 // DQ (5/15/2012): Need to be careful about forcing this to be treated as a specialization if it is not explicitly a specialization in the source code (comment out).
                 // DQ (5/2/2012): Force this to be a specialization since it is used (the declaration was in the class but is not output that way in the template string).
                    printf ("Forcing this template instantiation to be treated as a specialization (and output in the generated code). \n");
                    classInstantiation->set_specialization(SgDeclarationStatement::e_specialization);
                    isSpecialization = classInstantiation->isSpecialization();
#endif
                    if (isSpecialization == true)
                       {
#if 0
                         printf ("Calling markForOutputInCodeGeneration on classInstantiation = %p = %s \n",classInstantiation,classInstantiation->class_name().c_str());
#endif
                         markForOutputInCodeGeneration (classInstantiation);
                       }
                  }
             }
#if 0
          printf ("At base of loop: ProcessClassTemplateDeclarations() set element = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
        }
   }


void
markTemplateInstantiationsForOutput( SgNode* node )
   {
  // This function marks template instantiations for output within code generation.
  // By isolating the control over the output of template instantiations we simplify 
  // the design of the unparser (code generator).  Note also that while template
  // specializations appear as template declaration in their syntax, they are functionally
  // just explicit template instantiations.

  // This function has multiple phases:
  //    1) Locate all template instantiations that are referenced in the source file.
  //    2) Iterate over the list of instantiatied member functions
  //          a) if it is a specialization then mark it for output
  //          b) if the associated template definition appears in the source file then
  //                1. if it is defined in the class then mark the instantiation for output
  //                        EDG does not include the definition of member function in the string
  //                        representing the template definition.
  //                2. if it is not defined in the class then don't output the instantiation.
  //              FOR G++ 3.3.x
  //                3. Build a prototype for the member function and insert it into the correct 
  //                   scope (global scope should work).
  //              FOR G++ 3.4.x and likely 4.x (also works for 3.3.x)
  //                4. Move the definition to appear after the class instantiation's definition
  //    3) Iterate over the template function instantiations
  //          a) if it is a specialization then mark it for output
  //          b) if the associated template definition appears in the source file then 
  //             do NOT mark it for output.
  //    4) Iterate over the template class instantiations
  //          a) if it is a specialization then mark it for output
  //          b) if the associated template definition appears in the source file then 
  //             do NOT mark it for output.
  //          c) if a member function (or friend function) of the class template instantiation 
  //             is marked for output but the class template instantiation is not then copy the 
  //             member function's declaration and insert it after the class template instantiation.

  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Mark template instantiations for output:");

  // DQ (8/2/2005): Added better handling of AST fragments where template handling is not required!
  // DQ (7/29/2005): Added support with Qing for AST framents that occure in the ASTInterface classes.
     SgSourceFile* file                = NULL;
     SgProject* project                = NULL;
  // bool buildImplicitTemplates = false;

     ROSE_ASSERT(node != NULL);
     file = TransformationSupport::getSourceFile(node);
     project = isSgProject(node);
  // buildImplicitTemplates = (file != NULL) && (file->get_no_implicit_templates() == false);

  // printf ("buildImplicitTemplates = %s \n",buildImplicitTemplates ? "true" : "false");
  // if (buildImplicitTemplates == true)

  // printf ("markTemplateInstantiationsForOutput using file = %p \n",file);

  // This fixup is only possible if we have access to the entire AST (including SgFile)
     if (file != NULL)
        {
       // *************************************************************
       // Collect template instantiations that are used and MIGHT be output
       // *************************************************************

#if 0
          printf ("In markTemplateInstantiationsForOutput(): Calling BuildSetOfRequiredTemplateDeclarations() \n");
#endif

       // Build the lists of declarations. This requires multiple passes
       // to include function included by functions previously included
       // So we generate one list for each pass, this helps the debugging.
       // since we only look for declarations that are both defined in the 
       // current file and used in the current source file we only record 
       // those templates that are required.
          set<SgDeclarationStatement*> setOfRequiredDeclarations = MarkTemplateInstantiationsForOutput::BuildSetOfRequiredTemplateDeclarations(node,file);

#if 0
          printf ("In markTemplateInstantiationsForOutput(): setOfRequiredDeclarations: \n");
          for (set<SgDeclarationStatement*>::iterator i = setOfRequiredDeclarations.begin(); i != setOfRequiredDeclarations.end(); i++)
             {
               printf ("   --- set element = %p = %s \n",*i,(*i)->class_name().c_str());
             }
#endif
#if 0
          printf ("In markTemplateInstantiationsForOutput(): ProcessMemberFunctionTemplateDeclarations(): setOfRequiredDeclarations.size() = %" PRIuPTR " \n",setOfRequiredDeclarations.size());
#endif

       // ***************************************************************************************
       // Iterate over member function template instantiations and figure out which ones to output
       // ***************************************************************************************
          MarkTemplateInstantiationsForOutput::ProcessMemberFunctionTemplateDeclarations(setOfRequiredDeclarations,file);

#if 0
          printf ("In markTemplateInstantiationsForOutput(): ProcessFunctionTemplateDeclarations(): setOfRequiredDeclarations.size() = %" PRIuPTR " \n",setOfRequiredDeclarations.size());
#endif

       // ********************************************************************************
       // Iterate over function template instantiations and figure out which ones to output
       // ********************************************************************************

          MarkTemplateInstantiationsForOutput::ProcessFunctionTemplateDeclarations(setOfRequiredDeclarations,file);

#if 0
          printf ("In markTemplateInstantiationsForOutput(): ProcessClassTemplateDeclarations(): setOfRequiredDeclarations.size() = %" PRIuPTR " \n",setOfRequiredDeclarations.size());
#endif

       // *****************************************************************************
       // Iterate over class template instantiations and figure out which ones to output
       // *****************************************************************************

          MarkTemplateInstantiationsForOutput::ProcessClassTemplateDeclarations(setOfRequiredDeclarations,file);

        } // end of if (file != NULL)
       else 
        {
          if (project != NULL)
             {
            // GB (9/4/2009): Added this case for handling SgProject nodes. We do
            // this simply by iterating over the list of files in the project and
            // calling this function recursively. This is only one level of
            // recursion since files are not nested.
               SgFilePtrList &files = project->get_fileList();
               SgFilePtrList::iterator fIterator;
               for (fIterator = files.begin(); fIterator != files.end(); ++fIterator)
                  {
                    SgFile *file = *fIterator;
                    ROSE_ASSERT(file != NULL);
                    markTemplateInstantiationsForOutput(file);
                  }
             }
        }

#if 0
          printf ("Exiting after output of required declarations \n");
          ROSE_ASSERT(false);
#endif
   }

MarkTemplateInstantiationsForOutputSupportInheritedAttribute::MarkTemplateInstantiationsForOutputSupportInheritedAttribute()
   {
     insideDeclarationToOutput = false;
   }

MarkTemplateInstantiationsForOutputSupportSynthesizedAttribute::MarkTemplateInstantiationsForOutputSupportSynthesizedAttribute()
   {
   }


MarkTemplateInstantiationsForOutputSupport::MarkTemplateInstantiationsForOutputSupport (SgSourceFile* inputFile)
   {
  // Save the file in the traversal class so that we can access the backend specific 
  // template instatiation control flags easily.
     ROSE_ASSERT(inputFile != NULL);
     currentFile = inputFile;
   }

void
MarkTemplateInstantiationsForOutputSupport::saveDeclaration ( SgDeclarationStatement* declaration )
   {
  // Put the declaration's first non-defining declaration in the 
  // list with it's defining declaration, if it exists.

     SgDeclarationStatement* firstNondefiningDeclaration = declaration->get_firstNondefiningDeclaration();
     SgDeclarationStatement* definingDeclaration         = declaration->get_definingDeclaration();
#if 0
     if (firstNondefiningDeclaration == NULL)
        {
          printf ("Error firstNondefiningDeclaration == NULL declaration = %p = %s \n",declaration,declaration->sage_class_name());
          declaration->get_file_info()->display("Error definingDeclaration == NULL");
        }
#endif
  // DQ (8/30/2005): This need not always exist either
  // ROSE_ASSERT(firstNondefiningDeclaration != NULL);

#if 0
     if (definingDeclaration == NULL)
        {
          printf ("Error definingDeclaration == NULL declaration = %p = %s \n",declaration,declaration->sage_class_name());
          declaration->get_file_info()->display("Error definingDeclaration == NULL");
        }
#endif

  // DQ (6/22/2005): The defining declaration does not always exist (e.g. "~int();")
  // ROSE_ASSERT(definingDeclaration != NULL);

  // DQ (8/30/2005): This should at leas be true!
     ROSE_ASSERT(firstNondefiningDeclaration != NULL || definingDeclaration != NULL);

     if (firstNondefiningDeclaration != NULL)
        {
#if 0
          printf ("Saving firstNondefiningDeclaration = %p = %s as a used declaration \n",firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str());
#endif
       // DQ (3/31/2013): Only add each declaration once!
       // listOfTemplateDeclarationsToOutput.push_back(firstNondefiningDeclaration);
          if (find(listOfTemplateDeclarationsToOutput.begin(),listOfTemplateDeclarationsToOutput.end(),firstNondefiningDeclaration) == listOfTemplateDeclarationsToOutput.end())
             {
               listOfTemplateDeclarationsToOutput.push_back(firstNondefiningDeclaration);
             }
          ROSE_ASSERT(find(listOfTemplateDeclarationsToOutput.begin(),listOfTemplateDeclarationsToOutput.end(),firstNondefiningDeclaration) != listOfTemplateDeclarationsToOutput.end());
        }

  // The defining declaration does not always exist! (e.g. "extern struct _IO_FILE_plus _IO_2_1_stdin_;")
     if (definingDeclaration != NULL)
        {
#if 0
          printf ("Saving definingDeclaration = %p = %s as a used declaration \n",definingDeclaration,definingDeclaration->class_name().c_str());
#endif
       // DQ (3/31/2013): Only add each declaration once!
       // listOfTemplateDeclarationsToOutput.push_back(definingDeclaration);
          if (find(listOfTemplateDeclarationsToOutput.begin(),listOfTemplateDeclarationsToOutput.end(),definingDeclaration) == listOfTemplateDeclarationsToOutput.end())
             {
               listOfTemplateDeclarationsToOutput.push_back(definingDeclaration);
             }
          ROSE_ASSERT(find(listOfTemplateDeclarationsToOutput.begin(),listOfTemplateDeclarationsToOutput.end(),definingDeclaration) != listOfTemplateDeclarationsToOutput.end());
        }
   }


MarkTemplateInstantiationsForOutputSupportInheritedAttribute
MarkTemplateInstantiationsForOutputSupport::evaluateInheritedAttribute ( 
   SgNode* node,
   MarkTemplateInstantiationsForOutputSupportInheritedAttribute inheritedAttribute )
   {
     static int staticCounter = 0;
     MarkTemplateInstantiationsForOutputSupportInheritedAttribute returnAttribute = inheritedAttribute;

  // Mark this explicitly as false to turn off effect of SgGlobal turning it on
     returnAttribute.insideDeclarationToOutput = false;

#if 0
     printf ("In MarkTemplateInstantiationsForOutputSupport::evaluateInheritedAttribute(): node = %p = %s = %s \n",
          node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
#endif
#if 0
     SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDecl = isSgTemplateInstantiationFunctionDecl(node);
     if (templateInstantiationFunctionDecl != NULL)
        {
          printf ("   --- SgTemplateInstantiationFunctionDecl IR node = %p name = %s \n",node,templateInstantiationFunctionDecl->get_name().str());
       // printf ("   --- non-defining declaration = %p defining declaration = %p \n",templateInstantiationFunctionDecl->get_firstNondefiningDeclaration(),templateInstantiationFunctionDecl->get_definingDeclaration());
          printf ("   --- non-defining declaration = %p \n",templateInstantiationFunctionDecl->get_firstNondefiningDeclaration());
          printf ("   --- defining declaration     = %p \n",templateInstantiationFunctionDecl->get_definingDeclaration());
        }
#endif

     Sg_File_Info* fileInfo = node->get_file_info();
     if ( fileInfo != NULL )
        {
       // If this is marked for output then record this in the inherited attribute to be returned
          if ( fileInfo->isCompilerGeneratedNodeToBeUnparsed() == true )
             {
#if 0
               printf ("   --- fileInfo->isCompilerGeneratedNodeToBeUnparsed() == true \n");
#endif
            // printf ("Skipping compiler generated IR nodes to be unparsed = %s = %s \n",node->sage_class_name(),SageInterface::get_name(node).c_str());
            // returnAttribute.insideDeclarationToOutput = true;
            // printf ("Found compiler generated IR node to be unparsed = %s \n",node->sage_class_name());
             }
            else
             {
            // Maybe SgGlobal should return false for hasPositionInSource()?
               if (fileInfo->hasPositionInSource() == true)
                  {
                 // This node has a position is some source code so we can check if it is part of the current file!
#if 0
                    printf ("   --- In MarkTemplateInstantiationsForOutputSupport::evaluateInheritedAttribute(): currentFile = %s IR node from %s at line %d \n",
                            currentFile->getFileName().c_str(),fileInfo->get_filename(),fileInfo->get_line());
#endif
                    if ( (fileInfo->isSameFile(currentFile) == true) && (isSgGlobal(node) == NULL) )
                       {
                      // This is a node from the current file!
                         returnAttribute.insideDeclarationToOutput = true;
#if 0
                         printf ("   --- Found IR node %s from source file = %s at %d \n",node->sage_class_name(),fileInfo->get_filename(),fileInfo->get_line());
#endif
#if 0
                         if (staticCounter > 1)
                            {
                              printf ("Exiting as a test ... \n");
                              ROSE_ASSERT(false);
                            }
#endif
                         staticCounter++;
                       }
                  }
                 else
                  {
#if 0
                    printf ("fileInfo->hasPositionInSource() == false \n");
#endif
                  }
             }
        }

     return returnAttribute;
   }

MarkTemplateInstantiationsForOutputSupportSynthesizedAttribute
MarkTemplateInstantiationsForOutputSupport::evaluateSynthesizedAttribute ( 
   SgNode* node,
   MarkTemplateInstantiationsForOutputSupportInheritedAttribute   inheritedAttribute,
   SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     MarkTemplateInstantiationsForOutputSupportSynthesizedAttribute returnAttribute;

  // ROSE_ASSERT(inheritedAttribute.insideDeclarationToOutput == true);
#if 0
     if (inheritedAttribute.insideDeclarationToOutput == false)
        {
          printf ("#");
        }
#endif
     if (inheritedAttribute.insideDeclarationToOutput == true)
        {
#if 0
          printf ("In MarkTemplateInstantiationsForOutputSupport::evaluateSynthesizedAttribute(): node = %p = %s = %s process = %s \n",
                  node,node->class_name().c_str(),SageInterface::get_name(node).c_str(),
               (inheritedAttribute.insideDeclarationToOutput == true) ? "true" : "false");
#endif

          switch(node->variantT())
             {
               case V_SgMemberFunctionRefExp:
                  {
                    SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(node);
                 // Mark the file info object as being compiler generated instead of part of a transformation. If
                 // it were part of a transformation the the unparser would be forced to output the associated code.
                    SgMemberFunctionSymbol* memberFunctionSymbol = memberFunctionRefExp->get_symbol();
                    ROSE_ASSERT(memberFunctionSymbol != NULL);
                    SgMemberFunctionDeclaration* inputMemberFunctionDeclaration = memberFunctionSymbol->get_declaration();
                    ROSE_ASSERT(inputMemberFunctionDeclaration != NULL);

                 // printf ("inputMemberFunctionDeclaration = %p = %s symbol = %p \n",inputMemberFunctionDeclaration,inputMemberFunctionDeclaration->get_name().str(),memberFunctionSymbol);
                    if (isSgTemplateInstantiationMemberFunctionDecl(inputMemberFunctionDeclaration) != NULL)
                         saveDeclaration(inputMemberFunctionDeclaration);
                    break;
                  }

               case V_SgFunctionRefExp:
                  {
                    SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(node);
                 // Mark the file info object as being compiler generated instead of part of a transformation. If
                 // it were part of a transformation the the unparser would be forced to output the associated code.
                    SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
                    ROSE_ASSERT(functionSymbol != NULL);
                    SgFunctionDeclaration* inputFunctionDeclaration = functionSymbol->get_declaration();
                    ROSE_ASSERT(inputFunctionDeclaration != NULL);

                 // printf ("inputFunctionDeclaration = %p = %s \n",inputFunctionDeclaration,inputFunctionDeclaration->get_name().str());
                    if (isSgTemplateInstantiationFunctionDecl(inputFunctionDeclaration) != NULL)
                         saveDeclaration(inputFunctionDeclaration);
                    break;
                  }

#if 0
            // DQ (6/21/2005): Since we evaluate/traverse all SgInitializedName IR nodes we catch all variable 
            // declarations (so we don't need this case, it is redundant!).

            // DQ (6/20/2005): Now we also have to search for any place where a member function 
            // constructor or destructor could be hidden (such as in a variable declaration).
            // If we find a constructor call then we have to match it with the declartion of the 
            // destructor since it is called implicitly!  Where else can a constructor or 
            // destructor hide?  (cast expressions, constructor initializers?)
               case V_SgVariableDeclaration:
                  {
                    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(node);

                 // If this is for a class then identify the constructor and mark the constructor and the destructor for output!
#if 0
                    printf ("Looking for the constructor and and destructor for this variable declaration \n");
                    variableDeclaration->get_file_info()->display("debug");
#endif
                 // SubTreeSynthesizedAttributes::iterator i = synthesizedAttributeList.begin();
                    SubTreeSynthesizedAttributes::iterator i = synthesizedAttributeList.begin();
                    while ( i != synthesizedAttributeList.end() )
                       {
                      // iterator through the child attributes
                      // printf ("Looking for an implicit default constructor \n");

                      // Find the default destructor and add it to the list of functions to output!

                         SgDeclarationStatement* typeDeclaration = (*i)->get_declaration();
                         ROSE_ASSERT(typeDeclaration != NULL);
                         SgClassDeclaration* classDeclaration = isSgClassDeclaration( typeDeclaration );
                         if (classDeclaration != NULL)
                            {
                              SgMemberFunctionDeclaration destructor = SageInterface::getDefaultDestructor(classDeclaration);

                              if (isSgTemplateInstantiationMemberFunctionDecl(destructor) != NULL)
                                   saveDeclaration(destructor);
                            }

                      // increment through list of variables in declaration
                         i++;
                       }
#if 0
                    printf ("Exiting after evaluation at SgVariableDeclaration \n");
                    ROSE_ASSERT(false);
#endif
                    break;
                  }
#endif

#if 1
            // References to named types (typedefs, enums, and classes can be hidden in variable declarations, 
            // but we want to look specifically at the SgInitializedName objects (the variables) and look at 
            // there types individually since pointers or references to types might not count as a usage that 
            // triggers a template instantiation (or output of the instantiation in the code generation phase).
               case V_SgInitializedName:
                  {
                    SgInitializedName* initializedName = isSgInitializedName(node);
#if 0
                    printf ("initializedName->get_name() = %s \n",initializedName->get_name().str());
                    printf ("initializedName->get_declptr() = %p = %s \n",initializedName->get_declptr(),
                         (initializedName->get_declptr() != NULL) ? initializedName->get_declptr()->sage_class_name() : "NULL");
                    printf ("initializedName->get_declaration() = %p = %s \n",initializedName->get_declaration(),
                         (initializedName->get_declaration() != NULL) ? initializedName->get_declaration()->sage_class_name() : "NULL");
                    printf ("initializedName->get_definition() = %p = %s \n",initializedName->get_definition(),
                         (initializedName->get_definition() != NULL) ? initializedName->get_definition()->sage_class_name() : "NULL");
#endif

                    SgType*      type        = initializedName->get_type();
#if 1
                    SgType*      stripedType = type->stripType();
                    SgNamedType* namedType   = isSgNamedType(stripedType);
#else
                    SgNamedType* namedType = isSgNamedType(type);
#endif
                    if (namedType != NULL)
                       {
                      // printf ("Found a named type \n");
                         SgClassType* classType = isSgClassType(type);
                         if (classType != NULL)
                            {
                           // printf ("Found a named type, save the constructor for the declared class  \n");
                              SgDeclarationStatement* declaration = classType->get_declaration();
                              ROSE_ASSERT(declaration != NULL);
                              SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                              ROSE_ASSERT(classDeclaration != NULL);

                           // Record that the class was used (we don't check if it was used through a pointer or 
                           // reference or more directly).  Only when it is used directly does it really require 
                           // that we provide the forward declaration and the definition of the specialization.
                              if (isSgTemplateInstantiationDecl(classDeclaration) != NULL)
                                 {
#if 0
                                   printf ("initializedName->get_name() = %s \n",initializedName->get_name().str());
                                   initializedName->get_file_info()->display("calling saveDeclaration");
#endif
                                   saveDeclaration(classDeclaration);

                                // The constructor is available through the initializer (if it exists, I think)
                                // ROSE_ASSERT(initializedName->get_initptr() != NULL);
                                   SgInitializer* initializer = initializedName->get_initptr();
                                   if (initializer != NULL)
                                      {
                                        SgMemberFunctionDeclaration* constructor = NULL;
                                        switch(initializer->variantT())
                                           {
#if 0
                                          // DQ (8/21/2005): Commented out these cases since there should be 
                                          // templates hidden here (initailizers are already traversed).
                                             case V_SgAggregateInitializer:
                                                {
#if 0
                                               // I think there is nothing to do in this case!
                                                  printf ("SgAggregateInitializer not supported yet! \n");
                                                  SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(initializer);
                                                  constructor = aggregateInitializer->get_declaration();
                                                  printf ("Found a SgAggregateInitializer for a templaed class at: \n");
                                                  if (constructor != NULL)
                                                       constructor->get_file_info()->display("SgAggregateInitializer for a templaed class: debug");
                                                  ROSE_ASSERT(false);
#endif
                                                  break;
                                                }

                                             case V_SgAssignInitializer:
                                                {
#if 0
                                               // I think there is nothing to do in this case!
                                                  printf ("SgAssignInitializer not supported yet! \n");
                                                  SgAssignInitializer* assignmentInitializer = isSgAssignInitializer(initializer);
                                                  constructor = assignmentInitializer->get_declaration();
                                                  printf ("Found a SgAssignInitializer for a templaed class at: \n");
                                                  if (constructor != NULL)
                                                       constructor->get_file_info()->display("SgAssignInitializer for a templaed class: debug");
                                                  ROSE_ASSERT(false);
#endif
                                                  break;
                                                }
#endif
                                             case V_SgConstructorInitializer:
                                                {
                                               // printf ("SgConstructorInitializer has been found! \n");
                                                  SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(initializer);
                                                  constructor = constructorInitializer->get_declaration();

                                               // DQ (8/13/2005): KULL/src/utilities/Snapshot.cc demonstates this problem.
                                               // This need not be a valid point since the constructor might not explicitly be 
                                               // defined in the class or the SgConstructorInitializer may be used in a way such 
                                               // that EDG does not resolve the member function or even the class (if only the 
                                               // arugments are relavant).
                                               // ROSE_ASSERT(constructor != NULL);
                                                  break;
                                                }

                                             default:
                                                {
                                               // These are the SgAggregateInitializer and SgAssignInitializer cases

                                               // printf ("Error: default in switch! \n");
                                               // ROSE_ASSERT(false);
                                                }
                                           }

                                        if (constructor != NULL)
                                           {
                                          // Note that the constructor can be a member function (not a template).
#if PRINT_DEVELOPER_WARNINGS
                                             if (isSgTemplateInstantiationMemberFunctionDecl(constructor) == NULL)
                                                {
                                                  printf ("constructor is NOT a template (%s) \n",constructor->sage_class_name());
                                                }
#endif
                                          // ROSE_ASSERT(isSgTemplateInstantiationMemberFunctionDecl(constructor) != NULL);
                                             ROSE_ASSERT(isSgMemberFunctionDeclaration(constructor) != NULL);
                                             saveDeclaration(constructor);
                                           }
                                      }

#if 0
                                   if (initializedName->get_initptr() != NULL)
                                      {
                                     // Save the class's default constructor since it will be called implicitly (and MUST be declared if so)
                                        SgMemberFunctionDeclaration* constructor = SageInterface::getDefaultConstructor(classDeclaration);
                                        if (constructor != NULL)
                                           {
                                             ROSE_ASSERT(isSgTemplateInstantiationMemberFunctionDecl(constructor) != NULL);
                                             saveDeclaration(constructor);
                                           }
                                      }
                                     else
                                      {
                                     // If the initializer is preent then it would have been 
                                     // handled by the case V_SgConstructorInitializer (below)
                                        printf ("initializedName->get_initptr() = %p = %s \n",initializedName->get_initptr(),initializedName->get_initptr()->sage_class_name());
                                      }
#endif
                                // Save the class's destructor since it will be called implicitly (and MUST be declared if so)
                                   SgMemberFunctionDeclaration* destructor = SageInterface::getDefaultDestructor(classDeclaration);
                                   if (destructor != NULL)
                                      {
#if 0
                                        if (isSgTemplateInstantiationMemberFunctionDecl(destructor) == NULL)
                                           {
                                             printf ("ERROR: destructor = %p = %s \n",destructor,destructor->class_name().c_str());
                                             destructor->get_file_info()->display("Error: isSgTemplateInstantiationMemberFunctionDecl(destructor) == NULL: debug");
                                           }
#endif
                                     // DQ (3/2/2019): We are trying to refine what is a template instantiation and what is a normal function.
                                     // I think the example that is an issue here is in facat a normal function (see Cxx11_tests/test2014_04.C).
                                     // ROSE_ASSERT(isSgTemplateInstantiationMemberFunctionDecl(destructor) != NULL);

                                        saveDeclaration(destructor);
                                      }
                                 }
                            }
                           else
                            {
                           // This might be a typedef (not implemented), or a enum not required to be supported
                           // printf ("Not a SgClassType: namedType = %p = %s \n",namedType,namedType->sage_class_name());
                              ROSE_ASSERT(isSgTypedefType(namedType) == NULL);
                            }
                       }
                    break;
                  }
#endif
#if 0
               case V_SgConstructorInitializer:
                  {
                    SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(node);
                    ROSE_ASSERT(constructorInitializer != NULL);

                    SgDeclarationStatement* constructor = constructorInitializer->get_declaration();
                    ROSE_ASSERT(constructor != NULL);

                 // printf ("Found a constructor initializer \n");

                    if (isSgTemplateInstantiationMemberFunctionDecl(constructor) != NULL)
                         saveDeclaration(constructor);
                    break;
                  }
#endif
#if 0
            // DQ (6/21/2005): Take care of the use of class declarations hidden in base class specifications!
               case V_SgClassDefinition:
                  {
                    SgClassDefinition* classDefinition = isSgClassDefinition(node);
                    ROSE_ASSERT(classDefinition != NULL);

                    SgBaseClassPtrList::iterator i = classDefinition->get_inheritances().begin();
                    while ( i != classDefinition->get_inheritances().end() )
                       {
                      // Check the parent pointer to make sure it is properly set
                         ROSE_ASSERT( (*i)->get_parent() != NULL);
                         ROSE_ASSERT( (*i)->get_parent() == classDefinition);

                      // Calling resetTemplateName()
                         SgClassDeclaration* baseClassDeclaration = (*i)->get_base_class();
                         ROSE_ASSERT(baseClassDeclaration != NULL);

                         if (isSgTemplateInstantiationDecl(baseClassDeclaration) != NULL)
                              saveDeclaration(baseClassDeclaration);

                      // iterate through the base classes
                         i++;
                       }
                    break;
                  }
#endif
               default:
                  {
                 // Nothing to do here!
                  }
             }
        }

     return returnAttribute;
   }

