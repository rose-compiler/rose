#include "sage3basic.h"
#include "LtsminConnection.h"
#include "Visualizer.h"
#include "Flow.h"
#include "SpotConnection.h"

#include <stdio.h>
#include <stdlib.h>

//#include "boost/unordered_set.hpp"

using namespace CodeThorn;
using namespace CodeThorn;
using namespace std;


void LtsminConnection::init(string ltlFormulasFile) {
  //open text file that contains the properties
  ifstream ltl_input(ltlFormulasFile.c_str());
  if (ltl_input.is_open()) {
    _ltlResults = new PropertyValueTable();
    //load the containing formulae
    loadFormulae(ltl_input);  //load the formulae into class member "_ltlResults"
  }
}


string LtsminConnection::ltlFormula2LtsminSyntax(string formula) {
  // Transform property string using 'sed'
  string temp_filename = "temp_ltl_property.ltsmin";
  // TODO: RERS-specfic prototype with fixed regex for transition labels. Implement a more general solution
  string ltlConversion = "echo \""+formula+"\"" + " | sed -E 's/(c[0-9ct_]+_t[0-9]+)/(action == \"\\1\")/g'";
  //ltlConversion += " | sed -E 's/([a-z0-9]+_[a-z0-9])/(action == \"\\1\")/g'";
  ltlConversion += " | sed 's/|/||/g' | sed 's/&/&&/g' | sed 's/=>/->/g' | sed 's/G/[]/g' | sed 's/F/<>/g'";
  ltlConversion += " > " + temp_filename;  
  int retVal = system(ltlConversion.c_str());
  if (retVal < 0) {
    cout << "\"cat\"-based ltl transformation to LTSmin format resulted in error code: " << strerror(errno) << endl;
    ROSE_ASSERT(0);
  } else {
    if (!WIFEXITED(retVal)) {
      cout << "\"cat\"-based ltl transformation to LTSmin format did not exit correctly." << endl;
      ROSE_ASSERT(0);
    } 
  }
  // Read transformed property string from temporary file
  string result = "";
  ifstream ltl_input(temp_filename.c_str());
  if (ltl_input.is_open()) {
    getline(ltl_input, result);
  } else {
    cout << "ERROR: Could not load temporary file with ltl property in LTSmin syntax." << endl;
    ROSE_ASSERT(0);
  }
  // Remove temporary file
  string removeTempFile = "rm " + temp_filename;
  retVal = system(removeTempFile.c_str());
  if (retVal < 0) {
    cout << "Removing file \"" + temp_filename +  "\" resulted in error code: " << strerror(errno) << endl;
    ROSE_ASSERT(0);
  } else {
    if (!WIFEXITED(retVal)) {
      cout << "Call to remove file \"" + temp_filename +  "\" did not exit correctly." << endl;
      ROSE_ASSERT(0);
    } 
  }
  return result;
}

PropertyValueTable* LtsminConnection::checkLtlPropertiesParPro(map<int, Flow*> processes) {
  std::list<int>* yetToEvaluate = _ltlResults->getPropertyNumbers(PROPERTY_VALUE_UNKNOWN);
  for (std::list<int>::iterator i = yetToEvaluate->begin(); i != yetToEvaluate->end(); ++i) {
    string ltlProperty = ltlFormula2LtsminSyntax(_ltlResults->getFormula(*i));
    PropertyValue ltsminResult = checkPropertyParPro(ltlProperty, processes);
    _ltlResults->setPropertyValue(*i, ltsminResult);
  }
  return _ltlResults;
}

