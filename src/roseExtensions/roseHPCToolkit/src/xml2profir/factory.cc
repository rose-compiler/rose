// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/xml2profir/factory.cc
 *  \brief Implements a factory for instantiating profile IR nodes
 *  from XML elements.
 *  \ingroup ROSEHPCT_XML2PROFIR
 *
 *  $Id: factory.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

/*
 * This software was produced with support in part from the Defense Advanced
 * Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
 * Nothing in this work should be construed as reflecting the official policy
 * or position of the Defense Department, the United States government,
 * or Rice University.
 */

#include <cassert>
#include <cstdlib>

#include "rosehpct/util/general.hh"
#include "rosehpct/xml2profir/xml2profir.hh"

using namespace std;
using namespace GenUtil;
using namespace XMLUtil;
using namespace RoseHPCT;

/*!
 *  \name "Factory" routines that convert XMLElem objects into the
 *  corresponding RoseHPCTIR nodes.
 */
/*@{*/
static IRNode *
newProgram (const XMLElem& e)
{
  return (e.name == "SecFlatProfile") ? new Program (e.getAttr ("n")) : NULL;
}

static IRNode *
newSecHeader (const XMLElem& e)
{
  return (e.name == "SecHeader") ? new SecHeader : NULL;
}

static IRNode *
newMetricTable (const XMLElem& e)
{
  return (e.name == "MetricTable") ? new MetricTable : NULL;
}

static IRNode *
newMetricElement (const XMLElem& e)
{
  return (e.name == "Metric") ?
          new MetricElement(e.getAttr("i"), e.getAttr("n"), e.getAttr("v"), e.getAttr("t"), e.getAttr("show")):
          NULL;
}

static IRNode *
newSecFlatProfileData (const XMLElem& e)
{
  return (e.name == "SecFlatProfileData") ? new SecFlatProfileData : NULL;
}

static IRNode *
newGroup (const XMLElem& e)
{
  return (e.name == "G") ? new Group : NULL;
}

static IRNode *
newModule (const XMLElem& e)
{
  return (e.name == "LM") ? new Module (e.getAttr ("n")) : NULL;
}

static IRNode *
newFile (const XMLElem& e)
{
  return (e.name == "F") ? new File (e.getAttr ("n")) : NULL;
}

static IRNode *
newProcedure (const XMLElem& e)
{
  return (e.name == "P")
    // ? new Procedure (e.getAttr ("n"), atol (e.getAttr ("b")), atol (e.getAttr ("e")))
    ? new Procedure (atol (e.getAttr ("i")),e.getAttr ("n"), atol (e.getAttr ("l")))
    : NULL;
}

static IRNode *
newCallSite (const XMLElem& e)
{
  return (e.name == "C")
    ? new CallSite (atol (e.getAttr ("i")), atol (e.getAttr ("l")))
    : NULL;
}

static IRNode *
newProcFrame (const XMLElem& e)
{
  return (e.name == "PF")
    ? new ProcFrame (atol (e.getAttr ("i")), e.getAttr("n"), atol (e.getAttr ("l")))
    : NULL;
}

static IRNode *
newLoop (const XMLElem& e)
{
  return (e.name == "L")
    // ? new Loop (e.getAttr ("n"), atol (e.getAttr ("b")), atol (e.getAttr ("e")))
    ? new Loop (e.getAttr ("n"), atol (e.getAttr ("i")), atol (e.getAttr ("l")))
    : NULL;
}

static IRNode *
newStatement (const XMLElem& e)
{
  return (e.name == "S" || e.name == "LN")
    // ? new Statement (e.getAttr ("n"), atol (e.getAttr ("b")), atol (e.getAttr ("e")))
    ? new Statement (e.getAttr ("n"), atol (e.getAttr ("i")), atol (e.getAttr ("l")))
    : NULL;
}
/*@}*/

ProfIRFactory::ProfIRFactory (void)
{
  registerDefaults ();
}

void
ProfIRFactory::setElement (const XMLElem& e)
{
  element_ = e;
}

IRNode *
ProfIRFactory::callCreate (const string&, IRNodeCreator_t creator) const {
  return (*creator) (element_);
}

void
ProfIRFactory::registerDefaults (void)
{
  registerType (string ("SecFlatProfile"), ::newProgram);
  registerType (string ("SecHeader"), ::newSecHeader);
  registerType (string ("MetricTable"), ::newMetricTable);
  registerType (string ("Metric"), ::newMetricElement);
  registerType (string ("SecFlatProfileData"), ::newSecFlatProfileData);
  registerType (string ("G"), ::newGroup);
  registerType (string ("LM"), ::newModule);
  registerType (string ("F"), ::newFile);
  registerType (string ("P"), ::newProcedure);
  registerType (string ("C"), ::newCallSite);
  registerType (string ("PF"), ::newProcFrame);
  registerType (string ("L"), ::newLoop);
  registerType (string ("S"), ::newStatement);
  registerType (string ("LN"), ::newStatement); // synonym for "S"
}


/* eof */
