#ifndef ROSE_NUMBER_QUERY_INHERITED_ATTRIBUTE_H
#define ROSE_NUMBER_QUERY_INHERITED_ATTRIBUTE_H

/*! \if documentDevelopmentVersionUsingDoxygen
  \brief Inherited attribute for NumberQuery class.
  \ingroup nodeQueryLib

  This class defines the inherited attribute used within the traversal associated 
  with the NumberQuery library.
  \endif
 */
class NumberQueryInheritedAttributeType
{
  public:


    // string matchingName;

    // Used for building data field names in structures (prepends structure name to data field
    // (e.g. structureName::nestedStructureName::fieldName). An alternative would be to handle
    // this query using a different query library which returned a list of lists of strings
    // (e.g. list< list<string> >). Then we could more readily handle function names!
    // string prefixName;

    //! @{
    /*! 
      \name  Constructors,Destructor,Assignment Operator, etc.
      \brief The usual class member functions.
     */
    //! Destructor
    ~NumberQueryInheritedAttributeType ();
    //! Default constructor
    NumberQueryInheritedAttributeType ();
    //! The useful constructor
    //NumberQueryInheritedAttributeType( NumberQuery::TypeOfQueryType X );
    //! The useful constructor
    //NumberQueryInheritedAttributeType ( const NumberQueryInheritedAttributeType & X );
    //! Assignment operator
    //NumberQueryInheritedAttributeType & operator= ( const NumberQueryInheritedAttributeType & X );
    //! @}


};

// endif for  ROSE_NUMBER_QUERY_INHERITED_ATTRIBUTE_H
#endif
