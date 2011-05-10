// DQ (5/4/2011): This is the redesign of the support for name qualification.
// The steps are:
// 1) All using directives put alias sysmbols into place into the scope 
//    where they appear and for each symbol in the scope being used.
// 1) Compute the hidden type tables
// 2) Use the hidden type tables to determine which types need qualification.

// * We will want to keep track of hidden variables too!  But first handle types.
// * When we refer to names it will be of either variables or types.


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

          bool isANamedConstruct(SgNode* n);

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

   };
