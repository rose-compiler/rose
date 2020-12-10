
//#include "rose.h"
#include "sage3basic.h"
#include "Cxx_Grammar.h"
#include <roseTranslators.h>
#include "transformationTracking.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

CppToCppTranslator::CppToCppTranslator() {
  statusCode=0;
  setAstRoot(new SgProject());
}

CppToCppTranslator::~CppToCppTranslator() {
  delete getAstRoot();
  setAstRoot(NULL);
}

void CppToCppTranslator::setOptions(const std::vector <std::string>& argvList) {
  getAstRoot()->processCommandLine(argvList);
}

void CppToCppTranslator::setOptions(int argc,char** argv) {
  getAstRoot()->processCommandLine(argc,argv);
}

void CppToCppTranslator::frontend() {
  getAstRoot()->parse(); // C++ EDG front-end
}

void CppToCppTranslator::midend() { 
  // identity transformation
  }

void CppToCppTranslator::backend()
   {
     getAstRoot()->unparse(); // C++ backend

#if 1
  // DQ (11/30/2003): restored tests to compile the generated (unparsed) code
  // compile generated C++ source code with vendor compiler (cast away const)
     int finalCombinedExitStatus = getAstRoot()->compileOutput();

  // Avoid all I/O to stdout if useBackendOnly == true
  // if (getAstRoot()->get_useBackendOnly() == false)
     if ( SgProject::get_verbose() >= 1 )
          cout << "source file(s) compiled with vendor compiler. (exit status = " << finalCombinedExitStatus << ").\n" << endl;

     getAstRoot()->set_backendErrorCode(finalCombinedExitStatus);
#endif

     statusCode = (getAstRoot()->get_backendErrorCode());
   }

int CppToCppTranslator::status() {
  return statusCode;
}

SgProject* CppToCppTranslator::getAstRoot() {
  return sageProject;
}

void CppToCppTranslator::setAstRoot(SgProject* p) {
  sageProject=p;
}

/********************************************************/

bool CppToPdfTranslator::dumpFullAST = false; 

void CppToPdfTranslator::backend() {
#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
#if ROSE_WITH_LIBHARU
  AstPDFGeneration pdf;
  if (dumpFullAST)
    pdf.generate(getAstRoot()); // traverse entire AST 
  else  
    pdf.generateInputFiles(getAstRoot());  // traverse AST from the input files only
#else
  cout << "Warning: libharu support is not enabled" << endl;
#endif
#endif
}

/********************************************************/

//! the backend generates 5 different dot files, each representing a different kind of traversal/processing of the AST.
//! the tool cpp2dot uses this preprocessor
void CppToDotTranslator::backend() {
  AstDOTGeneration astdotgen;
  // Liao, 3/26/2014. Assign Unique Integer ID for all located nodes
  TransformationTracking::registerAstSubtreeIds (getAstRoot());
  // we should only generate one file (not all five) and process
  // the rose command line (right now tools/cpp2ps does that)
  astdotgen.generateInputFiles(getAstRoot());
//  astdotgen.generateInputFiles(getAstRoot(),AstDOTGeneration::PREORDER,"Preorder.");
//  astdotgen.generateInputFiles(getAstRoot(),AstDOTGeneration::POSTORDER,"Postorder.");
//  astdotgen.generateInputFiles(getAstRoot(),AstDOTGeneration::TOPDOWN,"TopDown.");
//  astdotgen.generateInputFiles(getAstRoot(),AstDOTGeneration::BOTTOMUP,"BottomUp.");
//  astdotgen.generateInputFiles(getAstRoot(),AstDOTGeneration::TOPDOWNBOTTOMUP,"TopDownBottomUp.");
}
void CppToCppTranslator::printMessage(string s) {
  // Avoid all I/O to stdout if useBackendOnly == true
  if (getAstRoot()->get_useBackendOnly() == false)
    cout << s << endl;
}


void CppToVendorTranslator::backend()
   {
     getAstRoot()->unparse(); // generate C++ file

     int finalCombinedExitStatus = getAstRoot()->compileOutput();
  // MS: why is the ErrorCode in SgProject not set by compileOutput()?
     getAstRoot()->set_backendErrorCode(finalCombinedExitStatus);
     ostringstream ss;
     if ( SgProject::get_verbose() >= 1 )
        {
          ss << "source file(s) compiled with vendor compiler. (exit status = " 
             << finalCombinedExitStatus << ").\n";
          printMessage(ss.str());
        }

     setStatusCode(getAstRoot()->get_backendErrorCode());
   }

/********************************************************/

void RoseTestTranslator::midend() {
  AstTests::runAllTests(getAstRoot());
}

/********************************************************/

