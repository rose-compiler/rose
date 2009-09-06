#include "rose.h"

// include file for transformation specification support
// include "specification.h"
// include "globalTraverse.h"
// include "query.h"


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

