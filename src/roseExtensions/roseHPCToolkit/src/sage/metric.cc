// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/sage/metric.cc
 *  \brief Implements metric attribute support for Sage III trees.
 *  \ingroup ROSEHPCT_SAGE
 *
 *  $Id: metric.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#include "rose.h"

#include "rosehpct/util/general.hh"
#include "rosehpct/sage/sage.hh"

/* ---------------------------------------------------------------- */

using namespace std;
using namespace GenUtil;
using namespace RoseHPCT;

/* ---------------------------------------------------------------- */

MetricAttr *
RoseHPCT::getMetric (const string& metric_name, SgNode* node)
  {
    if (node->attributeExists (metric_name))
      {
        AstAttribute* raw_attr = node->getAttribute (metric_name);
        return dynamic_cast<MetricAttr *> (raw_attr);
      }
    return 0; // not found
  }

/* ---------------------------------------------------------------- */

MetricAttr::MetricAttr (void)
  : name_ ("")
{
}

MetricAttr::MetricAttr (const std::string& name, double value,
                        bool is_derived)
  : MetricAttribute(value,false), name_ (name)
{
}


MetricAttr *
MetricAttr::clone (void) const
{
  return new MetricAttr (*this);
}


MetricAttr&
MetricAttr::operator+= (const MetricAttr& b)
{
  if (name_ == "")
    name_ = b.name_;
  else if (b.name_ != "" && b.name_ != name_)
    {
      name_ = "";  // this is an error, so turn self into empty attribute
      value_ = 0;
      is_derived_ = false;
      return *this;
    }

  value_ += b.value_;
  is_derived_ = true;
  return *this;
}

std::string MetricAttr::getName () const
{
    return name_;
}

/*!
 *
 *  The sum is the simple sum, value(a) + value(b), if and only if
 *  name(a) equals name(b), or one or both of name(a), name(b) is
 *  the empty string. Otherwise, the returned metric has an empty
 *  name and a value of 0.
 *
 *  \note By the condition above, the sum of many attributes with
 *  differing names is not well-defined.
 */
MetricAttr
RoseHPCT::operator+ (const MetricAttr& a, const MetricAttr& b)
{
  MetricAttr sum_attr (a);
  sum_attr += b;
  return sum_attr;
}

/* eof */
