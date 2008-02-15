#ifndef SOURCE_LOCATION_INHERITED_ATTRIBUTE_H
#define SOURCE_LOCATION_INHERITED_ATTRIBUTE_H

// class InheritedAttributeBaseClassType
class SourceLocationInheritedAttribute
   {
  // We want to suggest that users use the inherited attributes to represent all source code
  // specific context. This is impossible to inforce (I think) but likely the representation of all
  // context can be represented in the inherited attribute and the code to support this can be
  // automatically generated.  The following data is required to represent context (there might be a
  // better way of doing this):

     public:
          SgProject*             project;
          SgFile*                file;
          SgGlobal*              globalScope;
          SgFunctionDefinition*  currentFunctionDefinitionScope;

       // Different sorts of scopes (could be any scope not just a SgBasicBlock)

       // This is some what redundent since the the funtion body is available from the
       // currentFunctionDefinition.
          SgBasicBlock*          currentFunctionBasicBlockScope;

       // These are useful when we want to introduce transformations of loops but force hoisting of
       // loop invariant code (which may not appear to be loop invariant to the compiler!).
          SgScopeStatement*      loopNestParentScope;
          SgScopeStatement*      conditionalParentScope;

       // Local scope can be either a SgBasicBlock, SgCatchOptionStmt, SgClassDefinition,
       // SgDoWhileStmt, SgForStatement, SgFunctionDefinition, SgGlobal, SgIfStmt,
       // SgSwitchStatement, and SgWhileStmt.
          SgScopeStatement*      localScope;

       // Record the current statement so that we can easily retrive it in a consistant fashion as
       // the other scope info.
          SgStatement* currentStatement;

       // Allow for later representation of expressions (though AST rewrite on expression level is
       // not implemented yet).
          SgExpression* currentExpression;

       // Save the current AST node
          SgNode* currentNode;

       // Use a vector instead of a stack so that we can iterate over the vector (since we don't do
       // any random insertion we don't need a list).
          std::vector<SgScopeStatement*> scopeList;

       // list of statements (can be more useful than just the list of scopes)
          std::vector<SgStatement*> statementList;

     private:
       // Define the default constructor as private so that it can't be used in any derived class
       // SourceLocationInheritedAttribute ();

     public:
       // Destructor
          virtual ~SourceLocationInheritedAttribute ();

       // Constructors (the standard stuff)
          SourceLocationInheritedAttribute ( SgNode* astNode );
          SourceLocationInheritedAttribute ( const SourceLocationInheritedAttribute & X );
          SourceLocationInheritedAttribute ( const SourceLocationInheritedAttribute & X, SgNode* astNode );

          SourceLocationInheritedAttribute & operator= ( const SourceLocationInheritedAttribute & X );

       // If we have to build one use this method instead of a default constructor
       // static SourceLocationInheritedAttribute initialInstance();

       // Set all pointers to NULL (initialization)
          void initialize();

       // Main function to define values for all class member data
          void refineClassification ( SgNode* astNode );

       // Debugging info
          void display( const std::string s ) const;

       // Access functions
          SgNode* getGlobalScope() const;
          SgNode* getCurrentFunctionScope() const;
          SgNode* getLocalScope() const;
          SgNode* getLoopNestParentScope() const;
          SgNode* getConditionalParentScope() const;
          SgNode* getParentScope() const;
          SgNode* getCurrentFunctionBasicBlockScope() const;

          SgNode* getCurrentDeclarationInGlobalScope() const;
          SgNode* getCurrentStatement() const;
          SgNode* getCurrentExpression() const;
          SgNode* getCurrentNode() const;

          SgNode* getCurrentStatementInScope( SgScopeStatement* targetScope ) const;

       // error checking (not sure this makes sense)
       // void assertValidPointers();
   };

// endif for SOURCE_LOCATION_INHERITED_ATTRIBUTE_H
#endif
