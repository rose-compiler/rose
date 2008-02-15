#include "rose.h"

// include file for transformation specification support
// include "specification.h"
// include "globalTraverse.h"
// include "query.h"

// string class used if compiler does not contain a C++ string class
// #include <roseString.h>

#include "booleanQuery.h"
#include "booleanQueryInheritedAttribute.h"

BooleanQueryInheritedAttributeType::~BooleanQueryInheritedAttributeType()
   {
  // Nothing to do here
     typeOfQuery = BooleanQuery::UnknownListElementType;
   }

BooleanQueryInheritedAttributeType::BooleanQueryInheritedAttributeType()
   {
     typeOfQuery = BooleanQuery::UnknownListElementType;
   }

BooleanQueryInheritedAttributeType::BooleanQueryInheritedAttributeType
   ( const BooleanQueryInheritedAttributeType & X )
   {
     typeOfQuery  = X.typeOfQuery;
   }

BooleanQueryInheritedAttributeType &
BooleanQueryInheritedAttributeType::operator= ( const BooleanQueryInheritedAttributeType & X )
   {
     typeOfQuery  = X.typeOfQuery;

     return *this;
   }

#if 0
void
BooleanQueryInheritedAttributeType::setMatchingName ( const string & name )
   {
     matchingName = name;
   }

string
BooleanQueryInheritedAttributeType::getMatchingName() const
   {
     return matchingName;
   }
#endif

void
BooleanQueryInheritedAttributeType::setQuery ( const BooleanQuery::TypeOfQueryType inputQuery )
   {
     typeOfQuery = inputQuery;
   }

BooleanQuery::TypeOfQueryType
BooleanQueryInheritedAttributeType::getQuery() const
   {
     return typeOfQuery;
   }