PropertyValue LtsminConnection::checkPropertyParPro(string ltlProperty, std::map<int, Flow*> processes) {
// convert processes to under-approx and over-approx petri nets
  set<int> processIds;
  for (map<int, Flow*>::const_iterator i=processes.begin(); i!=processes.end(); ++i) {
    processIds.insert((*i).first);
  }
  string under_approx;
  string over_approx;
  if (_generatedDotFiles.find(processIds) != _generatedDotFiles.end()) {
    // this subsystem has been analyzed before, no need to generate new Petri nets
    pair<string, string> filenames = _generatedDotFiles[processIds];
    under_approx = filenames.first;
    over_approx = filenames.second;
  } else {
    pair<string, string> filenames = generateDotGraphs(processes);
    under_approx = filenames.first;
    over_approx = filenames.second;
  }
  // Note: 'LTL-MTS' is an external script that model checks an LTL property on an under- and over-approximation.
  //       It then combines the results and therefore mimics model checking on Modal Transition Systems.
  //       This script is based on LTSmin and is not part of the ROSE distribution.
  string modelCheckingAttempt = "LTL-MTS " + under_approx + " " + over_approx + " " + "'" + ltlProperty + "'";
  int retVal = system(modelCheckingAttempt.c_str());
  if (retVal < 0) {
    cout << "Script LTL-MTS returned error code: " << strerror(errno) << endl;
    ROSE_ASSERT(0);
  } else {
    if (!WIFEXITED(retVal)) {
      cout << "Script LTL-MTS did not exit correctly." << endl;
      ROSE_ASSERT(0);
    } else {
      if (WEXITSTATUS(retVal) == 0) {
	return PROPERTY_VALUE_YES;
      } else if (WEXITSTATUS(retVal) == 1) {
	return PROPERTY_VALUE_NO;
      } else if (WEXITSTATUS(retVal) == 2) {
	return PROPERTY_VALUE_UNKNOWN;
      }
    }
  }
  ROSE_ASSERT(0);
  return PROPERTY_VALUE_UNKNOWN; // this case should be unreachable, but avoids compiler warnings
}

