#include "sage3basic.h"

#include "fixupConstantFoldedValues.h"

//! This removes the original expression tree from value expressions where it has been constant folded by EDG.
void resetConstantFoldedValues( SgNode* node )
   {
  // This is the initial default, it will be changed later to be an optional behavior.
  // Note that all unparsing will use the original expression tree and is verified to generate
  // correct for all of the regression tests in C and C++.  The original expression trees are
  // saved in ROSE as part of supporting source to source work.  One could conclude that the
  // original expression tree should always of never used as part of analysis, but not both
  // the constant folded value AND the original expression tree.

  // This is a temporary option, if false then we pass all tests and are consistent with historical configurations 
  // of the ROSE AST, but it is inconsistent so we want to fix that (because it causes control flow problems).
#if 1
  // DQ (11/5/2012): Original value
     bool makeASTConstantFoldingExpressionConsistant = true;
#else
  // DQ (11/5/2012): An experimental value while I play with the original expression trees.
     bool makeASTConstantFoldingExpressionConsistant = false;
#endif

#if 1
  // This should be turned on except for internal testing.
     ROSE_ASSERT(makeASTConstantFoldingExpressionConsistant == true);
#else
     printf ("WARNING: INTERNAL TESTING MODE: In resetConstantFoldedValues() \n");
     ROSE_ASSERT(makeASTConstantFoldingExpressionConsistant == false);
#endif

  // This will become an input option in the near future.  Once they can be supported through all of the test codes.
  // The default is to use the original expression trees and replace the saved constant folded values in the AST.
  // Note that having both can lead to some confusion (e.g. in the control flow graph and analysis, so we select one
  // (no constant folded values) and make it an option to have the other (using constant folded values)).  Note also
  // This this is EDG's definition of constant folding which is fairly aggressive.
     bool makeASTConstantFoldingExpressionConsistant_useOriginalExpressionTrees = true;

  // Constant folding is optional and can be specified as an optional parameter to "frontend()" function.
  // If set it sets a flag in the SgProject IR node.
     SgProject* project = isSgProject(node);
     if (project != NULL)
        {
       // Read the optional setting for constant folding in the frontend (only applied to C/C++ when using EDG).
          makeASTConstantFoldingExpressionConsistant_useOriginalExpressionTrees = (project->get_frontendConstantFolding() == false);
        }
  // ROSE_ASSERT(project != NULL);
  // printf ("project->get_frontendConstantFolding() = %s \n",(project->get_frontendConstantFolding() == true) ? "true" : "false");

#if 0
  // SgProject* project = isSgProject(node);
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     ROSE_ASSERT(project != NULL);
     if (project != NULL)
        {
          printf ("OUTPUT A GRAPH BEFORE CONSTANT FOLDING \n");
          generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"_before_constantfolding");
        }
