// Generate a patch to segregate code into a base code and a code with enhansed capabilities.
// Note generation of the patch requires output using the token stream (-rose:unparse_tokens).

#include "rose.h"

// using namespace std;

// using namespace SageBuilder;
// using namespace SageInterface;

// string prefixForFunctions = "_abc_function_";
// string prefixForVariables = "_abc_var_";

// DQ (11/23/2018): Add that we need to inherite from SgSynthesizedAttribute.
class WhiteSpaceInheritedAttribute : public SgInheritedAttribute
   {
  // Attribute used to pass information computed DOWN within the tree traversal.
     public:
          bool foundSegragationBoundary_start;
          SgNode* astNode;
          bool segregateThisNode;
          bool foundSegregatedNode;

     public:
            WhiteSpaceInheritedAttribute ();
            WhiteSpaceInheritedAttribute ( const WhiteSpaceInheritedAttribute & X );
   };

// DQ (11/23/2018): Add that we need to inherite from SgSynthesizedAttribute.
class WhiteSpaceSynthesizedAttribute : public SgSynthesizedAttribute
   {
  // Attribute used to pass information computed UP within the tree traversal.
     public:
          bool foundSegragationBoundary_start;
          bool foundSegragationBoundary_end;
          SgNode* astNode;
          bool segregateThisNode;
          bool containsSegregatedNode;

     public:
          WhiteSpaceSynthesizedAttribute ();

          WhiteSpaceSynthesizedAttribute ( SgNode* input_astNode );

          WhiteSpaceSynthesizedAttribute ( const WhiteSpaceSynthesizedAttribute & X );

          WhiteSpaceSynthesizedAttribute & operator= ( const WhiteSpaceSynthesizedAttribute & X );
   };


class Traversal : public SgTopDownBottomUpProcessing<WhiteSpaceInheritedAttribute,WhiteSpaceSynthesizedAttribute>
   {
     public:
       // Accumulate the collection of intervales of segregated regions.
          std::vector<std::pair<SgNode*,SgNode*>*> intervalList;

       // Save symbols associated with identified nodes for code that will be segregated.
          std::set<SgSymbol*> symbolSet;

     public:
       // Functions required
          WhiteSpaceInheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode, 
             WhiteSpaceInheritedAttribute inheritedAttribute );

          WhiteSpaceSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             WhiteSpaceInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };


// Segregation attribute so that we can color the intervals.
class Segregation_Attribute: public AstAttribute 
   {
     public:
          Segregation_Attribute();
          virtual ~Segregation_Attribute();

          std::string toString();
          std::string additionalNodeOptions();

       // Most attributes will have children that define there embedding into the AST.
          std::vector<SgNode*> additionalChildren;

          SgNode* currentNode;
          std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();

          std::string get_name();
          std::string get_color();
   };