pair<string, string> LtsminConnection::generateDotGraphs(map<int, Flow*> processes) {
  // Note: This function relies on a script that is not part of the ROSE distribution. The script
  //       transforms parallel process graphs (instances of class Flow) into a Petri net representation 
  //       in PNML syntax. Environment variable "DOT_CONVERTER_PREFIX" should be set to the directory 
  //       where this script is located. 
  string dotConverterDir;
  if(const char* env_p = std::getenv("DOT_CONVERTER_PREFIX")) {
    dotConverterDir=string(env_p);
  } else {
    cout << "ERROR: Using LTSmin Petri Net-based verification, but env. variable DOT_CONVERTER_PREFIX is not set." << endl;
    ROSE_ASSERT(0);
  }

  // generate a unique string for the subset of parallel component ids and store the ids in a set
  string subsetIdsString = "";
  set<int> subsetIds;
  for (map<int, Flow*>::iterator i=processes.begin(); i!=processes.end(); ++i) {
    if (i != processes.begin()) {
      subsetIdsString += "_";
    }
    subsetIdsString += boost::lexical_cast<string>((*i).first);
    subsetIds.insert((*i).first);
  }

  // generate a minimal and a maximal language abstraction based on the chosen subset of non-abstracted components
  map<int, Flow*> minLanguageProcesses=componentAbstraction(processes, COMPONENTS_UNDER_APPROX);
  map<int, Flow*> maxLanguageProcesses=componentAbstraction(processes, COMPONENTS_OVER_APPROX);

  // write both abstractions to disk (.dot files)
  Visualizer visualizer;
  vector<Flow*> processVector;
  for (map<int, Flow*>::iterator i=minLanguageProcesses.begin(); i!=minLanguageProcesses.end(); ++i) {
    ROSE_ASSERT((*i).second);
    processVector.push_back((*i).second);
  }
  string dotProcessesMinLang = visualizer.cfasToDotSubgraphs(processVector);
  string fileNameMinLangDot = "processes_" + subsetIdsString + "_min_lang.dot";
  write_file(fileNameMinLangDot, dotProcessesMinLang);
  string fileNameMinLangPnml = "processes_" + subsetIdsString + "_min_lang.pnml";
  string commandLine = "python3 " + dotConverterDir + "/Dot2PNML.py " + fileNameMinLangDot + " > " + fileNameMinLangPnml;
  int retVal = system(commandLine.c_str());
  if (retVal < 0) {
    cout << "Python script Dot2PNML returned error code: " << strerror(errno) << endl;
    ROSE_ASSERT(0);
  } else {
    if (!WIFEXITED(retVal)) {
      cout << "Python script Dot2PNML did not exit correctly." << endl;
      ROSE_ASSERT(0);
    } 
  }

  processVector.clear();
  for (map<int, Flow*>::iterator i=maxLanguageProcesses.begin(); i!=maxLanguageProcesses.end(); ++i) {
    processVector.push_back((*i).second);
  }
  string dotProcessesMaxLang = visualizer.cfasToDotSubgraphs(processVector);
  string fileNameMaxLangDot = "processes_" + subsetIdsString + "_max_lang.dot";
  write_file(fileNameMaxLangDot, dotProcessesMaxLang);
  string fileNameMaxLangPnml = "processes_" + subsetIdsString + "_max_lang.pnml";
  commandLine = "python3 " + dotConverterDir + "/Dot2PNML.py " + fileNameMaxLangDot + " > " + fileNameMaxLangPnml;
  retVal = system(commandLine.c_str());
  if (retVal < 0) {
    cout << "Python script Dot2PNML returned error code: " << strerror(errno) << endl;
    ROSE_ASSERT(0);
  } else {
    if (!WIFEXITED(retVal)) {
      cout << "Python script Dot2PNML did not exit correctly." << endl;
      ROSE_ASSERT(0);
    } 
  }

  // add the filenames of generated Petri nets to the map of already created nets
  _generatedDotFiles[subsetIds] = pair<string, string>(fileNameMinLangPnml, fileNameMaxLangPnml);

  // return the filenames
  return pair<string, string>(fileNameMinLangPnml, fileNameMaxLangPnml);
}

 
map<int, Flow*> LtsminConnection::componentAbstraction(map<int, Flow*> processes, ComponentApproximation compAbstraction) {
  map<int, Flow*> result;
  if (compAbstraction == COMPONENTS_OVER_APPROX) {
    for (map<int, Flow*>::iterator i=processes.begin(); i!=processes.end(); ++i) {
      result[(*i).first] = (*i).second;
    }
    // Replace all other processes by a coarsest abstraction \mathcal{U} based on their combined alphabets
    // TODO: improve performance, use original processes that are to be abstracted from instead
    size_t startLabelId = (size_t)-100000; // hope that none of the parallel CFGs contain this node id
    size_t abstractionLabelId = (size_t)-99999; // hope that none of the parallel CFGs contain this node id
    size_t deadlockLabelId = (size_t)-99998; // hope that none of the parallel CFGs contain this node id
    Label startState = Label(startLabelId);
    Label abstractionState = Label(abstractionLabelId);
    Label deadlockState = Label(deadlockLabelId);
    Flow* abstraction = new Flow();
    Edge e(startState, abstractionState);
    e.setAnnotation("");
    abstraction->insert(e);
    abstraction->setStartLabel(startState);
    e = Edge(startState, deadlockState);
    e.setAnnotation("");
    abstraction->insert(e);
    for (EdgeAnnotationMap::iterator i=_annotationMap.begin(); i!=_annotationMap.end(); ++i) {
      boost::unordered_map<int, std::list<Edge> > occurrences = (*i).second;
      bool abstractedAway = false;
      for (boost::unordered_map<int, list<Edge> >::iterator p=occurrences.begin(); p!=occurrences.end();++p) {
	if (processes.find((*p).first) == processes.end()) {
	  abstractedAway = true;
	  break;
	}
      }
      if (abstractedAway) {
	Edge e(abstractionState, abstractionState);
	e.setAnnotation((*i).first);
	abstraction->insert(e);
	e = Edge(abstractionState, deadlockState);
	e.setAnnotation((*i).first);
	abstraction->insert(e);
      }
    }
    result[99999] = abstraction; // TODO: determine number
    return result;
  } else if (COMPONENTS_UNDER_APPROX) {
    set<string> newMayOnlyCommunication;
    list<int> worklist;
    for (map<int, Flow*>::iterator i=processes.begin(); i!=processes.end(); ++i) {
      worklist.push_back((*i).first);
      result[(*i).first] = new Flow(*(*i).second);
    } 
    while (!worklist.empty()) {
      int processId = worklist.front();
      Flow* process = result[processId];
      worklist.pop_front();
      Flow* newProcess = new Flow();
      Label startLabel = process->getStartLabel();
      newProcess->setStartLabel(startLabel);
      // add only the reachable part of must transitions
      set<Label> visited;
      std::list<Label> innerWorklist;
      innerWorklist.push_back(startLabel);
      visited.insert(startLabel);
      while (!innerWorklist.empty()) {
	Label current = innerWorklist.front();
	innerWorklist.pop_front();
        Flow outEdges = process->outEdges(current);
	for (Flow::iterator e=outEdges.begin(); e!=outEdges.end(); ++e) {
	  if (isMustTransition(*e, result, newMayOnlyCommunication) 
	      || ((*e).getAnnotation() == _startTransitionAnnotation)) {
	    newProcess->insert(*e);
	    if (visited.find((*e).target()) == visited.end()) {
	      innerWorklist.push_back((*e).target());
	      visited.insert((*e).target());
	    }
	  }	  
	}
      }
      // recursively prune leaves that were no leaves in the original process
      for (Flow::node_iterator k=newProcess->nodes_begin(); k!= newProcess->nodes_end(); ++k) {
	innerWorklist.push_back(*k);
      }
      while (!innerWorklist.empty()) {
	Label current = innerWorklist.front();
	innerWorklist.pop_front();
	if (newProcess->contains(current)) {
	  Flow outEdges = newProcess->outEdges(current);
	  if (outEdges.size() == 0 && (process->outEdges(current)).size() != 0) {
	    Flow inEdges = newProcess->inEdges(current);	  
	    for (Flow::iterator e=inEdges.begin(); e!=inEdges.end(); ++e) {
	      if ((*e).getAnnotation() != _startTransitionAnnotation) {
		innerWorklist.push_back((*e).source());
		newProcess->erase(*e);
	      }
	    }
	  }
	}
      }
      // increase set of may-only transitions and add affected processes to worklist if alphabet was reduced
      set<string> remainingAnnotations = newProcess->getAllAnnotations();
      set<string> previousAnnotations = process->getAllAnnotations();
      for (set<string>::iterator i=previousAnnotations.begin(); i!=previousAnnotations.end(); ++i) {
	if (remainingAnnotations.find(*i) == remainingAnnotations.end()) {
	  newMayOnlyCommunication.insert(*i);
	  boost::unordered_map<int, list<Edge> > occurrences = _annotationMap[*i];
	  for (boost::unordered_map<int, list<Edge> >::iterator p=occurrences.begin(); p!=occurrences.end();++p) {
	    int idOtherProcess = (*p).first;
	    if ( (result.find(idOtherProcess) != result.end()) 
		 && (idOtherProcess != processId) ) {
	      // There exists another process that now has an additional may-only transition; add it to the worklist 
	      worklist.push_back(idOtherProcess);	      
	    }
	  }
	}
      }
      // alter process in result set of processes (fixed-point algo)
      delete result[processId];
      result[processId] = newProcess;
    }
    return result;
  } else { // case of "COMPONENTS_NO_APPROX"
    return processes;
  }
}