#endif

  // printf ("In resetConstantFoldedValues(): makeASTConstantFoldingExpressionConsistant = %s \n",makeASTConstantFoldingExpressionConsistant ? "true" : "false");

     if (makeASTConstantFoldingExpressionConsistant == true)
        {
       // printf ("In resetConstantFoldedValues(): makeASTConstantFoldingExpressionConsistant_useOriginalExpressionTrees = %s \n",makeASTConstantFoldingExpressionConsistant_useOriginalExpressionTrees ? "true" : "false");

          if (makeASTConstantFoldingExpressionConsistant_useOriginalExpressionTrees == true)
             {
            // We want this to be the default for ROSE, since it matches what is output by the unparser and thus what 
            // is required to pass all of the tests codes.

            // DQ (9/17/2011): This case fails dozens of test codes!
            // However, this case fails lot of tests codes due to the verification of all OriginalExpressionTree pointer 
            // to be NULL and the name qualification tests (which fail as a result of the transformations to eliminate the 
            // constant folded expressions and instead use the original expression trees).

            // printf ("WARNING: Testing current default to replace constant folded value with the original expression tree (future default setting for ROSE). \n");

            // This replaces the constant folded value with the saved original expression tree (default).
            // This fails for test2011_113.C since we don't handle expressions in array types that contain 
            // original expression trees. The reason is that original expression tree is not used to replace
            // the constant folded value and then (because of recent changes to the AST traversal) the variables
            // in the array types index expression is not processed for name qualification.  
            // We need to either: 
            //    1) uniformly define the how constants are replaced (everywhere) with their original expressions, or
            //    2) fix the name qualification to look into the original expression trees explicitly.
            // or both.
            // Note: If we fix #1 then we will not have to worry about #2, also I think that #1 defines a consistent 
            // AST, where as #2 pushes the name qualification to simple work on the non-consistent AST.
               removeConstantFoldedValue(node);
             }
            else
             {
            // DQ (9/17/2011): This case fails only 2 test codes. Where the constant folded expression is unparsed but 
            // does not generate the correct code.  This may be a separate issue to investigate after we get the original 
            // expression trees to be used (and the constant folding overridden; the other face below).

            // DQ (9/18/2011): Leave this warning message in place for now (we will likely remove it later).
               printf ("WARNING: Current default is to reset constant folding by removing the original expression tree. \n");

            // This removes the original expression tree leaving the constant folded values (optional; not the default).
            // This fails for test2006_112.C (fails to compile unparsed code because the original expression is correct 
            // code (if unparsed) while the constant folded expression is not correct code (if unparsed).  In this case
            // the constant folded values are mostly useful for purposes of analysis.
               removeOriginalExpressionTrees(node);
             }

       // DQ (11/5/2012): I think this should be here rather than outside of this (put it back the way it was).
       // verifyOriginalExpressionTreesSetToNull(node);

       // DQ (4/26/2013): Moved here to permit makeASTConstantFoldingExpressionConsistant == false for debugging.
       // DQ (11/5/2012): Moved to inside of conditional (put this back the way it was).
       // verifyOriginalExpressionTreesSetToNull(node);
        }
       else
        {
       // DQ (4/26/2013): Added warning to support debugging of default argument handling in function call arguments.
          printf ("WARNING: makeASTConstantFoldingExpressionConsistant == false (temporary state for debugging mixed AST with constants and their original expression trees). \n");
        }

  // DQ (4/26/2013): Commented out (see comment above).
  // DQ (11/5/2012): Moved to inside of conditional (put this back the way it was).
     verifyOriginalExpressionTreesSetToNull(node);
   }

// ****************************************************************************
//   Supporting function used in both cases of the constant folding handling
// ****************************************************************************

void
deleteOriginalExpressionTree(SgExpression* exp)
   {
  // This function deleted the existing original expression tree for an expression.

     SgExpression* originalExpressionTree = exp->get_originalExpressionTree();
     if (originalExpressionTree != NULL)
        {
#if 0
          printf ("Removing the original expression tree from keyExpression = %p = %s originalExpressionTree = %p = %s \n",
               exp,exp->class_name().c_str(),originalExpressionTree,originalExpressionTree->class_name().c_str());
#endif
       // Remove the original expression tree...
       // delete value->get_originalExpressionTree();
       // SageInterface::deleteAST(originalExpressionTree);
          SageInterface::deleteExpressionTreeWithOriginalExpressionSubtrees(originalExpressionTree);

       // Set the pointer to the original expression tree to NULL.
          exp->set_originalExpressionTree(NULL);
        }
   }


// ****************************************************************************
// ****************************************************************************
//    Remove the Original Expression Tree (leaving the constant folded value)
// ****************************************************************************
// ****************************************************************************

void removeOriginalExpressionTrees( SgNode* node )
   {
  // This is the interface function called by resetConstantFoldedValues(SgNode*).

  // DQ (3/11/2006): Introduce tracking of performance of ROSE.
     TimingPerformance timer1 ("Fixup Constant Folded Values (remove the original expression tree, leaving the constant folded values):");

     RemoveOriginalExpressionTrees astFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
  // astFixupTraversal.traverse(node,preorder);

  // DQ (9/17/2011): Use a traversal over the memory pool so that we can catch all 
  // expressions, even those on in the AST traversal such as those in array types.
     astFixupTraversal.traverseMemoryPool();

#if 0
  // This verifies that we have cleared all original expression trees saved into the 
  // AST by the front-end translation into ROSE.
     VerifyOriginalExpressionTreesSetToNull verifyFixup;
     verifyFixup.traverseMemoryPool();
#endif
   }



void
RemoveOriginalExpressionTrees::visit ( SgNode* node )
   {
  // This support for leave the constant folded value is simpler than the other case 
  // of replacing the constant folded values with the original expression trees.

     ROSE_ASSERT(node != NULL);

  // DQ (3/11/2006): Set NULL pointers where we would like to have none.
  // printf ("In RemoveOriginalExpressionTrees::visit(): node = %s \n",node->class_name().c_str());

  // SgValueExp* value = isSgValueExp(node);
     SgExpression* value = isSgExpression(node);
     if (value != NULL)
        {
          deleteOriginalExpressionTree(value);
        }
   }




