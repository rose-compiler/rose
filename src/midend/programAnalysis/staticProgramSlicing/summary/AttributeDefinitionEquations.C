#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rose.h"

#include "SummaryGraph.h"

AttributeDefinitionEquations::AttributeDefinitionEquations(SystemDependenceGraph * sdg) :
  _sdg(sdg)
{
  _makeEquations();
}

void AttributeDefinitionEquations::_makeEquations() {

  set<ProgramDependenceGraph *> pdgs = _sdg->getPDGs();
  set<ProgramDependenceGraph *>::iterator i;
  for (i = pdgs.begin(); i != pdgs.end(); i++) {
    _generateEquationsForPDG(*i);
  }
}

void AttributeDefinitionEquations::_generateEquationsForPDG(ProgramDependenceGraph * pdg) {

  //go through all the nodes of the PDG, and process them if they should be attributes
  set<DirectedGraphNode *> nodes = pdg->getNodes();
  for(set<DirectedGraphNode *>::iterator i = nodes.begin(); i != nodes.end(); i++) {
    DependenceNode * node = dynamic_cast<DependenceNode *>(*i);

    //if it's not an attribute node, we ignore it
    if (!node->isAttribute()) 
      continue;
    
    //Otherwise, we figure out which nodes this one is dependent on. This is just a slice
    set<DependenceNode *> preds = pdg->getSlice(node);

    //get the SDG version of the node
    node = _sdg->getNode(node->getCopiedFrom());

    //go through the preds, and if any of them are attributes, add them to the equation
    for (set<DependenceNode *>::iterator j = preds.begin(); j != preds.end(); j++) {
      DependenceNode * pred = *j;

      //ignore non-attributes
      if (!pred->isAttribute())
	continue;

      //get the SDG version of the node
      pred = _sdg->getNode(pred->getCopiedFrom());

      //add attributes to the attribute equation for node
      _attribute_equations_map[pdg][node].insert(pred);
    }
  }
}

void AttributeDefinitionEquations::writeOut(ostream & os) {
  map<ProgramDependenceGraph *, AttributeEquations>::iterator i;
  for (i = _attribute_equations_map.begin(); i != _attribute_equations_map.end(); i++) {
    ProgramDependenceGraph * pdg = i->first;
    AttributeEquations ae = i->second;
    os << "Attribute Equations for " << pdg->getInterprocedural()->getFunctionDeclaration()->get_name().str() << endl;
    for (AttributeEquations::iterator j = ae.begin(); j != ae.end(); j++) {
      j->first->writeOut(os);
      os << endl;
      for (set<DependenceNode *>::iterator k = j->second.begin(); k != j->second.end(); k++) {
	os << "\t";
	(*k)->writeOut(os);
	os << endl;
      }
    }
  }
}

AttributeEquations AttributeDefinitionEquations::getEquations(ProgramDependenceGraph * pdg) {
  return _attribute_equations_map[pdg];
}
