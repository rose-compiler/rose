// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/profir/profir.cc
 *  \brief Intermediate representation for HPCToolkit profile data.
 *  \ingroup ROSEHPCT_PROFIR
 *
 *  $Id: profir.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#include "rosehpct/profir/profir.hh"

using namespace std;
using namespace RoseHPCT;

/* ---------------------------------------------------------------- */

RoseHPCT::Named::Named (void)
  : name_ ("")
{
}

RoseHPCT::Named::Named (const string& name)
  : name_ (name)
{
}

RoseHPCT::Named::Named (const Named& n)
{
  setName (n.getName ());
}

RoseHPCT::Named::~Named (void)
{
}

const string &
RoseHPCT::Named::getName (void) const
{
  return name_;
}

void
RoseHPCT::Named::setName (const std::string& new_name)
{
  name_ = new_name;
}

/* ---------------------------------------------------------------- */

Metric::Metric (void)
  : value_(0)
{
}

Metric::Metric (const Metric& m)
{
  setName (m.getName ());
  setValue (m.getValue ());
}

Metric::Metric (const string& name, double value)
  : Named(name), value_(value)
{
}

Metric::~Metric (void)
{
}

double
Metric::getValue (void) const
{
  return value_;
}

void
Metric::setValue (double new_val)
{
  value_ = new_val;
}

/* ---------------------------------------------------------------- */

Observable::Observable (void)
{
}

Observable::Observable (const Observable& o)
{
  metrics_ = o.metrics_;
}

Observable::~Observable (void)
{
}

size_t
Observable::getNumMetrics (void) const
{
  return metrics_.size ();
}

const Metric&
Observable::getMetric (size_t i) const
{
  return metrics_[i];
}

void
Observable::addMetric (const Metric& m)
{
  metrics_.push_back (m);
}

double
Observable::getMetricValue (size_t i) const
{
  return getMetric (i).getValue ();
}

std::string
Observable::getMetricName (size_t i) const
{
  return getMetric (i).getName ();
}

Observable::MetricIterator
Observable::beginMetric (void)
{
  return metrics_.begin ();
}

Observable::ConstMetricIterator
Observable::beginMetric (void) const
{
  return metrics_.begin ();
}

Observable::MetricIterator
Observable::endMetric (void)
{
  return metrics_.end ();
}

Observable::ConstMetricIterator
Observable::endMetric (void) const
{
  return metrics_.end ();
}

/* ---------------------------------------------------------------- */

Located::Located (void)
{
  setLines (0, 0);
}

Located::Located (const Located& l)
{
  setLines (l.getFirstLine (), l.getLastLine ());
}

Located::Located (size_t b, size_t e)
  : begin_(b), end_(e)
{
}

Located::~Located (void)
{
}

size_t
Located::getFirstLine (void) const
{
  return begin_;
}

void
Located::setFirstLine (size_t l) 
{
  begin_ = l;
}

size_t
Located::getLastLine (void) const
{
  return end_;
}

void
Located::setLastLine (size_t l) 
{
  end_ = l;
}

void
Located::setLines (size_t b, size_t e)
{
  setFirstLine (b);
  setLastLine (e);
}

/* ---------------------------------------------------------------- */

Statement::Statement (void)
{
  setId (0);
}

Statement::Statement (const Statement& s)
  : Located (s)
{
  setId (s.getId ());
}

Statement::~Statement (void)
{
}

Statement::id_t
Statement::getId (void) const
{
  return id;
}

void
Statement::setId (Statement::id_t new_id)
{
  id = new_id;
}

/* eof */