// ************************************************************************
// ************************************************************************
//    Replace the Constant Folded Value with the Original Expression Tree
// ************************************************************************
// ************************************************************************

void removeConstantFoldedValue( SgNode* node )
   {
  // This is the interface function called by resetConstantFoldedValues(SgNode*).

  // DQ (3/11/2006): Introduce tracking of performance of ROSE.
     TimingPerformance timer1 ("Fixup Constant Folded Values (replace with original expression trees):");

     RemoveConstantFoldedValue astFixupTraversal;

#if 0
     printf ("**************************************************************** \n");
     printf ("Processing the AST (AST traversal for RemoveConstantFoldedValue) \n");
     printf ("**************************************************************** \n");
#endif

  // I think the default should be preorder so that the interfaces would be more uniform
  // astFixupTraversal.traverse(node,postorder);
     astFixupTraversal.traverse(node);

#if 0
     printf ("**************************************** \n");
     printf ("Processing the array types and bitfields \n");
     printf ("**************************************** \n");
#endif
     
     {
     TimingPerformance timer1 ("Fixup Constant Folded Values (catch all expresions via memory pool traversal):");
  // DQ (9/17/2011): Use a traversal over the memory pool so that we can catch all 
  // expressions, even those not in the AST traversal such as those in array types.
     RemoveConstantFoldedValueViaParent astFixupTraversal_2;
     astFixupTraversal_2.traverseMemoryPool();
     }
     
  // DQ (10/12/2012): Turn on the verification that expression trees have been removed...previously commented out.
#if 1
     {
     TimingPerformance timer1 ("Fixup Constant Folded Values (verifyFixup):");
     VerifyOriginalExpressionTreesSetToNull verifyFixup;
     verifyFixup.traverseMemoryPool();
     }
#endif
   }


RemoveConstantFoldedValueSynthesizedAttribute::RemoveConstantFoldedValueSynthesizedAttribute(SgNode* n)
   : node(n)
   {
  // Nothing to do.
   }


