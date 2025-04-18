#include "sage3basic.h"

#include <Rose/Diagnostics.h>
#include "fixupDeclarationScope.h"

using namespace Rose::Diagnostics;
Sawyer::Message::Facility FixupAstDeclarationScope::mlog;

void fixupAstDeclarationScope(SgNode* /*node*/)
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
     astFixupTraversal.traverseMemoryPool();

  // Now process the map of sets of declarations.
     std::map<SgDeclarationStatement*,std::set<SgDeclarationStatement*>* > & mapOfSets = astFixupTraversal.mapOfSets;
     std::map<SgDeclarationStatement*,std::set<SgDeclarationStatement*>* >::iterator i = mapOfSets.begin();
     while (i != mapOfSets.end())
        {
          SgDeclarationStatement* firstNondefiningDeclaration = i->first;
          ASSERT_not_null(firstNondefiningDeclaration);
          ASSERT_not_null(firstNondefiningDeclaration->get_firstNondefiningDeclaration());

          if (firstNondefiningDeclaration != firstNondefiningDeclaration->get_firstNondefiningDeclaration())
             {
               printf ("WARNING: In fixupAstDeclarationScope(): firstNondefiningDeclaration != firstNondefiningDeclaration->get_firstNondefiningDeclaration() \n");
               printf ("   --- firstNondefiningDeclaration = %p = %s \n",
                    firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str());
               printf ("   --- firstNondefiningDeclaration->get_firstNondefiningDeclaration() = %p = %s \n",
                    firstNondefiningDeclaration->get_firstNondefiningDeclaration(),firstNondefiningDeclaration->get_firstNondefiningDeclaration()->class_name().c_str());
             }

          SgScopeStatement* correctScope = firstNondefiningDeclaration->get_scope();

          if (correctScope == nullptr)
             {
               printf ("Error: In fixupAstDeclarationScope(): correctScope == NULL: firstNondefiningDeclaration = %p = %s name = %s \n",
                    firstNondefiningDeclaration,firstNondefiningDeclaration->class_name().c_str(),SageInterface::get_name(firstNondefiningDeclaration).c_str());
               printf (" --- firstNondefiningDeclaration->hasExplicitScope() = %s \n",firstNondefiningDeclaration->hasExplicitScope() ? "true" : "false");
               printf (" --- firstNondefiningDeclaration->get_parent() = %p \n",firstNondefiningDeclaration->get_parent());
               if (firstNondefiningDeclaration->get_parent() != nullptr)
                  {
                    printf ("--- non-null: firstNondefiningDeclaration->get_parent() = %s name = %s \n",
                         firstNondefiningDeclaration->get_parent()->class_name().c_str(),SageInterface::get_name(firstNondefiningDeclaration->get_parent()).c_str());
                  }
             }
          ASSERT_not_null(correctScope);

          std::set<SgDeclarationStatement*>* declarationSet = i->second;
          ASSERT_not_null(declarationSet);

          std::set<SgDeclarationStatement*>::iterator j = declarationSet->begin();
          while (j != declarationSet->end())
             {
               SgScopeStatement* associatedScope = (*j)->get_scope();
               ASSERT_not_null(associatedScope);

            // DQ (6/11/2013): This is triggered by namespace definition scopes that are different 
            // due to re-entrant namespace declarations.  We should maybe fix this.
            // TV (7/22/13): This is also triggered when for global scope accross files.
               if (associatedScope != correctScope)
                  {
                    if (SgProject::get_verbose() > 0)
                       {
                         mprintf ("WARNING: This is the wrong scope (declaration = %p = %s): associatedScope = %p = %s correctScope = %p = %s \n",
                              *j,(*j)->class_name().c_str(),associatedScope,associatedScope->class_name().c_str(),correctScope,correctScope->class_name().c_str());
                       }
                  }

               j++;
             }

          i++;
        }
   }

void FixupAstDeclarationScope::initDiagnostics() 
   {
     static bool initialized = false;
     if (!initialized) 
        {
          initialized = true;
          Rose::Diagnostics::initAndRegister(&mlog, "Rose::FixupAstDeclarationScope");
          mlog.comment("normalizing AST declarations");
        }
   }

void
FixupAstDeclarationScope::visit ( SgNode* node )
   {
  // DQ (6/11/2013): This corrects where EDG can set the scope of a friend declaration to be different from the defining declaration.
  // We need it to be a rule in ROSE that the scope of the declarations are consistant between defining and all non-defining declaration).
     SgDeclarationStatement* declaration = isSgDeclarationStatement(node);
     if (declaration != nullptr)
        {
          SgDeclarationStatement* firstNondefiningDeclaration = declaration->get_firstNondefiningDeclaration();

       // Note that these declarations don't follow the same rules (namely the get_firstNondefiningDeclaration() can be NULL).
          if ( isSgFunctionParameterList(node)    ||
               isSgVariableDefinition(node)       ||
               isSgNonrealDecl(node)              ||
               isSgJovialCompoolStatement(node)   ||
               isSgJovialDirectiveStatement(node) ||
               isSgJovialDefineDeclaration(node)
             ) {
             }
            else
             {
            // DQ (6/15/2013): The older tutorial examples demonstrate addition of new functions using older rules that allows 
            // there to not be a non-defining declaration.  We need to remove these tutrial example in favor of the AST builder
            // API to build functions that will follow the newer AST constistancy rules.  Until we do this work in the tutorial
            // we can't inforce this below else the older tutorial examples (e.g. addFunctionDeclaration.C) will fail.  So I will
            // allow this for now and output a warning when (firstNondefiningDeclaration == NULL).
               if (firstNondefiningDeclaration == nullptr)
                  {
                    mprintf ("WARNING: In FixupAstDeclarationScope::visit(): firstNondefiningDeclaration == NULL for case of node = %p = %s (allowed for tutorial example transformations only) \n",node,node->class_name().c_str());
                  }
                 else
                  {
                    if (mapOfSets.find(firstNondefiningDeclaration) == mapOfSets.end())
                       {
                         std::set<SgDeclarationStatement*>* new_empty_set = new std::set<SgDeclarationStatement*>();
                         ASSERT_not_null(new_empty_set);
                         ASSERT_not_null(firstNondefiningDeclaration);

                         mapOfSets.insert(std::pair<SgDeclarationStatement*,std::set<SgDeclarationStatement*>*>(firstNondefiningDeclaration,new_empty_set));
                       }

                    ASSERT_require(mapOfSets.find(firstNondefiningDeclaration) != mapOfSets.end());
                    ASSERT_not_null(declaration);

                    mapOfSets[firstNondefiningDeclaration]->insert(declaration);
                  }
             }
        }
   }
