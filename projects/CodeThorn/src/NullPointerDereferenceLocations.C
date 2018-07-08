#include "sage3basic.h"
#include "NullPointerDereferenceLocations.h"
#include <iostream>
#include <fstream>
#include "CodeThornException.h"

using namespace std;
using namespace CodeThorn;
using namespace SPRAY;

string CodeThorn::NullPointerDereferenceLocations::programLocation(Labeler* labeler, Label lab) {
  SgNode* node=labeler->getNode(lab);
  ROSE_ASSERT(node);
  return SgNodeHelper::sourceLineColumnToString(node)+","+SgNodeHelper::sourceFilenameToString(node);
}

void CodeThorn::NullPointerDereferenceLocations::writeResultFile(string fileName, SPRAY::Labeler* labeler) {
  std::ofstream myfile;
  myfile.open(fileName.c_str(),std::ios::out);
  if(myfile.good()) {
    for(auto lab : definitiveDereferenceLocations) {
      myfile<<"definitive,"<<programLocation(labeler,lab)<<endl;
    }
    for(auto lab : potentialDereferenceLocations) {
      myfile<<"potential,"<<programLocation(labeler,lab)<<endl;
    }
    myfile.close();
  } else {
    throw CodeThorn::Exception("Error: could not open file "+fileName+".");
  }
}

void NullPointerDereferenceLocations::recordDefinitiveDereference(SPRAY::Label lab) {
  definitiveDereferenceLocations.insert(lab);
}
void NullPointerDereferenceLocations::recordPotentialDereference(SPRAY::Label lab) {
  potentialDereferenceLocations.insert(lab);

}

