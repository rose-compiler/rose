#include "sage3basic.h"
#include "ProgramLocationsReport.h"
#include <iostream>
#include <fstream>
#include "CodeThornException.h"
#include "SgNodeHelper.h"

using namespace std;
using namespace CodeThorn;
using namespace CodeThorn;

bool CodeThorn::ProgramLocationsReport::hasSourceLocation(SgStatement* stmt) {
  ROSE_ASSERT(stmt);
  Sg_File_Info* fi=stmt->get_file_info();
  return fi->get_line()>0;
}

string CodeThorn::ProgramLocationsReport::programLocation(Labeler* labeler, Label lab) {
  SgNode* node=labeler->getNode(lab);
  ROSE_ASSERT(node);
  // find non-transformation file info
  SgNode* parent=node->get_parent();
  // if node is inside expression, search for statement node
  while(!isSgStatement(node)) {
    node=node->get_parent();
    if(node==nullptr)
      return "[unresolved source location]";
  }
  SgStatement* stmt=isSgStatement(node);
  ROSE_ASSERT(stmt);
  while(!hasSourceLocation(stmt)) {
    stmt=SageInterface::getPreviousStatement(stmt);
    if(!stmt)
      return "[unresolved source location]";
  }
  node=stmt;
  ROSE_ASSERT(stmt);
  // return fileinfo as formatted string
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

bool CodeThorn::ProgramLocationsReport::isNonRecordedLocation(Label lab) {
  return !definitiveLocations.isElement(lab)&&!potentialLocations.isElement(lab);
}

LabelSet CodeThorn::ProgramLocationsReport::determineRecordFreeFunctions(CFAnalysis& cfAnalyzer, Flow& flow, LabelSet& ) {
  LabelSet funEntries=cfAnalyzer.functionEntryLabels(flow);
  LabelSet verifiedFunctions;
  for (Label entryLab : funEntries) {
    bool noLocationsRecorded=true;
    LabelSet funLabelSet=cfAnalyzer.functionLabelSet(entryLab,flow);
    // determine whether all labels are verified
    for (Label funLab : funLabelSet) {
      if(!isNonRecordedLocation(funLab)) {
        noLocationsRecorded=false;
        break;
      }
    }
    if(noLocationsRecorded) {
      // function has no violations
      verifiedFunctions.insert(entryLab);
    }
  }
  return verifiedFunctions;
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
#pragma omp critical(definitiveproglocrecording)
  {
  definitiveLocations.insert(lab);
  }
}
void ProgramLocationsReport::recordPotentialLocation(CodeThorn::Label lab) {
#pragma omp critical(potentialproglocrecording)
  {
  potentialLocations.insert(lab);
  }
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
