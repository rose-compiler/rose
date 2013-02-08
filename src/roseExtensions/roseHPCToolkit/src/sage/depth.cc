/*!
 *  \file src/sage/depth.cc
 *  \brief Implements tree depth calculator.
 *  \ingroup ROSEHPCT_SAGE
 *
 *  $Id: depth.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#include "sage3basic.h"

#include "rosehpct/util/general.hh"
#include "rosehpct/sage/sage.hh"

using namespace std;
using namespace RoseHPCT;
using namespace GenUtil;

DepthAttr::DepthAttr (size_t d)
  : depth (d)
{
}

string
DepthAttr::toString (void)
{
  return GenUtil::toString (depth);
}

const string TreeDepthCalculator::ATTR_NAME ("tree_depth");

TreeDepthCalculator::TreeDepthCalculator (SgProject* root)
{
  traverseInputFiles (root, preorder);
}

size_t
TreeDepthCalculator::evaluateInheritedAttribute (SgNode* node,
						 size_t d_parent)
{
  size_t d = d_parent + 1;
  if (node->attributeExists (ATTR_NAME))
    {
      DepthAttr* old_attr =
	dynamic_cast<DepthAttr *> (node->getAttribute (ATTR_NAME));
      ROSE_ASSERT (old_attr != NULL);
      old_attr->depth = d;
    }
  else
    node->setAttribute (ATTR_NAME, new DepthAttr (d));
  
  // Liao, add assertion here
  // numbers start from 0 (SgProject) -> 1 (SgFileList) -> 2 (SgSourceFile) -> 3 (SgGlobal)
  if (isSgSourceFile(node))
  {
    ROSE_ASSERT (d==2);
  }

  if (isSgGlobal(node))
  {
    ROSE_ASSERT (d==3);
  }
//  cout<<"-----------------------------------------------"<<endl;
//  cout<<"Node:"<<node<<" "<<node->class_name()<<" depth="<<d<<endl;
//  cout<<node->unparseToString()<<endl;
  return d;
}

/* eof */
