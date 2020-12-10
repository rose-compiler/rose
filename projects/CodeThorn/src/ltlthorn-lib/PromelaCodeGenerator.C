// Author: Marc Jasper, 2016.

#include "PromelaCodeGenerator.h"

using namespace CodeThorn;
using namespace std;

string PromelaCodeGenerator::generateCode(CfgsAndAnnotationMap& parallelComponents) {
  string result;
  list<Flow> cfgs = parallelComponents.first;
  EdgeAnnotationMap annotationMap = parallelComponents.second;
  boost::unordered_set<string> automataReceivingEnvInput;
  boost::unordered_set<pair<string, string> > automataPairsWithCommunication;

bool useTransitionIds = false;
  string messageType = "mtype";
  boost::unordered_map<string, int> transitionIdMap;
  if (annotationMap.size() > 256) {
    useTransitionIds = true;  // Promela's mtype cannot be larger than a byte
    messageType = "int";
    int currentId = 0;
    stringstream transitionMapping;
    transitionMapping << "/* ======================================================================== */" << endl; 
    transitionMapping << "/* Transitions are relabeled to integer values (more than 256 transitions): */" << endl; 
    for (EdgeAnnotationMap::iterator i=annotationMap.begin(); i!=annotationMap.end(); i++) {
      transitionIdMap[(*i).first] = currentId;
      transitionMapping << "  /* transition "<<(*i).first<<" gets relabeled with id "<<currentId<<" */" << endl; 
      ++currentId;
    }
    transitionMapping << "/* ======================================================================== */" << endl; 
    result += transitionMapping.str();
  }

  stringstream messageTypes;
  if (!useTransitionIds) {
    messageTypes << "/* Actions (message types) */" << endl;
    messageTypes << "mtype = { nop";
  }
  stringstream interProcessChannels;
  interProcessChannels << "/* Inter-process channels */" << endl;
  stringstream envProcessChannels;
  envProcessChannels << "/* Environement<->process channels */" << endl;
  stringstream envProcess;
  envProcess << "/* Environment process */" << endl;
  envProcess << "active proctype Environment()" << endl;
  envProcess << "{" << endl;
  envProcess << "  do" << endl;

  for (EdgeAnnotationMap::iterator i=annotationMap.begin(); i!=annotationMap.end(); i++) {
    if (i->first == "") { //special treatment of the implicit no-operation (nop) 
      for (boost::unordered_map<int, std::list<Edge> >::iterator k=i->second.begin(); k!= i->second.end(); k++) {
        string automatonId = boost::lexical_cast<string>(k->first);
	if (useTransitionIds) {
	  envProcess << "  :: p"<<automatonId<<" ! "<<transitionIdMap[""] << endl;
	} else {
	  envProcess << "  :: p"<<automatonId<<" ! "<<"nop" << endl;
	}
        boost::unordered_set<string>::iterator previousEntry = automataReceivingEnvInput.find(automatonId);
        if (previousEntry == automataReceivingEnvInput.end()) {
          envProcessChannels << "chan p"<<automatonId<<" = [0] of {"<<messageType<<"};" << endl;
	  automataReceivingEnvInput.insert(automatonId);
	}
      }
    } else {
      if (!useTransitionIds) {
	messageTypes << ", "<<i->first;
      }
      if (i->second.size() > 2) {
	cout << "ERROR: trying to generate PROMELA code for parallel automata with communication ";
	cout << "between more than 2 processes.";
	ROSE_ASSERT(0);
      }
      if (i->second.size() == 1) {
	boost::unordered_map<int, list<Edge> >::iterator automatonContainingEdge = i->second.begin();
	string automatonId = boost::lexical_cast<string>(automatonContainingEdge->first);
	if (useTransitionIds) {
	  envProcess << "  :: p"<<automatonId<<" ! "<<transitionIdMap[i->first] << endl;
	} else {
	  envProcess << "  :: p"<<automatonId<<" ! "<<i->first << endl;
	}
	boost::unordered_set<string>::iterator previousEntry = automataReceivingEnvInput.find(automatonId);
	if (previousEntry == automataReceivingEnvInput.end()) {
	  envProcessChannels << "chan p"<<automatonId<<" = [0] of {"<<messageType<<"};" << endl;
	  automataReceivingEnvInput.insert(automatonId);
	}
      } else if (i->second.size() == 2) {
	bool firstAutomaton = true;
	pair<string, string> communicatingAutomata;
	for (boost::unordered_map<int, std::list<Edge> >::iterator k=i->second.begin(); k!= i->second.end(); k++) {
	  if (firstAutomaton) {
	    communicatingAutomata.first = boost::lexical_cast<string>(k->first);
	    firstAutomaton = false;
	  } else {
	    communicatingAutomata.second = boost::lexical_cast<string>(k->first);
	  }
	}
	boost::unordered_set<pair<string, string> >::iterator previousEntry =  automataPairsWithCommunication.find(communicatingAutomata);
	if (previousEntry ==  automataPairsWithCommunication.end()) {
	  interProcessChannels << "chan p"<<communicatingAutomata.first<<"_"<<communicatingAutomata.second;
	  interProcessChannels << " = [0] of {"<<messageType<<"};" << endl;
	  automataPairsWithCommunication.insert(communicatingAutomata);
	}
      }
    }
  }  
  if (!useTransitionIds) {
    messageTypes << " };" << endl;
  }
  envProcess << "  od" << endl;
  envProcess << "}" << endl;

  if (!useTransitionIds) {
    result += messageTypes.str() + "\n"; 
  }
  result += interProcessChannels.str() + "\n";
  result += envProcessChannels.str() + "\n";
  result += envProcess.str() + "\n";
  result += generateActionListener(messageType, useTransitionIds, transitionIdMap) + "\n";

  int cfgId = 0;
  for (list<Flow>::iterator i= cfgs.begin(); i!=cfgs.end(); i++) {
    result += generateCode(*i, cfgId, annotationMap, useTransitionIds, transitionIdMap) + "\n";
    cfgId++;
  }

  result += ltlAtomicPropositions(annotationMap, useTransitionIds, transitionIdMap) + "\n";

  return result;
};

