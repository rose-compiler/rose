// This code implements a generic detection for an specific DSL abstraction
// that is know at compile time and provided via a const array of names (defined
// in generated_dsl_attributes.h).




// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed seperately.
#include "rose.h"

// Supporting code for recognition of types by name and variables in variable declarations, etc.
#include "dslSupport.h"

// #include "generated_dsl_attributes.h"
#include "dsl_attribute_support.h"

// #include "array.h"
// #include "nongenerated_dsl_attributes.h"

#include "dsl_detection.h"

using namespace std;

using namespace DSL_Support;


Detection_InheritedAttribute::Detection_InheritedAttribute()
   {
  // DQ (2/18/2015): Added to support debugging by restricting traversals to main function.
     inFunction     = false;
     inDSL_Function = false;
   }

Detection_InheritedAttribute::Detection_InheritedAttribute( const Detection_InheritedAttribute & X )
   {
  // DQ (2/18/2015): Added to support debugging by restricting traversals to functions 
  // relevant to the DSL (simplifies the debugging).
     inFunction     = X.inFunction;
     inDSL_Function = X.inDSL_Function;
   }


#if 1
// DQ (2/16/2015): We want to support saving the AST node, but this default constructor is required by the AstProcessing.h file.
Detection_SynthesizedAttribute::Detection_SynthesizedAttribute()
   : node(NULL)
   {
   }
#endif

Detection_SynthesizedAttribute::Detection_SynthesizedAttribute( SgNode* astNode )
   : node(astNode)
   {
   }

Detection_SynthesizedAttribute::Detection_SynthesizedAttribute( const Detection_SynthesizedAttribute & X )
   : node(X.node)
   {
  // copy the accumulated children
     dslChildren = X.dslChildren;
   }

DetectionTraversal::DetectionTraversal( SgNode* input_root )
   {
     root = input_root;
   }

