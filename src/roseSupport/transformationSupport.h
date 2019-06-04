#ifndef ROSE_TRANSFORMATION_SUPPORT
#define ROSE_TRANSFORMATION_SUPPORT

// #include "transformationOptions.h"

#include "optionDeclaration.h"

// Access to Query Libraries
#include "roseQueryLib.h"

// include "nodeQuery.h"
// include "nameQuery.h"
// include "numberQuery.h"

 /*! \brief This class simplifies the development of queries on the AST resulting in a
            list of AST nodes.
     \ingroup transformationSupportLib

     This class holds functions that are generally useful for a broad cross-section of
     transformations.  Functionality specific to any specific transformation should be represented in
     a similar transformation support class that is a part of the preprocessor source code
     representing that specific transformation.
 */
class ROSE_DLL_API TransformationSupport
   {
     public:

      /*! \if documentDevelopmentVersionUsingDoxygen
               \brief Enum values required to classify all possible overloaded C++ operators.

               Enum value supports the conversion of overloaded function names --> operator codes used to
               distiguish different sorts of transformations for each catagory of overloaded function.
               \internal  We could not use the Cxx_GrammarVariants enum since it does
                          not contain a classification for operator() and operator[].
          \endif
      */
          enum operatorCodeType
             {
            //! normal function call (used as the default value)
               FUNCTION_CALL_OPERATOR_CODE = 0,

            // binary operators
               ADD_OPERATOR_CODE          = 1,
               SUBT_OPERATOR_CODE         = 2,
               MULT_OPERATOR_CODE         = 3,
               DIV_OPERATOR_CODE          = 4,
               INTEGER_DIV_OPERATOR_CODE  = 5,
               MOD_OPERATOR_CODE          = 6,
               AND_OPERATOR_CODE          = 7,
               OR_OPERATOR_CODE           = 8,
               BITXOR_OPERATOR_CODE       = 9,
               BITAND_OPERATOR_CODE       = 10,
               BITOR_OPERATOR_CODE        = 11,
               EQ_OPERATOR_CODE           = 12,
               LT_OPERATOR_CODE           = 13,
               GT_OPERATOR_CODE           = 14,
               NE_OPERATOR_CODE           = 15,
               LE_OPERATOR_CODE           = 16,
               GE_OPERATOR_CODE           = 17,
               ASSIGN_OPERATOR_CODE       = 18,
               PLUS_ASSIGN_OPERATOR_CODE  = 19,
               MINUS_ASSIGN_OPERATOR_CODE = 20,
               AND_ASSIGN_OPERATOR_CODE   = 21,
               IOR_ASSIGN_OPERATOR_CODE   = 22,
               MULT_ASSIGN_OPERATOR_CODE  = 23,
               DIV_ASSIGN_OPERATOR_CODE   = 24,
               MOD_ASSIGN_OPERATOR_CODE   = 25,
               XOR_ASSIGN_OPERATOR_CODE   = 26,

            // operator() and operator[]
               PARENTHESIS_OPERATOR_CODE  = 27,
               BRACKET_OPERATOR_CODE      = 28,

            // unary operators
               NOT_OPERATOR_CODE          = 29,
               DEREFERENCE_OPERATOR_CODE  = 30,
               ADDRESS_OPERATOR_CODE      = 31,
               LSHIFT_OPERATOR_CODE       = 32,
               RSHIFT_OPERATOR_CODE       = 33,
               LSHIFT_ASSIGN_OPERATOR_CODE      = 34,
               RSHIFT_ASSIGN_OPERATOR_CODE      = 35,
               PREFIX_PLUSPLUS_OPERATOR_CODE    = 36,
               POSTFIX_PLUSPLUS_OPERATOR_CODE   = 37,
               PREFIX_MINUSMINUS_OPERATOR_CODE  = 38,
               POSTFIX_MINUSMINUS_OPERATOR_CODE = 39,

            // End of list tag
               OPERATOR_CODE_LAST_TAG = 99
             };

       /*! \brief generates a source code string from operator name.

           This function returns a string representing the elementwise operator (for primative types)
           that would be match that associated with the overloaded operator for a user-defined
           abstractions (e.g. identifyOperator("operator+()") returns "+").
        */
          static std::string stringifyOperator (std::string name);

       /*! \brief Classifies operator and returns an operatorCodeType value 
                  (useful for switch statements).

           This function is useful in switch statements to further refine the type of
           function based on the function name, number of parameters, and if it is a 
           prefix or postfix operator.  These are the minimum requirements to classify 
           all possible C++ overloaded functions (though usually the name is sufficient 
           so the additional parameters are optional).
        */
          static operatorCodeType classifyOverloadedOperator (
               std::string name,
               int numberOfParameters = 0,
               bool prefixOperator    = false);

      /*! \brief Return string representing operator node in AST.

          This function returns a string representing the operator associated with the SgNode*
          (assuming it is an SgExpression*).  It calls the identifyOperator() function to convert
          overloaded functions to strings.
       */
          static std::string buildOperatorString ( SgNode* astNode );

      //! Get the string representing the function name (should be added to Sage III interface).
          static std::string getFunctionName ( SgFunctionCallExp* functionCallExp );

      //! Get the string representing the type name (should be added to Sage III interface).
          static std::string getTypeName ( SgType* type );

      /*! \brief Similar to getFunctionName(SgNode* astNode) above, but for member functions.

          For member fuinctions this returns the class name of the class for which this functon is a
          member and for non-member functions this function returns the name of the class associated
          with the return type (this might be done better).
       */
          static std::string getFunctionTypeName ( SgFunctionCallExp* functionCallExpression );

      /*! \brief wrap any string representing a macro into something that the unparser will unparse as a macro definition

          This mechanism is a hack for now since it embeds a substring into a 
          comment which the unparser presently will recognize.  This needs to 
          be fixed to work more cleanly.
       */
          static std::string buildMacro ( std::string s );

      /*! @{
          \name Applications Specific Transformation Option Mechanism
          \brief Supporting functions for reading transformation specifications (hints)

          Preprocessors can optionally retrive hints embedded within the user's application.
          The mechanism is useful to provide additional information used to make more 
          sophisticated optimizations than the knowledge of the semantics of abstractions 
          might provide.  It also is a way to pass on information as a substitution for 
          program analysis.
       */

      /*! \brief This interface permits a more general specification of options 
                 using strings to identify the options and values to associate 
                 with the identified option. \sa OptionDeclaration

          This function is the only interface to the general mechanism that permits passing data from the user's application to a preprocessor.  If this mechanism is used, the user's application is optionally annotated with declarations (which have no side-effect to the execution of the user's application) but which can be read and interpreted by a preprocessor.  This function makes ...
       */
          static void getTransformationOptions (
                    SgNode* astNode,
                    std::list<OptionDeclaration> & variableNameList,
                    std::string identifingTypeName );

      /*! \if documentDevelopmentVersionUsingDoxygen
               \brief Get the list\<int\> of options (if any are specified).
               \deprecated This function is superceeded by the more general
                           form:
                    getTransformationOptions (SgNode* astNode,list<OptionDeclaration>&variableNameList,string identifingTypeName )
          \endif
       */
          static void getTransformationOptions (
                    SgNode* astNode,
                    std::list<int> & variableNameList,
                    std::string identifingTypeName );

      /*! \if documentDevelopmentVersionUsingDoxygen
               \brief Function used internally (for list\<int\> interface)
          \endif
       */
          static void getTransformationOptionsFromVariableDeclarationConstructorArguments (
                    SgVariableDeclaration* variableDeclaration,
                    std::list<int> & returnList );

      /*! \if documentDevelopmentVersionUsingDoxygen
               \brief Function used internally (used with list\<string\> version of interface)
          \endif
       */
          static void getTransformationOptionsFromVariableDeclarationConstructorArguments (
                    SgVariableDeclaration* variableDeclaration,
                    std::list<OptionDeclaration> & returnList );
      /*! @} */

      /*! \brief Builds variable declarations for all variable and types defined in subtree at astNode.

          New variables and types introduced by way of a transformation need to be 
          defined in the application before they are used.  This sort of transofrmation 
          can be generally automated. 

          \internal There may be some cases where this mechanism can be outsmarted :-).
       */
          static std::string internalSupportingGlobalDeclarations( SgNode* astNode, std::string prefixString );

       // DQ (9/26/03) Added here to eliminate ProjectQueryLibrary 
       // (which as mostly an experiment with the reverse traversal)
          static SgProject* getProject( const SgNode* astNode);
          static SgDirectory* getDirectory( const SgNode* astNode);
          static SgFile* getFile( const SgNode* astNode);
          static SgSourceFile* getSourceFile( const SgNode* astNode);
          static SgBinaryComposite* getBinaryFile( const SgNode* astNode);
          static SgGlobal* getGlobalScope( const SgNode* astNode);
          static SgStatement* getStatement(const SgNode* astNode);
          static SgFunctionDeclaration* getFunctionDeclaration( const SgNode* astNode);
          static SgFunctionDefinition* getFunctionDefinition( const SgNode* astNode);
          static SgClassDefinition* getClassDefinition( const SgNode* astNode);
          static SgModuleStatement* getModuleStatement( const SgNode* astNode);

       // DQ (7/25/2012): Modified to reflect new template design using different types of template IR nodes.
       // static SgTemplateDeclaration* getTemplateDeclaration( const SgNode* astNode);
          static SgDeclarationStatement* getTemplateDeclaration( const SgNode* astNode);

       // DQ (8/19/2014): Iterate back through the parents and scopes to find the SgType that the current node is embedded into.
      /*! \brief Find the associated type that might be a parent (through a parent chain) of the input SgNode.

          \internal Returns NULL if a SgStatement, SgFile, or SgProject is found (SgSymbol should never be found).
       */
          static SgType* getAssociatedType( const SgNode* astNode );

       // DQ (5/11/2011): This function was moved to SgTemplateArgument a long time ago and is not longer available.
       // DQ (6/9/2007): This function traverses through the parents to the first scope (used for name qualification support of template arguments)
       // static SgScopeStatement* getScope(const SgNode* astNode);

       // static SgValueExp* getValueExpr(const SgNode* astNode);
   };

// endif for ROSE_TRANSFORMATION_SUPPORT
#endif




