string PromelaCodeGenerator::ltlAtomicPropositions(EdgeAnnotationMap& annotationMap, bool useTransitionIds, boost::unordered_map<string, int>& transitionIdMap) {
  stringstream ltlAtomicPropositions;
  for (EdgeAnnotationMap::iterator i=annotationMap.begin(); i!=annotationMap.end(); i++) {
    if (i->first != "") { //special treatment of the implicit no-operation (nop) 
      if (useTransitionIds) {
	ltlAtomicPropositions << "#define p_"<<i->first<<"\t (lastAction == "<<transitionIdMap[i->first]<<")" << endl;
      } else {
	ltlAtomicPropositions << "#define p_"<<i->first<<"\t (lastAction == "<<i->first<<")" << endl;
      }
    }
  }
  return ltlAtomicPropositions.str();
}

string PromelaCodeGenerator::generateActionListener(string messageType, bool useTransitionIds, 
						    boost::unordered_map<string, int>& transitionIdMap) {
  stringstream actionChannelAndListener;
  actionChannelAndListener << "/* Action channel */" << endl;
  actionChannelAndListener << "chan act = [0] of {"<<messageType<<"};" << endl;
  actionChannelAndListener << endl;
  actionChannelAndListener << "/* Most recent message event */" << endl;
  if (useTransitionIds) {
    actionChannelAndListener << messageType<<" lastAction = "<<transitionIdMap[""]<<";" << endl;
  } else {
    actionChannelAndListener << messageType<<" lastAction = nop;" << endl;
  }
  actionChannelAndListener << endl;
  actionChannelAndListener << "/* Action listener */" << endl;
  actionChannelAndListener << "active proctype Listener()" << endl;
  actionChannelAndListener << "{" << endl;
  actionChannelAndListener << "  atomic {" << endl;
  actionChannelAndListener << "    do" << endl;
  actionChannelAndListener << "    :: act ? lastAction ->" << endl;
  actionChannelAndListener << "  step: skip" << endl;
  actionChannelAndListener << "    od" << endl;
  actionChannelAndListener << "  }" << endl;
  actionChannelAndListener << "}" << endl;
  return actionChannelAndListener.str();
}

