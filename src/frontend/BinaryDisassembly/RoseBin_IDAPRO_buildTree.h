/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 3Apr07
 * Decription : Code that actually builds the Rose Tree
 ****************************************************/

#ifndef __RoseBin_IDAPRO_buildTree__
#define __RoseBin_IDAPRO_buildTree__

#include <stdio.h>
#include <iostream>

// rose.h and sage3basic.h should not be included in librose header files. [Robb P. Matzke 2014-10-15]
// #include "rose.h"
#include "RoseBin_IDAPRO_exprTree.h"
#include "RoseBin_IDAPRO_substTree.h"
// #include "RoseBin_support.h"
#include "RoseBin_buildTree.h"

class RoseBin_IDAPRO_buildTree : public RoseBin_buildTree  {
 private:
  SgAsmExpression* previousExp;

  /****************************************************
   * create a BinaryNode
   ****************************************************/
  SgAsmExpression* convertBinaryNode(exprTreeType* expt, 
                                        std::list<SgAsmExpression*> *children, 
                                        const std::string& typeOfOperand);

  /****************************************************
   * return the replacement text for an expression
   ****************************************************/
  std::string getReplacementText(rose_hash::unordered_map< int, exprSubstitutionType> *rememberSubstitution, 
                            int operand_id, 
                            int expr_id_root,
                            int address);

  /****************************************************
   * return the comment for an expression (address)
   ****************************************************/
  std::string getComment(int address, rose_hash::unordered_map< int, std::string> *rememberComments);

 public:
  RoseBin_IDAPRO_buildTree() {previousExp=NULL; };
  ~RoseBin_IDAPRO_buildTree() {
    
  }

  /****************************************************
   * recursive algorithm
   * to create the subtree of one operand
   ****************************************************/
  SgAsmExpression* resolveRecursivelyExpression(int address,
                                                   int expr_id_root, 
                                                   const std::map <int, std::vector<int> >& subTree, 
                                                   const std::string& typeOfOperand,
                                                   std::vector < exprTreeType > *rememberExpressionTree,
                                                   int operand_id,
                                                   rose_hash::unordered_map <int, exprSubstitutionType> *rememberSubstitution,
                                                   rose_hash::unordered_map <int, std::string> *rememberComments);


};

#endif



