#include "rose.h"

#include "nodeQuery.h"
#include "nodeQueryInheritedAttribute.h"

NodeQueryInheritedAttributeType::~NodeQueryInheritedAttributeType ()
   {
   }

NodeQueryInheritedAttributeType::NodeQueryInheritedAttributeType ()
   {
#if 0
  // DQ (3/25/2004): Added to initialize data members
     queryFunctionOneParameter = NULL;
     queryFunctionTwoParameter = NULL;
     targetNode                = NULL;
     variant                   = V_SgNumVariants;
#endif
   }

/*
NodeQueryInheritedAttributeType::NodeQueryInheritedAttributeType
   ( const NodeQueryInheritedAttributeType & X )
   {
     operator=(X);
   }

NodeQueryInheritedAttributeType::operator= ( const NodeQueryInheritedAttributeType & X )
   {
     typeOfQuery  = X.typeOfQuery;
  // matchingName = X.matchingName;

     //     parentForChildrenOnlySearch = X.parentForChildrenOnlySearch;
     //     depthOfQuery                = X.depthOfQuery;

     return *this;
   }
*/
#if 0
void
NodeQueryInheritedAttributeType::setMatchingName (const string & name)
{
  matchingName = name;
}

string NodeQueryInheritedAttributeType::getMatchingName () const
{
  return matchingName;
}
#endif

/*
void
NodeQueryInheritedAttributeType::setQuery ( const NodeQuery::TypeOfQueryType inputQuery )
   {
     typeOfQuery = inputQuery;
   }

NodeQuery::TypeOfQueryType
NodeQueryInheritedAttributeType::getQuery() const
   {
     return typeOfQuery;
   }
*/
