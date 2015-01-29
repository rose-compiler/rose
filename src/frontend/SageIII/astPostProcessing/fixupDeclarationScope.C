#include "sage3basic.h"

#include "fixupDeclarationScope.h"


void fixupAstDeclarationScope( SgNode* node )
   {
  // This function was designed to fixup what I thought were inconsistancies in how the 
  // defining and some non-defining declarations associated with friend declarations had 
  // their scope set.  I now know this this was not a problem, but it is helpful to enforce the
  // consistancy.  It might also be useful to process declarations with scopes set to 
  // namespace definitions, so that the namespace definition can be normalized to be 
  // consistant across all of the different re-entrant namespace definitions.  This is 
  // possible within the new namespace support in ROSE.

     TimingPerformance timer ("Fixup declaration scopes:");

  // This simplifies how the traversal is called!
     FixupAstDeclarationScope astFixupTraversal;

  // DQ (1/29/2007): This traversal now uses the memory pool (so that we will visit declaration hidden in types (e.g. SgClassType)
  // SgClassType::traverseMemoryPoolNodes(v);
     astFixupTraversal.traverseMemoryPool();

  // Now process the map of sets of declarations.
     std::map<SgDeclarationStatement*,std::set<SgDeclarationStatement*>* > & mapOfSets = astFixupTraversal.mapOfSets;

#if 0
     printf ("In fixupAstDeclarationScope(): mapOfSets.size() = %" PRIuPTR " \n",mapOfSets.size());
#endif

     std::map<SgDeclarationStatement*,std::set<SgDeclarationStatement*>* >::iterator i = mapOfSets.begin();
     while (i != mapOfSets.end())
        {
          SgDeclarationStatement* firstNondefiningDeclaration = i->first;
          ROSE_ASSERT(firstNondefiningDeclaration == firstNondefiningDeclaration->get_firstNondefiningDeclaration());

          SgScopeStatement* correctScope = firstNondefiningDeclaration->get_scope();
          ROSE_ASSERT(correctScope != NULL);

#if 0
          printf ("In FixupAstDeclarationScope::visit(): node = %p = %s firstNondefiningDeclaration = %p correctScope = %p = %s \n",node,node->class_name().c_str(),firstNondefiningDeclaration,correctScope,correctScope->class_name().c_str());
#endif

          std::set<SgDeclarationStatement*>* declarationSet = i->second;
          ROSE_ASSERT(declarationSet != NULL);

#if 0
          printf ("In fixupAstDeclarationScope(): mapOfSets[%p]->size() = %" PRIuPTR " \n",firstNondefiningDeclaration,mapOfSets[firstNondefiningDeclaration]->size());
#endif

          std::set<SgDeclarationStatement*>::iterator j = declarationSet->begin();
          while (j != declarationSet->end())
             {
               SgScopeStatement* associatedScope = (*j)->get_scope();
               ROSE_ASSERT(associatedScope != NULL);

            // DQ (6/11/2013): This is triggered by namespace definition scopes that are different 
            // due to re-entrant namespace declarations.  We should maybe fix this.
            // TV (7/22/13): This is also triggered when for global scope accross files.
               if (associatedScope != correctScope)
                  {
                 // DQ (1/30/2014): Cleaning up some output spew.
                    if (SgProject::get_verbose() > 0)
                       {
                         printf ("WARNING: This is the wrong scope (declaration = %p = %s): associatedScope = %p = %s correctScope = %p = %s \n",
                              *j,(*j)->class_name().c_str(),associatedScope,associatedScope->class_name().c_str(),correctScope,correctScope->class_name().c_str());
                       }
#if 0
                    printf ("Make this an error for now! \n");
                    ROSE_ASSERT(false);
#endif
                  }

               j++;
             }

          i++;
        }

#if 0
     printf ("Leaving fixupAstDeclarationScope() node = %p = %s \n",node,node->class_name().c_str());
#endif
   }

void
FixupAstDeclarationScope::visit ( SgNode* node )
   {
  // DQ (6/11/2013): This corrects where EDG can set the scope of a friend declaration to be different from the defining declaration.
  // We need it to be a rule in ROSE that the scope of the declarations are consistant between defining and all non-defining declaration).

#if 0
     printf ("In FixupAstDeclarationScope::visit(node = %p = %s) \n",node,node->class_name().c_str());
#endif

     SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
     if (declaration != NULL)
        {
       // SgDeclarationStatement* definingDeclaration         = declaration->get_definingDeclaration();
          SgDeclarationStatement* firstNondefiningDeclaration = declaration->get_firstNondefiningDeclaration();

       // Note that these declarations don't follow the same rules (namely the get_firstNondefiningDeclaration() can be NULL).
          if ( isSgFunctionParameterList(node) != NULL || isSgVariableDefinition(node) != NULL)
             {
#if 0
               printf ("In FixupAstDeclarationScope::visit(): node = %p = %s firstNondefiningDeclaration = %p (skipping this processing) \n",node,node->class_name().c_str(),firstNondefiningDeclaration);
#endif
             }
            else
             {
            // DQ (6/15/2013): The older tutorial examples demonstrate addition of new functions using older rules that allows 
            // there to not be a non-defining declaration.  We need to remove these tutrial example in favor of the AST builder
            // API to build functions that will follow the newer AST constistancy rules.  Until we do this work in the tutorial
            // we can't inforce this below else the older tutorial examples (e.g. addFunctionDeclaration.C) will fail.  So I will
            // allow this for now and output a warning when (firstNondefiningDeclaration == NULL).
            // ROSE_ASSERT(firstNondefiningDeclaration != NULL);
               if (firstNondefiningDeclaration == NULL)
                  {
                    printf ("WARNING: In FixupAstDeclarationScope::visit(): firstNondefiningDeclaration == NULL for case of node = %p = %s (allowed for tutorial example transformations only) \n",node,node->class_name().c_str());
                  }
                 else
                  {
                    if (mapOfSets.find(firstNondefiningDeclaration) == mapOfSets.end())
                       {
                         std::set<SgDeclarationStatement*>* new_empty_set = new std::set<SgDeclarationStatement*>();
                         ROSE_ASSERT(new_empty_set != NULL);
                         mapOfSets.insert(std::pair<SgDeclarationStatement*,std::set<SgDeclarationStatement*>*>(firstNondefiningDeclaration,new_empty_set));
                       }

                     ROSE_ASSERT(mapOfSets.find(firstNondefiningDeclaration) != mapOfSets.end());

                     mapOfSets[firstNondefiningDeclaration]->insert(declaration);
                  }
             }
        }
   }

