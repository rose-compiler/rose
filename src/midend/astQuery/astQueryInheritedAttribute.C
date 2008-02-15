
#include "rose.h"

#include "astQuery.h"
#include "astQueryInheritedAttribute.h"

AstQueryNamespace::AstQueryInheritedAttributeType::~AstQueryInheritedAttributeType ()
   {
   }

AstQueryNamespace::AstQueryInheritedAttributeType::AstQueryInheritedAttributeType ()
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
AstQueryInheritedAttributeType::AstQueryInheritedAttributeType
   ( const AstQueryInheritedAttributeType & X )
   {
     operator=(X);
   }

AstQueryInheritedAttributeType::operator= ( const AstQueryInheritedAttributeType & X )
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
AstQueryInheritedAttributeType::setMatchingName (const string & name)
{
  matchingName = name;
}

string AstQueryInheritedAttributeType::getMatchingName () const
{
  return matchingName;
}
#endif

/*
void
AstQueryInheritedAttributeType::setQuery ( const AstQuery::TypeOfQueryType inputQuery )
   {
     typeOfQuery = inputQuery;
   }

AstQuery::TypeOfQueryType
AstQueryInheritedAttributeType::getQuery() const
   {
     return typeOfQuery;
   }
*/
