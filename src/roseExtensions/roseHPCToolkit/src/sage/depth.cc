/*!
 *  \file src/sage/depth.cc
 *  \brief Implements tree depth calculator.
 *  \ingroup ROSEHPCT_SAGE
 *
 *  $Id: depth.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#include "rose.h"

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
  return d;
}

/* eof */
