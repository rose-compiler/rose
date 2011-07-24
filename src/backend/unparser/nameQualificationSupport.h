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
     public:
          NameQualificationInheritedAttribute();
          NameQualificationInheritedAttribute(const NameQualificationInheritedAttribute & X);
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

       // DQ (6/3/2011): This is to save the names of types where they can be named differently when referenced 
       // from different locations in the source code.
          std::map<SgNode*,std::string> & typeNameMap;

       // DQ (7/22/2011): Alternatively we should treat array types just like templated types that can
       // contain subtypes that require arbitrarily complex name qualification for their different parts.
       // DQ (7/22/2011): We need to handle array types with dimensions that require qualification.
       // These are a vector of strings to support the name qualification of each indexed dimension 
       // of the array type.  The key is NOT the SgArrayType (since these are shared where the mangled
       // names match; mangled names use fully qualified names to avoid ambiguity).  Note that bit
       // field widths can require name qualification but are handled using the qualifiedNameMapForNames
       // map and the SgVarRefExp as the key to the map (so no special extra support is required).
       // std::map<SgNode*, std::vector<std::string> > & qualifiedNameMapForArrayTypes;

       // Member functions:
          std::list<SgNode*> gatherNamesInClass( SgClassDefinition* classDefinition );

       // DQ (7/23/2011): This supports nested calls where the scope of a subtrees must have its scope explicitly 
       // specified. I think this only happens for the indedx in the SgArrayType.
          SgScopeStatement* explictlySpecifiedCurrentScope;

     public:
       // HiddenListTraversal();
       // HiddenListTraversal(SgNode* root);
          NameQualificationTraversal(std::map<SgNode*,std::string> & input_qualifiedNameMapForNames, std::map<SgNode*,std::string> & input_qualifiedNameMapForTypes, std::map<SgNode*,std::string> & input_typeNameMap, std::set<SgNode*> & input_referencedNameSet);

       // DQ (7/23/2011): This permits recursive calls to the traversal AND specification of the current scope
       // used to support name qualification on expressions where we can't backout the current scope.  Used 
       // for name qualification of const expressions in SgArrayType index expressions.
          void generateNestedTraversalWithExplicitScope( SgNode* node, SgScopeStatement* currentScope );

       // Evaluates how much name qualification is required (typically 0 (no qualification), but sometimes 
       // the depth of the nesting of scopes plus 1 (full qualification with global scoping operator)).
       // int nameQualificationDepth ( SgClassDefinition* classDefinition );
          int nameQualificationDepth ( SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement, bool forceMoreNameQualification = false );
          int nameQualificationDepth ( SgInitializedName* initializedName, SgScopeStatement* currentScope, SgStatement* positionStatement );
          int nameQualificationDepth ( SgType* type, SgScopeStatement* currentScope, SgStatement* positionStatement );

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

       // DQ (6/5/2011): Added to support case where SgInitializedName in SgVarRefExp can't be traced back to a SgVariableDeclaration (see test2011_75.C).
          void setNameQualification ( SgVarRefExp* varRefExp, SgScopeStatement* scopeStatement,int amountOfNameQualificationRequired );

          void setNameQualification ( SgBaseClass* baseClass, SgClassDeclaration*    classDeclaration,   int amountOfNameQualificationRequired );
          void setNameQualification ( SgUsingDeclarationStatement* usingDeclaration, SgInitializedName* associatedInitializedName, int amountOfNameQualificationRequired );
          void setNameQualification ( SgUsingDeclarationStatement* usingDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgUsingDirectiveStatement* usingDirective, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgFunctionRefExp* functionRefExp, SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgMemberFunctionRefExp* functionRefExp, SgMemberFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired );

       // DQ (6/4/2011): This handles the case of both the declaration being a SgMemberFunctionDeclaration and a SgClassDeclaration.
       // void setNameQualification ( SgConstructorInitializer* constructorInitializer, SgMemberFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired);
          void setNameQualification ( SgConstructorInitializer* constructorInitializer, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired);

          void setNameQualification ( SgInitializedName* initializedName, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgVariableDeclaration* variableDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgTypedefDeclaration* typedefDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgTemplateArgument* templateArgument, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
       // void setNameQualification ( SgCastExp* castExp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired);
          void setNameQualification ( SgExpression* exp, SgDeclarationStatement* typeDeclaration, int amountOfNameQualificationRequired);

          void setNameQualification ( SgEnumVal* enumVal, SgEnumDeclaration* enumDeclaration, int amountOfNameQualificationRequired);

       // This takes only a SgMemberFunctionDeclaration since it is where we locate the name qualification information AND is the correct scope from which to iterate backwards through scopes to evaluate what name qualification is required.
       // void setNameQualification ( SgMemberFunctionDeclaration* memberFunctionDeclaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired );

       // This case is demonstrated in test2011_62.C
       // void setNameQualification ( SgClassDeclaration* classDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgClassDeclaration* classDeclaration, int amountOfNameQualificationRequired );

       // This is a separate function just for setting the information specific to the name qualification of return types.
       // This information cannot be stored in the SgFunctionType since that might be shared and referenced from different locations.
       // void setNameQualificationReturnType ( SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired );
          void setNameQualificationReturnType ( SgFunctionDeclaration* functionDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );

          SgDeclarationStatement* getDeclarationAssociatedWithType( SgType* type );

       // Supporting function for different overloaded versions of the setNameQualification() function.
          std::string setNameQualificationSupport ( SgScopeStatement* scope, const int inputNameQualificationLength, int & output_amountOfNameQualificationRequired , bool & outputGlobalQualification, bool & outputTypeEvaluation );
 
       // DQ (5/14/2011): type elaboration only works between non-types and types.  Different types must be distinquished using name qualification.
          bool requiresTypeElaboration(SgSymbol* symbol);

       // DQ (5/15/2011): Added support for template arguments and their recursive handling.
          void evaluateNameQualificationForTemplateArgumentList ( SgTemplateArgumentPtrList & templateArgumentList, SgScopeStatement* currentScope, SgStatement* positionStatement );

       // DQ (5/28/2011): Added support to set the global qualified name map.
          const std::map<SgNode*,std::string> & get_qualifiedNameMapForNames() const;
          const std::map<SgNode*,std::string> & get_qualifiedNameMapForTypes() const;

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

       // DQ (6/21/2011): Added support to generate function names containging template arguments.
          void traverseTemplatedFunction(SgFunctionRefExp* functionRefExp, SgNode* nodeReferenceToType, SgScopeStatement* currentScope, SgStatement* positionStatement );

       // DQ (6/21/2011): Added function to store names with associated SgNode IR nodes.
          void addToNameMap ( SgNode* nodeReferenceToType, std::string typeNameString );

       // This extracts the template arguments and calls the function to evaluate them.
          void evaluateTemplateInstantiationDeclaration ( SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement );

       // If a declaration has not been defined in a location (scope) which could support its declaration then it can be qualified (any qualification would be ambigous).
          bool skipNameQualificationIfNotProperlyDeclaredWhereDeclarationIsDefinable(SgDeclarationStatement* declaration);

   };



