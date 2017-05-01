#include "sage3basic.h"
#include "LtsminConnection.h"
#include "Visualizer.h"

#include <stdio.h>
#include <stdlib.h>

using namespace CodeThorn;
using namespace SPRAY;
using namespace std;



PropertyValue LtsminConnection::checkPropertyParPro(string ltlProperty, std::map<int, Flow*> processes) {

// convert processes to under-approx and over-approx petri nets
  set<int> processIds;
  for (map<int, Flow*>::const_iterator i=processes.begin(); i!=processes.end(); ++i) {
    processIds.insert((*i).first);
  }
  string under_approx;
  string over_approx;
  if (_generatedDotFiles.find(processIds) != _generatedDotFiles.end()) {
    pair<string, string> filenames = _generatedDotFiles[processIds];
    under_approx = filenames.first;
    over_approx = filenames.second;
  } else {
    pair<string, string> filenames = generateDotGraphs(processes);
    under_approx = filenames.first;
    over_approx = filenames.second;
  }

  string modelCheckingAttempt = "LTL-MTS " + under_approx + " " + over_approx + " " + "'" + ltlProperty + "'";
  int retVal = system(modelCheckingAttempt.c_str());
  if (retVal < 0 || retVal > 2) {
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

  return PROPERTY_VALUE_UNKNOWN; // this case should be unreachable
}

pair<string, string> LtsminConnection::generateDotGraphs(map<int, Flow*> processes) {

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

  // generate minimal and maximal language abstraction based on the chosen subset of non-abstracted components
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
  string commandLine = "python3 " + dotConverterDir + "/DotETF_old.py " + fileNameMinLangDot + " -PNML > " + fileNameMinLangPnml;
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
  commandLine = "python3 " + dotConverterDir + "/DotETF_old.py " + fileNameMaxLangDot + " -PNML > " + fileNameMaxLangPnml;
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

  // add the filenames of generated graphs to the map of already created graphs
  _generatedDotFiles[subsetIds] = pair<string, string>(fileNameMinLangPnml, fileNameMaxLangPnml);

  // return the filenames
  return pair<string, string>(fileNameMinLangPnml, fileNameMaxLangPnml);
}

 
map<int, Flow*> LtsminConnection::componentAbstraction(map<int, Flow*> processes, ComponentApproximation compAbstraction) {
  return processes; // TODO
}
