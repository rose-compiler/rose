#ifndef ASLANALYSIS_H
#define ASLANALYSIS_H

/**
 ********************************************
 * @file aslanalysis.h
 * @author Dietmar Schreiner
 * @version 1.0
 * @date 2009-11-02
 ********************************************
 */ 

#include <string>
#include <satire_rose.h>
#include <satire.h>

#include "locationmapper.h"

namespace SATIrE { namespace Analyses {

/** 
 * @class ASLAnalysis implements the main analysis to calculate ASLs.
 */   
class ASLAnalysis : public AstSimpleProcessing {
 private:
   AbstractSites aslRepository; 
   std::vector<LocalSite> *localSites;
   std::string currentFunction;   

   unsigned int callSiteCounter;
   unsigned int functionPointerCallCounter;

   void enterFunction(std::string functionName); // store current function's name and reset counters
   void storeLocalSite(SgNode* node);

   
 protected:
   void visit(SgNode* node);

 public:
   ASLAnalysis();
   ~ASLAnalysis();
   
   void run(SgProject* root);
   
   unsigned int getCallSiteCounter();
   unsigned int getFunctionPointerCallCounter(); 
};

   
}} // end namespace

#endif
