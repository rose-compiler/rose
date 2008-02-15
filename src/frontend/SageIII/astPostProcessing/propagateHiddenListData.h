
#ifndef PROPAGATE_HIDDEN_LIST_DATA_H
#define PROPAGATE_HIDDEN_LIST_DATA_H

// DQ (6/26/2005):
/*! \brief Propagate hidden list information for variables, types, and class elaboration.

    This data is used in the unparser to support name qualification.  The names that will 
    require name qualification or type elaboration are computed and propogated to lists
    of associated symbols in each child scope so that the unparser can easily chekc if 
    name qualification is required.  The symbol of a declaration whose name hides that 
    of an outer name is placed into the list so that the unparse is as simple as possible.

    I am unclear if this is required.

    \implementation Work done with Robert Preissl
 */
void propagateHiddenListData (SgNode* node);

//! Inherited attribute required for hidden list propagation to child scopes.
class PropagateHiddenListDataInheritedAttribute
   {
     public:

      //! Default constructor
          PropagateHiddenListDataInheritedAttribute() {}

      //! Store sets of symbols associated with names that are hidden in outer scopes.
          std::set<SgSymbol*> inherited_type_elaboration_list;
          std::set<SgSymbol*> inherited_hidden_type_list;
          std::set<SgSymbol*> inherited_hidden_declaration_list;
   };

class PropagateHiddenListData : public SgTopDownProcessing<PropagateHiddenListDataInheritedAttribute>
   {
     public:
      //! Required traversal function
          PropagateHiddenListDataInheritedAttribute evaluateInheritedAttribute(SgNode* node, PropagateHiddenListDataInheritedAttribute inheritedAttribute);
   };

// endif for PROPAGATE_HIDDEN_LIST_DATA_H
#endif