bool LtsminConnection::isMustTransition(Edge e, map<int, Flow*>& processes) {
  bool must = true;
  boost::unordered_map<int, list<Edge> > occurrences = _annotationMap[e.getAnnotation()];
  for (boost::unordered_map<int, list<Edge> >::iterator p=occurrences.begin(); p!=occurrences.end();++p) {
    if (processes.find((*p).first) == processes.end()) {
      must = false;
      break;
    }
  }
  return must;
}

bool LtsminConnection::isMustTransition(Edge e, map<int, Flow*>& processes, set<string> mayOnlyCommunication) {
  if (mayOnlyCommunication.find(e.getAnnotation()) != mayOnlyCommunication.end()) {
    return false;
  }
  return isMustTransition(e, processes);
}



void LtsminConnection::loadFormulae(istream& input) {
  std::string line;
  int defaultPropertyNumber=0; //for RERS 2012 because no numbers were assigned in the properties.txt files
  bool explicitPropertyNumber = false;  //indicates whether or not an ID for the property could be extraced from the file
  int propertyNumber = -1;  //initialize to avoid compiler warnings
  while (std::getline(input, line)){
    //basic assumption: there is a formula after each number "#X:" (X being an integer) before the next number occurs
    // e.g. "#1:", the number by which the porperty is being refered to comes after '#'
    if (line.size() > 0 && line.at(0) == '#' && line.find_first_of(':') != string::npos) {  
      int endIndex = line.find_first_of (':', 0); //the ':' marks the end of the property number
      propertyNumber = boost::lexical_cast<int>(line.substr(1, (endIndex-1)));
      explicitPropertyNumber = true;
    } else if (line.size() > 0 && line.at(0) == '(') {   // '(' at column 0 indicates the beginning of a formula
      if (!explicitPropertyNumber) {  //enumerate those properties without any ID from 0 (therefore either all or non should have a number)
        propertyNumber = defaultPropertyNumber; 
        defaultPropertyNumber++;
      }
      _ltlResults->addProperty(parseWeakUntil(line), propertyNumber);
      explicitPropertyNumber = false;
    }
    //ignore any other lines that neither contain a property number nor a formula
  }
}

string& LtsminConnection::parseWeakUntil(std::string& ltl_string) {
  int positionCharW;
  while ( (positionCharW = ltl_string.find("WU")) != -1) {
    ltl_string.erase((positionCharW +1), 1);  //delete the following "U" of "WU"
  }
  return ltl_string;
}
