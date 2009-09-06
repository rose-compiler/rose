#ifndef ROSE_NAME_QUERY_INHERITED_ATTRIBUTE_H
#define ROSE_NAME_QUERY_INHERITED_ATTRIBUTE_H

/*! \if documentDevelopmentVersionUsingDoxygen
  \brief Inherited attribute for NameQuery class.
  \ingroup nameQueryLib

  This class defines the inherited attribute used within the traversal associated 
  with the NameQuery library.
  \endif
 */
class NameQueryInheritedAttributeType
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
    ~NameQueryInheritedAttributeType ();
    //! Default constructor
    NameQueryInheritedAttributeType ();
    //! The useful constructor
    //NameQueryInheritedAttributeType( NameQuery::TypeOfQueryType X );
    //! The useful constructor
    //NameQueryInheritedAttributeType ( const NameQueryInheritedAttributeType & X );
    //! Assignment operator
    //NameQueryInheritedAttributeType & operator= ( const NameQueryInheritedAttributeType & X );
    //! @}

    // void setMatchingName ( const string & name );
    // string getMatchingName() const;

    //@{
    /** Access functions for typeOfQuery member data */
    /*
       void setQuery ( const NameQuery::TypeOfQueryType X );
       NameQuery::TypeOfQueryType getQuery () const;
     */
    //@}

    //@{
    /** Access function for depthOfQuery date member */

    //@}
};

// endif for  ROSE_NODE_QUERY_INHERITED_ATTRIBUTE_H
#endif
