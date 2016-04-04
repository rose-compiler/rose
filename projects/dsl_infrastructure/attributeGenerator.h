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
       // Save the pragma so that we can evaluate which of different possible DSL pragmas were used.
          std::string pragma_string;

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

       // Make a seperate list of DSL nodes where we will not try to support value tracking in the data flow transfer equations.
          std::set<SgStatement*> DSLnodes_novalue;

          SgSourceFile* generatedHeaderFile;
          SgSourceFile* generatedSourceFile;

       // Required code to support our code generator.
          SgSourceFile* requiredSourceCodeFile;

          SgGlobal* global_scope_header;
          SgGlobal* global_scope_source;

          SgGlobal* global_scope_requiredSourceCode;

       // Variables that we will initialize with types, functions, etc.
          SgInitializedName* dsl_type_names_variable;
          SgInitializedName* dsl_function_names_variable;
          SgInitializedName* dsl_member_function_names_variable;
          SgInitializedName* dsl_attribute_map_variable;

#if 0
       // This is the code generation, we are passing the inforamtion to
       // the DSL compiler for types and functions in terms of names.
          std::vector<std::string> dsl_type_name_list;
          std::vector<std::string> dsl_function_name_list;
          std::vector<std::string> dsl_member_function_name_list;

       // Here we have to generate constructor calls in the generated code that will be used
       // in the DSL compiler, so we need to generate references to constructor initializers.
          std::vector<SgConstructorInitializer*> dsl_attribute_map_list;
#endif
       // I think it is better to save the pointer to the DSL abstraction type instead of a string representing the name.
       // All types are then used to generate attributes as a last step in the traversal (in the syntheziedAttribute evaluation.
          std::vector<SgType*> dsl_type_list;
          std::vector<SgFunctionDeclaration*> dsl_function_list;

       // I now think that this will simplify the handling of the generated code since we support a 
       // dsl_member_function_names_variable to know the class associated with each member function.
       // I don't think we need this since member functions and nonmember functions can be combined in the dsl_function_list.
          std::vector<SgMemberFunctionDeclaration*> dsl_member_function_list;

       // I am not yet clar how to generate the initializers for this DSL variable.
          std::vector<SgConstructorInitializer*> dsl_attribute_map_list;

       // DQ (3/16/2016): Name collision testing.
          std::map<std::string,int> dsl_attribute_name_collision_map;

       // DQ (3/4/2016): Turning on internal debugging.
          bool internal_debugging;

       // AttributeGeneratorTraversal();
          AttributeGeneratorTraversal( SgProject* project );

       // Functions required to overload the pure virtual functions in the abstract base class.
          AttributeGenerator_InheritedAttribute   evaluateInheritedAttribute   (SgNode* astNode, AttributeGenerator_InheritedAttribute inheritedAttribute );
          AttributeGenerator_SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* astNode, AttributeGenerator_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );

#if 1
       // SgNode* buildAttribute(SgType* type);
          SgNode* buildAttribute(SgType* type, bool isDSLnode_valueTracking);
          SgNode* buildAttribute(SgFunctionDeclaration* functionDeclaration);
#endif

          void unparseGeneratedCode();

       // Process the variables used to communicate DSL abstractions to the DSL compiler.
          void processvariable(SgInitializedName* initializedName);

          void modify_dsl_variable_initializers();

       // Generate unique name for use as a class name for the generated attribute classes.
       // std::string generateUniqueNameForUseAsIdentifier ( SgDeclarationStatement* declaration );
          std::string generateUniqueName ( SgDeclarationStatement* declaration );
   };


#if 0
class AttributeGenerator
   {
     public:
          std::string abstractionName;


   };
#endif
