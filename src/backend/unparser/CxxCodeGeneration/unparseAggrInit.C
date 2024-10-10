
#include "sage3basic.h"
#include "unparser.h"
#include "rose_config.h"

#define DEBUG__unparseAggrInit 0

static bool
sharesSameStatement(SgExpression*, SgType* expressionType)
   {
  // DQ (7/29/2013): This function supports the structural analysis to determine when we have to output the type definition
  // or just the type name for a compound literal.

     bool result = false;
     SgNamedType* namedType = isSgNamedType(expressionType);

  // DQ (9/14/2013): I think this is a better implementation (test2012_46.c was failing before).
  // This permits both test2013_70.c and test2012_46.c to unparse properly, where the two were
  // previously mutually exclusive.
     if (namedType != NULL)
        {
          ASSERT_not_null(namedType->get_declaration());
          SgDeclarationStatement* declarationStatementDefiningType = namedType->get_declaration();
          SgDeclarationStatement* definingDeclarationStatementDefiningType = declarationStatementDefiningType->get_definingDeclaration();
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(definingDeclarationStatementDefiningType);

       // DQ (3/26/2015): Need to handle case where isAutonomousDeclaration() == true, but we still don't want to unparse the definition.
          bool isDeclarationPartOfTypedefDeclaration  = false;
          bool isDeclarationPartOfVariableDeclaration = false;
          if (classDeclaration != NULL)
             {
               isDeclarationPartOfVariableDeclaration = isSgVariableDeclaration(classDeclaration->get_parent());
               isDeclarationPartOfTypedefDeclaration  = isSgTypedefDeclaration(classDeclaration->get_parent());
             }

          if (classDeclaration != NULL && classDeclaration->get_isAutonomousDeclaration() == false && isDeclarationPartOfVariableDeclaration == false && isDeclarationPartOfTypedefDeclaration == false)
             {
            // This declaration IS defined imbedded in another statement.
               result = true;
             }
            else
             {
            // This declaration is NOT defined imbedded in another statement.
               result = false;
             }
        }
     return result;
   }


