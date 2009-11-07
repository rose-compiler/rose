/**
 *******************************************
 * @file aslanalysis.cpp
 * @author Dietmar Schreiner
 * @version 1.0
 * @date 2009-11-02
 *******************************************
 */ 

//#define DEBUG

#ifdef HAVE_CONFIG_H
#include <rose_config.h>
#endif

#include <algorithm>

#include "aslanalysis.h"
#include "aslattribute.h"
#include "asl.h"
#include <StatementAttributeTraversal.h>

using namespace SATIrE;
namespace SATIrE { namespace Analyses{

// --------------------------------------------------------------------------------------------
/** 
 * Constructor.
 */
// --------------------------------------------------------------------------------------------

ASLAnalysis::ASLAnalysis(){
   callSiteCounter=0;
   functionPointerCallCounter=0;
   currentFunction="";
   localSites=NULL;
}

// --------------------------------------------------------------------------------------------
/** 
 * Destructor. Has to free the result database.
 */
// --------------------------------------------------------------------------------------------
   
ASLAnalysis::~ASLAnalysis(){
   for(AbstractSites::iterator i=aslRepository.begin();i!=aslRepository.end();++i){
      delete i->second;
   }
   
}
   
// --------------------------------------------------------------------------------------------
/** 
 * The visit function for the ASL traversal.
 * @param node a pointer to the root note of the (sub)tree to traverse.
 */
// --------------------------------------------------------------------------------------------

void ASLAnalysis::visit(SgNode* node){

   // concrete classes of AST nodes
   switch(node->variantT()){

      // naming scheme for variants: V_<classname>
      case V_SgFunctionDeclaration:{
         SgFunctionDeclaration* fdecl=isSgFunctionDeclaration(node);
      
         if(SgFunctionDefinition* fdef=fdecl->get_definition()) {
            std::string functionName=fdecl->get_name().getString();
            Sg_File_Info* ptrFileInfo=node->get_file_info();
            
            std::string aslPrefix="";
            if(ptrFileInfo){
               aslPrefix=ptrFileInfo->get_filenameString()+"/";
            }
            
            enterFunction(aslPrefix+functionName);  // set function name and reset enumeration counter
         }
      }
      break;
 
      // function calls through function pointers
      case V_SgPointerDerefExp:{
         if(isSgFunctionCallExp(node->get_parent()) && !(isSgFunctionCallExp(node->get_parent())->get_function() != node)){
#ifdef DEBUG
            std::cout<<"Function Pointer at call-site"<<std::endl;
#endif
            storeLocalSite(node);
            callSiteCounter++;
            functionPointerCallCounter++;
         }
      }
      break;
      
      case V_SgFunctionCallExp:{
         SgFunctionCallExp* fcall=isSgFunctionCallExp(node);
      
         if(SgFunctionRefExp* func=isSgFunctionRefExp(fcall->get_function())) {
            // SgFunctionSymbol* functionSymbol=func->get_symbol();
            // std::string functionName=functionSymbol->get_name().getString();
            callSiteCounter++;
         }
      }
      break;
      
   }
}


// --------------------------------------------------------------------------------------------
/** 
 * When entering a function, store the local sites (if any exist they must be from the
 * previously visited function) within the global database and clear the localSites container.
 * @param functionName the full quualified name of the function that has been entered.
 */
// --------------------------------------------------------------------------------------------
   
void ASLAnalysis::enterFunction(std::string functionName){
#ifdef DEBUG
   std::cout<<"entering "<<functionName<<"..."<<std::endl;
#endif
   
   if(currentFunction.length()>0){ // has a function been visited ?
      if((localSites!=NULL)&&(localSites->size()>0)){ // previous function contained local sites
         aslRepository[currentFunction]=localSites;
      }
   }

   localSites=new std::vector<LocalSite>;
   currentFunction=functionName;
}

// --------------------------------------------------------------------------------------------
/** 
 * Store a local site in the currrently active "local" database.
 * @param node the AST node which has to be stored as ASL.
 */
// --------------------------------------------------------------------------------------------
   
void ASLAnalysis::storeLocalSite(SgNode* node){
   LocalPosition position;
   LocalSite site;
   
   Sg_File_Info* ptrFileInfo=node->get_file_info();
   if(ptrFileInfo){
      position.first=ptrFileInfo->get_line();
      position.second=ptrFileInfo->get_col();
      site.first=position;
      site.second=node;
#ifdef DEBUG   
      std::cout<<"(ASLTraversal::storeLocalSite) Info: location at "<<site.first.first<<","<<site.first.second<<std::endl;
#endif      
      localSites->push_back(site);
      std::sort(localSites->begin(),localSites->end());
   }
#ifdef DEBUG   
   else{
      std::cout<<"(ASLTraversal::storeLocalSite) Error: No fileInfo available for node."<<std::endl;
   }
#endif
   
}

// --------------------------------------------------------------------------------------------
/** 
 * Run the analysis an annotate the AST via attributes.
 * @param root the root node of the AST.
 */
// --------------------------------------------------------------------------------------------
   
void ASLAnalysis::run(SgProject* root){
   std::string functionName="";
   int callEnumeration=1;
   
   // calculate the analysis result db
   traverse(root,preorder);
   
   // and see if there is any unsaved local data
   if(currentFunction.length()>0){ // have been in a function
      if((localSites!=NULL)&&(localSites->size()>0)){ // previous function contained local sites
         aslRepository[currentFunction]=localSites;
         currentFunction="";
         localSites=NULL;
      }
   }
   
   // create AST attributes
   for(AbstractSites::iterator i=aslRepository.begin();i!=aslRepository.end();++i,callEnumeration=1){
      functionName=i->first;
#ifdef DEBUG
      std::cout<<"Function "<<functionName<<" contains a call site of a function pointers"<<std::endl;
#endif
      for(std::vector<LocalSite>::iterator j=i->second->begin();j!=i->second->end();++j,callEnumeration++){
#ifdef DEBUG
         std::cout<<"   local position: "<<(j->first).first<<"/"<<(j->first).second<<std::endl;
#endif
         // add asl to AST
         AbstractSourceLocation asl=AbstractSourceLocation(functionName,AbstractSourceLocation::CALL_SITE,callEnumeration);
#ifdef DEBUG
         std::cout<<"ASL: "<<asl.toString()<<std::endl;
#endif
         (j->second)->setAttribute(ASL_ATTRIBUTE_ID,new ASLAttribute(asl));  
      }
   }
}

// --------------------------------------------------------------------------------------------
/** 
 * Get number of call sites within the AST.
 * @result total number of found call sites.
 */
// --------------------------------------------------------------------------------------------
   
unsigned int ASLAnalysis::getCallSiteCounter(){
   return callSiteCounter;
}
   

// --------------------------------------------------------------------------------------------
/** 
 * Get number of calls through function pointers in AST.
 * @result number of call sites via function pointer.
 */
// --------------------------------------------------------------------------------------------
   
unsigned int ASLAnalysis::getFunctionPointerCallCounter(){
   return functionPointerCallCounter;
}


}} // end namespace