string PromelaCodeGenerator::generateCode(Flow& automaton, int id, EdgeAnnotationMap edgeAnnotationMap, 
					  bool useTransitionIds, boost::unordered_map<string, int>& transitionIdMap) {
  stringstream ss;
  ss << "/* Process "<<id<<" */" << endl;
  ss << "active proctype Proc"<<id<<"()" << endl;
  ss << "{" << endl;
  ss << "  int state = "<<automaton.getStartLabel().getId()<<";" << endl;
  ss << "  do" << endl;

  set<Label> visited; //TODO: maybe change to hashset
  list<Label> worklist;
  worklist.push_back(automaton.getStartLabel());
  visited.insert(automaton.getStartLabel());
  while (!worklist.empty()) {
    Label label = worklist.front();
    worklist.pop_front();
    ss << "  :: state == "<<label.getId()<<" ->" << endl;
    ss << "    atomic {" << endl;
    ss << "      if" << endl;
    Flow outEdges = automaton.outEdges(label);
    for (Flow::iterator i=outEdges.begin(); i!= outEdges.end(); ++i) {
      ss << "      :: "<<communicationDetails((*i).getAnnotation(), id, edgeAnnotationMap, useTransitionIds, transitionIdMap) << endl;
      ss << "        state = "<<(*i).target().getId()<<";" << endl;
      if (visited.find((*i).target()) == visited.end()) {
	if (automaton.outEdges((*i).target()).size() > 0) { // no need to create a case for leaves, nothing happens anymore (deadlock)
	  worklist.push_back((*i).target());
	}
        visited.insert((*i).target());
      }
    }
    ss << "      fi" << endl;
    ss << "    }" << endl;
  }

  ss << "  od" << endl;
  ss << "}" << endl;
  return ss.str();
}

string PromelaCodeGenerator::communicationDetails(string edgeAnnotation, int currentAutomaton, 
						  EdgeAnnotationMap edgeAnnotationMap, bool useTransitionIds, 
						  boost::unordered_map<string, int>& transitionIdMap) {
  stringstream ss;
  EdgeAnnotationMap::iterator iter = edgeAnnotationMap.find(edgeAnnotation);
  if (iter == edgeAnnotationMap.end()) {
    cout << "ERROR: asking for an edge annotation that is not registered for any automaton." << endl;
    ROSE_ASSERT(0);
  }
  bool messageReceived = false;
  if (edgeAnnotation == "") { // nop
    if (useTransitionIds) {
      ss << "p"<<currentAutomaton<<" ? "<<transitionIdMap[""];
    } else {
      ss << "p"<<currentAutomaton<<" ? nop";
    }
  } else if (iter->second.size() == 1) {
    int automatonIdWithAnnotation = iter->second.begin()->first;
    ROSE_ASSERT(automatonIdWithAnnotation == currentAutomaton);
    string automatonId = boost::lexical_cast<string>(currentAutomaton);
    if (useTransitionIds) {
      ss << "p"<<automatonId<<" ? "<<transitionIdMap[edgeAnnotation];
    } else {
      ss << "p"<<automatonId<<" ? "<<edgeAnnotation;
    }
    messageReceived = true;
  } else if (iter->second.size() == 2) {
    string sendOrReceive;
    pair<string, string> senderAndReceiver;
    bool firstAutomaton = true;
    for (boost::unordered_map<int, std::list<Edge> >::iterator k=iter->second.begin(); k!= iter->second.end(); k++) {
      if (firstAutomaton) { // the first entry is always chosen to be the sender of the message
	senderAndReceiver.first = boost::lexical_cast<string>(k->first);
	firstAutomaton = false;
	if (currentAutomaton == k->first) {
	  sendOrReceive = "!"; // the current automaton is the sender
	} else { 
	  sendOrReceive = "?";
	  messageReceived = true;
        }
      } else {
	senderAndReceiver.second = boost::lexical_cast<string>(k->first);
      }
    }
    if (useTransitionIds) {
      ss << "p"<<senderAndReceiver.first<<"_"<<senderAndReceiver.second<<" "<<sendOrReceive<<" "<<transitionIdMap[edgeAnnotation];
    } else {
      ss << "p"<<senderAndReceiver.first<<"_"<<senderAndReceiver.second<<" "<<sendOrReceive<<" "<<edgeAnnotation;
    }
  } else {
    cout << "DEBUG: edgeAnnotation: " << edgeAnnotation << endl;
    if (useTransitionIds) {
    cout << "DEBUG: edgeAnnotation ID: " << transitionIdMap[edgeAnnotation] << endl;
    }
    cout << "ERROR: encountered an edge annotation that is present in none or in more than two parallel component automata." << endl;
    ROSE_ASSERT(0);
  }
  ss << " ->" << endl;
  if (messageReceived) {
    if (useTransitionIds) {
      ss << "        act ! "<<transitionIdMap[edgeAnnotation]<<";";
    } else {
      ss << "        act ! "<<edgeAnnotation<<";";
    }
  }
  return ss.str();
}
