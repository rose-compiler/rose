#ifndef FIXUP_CONSTANT_FOLDED_VALUES_H
#define FIXUP_CONSTANT_FOLDED_VALUES_H

/*! \brief This traversal can either replace the constant folded values with the original expression tree or leave the constant folded value and remove the original expression tree.

    This traversal uses the Memory Pool traversal to fixup SgValueExp IR nodes that contain valid pointers to original expression trees.
 */

// ******************************************
// This is the API function we use externally
// ******************************************
//! This function acts in two different ways
//  Remove originalExpressionTree, which is used to preserver the original expression tree before constant folding applied by EDG 
//  Or replace folded expressions with their original expression trees
void resetConstantFoldedValues( SgNode* node );

#endif
