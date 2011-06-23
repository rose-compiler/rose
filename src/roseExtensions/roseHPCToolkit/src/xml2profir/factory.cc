// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \file src/xml2profir/factory.cc
 *  \brief Implements a factory for instantiating profile IR nodes
 *  from XML elements.
 *  \ingroup ROSEHPCT_XML2PROFIR
 *
 *  $Id: factory.cc,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
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
    ? new Procedure (e.getAttr ("n"),
		     atol (e.getAttr ("b")),
		     atol (e.getAttr ("e")))
    : NULL;
}

static IRNode *
newLoop (const XMLElem& e)
{
  return (e.name == "L")
    ? new Loop (e.getAttr ("n"),
		atol (e.getAttr ("b")),
		atol (e.getAttr ("e")))
    : NULL;
}

static IRNode *
newStatement (const XMLElem& e)
{
  return (e.name == "S" || e.name == "LN")
    ? new Statement (e.getAttr ("n"),
		     atol (e.getAttr ("b")),
		     atol (e.getAttr ("e")))
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
ProfIRFactory::callCreate (const string& id, IRNodeCreator_t creator) const
{
  return (*creator) (element_);
}

void
ProfIRFactory::registerDefaults (void)
{
  registerType (string ("SecFlatProfile"), ::newProgram);
  registerType (string ("G"), ::newGroup);
  registerType (string ("LM"), ::newModule);
  registerType (string ("F"), ::newFile);
  registerType (string ("P"), ::newProcedure);
  registerType (string ("L"), ::newLoop);
  registerType (string ("S"), ::newStatement);
  registerType (string ("LN"), ::newStatement); // synonym for "S"
}


/* eof */