static bool
containsIncludeDirective(SgLocatedNode* locatedNode)
   {
     bool returnResult = false;
     AttachedPreprocessingInfoType* comments = locatedNode->getAttachedPreprocessingInfo();

     if (comments != NULL)
        {
          AttachedPreprocessingInfoType::iterator i;
          for (i = comments->begin(); i != comments->end(); i++)
             {
               ASSERT_not_null((*i));
               if (locatedNode->get_startOfConstruct()->isSameFile((*i)->get_file_info()) == true)
                  {
                 // This should also be true.
                    ROSE_ASSERT(locatedNode->get_endOfConstruct()->isSameFile((*i)->get_file_info()) == true);
                  }

               if ( *(locatedNode->get_startOfConstruct()) <= *((*i)->get_file_info()) && *(locatedNode->get_endOfConstruct()) >= *((*i)->get_file_info()) )
                  {
                 // Then the comment is in between the start of the construct and the end of the construct.
                    if ((*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration)
                       {
                         returnResult = true;
                       }
                  }
             }
        }

     return returnResult;
   }


static void
removeIncludeDirective(SgLocatedNode* locatedNode)
   {
//   bool returnResult = false;
     AttachedPreprocessingInfoType* comments = locatedNode->getAttachedPreprocessingInfo();
     AttachedPreprocessingInfoType includeDirectiveList;

     if (comments != NULL)
        {
#if 0
          printf ("Found attached comments (at %p of type: %s): \n",locatedNode,locatedNode->class_name().c_str());
#endif
          for (AttachedPreprocessingInfoType::iterator i = comments->begin(); i != comments->end(); i++)
             {
               ASSERT_not_null((*i));
#if 0
               printf ("          Attached Comment (relativePosition=%s): %s\n",
                    ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                    (*i)->getString().c_str());
               printf ("Comment/Directive getNumberOfLines = %d getColumnNumberOfEndOfString = %d \n",(*i)->getNumberOfLines(),(*i)->getColumnNumberOfEndOfString());
               (*i)->get_file_info()->display("comment/directive location");
#endif
               if (locatedNode->get_startOfConstruct()->isSameFile((*i)->get_file_info()) == true)
                  {
                 // This should also be true.
                    ROSE_ASSERT(locatedNode->get_endOfConstruct()->isSameFile((*i)->get_file_info()) == true);
                  }

               if ( *(locatedNode->get_startOfConstruct()) <= *((*i)->get_file_info()) && *(locatedNode->get_endOfConstruct()) >= *((*i)->get_file_info()) )
                  {
                 // Then the comment is in between the start of the construct and the end of the construct.
                    if ((*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorIncludeDeclaration)
                       {
#if 0
                         printf ("Found cpp include directive \n");
#endif
//                       returnResult = true;
                         includeDirectiveList.push_back(*i);
                       }
                  }
             }

       // Remove the list of include directives.
          for (AttachedPreprocessingInfoType::iterator i = includeDirectiveList.begin(); i != includeDirectiveList.end(); i++)
             {
#if 0
               printf ("In removeIncludeDirective(): Removing cpp include directive \n");

               printf ("     Remove Comment (relativePosition=%s): %s\n",((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",(*i)->getString().c_str());
               printf ("     Remove Comment/Directive getNumberOfLines = %d getColumnNumberOfEndOfString = %d \n",(*i)->getNumberOfLines(),(*i)->getColumnNumberOfEndOfString());
               (*i)->get_file_info()->display("remove comment/directive location");
#endif
               comments->erase(find(comments->begin(),comments->end(),*i));
             }
        }
       else
        {
#if 0
          printf ("In removeIncludeDirective(): No attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
#endif
        }
   }

///! Inspect the structure of this initialization is to see if it is using the C++11 initialization features for structs.
static bool uses_cxx11_initialization (SgNode* n) {
  // SgInitializer* initializerChain[3] = { NULL, NULL, NULL };
     std::vector<SgInitializer*> initializerChain;
     bool returnValue = false;

#if 0
     printf ("In uses_cxx11_initialization(): n = %p = %s \n",n,n->class_name().c_str());
#endif

  // This version starts at the second in the chain of three SgAggregateInitializer IR nodes.
  // It initializes the first intry in the chain through accessing the parents and the last two
  // from the current node and it's children.

     class InitializerTraversal : public AstSimpleProcessing
        {
          private:
               std::vector<SgInitializer*> & initializerChain;
               int counter;

          public:
               InitializerTraversal(std::vector<SgInitializer*> & x) : initializerChain(x), counter(1) {}
               void visit (SgNode* node)
                  {
                    ASSERT_not_null(node);
#if 0
                    printf ("In InitializerTraversal::visit(): node = %p = %s \n",node,node->class_name().c_str());
#endif
                    SgInitializer* initializer = isSgInitializer(node);
                    if (initializer != NULL && counter < 3)
                       {
                      // initializerChain[counter] = initializer;
                         initializerChain.push_back(initializer);
                         counter++;
                       }
                  }
        };


     SgExprListExp* expressonList = isSgExprListExp(n->get_parent());
     if (expressonList != NULL)
        {
          SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(expressonList->get_parent());
          if (aggregateInitializer != NULL)
             {
               initializerChain.push_back(aggregateInitializer);

               ROSE_ASSERT(initializerChain.size() == 1);

            // Now buid the traveral object and call the traversal (preorder) on the AST subtree.
               InitializerTraversal traversal(initializerChain);
               traversal.traverse(n, preorder);

             }
        }

#if 0
     printf ("In uses_cxx11_initialization(): initializerChain.size() = %lu \n",initializerChain.size());
#endif

     ROSE_ASSERT(initializerChain.size() <= 3);

     if (initializerChain.size() == 3)
        {
       // Check the order of the initializers.
          SgAggregateInitializer* agregateInitializer_0 = isSgAggregateInitializer(initializerChain[0]);
          SgAggregateInitializer* agregateInitializer_1 = isSgAggregateInitializer(initializerChain[1]);
          SgAggregateInitializer* agregateInitializer_2 = isSgAggregateInitializer(initializerChain[2]);
#if 0
          printf ("agregateInitializer_0 = %p = %s \n",agregateInitializer_0,initializerChain[0]->class_name().c_str());
          printf ("agregateInitializer_1 = %p = %s \n",agregateInitializer_1,initializerChain[1]->class_name().c_str());
          printf ("agregateInitializer_2 = %p = %s \n",agregateInitializer_2,initializerChain[2]->class_name().c_str());
#endif
          if (agregateInitializer_0 != NULL && agregateInitializer_1 != NULL && agregateInitializer_2 != NULL)
             {
               SgType* agregateInitializer_type_0 = isSgClassType(agregateInitializer_0->get_type());
               SgType* agregateInitializer_type_1 = isSgArrayType(agregateInitializer_1->get_type());
               SgType* agregateInitializer_type_2 = isSgClassType(agregateInitializer_2->get_type());
#if 0
               printf ("agregateInitializer_type_0 = %p = %s \n",agregateInitializer_type_0,agregateInitializer_0->get_type()->class_name().c_str());
               printf ("agregateInitializer_type_1 = %p = %s \n",agregateInitializer_type_1,agregateInitializer_1->get_type()->class_name().c_str());
               printf ("agregateInitializer_type_2 = %p = %s \n",agregateInitializer_type_2,agregateInitializer_2->get_type()->class_name().c_str());
#endif
               SgClassType* classType_0 = isSgClassType(agregateInitializer_type_0);
               SgArrayType* arrayType_1 = isSgArrayType(agregateInitializer_type_1);
               SgClassType* classType_2 = isSgClassType(agregateInitializer_type_2);

               if (classType_0 != NULL && arrayType_1 != NULL && classType_2 != NULL)
                  {
#if 0
                    printf ("Found C++11 specific initializer chain that requires use of class specifier \n");
#endif
                    returnValue = true;
                  }
                 else
                  {
#if 0
                    printf ("No C++11 class specification required for this initializer chain \n");
#endif
                  }
             }
        }


  // DQ (6/27/2018): check if this is part of a template instantiation that would require the class name in the initializer to trigger the instantiation.
     SgAggregateInitializer* aggregateInitializer = isSgAggregateInitializer(n);
     if (aggregateInitializer != NULL)
        {
          SgExprListExp* expListExp = isSgExprListExp(aggregateInitializer->get_parent());
          if (expListExp != NULL)
             {
               SgFunctionCallExp* functionCall = isSgFunctionCallExp(expListExp->get_parent());

               if (functionCall != NULL)
                  {
                    SgExpression* functionExpression = functionCall->get_function();
                    SgFunctionRefExp*       functionRefExp       = isSgFunctionRefExp(functionCall->get_function());
                    SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(functionCall->get_function());

                    SgArrowExp* arrowExpression = isSgArrowExp(functionExpression);
                    SgDotExp*   dotExpression   = isSgDotExp(functionExpression);

                    SgExpression* rhs = NULL;
                    if (arrowExpression != NULL)
                       {
                         rhs = arrowExpression->get_rhs_operand();
                       }
                    if (dotExpression != NULL)
                       {
                         rhs = dotExpression->get_rhs_operand();
                       }

                    if (functionRefExp == NULL && memberFunctionRefExp == NULL)
                       {
                      // Get the pointer from the rhs.
                         functionRefExp       = isSgFunctionRefExp(rhs);
                         memberFunctionRefExp = isSgMemberFunctionRefExp(rhs);
                       }

                 // DQ (6/27/2018): This assertion fails for test2018_111.C.
                 // ASSERT_not_null(functionRefExp);

                    SgFunctionDeclaration* functionDeclaration = NULL;
                    if (functionRefExp != NULL)
                       {
                         SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
#if 0
                         printf ("functionSymbol = %p = %s \n",functionSymbol,functionSymbol->class_name().c_str());
#endif
                      // SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
                         functionDeclaration = functionSymbol->get_declaration();
                       }

                    if (memberFunctionRefExp != NULL)
                       {
                         SgMemberFunctionSymbol* functionSymbol = memberFunctionRefExp->get_symbol();
#if 0
                         printf ("member functionSymbol = %p = %s \n",functionSymbol,functionSymbol->class_name().c_str());
#endif
                      // SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
                         functionDeclaration = functionSymbol->get_declaration();
                       }

                    if (functionDeclaration != NULL)
                       {

                         bool isTemplateInstantiation = SageInterface::isTemplateInstantiationNode(functionDeclaration);
#if 0
                         printf ("isTemplateInstantiation = %s \n",isTemplateInstantiation ? "true" : "false");
#endif
                         if (isTemplateInstantiation == true)
                            {
#if 0
                              printf ("Found C++11 specific initializer function (%s) argument to template instantiation: that requires use of class specifier \n",functionDeclaration->class_name().c_str());
#endif
                           // This might be overly general.
                              returnValue = true;
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ABORT();
#endif
                            }
                       }
                  }
             }
        }

#if 0
     printf ("Leaving uses_cxx11_initialization(): n = %p = %s returnValue = %s \n",n,n->class_name().c_str(),returnValue ? "true" : "false");
#endif

     return returnValue;
   }

void Unparse_ExprStmt::unparseAggrInit(SgExpression* expr, SgUnparse_Info& info)
   {
     SgAggregateInitializer * aggr_init = isSgAggregateInitializer(expr);
     ASSERT_not_null(aggr_init);

     bool compiler_generated = aggr_init->get_startOfConstruct()->isCompilerGenerated();
#if DEBUG__unparseAggrInit
     printf("Enter Unparse_ExprStmt::unparseAggrInit():\n");
     printf("  aggr_init = %p = %s\n", aggr_init, aggr_init->class_name().c_str());
     printf("    ->get_uses_compound_literal() = %s\n", aggr_init->get_uses_compound_literal() ? "true" : "false");
     printf("  compiler_generated = %s\n", compiler_generated ? "true" : "false");
#endif

     bool need_cxx11_class_specifier = uses_cxx11_initialization (expr);

     SgSourceFile* sourceFile = info.get_current_source_file();
     if ( (sourceFile != NULL) && !( sourceFile->get_Cxx11_only() || sourceFile->get_Cxx14_only() ) ) {
       need_cxx11_class_specifier = false;
     }

     SgUnparse_Info newinfo2(info);
     newinfo2.set_inAggregateInitializer();

     if (aggr_init->get_uses_compound_literal() == true)
        {
       // This aggregate initializer is using a compound literal and so we need to output the type.
       // This looks like an explict cast, but is not a cast internally in the language, just that
       // this is how compound literals are supposed to be handled.
          ASSERT_not_null(aggr_init->get_type());

#if DEBUG__unparseAggrInit
          printf ("  aggr_init->get_type() = %p = %s \n", aggr_init->get_type(), aggr_init->get_type()->class_name().c_str());
#endif

          bool shares = sharesSameStatement(aggr_init, aggr_init->get_type());
#if DEBUG__unparseAggrInit
          printf ("  shares = %s \n",shares ? "true" : "false");
#endif
          if (shares)
             {
               newinfo2.unset_SkipClassDefinition();
               newinfo2.unset_SkipEnumDefinition();
             }
            else
             {
               newinfo2.set_SkipClassDefinition();
               newinfo2.set_SkipEnumDefinition();
             }

#if DEBUG__unparseAggrInit
          printf ("  SkipClassDefinition() = %s \n", newinfo2.SkipClassDefinition() ? "true" : "false");
          printf ("  SkipEnumDefinition()  = %s \n", newinfo2.SkipEnumDefinition()  ? "true" : "false");
#endif

          curprint ("(");
          unp->u_type->outputType<SgAggregateInitializer>(aggr_init,aggr_init->get_type(), newinfo2);
          curprint (")");
        }

  // DQ (9/29/2012): We don't want to use the explicit "{}" inside of function argument lists (see C test code: test2012_10.c).
     bool need_explicit_braces = aggr_init->get_need_explicit_braces();

#if DEBUG__unparseAggrInit
     printf ("  need_explicit_braces     = %s \n", need_explicit_braces              ? "true" : "false");
     printf ("  SkipEnumDefinition()     = %s \n", newinfo2.SkipEnumDefinition()     ? "true" : "false");
     printf ("  SkipClassDefinition()    = %s \n", newinfo2.SkipClassDefinition()    ? "true" : "false");
     printf ("  inAggregateInitializer() = %s \n", newinfo2.inAggregateInitializer() ? "true" : "false");
#endif

     SgExpressionPtrList& list = aggr_init->get_initializers()->get_expressions();
     if (need_cxx11_class_specifier && need_explicit_braces)
        {
          ASSERT_not_null(aggr_init);
          SgClassType* classType = isSgClassType(aggr_init->get_type());

          if (classType != nullptr)
             {
               newinfo2.set_SkipClassSpecifier();
               unp->u_type->outputType<SgAggregateInitializer>(aggr_init,classType,newinfo2);
             }
        }

     if (need_explicit_braces)
        {
          curprint("{");
        }

     if (containsIncludeDirective(aggr_init))
        {
#if DEBUG__unparseAggrInit
          printf ("  Found an include directive to be removed \n");
#endif
          removeIncludeDirective(aggr_init);
        }

#if DEBUG__unparseAggrInit
     printf ("  list.size() = %zu \n",list.size());
#endif

     size_t last_index = list.size() - 1;
     for (size_t index = 0; index < list.size(); index++)
        {
          if (index > 0) {
            curprint (", ");
          }
          SgExpression * element = list[index];
          SgType * etype = element->get_type();

          SgAggregateInitializer * aggr_init = isSgAggregateInitializer(element);
          SgConstructorInitializer * ctor_init = isSgConstructorInitializer(element);

          bool element_compiler_generated = element->isCompilerGenerated();
#if DEBUG__unparseAggrInit
          printf ("  - list index = %zu \n", index);
          printf ("    element = %p = %s\n", element, element->class_name().c_str());  
          printf ("      type = %p = %s\n", etype, etype->class_name().c_str());  
          printf ("      compiler_generated = %s\n", element_compiler_generated ? "true" : "false");  
#endif
          if (ctor_init != nullptr && element_compiler_generated) {
            break;
          }

          if (need_cxx11_class_specifier && aggr_init && isSgClassType(etype)) {
            newinfo2.set_SkipClassSpecifier();
            unp->u_type->outputType<SgAggregateInitializer>(aggr_init, etype, newinfo2);
          }

          SgUnparse_Info newinfo(info);
          newinfo.set_inAggregateInitializer();

#if DEBUG__unparseAggrInit
          printf ("    SkipEnumDefinition()  = %s \n",newinfo.SkipEnumDefinition() ? "true" : "false");
          printf ("    SkipClassDefinition() = %s \n",newinfo.SkipClassDefinition() ? "true" : "false");
#endif
          unparseExpression(element, newinfo);
        }

     unparseAttachedPreprocessingInfo(aggr_init, info, PreprocessingInfo::inside);

     if (need_explicit_braces)
        {
          curprint("}");
        }

#if DEBUG__unparseAggrInit
     printf ("Leaving Unparse_ExprStmt::unparseAggrInit() \n");
#endif
   }
