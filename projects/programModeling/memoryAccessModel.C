#include "rose.h"
#include "memoryAccessModel.h"

using namespace std;
using namespace rose;

MemoryAccessAttributeAttachmentInheritedAttribute::
MemoryAccessAttributeAttachmentInheritedAttribute () 
   : memoryAccess(false)
   {
   }

MemoryAccessAttributeAttachmentInheritedAttribute::
MemoryAccessAttributeAttachmentInheritedAttribute( const MemoryAccessAttributeAttachmentInheritedAttribute & X ) 
   : memoryAccess(X.memoryAccess)
   {
   }

MemoryAccessAttributeAttachmentSynthesizedAttribute::
MemoryAccessAttributeAttachmentSynthesizedAttribute()
   : numberOfAccesses(0)
   {
   }

// Functions required by the tree traversal mechanism
MemoryAccessAttributeAttachmentInheritedAttribute
MemoryAccessAttributeAttachmentTraversal::evaluateInheritedAttribute (
     SgNode* astNode,
     MemoryAccessAttributeAttachmentInheritedAttribute inheritedAttribute )
   {
  // Things not counted:
  //    pointer dereference operations
  //    structure data member access


#if 1
     static int traversalNodeCounter = 0;
     printf ("!!!!! In evaluateInheritedAttribute() \n");
     printf ("     (traversalNodeCounter=%d) astNode->sage_class_name() = %s \n",
          traversalNodeCounter++,astNode->sage_class_name());
     printf ("     astNode->unparseToString() = %s \n",astNode->unparseToString().c_str());
#endif

#if 0
     vector<string> successorNames = astNode->get_traversalSuccessorNamesContainer();
     printf ("     #of successors = %zu successorNames = ",successorNames.size());
     for (vector<string>::iterator s = successorNames.begin(); s != successorNames.end(); s++)
        {
           printf ("%s ",(*s).c_str());
        }
     printf ("\n");
#endif

     switch(astNode->variantT())
        {
          case V_SgValueExp:
          case V_SgBoolValExp:
          case V_SgStringVal:
          case V_SgShortVal:
          case V_SgCharVal:
          case V_SgUnsignedCharVal:
          case V_SgWcharVal:
          case V_SgUnsignedShortVal:
          case V_SgIntVal:
          case V_SgEnumVal:
          case V_SgUnsignedIntVal:
          case V_SgLongIntVal:
          case V_SgLongLongIntVal:
          case V_SgUnsignedLongLongIntVal:
          case V_SgUnsignedLongVal:
          case V_SgFloatVal:
          case V_SgDoubleVal:
          case V_SgLongDoubleVal:
             {
            // printf ("Found a SgValueExp object (attaching register access modeling attribute) \n");

            // Build an attribute (on the heap)
               AstAttribute* accessAttribute = new AccessModelAttribute(AccessModelAttribute::registerAccess);
               ROSE_ASSERT(accessAttribute != NULL);

            // Add it to the AST (so it can be found later in another pass over the AST)
#if 0
               if (astNode->get_attribute() == NULL)
                  {
                    AstAttributeMechanism* attributePtr = new AstAttributeMechanism();
                    ROSE_ASSERT(attributePtr != NULL);
                    astNode->set_attribute(attributePtr);
                  }
#endif
            // astNode->attribute.add("RegisterMemoryAccessAttribute",accessAttribute);
            // astNode->attribute().add("RegisterMemoryAccessAttribute",accessAttribute);
               astNode->addNewAttribute("RegisterMemoryAccessAttribute",accessAttribute);
               inheritedAttribute.memoryAccess = false;
               break;
             }

          case V_SgVarRefExp:
             {
               printf ("Found a SgVarRefExp object (attaching scalar access modeling attribute) \n");

            // By default mark this as a scalar access
               AccessModelAttribute::AccessTypeEnum assessType = AccessModelAttribute::scalarAccess;

            // If this is the parent of an array reference (SgPntrArrRefExp) then mark this as an array base pointer
               SgPntrArrRefExp* arrayRefExp = isSgPntrArrRefExp(astNode->get_parent());
               if (arrayRefExp != NULL && arrayRefExp->get_lhs_operand() == astNode)
                  {
                 // This helps trap the case of "a[0]" and ignore the case of "a[n]",
                 // so that only the "a" will be marked a a base pointer address.
                    assessType = AccessModelAttribute::arrayBasePointerAccess;
                  }

            // Build an attribute (on the heap)
               AstAttribute* accessAttribute = new AccessModelAttribute(assessType);
               ROSE_ASSERT(accessAttribute != NULL);

            // Add it to the AST (so it can be found later in another pass over the AST)
            // astNode->attribute.add("MemoryAccessAttribute",accessAttribute);
#if 0
               if (astNode->get_attribute() == NULL)
                  {
                    AstAttributeMechanism* attributePtr = new AstAttributeMechanism();
                    ROSE_ASSERT(attributePtr != NULL);
                    astNode->set_attribute(attributePtr);
                  }
#endif
            // astNode->attribute.add("ScalarMemoryAccessAttribute",accessAttribute);
            // astNode->attribute().add("ScalarMemoryAccessAttribute",accessAttribute);
               astNode->addNewAttribute("ScalarMemoryAccessAttribute",accessAttribute);

            // The inherited attribute redundently stores the present of a memory access.
            // This might not be a strong point of the design :-).
               inheritedAttribute.memoryAccess = true;
               break;
             }

          case V_SgPntrArrRefExp:
             {
               printf ("Found a SgPntrArrRefExp object (attaching array access modeling attribute) \n");

            // Build an attribute (on the heap)
               AstAttribute* accessAttribute = new AccessModelAttribute(AccessModelAttribute::arrayAccess);
               ROSE_ASSERT(accessAttribute != NULL);

            // Add it to the AST (so it can be found later in another pass over the AST)
#if 0
               if (astNode->get_attribute() == NULL)
                  {
                    AstAttributeMechanism* attributePtr = new AstAttributeMechanism();
                    ROSE_ASSERT(attributePtr != NULL);
                    astNode->set_attribute(attributePtr);
                  }
#endif
            // astNode->attribute.add("ArrayMemoryAccessAttribute",accessAttribute);
            // astNode->attribute().add("ArrayMemoryAccessAttribute",accessAttribute);
               astNode->addNewAttribute("ArrayMemoryAccessAttribute",accessAttribute);

            // The inherited attribute redundently stores the present of a memory access.
            // This might not be a strong point of the design :-).
               inheritedAttribute.memoryAccess = true;
               break;
             }

          default:
             {
            // This is not required since the value is initialized for false in the constructor
               inheritedAttribute.memoryAccess = false;
             }
        }

     return inheritedAttribute;
   }

