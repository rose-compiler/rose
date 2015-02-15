
// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed seperately.
#include "rose.h"

// #include "shiftCalculusCompiler.h"
#include "stencilAndStencilOperatorDetection.h"

#include "dslSupport.h"

using namespace std;

using namespace DSL_Support;

Detection_InheritedAttribute::Detection_InheritedAttribute()
   {
     isStencilDeclaration = false;
     isStencilOperatorDeclaration = false;
     isShiftExpression    = false;
   }

Detection_InheritedAttribute::Detection_InheritedAttribute( const Detection_InheritedAttribute & X )
   {
     isStencilDeclaration = X.isStencilDeclaration;
     isStencilOperatorDeclaration = X.isStencilOperatorDeclaration;
     isShiftExpression    = X.isShiftExpression;
   }

void
Detection_InheritedAttribute::set_StencilDeclaration(bool value)
   {
     isStencilDeclaration = value;
   }

bool
Detection_InheritedAttribute::get_StencilDeclaration()
   {
     return isStencilDeclaration;
   }

void
Detection_InheritedAttribute::set_StencilOperatorDeclaration(bool value)
   {
     isStencilOperatorDeclaration = value;
   }

bool
Detection_InheritedAttribute::get_StencilOperatorDeclaration()
   {
     return isStencilOperatorDeclaration;
   }

void
Detection_InheritedAttribute::set_StencilOperatorRef(bool value)
   {
     isStencilOperatorRef = value;
   }

bool
Detection_InheritedAttribute::get_StencilOperatorRef()
   {
     return isStencilOperatorRef;
   }




Detection_SynthesizedAttribute::Detection_SynthesizedAttribute()
   {
     stencilOperatorTransformed = false;
   }

Detection_SynthesizedAttribute::Detection_SynthesizedAttribute( const Detection_SynthesizedAttribute & X )
   {
     stencilOperatorTransformed = X.stencilOperatorTransformed;
   }

void
Detection_SynthesizedAttribute::set_stencilOperatorTransformed(bool value)
   {
     stencilOperatorTransformed = value;
   }

bool
Detection_SynthesizedAttribute::get_stencilOperatorTransformed()
   {
     return stencilOperatorTransformed;
   }



DetectionTraversal::DetectionTraversal()
   {
   }

std::vector<SgInitializedName*> & 
DetectionTraversal::get_stencilInputInitializedNameList()
   {
     return stencilInputInitializedNameList;
   }

std::vector<SgInitializedName*> & 
DetectionTraversal::get_stencilOperatorInitializedNameList()
   {
     return stencilOperatorInitializedNameList;
   }

std::vector<SgFunctionCallExp*> & 
DetectionTraversal::get_stencilOperatorFunctionCallList()
   {
     return stencilOperatorFunctionCallList;
   }

