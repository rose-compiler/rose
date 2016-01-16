// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed seperately.
#include "rose.h"

#include "attributeGenerator.h"

// This might be helpful, but not clear yet.
// #include "dslSupport.h"
// using namespace DSL_Support;

using namespace std;


// Inherited Attribute
AttributeGenerator_InheritedAttribute::AttributeGenerator_InheritedAttribute()
   {
   }

AttributeGenerator_InheritedAttribute::AttributeGenerator_InheritedAttribute(const AttributeGenerator_InheritedAttribute & X)
   {
   }


// Synthesized Attribute
AttributeGenerator_SynthesizedAttribute::AttributeGenerator_SynthesizedAttribute()
   {
   }

AttributeGenerator_SynthesizedAttribute::AttributeGenerator_SynthesizedAttribute(SgNode* ast)
   {
   }

AttributeGenerator_SynthesizedAttribute::AttributeGenerator_SynthesizedAttribute(const AttributeGenerator_SynthesizedAttribute & X)
   {
   }


// Attribute Generator Traversal
AttributeGeneratorTraversal::AttributeGeneratorTraversal()
   {
   }

AttributeGenerator_InheritedAttribute
AttributeGeneratorTraversal::evaluateInheritedAttribute   (SgNode* astNode, AttributeGenerator_InheritedAttribute inheritedAttribute )
   {
     ROSE_ASSERT(astNode != NULL);

     AttributeGenerator_InheritedAttribute return_inheritedAttribute;

     switch (astNode->variantT())
        {
          case V_SgClassDeclaration:
          case V_SgTemplateClassDeclaration:
             {
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(astNode);
               ROSE_ASSERT(classDeclaration != NULL);

               printf ("Build DSL attribute for class = %p = %s = %s \n",classDeclaration,classDeclaration->class_name().c_str(),classDeclaration->get_name().str());

               SgType* type = classDeclaration->get_type();
               ROSE_ASSERT(type != NULL);

               SgNode* ast_fragment = buildAttribute(type);

             }

          default:
             {
            // Cases not handled
               printf ("AttributeGeneratorTraversal::evaluateInheritedAttribute: case not handled: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
             }
        }

     return return_inheritedAttribute;
   }

AttributeGenerator_SynthesizedAttribute
AttributeGeneratorTraversal::evaluateSynthesizedAttribute (SgNode* astNode, AttributeGenerator_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
  // We might not need the synthesized attribute evaluation (we will see).
  // AttributeGenerator_SynthesizedAttribute return_SynthesizedAttribute;

     SgNode* generated_ast = NULL;



  // return return_SynthesizedAttribute(generated_ast);
     return AttributeGenerator_SynthesizedAttribute (generated_ast);
   }


SgNode* 
AttributeGeneratorTraversal::buildAttribute(SgType* type)
   {
     printf ("Build DSL attribute for type = %p = %s \n",type,type->class_name().c_str());

     SgTemplateInstantiationDecl* templateClass = NULL;

     return NULL;
   }


#define DEBUG_USING_DOT_GRAPHS 1

int main( int argc, char * argv[] )
   {
  // We don't need the constant folding to support the generation of the DSL attributes.

     bool frontendConstantFolding = false;
     SgProject* project = frontend(argc,argv,frontendConstantFolding);
     ROSE_ASSERT(project != NULL);

#if DEBUG_USING_DOT_GRAPHS
     generateDOT(*project,"_before_transformation");
#endif

#if 1
     AttributeGeneratorTraversal t;
     AttributeGenerator_InheritedAttribute ih;

  // AttributeGenerator_SynthesizedAttribute sh = t.traverseInputFiles(project,ih);
     t.traverseInputFiles(project,ih);
#endif

     return 0;
   }
