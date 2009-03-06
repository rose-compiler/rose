// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/sage/propagate.cc
 *  \brief Implements metric propagation support for Sage III trees.
 *  \ingroup ROSEHPCT_SAGE
 *
 *  This propagator pushes metric attributes upward in the tree, so
 *  that all scopes have a metric attribute. The propagation procedure
 *  uses synthesized attributes, computing an attribute at a given
 *  node by summing the attribute over the immediate child nodes.
 *
 *  Only certain types of nodes will have attributes. These nodes are
 *  specializations of templated member function,
 *  MetricScopePropagator::eval ().
 *
 *  \note Metric propagation as implemented in MetricScopePropagator
 *  is only approximation to the "true" metric value at a given node
 *  since the computation is flow-insensitive.
 *
 *  $Id: propagate.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#include "rose.h"
#include "rosehpct/rosehpct.hh"
#include "rosehpct/util/general.hh"
#include "rosehpct/sage/sage.hh"

using namespace std;
using namespace GenUtil;
using namespace RoseHPCT;

MetricScopePropagator::MetricScopePropagator (const string& name)
  : metric_name_ (name)
{
}

/*!
 *  \brief Given a list of metric attributes, computes and returns
 *  their sum.
 *
 *  \note See the sum operator for metric attributes for the
 *  conditions under which a sum of metric attributes may be correctly
 *  computed.
 *  If the metric name of a metric from attr_list does not match attr_name,
 *  the result metricAttr will be empty. 
 */
static
MetricAttr
sumMetricAttributes (const string& attr_name,
                     const AstBottomUpProcessing<MetricAttr>::SynthesizedAttributesList& attr_list)
{
  MetricAttr total (attr_name, 0, true);
  for (vector<MetricAttr>::const_iterator i = attr_list.begin ();
       i != attr_list.end ();
       ++i)
    {
      total += *i;
    }
  return total;
}

/*!
 *  \brief Returns the filename component of a Sg_File_Info object, or
 *  the empty string if it cannot be determined.
 */
static
string
getFilename (const Sg_File_Info* info)
{
  return (info == NULL) ? "" : info->get_raw_filename ();
}

/*!
 *  \brief Returns the line number component of a Sg_File_Info object,
 *  or 0 if it cannot be determined.
 */
static
int
getLineNumber (const Sg_File_Info* info)
{
  return (info == NULL) ? 0 : info->get_line ();
}

string
RoseHPCT::getFilename (const SgLocatedNode* node)
{
  return (node == NULL) ? "" : ::getFilename (node->get_startOfConstruct ());
}

int
RoseHPCT::getLineStart (const SgLocatedNode* node)
{
  return (node == NULL) ? 0 : ::getLineNumber (node->get_startOfConstruct ());
}

int
RoseHPCT::getLineEnd (const SgLocatedNode* node)
{
  return (node == NULL) ? 0 : ::getLineNumber (node->get_endOfConstruct ());
}

string
RoseHPCT::toFileLoc (const SgLocatedNode* node)
{
  if (node == NULL) return "";

  string class_name =
    string("(")
    + string(node->sage_class_name ())
    + string(")");

  Sg_File_Info* info_start = node->get_startOfConstruct ();
  Sg_File_Info* info_end = node->get_endOfConstruct ();

  string filename = getFilename (info_start);
  int line_start = getLineNumber (info_start);
  int line_end = getLineNumber (info_end);

  return ::toFileLoc (filename, line_start, line_end) + " " + class_name;
}
// add values from attr_list and set it to the current node's metric
template <class SgType>
bool
MetricScopePropagator::eval (SgType* node,
			     const SynthesizedAttributesList& attr_list)
{
  if (node == NULL) return false;

  MetricAttr total = sumMetricAttributes (metric_name_, attr_list);
  if (total.getValue () > 0)
    if (total.getName () == metric_name_)
      {
        if (RoseHPCT::enable_debug)
        {
          std::ostringstream o;
          o<<"\n/* ROSE-HPCT propagated metrics "
            << metric_name_ << ":"<<total.getValue() 
            << "["<< node->sage_class_name() << " at " << node
            << "] */\n";

          SageInterface::addTextForUnparser(node,o.str(),AstUnparseAttribute::e_after); 
          cerr << "  [" << toFileLoc (node) << "]"
            << " " << total.toString ()
            << endl;
        }
        // ignore this step if it already has a metric with the same name
        // This is a double check, since eval() is called for nodes without the metric only
        //node->setAttribute (metric_name_, new MetricAttr (total));
        if (!node->attributeExists(metric_name_))
        {  
          node->setAttribute (metric_name_, new MetricAttr (total));
          return true;
        }
        else
          return false;
      }
  return false;
}

/*!
 *  \brief Checks if a given Sage node already has a metric attribute
 *  with the specified name and returns a pointer to this attribute if
 *  so, or NULL otherwise.
 */
static
MetricAttr *
checkMetricAttribute (SgNode* node, const string& metric_name)
{
  if (node->attributeExists (metric_name))
    {
      AstAttribute* raw_attr = node->getAttribute (metric_name);
      return dynamic_cast<MetricAttr *> (raw_attr);
    }
  return NULL;
}

/*!
 *  \brief Synthesizes a metric attribute by summing the current list
 *  of child attributes.
 *
 *  However, if the specified node already has the metric, this value
 *  takes precedence and the child attributes are ignored.
 */
MetricAttr
MetricScopePropagator::evaluateSynthesizedAttribute
   (SgNode* node, SynthesizedAttributesList attr_list)
{
  MetricAttr* raw_attr = checkMetricAttribute (node, metric_name_);
  // try to synthesize only when the node has no existing such metric
  if (raw_attr == NULL) 
  {
    bool can_eval =
      eval (isSgForInitStatement (node), attr_list)
      || eval (isSgScopeStatement (node), attr_list)
      || eval (isSgFunctionDeclaration (node), attr_list)
      ;
    if (can_eval)
      raw_attr = checkMetricAttribute (node, metric_name_);
  }
  if (raw_attr != NULL)
    return *raw_attr;
  else
  {
    MetricAttr empty_attr;
    return empty_attr;
  }
}

void
RoseHPCT::propagateMetrics (SgProject* node,
                            const MetricNameList_t& metrics,
                            const char* prefix)
{
  if (prefix == NULL)
    propagateMetrics (node, metrics, string (""));
  else
    propagateMetrics (node, metrics, string (prefix));
}

void
RoseHPCT::propagateMetrics (SgProject* node,
                            const MetricNameList_t& metrics,
                            const string& prefix)
{
  for (MetricNameList_t::const_iterator i = metrics.begin ();
       i != metrics.end ();
       ++i)
    {
      string name = prefix + *i;
      if (enable_debug)
        cerr << "Propagating " << name << "..." << endl;
      MetricScopePropagator prop (name);
      prop.traverseInputFiles (node);
    }
}

/* eof */