void
ConstantFoldedValueReplacer::operator()(SgNode*& key, const SgName & debugStringName, bool /* traverse */ traceReplacement)
   {
  // This function is used to replace the expression in an AST node and avoids explicitly handling all of the cases 
  // of where an IR node can exist in the AST (in this case expressions containing an opriginal expression tree could 
  // be in a lot of locations).

  // Note that the key will be a reference to the pointer for each data member of the IR node where: 
  //      node->processDataMemberReferenceToPointers(&r);
  // is called.

#if 0
     printf ("Inside of ConstantFoldedValueReplacer::operator() key = %p = %s = %s node = %p = %s = %s \n",
          key,(key != NULL) ? key->class_name().c_str() : "NULL",(key != NULL) ? SageInterface::get_name(key).c_str() : "NULL",
          targetNode,(targetNode != NULL) ? targetNode->class_name().c_str() : "NULL",(targetNode != NULL) ? SageInterface::get_name(targetNode).c_str() : "NULL");
#endif

     if (key != NULL)
        {
       // Now reset the pointer to the subtree identified as redundent with a
       // subtree in the original AST to the subtree in the original (merged) AST.

       // Note: targetNode is the IR node to be replaced (set in the ConstantFoldedValueReplacer constructor call).
          if (key == targetNode)
             {
#if 0
               printf ("Note that key == originalNode \n");
#endif
               SgExpression* keyExpression = isSgExpression(key);

            // DQ (9/17/2011): We don't want to eliminate references to enum values (see test2005_194.C).
            // Though I wonder if it could be that we are not distinguishing the enum value and the 
            // values of the enum variables (the order of the fields in the enum declaration).
               if (isSgEnumVal(keyExpression) == NULL)
                  {
                    if (keyExpression != NULL)
                       {
#if 0
                         printf ("Note that key is a valid expression keyExpression->get_originalExpressionTree() = %p \n",keyExpression->get_originalExpressionTree());
#endif
                         if (keyExpression->get_originalExpressionTree() != NULL)
                            {
#if 0
                              printf ("key contains a originalExpressionTree = %p = %s \n",keyExpression->get_originalExpressionTree(),keyExpression->get_originalExpressionTree()->class_name().c_str());
#endif

                           // DQ (10/8/2011): Added support for chains of expression trees.
                           // while (keyExpression->get_originalExpressionTree()->get_originalExpressionTree() != NULL)
                              while (keyExpression->get_originalExpressionTree()->get_originalExpressionTree() != NULL && isSgEnumVal(keyExpression->get_originalExpressionTree()) == NULL)
                                 {
                                   SgExpression* nestedOriginalExpressionTree = keyExpression->get_originalExpressionTree();
                                   ROSE_ASSERT(nestedOriginalExpressionTree != NULL);
#if 0
                                   printf ("Found a chain of original expression trees (iterate to find the end of the chain: keyExpression = %p = %s keyExpression->get_originalExpressionTree() = %p = %s \n",
                                        keyExpression,keyExpression->class_name().c_str(),nestedOriginalExpressionTree,nestedOriginalExpressionTree->class_name().c_str());
#endif
                                   keyExpression = nestedOriginalExpressionTree;
                                 }

                           // If this is an enum value, then we don't want the original expression tree (which 
                           // will otherwise be substituted into such places as SgCaseOptionStmt nodes, etc.).
                              SgEnumVal* enumValue = isSgEnumVal(keyExpression->get_originalExpressionTree());
                              if (enumValue != NULL)
                                 {
#if 0
                                   printf ("Detected case of enumValue = %p \n",enumValue);
#endif
                                   if (enumValue->get_originalExpressionTree())
                                      {
#if 0
                                        printf ("Deleting the original expression in the nested enumValue \n");
#endif
                                        deleteOriginalExpressionTree(enumValue->get_originalExpressionTree());
                                        enumValue->set_originalExpressionTree(NULL);
                                      }
                                 }

                              ROSE_ASSERT(keyExpression->get_originalExpressionTree() != NULL);
                              key = keyExpression->get_originalExpressionTree();
                              ROSE_ASSERT(key != NULL);
                              key->set_parent(targetNode->get_parent());
                              keyExpression->set_originalExpressionTree(NULL);

                              SgExpression* targetExpression = isSgExpression(targetNode);
                              while (targetExpression != NULL) {
                                targetExpression->set_parent(NULL);
                                SgExpression * e = targetExpression;
                                targetExpression = targetExpression->get_originalExpressionTree();
                                SageInterface::deleteAST(e);
                              }
                           // Reset the pointer to avoid any dangling pointer problems.
                              targetNode = NULL;
                            }
                       }
                      else
                       {
#if 0
                         printf ("key is not a SgExpression \n");
#endif
                       }
                  }
                 else
                  {
                 // For the case of a SgEnumVal, don't use the original expression tree (see test2005_194.C)
                 // The original expression tree holds the value used for the enum field, instead of the 
                 // reference to the correct enum field).
#if 0
                    printf ("ENUM VALUE special handling: we call deleteOriginalExpressionTree(keyExpression = %p) \n",keyExpression);
#endif
                    deleteOriginalExpressionTree(keyExpression);
                  }
             }
            else
             {
#if 0
               printf ("key != originalNode \n");
#endif
             }
        }
       else
        {
#if 0
          printf ("key == NULL \n");
#endif
        }

#if 0
     printf ("Leaving ConstantFoldedValueReplacer::operator() new reset key = %p = %s \n",key,(key != NULL) ? key->class_name().c_str() : "NULL");
#endif
   }

void
RemoveConstantFoldedValue::handleTheSynthesizedAttribute( SgNode* node, const RemoveConstantFoldedValueSynthesizedAttribute & i )
   {
     SgExpression* value = isSgExpression(i.node);
     if (value != NULL)
        {
          SgExpression* originalExpressionTree = value->get_originalExpressionTree();
          if (originalExpressionTree != NULL)
             {
#if 0
               printf ("Found an originalExpressionTree = %p = %s \n",originalExpressionTree,originalExpressionTree->class_name().c_str());
               printf ("   --- node = %p = %s \n",node,node->class_name().c_str());
#endif
               if (node == value->get_parent())
                  {
                 // What kind of IR node are we at presently? Replace the expression representing the SgValueExp with the Expression representing the original subtree.
#if 0
                    printf ("Current IR node with SgExpression child = %p = %s originalExpressionTree = %p = %s \n",node,node->class_name().c_str(),originalExpressionTree,originalExpressionTree->class_name().c_str());
#endif
#if 0
                    node->get_startOfConstruct()->display("handleTheSynthesizedAttribute(): node source position: debug");
                    value->get_startOfConstruct()->display("handleTheSynthesizedAttribute(): value source position: debug");
#endif
                    bool traceReplacement = true;
                    ConstantFoldedValueReplacer r(traceReplacement, value);
                    node->processDataMemberReferenceToPointers(&r);
                  }
                 else
                  {
                    printf ("*** Strange case of child attribute not having the current node as a parent child = %p = %s originalExpressionTree = %p = %s \n",node,node->class_name().c_str(),originalExpressionTree,originalExpressionTree->class_name().c_str());
                  }
             }
        }
   }


