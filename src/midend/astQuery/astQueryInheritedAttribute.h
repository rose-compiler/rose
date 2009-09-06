#ifndef ROSE_AST_QUERY_INHERITED_ATTRIBUTE_H
#define ROSE_AST_QUERY_INHERITED_ATTRIBUTE_H

/*! \if documentDevelopmentVersionUsingDoxygen
  \brief Inherited attribute for AstQuery class.
  \ingroup nodeQueryLib

  This class defines the inherited attribute used within the traversal associated 
  with the AstQuery library.
  \endif
 */
namespace AstQueryNamespace{

  class AstQueryInheritedAttributeType
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
      ~AstQueryInheritedAttributeType ();
      //! Default constructor
      AstQueryInheritedAttributeType ();
      //! The useful constructor
      //AstQueryInheritedAttributeType( AstQuery::TypeOfQueryType X );
      //! The useful constructor
      //AstQueryInheritedAttributeType ( const AstQueryInheritedAttributeType & X );
      //! Assignment operator
      //AstQueryInheritedAttributeType & operator= ( const AstQueryInheritedAttributeType & X );
      //! @}

      // void setMatchingName ( const string & name );
      // string getMatchingName() const;

      //@{
      /** Access functions for typeOfQuery member data */
      /*
         void setQuery ( const AstQuery::TypeOfQueryType X );
         AstQuery::TypeOfQueryType getQuery () const;
       */
      //@}

      //@{
      /** Access function for depthOfQuery date member */

      //@}
  };

}//END NAMESPACE
// endif for  ROSE_NODE_QUERY_INHERITED_ATTRIBUTE_H
#endif
