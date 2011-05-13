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
void newBuildHiddenTypeAndDeclarationLists( SgNode* node );



class HiddenListInheritedAttribute
   {
     public:
          HiddenListInheritedAttribute();
          HiddenListInheritedAttribute(const HiddenListInheritedAttribute & X);


   };


class HiddenListSynthesizedAttribute
   {
     public:
          HiddenListSynthesizedAttribute();
          HiddenListSynthesizedAttribute( const HiddenListSynthesizedAttribute & X );
   };


class nameQualificationInformation
   {
   };

class HiddenListTraversal : public AstTopDownBottomUpProcessing<HiddenListInheritedAttribute, HiddenListSynthesizedAttribute>
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
          std::set<SgNode*> referencedNameSet;

       // Member functions:
          std::list<SgNode*> gatherNamesInClass( SgClassDefinition* classDefinition );

     public:
          HiddenListTraversal();
       // HiddenListTraversal(SgNode* root);

       // Evaluates how much name qualification is required (typically 0 (no qualification), but sometimes 
       // the depth of the nesting of scopes plus 1 (full qualification with global scoping operator)).
       // int nameQualificationDepth ( SgClassDefinition* classDefinition );
          int nameQualificationDepth ( SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement );
          int nameQualificationDepth ( SgInitializedName* initializedName, SgScopeStatement* currentScope, SgStatement* positionStatement );

          int nameQualificationDepthOfParent(SgDeclarationStatement* declaration, SgScopeStatement* currentScope, SgStatement* positionStatement );
          int nameQualificationDepthForType ( SgInitializedName* initializedName, SgStatement* positionStatement );

       // SgName associatedName(SgScopeStatement* scope);
          SgDeclarationStatement* associatedDeclaration(SgScopeStatement* scope);

       // These don't really need to be virtual, since we don't derive from this class.
          virtual HiddenListInheritedAttribute evaluateInheritedAttribute(SgNode* n, HiddenListInheritedAttribute inheritedAttribute);

          virtual HiddenListSynthesizedAttribute evaluateSynthesizedAttribute(SgNode* n, HiddenListInheritedAttribute inheritedAttribute, SynthesizedAttributesList synthesizedAttributeList);

       // Set the values in each reference to the name qualified language construct.
          void setNameQualification ( SgVarRefExp* varRefExp, SgVariableDeclaration* variableDeclaration,int amountOfNameQualificationRequired );
          void setNameQualification ( SgBaseClass* baseClass, SgClassDeclaration*    classDeclaration,   int amountOfNameQualificationRequired );
          void setNameQualification ( SgUsingDeclarationStatement* usingDeclaration, SgInitializedName* associatedInitializedName, int amountOfNameQualificationRequired );
          void setNameQualification ( SgUsingDeclarationStatement* usingDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgUsingDirectiveStatement* usingDirective, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgFunctionRefExp* functionRefExp, SgFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgMemberFunctionRefExp* functionRefExp, SgMemberFunctionDeclaration* functionDeclaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgInitializedName* initializedName, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );
          void setNameQualification ( SgVariableDeclaration* variableDeclaration, SgDeclarationStatement* declaration, int amountOfNameQualificationRequired );

          SgDeclarationStatement* getDeclarationAssociatedWithType( SgType* type );

       // Supporting function for different overloaded versions of the setNameQualification() function.
          void setNameQualificationSupport(SgScopeStatement* scope, const int inputNameQualificationLength, int & output_amountOfNameQualificationRequired , bool & outputGlobalQualification, bool & outputTypeEvaluation );

   };