// void RemoveConstantFoldedValue::visit ( SgNode* node )
RemoveConstantFoldedValueSynthesizedAttribute
RemoveConstantFoldedValue::evaluateSynthesizedAttribute ( SgNode* node, SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     ROSE_ASSERT(node != NULL);

  // DQ (3/11/2006): Set NULL pointers where we would like to have none.
#if 0
#if 0
  // Avoid excessive output.
     if (isSgFunctionDeclaration(node) == NULL && isSgInitializedName(node) == NULL && isSgFunctionParameterList(node) == NULL)
          printf ("In RemoveConstantFoldedValue::evaluateSynthesizedAttribute(): node = %p = %s synthesizedAttributeList.size() = %" PRIuPTR " \n",node,node->class_name().c_str(),synthesizedAttributeList.size());
#else
     printf ("In RemoveConstantFoldedValue::evaluateSynthesizedAttribute(): node = %p = %s synthesizedAttributeList.size() = %" PRIuPTR " \n",node,node->class_name().c_str(),synthesizedAttributeList.size());
#endif
#endif

  // Here we reset the pointer to the constant folded value to be the pointer to the original expression tree.
     SubTreeSynthesizedAttributes::iterator i = synthesizedAttributeList.begin();
     while (i != synthesizedAttributeList.end())
        {
#if 0
       // Avoid excessive output.
          if (isSgFunctionDeclaration(i->node) == NULL && isSgInitializedName(i->node) == NULL && isSgFunctionParameterList(i->node) == NULL && i->node != NULL)
               printf ("synthesizedAttribute = %p = %s \n",i->node,(i->node != NULL) ? i->node->class_name().c_str() : "NULL");
#endif

       // DQ (10/8/2011): Refectored this code so that we could better support chains of original expression trees (see test2011_146.C).
          handleTheSynthesizedAttribute(node,*i);

          i++;
        }

  // Here we force the nested traversal of the originalExpressionTree, since it is not traversed as part of the AST.
  // Note that these are not always leaf nodes that we want to interogate (see test2011_140.C).

  // DQ (9/24/2011): I think this is the wrong place to process this case (see test2011_140.C).
  // DQ (9/24/2011): We have fixed the AST traversal to no longer traverse originalExpressionTree subtree's (since 
  // it make analysis using the traversal redundant with the constant folded values).
  // If the current node is an expression and a leaf node of the AST, then check if it has an originalExpressionTree, 
  // since we no longer traverse the originalExpressionTree as part of the definition of the AST.
  // SgExpression* leafExpression = (synthesizedAttributeList.empty() == true) ? isSgExpression(node) : NULL;
     SgExpression* expression = isSgExpression(node);
     if (expression != NULL)
        {
#if 0
          printf ("In RemoveConstantFoldedValue::evaluateSynthesizedAttribute(): Found an expression \n");
#endif
          SgExpression* originalExpressionTree = expression->get_originalExpressionTree();
          if (originalExpressionTree != NULL)
             {
#if 0
               printf ("Found an expression with a valid originalExpressionTree\n");
#endif
            // Make sure that the traversal will see the nested subtrees of any originalExpressionTree (since they may have constant folded subexpresions).
               RemoveConstantFoldedValue nestedOriginalExpressionTreeTraversal;

            // DQ (12/8/2016): This is commented out as part of eliminating warnings we want to have be errors: [-Werror=unused-but-set-variable.
            // RemoveConstantFoldedValueSynthesizedAttribute nestedSynthesizedAttribute = nestedOriginalExpressionTreeTraversal.traverse(originalExpressionTree);

            // DQ (12/8/2016): Note return value is not used.
               nestedOriginalExpressionTreeTraversal.traverse(originalExpressionTree);

#if 0
            // DQ (10/8/2011): We should not handl this here, I think.
               handleTheSynthesizedAttribute(node,nestedSynthesizedAttribute);
#endif
#if 0
               printf ("DONE: Found an expression with a valid originalExpressionTree nestedSynthesizedAttribute \n");
#endif
             }
        }

     return RemoveConstantFoldedValueSynthesizedAttribute(node);
   }



