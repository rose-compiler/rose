// This code demonstrate the automated generation of the code to support 
// detection and maybe also evaluation of DSL abstractions.
//
// The steps are:
//    1) Travers header files representing an implementation of the DSL.
//    2) For each DSL abstraction language construct, generate and attribute
//       to represent each function, class, member function, as needed to 
//       support the DSL abstraction's public API.
//    3) Generate the code to support the evaluation of the DSL abstractions
//       at compile time (by the DSL compiler).
//
//    Notes: 
//    a) Generated code is used in the implementaiton of the DSL compiler.
//    b) The DSL compiler reads any input code using the DSL abstraction and
//       calls the evaluation (does the partial evaluation) of the DSL abstractions.
//    c) At the end of the DSL compiler pass, all compile-time defined properties 
//       of the DSL are known.
//    d) The manual step is to connect the know semantics of the evaluated DSL 
//       abstractions to one or more code generators.
//

#include "dslAttribute.h"
       
class AttributeGenerator_InheritedAttribute
   {
     public:
          AttributeGenerator_InheritedAttribute();
          AttributeGenerator_InheritedAttribute(const AttributeGenerator_InheritedAttribute & X);
   };


class AttributeGenerator_SynthesizedAttribute
   {
     public:
          AttributeGenerator_SynthesizedAttribute();

          AttributeGenerator_SynthesizedAttribute(SgNode* ast);

          AttributeGenerator_SynthesizedAttribute(const AttributeGenerator_SynthesizedAttribute & X);
   };


class AttributeGeneratorTraversal : public SgTopDownBottomUpProcessing<AttributeGenerator_InheritedAttribute,AttributeGenerator_SynthesizedAttribute>
   {
     public:
       // Keep a map of DLS nodes identified via pragmas (as the statement after the pragma).
          std::set<SgStatement*> DSLnodes;

          SgSourceFile* generatedHeaderFile;
          SgSourceFile* generatedSourceFile;

       // Required code to support our code generator.
          SgSourceFile* requiredSourceCodeFile;

          SgGlobal* global_scope_header;
          SgGlobal* global_scope_source;

          SgGlobal* global_scope_requiredSourceCode;

          AttributeGeneratorTraversal();

       // Functions required to overload the pure virtual functions in the abstract base class.
          AttributeGenerator_InheritedAttribute   evaluateInheritedAttribute   (SgNode* astNode, AttributeGenerator_InheritedAttribute inheritedAttribute );
          AttributeGenerator_SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* astNode, AttributeGenerator_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );

          SgNode* buildAttribute(SgType* type);

          void unparseGeneratedCode();
   };


#if 0
class AttributeGenerator
   {
     public:
          std::string abstractionName;


   };
#endif
