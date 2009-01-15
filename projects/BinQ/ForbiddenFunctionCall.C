#include "BinQGui.h"
#include "ForbiddenFunctionCall.h"

#include <iostream>
#include "BinQSupport.h"
#include "slide.h"
#include <qtabwidget.h>


using namespace qrs;
using namespace std;
using namespace __gnu_cxx;

std::string ForbiddenFunctionCall::name() {
  return "Forbidden Function Call";
}

std::string ForbiddenFunctionCall::getDescription() {
  return "Contains a black list of functions that should not be called. Depends on DynamicInfo analysis.";
}


void
ForbiddenFunctionCall::visit(SgNode* node) {
  if (isSgAsmx86Instruction(node) && isSgAsmx86Instruction(node)->get_kind() == x86_call) {
    SgAsmx86Instruction* inst = isSgAsmx86Instruction(node);
    string calleeName = inst->get_comment();
    std::vector<std::string>::const_iterator it = blackList.begin();
    for (;it!=blackList.end();++it) {
      string name = *it;
      if (name==calleeName) {
	//cerr << " match : " << name << endl;
	string res = "Dont call: ";
	string funcname="";
	SgAsmBlock* b = isSgAsmBlock(inst->get_parent());
	SgAsmFunctionDeclaration* func = NULL;
	if (b)
	  func=isSgAsmFunctionDeclaration(b->get_parent()); 
	if (func)
	  funcname = func->get_name();
	res+=name+" ("+RoseBin_support::HexToString(inst->get_address())+") : "+unparseInstruction(inst)+" <"+inst->get_comment()+">";
	res+="  in function:  " +funcname;
	result[inst]= res;
      }
    }
  }
  if (isSgAsmFunctionDeclaration(node)) {
    string fname = isSgAsmFunctionDeclaration(node)->get_name();
    //cerr << " name === " << fname << endl;
    std::set<std::string>::const_iterator it = foundFunction.find(fname);
    if (it==foundFunction.end())
      foundFunction.insert(fname);
  }
  if (isSgAsmInstruction(node)) 
      foundInstruction.insert(node);
}

void
ForbiddenFunctionCall::runTraversal(SgNode* project) {
  blackList.clear();
  blackList.push_back("vfork");
  blackList.push_back("sprintf");
  blackList.push_back("scanf");
  blackList.push_back("sscanf");
  blackList.push_back("gets");
  blackList.push_back("strcpy");
  blackList.push_back("_mbscpy");
  blackList.push_back("lstrcat");
  blackList.push_back("memcpy");

  blackList.push_back("strcat");
  blackList.push_back("rand");
  blackList.push_back("rewind");
  blackList.push_back("atoi");
  blackList.push_back("atol");
  blackList.push_back("atoll");
  blackList.push_back("atof");
  blackList.push_back("unparse");
  blackList.push_back("unparseToString");

  foundFunction.clear();
  foundInstruction.clear();
  this->traverse(project,preorder);
}

void
ForbiddenFunctionCall::run(SgNode* fileA, SgNode* fileB) {
  instance=NULL;
  if (!testFlag)
    instance = QROSE::cbData<BinQGUI *>();

  if (isSgProject(fileA)==NULL) {
    cerr << "This is not a valid file for this analysis!" << endl;
    QString res = QString("This is not a valid file for this analysis");
    instance->analysisResult->append(res);  
    return;
  }

  SgBinaryFile* binaryFile = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
  SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;
  ROSE_ASSERT(file);
  info = new VirtualBinCFG::AuxiliaryInformation(file);

  if (debug)
    cerr << " Running forbidden function call ... " << endl;
  if (!testFlag) {
    ROSE_ASSERT(instance);
    ROSE_ASSERT(instance->analysisTab);
    instance->analysisTab->setCurrentIndex(1);
    QString res = QString("Looking at dynamic information : %1").arg(file->get_name().c_str());
    instance->analysisResult->append(res);  
  }

  genericF = file->get_genericFile() ;
  runTraversal(isSgProject(fileA));

  //  if (debug) {
    std::set<std::string>::const_iterator it = foundFunction.begin();
    for (;it!=foundFunction.end();++it) {
      string fname = *it;
      std::cout << " functions traversed: name ==== " << fname << std::endl;
    }
    //  }

  if (instance) {
    QString res = QString("\n>>>>>>>>>>>>>>>> Resolving call addresses to names ... total # functions: %1")
      .arg(RoseBin_support::ToString(foundFunction.size()).c_str());
    instance->analysisResult->append(res);  
  }
  std::cerr << "    ForbiddenFunctionCall : Fobidden Functions : " <<
    RoseBin_support::ToString(result.size()) << "   Total # functions: " << 
    RoseBin_support::ToString(foundFunction.size()) 
	    << "    # instructions: " << 
    RoseBin_support::ToString(foundInstruction.size()) << std::endl;
}



void
ForbiddenFunctionCall::test(SgNode* fileA, SgNode* fileB) {
  testFlag=true;
  run(fileA,fileB);
  testFlag=false;
}