MemoryAccessAttributeAttachmentSynthesizedAttribute
MemoryAccessAttributeAttachmentTraversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     MemoryAccessAttributeAttachmentInheritedAttribute inheritedAttribute,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
  // This function progates the memory access attribute information up the AST
     MemoryAccessAttributeAttachmentSynthesizedAttribute returnAttribute;

  // printf ("In evaluateSynthesizedAttribute (astNode = %p = %s) \n",astNode,astNode->class_name().c_str());

     if (inheritedAttribute.memoryAccess == true)
        {
       // Found a memory access
          returnAttribute.numberOfAccesses = 1;
        }

     SubTreeSynthesizedAttributes::iterator childIndex = synthesizedAttributeList.begin();
     while ( childIndex != synthesizedAttributeList.end() )
        {
       // For scope statements handle special cases
          switch(astNode->variantT())
             {
            // It does not make since to propagate memory access sums structurally beyond some points in the AST
            // This whole summary process should really happen on the control flow graph (and better yet on a 
            // contex sensitive control flow graph).
               case V_SgProject:
               case V_SgFile:
               case V_SgGlobal:
               case V_SgClassDeclaration:
               case V_SgTemplateDeclaration:
               case V_SgTemplateInstantiationDecl:
               case V_SgNamespaceDeclarationStatement:
               case V_SgNamespaceAliasDeclarationStatement:
               case V_SgUsingDeclarationStatement:
               case V_SgUsingDirectiveStatement:
               case V_SgTemplateInstantiationDirectiveStatement:
                  {
                    returnAttribute.numberOfAccesses         = 0;
                    returnAttribute.equationString           = "";
                    returnAttribute.simplifiedEquationString = "";
                    break;
                  }

            // This should generate a piecewise function (later), for now just generate max of true and false blocks
               case V_SgIfStmt:
                  {
                    int memoryAccessesTrueBlock  = synthesizedAttributeList[SgIfStmt_true_body ].numberOfAccesses; 
                    int memoryAccessesFalseBlock = synthesizedAttributeList[SgIfStmt_false_body].numberOfAccesses;
                    int maxMemoryAccesses = memoryAccessesTrueBlock > memoryAccessesTrueBlock ? 
                                            memoryAccessesTrueBlock : memoryAccessesFalseBlock;
                    returnAttribute.numberOfAccesses = maxMemoryAccesses;
                    break;
                  }

               case V_SgSwitchStatement:
                  {
                    printf ("Sorry, not implemented: Memory access summaries not passed beyond switch statements \n");
                    returnAttribute.numberOfAccesses         = 0;
                    returnAttribute.equationString           = "";
                    returnAttribute.simplifiedEquationString = "";
                    break;
                  }

               case V_SgMemberFunctionRefExp:
                  {
                    printf ("Member function calls must be handled the same as function calls, not implemented! \n");
                    ROSE_ASSERT(false);
                    break;
                  }

               default:
                  {
                 // Sum the accesses from child nodes
                    returnAttribute.numberOfAccesses         += (*childIndex).numberOfAccesses;
                    returnAttribute.equationString           += (*childIndex).equationString;
                    returnAttribute.simplifiedEquationString += (*childIndex).simplifiedEquationString;
                  }
             }

          childIndex++;
        }

  // Save the number of accesses in a summary node and attach it to specfic scopes
     switch(astNode->variantT())
        {
       // We might want to do this for every scope instead of just for each function definition
       // case V_SgScopeStatement:
          case V_SgBasicBlock:
          case V_SgIfStmt:
          case V_SgWhileStmt:
          case V_SgDoWhileStmt:
          case V_SgSwitchStatement:
          case V_SgCatchOptionStmt:
          case V_SgFunctionDefinition:
             {
#if 0
               printf ("\n\nFound a relavant SgScopeStatement = %p = %s object (attaching a summary of access models %d as attribute) \n",
                    astNode,astNode->class_name().c_str(),returnAttribute.numberOfAccesses);
#endif
            // Build an attribute (on the heap)
            // AstAttribute* accessSummaryAttribute = new AccessModelSummaryAttribute();
               AccessModelSummaryAttribute* accessSummaryAttribute = new AccessModelSummaryAttribute();
               ROSE_ASSERT(accessSummaryAttribute != NULL);

            // printf ("returnAttribute.equationString = \n%s \n",returnAttribute.equationString.c_str());

               const string statementPlaceholder  = "statement_equation";
               const string rhsSummaryPlaceholder = "$RHS";

               string statementEquationString = "statement_equation := $RHS; \n";

               string statementName           = "statement" + StringUtility::numberToString(astNode);

            // Now generate the equation to sum over all statements in this scope
            // string expressionSummaryEquation = StringUtility::numberToString(returnAttribute.numberOfAccesses);
               string rhsSumOfStatementEquation;

               switch(astNode->variantT())
                  {
                    case V_SgBasicBlock:
                       {
                         SgScopeStatement* scopeStatement = isSgScopeStatement(astNode);
                         ROSE_ASSERT(scopeStatement != NULL);
                         const SgStatementPtrList & statementList = scopeStatement->generateStatementList();
                         SgStatementPtrList::const_iterator i = statementList.begin();
                         while ( i != statementList.end() )
                            {
                           // generate equation that sums over all statement equations
                              rhsSumOfStatementEquation += "statement" + StringUtility::numberToString(*i);
                              i++;
                              if (i != statementList.end())
                                 {
                                   rhsSumOfStatementEquation += " + ";
                                 }
                            }
                         break;
                       }

                    case V_SgIfStmt:
                       {
                         SgIfStmt* statement = isSgIfStmt(astNode);
                         ROSE_ASSERT(statement != NULL);
                         rhsSumOfStatementEquation = "statement" + StringUtility::numberToString(statement->get_conditional());
                         if (statement->get_true_body() != NULL)
                              rhsSumOfStatementEquation += "statement" + StringUtility::numberToString(statement->get_true_body());
                         if (statement->get_false_body() != NULL)
                              rhsSumOfStatementEquation += "statement" + StringUtility::numberToString(statement->get_false_body());
                         break;
                       }
                     
                    case V_SgWhileStmt:
                       {
                         SgWhileStmt* statement = isSgWhileStmt(astNode);
                         ROSE_ASSERT(statement != NULL);
                         rhsSumOfStatementEquation = "statement" + StringUtility::numberToString(statement->get_body());
                         break;
                       }
                     
                    case V_SgDoWhileStmt:
                       {
                         SgDoWhileStmt* statement = isSgDoWhileStmt(astNode);
                         ROSE_ASSERT(statement != NULL);
                         rhsSumOfStatementEquation = "statement" + StringUtility::numberToString(statement->get_body());
                         break;
                       }
                     
                    case V_SgSwitchStatement:
                       {
                         SgSwitchStatement* statement = isSgSwitchStatement(astNode);
                         ROSE_ASSERT(statement != NULL);
                         rhsSumOfStatementEquation = "statement" + StringUtility::numberToString(statement->get_body());
                         break;
                       }
                     
                    case V_SgCatchOptionStmt:
                       {
                         SgCatchOptionStmt* statement = isSgCatchOptionStmt(astNode);
                         ROSE_ASSERT(statement != NULL);
                         rhsSumOfStatementEquation = "statement" + StringUtility::numberToString(statement->get_body());
                         break;
                       }
                     
                    case V_SgFunctionDefinition:
                       {
                         SgFunctionDefinition* statement = isSgFunctionDefinition(astNode);
                         ROSE_ASSERT(statement != NULL);
                         rhsSumOfStatementEquation = "statement" + StringUtility::numberToString(statement->get_body());
                         break;
                       }

                    default:
                       {
                         printf ("Default reached in switch \n");
                         ROSE_ASSERT(false);
                       }
                  }

               string & s = statementEquationString;
               s.replace(s.find(rhsSummaryPlaceholder),rhsSummaryPlaceholder.length(),rhsSumOfStatementEquation);

               string listOfStatementEquationsForScope = returnAttribute.equationString;

            // prepend the equations for each statement in the scope
               statementEquationString = listOfStatementEquationsForScope + statementEquationString;

#if 0
               printf ("statementEquationString = \n%s \n",statementEquationString.c_str());
               printf ("Exiting as part of test ... \n");
               ROSE_ASSERT(false);
#endif
            // Call Maple to simplify the equations
               string simplifiedEquationString = AccessModeling::simplifyEquation ( statementEquationString );

            // Substitute equation name with new unique name base on this SgForStatement.
            // If we had done so before then the Maple script would have to had been 
            // modified for each equatuion name.
               statementEquationString.replace (statementEquationString.find (statementPlaceholder),statementPlaceholder.length(),statementName);
               simplifiedEquationString.replace(simplifiedEquationString.find(statementPlaceholder),statementPlaceholder.length(),statementName);

            // printf ("returnAttribute.simplifiedEquationString = \n%s \n",returnAttribute.simplifiedEquationString.c_str());

            // Store the total memory accesses in the summary node
               accessSummaryAttribute->numberOfAccesses         = returnAttribute.numberOfAccesses;
               accessSummaryAttribute->equationString           = statementEquationString;
               accessSummaryAttribute->simplifiedEquationString = simplifiedEquationString;
#if 0
               printf ("(scope) numberOfAccesses         = %d   \n",accessSummaryAttribute->numberOfAccesses);
               printf ("(scope) statementEquationString  = \n%s \n",accessSummaryAttribute->equationString.c_str());
               printf ("(scope) simplifiedEquationString = \n%s \n",accessSummaryAttribute->simplifiedEquationString.c_str());
#endif
            // Set the return attribute so that the equations will be accumulated
               returnAttribute.equationString           = accessSummaryAttribute->equationString;
               returnAttribute.simplifiedEquationString = accessSummaryAttribute->simplifiedEquationString;

            // Add it to the AST (so it can be found later in another pass over the AST)
#if 0
               if (astNode->get_attribute() == NULL)
                  {
                    AstAttributeMechanism* attributePtr = new AstAttributeMechanism();
                    ROSE_ASSERT(attributePtr != NULL);
                    astNode->set_attribute(attributePtr);
                  }
#endif
            // astNode->attribute.add("MemoryAccessSummaryAttribute",accessSummaryAttribute);
            // astNode->attribute().add("MemoryAccessSummaryAttribute",accessSummaryAttribute);
               astNode->addNewAttribute("MemoryAccessSummaryAttribute",accessSummaryAttribute);
               break;
             }

          case V_SgGlobal:
          case V_SgClassDefinition:
          case V_SgTemplateInstantiationDefn:
          case V_SgNamespaceDefinitionStatement:
             {
            // Ignore this case since we can't know what functions are called from the these sorts of scopes
            // (consider that the main function forms a root summary node since it is the start of the control flow).
               break;
             }

          case V_SgForStatement:
             {
            // This is the equation generated for the SgForStatement.  Note that all loop variables are given 
            // special names, "loop_expression" is used a a prefix.
#if 1
            // printf ("Case SgForStatement: Memory access summaries computed symbolically (total static memory accesses = %d) \n",returnAttribute.numberOfAccesses);
               printf ("\n\nCase SgForStatement = %p: Memory accesses in initializer = %d, condition = %d, increment = %d, body = %d (static total = %d) \n",
                    astNode,
                    synthesizedAttributeList[SgForStatement_for_init_stmt].numberOfAccesses,
                 // DQ (11/28/2005): The interface to SgForStatement has been changed to allow for tests to be a SgStatement (declaration)
                 // synthesizedAttributeList[SgForStatement_test_expr_root].numberOfAccesses,
                    synthesizedAttributeList[SgForStatement_test].numberOfAccesses,
                    synthesizedAttributeList[SgForStatement_increment].numberOfAccesses,
                    synthesizedAttributeList[SgForStatement_loop_body].numberOfAccesses,
                    returnAttribute.numberOfAccesses);
#endif
               SgForStatement* forStatement = isSgForStatement(astNode);
               ROSE_ASSERT(forStatement != NULL);

            // We call the Maple interface to generate a symbolic representation of the equation 
            // within a separate traversal (ProgramModeling::computeModelEquations).

            // Express the equation
               const string loopPlaceholder        = "statement_equation";
               const string bodyPlaceholder        = "$BODY";
               const string initializerPlaceholder = "$INITIALIZER";
               const string conditionalPlaceholder = "$CONDITION";
               const string incrementPlaceholder   = "$INCREMENT";
               const string indexPlaceholder       = "$INDEX";
               const string basePlaceholder        = "$BASE";
               const string boundPlaceholder       = "$BOUND";

            // Where the loop bounds can be determined we translate this to the Maple sum notation
            // for "for(i=b; i op n; i +=s) body(i)" the sum is "sum(body(b+sj),j=0..(n-b)/s)"
               string loopTripEquationString = "statement_equation := $INITIALIZER + sum($CONDITION+$BODY+$INCREMENT,$INDEX=$BASE..$BOUND); \n";

               string loop        = "statement"   + StringUtility::numberToString(forStatement);

            // This should be set to the symbol used for the index variable
            // string index       = "index"       + StringUtility::numberToString(forStatement->get_for_init_stmt());
               string initializer = "statement"   + StringUtility::numberToString(forStatement->get_for_init_stmt());

            // This should be set to the initializer of the SgForInitStmt (later)
            // string base        = "base"           + StringUtility::numberToString(forStatement->get_test_expr());
            // string base        = "expression_value" + StringUtility::numberToString(binaryOp->get_lhs_operand());
            // SgForInitStatement* forInitStatement = forStatement->get_for_init_stmt();
               SgStatementPtrList & forInitList = forStatement->get_init_stmt();
               ROSE_ASSERT(forInitList.size() == 1);
               SgStatement* initStatement = *(forInitList.begin());
               ROSE_ASSERT(initStatement != NULL);
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(initStatement);
               ROSE_ASSERT(variableDeclaration != NULL);
               ROSE_ASSERT(variableDeclaration->get_variables().size() == 1);
               SgInitializedName* initializedName = *(variableDeclaration->get_variables().begin());
               ROSE_ASSERT(initializedName != NULL);
            // printf ("for loop index name = %s \n",initializedName->get_name().str());
               SgInitializer* initializerExp = initializedName->get_initptr();
               ROSE_ASSERT(initializerExp != NULL);
               SgAssignInitializer* assignInitializer = isSgAssignInitializer(initializerExp);
               ROSE_ASSERT(assignInitializer != NULL);
               SgExpression* baseExpression = isSgExpression(assignInitializer->get_operand());
               ROSE_ASSERT(baseExpression != NULL);
            // string base = "base_bound_expression" + StringUtility::numberToString(baseExpression);
               string base = "loop_expression" + StringUtility::numberToString(baseExpression);
               string baseEquation = AccessModeling::generateBaseBoundEquation (baseExpression);
            // printf ("After generateBaseBoundEquation ... base = %s baseEquation = %s \n",base.c_str(),baseEquation.c_str());

#if 0
               printf ("Exiting ... base = %s baseEquation = %s \n",base.c_str(),baseEquation.c_str());
               ROSE_ASSERT(false);
#endif
            // This code is assuming that the conditional is a binary operator and 
            // that the loop bound is on the rhs in the binary operator.  Also, we want the expression value
            // and not the equation representing the model's count!
               SgBinaryOp* binaryOp = isSgBinaryOp(forStatement->get_test_expr());
               ROSE_ASSERT(binaryOp != NULL);
            // string bound       = "bound"       + StringUtility::numberToString(forStatement->get_test_expr());
               ROSE_ASSERT(isSgVarRefExp(binaryOp->get_lhs_operand()) != NULL);
            // ROSE_ASSERT(isSgVarRefExp(binaryOp->get_rhs_operand()) != NULL);
            // string bound       = "expression_value" + StringUtility::numberToString(binaryOp->get_rhs_operand());
            // string bound       = "expression" + StringUtility::numberToString(binaryOp->get_rhs_operand());
               SgVarRefExp* indexVarRefExp = isSgVarRefExp(binaryOp->get_lhs_operand());
               ROSE_ASSERT(indexVarRefExp->get_symbol() != NULL);
            // string index       = "expression" + StringUtility::numberToString(indexVarRefExp->get_symbol());
               string index       = "loop_expression" + StringUtility::numberToString(indexVarRefExp->get_symbol());
            // SgVarRefExp* boundVarRefExp = isSgVarRefExp(binaryOp->get_rhs_operand());
            // ROSE_ASSERT(boundVarRefExp != NULL);
            // ROSE_ASSERT(boundVarRefExp->get_symbol() != NULL);
            // string bound       = "expression" + StringUtility::numberToString(boundVarRefExp->get_symbol());
               SgExpression* boundExpression = binaryOp->get_rhs_operand();
            // string bound       = "base_bound_expression" + StringUtility::numberToString(boundExpression);
               string bound       = "loop_expression" + StringUtility::numberToString(boundExpression);
               string boundEquation = AccessModeling::generateBaseBoundEquation(boundExpression);

               printf ("baseEquation  = %s \n",baseEquation.c_str());
               printf ("boundEquation = %s \n",boundEquation.c_str());

            // This will later be a statement! See todo list in SgForStatement
            // string conditional = "conditional" + StringUtility::numberToString(forStatement->get_test_expr());
               string conditional = "expression"  + StringUtility::numberToString(forStatement->get_test());
               string increment   = "expression"  + StringUtility::numberToString(forStatement->get_increment());

            // Note that the body is a statement and we have to get the Maple variable name right
               string body        = "statement"   + StringUtility::numberToString(forStatement->get_loop_body());

            // Change the name to make the code more managable
               string & s = loopTripEquationString;

               s.replace(s.find(indexPlaceholder),indexPlaceholder.length(),index);
               s.replace(s.find(initializerPlaceholder),initializerPlaceholder.length(),initializer);
               s.replace(s.find(basePlaceholder),basePlaceholder.length(),base);
               s.replace(s.find(boundPlaceholder),boundPlaceholder.length(),bound);
               s.replace(s.find(conditionalPlaceholder),conditionalPlaceholder.length(),conditional);
               s.replace(s.find(incrementPlaceholder),incrementPlaceholder.length(),increment);
               s.replace(s.find(bodyPlaceholder),bodyPlaceholder.length(),body);

            // printf ("returnAttribute.equationString = \n%s \n",returnAttribute.equationString.c_str());

            // Include any collected equations from the children (loop body)
            // Add the base equation so to handle case of for loop initializer being a value or a variable
               loopTripEquationString = baseEquation + boundEquation + returnAttribute.equationString + loopTripEquationString;

               printf ("loopTripEquationString = \n%s \n\n",loopTripEquationString.c_str());

            // Call Maple to simplify the equations
               string simplifiedLoopTripEquationString = AccessModeling::simplifyEquation ( loopTripEquationString );

            // Substitute equation name with new unique name base on this SgForStatement.
            // If we had done so before then the Maple script would have to had been 
            // modified for each equatuion name.
               loopTripEquationString.replace(loopTripEquationString.find(loopPlaceholder),loopPlaceholder.length(),loop);
               simplifiedLoopTripEquationString.replace(simplifiedLoopTripEquationString.find(loopPlaceholder),loopPlaceholder.length(),loop);

            // printf ("loopTripEquationString           = %s \n",loopTripEquationString.c_str());
            // printf ("simplifiedLoopTripEquationString = %s \n",simplifiedLoopTripEquationString.c_str());

            // Build a summary attribute
               AccessModelSummaryAttribute* accessSummaryAttribute = new AccessModelSummaryAttribute();
               ROSE_ASSERT(accessSummaryAttribute != NULL);

            // Store the total memory accesses in the summary node
               accessSummaryAttribute->numberOfAccesses         = returnAttribute.numberOfAccesses;
               accessSummaryAttribute->equationString           = loopTripEquationString;
               accessSummaryAttribute->simplifiedEquationString = simplifiedLoopTripEquationString;
#if 1
               printf ("(for loop) numberOfAccesses         = %d   \n",accessSummaryAttribute->numberOfAccesses);
               printf ("(for loop) statementEquationString  = \n%s \n",accessSummaryAttribute->equationString.c_str());
               printf ("(for loop) simplifiedEquationString = \n%s \n",accessSummaryAttribute->simplifiedEquationString.c_str());
#endif
            // Set the return attribute so that the equations will be accumulated
               returnAttribute.equationString           = accessSummaryAttribute->equationString;
               returnAttribute.simplifiedEquationString = accessSummaryAttribute->simplifiedEquationString;

            // Add it to the AST (so it can be found later in another pass over the AST)
#if 0
               if (astNode->get_attribute() == NULL)
                  {
                    AstAttributeMechanism* attributePtr = new AstAttributeMechanism();
                    ROSE_ASSERT(attributePtr != NULL);
                    astNode->set_attribute(attributePtr);
                  }
#endif
            // astNode->attribute.add("MemoryAccessSummaryAttribute",accessSummaryAttribute);
            // astNode->attribute().add("MemoryAccessSummaryAttribute",accessSummaryAttribute);
               astNode->addNewAttribute("MemoryAccessSummaryAttribute",accessSummaryAttribute);
#if 0
               printf ("Exiting at base of for loop equation generation for SgForStatement \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

         case V_SgExprStatement:
             {
            // For expressions we don't want to call Maple (wait until we get to the statement and then summarize the equations there)
               SgExprStatement* exprStatement = isSgExprStatement(astNode);
               ROSE_ASSERT(exprStatement != NULL);

            // Handle this case specially
               AccessModeling::buildEquation(exprStatement,returnAttribute);

#if 1
               printf ("\n\nAt bottom of evaluateSynthesizedAttribute (astNode = %p = %s) \n",astNode,astNode->class_name().c_str());
               printf ("(SgExprStatement) returnAttribute.numberOfAccesses         = %d   \n",returnAttribute.numberOfAccesses);
               printf ("(SgExprStatement) returnAttribute.equationString           = \n%s \n",returnAttribute.equationString.c_str());
               printf ("(SgExprStatement) returnAttribute.simplifiedEquationString = \n%s \n",returnAttribute.simplifiedEquationString.c_str());
#endif
#if 0
               printf ("Exiting at base of for loop equation generation for SgExprStatement \n");
               ROSE_ASSERT(false);
#endif
               break;
            }

          case V_SgClassDeclaration:
          case V_SgMemberFunctionDeclaration:
          case V_SgFunctionDeclaration:
          case V_SgEnumDeclaration:
          case V_SgAsmStmt:
          case V_SgTypedefDeclaration:
          case V_SgPragmaDeclaration:
          case V_SgTemplateDeclaration:
          case V_SgTemplateInstantiationDecl:
          case V_SgTemplateInstantiationFunctionDecl:
          case V_SgTemplateInstantiationMemberFunctionDecl:
          case V_SgNamespaceDeclarationStatement:
          case V_SgNamespaceAliasDeclarationStatement:
          case V_SgUsingDeclarationStatement:
          case V_SgUsingDirectiveStatement:
          case V_SgTemplateInstantiationDirectiveStatement:
             {
            // Ignore thes cases since we can't know what functions are called these declarations.
               break;
             }

          case V_SgGotoStatement:
          case V_SgSpawnStmt:
          case V_SgClinkageStartStatement:
          case V_SgContinueStmt:
          case V_SgCatchStatementSeq:
          case V_SgFunctionTypeTable:
          case V_SgLabelStatement:
          case V_SgBreakStmt:
             {
            // Ignore these cases since we can't know what functions are called these statements.
               break;
             }

          case V_SgFunctionParameterList:
             {
            // Ignore this cases we don't call function here or execute any other code.
            // NOTE: What about default initializations???
               break;
             }

       // case V_SgStatement:
          case V_SgForInitStatement:
          case V_SgCaseOptionStmt:
          case V_SgTryStmt:
          case V_SgDefaultOptionStmt:
          case V_SgReturnStmt:

       // case V_SgDeclarationStatement:
          case V_SgCtorInitializerList:
          case V_SgVariableDeclaration:
          case V_SgVariableDefinition:
             {
#if 0
               printf ("\n\nFound a relavant SgStatement = %p = %s object (attaching a summary of access models %d as attribute) \n",
                    astNode,astNode->class_name().c_str(),returnAttribute.numberOfAccesses);
#endif
            // Build an attribute (on the heap)
            // AstAttribute* accessSummaryAttribute = new AccessModelSummaryAttribute();
               AccessModelSummaryAttribute* accessSummaryAttribute = new AccessModelSummaryAttribute();
               ROSE_ASSERT(accessSummaryAttribute != NULL);

            // Store the total memory accesses in the summary node
               accessSummaryAttribute->numberOfAccesses = returnAttribute.numberOfAccesses;

               const string statementPlaceholder  = "statement_equation";
               const string rhsSummaryPlaceholder = "$RHS";

               string statementEquationString = "statement_equation := $RHS; \n";

               string statementName             = "statement" + StringUtility::numberToString(astNode);
               string expressionSummaryEquation = StringUtility::numberToString(returnAttribute.numberOfAccesses);

               string & s = statementEquationString;
               s.replace(s.find(rhsSummaryPlaceholder),rhsSummaryPlaceholder.length(),expressionSummaryEquation);

            // string equationString = statementEquationString;

            // Call Maple to simplify the equations
               string simplifiedEquationString = AccessModeling::simplifyEquation ( statementEquationString );

            // Substitute equation name with new unique name base on this SgForStatement.
            // If we had done so before then the Maple script would have to had been 
            // modified for each equatuion name.
               statementEquationString.replace (statementEquationString.find (statementPlaceholder),statementPlaceholder.length(),statementName);
               simplifiedEquationString.replace(simplifiedEquationString.find(statementPlaceholder),statementPlaceholder.length(),statementName);

            // Store the total memory accesses in the summary node
               accessSummaryAttribute->numberOfAccesses         = returnAttribute.numberOfAccesses;
               accessSummaryAttribute->equationString           = statementEquationString;
               accessSummaryAttribute->simplifiedEquationString = simplifiedEquationString;
#if 0
               printf ("(statement) numberOfAccesses         = %d \n",accessSummaryAttribute->numberOfAccesses);
               printf ("(statement) statementEquationString  = \n%s \n",accessSummaryAttribute->equationString.c_str());
               printf ("(statement) simplifiedEquationString = \n%s \n",accessSummaryAttribute->simplifiedEquationString.c_str());
#endif
            // Set the return attribute so that the equations will be accumulated
               returnAttribute.equationString           = accessSummaryAttribute->equationString;
               returnAttribute.simplifiedEquationString = accessSummaryAttribute->simplifiedEquationString;

            // Add it to the AST (so it can be found later in another pass over the AST)
#if 0
               if (astNode->get_attribute() == NULL)
                  {
                    AstAttributeMechanism* attributePtr = new AstAttributeMechanism();
                    ROSE_ASSERT(attributePtr != NULL);
                    astNode->set_attribute(attributePtr);
                  }
#endif
            // astNode->attribute().add("MemoryAccessSummaryAttribute",accessSummaryAttribute);
               astNode->addNewAttribute("MemoryAccessSummaryAttribute",accessSummaryAttribute);
               break;
             }

          case V_SgVarRefExp:
             {
            // For expressions we don't want to call Maple (wait until we get to the statement and then summarize the equations there)
               SgVarRefExp* varRefExp = isSgVarRefExp(astNode);
               ROSE_ASSERT(varRefExp != NULL);

            // Handle this case specially
               AccessModeling::buildEquation(varRefExp,returnAttribute);
               break;
            }

          case V_SgFunctionRefExp:
             {
            // For expressions we don't want to call Maple (wait until we get to the statement and then summarize the equations there)
               SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(astNode);
               ROSE_ASSERT(functionRefExp != NULL);

            // Handle this case specially
               AccessModeling::buildEquation(functionRefExp,returnAttribute);

#if 0
               printf ("Exiting at base of for loop equation generation for SgFunctionRefExp \n");
               ROSE_ASSERT(false);
#endif
               break;
            }

          case V_SgFunctionCallExp:
             {
            // For expressions we don't want to call Maple (wait until we get to the statement and then summarize the equations there)
               SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
               ROSE_ASSERT(functionCallExp != NULL);

            // Handle this case specially
               AccessModeling::buildEquation(functionCallExp,returnAttribute);

#if 0
               printf ("Exiting at base of for loop equation generation for SgFunctionCallExp \n");
               ROSE_ASSERT(false);
#endif
               break;
            }

          case V_SgExpression:
          case V_SgExprListExp:
       // case V_SgVarRefExp:
          case V_SgClassNameRefExp:
          case V_SgMemberFunctionRefExp:
          case V_SgSizeOfOp:
          case V_SgVarArgStartOp:
          case V_SgVarArgStartOneOperandOp:
          case V_SgVarArgOp:
          case V_SgVarArgEndOp:
          case V_SgVarArgCopyOp:
          case V_SgTypeIdOp:
          case V_SgConditionalExp:
          case V_SgNewExp:
          case V_SgDeleteExp:
          case V_SgThisExp:
          case V_SgRefExp:
             {
               SgExpression* expression = isSgExpression(astNode);
               ROSE_ASSERT(expression != NULL);
               AccessModeling::buildEquation(expression,returnAttribute);
               break;
             }

          case V_SgInitializer:
          case V_SgAggregateInitializer:
          case V_SgConstructorInitializer:
          case V_SgAssignInitializer:
             {
            // There is not required to be done here, I think.

            // printf ("Note: SgInitializer not handled \n");
            // ROSE_ASSERT(false);
               break;
             }

          case V_SgUnaryOp:
          case V_SgExpressionRoot:
          case V_SgMinusOp:
          case V_SgUnaryAddOp:
          case V_SgNotOp:
          case V_SgPointerDerefExp:
          case V_SgAddressOfOp:
          case V_SgMinusMinusOp:
          case V_SgPlusPlusOp:
          case V_SgBitComplementOp:
          case V_SgCastExp:
          case V_SgThrowOp:
             {
               SgUnaryOp* unaryOp = isSgUnaryOp(astNode);
               ROSE_ASSERT(unaryOp != NULL);
               AccessModeling::buildEquation(unaryOp,returnAttribute);
               break;
             }

          case V_SgBinaryOp:
          case V_SgDotExp:
          case V_SgDotStarOp:
          case V_SgArrowExp:
          case V_SgArrowStarOp:
          case V_SgEqualityOp:
          case V_SgLessThanOp:
          case V_SgGreaterThanOp:
          case V_SgNotEqualOp:
          case V_SgLessOrEqualOp:
          case V_SgGreaterOrEqualOp:
          case V_SgAddOp:
          case V_SgSubtractOp:
          case V_SgMultiplyOp:
          case V_SgDivideOp:
          case V_SgIntegerDivideOp:
          case V_SgModOp:
          case V_SgAndOp:
          case V_SgOrOp:
          case V_SgBitXorOp:
          case V_SgBitAndOp:
          case V_SgBitOrOp:
          case V_SgCommaOpExp:
          case V_SgLshiftOp:
          case V_SgRshiftOp:
          case V_SgPntrArrRefExp:
          case V_SgScopeOp:
          case V_SgAssignOp:
          case V_SgPlusAssignOp:
          case V_SgMinusAssignOp:
          case V_SgAndAssignOp:
          case V_SgIorAssignOp:
          case V_SgMultAssignOp:
          case V_SgDivAssignOp:
          case V_SgModAssignOp:
          case V_SgXorAssignOp:
          case V_SgLshiftAssignOp:
          case V_SgRshiftAssignOp:
             {
               SgBinaryOp* binaryOp = isSgBinaryOp(astNode);
               ROSE_ASSERT(binaryOp != NULL);
               AccessModeling::buildEquation(binaryOp,returnAttribute);
               break;
             }

          case V_SgValueExp:
          case V_SgBoolValExp:
          case V_SgStringVal:
          case V_SgShortVal:
          case V_SgCharVal:
          case V_SgUnsignedCharVal:
          case V_SgWcharVal:
          case V_SgUnsignedShortVal:
          case V_SgIntVal:
          case V_SgEnumVal:
          case V_SgUnsignedIntVal:
          case V_SgLongIntVal:
          case V_SgLongLongIntVal:
          case V_SgUnsignedLongLongIntVal:
          case V_SgUnsignedLongVal:
          case V_SgFloatVal:
          case V_SgDoubleVal:
          case V_SgLongDoubleVal:
             {
            // I don't think this has to be special (might not even be required for memory accesses, but is for mre general modeling)!
               SgExpression* expression = isSgExpression(astNode);
               ROSE_ASSERT(expression != NULL);
               AccessModeling::buildEquation(expression,returnAttribute);
               break;
             }

          default:
             {
            // Nothing to do here, unless we missed a SgStatement!
               if (isSgStatement(astNode) != NULL)
                  {
                    printf ("Error: default reached in switch for SgStatement = %s \n",astNode->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
                 else
                  {
                    SgExpression* expression = isSgExpression(astNode);
                    if (expression != NULL)
                       {
                         printf ("Error: default reached in switch for SgExpression = %s \n",astNode->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }
             }
        }

#if 1
     printf ("\n\nAt bottom of evaluateSynthesizedAttribute (astNode = %p = %s) \n",astNode,astNode->class_name().c_str());
     printf ("(bottom) returnAttribute.numberOfAccesses         = %d   \n",returnAttribute.numberOfAccesses);
     printf ("(bottom) returnAttribute.equationString           = \n%s \n",returnAttribute.equationString.c_str());
     printf ("(bottom) returnAttribute.simplifiedEquationString = \n%s \n",returnAttribute.simplifiedEquationString.c_str());
#endif

     return returnAttribute;
   }



string
AccessModeling::generateBaseBoundEquation ( SgExpression* expression )
   {
  // This function returns an equation that connect the lhs (a variable representing the input expression)
  // and the rhs.  The rhs can be either a SgValueExp or a reference to another variable (SgVarRefExp).
  // If it is more complex then just exit with an error.

     string equation;
     SgValueExp* valueExp = isSgValueExp(expression);
     if (valueExp != NULL)
        {
       // Handle case of "i = 42" in "for (int i = 42; i < bound; i++)"
          string valueString;
          switch(valueExp->variantT())
             {
               case V_SgIntVal:
                  {
                    SgIntVal* intVal = isSgIntVal(valueExp);
                    valueString = StringUtility::numberToString(intVal->get_value());
                    break;
                  }
                        
               default:
                  {
                    printf ("Default reached in switch \n");
                    ROSE_ASSERT(false);
                  }
             }

       // equation = "base_bound_expression" + StringUtility::numberToString(expression) + " := " + valueString + "; \n";
          equation = "loop_expression" + StringUtility::numberToString(expression) + " := " + valueString + "; \n";
        }
       else
        {
       // Handle case of "for (int i = base; i < bound; i++)".  Note that loop equations 
       // are always in terms of "loop_expression" index variable prefix
          SgVarRefExp* varRefExp = isSgVarRefExp(expression);
          if (varRefExp != NULL)
             {
            // Handle case of i = base (or i < n, where bound = n)
            // equation = "base_bound_expression" + StringUtility::numberToString(expression)
            //          + ":= expression" + StringUtility::numberToString(varRefExp->get_symbol()) + "; \n";
               equation = "loop_expression" + StringUtility::numberToString(expression)
                        + ":= loop_expression" + StringUtility::numberToString(varRefExp->get_symbol()) + "; \n";
             }
            else
             {
            // Don't build any equation in this case (the final symbolic equation refers to
            // the base or bound directly and that will be as simplified as possible (for now)).
               printf ("Error: not sure what this is (expression = %s) \n",expression->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

     return equation;
   }


void
AccessModeling::buildEquation( SgExprStatement* exprStatement, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute )
   {
     ROSE_ASSERT(exprStatement != NULL);
     ROSE_ASSERT(exprStatement->get_expression() != NULL);
     string expressionEquationString = "statement" 
                                     + StringUtility::numberToString(exprStatement) 
                                     + " := "
                                     + "expression" + StringUtility::numberToString(exprStatement->get_expression()) 
                                     + "; \n";

  // Don't bother calling Maple to simplify this equation
     string simplifiedEquationString = expressionEquationString;

     attribute.equationString           += expressionEquationString;
     attribute.simplifiedEquationString += simplifiedEquationString;
   }

void
AccessModeling::buildEquation( SgExpression* equationPointer, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute )
   {
     ROSE_ASSERT(equationPointer != NULL);
     string expressionEquationString = "expression" 
                                     + StringUtility::numberToString(equationPointer) 
                                     + " := " 
                                     + StringUtility::numberToString(attribute.numberOfAccesses) 
                                     + "; \n";

  // Don't bother calling Maple to simplify this equation
     string simplifiedEquationString = expressionEquationString;

     attribute.equationString           += expressionEquationString;
     attribute.simplifiedEquationString += simplifiedEquationString;
   }

void
AccessModeling::buildEquation( SgExprListExp* expressionListExp, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute )
   {
  // This generates the modeling equations for the function call parameters

     ROSE_ASSERT(expressionListExp != NULL);

     string expressionEquationString;

     SgExpressionPtrList & expressionList = expressionListExp->get_expressions();
     SgExpressionPtrList::iterator i = expressionList.begin();

  // Sum the contributions from all members of the expression list
     while ( i != expressionList.end() )
        {
       // Something to do at the start of the expression list
          if ( i == expressionList.begin() )
               expressionEquationString = "expression" + StringUtility::numberToString(expressionListExp) + " := ";

       // Append the term in the sumation over the elements in ths list
          expressionEquationString += "expression" + StringUtility::numberToString(*i);

       // increment to the next expression in the expression list
          i++;

       // Something to do for each element of the expression list and as for the last element.
          if ( i != expressionList.end() )
               expressionEquationString += " + ";
          else
               expressionEquationString += "; \n";
        }

  // Don't bother calling Maple to simplify this equation
     string simplifiedEquationString = expressionEquationString;

     attribute.equationString           += expressionEquationString;
     attribute.simplifiedEquationString += simplifiedEquationString;
   }

void
AccessModeling::buildEquation( SgVarRefExp* varRefExp, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute )
   {
  // Use varRefExp->get_symbol() so that the name will be a shared name (so that other equations will simplify)
     ROSE_ASSERT(varRefExp != NULL);
     ROSE_ASSERT(varRefExp->get_symbol() != NULL);
     string expressionEquationString = "expression"
                                     + StringUtility::numberToString(varRefExp->get_symbol())
                                     + " := "
                                     + StringUtility::numberToString(attribute.numberOfAccesses)
                                     + "; \n";

  // Need to build equation in terms of varRefExp, to access the equation in terms of varRefExp->get_symbol()
     expressionEquationString       += "expression"
                                     + StringUtility::numberToString(varRefExp)
                                     + " := "
                                     + "expression"
                                     + StringUtility::numberToString(varRefExp->get_symbol())
                                     + "; \n";

  // Don't bother calling Maple to simplify this equation
     string simplifiedEquationString = expressionEquationString;

  // printf ("BEFORE +=: attribute.equationString = %s \n",attribute.equationString.c_str());
     
     attribute.equationString           += expressionEquationString;
     attribute.simplifiedEquationString += simplifiedEquationString;
   }

void
AccessModeling::buildEquation( SgUnaryOp* unaryOp, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute )
   {
     ROSE_ASSERT(unaryOp != NULL);
     ROSE_ASSERT(unaryOp->get_operand() != NULL);
     string expressionEquationString = "expression"
                                     + StringUtility::numberToString(unaryOp)
                                     + " := "
                                     + "expression" + StringUtility::numberToString(unaryOp->get_operand())
                                     + "; \n";

  // Don't bother calling Maple to simplify this equation
     string simplifiedEquationString = expressionEquationString;

  // printf ("BEFORE +=: attribute.equationString = %s \n",attribute.equationString.c_str());

     attribute.equationString           += expressionEquationString;
     attribute.simplifiedEquationString += simplifiedEquationString;
   }

void
AccessModeling::buildEquation( SgBinaryOp* binaryOp, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute )
   {
     ROSE_ASSERT(binaryOp != NULL);
     ROSE_ASSERT(binaryOp->get_lhs_operand() != NULL);
     ROSE_ASSERT(binaryOp->get_rhs_operand() != NULL);
     string expressionEquationString = "expression"
                                     + StringUtility::numberToString(binaryOp)
                                     + " := "
                                     + "expression" + StringUtility::numberToString(binaryOp->get_lhs_operand())
                                     + " + "
                                     + "expression" + StringUtility::numberToString(binaryOp->get_rhs_operand())
                                     + "; \n";

  // Don't bother calling Maple to simplify this equation
     string simplifiedEquationString = expressionEquationString;

  // printf ("BEFORE +=: attribute.equationString = %s \n",attribute.equationString.c_str());

     attribute.equationString           += expressionEquationString;
     attribute.simplifiedEquationString += simplifiedEquationString;
   }


void
AccessModeling::buildEquation( SgFunctionCallExp* functionCallExp, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute )
   {
     ROSE_ASSERT(functionCallExp != NULL);
     ROSE_ASSERT(functionCallExp->get_function() != NULL);
     ROSE_ASSERT(functionCallExp->get_args() != NULL);
     string expressionEquationString = "expression"
                                     + StringUtility::numberToString(functionCallExp)
                                     + " := "
                                     + "expression" + StringUtility::numberToString(functionCallExp->get_function())
                                     + " + "
                                     + "expression" + StringUtility::numberToString(functionCallExp->get_args())
                                     + "; \n";

#if 0
  // Redone, commented out for now!
  // Build equations from the functions input parameter to the function call 
  // parameter (equations mapping function arguments to function parameters)

  // Am I mixing concepts of static data flow analysis inappropriately.
  // Might want to check for loop dependence information to trigger this,
  // else we should only generate some useless equations.
  // Goal is to permit user to define these so that abstractions can be declared 
  // to have a concept of size which could be used for iterations over arbitrary 
  // containers.  Need to map initializers to the variables that they initialize 
  // (same concept). Is this a start at symbolic computation, or just a form of 
  // constant propogation?  Symbolic constant propogation?

     SgExprListExp* expressionListExp = functionCallExp->get_args();
     ROSE_ASSERT(expressionListExp != NULL);
     SgExpressionPtrList & expressionList = expressionListExp->get_expressions();
     SgExpressionPtrList::iterator i = expressionList.begin();

  // Form the iterator over the function's parameter list
     SgInitializedNameList::iterator j = expressionList.begin();

  // Sum the contributions from all members of the expression list
     while ( i != expressionList.end() )
        {
       // Something to do at the start of the expression list
          if ( i == expressionList.begin() )
               expressionEquationString = "expression" + StringUtility::numberToString(expressionListExp) + " := ";

       // loop over the function arguments
          expressionEquationString += "expression"
                                   + StringUtility::numberToString(*i)
                                   + " := "
                                   + "expression"
                                   + StringUtility::numberToString(*j)
                                   + "; \n";

       // increment to the next expression in the expression list
          i++;

       // Something to do for each element of the expression list and as for the last element.
          if ( i != expressionList.end() )
               expressionEquationString += " + ";
          else
               expressionEquationString += "; \n";
        }
#endif

  // Don't bother calling Maple to simplify this equation
     string simplifiedEquationString = expressionEquationString;

  // printf ("BEFORE +=: attribute.equationString = %s \n",attribute.equationString.c_str());

     attribute.equationString           += expressionEquationString;
     attribute.simplifiedEquationString += simplifiedEquationString;
   }


void
AccessModeling::buildEquation( SgFunctionRefExp* functionRefExp, MemoryAccessAttributeAttachmentSynthesizedAttribute & attribute )
   {
  // Any function reference must retrive the equations for the function being called
  // from the function's definition (where they must have been previously summarized).
#if 1
     printf ("\n\nFound a relevant SgFunctionRefExp = %p object (attaching a summary of access models %d as attribute) \n",
          functionRefExp,attribute.numberOfAccesses);
#endif

     ROSE_ASSERT(functionRefExp != NULL);

  // Get the symbol
     SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
     ROSE_ASSERT(functionSymbol != NULL);

  // Get the function declaration
     SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
     ROSE_ASSERT(functionDeclaration != NULL);

  // Get the defining declaration (we don't want to get any forward 
  // declaration this will exist only if the function is defined).
     SgDeclarationStatement* definingDeclaration = functionDeclaration->get_definingDeclaration();
     ROSE_ASSERT(definingDeclaration != NULL);

  // Get the function declaration which will have the function definition 
  // (since our summary attribute is in the function definition).  We migh 
  // want to to have left it in the function declaration of the defining 
  // declaration instead.
     SgFunctionDeclaration* definingFunctionDeclaration = isSgFunctionDeclaration(definingDeclaration);
     ROSE_ASSERT(definingFunctionDeclaration != NULL);
     SgFunctionDefinition* functionDefinition = definingFunctionDeclaration->get_definition();
     ROSE_ASSERT(functionDefinition != NULL);

  // Get the summary model attribute (note that required function definitions must have been previously traversed)
  // AstAttribute* functionAttribute = functionDefinition->attribute["MemoryAccessSummaryAttribute"];
  // AstAttribute* functionAttribute = functionDefinition->attribute()["MemoryAccessSummaryAttribute"];
     AstAttribute* functionAttribute = functionDefinition->getAttribute("MemoryAccessSummaryAttribute");
     ROSE_ASSERT(functionAttribute != NULL);
     AccessModelSummaryAttribute* accessSummaryAttribute = dynamic_cast<AccessModelSummaryAttribute*>(functionAttribute);
     ROSE_ASSERT(accessSummaryAttribute != NULL);

  // Need to build equation in terms of varRefExp, to access the equation in terms of varRefExp->get_symbol()
     string expressionEquationString = "expression"
                                     + StringUtility::numberToString(functionRefExp)
                                     + " := "
                                     + "statement"
                                     + StringUtility::numberToString(functionDefinition)
                                     + "; \n";

  // printf ("Summary modeling attribute = %p at %p \n",accessSummaryAttribute,functionDefinition);
#if 0
     printf ("(SgFunctionRefExp) numberOfAccesses         = %d   \n",accessSummaryAttribute->numberOfAccesses);
     printf ("(SgFunctionRefExp) statementEquationString  = \n%s \n",accessSummaryAttribute->equationString.c_str());
     printf ("(SgFunctionRefExp) simplifiedEquationString = \n%s \n",accessSummaryAttribute->simplifiedEquationString.c_str());
#endif

     attribute.numberOfAccesses         += accessSummaryAttribute->numberOfAccesses;
     attribute.equationString           += accessSummaryAttribute->equationString + expressionEquationString;
     attribute.simplifiedEquationString += accessSummaryAttribute->simplifiedEquationString + expressionEquationString;

#if 1
     printf ("(SgFunctionRefExp) numberOfAccesses         = %d   \n",attribute.numberOfAccesses);
     printf ("(SgFunctionRefExp) statementEquationString  = \n%s \n",attribute.equationString.c_str());
     printf ("(SgFunctionRefExp) simplifiedEquationString = \n%s \n",attribute.simplifiedEquationString.c_str());
#endif

  // printf ("Exiting after processing of SgFunctionRefExp \n");
  // ROSE_ASSERT(false);
   }


void
AccessModeling::attachedAcessAttributes (SgProject* project )
   {
     printf ("Attaching memory access attributes to the AST \n");

     MemoryAccessAttributeAttachmentInheritedAttribute inheritedAttribute;

     MemoryAccessAttributeAttachmentTraversal traversal;
     traversal.traverseInputFiles(project,inheritedAttribute);
   }

#if 1
// Declare the counter so that we can track the number of Maple invocations
namespace MapleRoseInterface
   {
     int mapleEquationCounter = 0;
   }
#endif

string
AccessModeling::simplifyEquation ( string inputEquation )
   {
     string simplifiedEquation;

#if 0
  // Call the MapleRoseInterface function to evaluate the input Maple command
  // Speedup is about a factor of 20 over the file interface.
     printf ("inputEquation = %s \n",inputEquation.c_str());
     simplifiedEquation = MapleRoseInterface::evaluateMapleStatement (inputEquation);
     printf ("simplifiedEquation = %s \n",simplifiedEquation.c_str());
#else
  // ofstream datfile ( "inputEquationFile.eq" , ios::out | ios::app );
     ofstream outputFile ( "inputEquationFile.eq" , ios::out | ios::trunc );
     if ( outputFile.good() == false )
        {
          printf ("\"inputEquationFile.eq\" file failed to open \n");
          ROSE_ASSERT(false);
        }

  // Write the equation string to the file
     outputFile << inputEquation << std::endl;
     outputFile.close();

  // Invocation counter (useful for debugging)
     MapleRoseInterface::mapleEquationCounter++;

  // Call Maple using file interface (ugly be effective) ("-q" option means quite mode)
     system("maple -q /home/dquinlan/ROSE/NEW_ROSE/projects/programModeling/loopTripMapleScript");

  // Now read the file generated using Maple (to simplify the equation)
     ifstream inputFile ( "outputEquationFile.eq" , ios::in );
     if ( inputFile.good() == false )
        {
          printf ("\"outputEquationFile.eq\" file failed to open \n");
          ROSE_ASSERT(false);
        }

  // Read the maple generated simplified equation (string) from the file (this could be better handled)
     char buffer[5000];

  // Long equations are split over several lines in the output file from Maple
     int count = 0;
     while (!inputFile.eof())
        {
          inputFile.getline (buffer,1000);
          string s = buffer;
          if (inputFile.eof() == false)
             {
            // printf ("count = %4d  s = %s \n",count,s.c_str());
               simplifiedEquation += s + " ";
               count++;
             }
        }

     inputFile.close();
#endif

     return simplifiedEquation;
   }
