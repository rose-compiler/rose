#include "sage3basic.h"
#include "ProgramLocationsReport.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include "CodeThornException.h"
#include "SgNodeHelper.h"
#include "CppStdUtilities.h"

using namespace std;
using namespace CodeThorn;

void CodeThorn::ProgramLocationsReport::setReachableLocations(LabelSet loc) {
  reachableLocations=loc;
}
void CodeThorn::ProgramLocationsReport::setUnreachableLocations(LabelSet loc) {
  unreachableLocations=loc;
}

LabelSet CodeThorn::ProgramLocationsReport::verifiedLocations(){
  LabelSet a=reachableLocations;
  a-=definitiveLocations;
  a-=potentialLocations;
  return a;
}

LabelSet CodeThorn::ProgramLocationsReport::falsifiedLocations() {
  return definitiveLocations;
}

LabelSet CodeThorn::ProgramLocationsReport::unverifiedLocations() {
  return potentialLocations-definitiveLocations;
}

LabelSet CodeThorn::ProgramLocationsReport::verifiedFunctions(Labeler* labeler) {
  return filterFunctionEntryLabels(labeler,verifiedLocations());
}

LabelSet CodeThorn::ProgramLocationsReport::falsifiedFunctions(Labeler* labeler) {
  return filterFunctionEntryLabels(labeler,falsifiedLocations());
}

LabelSet CodeThorn::ProgramLocationsReport::unverifiedFunctions(Labeler* labeler) {
  return filterFunctionEntryLabels(labeler,unverifiedLocations());
}

LabelSet CodeThorn::ProgramLocationsReport::filterFunctionEntryLabels(Labeler* labeler, LabelSet labSet) {
  LabelSet functionEntryLabels;
  for(auto lab : labSet) {
    if(labeler->isFunctionEntryLabel(lab)) {
      functionEntryLabels.insert(lab);
    }
  }
  return functionEntryLabels;
}

bool CodeThorn::ProgramLocationsReport::hasSourceLocation(SgNode* stmt) {
  ROSE_ASSERT(stmt);
  Sg_File_Info* fi=stmt->get_file_info();
  if(fi)
    return fi->get_line()>0;
  return false;
}

string CodeThorn::ProgramLocationsReport::findOriginalProgramLocationOfLabel(Labeler* labeler, Label lab) {
  if(lab.isValid()) {
    SgNode* node=labeler->getNode(lab);
    return findOriginalProgramLocationOfNode(node);
  } else {
    return "?:?";
  }
}

