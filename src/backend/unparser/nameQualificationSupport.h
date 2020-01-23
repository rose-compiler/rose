// DQ (5/4/2011): This is the redesign of the support for name qualification.
// The steps are:
// 1) All using directives put alias sysmbols into place into the scope 
//    where they appear and for each symbol in the scope being used.
// 1) Compute the hidden type tables
// 2) Use the hidden type tables to determine which types need qualification.

// * We will want to keep track of hidden variables too!  But first handle types.
// * When we refer to names it will be of either variables or types.

// This is a part of a rewrite of the name qualification support in ROSE with the follwoing properties:
//    1) It is exact (no over qualification).
//    2) It handled visibility of names constructs
//    3) It resolves ambiguity of named constructs.
//    4) It resolves where type elaboration is required.
//    5) The inputs are carried in the SgUnparse_Info object for uniform handling.
//    6) The the values in the SgUnparse_Info object are copied from the AST references to the named 
//       constructs to avoid where named constructs are referenced from multiple locations and the 
//       name qulification might be different.
//
//    7) What about base class qualification? I might have forgotten this one! No this is handled using standard rules (above).


// API function for new hidden list support.
void generateNameQualificationSupport( SgNode* node, std::set<SgNode*> & referencedNameSet );

class NameQualificationInheritedAttribute
   {
     private:
          SgScopeStatement* currentScope;

       // DQ (4/19/2019): Added support to include current statement (required for nested traversals 
       // of types to support name qualification for SgPointerMemberType).
          SgStatement* currentStatement;
          SgNode* referenceNode;

#if 0
       // DQ (2/8/2019): And then I woke up in the morning and had a better idea.

       // DQ (2/7/2019): Namen qaulification can under rare circumstances depende on the type.  And we need 
       // to pass the type through from the lhs to the rhs to get the name qualification correct on the rhs.
       // See Cxx11_tests/test2019_80.C and test2019_81.C for examples of this.
          SgPointerMemberType* usingPointerToMemberType;

          bool containsFunctionArgumentsOfPointerMemberType;
#endif

     public:

          NameQualificationInheritedAttribute();
          NameQualificationInheritedAttribute(const NameQualificationInheritedAttribute & X);

       // DQ (5/24/2013): Allow the current scope to be tracked from the traversal of the AST
       // instead of being computed at each IR node which is a problem for template arguments.
       // See test2013_187.C for an example of this.
          SgScopeStatement* get_currentScope();
          void set_currentScope(SgScopeStatement* scope);         

       // DQ (4/19/2019): Added support to include current statement (required for nested traversals 
       // of types to support name qualification for SgPointerMemberType).
          SgStatement* get_currentStatement();
          void set_currentStatement(SgStatement* statement);         
          SgNode* get_referenceNode();
          void set_referenceNode(SgNode* referenceNode);         

#if 0
       // DQ (2/8/2019): And then I woke up in the morning and had a better idea.

       // DQ (2/7/2019): Name qualification can under rare circumstances depends on the type.
          SgPointerMemberType* get_usingPointerToMemberType();
          void set_usingPointerToMemberType(SgPointerMemberType* type);

       // DQ (2/7/2019): Name qualification can under rare circumstances depends on the type.
          bool get_containsFunctionArgumentsOfPointerMemberType();
          void set_containsFunctionArgumentsOfPointerMemberType( bool x );
#endif
   };


class NameQualificationSynthesizedAttribute
   {
     public:
          NameQualificationSynthesizedAttribute();
          NameQualificationSynthesizedAttribute( const NameQualificationSynthesizedAttribute & X );
   };

