// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "markLhsValues.h"
// This controls output for debugging
#define WARN_ABOUT_ATYPICAL_LVALUES 0

void
markLhsValues( SgNode* node )
   {
// Liao, 2/19/2009, turn this off since it is called hundreds times and pollute the timing report   
//     TimingPerformance timer ("Fixup l-values:");

     MarkLhsValues astFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
   }

void
MarkLhsValues::visit(SgNode* node)
   {
  // DQ (1/19/2008): Fixup the get_lvalue() member function which is common on expressions.
  // printf ("In TestLValueExpressions::visit(): node = %s \n",node->class_name().c_str());

     ROSE_ASSERT(node != NULL);
#if 0
     Sg_File_Info* fileInfo = node->get_file_info();

     printf ("In MarkLhsValues::visit(): node = %s fileInfo = %p \n",node->class_name().c_str(),fileInfo);
     if (fileInfo != NULL)
        {
          bool isCompilerGenerated = fileInfo->isCompilerGenerated();
          std::string filename = fileInfo->get_filenameString();
          int line_number = fileInfo->get_line();
          int column_number = fileInfo->get_line();

          printf ("--- isCompilerGenerated = %s position = %d:%d filename = %s \n",isCompilerGenerated ? "true" : "false",line_number,column_number,filename.c_str());
        }
#endif

  // This function most often sets the SgVarRefExp which appears as an lhs operand in a limited set of binary operators.

     SgExpression* expression = isSgExpression(node);
     if (expression != NULL)
        {
#if 0
          printf ("MarkLhsValues::visit(): calling expression->get_lvalue() on expression = %p = %s \n",expression,expression->class_name().c_str());
#endif
          SgBinaryOp* binaryOperator = isSgBinaryOp(expression);

          if (binaryOperator != NULL)
             {
               switch (expression->variantT())
                  {
                 // IR nodes that have an l-value (required by C/C++/Fortran standard)
                    case V_SgAssignOp:
                    case V_SgAndAssignOp:
                    case V_SgDivAssignOp:
                    case V_SgIorAssignOp:
                    case V_SgLshiftAssignOp:
                    case V_SgMinusAssignOp:
                    case V_SgModAssignOp:
                    case V_SgMultAssignOp:
                    case V_SgPlusAssignOp:
                    case V_SgRshiftAssignOp:
                    case V_SgXorAssignOp:
                       {
                         SgExpression* lhs = binaryOperator->get_lhs_operand();
                         ROSE_ASSERT(lhs != NULL);
                         SgExpression* rhs = binaryOperator->get_rhs_operand();
                         ROSE_ASSERT(rhs != NULL);

                      // This is violated by the ROSE/tests/roseTests/astInliningTests/pass16.C test code!
                      // ROSE_ASSERT(lhs->get_lvalue() == true);

                      // This is a value that I know has to be set, the AST generation in EDG/Sage and OFP/Sage
                      // sets this properly, but some transformations of the AST do not, so we fix it up here.
                         lhs->set_lvalue(true);
                         rhs->set_lvalue(false);
                         break;
                       }

                 // These cases are less clear so don't explicitly mark it as an l-value!
                    case V_SgDotExp:
                    case V_SgArrowExp:
                       {
                         SgExpression* lhs = binaryOperator->get_lhs_operand();
                         ROSE_ASSERT(lhs != NULL);
#if WARN_ABOUT_ATYPICAL_LVALUES
                         printf ("L-value test for SgBinaryOp = %s: not clear how to assert value -- lhs->get_lvalue() = %s \n",binaryOperator->class_name().c_str(),lhs->get_lvalue() ? "true" : "false");
#endif
                      // ROSE_ASSERT(lhs->get_lvalue() == true);
                         break;
                       }

                 // DQ (10/9/2008): For the Fortran user defined operator, the lhs is not an L-value.
                 // This represents my understanding, because assignment is explicitly handled separately.
                    case V_SgUserDefinedBinaryOp:
                       {
                         SgExpression* lhs = binaryOperator->get_lhs_operand();
                         ROSE_ASSERT(lhs != NULL);
                         SgExpression* rhs = binaryOperator->get_rhs_operand();
                         ROSE_ASSERT(rhs != NULL);

                      // This is a value that I know has to be set, the AST generation in EDG/Sage and OFP/Sage
                      // sets this properly, but some transformations of the AST do not, so we fix it up here.
                         lhs->set_lvalue(false);
                         rhs->set_lvalue(false);
                         break;
                       }

                    default:
                       {
                      // Make sure that the lhs is not an L-value
                         SgExpression* lhs = binaryOperator->get_lhs_operand();
                         ROSE_ASSERT(lhs != NULL);

#if WARN_ABOUT_ATYPICAL_LVALUES
                         if (lhs->get_lvalue() == true)
                            {
                              printf ("Error for lhs = %p = %s = %s in binary expression = %s \n",
                                   lhs,lhs->class_name().c_str(),SageInterface::get_name(lhs).c_str(),expression->class_name().c_str());
                              binaryOperator->get_startOfConstruct()->display("Error for lhs: lhs->get_lvalue() == true: debug");
                            }
#endif

                      // ROSE_ASSERT(lhs->get_lvalue() == false);
                       }
                  }

               //SgExpression* rhs = binaryOperator->get_rhs_operand();
               // Liao 3/14/2011. This function is called by builders for binary expressions. 
               // These builders can accept empty right hand operands.
               // ROSE_ASSERT(rhs != NULL);

#if WARN_ABOUT_ATYPICAL_LVALUES
               if (rhs != NULL)
                 if (rhs->get_lvalue() == true)
                  {
                    printf ("Error for rhs = %p = %s = %s in binary expression = %s \n",
                         rhs,rhs->class_name().c_str(),SageInterface::get_name(rhs).c_str(),expression->class_name().c_str());
                    binaryOperator->get_startOfConstruct()->display("Error for rhs: rhs->get_lvalue() == true: debug");
                  }
#endif

            // ROSE_ASSERT(rhs->get_lvalue() == false);
             }
          
          SgUnaryOp* unaryOperator = isSgUnaryOp(expression);
          if (unaryOperator != NULL)
             {
               switch (expression->variantT())
                  {
                 // IR nodes that should have a valid lvalue
                 // What about SgAddressOfOp?
              
                    case V_SgAddressOfOp: break; // JJW 1/31/2008

                    case V_SgMinusMinusOp:
                    case V_SgPlusPlusOp:
                       {
                         SgExpression* operand = unaryOperator->get_operand();
                         ROSE_ASSERT(operand != NULL);

#if WARN_ABOUT_ATYPICAL_LVALUES
                      // if (operand->get_lvalue() == true)
                         if (operand->get_lvalue() == false)
                            {
                              printf ("Error for operand = %p = %s = %s in unary expression (SgMinusMinusOp or SgPlusPlusOp) = %s \n",
                                   operand,operand->class_name().c_str(),SageInterface::get_name(operand).c_str(),expression->class_name().c_str());
                              unaryOperator->get_startOfConstruct()->display("Error for operand: operand->get_lvalue() == true: debug");
                            }
#endif

                      // ROSE_ASSERT(operand->get_lvalue() == false);
                         operand->set_lvalue(true);
                      // ROSE_ASSERT(operand->get_lvalue() == true);
                         break;
                       }

                    case V_SgThrowOp:
                       {
#if WARN_ABOUT_ATYPICAL_LVALUES
                      // Note that the gnu " __throw_exception_again;" can cause a SgThrowOp to now have an operand!
                         SgExpression* operand = unaryOperator->get_operand();
                         if (operand == NULL)
                            {
                              printf ("Warning: operand == NULL in SgUnaryOp = %s (likely caused by __throw_exception_again) \n",expression->class_name().c_str());
                           // unaryOperator->get_startOfConstruct()->display("Error: operand == NULL in SgUnaryOp: debug");
                            }
#endif
                      // ROSE_ASSERT(operand != NULL);
                         break;
                       }

                 // DQ (10/9/2008): For the Fortran user defined operator, the operand is not an L-value.
                 // This represents my understanding, because assignment is explicitly handled separately.
                    case V_SgUserDefinedUnaryOp:
                       {
                         SgExpression* operand = unaryOperator->get_operand();
                         ROSE_ASSERT(operand != NULL);

                         operand->set_lvalue(false);
                       }

                 // Added to address problem on Qing's machine using g++ 4.0.2
                    case V_SgNotOp:

                 // These are where some error occur.  I want to isolate then so that I know the current status of where lvalues are not marked correctly!
                    case V_SgPointerDerefExp:
                    case V_SgCastExp:
                    case V_SgMinusOp:
                    case V_SgBitComplementOp:
                 // case V_SgPlusOp:
                       {
                         SgExpression* operand = unaryOperator->get_operand();
                         ROSE_ASSERT(operand != NULL);

#if WARN_ABOUT_ATYPICAL_LVALUES
                      // Most of the time this is false, we only want to know when it is true
                         if (operand->get_lvalue() == true)
                            {
                              printf ("L-value test for SgUnaryOp = %s: not clear how to assert value -- operand->get_lvalue() = %s \n",unaryOperator->class_name().c_str(),operand->get_lvalue() ? "true" : "false");
                           // unaryOperator->get_startOfConstruct()->display("L-value test for SgUnaryOp: operand->get_lvalue() == true: debug");
                            }
#endif
                      // ROSE_ASSERT(operand->get_lvalue() == false);
                         break;
                       }

                    default:
                       {
                         SgExpression* operand = unaryOperator->get_operand();
                         ROSE_ASSERT(operand != NULL);

#if WARN_ABOUT_ATYPICAL_LVALUES
                         if (operand->get_lvalue() == true)
                            {
                              printf ("Error for operand = %p = %s = %s in unary expression = %s \n",
                                   operand,operand->class_name().c_str(),SageInterface::get_name(operand).c_str(),expression->class_name().c_str());
                              unaryOperator->get_startOfConstruct()->display("Error for operand: operand->get_lvalue() == true: debug");
                            }
#endif

                      // DQ (10/9/2008): What is the date and author for this comment?  Is it fixed now? Was it made into a test code?
                      // Note that this fails for line 206 of file: include/g++_HEADERS/hdrs1/ext/mt_allocator.h
                         ROSE_ASSERT(operand->get_lvalue() == false);
                       }          
                  }
             }
        }
   }

