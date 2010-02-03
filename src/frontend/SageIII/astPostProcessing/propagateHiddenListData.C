// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "propagateHiddenListData.h"
// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (5/23/2007): It appears that the hidden type, declaration, and type elaboration lists 
// already propogate down the information from parent lists to all child lists.  If this is
// true then this step is not required and we don't need the code below (which is left 
// unimplemented for now).

PropagateHiddenListDataInheritedAttribute
PropagateHiddenListData::evaluateInheritedAttribute(SgNode* node, PropagateHiddenListDataInheritedAttribute inheritedAttribute)
   {
     ROSE_ASSERT(node != NULL);

#if 0
     printf ("##### PropagateHiddenListData::evaluateInheritedAttribute(node = %p = %s) \n",node,node->class_name().c_str());
#endif

     SgScopeStatement* scope = isSgScopeStatement(node);
     if (scope != NULL)
        {
          Sg_File_Info* fileInfo = scope->get_startOfConstruct();
          ROSE_ASSERT(fileInfo != NULL);
#if 0
          printf ("Scope = %p = %s file = %s line = %d \n",scope,scope->class_name().c_str(),fileInfo->get_filenameString().c_str(),fileInfo->get_line());
          printf ("   BEFORE: hidden list size (type_elaboration_list)        = %ld \n",scope->get_type_elaboration_list().size());
          printf ("   BEFORE: hidden list size (type_hidden_type_list)        = %ld \n",scope->get_hidden_type_list().size());
          printf ("   BEFORE: hidden list size (type_hidden_declaration_list) = %ld \n",scope->get_hidden_declaration_list().size());
#endif

          set<SgSymbol*> & type_elaboration_list   = scope->get_type_elaboration_list();
          set<SgSymbol*> & hidden_type_list        = scope->get_hidden_type_list();
          set<SgSymbol*> & hidden_declaration_list = scope->get_hidden_declaration_list();

       // There a at least two ways to do this, we want to make the local copy of the list in the
       // scope equal to the union of the list from the inherited attribute and the scopes original list.

       // Add elements to the list to push down to lower scopes
          inheritedAttribute.inherited_type_elaboration_list.insert(type_elaboration_list.begin(),type_elaboration_list.end());
          inheritedAttribute.inherited_hidden_type_list.insert(hidden_type_list.begin(),hidden_type_list.end());
          inheritedAttribute.inherited_hidden_declaration_list.insert(hidden_declaration_list.begin(),hidden_declaration_list.end());

       // Copy modified inherited attribute to new list in the current scope
          type_elaboration_list = inheritedAttribute.inherited_type_elaboration_list;
          hidden_type_list = inheritedAttribute.inherited_hidden_type_list;
          hidden_declaration_list = inheritedAttribute.inherited_hidden_declaration_list;

#if 0
          printf ("Scope = %p = %s file = %s line = %d \n",scope,scope->class_name().c_str(),fileInfo->get_filenameString().c_str(),fileInfo->get_line());
          printf ("   AFTER: hidden list size (type_elaboration_list)        = %ld \n",scope->get_type_elaboration_list().size());
          printf ("   AFTER: hidden list size (type_hidden_type_list)        = %ld \n",scope->get_hidden_type_list().size());
          printf ("   AFTER: hidden list size (type_hidden_declaration_list) = %ld \n",scope->get_hidden_declaration_list().size());
#endif
        }

     return inheritedAttribute;
   }


void
propagateHiddenListData (SgNode* node)
   {
  // DQ (3/10/2007): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Propagate Hidden List Data time (sec) = ");

  // printf ("Inside of propagateHiddenListData (node = %p = %s) \n",node,node->class_name().c_str());

  // DQ (6/5/2007): We actually need this now since the hidden lists are not pushed to lower scopes where they are required.
     PropagateHiddenListDataInheritedAttribute inheritedAttribute;
     PropagateHiddenListData temp_traversal;
     temp_traversal.traverse(node,inheritedAttribute);
   }

