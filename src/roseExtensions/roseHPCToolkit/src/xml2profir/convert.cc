// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/xml2profir/convert.cc
 *  \brief Converts an XML HPCToolkit profile file into an RoseHPCTIR
 *  tree.
 *  \ingroup ROSEHPCT_XML2PROFIR
 *
 *  $Id: convert.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#include <cassert>
#include <cstdlib>

#include "rosehpct/util/general.hh"
#include "rosehpct/util/types.hh"
#include "rosehpct/xml2profir/xml2profir.hh"

using namespace std;
using namespace GenUtil;
using namespace XMLUtil;
using namespace RoseHPCT;

/*!
 *  \brief If the given XML tree element is a metric, attach it to the
 *  given node and return true; return false otherwise.
 */
static bool
addMetric (const XMLElem& e, Observable* node)
{
  if (node == NULL || e.name != "M")
    return false;
  Metric m (e.getAttr ("n"), atof (e.getAttr ("v")));
  node->addMetric (m);
  return true;
}

/*!
 *  \brief Implements conversion of HPCToolkit XML tree representation
 *  to the RoseHPCT IR representation.
 *
 *  This class uses a pre-order traversal, maintaining the following
 *  invariants during the traversal:
 *  - saved_parents_ is the list of nodes whose children are still
 *  being processed.
 *  - At each call to 'visit ()', saved_parents_.top () is the current
 *  node's parent.
 *  - The first node pushed onto saved_parents_ is the root program
 *  node of the XML tree.
 *  - When a 'metric' node is encountered, 'visit ()' pushes a dummy
 *  node (NULL) onto the list of parents, since metrics are not
 *  explicitly represented by IR nodes but rather attached to IR
 *  nodes.
 *
 *  \note After each traversal, the user is responsible for deleting
 *  the new IR tree.
 */
class XMLToProfIRConverter : public Trees::PreTraversal<XMLNode_t>
{
public:
  XMLToProfIRConverter (void);
  virtual ~XMLToProfIRConverter (void);
  virtual void visit (TreeParamPtr_t tree);

  /*! \brief Call after traversal to obtain the root node of the IR tree */
  IRTree_t* getRoot (void) const;

protected:
  /*! \brief Pop node to maintain the saved_parents_ invariant. */
  virtual void ascending (void);

private:
  /*! \brief Root of the IR tree */
  IRTree_t* prog_root_;

  /*!
   *  List of parents being processed.
   */
  std::stack<IRTree_t *> saved_parents_;
};

XMLToProfIRConverter::XMLToProfIRConverter (void)
  : prog_root_ (NULL)
{
}

XMLToProfIRConverter::~XMLToProfIRConverter (void)
{
}

void
XMLToProfIRConverter::visit (TreeParamPtr_t tree)
{
  /* Conversion only starts once we encounter a program root node. */
  if (prog_root_ == NULL)
    {
      ProfIRFactory_t::instance ().setElement (tree->value);
      IRTree_t::Val_t v =
	ProfIRFactory_t::instance ().createObject ("SecFlatProfile");
      if (v != NULL)
	{
	  prog_root_ = new IRTree_t (v);
	  saved_parents_.push (prog_root_);
	}
      return;
    }

  assert (prog_root_ != NULL);

  if (saved_parents_.size () <= 0)
    return;

  const XMLElem& e = tree->value;
  IRTree_t* parent = saved_parents_.top ();

  if (addMetric (e, parent->value))
    {
      // Push a dummy node for 'ascending' to pop off
      saved_parents_.push (NULL);
      return;
    }

  ProfIRFactory_t::instance ().setElement (e);
  IRTree_t::Val_t new_val =
    ProfIRFactory_t::instance ().createObject (e.name);
  assert (new_val != NULL);

  size_t num_kids = parent->getNumChildren ();
  IRTree_t* child = parent->setChildValue (num_kids+1, new_val);
  saved_parents_.push (child);
}

void
XMLToProfIRConverter::ascending (void)
{
  if (saved_parents_.size () > 0)
    saved_parents_.pop ();
}

IRTree_t *
XMLToProfIRConverter::getRoot (void) const
{
  return prog_root_;
}

IRTree_t *
RoseHPCT::convertXMLToProfIR (const XMLNode_t* root)
{
  XMLToProfIRConverter converter;
  converter.traverse (root);
  return converter.getRoot ();
}

/* eof */