void
RemoveConstantFoldedValueViaParent::visit ( SgNode* node )
   {
  // This is an alternative implementation that allows us to handle expression that are not 
  // traversed in the AST (e.g. types like SgArrayType which can contain expressions).

     ROSE_ASSERT(node != NULL);

  // DQ (3/11/2006): Set NULL pointers where we would like to have none.
#if 0
     printf ("In RemoveConstantFoldedValueViaParent::visit(): node = %p = %s \n",node,node->class_name().c_str());
#endif

  // DQ (10/12/2012): Turn this on so that we can detect failing IR nodes (failing later) that have valid originalExpressionTrees.
  // DQ (10/12/2012): Turn this back off because it appears to fail...
#if 0
     SgExpression* exp = isSgExpression(node);
     if (exp != NULL)
        {
          SgExpression* originalExpressionTree = exp->get_originalExpressionTree();
          if (originalExpressionTree != NULL)
             {
               SgNode* parent = exp->get_parent();
               if (parent != NULL)
                  {
                    printf ("Current IR node with SgExpression parent = %p = %s child = %p = %s originalExpressionTree = %p = %s \n",parent,parent->class_name().c_str(),node,node->class_name().c_str(),originalExpressionTree,originalExpressionTree->class_name().c_str());
                    bool traceReplacement = true;
                    ConstantFoldedValueReplacer r(traceReplacement, exp);
                    parent->processDataMemberReferenceToPointers(&r);
                 // node->processDataMemberReferenceToPointers(&r);
                  }

            // Set the originalExpressionTree to NULL.
               exp->set_originalExpressionTree(NULL);

            // Set the parent of originalExpressionTree to be the parent of exp.
               originalExpressionTree->set_parent(parent);

            // And then delete the folded constant.
               SageInterface::deleteAST(exp);
             }
        }
#endif

     SgArrayType* arrayType = isSgArrayType(node);
     if (arrayType != NULL)
        {
#if 0
          printf ("Found an array type arrayType = %p arrayType->get_index() = %p \n",arrayType,arrayType->get_index());
#endif
          SgExpression* index = arrayType->get_index();
          if (index != NULL)
             {
#if 0
               printf ("Fixup array index = %p = %s (traverse index AST subtree) \n",index,index->class_name().c_str());
#endif
               RemoveConstantFoldedValue astFixupTraversal;
               astFixupTraversal.traverse(index);
#if 0
               printf ("DONE: Fixup array index = %p (traverse index AST) \n\n\n\n",index);
#endif
#if 0
               printf ("Found an array index = %p (fixup index directly) \n",index);
#endif
            // Handle the case where the original expression tree is at the root of the subtree.
               SgExpression* originalExpressionTree = index->get_originalExpressionTree();
               if (originalExpressionTree != NULL)
                  {
#if 0
                    printf ("Found an originalExpressionTree in the array index originalExpressionTree = %p \n",originalExpressionTree);
#endif
                 // DQ (6/12/2013): This appears to be a problem in EDG 4.7 (see test2011_117.C).
                    std::vector<SgExpression*> redundantChainOfOriginalExpressionTrees;
                    if (originalExpressionTree->get_originalExpressionTree() != NULL)
                       {
#if 0
                         printf ("Detected originalExpressionTree nested directly within the originalExpressionTree \n",
                              originalExpressionTree,originalExpressionTree->class_name().c_str(),
                              originalExpressionTree->get_originalExpressionTree(),originalExpressionTree->get_originalExpressionTree()->class_name().c_str());
#endif
                      // Loop to the end of the chain of original expressions (which EDG 4.7 should never have constructed).
                         while (originalExpressionTree->get_originalExpressionTree() != NULL)
                            {
#if 0
                              printf ("Looping through a chain of originalExpressionTrees \n");
#endif
                           // Save the list of redundnat nodes so that we can delete them properly.
                              redundantChainOfOriginalExpressionTrees.push_back(originalExpressionTree);

                              originalExpressionTree = originalExpressionTree->get_originalExpressionTree();
                            }
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }

                    arrayType->set_index(originalExpressionTree);
                    originalExpressionTree->set_parent(arrayType);

                    index->set_originalExpressionTree(NULL);

                 // printf ("DEBUGING: skip delete of index in array type \n");
                    delete index;

                 // DQ (6/12/2013): Delete the nodes that we had to skip over (caused by chain of redundant entries from EDG 4.7).
                    std::vector<SgExpression*>::iterator i = redundantChainOfOriginalExpressionTrees.begin();
                    while (i != redundantChainOfOriginalExpressionTrees.end())
                       {
#if 0
                         printf ("deleting the redundnat originalExpressionTree chain caused by EDG 4.7 (delete %p = %s) \n",*i,(*i)->class_name().c_str());
#endif
                         delete *i;
                         i++;
                       }

                    index = NULL;
                  }
             }
        }

     SgVariableDefinition* variableDefinition = isSgVariableDefinition(node);
     if (variableDefinition != NULL)
        {
#if 0
          printf ("Found a SgVariableDefinition \n");
#endif
          SgExpression* bitfieldExp = variableDefinition->get_bitfield();
          if (bitfieldExp != NULL)
             {
#if 0
               printf ("Fixup bitfieldExp = %p (traverse bitfieldExp AST subtree) \n",bitfieldExp);
#endif
               RemoveConstantFoldedValue astFixupTraversal;
               astFixupTraversal.traverse(bitfieldExp);

            // Handle the case where the original expression tree is at the root of the subtree.
               SgExpression* originalExpressionTree = bitfieldExp->get_originalExpressionTree();
               if (originalExpressionTree != NULL)
                  {
#if 0
                 // DQ (9/18/2011): This code will not work since the bitfile data member in SgVariableDefinition is a SgUnsignedLongVal instead of a SgExpression.
                    variableDefinition->set_bitfield(originalExpressionTree);
                    originalExpressionTree->set_parent(variableDefinition);

                    bitfieldExp->set_originalExpressionTree(NULL);
                    delete bitfieldExp;
                    bitfieldExp = NULL;
#else
                 // The ROSE AST needs to be fixed to handle more general expressions for bitfield widths (this does not effect the CFG).
                 // TODO: Change the data type of the bitfield data member in SgVariableDefinition.

                 // DQ (1/20/2014): This has been done now.
                 // printf ("Member data bitfield widths need to be changed (in the ROSE IR) to support more general expressions (can't fix this original expression tree) \n");
#endif
#if 0
                 // This case is not handled yet!
                    printf ("Found an original expression tree in a bitfield expression \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
        }
   }



// **************************************************************
// **************************************************************
//    Verification of all original expression trees being reset
// **************************************************************
// **************************************************************

void verifyOriginalExpressionTreesSetToNull( SgNode* node )
   {
  // This is the interface function called by resetConstantFoldedValues(SgNode*).

  // DQ (3/11/2006): Introduce tracking of performance of ROSE.
     TimingPerformance timer1 ("Fixup Constant Folded Values (remove the original expression tree, leaving the constant folded values):");

  // This is a pass over the AST (but not hidden subtrees in array types and bitfield expressions).
     DetectOriginalExpressionTreeTraversal t1;
     t1.traverse(node,preorder);

  // This is a traversal over the expreesion trees in array types and bitfield expressions.
  // This is a simpler test to pass since it will not visit orphaned expression trees.
     DetectHiddenOriginalExpressionTreeTraversal t2;
     t2.traverseMemoryPool();

#if 0
  // This is a memory pool traversal over all expression and is difficult to pass since there can be ophaned expressions (which will be fixed).
  // This verifies that we have cleared all original expression trees saved into the 
  // AST by the front-end translation into ROSE.
     VerifyOriginalExpressionTreesSetToNull verifyFixup;
     verifyFixup.traverseMemoryPool();
#endif
   }


void
DetectHiddenOriginalExpressionTreeTraversal::visit ( SgNode* node )
   {
  // We only want to search for original expression trees where they can be hidden.

     ROSE_ASSERT(node != NULL);

     SgArrayType* arrayType = isSgArrayType(node);
     if (arrayType != NULL)
        {
#if 0
          printf ("Found an array type arrayType = %p (looking for original expression tree) \n",arrayType);
#endif
          SgExpression* index = arrayType->get_index();
          if (index != NULL)
             {
               DetectOriginalExpressionTreeTraversal t;
               t.traverse(index,preorder);
             }
        }

#if 0
  // DQ (9/18/2011): This code will not work since the bitfile data member in SgVariableDefinition is a SgUnsignedLongVal instead of a SgExpression.
     SgVariableDefinition* variableDefinition = isSgVariableDefinition(node);
     if (variableDefinition != NULL)
        {
#if 0
          printf ("Found a SgVariableDefinition (looking for original expression tree) \n");
#endif
          SgExpression* bitfieldExp = variableDefinition->get_bitfield();
          if (bitfieldExp != NULL)
             {
               DetectOriginalExpressionTreeTraversal t;
               t.traverse(bitfieldExp,preorder);
             }
        }
#endif
   }



void
DetectOriginalExpressionTreeTraversal::visit ( SgNode* node )
   {
  // This is used to operate on the AST and on subtree that are hidden in SgArrayType and bitfile expressions.

     ROSE_ASSERT(node != NULL);

  // printf ("In DetectOriginalExpressionTreeTraversal::visit(): node = %s \n",node->class_name().c_str());

     SgExpression* exp = isSgExpression(node);
     if (exp != NULL)
        {
          SgExpression* originalExpressionTree = exp->get_originalExpressionTree();

          if (originalExpressionTree != NULL)
             {
               printf ("Error: DetectOriginalExpressionTreeTraversal::visit() -- there is a valid originalExpressionTree = %p = %s on node = %p = %s \n",originalExpressionTree,originalExpressionTree->class_name().c_str(),exp,exp->class_name().c_str());
               ROSE_ASSERT(originalExpressionTree->get_startOfConstruct() != NULL);
               originalExpressionTree->get_startOfConstruct()->display("Error: DetectOriginalExpressionTreeTraversal::visit()");
             }

       // DQ (6/12/2013): Commented out as part of EDG 4.7 testing.
#if 0
          ROSE_ASSERT(originalExpressionTree == NULL);
#endif
        }
   }



void
VerifyOriginalExpressionTreesSetToNull::visit ( SgNode* node )
   {
  // This traversal is used to verify that all of the original expression trees in the AST have
  // been either deleted (optional) or used to replace the constant folded values (the default
  // for the ROSE AST so that we can preserve the greatest amount of source-to-source detail).

  // Note that it can detect problems that are due to orphaned expressions in the AST.
  // An example is test2011_138.C where the multidimensional array indexing causes and orphaned 
  // expression to be created and it has a original expression tree.  Since the orphaned expression 
  // can't be reached we can eliminate the original expression tree.  The bug in ROSE is that there
  // is an orphaned expression tree not that there is a remaining original expression tree.

  // We need a mechanism to detect nodes that exist in the AST and are not pointed to by any other 
  // IR node (and then we have to decide if parent pointers count).

     ROSE_ASSERT(node != NULL);

  // printf ("In VerifyOriginalExpressionTreesSetToNull::visit(): node = %s \n",node->class_name().c_str());

     SgExpression* exp = isSgExpression(node);
     if (exp != NULL)
        {
          SgExpression* originalExpressionTree = exp->get_originalExpressionTree();

          if (originalExpressionTree != NULL)
             {
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("Error: there is a valid originalExpressionTree = %p = %s on node = %p = %s \n",originalExpressionTree,originalExpressionTree->class_name().c_str(),exp,exp->class_name().c_str());
#endif
             }
#if 0 // Liao debugging 11/13/2012
          ROSE_ASSERT(originalExpressionTree == NULL);
#endif

       // Allow us to ignore the cases of originalExpressionTrees hidden in array types.
       // I want to narrow down the failing tests codes to eliminate this case which is handled separately.

          if (originalExpressionTree != NULL)
             {
#if 0
               SgNode* parent = exp;

            // Note that test2011_121.C fails to be either a SgArrayType or a SgVariableDefinition (failing in some part of "complex" header file).
            // test2005_203.C demonstrates the use of constant folding in bitfield specifications.
            // while (parent != NULL && isSgArrayType(parent) == NULL) 
               while (parent != NULL && isSgArrayType(parent) == NULL && isSgVariableDefinition(parent) == NULL) 
                  {
                    parent = parent->get_parent();
                  }

               if (isSgArrayType(parent) == NULL)
                  {
                    printf ("So what is the parent: parent = %p = %s \n",parent, (parent != NULL) ? parent->class_name().c_str() : "NULL");
                  }
               ROSE_ASSERT(isSgArrayType(parent) != NULL || isSgVariableDefinition(parent) != NULL);
#else
#ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
               printf ("In VerifyOriginalExpressionTreesSetToNull(): originalExpressionTree = %p = %s on node = %p = %s Ingoring originalExpressionTree != NULL \n",
                    originalExpressionTree,originalExpressionTree->class_name().c_str(),exp,exp->class_name().c_str());
#endif
#endif
             }
        }
   }
