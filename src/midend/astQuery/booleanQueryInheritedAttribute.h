#ifndef ROSE_BOOLEAN_QUERY_INHERITED_ATTRIBUTE_H
#define ROSE_BOOLEAN_QUERY_INHERITED_ATTRIBUTE_H

class BooleanQueryInheritedAttributeType
   {
     public:
       // listElementReturnType elementReturnType;

          BooleanQuery::TypeOfQueryType typeOfQuery;
       // string matchingName;

       // Used for building data field names in structures (prepends structure name to data field
       // (e.g. structureName::nestedStructureName::fieldName). An alternative would be to handle
       // this query using a different query library which returned a list of lists of strings
       // (e.g. list< list<string> >). Then we could more readily handle function names!
       // string prefixName;

       // access functions
         ~BooleanQueryInheritedAttributeType();
          BooleanQueryInheritedAttributeType();
          BooleanQueryInheritedAttributeType( BooleanQuery::TypeOfQueryType X );
          BooleanQueryInheritedAttributeType ( const BooleanQueryInheritedAttributeType & X );
          BooleanQueryInheritedAttributeType & operator= ( const BooleanQueryInheritedAttributeType & X );

       // void setMatchingName ( const string & name );
       // string getMatchingName() const;

          void setQuery ( const BooleanQuery::TypeOfQueryType X );
          BooleanQuery::TypeOfQueryType getQuery () const;
   };

// endif for  ROSE_BOOLEAN_QUERY_INHERITED_ATTRIBUTE_H
#endif

