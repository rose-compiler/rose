#ifndef ROSE_AST_UNPARSE_ATTRIBUTE_HEADER
#define ROSE_AST_UNPARSE_ATTRIBUTE_HEADER

class AstUnparseAttribute : public AstAttribute
   {
  // DQ (7/20/2008): Added support for arbitrary text to be unparsed at each SgLocatedNode.

  // This class is supports a mechanism to have text be output within the unparsing phase.
  // This mechanism is to be used for code generation of text that is not proper code.
  // It is not intended to be a mechanism to do significant code generation (or be a crutch for transformations).
  // however it is particularly useful for backend specific code generation such as:
  //    1) backend compiler directives (which by definition are not a part of the language grammar)
  //    2) tool specific annotations
  //    3)
  //
  // It does not replace the comment or CPP directive mechanism, but does permit it to be more precise
  // or user defined (if required in rare cases).  In many cases either that mechanism or this mechanism 
  // could be used to accomplish the same goal.  It is prefered that the comment or CPP directive mechanism
  // be used and that this mechanism be used only as a backup. It is expected that the user will select the 
  // appropriate mechanism in each case.

  // Note: CPP directives will soon be formal IR nodes, but that will only handle the common case of where
  // they are used, not the more general problem (corner cases).

     public:

     //! Static data used as name in AstAttribute mechanism.
         static const std::string markerName;

       // DQ (7/19/2008): I think we should not reuse the PreprocessingInfo::RelativePositionType
       // since it does not make sense to "replace" an IR node with a comment or CPP directive, I think.
       // typedef PreprocessingInfo::RelativePositionType RelativeLocation;
          enum RelativePositionType
             {
               e_defaultValue = 0,  // let the zero value be an error value
               e_undef        = 1,  // Position of the directive is only going to be defined
                                    // when the preprocessing object is copied into the AST,
                                    // it remains undefined before that
               e_before       = 2,  // Directive goes before the correponding code segment
               e_after        = 3,  // Directive goes after the correponding code segment
               e_inside       = 4,  // Directive goes inside the correponding code segment (as in between "{" and "}" of an empty basic block)

            // DQ (7/19/2008): Added additional fields so that we could use this enum type in the AstUnparseAttribute
               e_replace       = 5, // Support for replacing the IR node in the unparsing of any associated subtree
               e_before_syntax = 6, // We still have to specify the syntax
               e_after_syntax  = 7, // We still have to specify the syntax

            // DQ (10/4/2020): Added another kind of location.
            // Sometimes the added text will reference declarations from included files, 
            // in this case the text from be located after a statements associated CPP 
            // directives (specifically the include directives).
               e_before_but_after_cpp_directives_and_comments  = 8
             };

       // RelativePositionType location;
       // std::string unparseReplacement;

          typedef std::pair<std::string,RelativePositionType> StringPositionPairType;
          typedef std::vector< StringPositionPairType > StringPositionPairListType;
          StringPositionPairListType stringList;

      //! Constructor
          AstUnparseAttribute( std::string s, RelativePositionType inputlocation );

      //! Desstructor
          virtual ~AstUnparseAttribute();

       //! Adds string at location relative to IR node
          void addString(std::string s, RelativePositionType inputlocation );

       //! This reports if there exists a replacement string for the IR nodes where the AstUnparseAttribute is attached.
          bool replacementStringExists();

       //! This will output all the strings (mostly for debugging).
          virtual std::string toString() ROSE_OVERRIDE;

       //! This is selective in putting out only the strings that match a specific location.
          virtual std::string toString( RelativePositionType inputlocation );

          //! required for ast attribute mechanism
          virtual AstAttribute::OwnershipPolicy getOwnershipPolicy() const ROSE_OVERRIDE;

   };

// This is the "endif" for ROSE_AST_UNPARSE_ATTRIBUTE_HEADER include guard
#endif
