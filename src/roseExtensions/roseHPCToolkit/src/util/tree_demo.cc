// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file rosehpct/util/tree_demo.cc
 *  \brief Demonstrates how to use the tree data structure and
 *  traversal classes to construct and display a binary search tree.
 *  \ingroup ROSEHPCT_UTIL
 *
 *  $Id: tree_demo.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#include <cassert>
#include <iostream>
#include <string>

#include "rosehpct/util/general.hh"
#include "rosehpct/util/tree2.hh"

using namespace std;
using namespace GenUtil;
using namespace Trees;

/*! \name Identifiers for identifying child nodes of a binary tree. */
/*@{*/
#define LEFT 0  /*!< left child */
#define RIGHT 1 /*!< right child */
/*@}*/

/*!
 *  \name Identifiers for distinguishing between pre- and post-order
 * traversals.
 */
/*@{*/
typedef Types::Int2Type<0> Preorder; /*!< preorder */
typedef Types::Int2Type<1> Postorder; /*!< postorder */
/*@}*/

/*! \brief Defines concrete tree type. */
typedef Tree<double, int> TreeType;

/*! \brief Insert a value into a binary search tree. */
static
bool
insert (TreeType* subtree, TreeType::Val_t v)
{
  assert (subtree != NULL);

  if (v == subtree->value)
    return true;

  const TreeType::Ind_t child_id = (v < subtree->value) ? LEFT : RIGHT;
  TreeType* kid = subtree->getSubtree (child_id);

  if (kid == NULL)
    return (subtree->setChildValue (child_id, v)) != NULL;
  else
    return insert (kid, v);
}

/*! \brief Print a tree using a preorder traversal. */
static
void
printTree (TreeType* tree, Preorder)
{
  class PrintTree : public PreTraversal<TreeType>
  {
  public:
    PrintTree (void) : level_ (0) {}

  protected:
    virtual void descending (void) { level_++; }
    virtual void ascending (void) { level_--; }

    virtual void visit (TreeParamPtr_t tree) {
      cout << tabs (level_) << "[" << level_ << "] " << tree->value
	   << endl;
    }

  private:
    size_t level_;
  };

  cerr << "--- Preorder traversal ---" << endl;
  PrintTree printer;
  printer.traverse (tree);
}

/*! \brief Print a tree using a postorder traversal. */
static
void
printTree (TreeType* tree, Postorder)
{
  class PrintTree : public PostTraversal<TreeType>
  {
  public:
    PrintTree (void) : level_ (0) {}

  protected:
    virtual void descending (void) { level_++; }
    virtual void ascending (void) { level_--; }

    virtual void visit (TreeParamPtr_t tree) {
      cout << tabs (level_-1) << "[" << level_-1 << "] " << tree->value
	   << endl;
    }

  private:
    size_t level_;
  };

  cerr << "--- Postorder traversal ---" << endl;
  PrintTree printer;
  printer.traverse (tree);
}

/*! \brief Convert a string to a tree node's value type. */
static
TreeType::Val_t
toValue (const char* s)
{
  return atof (s);
}

/*! \brief Construct a binary search tree from a list of strings values to insert. */
static
TreeType *
buildTree (int num_elems, char** str_vals)
{
  if (num_elems <= 0) return NULL;

  TreeType* root = new TreeType (toValue (str_vals[0]));
  for (int i = 1; i < num_elems; i++)
    {
      TreeType::Val_t v_i = toValue (str_vals[i]);
      if (!insert (root, v_i))
	{
	  cerr << "*** Can't insert '" << v_i << "' [" << i << "]"
	       << " into the tree ***" << endl;
	  break;
	}
    }
  return root;
}

/*! \brief Driver for binary search tree demo. */
int
main (int argc, char** argv)
{
  if (argc < 2)
    {
      cerr << endl
	   << "usage: " << argv[0] << " <s1> [<s2> ...]" << endl
	   << endl;
      return 1;
    }

  assert (argc >= 2);
  TreeType* root = buildTree (argc-1, argv + 1);
  printTree (root, Preorder());
  printTree (root, Postorder());

  return 0;
}

/* eof */
