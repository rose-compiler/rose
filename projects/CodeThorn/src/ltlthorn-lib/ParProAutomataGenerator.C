// Author: Marc Jasper, 2016.

#include "ParProAutomataGenerator.h"
#include "Miscellaneous2.h"

using namespace CodeThorn;
using namespace std;

NumberGenerator::NumberGenerator(int start) : _current(start) {}

int NumberGenerator::next() {
  return _current++;
}

CfaAnnotationGenerator::CfaAnnotationGenerator(int cfaId) : _cfaId(cfaId), _currentId(0) {}

string CfaAnnotationGenerator::next() {
  stringstream ss;
  ss << "c" << _cfaId << "_t" << _currentId;
  ++_currentId;
  return ss.str();
}

Flow* ParProAutomataGenerator::randomCircleAutomaton(int numCircles, 
						       pair<int,int> lengthBounds, pair<int,int> numIntersectionsBounds, 
						       NumberGenerator& labelIds, CfaAnnotationGenerator& annotations) {
  Flow* result = new Flow();
  Label startNode = Label(labelIds.next());
  for (int i = 0; i < numCircles; ++i) {
    int cycleLength = randomIntInRange(lengthBounds);
    vector<Label> cycleNodes(cycleLength);
    vector<bool> isExistingNode(cycleLength);
    // determine the node labels
    if (result->size() > 0) {  // the first cycle has no intersections with previous ones
      unsigned int numIntersections = randomIntInRange(numIntersectionsBounds);
      ROSE_ASSERT(numIntersections <= cycleNodes.size());
      for (unsigned int k = 0; k < numIntersections; ++k) {
	Label existing = chooseRandomNode(result);
	int cycleIndex = randomIntInRange(pair<int,int>(0, cycleNodes.size() - 1));
	while (isExistingNode[cycleIndex]) {  // redraw until a new cycle node index was chosen
	  cycleIndex = randomIntInRange(pair<int,int>(0, cycleNodes.size() - 1));
	}
	isExistingNode[cycleIndex] = true;
	cycleNodes[cycleIndex] = existing;
      }
    }
    for (unsigned int k = 0; k < cycleNodes.size(); ++k) {
      if (!isExistingNode[k]) {
	cycleNodes[k] = labelIds.next();
      }
    }
    // add the nodes and connect them with new edges
    for (unsigned int k = 0; k < cycleNodes.size(); ++k) {
      int targetLabelIndex = k+1;
      if (k == cycleNodes.size() - 1) {
	targetLabelIndex = 0;
      }
      Edge e(cycleNodes[k], cycleNodes[targetLabelIndex]);
      e.setAnnotation(annotations.next());
      result->insert(e);
    }
  }
  // add the artifical start node and connect it to a random node
  Label startNodeSuccessor = chooseRandomNode(result);
  Edge e(startNode, startNodeSuccessor);
  // no annotation (a.k.a. the empty string) denotes the initial transition
  result->insert(e);
  result->setStartLabel(startNode);
  return result;
}

Edge ParProAutomataGenerator::chooseRandomEdge(Flow* cfa) {
  int finalIteratorIndex = randomIntInRange(pair<int, int>(0, (cfa->size() - 1)));
  Flow::iterator iter = cfa->begin();
  for (int i = 0; i < finalIteratorIndex; ++i) {
    ++iter;
  }
  return (*iter);
}

Label ParProAutomataGenerator::chooseRandomNode(Flow* cfa) {
  return chooseRandomEdge(cfa).source();
}

vector<Flow*> ParProAutomataGenerator::randomlySyncedCircleAutomata(int numAutomata, pair<int,int> numSyncsBounds, 
					   pair<int,int> numCirclesBounds, pair<int,int> lengthBounds, 
					   pair<int,int> numIntersectionsBounds) {
  vector<Flow*> result = vector<Flow*>(numAutomata);
  // generate the automata ...
  NumberGenerator numGen = NumberGenerator(0);
  for (unsigned int i = 0; i < result.size(); ++i) {
    int numCircles = randomIntInRange(numCirclesBounds);
    CfaAnnotationGenerator annotationGen(i);
    result[i] = randomCircleAutomaton(numCircles, lengthBounds, numIntersectionsBounds, numGen, annotationGen);
  }
  // ... and then synchronize them using random relabeling
  randomlySynchronizeAutomata(result, numSyncsBounds);
  return result;
}

void ParProAutomataGenerator::randomlySynchronizeAutomata(vector<Flow*> automata, pair<int,int> numSyncBounds) {
  int numSynchronizations = randomIntInRange(numSyncBounds);
  unsigned int totalNumberOfEdges = 0;
  vector<unsigned int> numSyncedTransitions(automata.size());
  for (unsigned int i = 0; i < automata.size(); ++i) {
    totalNumberOfEdges += (automata[i]->size() - 1);  // do not count the initial transition (must not be relabeled)
    numSyncedTransitions[i] = 0;
  }
  if ((unsigned int) numSynchronizations > (totalNumberOfEdges / 2)) {
    cerr << "ERROR: "<<numSynchronizations<<" synchronizing relabelings where selected, however only ";
    cerr <<totalNumberOfEdges<<" edges exist in total." << endl;
    ROSE_ASSERT(0);
  }
  boost::unordered_map<string, pair<string, string> > synchronized;
  for (int i = 0; i < numSynchronizations; ++i) {
    // select two different automata for synchronization
    int automataIndexOne;
    do {
      automataIndexOne = randomIntInRange(pair<int,int>(0, automata.size()-1));
    } while (numSyncedTransitions[automataIndexOne] >= automata[automataIndexOne]->size());
    int automataIndexTwo;
    do {
      automataIndexTwo = randomIntInRange(pair<int,int>(0, automata.size()-1));
    } while (automataIndexOne == automataIndexTwo 
	     || numSyncedTransitions[automataIndexTwo] >= automata[automataIndexTwo]->size());
    // only select edges that are not synchronized yet
    Edge edgeOne;
    do {
      edgeOne = chooseRandomEdge(automata[automataIndexOne]);
    } while (synchronized.find(edgeOne.getAnnotation()) != synchronized.end()
	     || edgeOne.getAnnotation() == "");  // do not relabel the initial starting transition
    Edge edgeTwo;
    do {
      edgeTwo = chooseRandomEdge(automata[automataIndexTwo]);
    } while (synchronized.find(edgeTwo.getAnnotation()) != synchronized.end()
	     || edgeTwo.getAnnotation() == "");  // do not relabel the initial starting transition
    // relabel and store information on what has been relabeled
    string newAnnotation = edgeOne.getAnnotation() + "__" + edgeTwo.getAnnotation();
    synchronized[newAnnotation] = pair<string, string>(edgeOne.getAnnotation(), edgeTwo.getAnnotation());
    ++numSyncedTransitions[automataIndexOne];
    ++numSyncedTransitions[automataIndexTwo];
    // TODO: maybe add a relabeling function to the Flow interface
    Edge newEdgeOne = edgeOne;
    Edge newEdgeTwo = edgeTwo;
    newEdgeOne.setAnnotation(newAnnotation);
    newEdgeTwo.setAnnotation(newAnnotation);
    automata[automataIndexOne]->erase(edgeOne);
    automata[automataIndexOne]->insert(newEdgeOne);
    automata[automataIndexTwo]->erase(edgeTwo);
    automata[automataIndexTwo]->insert(newEdgeTwo);
  }
}

