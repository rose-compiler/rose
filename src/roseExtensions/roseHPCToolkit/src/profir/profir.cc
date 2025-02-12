// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/profir/profir.cc
 *  \brief Intermediate representation for HPCToolkit profile data.
 *  \ingroup ROSEHPCT_PROFIR
 *
 *  $Id: profir.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

/*
 * This software was produced with support in part from the Defense Advanced
 * Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
 * Nothing in this work should be construed as reflecting the official policy
 * or position of the Defense Department, the United States government,
 * or Rice University.
 */

#include "rosehpct/profir/profir.hh"
#include <boost/lexical_cast.hpp> // DXN: converting int to string

using namespace std;
using namespace RoseHPCT;
#include <iostream>
#include <sstream>
#include <string>
#include <cassert> 

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

std::string RoseHPCT::Named::toString(void) const
{
  return name_;
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

std::string Metric::toString(void) const
{
  std::ostringstream o;
  o<<getValue();
  return RoseHPCT::Named::toString()+":"+o.str();
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
  for (ConstMetricIterator iter =beginMetric(); iter!=endMetric(); iter++)
  {
    Metric current = *iter;
    if (current.getName() == m.getName())
    {
      cerr<<"Fatal error: Observable::addMetric() tries to add the same type of metric twice."<<endl;
      assert(false);
    }
  }

  metrics_.push_back (m);
}

double
Observable::getMetricValue (size_t i) const
{
  return getMetric (i).getValue ();
}

double
Observable::getMetricValue (const string& mname) const
{
  double result =0.0;
  for (ConstMetricIterator iter =beginMetric(); iter!=endMetric(); iter++)
  {
    Metric m = *iter;
    if (m.getName() == mname)
    {
      result = m.getValue();
      break;
    }
  }
  return result;  
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

std::string Observable::toString() const
{
  std::string result;
  ConstMetricIterator iter= beginMetric();
  for (;iter!=endMetric(); iter++)
  { 
    if (iter!=beginMetric())
      result+=" ";
    result+=(*iter).toString(); 
  }
  return result;
}

/* ---------------------------------------------------------------- */

Located::Located (void)
{
  setLines (0, 0);
  setFileNode(0);
}

Located::Located (const Located& l)
{
  setLines (l.getFirstLine (), l.getLastLine ());
  setFileNode(l.getFileNode());
}

Located::Located (size_t b, size_t e, File* f/*=NULL*/)
  : begin_(b), end_(e), filenode_(f)
{
}

Located::~Located (void)
{
}

File* Located::getFileNode(void) const
{
  return filenode_;
}
void Located::setFileNode(File* filenode)
{
  filenode_= filenode;
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

std::string Located::toString() const
{
  std::string result;
  std::ostringstream o1,o2;
  o1<<begin_;
  o2<<end_;
  result = o1.str()+"-"+o2.str();
  if (filenode_)
    result= filenode_->toString()+" "+ result;
  else
    result = " no_filenode "+result;
  return result;
}
/* ---------------------------------------------------------------- */
std::string IRNode::toString() const
{
  std::string result;
  result += Named::toString()+ " " + Observable::toString();
  return result;
}
/* ---------------------------------------------------------------- */
std::string Program::toString() const
{
  std::string result;
  result = "Program "+IRNode::toString();
  return result;
}

std::string SecHeader::toString() const
{
  std::string result;
  result = "SecHeader " + IRNode::toString();
  return result;
}

std::string MetricTable::toString() const
{
  std::string result;
  result = "MetricTable " + IRNode::toString();
  return result;
}

std::string MetricElement::toString() const
{
  std::string result;
  result = "MetricElement " + IRNode::toString();
  return result;
}

std::string SecFlatProfileData::toString() const
{
  std::string result;
  result = "SecFlatProfileData " + IRNode::toString();
  return result;
}

std::string Group::toString() const
{
  std::string result;
  result = "Group "+ IRNode::toString();
  return result;
}
std::string Module::toString() const
{
  std::string result;
  result = "Module "+ IRNode::toString();
  return result;
}
std::string File::toString() const
{
  std::string result;
  result = "File "+ IRNode::toString();
  return result;
}

Procedure::Procedure (long int i, const std::string& name, long int l) 
  : IRNode{name},
    Located{static_cast<size_t>(l),static_cast<size_t>(l),nullptr},
    id{i}
{
  std::cerr << "Procedure::Procedure: unused private variable id = " << id << "\n";
}

std::string Procedure::toString() const
{
  std::string result;
  result = "Procedure "+IRNode::toString() + " @ " + Located::toString();
  return result;
}

CallSite::CallSite(long int i, long int l)
  : id{i}, line{l}
{
  std::cerr << "CallSite::CallSite: unused private variable id = " << id << "\n";
}

std::string CallSite::toString() const
{
  std::string result;
  result = "CallSite " + IRNode::toString()  + " @ line " + boost::lexical_cast<std::string>(line);
  return result;
}

ProcFrame::ProcFrame(long int i, const std::string &n, long int l)
  : id{i}, name{n}, line{l}
{
  std::cerr << "ProcFrame::ProcFrame: unused private variable id = " << id << "\n";
}

std::string ProcFrame::toString() const
{
  std::string result;
  result = "ProcFrame " + IRNode::toString()  + " @ line " + boost::lexical_cast<std::string>(line);
  return result;
}

Loop::Loop (const std::string& name, long int i, long int l) :
        IRNode (name), Located(l, l)
{
    id = i;
}

std::string Loop::toString() const
{
  std::string result;
  result = "Loop " +IRNode::toString() +" @ " + Located::toString();
  return result;
}

std::string Statement::toString() const
{
  std::string result;
  result = "Statement "+ IRNode::toString() + "@"+ Located::toString();
  return result;
}

Statement::Statement (void): id(0)
{
}

Statement::Statement (const Statement& s)
   : Located (s), id(s.getId())
{
    setHasMatchingSgNode(s.hasMatchingSgNode());
}

Statement::Statement (const std::string& name, id_t i, size_t l)
   : IRNode (name), Located(l, l), id(i)
{
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