class NameQualificationTraversal : public AstTopDownBottomUpProcessing<NameQualificationInheritedAttribute, NameQualificationSynthesizedAttribute>
   {
     private:
       // We might want to keep track of types that have been seen already.
       // Though, as a detail, all names in a class would be included as 
       // having been seen as we enter the scope of the class (not as they 
       // are traversed in the class).
       // * Keep a set of SgNodes that have been seen as declarations 
       //   (referenced names don't count if they don't specify a scope (location)).
       // * So we need a function to gather all the name in a class.

       // Data
       // DQ (6/21/2011): since this is used recursively we can't have this be a new set each time.
       // Build the local set to use to record when declaration that might required qualified references have been seen.
          std::set<SgNode*> & referencedNameSet;

       // We keep the qualified names as a map of strings with keys defined by the SgNode pointer values.
       // These are referenced to the static data members in SgNode (SgNode::get_globalQualifiedNameMapForNames() 
       // and SgNode::get_globalQualifiedNameMapForTypes()).  A later implementation could reference the versions
       // in SgNode directly. Initially in the design these were not references, they were built up and assigned 
       // to the static data members in SgNode, but this does not permit the proper handling of nexted types in 
       // templates since the unparser uses the SgNode static members directly.  so the switch to make this a 
       // reference fixes this problem.
          std::map<SgNode*,std::string> & qualifiedNameMapForNames;
          std::map<SgNode*,std::string> & qualifiedNameMapForTypes;

       // DQ (9/7/2014): Modified to handle template header map (for template declarations).
          std::map<SgNode*,std::string> & qualifiedNameMapForTemplateHeaders;

       // DQ (6/3/2011): This is to save the names of types where they can be named differently when referenced 
       // from different locations in the source code.
          std::map<SgNode*,std::string> & typeNameMap;

       // DQ (3/13/2019): Adding support for name qualification of the many parts of more complex types such as template types.
          std::map<SgNode*,std::map<SgNode*,std::string> > & qualifiedNameMapForMapsOfTypes;

       // DQ (1/24/2019): We need to accumulate the list of possible classes that are private base classes so 
       // that additional name qualification can be added to prevent the access of base classes that have been 
       // made private in nested chass hierarchies.
          typedef std::map<SgClassDeclaration*,std::set<SgClassDeclaration*> > BaseClassSetMap;

       // DQ (1/24/2019): This is the list of private base classes.
          BaseClassSetMap privateBaseClassSets;

       // DQ (1/24/2019): From the set of private base classes we construct the set of unaccessible base classes
          BaseClassSetMap inaccessibleClassSets;

       // DQ (7/22/2011): Alternatively we should treat array types just like templated types that can
       // contain subtypes that require arbitrarily complex name qualification for their different parts.
       // DQ (7/22/2011): We need to handle array types with dimensions that require qualification.
       // These are a vector of strings to support the name qualification of each indexed dimension 
       // of the array type.  The key is NOT the SgArrayType (since these are shared where the mangled
       // names match; mangled names use fully qualified names to avoid ambiguity).  Note that bit
       // field widths can require name qualification but are handled using the qualifiedNameMapForNames
       // map and the SgVarRefExp as the key to the map (so no special extra support is required).
       // std::map<SgNode*, std::vector<std::string> > & qualifiedNameMapForArrayTypes;

       // DQ (3/31/2014): I don't think this function is implemented anywhere in ROSE.
       // Member functions:
       // std::list<SgNode*> gatherNamesInClass( SgClassDefinition* classDefinition );

       // DQ (7/23/2011): This supports nested calls where the scope of a subtrees must have its scope explicitly 
       // specified. I think this only happens for the index in the SgArrayType.
          SgScopeStatement* explictlySpecifiedCurrentScope;

       // DQ (4/19/2019): Added support to include current statement (required for nested traversals 
       // of types to support name qualification for SgPointerMemberType).
          SgStatement* explictlySpecifiedCurrentStatement;

     public:
       // DQ (3/24/2016): Adding Robb's meageage mechanism (data member and function).
          static Sawyer::Message::Facility mlog;
          static void initDiagnostics();

     public:
       // DQ (4/3/2014): This map of sets is build once and then used to resolve when declarations have been
       // placed into scopes where they would permit name qualification (see test2014_32.C).
          SageInterface::DeclarationSets* declarationSet;

     public:
       // HiddenListTraversal();
       // HiddenListTraversal(SgNode* root);

       // DQ (9/7/2014): Modified to handle template header map (for template declarations).
          NameQualificationTraversal(std::map<SgNode*,std::string> & input_qualifiedNameMapForNames,
                                     std::map<SgNode*,std::string> & input_qualifiedNameMapForTypes,
                                     std::map<SgNode*,std::string> & input_qualifiedNameMapForTemplateHeaders,
                                     std::map<SgNode*,std::string> & input_typeNameMap,
                                     std::map<SgNode*,std::map<SgNode*,std::string> > & input_qualifiedNameMapForMapsOfTypes, 
                                     std::set<SgNode*> & input_referencedNameSet);

       // DQ (4/19/2019): When a type is the input we need the current statement as well, might want to require this uniformally.
       // DQ (7/23/2011): This permits recursive calls to the traversal AND specification of the current scope
       // used to support name qualification on expressions where we can't backout the current scope.  Used 
       // for name qualification of const expressions in SgArrayType index expressions.
       // void generateNestedTraversalWithExplicitScope( SgNode* node, SgScopeStatement* currentScope );
          void generateNestedTraversalWithExplicitScope( SgNode* node, SgScopeStatement* currentScope, SgStatement* currentStatement = NULL, SgNode* referenceNode = NULL );

       // Evaluates how much name qualification is required (typically 0 (no qualification), but sometimes 
       // the depth of the nesting of scopes plus 1 (full qualification with global scoping operator)).
       // int nameQualificationDepth ( SgClassDefinition* classDefinition );
          int nameQualificationDepth ( SgDeclarationStatement* declaration,     SgScopeStatement* currentScope, SgStatement* positionStatement, bool forceMoreNameQualification = false );
          int nameQualificationDepth ( SgInitializedName*      initializedName, SgScopeStatement* currentScope, SgStatement* positionStatement );
          int nameQualificationDepth ( SgType*                 type,            SgScopeStatement* currentScope, SgStatement* positionStatement );

          int nameQualificationDepthOfParent ( SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement );
       // int nameQualificationDepthForType  ( SgInitializedName* initializedName, SgStatement* positionStatement );
          int nameQualificationDepthForType  ( SgInitializedName* initializedName, SgScopeStatement* currentScope, SgStatement* positionStatement );

       // DQ (7/23/2011): Added support for array type index expressions.
          void processNameQualificationArrayType(SgArrayType* arrayType, SgScopeStatement* currentScope);
          void processNameQualificationForPossibleArrayType(SgType* possibleArrayType, SgScopeStatement* currentScope);

       // SgName associatedName(SgScopeStatement* scope);
          SgDeclarationStatement* associatedDeclaration ( SgScopeStatement* scope );
          SgDeclarationStatement* associatedDeclaration ( SgType* type );

       // These don't really need to be virtual, since we don't derive from this class.
          virtual NameQualificationInheritedAttribute evaluateInheritedAttribute(SgNode* n, NameQualificationInheritedAttribute inheritedAttribute);

          virtual NameQualificationSynthesizedAttribute evaluateSynthesizedAttribute(SgNode* n, NameQualificationInheritedAttribute inheritedAttribute, SynthesizedAttributesList synthesizedAttributeList);

       // Set the values in each reference to the name qualified language construct.
          void setNameQualification ( SgVarRefExp* varRefExp, SgVariableDeclaration* variableDeclaration,int amountOfNameQualificationRequired );

       // DQ (7/11/2014): This function is not implemented (not sure where it might have been).
       // DQ (6/5/2011): Added to support case where SgInitializedName in SgVarRefExp can't be traced back to a SgVariableDeclaration (see test2011_75.C).
       // void setNameQualification ( SgVarRefExp* varRefExp, SgScopeStatement* scopeStatement,int amountOfNameQualificationRequired );

          void setNameQualification ( SgBaseClass* baseClass, SgClassDeclaration*    classDeclaration,   int amountOfNameQualificationRequired );
          void setNameQualification ( SgUsingDeclarationStatement* usingDeclaration, SgInitializedName* associatedInitializedName, int amountOfNameQualificationRequired );
          void setNameQualification ( SgUsingDeclarationStatement* usingDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgUsingDirectiveStatement* usingDirective, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgFunctionRefExp* functionRefExp, SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgMemberFunctionRefExp* functionRefExp, SgMemberFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired );

       // DQ (1/18/2020): Adding name qualification support for SgPsuedoDestructorRefExp.
          void setNameQualification ( SgPseudoDestructorRefExp* psuedoDestructorRefExp, SgDeclarationStatement* declarationStatement, int amountOfNameQualificationRequired);

       // DQ (6/4/2011): This handles the case of both the declaration being a SgMemberFunctionDeclaration and a SgClassDeclaration.
       // void setNameQualification ( SgConstructorInitializer* constructorInitializer, SgMemberFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired);
          void setNameQualification ( SgConstructorInitializer* constructorInitializer, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired);

       // DQ (3/22/2018): Added support for name qualification of type output within C++11 specific support for initalization (see Cxx11_tests/test2018_47.C).
          void setNameQualification(SgAggregateInitializer* exp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired);

       // DQ (8/4/2012): Added support to permit global qualification be be skipped explicitly (see test2012_164.C and test2012_165.C for examples where this is important).
       // void setNameQualification ( SgInitializedName* initializedName, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
       // void setNameQualification ( SgInitializedName* initializedName, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired, bool skipGlobalQualification );
          void setNameQualificationOnType ( SgInitializedName* initializedName, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired, bool skipGlobalQualification );

       // DQ (12/17/2013): Added support for the name qualification of the SgInitializedName object when used in the context of the preinitialization list.
          void setNameQualificationOnName(SgInitializedName* initializedName,SgDeclarationStatement* declaration, int amountOfNameQualificationRequired, bool skipGlobalQualification);

          void setNameQualification ( SgVariableDeclaration* variableDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );

       // DQ (4/10/2019): We need to handle the general case of name qualification on the base type, AND also when the base type is a SgPointerMemberType 
       // we need to handled the PointerMemberType base type and the SgPointerMemberType class.  The setNameQualificationOnBaseType() is used to support 
       // the base type of the SgPointerMemberType where that is used.
       // void setNameQualification ( SgTypedefDeclaration* typedefDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualificationOnBaseType ( SgTypedefDeclaration* typedefDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualificationOnPointerMemberClass ( SgTypedefDeclaration* typedefDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );

          void setNameQualification ( SgTemplateArgument* templateArgument, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
       // void setNameQualification ( SgCastExp* castExp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired);
          void setNameQualification ( SgExpression* exp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired);

       // DQ (4/16/2019): Added to support use of SgPointerMemberType with subset of expressions.
          void setNameQualificationForPointerToMember(SgExpression* exp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired);

          void setNameQualification ( SgNonrealRefExp* exp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired);

          void setNameQualification ( SgEnumVal* enumVal, SgEnumDeclaration* enumDeclaration, int amountOfNameQualificationRequired);

       // This takes only a SgMemberFunctionDeclaration since it is where we locate the name qualification information AND is the correct scope from which to iterate backwards through scopes to evaluate what name qualification is required.
       // void setNameQualification ( SgMemberFunctionDeclaration* memberFunctionDeclaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired );

       // This case is demonstrated in test2011_62.C
       // void setNameQualification ( SgClassDeclaration* classDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgClassDeclaration* classDeclaration, int amountOfNameQualificationRequired );

       // DQ (7/8/2014): Adding support for name qualification of SgNamespaceDeclarations within a SgNamespaceAliasDeclarationStatement.
          void setNameQualification ( SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );

       // DQ (2/14/2019): Adding support for C++11 enum prototypes (and defining declaratins in different scopes requiring name qualification).
          void setNameQualification ( SgEnumDeclaration* enumDeclaration, int amountOfNameQualificationRequired );

       // This is a separate function just for setting the information specific to the name qualification of return types.
       // This information cannot be stored in the SgFunctionType since that might be shared and referenced from different locations.
       // void setNameQualificationReturnType ( SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired );
          void setNameQualificationReturnType ( SgFunctionDeclaration* functionDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );

       // DQ (4/19/2019): Adding support for chains of SpPointerMemberType types (requires type traversal).
       // void setNameQualification ( SgPointerMemberType* pointerMemberType, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualificationOnClassOf  ( SgPointerMemberType* pointerMemberType, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualificationOnBaseType ( SgPointerMemberType* pointerMemberType, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );

          SgDeclarationStatement* getDeclarationAssociatedWithType( SgType* type );

       // Supporting function for different overloaded versions of the setNameQualification() function.
          std::string setNameQualificationSupport ( SgScopeStatement* scope, const int inputNameQualificationLength, int & output_amountOfNameQualificationRequired , bool & outputGlobalQualification, bool & outputTypeEvaluation );
 
       // DQ (9/7/2014): Added template header support (associated with name qualification for template declarations.
          std::string setTemplateHeaderNameQualificationSupport(SgScopeStatement* scope, const int inputNameQualificationLength );

       // DQ (5/14/2011): type elaboration only works between non-types and types.  Different types must be distinquished using name qualification.
          bool requiresTypeElaboration(SgSymbol* symbol);

       // DQ (5/15/2011): Added support for template arguments and their recursive handling.
          void evaluateNameQualificationForTemplateArgumentList ( SgTemplateArgumentPtrList & templateArgumentList, SgScopeStatement* currentScope, SgStatement* positionStatement );

       // DQ (5/28/2011): Added support to set the global qualified name map.
          const std::map<SgNode*,std::string> & get_qualifiedNameMapForNames() const;
          const std::map<SgNode*,std::string> & get_qualifiedNameMapForTypes() const;
          const std::map<SgNode*,std::string> & get_qualifiedNameMapForTemplateHeaders() const;

       // DQ (3/13/2019): Adding support for name qualification to support multiple types that may be asociated with a 
       // single type used as a function return type (for example, always a template type containing multiple template 
       // arguments that require additional name qualification).
          const std::map<SgNode*,std::map<SgNode*,std::string> > & get_qualifiedNameMapForMapsOfTypes() const;

       // DQ (6/3/2011): Evaluate types to permit the strings representing unparsing the types 
       // are saved in a separate map associated with the IR node referencing the type.  This 
       // supports the cases where a type with template arguments may require different name 
       // qualifications on its template arguments when it is referenced from different locations 
       // in the source code.  The name qualification support saved in qualifiedNameMapForTypes 
       // only saves the name qualification of the type and not the name of the type which can 
       // itself have and require name qualification of its subtypes.  I am not clear how this 
       // may interact with function types, but we could just same the substring representing the
       // template parameters if required, my preference is to save the string for the whole time.
          void traverseType ( SgType* type, SgNode* nodeReferenceToType, SgScopeStatement* currentScope, SgStatement* positionStatement );

       // DQ (6/21/2011): Added support to generate function names containing template arguments.
          void traverseTemplatedFunction(SgFunctionRefExp* functionRefExp, SgNode* nodeReference, SgScopeStatement* currentScope, SgStatement* positionStatement );

       // DQ (5/24/2013): Added support to generate member function names containing template arguments.
          void traverseTemplatedMemberFunction(SgMemberFunctionRefExp* memberFunctionRefExp, SgNode* nodeReference, SgScopeStatement* currentScope, SgStatement* positionStatement );

       // DQ (4/12/2019): Added support to generate class names containing template arguments.
          void traverseTemplatedClass(SgBaseClass* baseClass, SgNode* nodeReference, SgScopeStatement* currentScope, SgStatement* positionStatement );

       // DQ (6/21/2011): Added function to store names with associated SgNode IR nodes.
          void addToNameMap ( SgNode* nodeReference, std::string typeNameString );

       // This extracts the template arguments and calls the function to evaluate them.
          void evaluateTemplateInstantiationDeclaration ( SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement );

       // If a declaration has not been defined in a location (scope) which could support its declaration then it can be qualified (any qualification would be ambigous).
          bool skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(SgDeclarationStatement* declaration);

       // DQ (9/12/2014): Refactored support for building part of the template declaration header so it could be called recursively for template template parameters.
          std::string buildTemplateHeaderString(SgTemplateParameterPtrList & templateParameterList);

       // DQ (3/31/2014): Adding support for global qualifiction.
          size_t depthOfGlobalNameQualification(SgDeclarationStatement* declaration);

       // DQ (4/5/2018): Debugging support.
          void functionReport(SgFunctionDeclaration* functionDeclaration);

       // DQ (1/24/2019): display accumulated private base class map.
       // void displayBaseClassMap (BaseClassSetMap & x);
          void displayBaseClassMap ( const std::string & label, BaseClassSetMap & x );

       // DQ (3/14/2019): Adding debugging support to output the map of names.
          void outputNameQualificationMap( const std::map<SgNode*,std::string> & qualifiedNameMap );
   };



