#include "sage3basic.h"
#include "ProgramLocationsReport.h"
#include <iostream>
#include <fstream>
#include "CodeThornException.h"
#include "SgNodeHelper.h"

using namespace std;
using namespace CodeThorn;
using namespace CodeThorn;

string CodeThorn::ProgramLocationsReport::programLocation(Labeler* labeler, Label lab) {
  SgNode* node=labeler->getNode(lab);
  ROSE_ASSERT(node);
  return SgNodeHelper::sourceLineColumnToString(node)+","+SgNodeHelper::sourceFilenameToString(node);
}

string CodeThorn::ProgramLocationsReport::sourceCodeAtProgramLocation(Labeler* labeler, Label lab) {
  SgNode* node=labeler->getNode(lab);
  ROSE_ASSERT(node);
  return SgNodeHelper::doubleQuotedEscapedString(node->unparseToString());
}

void CodeThorn::ProgramLocationsReport::writeResultToStream(std::ostream& stream, CodeThorn::Labeler* labeler) {
    for(auto lab : definitiveLocations) {
      stream<<"definitive: "<<programLocation(labeler,lab);
      stream<<": "<<sourceCodeAtProgramLocation(labeler,lab);
      stream<<endl;
    }
    for(auto lab : potentialLocations) {
      stream<<"potential: "<<programLocation(labeler,lab);
      stream<<": "<<sourceCodeAtProgramLocation(labeler,lab);
      stream<<endl;
    }
}

void CodeThorn::ProgramLocationsReport::writeResultFile(string fileName, CodeThorn::Labeler* labeler) {
  std::ofstream myfile;
  myfile.open(fileName.c_str(),std::ios::out);
  if(myfile.good()) {
    for(auto lab : definitiveLocations) {
      myfile<<"definitive,"<<programLocation(labeler,lab);
      myfile<<","<<sourceCodeAtProgramLocation(labeler,lab);
      myfile<<endl;
    }
    for(auto lab : potentialLocations) {
      myfile<<"potential,"<<programLocation(labeler,lab);
      myfile<<","<<sourceCodeAtProgramLocation(labeler,lab);
      myfile<<endl;
    }
    myfile.close();
  } else {
    throw CodeThorn::Exception("Error: could not open file "+fileName+".");
  }
}

void ProgramLocationsReport::recordDefinitiveLocation(CodeThorn::Label lab) {
  definitiveLocations.insert(lab);
}
void ProgramLocationsReport::recordPotentialLocation(CodeThorn::Label lab) {
  potentialLocations.insert(lab);
}

size_t ProgramLocationsReport::numDefinitiveLocations() {
  return definitiveLocations.size();
}
size_t ProgramLocationsReport::numPotentialLocations() {
  return potentialLocations.size();
}
size_t ProgramLocationsReport::numTotalLocations() {
  return definitiveLocations.size()+potentialLocations.size();
}