void
DetectionTraversal::display()
   {
#if 1
     printf ("In DetectionTraversal::display(): stencilInputInitializedNameList.size() = %zu \n",stencilInputInitializedNameList.size());
#endif
#if 1
     printf ("In DetectionTraversal::display(): stencilInputInitializedNameList.size() = %zu \n",stencilInputInitializedNameList.size());
     for (size_t i = 0; i < stencilInputInitializedNameList.size(); i++)
        {
          printf ("   --- stencilInputInitializedNameList[%zu]->get_name() = %s \n",i,stencilInputInitializedNameList[i]->get_name().str());
        }
#endif
#if 1
     printf ("In DetectionTraversal::display(): stencilInitializedNameList.size() = %zu \n",stencilInitializedNameList.size());
     for (size_t i = 0; i < stencilInitializedNameList.size(); i++)
        {
          printf ("   --- stencilInitializedNameList[%zu]->get_name() = %s \n",i,stencilInitializedNameList[i]->get_name().str());
        }
#endif

#if 1
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


Detection_InheritedAttribute
DetectionTraversal::evaluateInheritedAttribute (SgNode* astNode, Detection_InheritedAttribute inheritedAttribute )
   {
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

          bool foundStencilVariable = false;

          SgClassType* classType = isSgClassType(initializedName->get_type());

          SgArrayType* arrayType = isSgArrayType(initializedName->get_type());
          if (classType == NULL && arrayType != NULL)
             {
               classType = isSgClassType(arrayType->get_base_type());
               if (classType != NULL)
                  {
                    printf ("Detected class type in array type base type \n");
                  }
             }

          if (classType != NULL)
             {
#if 0
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

                         ROSE_ASSERT(initializedName->get_startOfConstruct() != NULL);
                         bool isCompilerGenerated_result = initializedName->get_startOfConstruct()->isCompilerGenerated();
                         ROSE_ASSERT(initializedName->get_endOfConstruct() != NULL);
                         if (isCompilerGenerated_result != initializedName->get_endOfConstruct()->isCompilerGenerated())
                            {
                              initializedName->get_endOfConstruct()->display("In DetectionTraversal::evaluateInheritedAttribute(): error: startOfConstruct()->isCompilerGenerated() != endOfConstruct()->isCompilerGenerated(): debug");
                              initializedName->get_endOfConstruct()->setCompilerGenerated();
                            }
                         ROSE_ASSERT(isCompilerGenerated_result == initializedName->get_endOfConstruct()->isCompilerGenerated());

                         if (initializedName->isCompilerGenerated() == false)
                            {
                           // Save the SgInitializedName associated with the stencil.
                              stencilInitializedNameList.push_back(initializedName);
                              inheritedAttribute.set_StencilDeclaration(true);
                              foundStencilVariable = true;
#if 1
                              printf ("Detected Stencil<> typed variable: initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
#endif
#if 0
                              initializedName->get_file_info()->display("In DetectionTraversal::evaluateInheritedAttribute(): initializedName : debug");
#endif
                            }
                       }

                 // I think that this could be put into the second traversal instead of this traversal with 
                 // is the first traversal, but it is not critical to the design either way.
                    if (templateInstantiationDecl->get_templateName() == "StencilOperator")
                       {
                      // Save the SgInitializedName associated with the stencil operator.
                         stencilOperatorInitializedNameList.push_back(initializedName);
                         inheritedAttribute.set_StencilOperatorDeclaration(true);
#if 1
                         printf ("Detected StencilOperator<> typed variable: initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
#endif
                       }
                  }
             }

#if 0
          printf ("case inheritedAttribute.get_StencilDeclaration() == true: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
#if 0
          printf ("Found a stencil declaration \n");
          variableDeclaration->get_file_info()->display("location of stencil declaration: debug");
#endif
       // SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(astNode);
          SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(initializedName->get_initptr());
       // if (constructorInitializer != NULL)
          if (foundStencilVariable == true && constructorInitializer != NULL)
             {
            // SgMemberFunctionDeclaration* memberFunctionDeclaration = constructorInitializer->get_declaration();
#if 0
               printf ("memberFunctionDeclaration = %p = %s name = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->class_name().c_str(),memberFunctionDeclaration->get_name().str());
               memberFunctionDeclaration->get_file_info()->display("memberFunctionDeclaration (constructor): debug");
#endif
            // Process (save) the inputs to the stencil declaration.
               SgExprListExp* expListExp = constructorInitializer->get_args();
               for (size_t i = 0; i < expListExp->get_expressions().size(); i++)
                  {
                    SgExpression* exp = expListExp->get_expressions()[i];
                    stencilInputExpressionList.push_back(exp);
                    SgVarRefExp* varRefExp = isSgVarRefExp(exp);
                    if (varRefExp != NULL)
                       {
                         SgVariableSymbol*  variableSymbol = isSgVariableSymbol(varRefExp->get_symbol());
                         SgInitializedName* initalizedName = variableSymbol->get_declaration();
                         ROSE_ASSERT(initalizedName != NULL);

                         stencilInputInitializedNameList.push_back(initalizedName);
                       }
                      else
                       {
#if 0
                         printf ("Input to stencil declaration is not a SgVarRefExp: exp = %p = %s \n",exp,exp->class_name().c_str());
#endif
                       }
                  }
#if 0
               printf ("stencilInputExpressionList.size()      = %zu \n",stencilInputExpressionList.size());
               printf ("stencilInputInitializedNameList.size() = %zu \n",stencilInputInitializedNameList.size());
#endif
             }
        }

  // Find the stencil operator (variable)
     if (inheritedAttribute.get_StencilOperatorDeclaration() == true)
        {
       // Get the name of the stencil operator.
#if 0
          printf ("case inheritedAttribute.get_StencilOperatorDeclaration() == true: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

          SgInitializedName* initializedName = isSgInitializedName(astNode);
          if (initializedName != NULL)
             {
            // Save the SgInitializedName (has type StencilOperator, and stencil operator is triggered by StencilOperator::operator()).
               initializedNameList.push_back(initializedName);
             }
        }

  // Find the invocation of the stencil operator (via a SgFunctionCallExp).
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
     if (functionCallExp != NULL)
        {
       // Here we make assumptions on how the stencil is specified in the DSL.
          SgDotExp* dotExp = isSgDotExp(functionCallExp->get_function());
          if (dotExp != NULL)
             {
               SgVarRefExp* varRefExp = isSgVarRefExp(dotExp->get_lhs_operand());
               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(dotExp->get_rhs_operand());

               bool stencilOperatorFound = false;
               if (varRefExp != NULL && memberFunctionRefExp != NULL)
                  {
                 // if (initializedNameList.find(initializedName) != initializedNameList.end())
                    SgVariableSymbol* variableSymbol = isSgVariableSymbol(varRefExp->get_symbol());
                    SgInitializedName* initializedName = variableSymbol->get_declaration();
                    ROSE_ASSERT(initializedName != NULL);

                    SgMemberFunctionSymbol* memberFunctionSymbol = memberFunctionRefExp->get_symbol();

                 // Check that this is the correct member function on the correctly typed variable reference.
#if 0
                    stencilOperatorFound = (find(initializedNameList.begin(),initializedNameList.end(),initializedName) != initializedNameList.end()) && (memberFunctionSymbol->get_name() == "operator()");
#else
                    if (find(initializedNameList.begin(),initializedNameList.end(),initializedName) != initializedNameList.end())
                       {
                      // Found an expression using the StencilOperator (need to check if this is the operator()).
                      // inheritedAttribute.set_StencilOperatorRef(true);
                         stencilOperatorFound = true;
                       }

                    stencilOperatorFound = stencilOperatorFound && (memberFunctionSymbol->get_name() == "operator()");
#endif
                  }

               if (stencilOperatorFound == true)
                  {
                 // This is the location of the operator where the stencil is to be put in the transformed code.
                    stencilOperatorFunctionCallList.push_back(functionCallExp);
#if 0
                    printf ("Found stencil location: functionCallExp = %p \n",functionCallExp);
                    functionCallExp->get_file_info()->display("location of stencil operator use: debug");
#endif
                  }
             }
        }

  // Construct the return attribute from the modified input attribute.
     return Detection_InheritedAttribute(inheritedAttribute);
   }


Detection_SynthesizedAttribute
DetectionTraversal::evaluateSynthesizedAttribute (SgNode* astNode, Detection_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     Detection_SynthesizedAttribute return_synthesizedAttribute;

  // This function can't be used to find the variable declaration for the variables used in the stencil constructor.

#if 0
     printf ("In evaluateSynthesizedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

#if 0
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(astNode);
     if (variableDeclaration != NULL)
        {
       // Get the SgInitializedName from the SgVariableDeclaration.
          SgInitializedName* initializedName = SageInterface::getFirstInitializedName(variableDeclaration);
#if 0
          printf ("In evaluateSynthesizedAttribute(): case SgInitializedName from variable declaration: initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
#endif
          if (find(stencilInputInitializedNameList.begin(),stencilInputInitializedNameList.end(),initializedName) != stencilInputInitializedNameList.end())
             {
               printf ("Found declaration associated with stencil input: initializedName = %p = %s name = %s \n",initializedName,initializedName->class_name().c_str(),initializedName->get_name().str());

               printf ("I think it is not possible to find this using the same traversal (need to use a 2nd traversal) \n");
               ROSE_ASSERT(false);
             }
        }
#endif

#if 0
  // This might not be the correct approach.
     SgInitializedName* initializedName = isSgInitializedName(astNode);
     if (initializedName != NULL)
        {
#if 1
          printf ("In evaluateSynthesizedAttribute(): case SgInitializedName: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
          if (find(stencilInputInitializedNameList.begin(),stencilInputInitializedNameList.end(),initializedName) != stencilInputInitializedNameList.end())
             {
               printf ("Found declaration associated with stencil input: initializedName = %p = %s name = %s \n",initializedName,initializedName->class_name().c_str(),initializedName->get_name().str());
             }
        }
#endif

     return return_synthesizedAttribute;
   }