void
DetectionTraversal::display()
   {
  // This function may be useful for representing the DSL child nodes for operators and such as part of debugging.

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


Detection_InheritedAttribute
DetectionTraversal::evaluateInheritedAttribute (SgNode* astNode, Detection_InheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("In DetectionTraversal::evaluateInheritedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

  // DQ (2/3/2016): Recognize IR nodes that are representative of target DSL abstractions.
     bool foundTargetDslAbstraction = DSL_Support::isDslAbstraction(astNode);

#if 1
     printf ("In DetectionTraversal::evaluateInheritedAttribute(): astNode = %p = %s: foundTargetDslAbstraction = %s \n",astNode,astNode->class_name().c_str(),foundTargetDslAbstraction ? "true" : "false");
#endif

#if 0
  // OLD CODE (represented by DSL_Support::isDslAbstraction() function).

  // Detection of stencil declaration and stencil operator.
  // Where the stencil specification is using std::vectors as parameters to the constructor, we have to first
  // find the stencil declaration and read the associated SgVarRefExp to get the variable names used.  
  // Then a finite state machine can be constructed for each of the input variables so that we can 
  // interpret the state when the stencil operator is constructed.
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(astNode);
     if (variableDeclaration != NULL)
        {
       // Get the SgInitializedName from the SgVariableDeclaration.
          SgInitializedName* initializedName = SageInterface::getFirstInitializedName(variableDeclaration);

          SgType* base_type = initializedName->get_type()->findBaseType();
          ROSE_ASSERT(base_type != NULL);

       // SgClassType* classType = isSgClassType(initializedName->get_type());
          SgClassType* classType = isSgClassType(base_type);

          if (classType != NULL)
             {
#if 1
               printf ("In DetectionTraversal::evaluateInheritedAttribute(): case SgClassType: class name = %s \n",classType->get_name().str());
#endif
            // Check if this is associated with a template instantiation.
               SgTemplateInstantiationDecl* templateInstantiationDecl = isSgTemplateInstantiationDecl(classType->get_declaration());
               if (templateInstantiationDecl != NULL)
                  {
#if 1
                    printf ("case SgTemplateInstaiationDecl: class name = %s \n",classType->get_name().str());
                    printf ("case SgTemplateInstaiationDecl: templateInstantiationDecl->get_templateName() = %s \n",templateInstantiationDecl->get_templateName().str());
#endif
                 // inheritedAttribute.set_StencilDeclaration(templateInstantiationDecl->get_templateName() == "Stencil");
                 // inheritedAttribute.set_StencilOperatorDeclaration(templateInstantiationDecl->get_templateName() == "StencilOperator");

                    if (templateInstantiationDecl->get_templateName() == "Stencil")
                       {
                      // DQ (2/8/2015): Ignore compiler generated IR nodes (from template instantiations, etc.).
                      // Note that simpleCNS.cpp generates one of these from it's use of the tuple template and associated template instantations.

                      // DQ: Test the DSL support.
                         ROSE_ASSERT(isMatchingClassType(classType,"Stencil",true) == true);

                         checkAndResetToMakeConsistantCompilerGenerated(initializedName);

                         if (initializedName->isCompilerGenerated() == false)
                            {
                           // Save the SgInitializedName associated with the stencil.
                           // stencilInitializedNameList.push_back(initializedName);
                           // inheritedAttribute.set_StencilDeclaration(true);
                           // foundStencilVariable = true;
#if 1
                              printf ("Detected Stencil<> typed variable: initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
                           // printf ("   --- stencilInitializedNameList.size() = %zu \n",stencilInitializedNameList.size());
#endif
#if 1
                              initializedName->get_file_info()->display("In DetectionTraversal::evaluateInheritedAttribute(): initializedName : debug");
#endif
#if 0
                              Stencil_Attribute* dslAttribute = new Stencil_Attribute();
#if 1
                              printf ("Adding (Stencil) dslAttribute = %p \n",dslAttribute);
#endif
                              ROSE_ASSERT(dslAttribute != NULL);

                           // virtual void addNewAttribute (std::string s, AstAttribute *a);   
                              initializedName->addNewAttribute(StencilVariable,dslAttribute);
#endif
                            }
                       }
                  }

               SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
               if (classDeclaration != NULL)
                  {
                    if (classDeclaration->get_name() == "Point")
                       {
                      // Save the SgInitializedName associated with the Point type.
#if 0
                         printf ("Detected Point<> typed variable: initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
#endif
                         checkAndResetToMakeConsistantCompilerGenerated(initializedName);

                         if (initializedName->isCompilerGenerated() == false)
                            {
                           // pointInitializedNameList.push_back(initializedName);
#if 0
                              Point_Attribute* dslAttribute = new Point_Attribute();
                              printf ("Adding (Point) dslAttribute = %p \n",dslAttribute);
                              ROSE_ASSERT(dslAttribute != NULL);

                           // virtual void addNewAttribute (std::string s, AstAttribute *a);   
                              initializedName->addNewAttribute(PointVariable,dslAttribute);
#endif
                            }
                       }
                  }
             }
        }
#endif

#if 1
     printf ("Leaving DetectionTraversal::evaluateInheritedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

  // Construct the return attribute from the modified input attribute.
     return Detection_InheritedAttribute(inheritedAttribute);
   }


Detection_SynthesizedAttribute
DetectionTraversal::evaluateSynthesizedAttribute (SgNode* astNode, Detection_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     ROSE_ASSERT(astNode != NULL);

     Detection_SynthesizedAttribute return_synthesizedAttribute(astNode);

#if 1
     printf ("In evaluateSynthesizedAttribute(): astNode = %p = %s synthesizedAttributeList.size() = %zu dslChildren.size() = %zu \n",
          astNode,astNode->class_name().c_str(),synthesizedAttributeList.size(),return_synthesizedAttribute.dslChildren.size());
#endif

  // At each IR node and across all children, accumulate the dslChildren (child nodes for each of the DSL AST nodes).
     for (SubTreeSynthesizedAttributes::iterator i = synthesizedAttributeList.begin(); i != synthesizedAttributeList.end(); i++)
        {
          SgNode* childNode = (*i).node;
       // ROSE_ASSERT(childNode != NULL);
          if (childNode != NULL)
             {
#if 0
               printf ("Identified child node in evaluateSynthesizedAttribute(): childNode = %p = %s \n",childNode,childNode->class_name().c_str());
#endif
            // Insert each list from the child into the accumulated list in the current Synthesized Attribute.
               return_synthesizedAttribute.dslChildren.insert(return_synthesizedAttribute.dslChildren.end(),i->dslChildren.begin(),i->dslChildren.end());
#if 0
               printf ("   --- copying i->dslChildren.size() = %zu into return_synthesizedAttribute.dslChildren.size() = %zu \n",i->dslChildren.size(),return_synthesizedAttribute.dslChildren.size());
#endif
               if (return_synthesizedAttribute.dslChildren.empty() == false)
                  {
#if 0
                    printf ("In evaluateSynthesizedAttribute(): dslChildren.size() = %zu \n",return_synthesizedAttribute.dslChildren.size());
#endif
                  }
             }
            else
             {
#if 1
               printf ("childNode == NULL \n");
#endif
             }
        }


  // Recognition of other control statements (e.g. for loops for the stencil evaluation) or expression/statement abstractions (e.g. function calls)
  // Note: DSL specific control abstractions might be recognised as loops containing DSL abstractions and having constant evaluatable base and bounds.


  // For any DSL specific AST nodes (C++ AST nodes containing a DSL_Attribute), initialize the pointers to children of the DSL IR node.
     AstAttributeMechanism* astAttributeContainer = astNode->get_attributeMechanism();
     if (astAttributeContainer != NULL)
        {
#if 1
          printf ("In evaluateSynthesizedAttribute(): found a attribute on astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
       // I think there should only be one DSL attribute, in the future we can support more on a single IR node.
          if (astAttributeContainer->size() != 1)
             {
               printf ("WARNING: astAttributeContainer->size() != 1: astAttributeContainer->size() = %zu \n",astAttributeContainer->size());
             }
#if 1
       // DQ: Allow this for the moment while testing.
          ROSE_ASSERT(astAttributeContainer->size() == 1);
#endif
       // Loop over all the attributes at this IR node
       // Pei-Hung (12/22/15): THe ASTAttributeMechanmsim is changed and has to use new API
       // for (AstAttributeMechanism::iterator i = astAttributeContainer->begin(); i != astAttributeContainer->end(); i++)
          BOOST_FOREACH (const std::string &attributeName, astAttributeContainer->getAttributeIdentifiers()) 
             {
               AstAttribute* attribute = astNode->getAttribute(attributeName);
               ROSE_ASSERT(attribute != NULL);
#if 1
            // DSL_Attribute* dslAstAttribute = dynamic_cast<DSL_Attribute*>(attribute);
               dsl_attribute* dslAstAttribute = dynamic_cast<dsl_attribute*>(attribute);
               ROSE_ASSERT(dslAstAttribute != NULL);
#if 1
               printf ("Identified dslAstAttribute in evaluateSynthesizedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
               printf ("   --- return_synthesizedAttribute.dslChildren.size() = %zu \n",return_synthesizedAttribute.dslChildren.size());
#endif
            // Copy the dsl child data to the dsl attribute.
               dslAstAttribute->currentNode = astNode;
               dslAstAttribute->dslChildren = return_synthesizedAttribute.dslChildren;
#endif
             }

       // Clear the dsl attributes becasue we the only collect dsl child attributes at dsl attributed IR nodes and don't pass then further up the tree.          
          return_synthesizedAttribute.dslChildren.clear();

       // Add the current node since it has an attribute.
          return_synthesizedAttribute.dslChildren.push_back(astNode);

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
#if 1
          printf ("no attributes: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
        }

#if 1
     printf ("Leaving evaluateSynthesizedAttribute(): return_synthesizedAttribute.dslChildren.size() = %zu \n",return_synthesizedAttribute.dslChildren.size());
#endif

     return return_synthesizedAttribute;
   }

