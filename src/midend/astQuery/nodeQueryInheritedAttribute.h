#ifndef ROSE_NODE_QUERY_INHERITED_ATTRIBUTE_H
#define ROSE_NODE_QUERY_INHERITED_ATTRIBUTE_H

/*! \if documentDevelopmentVersionUsingDoxygen
  \brief Inherited attribute for NodeQuery class.
  \ingroup nodeQueryLib

  This class defines the inherited attribute used within the traversal associated 
  with the NodeQuery library.
  \endif
 */
class NodeQueryInheritedAttributeType
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
    ~NodeQueryInheritedAttributeType ();
    //! Default constructor
    NodeQueryInheritedAttributeType ();
    //! The useful constructor
    //NodeQueryInheritedAttributeType( NodeQuery::TypeOfQueryType X );
    //! The useful constructor
    //NodeQueryInheritedAttributeType ( const NodeQueryInheritedAttributeType & X );
    //! Assignment operator
    //NodeQueryInheritedAttributeType & operator= ( const NodeQueryInheritedAttributeType & X );
    //! @}

    // void setMatchingName ( const string & name );
    // string getMatchingName() const;

    //@{
    /** Access functions for typeOfQuery member data */
    /*
       void setQuery ( const NodeQuery::TypeOfQueryType X );
       NodeQuery::TypeOfQueryType getQuery () const;
     */
    //@}

    //@{
    /** Access function for depthOfQuery date member */

    //@}
};

// endif for  ROSE_NODE_QUERY_INHERITED_ATTRIBUTE_H
#endif