string CodeThorn::ProgramLocationsReport::findOriginalProgramLocationOfNode(SgNode* node) {
  ROSE_ASSERT(node);
  // if node is inside expression, search for statement node
  while(!isSgStatement(node)) {
    if(hasSourceLocation(node)) {
      return CodeThorn::ProgramLocationsReport::fileInfoAsFormattedProgramLocation(node);
    }
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
  return CodeThorn::ProgramLocationsReport::fileInfoAsFormattedProgramLocation(node);
}

string CodeThorn::ProgramLocationsReport::fileInfoAsFormattedProgramLocation(SgNode* node) {
  return SgNodeHelper::sourceFilenameToString(node)+","+SgNodeHelper::sourceLineColumnToString(node);
}

// not used anymore
string CodeThorn::ProgramLocationsReport::sourceCodeAtLabel(Labeler* labeler, Label lab) {
  if(lab.isValid()) {
    SgNode* node=labeler->getNode(lab);
    ROSE_ASSERT(node);
    return SgNodeHelper::doubleQuotedEscapedString(node->unparseToString());
  } else {
    return "invalid_label_error2";
  }
}

bool CodeThorn::ProgramLocationsReport::isRecordedLocation(Label lab) {
  return definitiveLocations.isElement(lab)||potentialLocations.isElement(lab);
}

// unused function
LabelSet CodeThorn::ProgramLocationsReport::determineRecordFreeFunctions(CFAnalysis& cfAnalysis, Flow& flow) {
  LabelSet funEntries=cfAnalysis.functionEntryLabels(flow);
  LabelSet verifiedFunctions;
  for (Label entryLab : funEntries) {
    bool locationsRecorded=false;
    LabelSet funLabelSet=cfAnalysis.functionLabelSet(entryLab,flow);
    // determine whether all labels are verified
    for (Label funLab : funLabelSet) {
      if(isRecordedLocation(funLab)) {
        locationsRecorded=true;
        break;
      }
    }
    if(!locationsRecorded) {
      // entire function has no violations
      verifiedFunctions.insert(entryLab);
    }
  }
  return verifiedFunctions;
}

void ProgramLocationsReport::filterDefinitiveFromPotentialLocations() {
  for (auto defLoc : definitiveLocations) {
    if(potentialLocations.find(defLoc)!=potentialLocations.end()) {
      potentialLocations.erase(defLoc);
    }
  }
}

void ProgramLocationsReport::recordDefinitiveLocation(CodeThorn::Label lab) {
#pragma omp critical(definitiveproglocrecording)
  {
    if(lab.isValid())
      definitiveLocations.insert(lab);
  }
}
void ProgramLocationsReport::recordPotentialLocation(CodeThorn::Label lab) {
#pragma omp critical(potentialproglocrecording)
  {
    if(lab.isValid())
      potentialLocations.insert(lab);
  }
}

size_t ProgramLocationsReport::numDefinitiveLocations() {
  return definitiveLocations.size();
}
 size_t ProgramLocationsReport::numPotentialLocations() {
  return potentialLocations.size();
}
size_t ProgramLocationsReport::numTotalRecordedLocations() {
  // elements can be in both sets
  return (potentialLocations+definitiveLocations).size();
}

ProgramLocationsReport::SortedProgramLocationStringSet ProgramLocationsReport::createOriginalProgramLocationSet(LabelSet labSet, Labeler* labeler) {
  SortedProgramLocationStringSet lcSet;
  for(auto lab : labSet) {
    lcSet.insert(findOriginalProgramLocationOfLabel(labeler,lab));
  }
  return lcSet;
}

void CodeThorn::ProgramLocationsReport::writeResultFile(CodeThornOptions& ctOpt, string fileName, CodeThorn::Labeler* labeler) {
  string writeMode=ctOpt.csvReportModeString;
  std::ofstream myfile;
  if(writeMode==CppStdUtilities::getFileGenerateModeSelector()) {
    myfile.open(fileName.c_str(),std::ios::out);
  } else if(writeMode==CppStdUtilities::getFileAppendModeSelector()) {
    myfile.open(fileName.c_str(),std::ios::app);
  } else {
    cerr<<"Error: unknown write mode: "<<writeMode<<endl;
    exit(1);
  }

  if(myfile.good()) {
    for(auto loc : createOriginalProgramLocationSet(definitiveLocations, labeler)) {
      myfile<<"definitive,"<<loc;
      myfile<<endl;
    }
    for(auto loc : createOriginalProgramLocationSet(potentialLocations, labeler)) {
      myfile<<"potential,"<<loc;
      myfile<<endl;
    }
    myfile.close();
  } else {
    throw CodeThorn::Exception("Error: could not open file "+fileName+".");
  }
}

void CodeThorn::ProgramLocationsReport::writeResultToStream(CodeThornOptions&, std::ostream& stream, CodeThorn::Labeler* labeler) {
  writeAllDefinitiveLocationsToStream(stream,labeler,true,true);
  writeAllPotentialLocationsToStream(stream,labeler,true,true);
}

void CodeThorn::ProgramLocationsReport::writeAllDefinitiveLocationsToStream(std::ostream& stream, CodeThorn::Labeler* labeler, bool /*qualifier*/, bool /*programLocation*/) {
  writeLocationsToStream(stream,labeler,definitiveLocations,"definitive",true);
}

void CodeThorn::ProgramLocationsReport::writeAllPotentialLocationsToStream(std::ostream& stream, CodeThorn::Labeler* labeler, bool /*qualifier*/, bool /*programLocation*/) {
  writeLocationsToStream(stream,labeler,potentialLocations,"potential",true);
}

void CodeThorn::ProgramLocationsReport::writeLocationsToStream(std::ostream& stream, CodeThorn::Labeler* labeler, LabelSet& set, string qualifier, bool programLocation) {
  for(auto loc : createOriginalProgramLocationSet(set,labeler)) {
    if(qualifier.size()>0)
      stream<<qualifier;
    if(qualifier.size()>0&&(programLocation))
      stream<<": ";
    if(programLocation) {
      stream<<loc;
    }
    stream<<endl;
  }
}

void ProgramLocationsReport::writeLocationsVerificationOverview(CodeThornOptions&, std::ostream& os, Labeler*) {
  int int_n=reachableLocations.size();
  double n=(double)int_n;
  LabelSet verified=verifiedLocations();
  int v=verified.size();
  LabelSet falsified=falsifiedLocations();
  int f=falsified.size();
  LabelSet unverified=unverifiedLocations();
  int u=unverified.size();
  int d=unreachableLocations.size();
  int t=int_n+d;
  os<<std::fixed<<std::setprecision(2);
  //os<<"Reachable verified locations  : "<<setw(6)<<f+v<<" ["<<setw(6)<<(f+v)/n*100.0<<"%]"<<endl;
  os<<"Verified  (definitely safe)    locations: "<<setw(6)<< v <<" ["<<setw(6)<<v/n*100.0<<"% of reachable]"<<endl;
  os<<"Violated  (definitely unsafe)  locations: "<<setw(6)<< f <<" ["<<setw(6)<<f/n*100.0<<"% of reachable]"<<endl;
  os<<"Undecided (potentially unsafe) locations: "<<setw(6)<< u <<" ["<<setw(6)<<u/n*100.0<<"% of reachable]"<<endl;
  os<<"Total     (reachable)          locations: "<<setw(6)<<int_n<<" ["<<setw(6)<<n/t*100.0<<"% of total]"<<endl;
  os<<"Dead      (unreachable)        locations: "<<setw(6)<<d<<" ["<<setw(6)<<(double)d/t*100.0<<"% of total]"<<endl;
  os<<"Total                          locations: "<<setw(6)<<t<<endl;
#if 0
  os<<"Detected Errors:"<<endl;
  if(f==0) {
    cout<<"None."<<endl;
  } else {
    for(auto lab:definitiveLocations) {
      os<<sourceCodeAtLabel(labeler,lab)<<endl;
    }
  }
#endif
}
