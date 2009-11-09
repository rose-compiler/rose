#include <aralir.h>


using namespace Aral;

/* The visitor identifies the nodes of relevant information in the ARAL
   file and stores the 4 relevant pointers in the following 4 variables.
   The 4 relevant pointers are stored in public variables for easy access.
  */
class PointsToInfoCollector : public EmptyDataVisitor {
  public:
  // The following four pointers are set after calling the 'accept'
  // method on the ARAL-IR root node.
  Map* varidTovarName; // collected info 1
  Set* pointsToLocSet; // collected info 2
  Set* pointsToRelSet; // collected info 3
  InfoElementList* variableLocationList; // collected info 4

 public:
  // initialize all our variables in the constructor
  PointsToInfoCollector():
    withinPointsToRel(false),
    withinPointsToLoc(false),
    withinVariableLocations(false),
    varidTovarName(0),
    pointsToLocSet(0),
    pointsToRelSet(0),
    variableLocationList(0)
    {}

  // The MapList represents the list of maps in the IDMAP/MAPPING section.
  // We know that the second map is the varid-varname map. We store that
  // pointer for later use.
  // Planned: A more advanced mechanism will allow to have the map obtained by
  // providing the type of the map.
  void preVisitMapList(MapList* mapList) {
    int pos=0;
    for(MapList::iterator i=mapList->begin();i!=mapList->end();i++) {
      if(pos==1)
	varidTovarName=static_cast<Map*>(*i);
      pos++;
    }
  }

  // We have found a result-section and check whether it is
  // a section we are interested in. If yes, remember it.
  void preVisitResultSection(ResultSection* resSec) {
    if(resSec->getName()=="pointsto_locations") {   
      withinPointsToLoc=true;
    }
    if(resSec->getName()=="pointsto_relations") {   
      withinPointsToRel=true;
    }
    if(resSec->getName()=="variable_locations") {   
      withinVariableLocations=true;
    }
  }


  // We are leaving a result section and therefore can no longer be
  // within a section of interest
  void postVisitResultSection(ResultSection* resSec) {
    withinPointsToLoc=false;
    withinPointsToRel=false;
    withinVariableLocations=false;
  }
    

  void preVisitSet(Set* set) {
    if(withinPointsToRel) {
    // we know that there is only one set in the result-section of the pointsto locations.
      pointsToRelSet=set;
    }
    // we know that there is only one set in the result-section of the pointsto-relations.
    if(withinPointsToLoc) {
      pointsToLocSet=set;
    }
  }
  void preVisitInfoElementList(InfoElementList* list) {
    // we know that there is only one InfoElementList in the result-section of the pointsto-relations.
    if(withinVariableLocations) {
	variableLocationList=list;
    }
  }

  bool checkResults() {
    // all 4 pointers must have been initialized with results
    assert(varidTovarName);
    assert(pointsToLocSet);
    assert(pointsToRelSet);
    assert(variableLocationList);
    return true;
  }
  private:
    bool withinPointsToRel;
    bool withinPointsToLoc;
    bool withinVariableLocations;
};

int main() {
  // call the ARAL Front End and create the ARAL-IR in memory.
  Data* root=Translator::frontEnd();
  // use the visitor PointsToInfoCollector to collect direct pointers to the
  // three relevant information-sets points_to_locations, pointsto_relations,
  // and variable_locations in the ARAL file.
  PointsToInfoCollector ic;
  root->accept(ic);
  // check that all results have been obtained.
  ic.checkResults();

  // if you want to print verbatim what we have found, set the #if to 1
  #if 0
  // now let's see what we have got, and print the pointsto_locations set and
  // the pointes_to_relations set to the screen
  std::cout << "\nWhat we have found so far: "
	    << ic.varidTovarName->toString() << std::endl
	    << ic.pointsToLocSet->toString() << std::endl
  	    << ic.pointsToRelSet->toString() << std::endl
  	    << ic.variableLocationList->toString() << std::endl
	    << "----------------------------------------------------"
            << std::endl;
  #endif

  // next we can process some information on these sets
  // 1) demonstrates how to cast all objects to their concrete types and
  //    access each object's interface. We filter those locations that represent
  //    at least one variable.
  // 2) simply uses the toString function to print the original data
  // 3) combines the use of STL-iterators and the toString method to separate
  //    the context and data information of a data item.
  std::cout << "EXAMPLE 1:" << std::endl << std::endl;
  std::cout << "1) points_to_locations: We extract all information and" << std::endl;
  std::cout << "   only print locations that represent at least one variable." << std::endl;
  std::cout << "   We use the varid-varname map to print the names of variables." << std::endl;

  for(Set::iterator i=ic.pointsToLocSet->begin();i!=ic.pointsToLocSet->end();i++) {
    // the pointsToLocSet consists of pairs, which are represented by objects of type Tuple.
    Tuple* tuple=static_cast<Tuple*>(*i);
    // we can access the tuple members with the method getAt(Position)
    // we access the Element at position 0 and since we know it has to be an object of type Int32
    // we use a static_cast to get a cast the Data* pointer to Int32.
    Int32* location=static_cast<Int32*>(tuple->getAt(0));
    // now, since we have an Int32 pointer we can access the method getValue to get the integer-value
    // of the location.
    int intLocation=location->getValue();
    // the element at position 1 in the tuple is a List of variableids.
    List* varidList=static_cast<List*>(tuple->getAt(1));
    // all ARAL collection nodes offer an STL-like interface
    // the class Tuple offers the interface of an stl-vector
    // we first check whether the size of the list is zero
    if(varidList->size()>0) {
      std::cout << "LOCATION:" << intLocation;
      std::cout << " with list of VarIds:";
      // since the length of the list is larger than 0 we print the list.
      // we use an iterator for printing each element
      for(List::iterator i=varidList->begin();i!=varidList->end();i++) {
	// for printing each element of the varid-list we use the default toString method.
	// alternatively we could decend down to each node of the 
	std::cout << "[ ID:" << (*i)->toString() << " ";
	// we determine also the name of the id by using the varid-varname map of the mapping section
	// the varid2varName pointer refers to a Aral::Map that provides the getMappedElement method.
	// we can simply provide the pointer to an Aral::Data element. The map maintains the mapping
	// and returns (here) an Aral::String object. Like every other Aral-node this object offers
	// a toString method which we use to print the Name.
	Data* variableName=ic.varidTovarName->getMappedElement(*i);
	std::cout << "NAME:" << variableName->toString() <<"]";
      }
      std::cout << std::endl;
    }
  }
  std::cout << "\n2) pointsto_relations: Since every ARAL node offers a toString function we can also print the original data:\n";
  std::cout << ic.pointsToRelSet->toString();
  std::cout << std::endl;
  std::cout << "\n3) variable_locations: Here we combine both, using STL-iterators and the toString methods and separate context and data:\n";
  for(InfoElementList::iterator i=ic.variableLocationList->begin();i!=ic.variableLocationList->end();i++) {
    InfoElement* resultdata=static_cast<InfoElement*>(*i);
    // have a look at the interface of InfoElement in aralir.h
    // we can access the context-element and the data-element.
    Data* context=resultdata->getContext(); // note that a context is represented as arbitrary data allowing callstrings, value sets, etc.
    Data* data=resultdata->getData();
    std::cout << "CONTEXT:" << context->toString() << " ";
    std::cout << "DATA:" << data->toString();
    std::cout << std::endl;
  }
  std::cout << std::endl;
}
