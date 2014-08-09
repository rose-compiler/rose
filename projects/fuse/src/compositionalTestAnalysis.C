#include "sage3basic.h"
#include "compose.h"
#include "const_prop_analysis.h"
#include "live_dead_analysis.h"
#include "ortho_array_analysis.h"
#include "call_context_sensitivity_analysis.h"
#include "dead_path_elim_analysis.h"
#include "printAnalysisStates.h"
#include "pointsToAnalysis.h"
#include "analysis_tester.h"
#include <vector>
#include <ctype.h>
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>

using namespace std;
using namespace fuse;
using namespace boost::xpressive;

// Regex expressions for the composition command, defined globally so that they can be used inside main 
// (where they're initialized) as well as inside output_nested_results()
sregex composer, lcComposer, lpComposer, analysis, cpAnalysis, ldAnalysis, oaAnalysis, ccsAnalysis, dpAnalysis, ptAnalysis, analysisList, compSpec;

// Displays nested results to std::cout with indenting
struct output_nested_results
{
  typedef enum {looseSeq, loosePar, tight, unknown} composerType;
  
  int tabs_;
  composerType parentComposerType;
  list<ComposedAnalysis*>& parentSubAnalyses;
  Composer** parentComposer; 
  checkDataflowInfoPass* cdip;
      
  output_nested_results(int tabs, composerType& parentComposerType, list<ComposedAnalysis*>& parentSubAnalyses, Composer** parentComposer, checkDataflowInfoPass* cdip)
      : tabs_( tabs ), parentComposerType(parentComposerType), parentSubAnalyses(parentSubAnalyses), parentComposer(parentComposer), cdip(cdip)
  {
  }

  template< typename BidiIterT >
  void operator ()( match_results< BidiIterT > const &what ) 
  {
    // first, do some indenting
    typedef typename std::iterator_traits< BidiIterT >::value_type char_type;
    //char_type space_ch = char_type(' ');

    string match = what[0];
    
    // If this term is an analysis rather than a composer
    smatch subWhat;
    if(regex_match(match, subWhat, analysis)) {
//      cout << "analysis match="<<match<<", smatch="<<subWhat<<endl;
      // Create the selected analysis and add it to the parent's sub-analysis list
           if(regex_match(match, subWhat, cpAnalysis))  { parentSubAnalyses.push_back(new ConstantPropagationAnalysis()); }
      else if(regex_match(match, subWhat, ldAnalysis))  { parentSubAnalyses.push_back(new LiveDeadMemAnalysis()); }
      else if(regex_match(match, subWhat, oaAnalysis))  { parentSubAnalyses.push_back(new OrthogonalArrayAnalysis()); }//ComposedAnalysis* ca = new OrthogonalArrayAnalysis(); cout << "OrthogonalArrayAnalysis="<<ca->str()<<endl; parentSubAnalyses.push_back(ca); }
      else if(regex_match(match, subWhat, dpAnalysis))  { parentSubAnalyses.push_back(new DeadPathElimAnalysis()); }
      else if(regex_match(match, subWhat, ccsAnalysis)) { 
        parentSubAnalyses.push_back(new CallContextSensitivityAnalysis(1, CallContextSensitivityAnalysis::callSite));
        
/*        list<ComposedAnalysis*> mySubAnalyses;
        composerType myComposerType = unknown;
        output_nested_results ons(tabs_ + 1, myComposerType, mySubAnalyses, NULL, NULL);
        std::for_each(
            subWhat.nested_results().begin(),
            subWhat.nested_results().end(),
            ons);*/
      }
      else if(regex_match(match, subWhat, ptAnalysis))  { parentSubAnalyses.push_back(new PointsToAnalysis()); }
    // Otherwise, if this is a composer, create the analyses in its sub-analysis list and then create the composer
    } else if(regex_match(match, subWhat, lcComposer)) {
      //std::fill_n( std::ostream_iterator<char_type>( std::cout ), tabs_ * 4, space_ch ); cout << "LOOSE SEQUENTIAL\n"<<endl;
      parentComposerType = looseSeq;
    } else if(regex_match(match, subWhat, lpComposer)) {
      //std::fill_n( std::ostream_iterator<char_type>( std::cout ), tabs_ * 4, space_ch ); cout << "LOOSE PARALLEL\n"<<endl;
      parentComposerType = loosePar;
    // Finally, if this is a list of analyses for a given parent composer
    } else {
      //cout << "other match="<<match<<endl;
      
      assert(parentComposerType != unknown);
      list<ComposedAnalysis*> mySubAnalyses;
      composerType myComposerType = unknown;
      
      // Output any nested matches
      output_nested_results ons(tabs_ + 1, myComposerType, mySubAnalyses, NULL, NULL);
      std::for_each(
          what.nested_results().begin(),
          what.nested_results().end(),
          ons);
      // std::fill_n( std::ostream_iterator<char_type>( std::cout ), tabs_ * 4, space_ch );
      /*cout << "#mySubAnalyses="<<mySubAnalyses.size()<<endl;
      for(list<ComposedAnalysis*>::iterator i=mySubAnalyses.begin(); i!=mySubAnalyses.end(); i++)
      { // std::fill_n( std::ostream_iterator<char_type>( std::cout ), tabs_ * 4, space_ch );
        cout << "    "<<(*i)->str()<<endl; }*/
      
      if(parentComposerType == looseSeq) {
        ChainComposer* cc = new ChainComposer(mySubAnalyses, cdip, true);
        // Until ChainComposer is made to be a ComposedAnalysis, we cannot add it to the parentSubAnalyses list. This means that 
        // LooseComposer can only be user at the outer-most level of composition
        // !!!parentSubAnalyses.push_back(cc);
        *parentComposer = cc;
      } else if(parentComposerType == loosePar) {
        LooseParallelComposer* lp = new LooseParallelComposer(mySubAnalyses);
        parentSubAnalyses.push_back(lp);
        *parentComposer = lp;
      }
    }
  }
};

int main(int argc, char** argv)
{
  printf("========== S T A R T ==========\n");
    
  Rose_STL_Container<string> args = CommandlineProcessing::generateArgListFromArgcArgv(argc, argv);
  // Strip the dataflow analysis options
  
  // Run the front end
  SgProject* project = frontend(argc, argv);
  //generatePDF(*project);

#if 0
  // DQ (6/1/2014): Added generation of graphs to support debugging.
     generateDOTforMultipleFile(*project);

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
#endif

  printf("Frontend done\n");fflush(stdout);
  
  string fuseCmd = "";
  Rose_STL_Container<string> dataflowoptions = CommandlineProcessing::generateOptionList(args, "-fuse:");
  //std::vector<std::string>  dataflowoptions = project->get_originalCommandLineArgumentList();
  if(dataflowoptions.size()>1) { cout << "ERROR: too many -dataflow options!"; exit(-1); }
  // If the caller provided a description of the composition structure on the command line, set fuseCmd to it
  else if(dataflowoptions.size()==1) {
    fuseCmd = dataflowoptions[0];
  // Otherwise, read it from the fuse pragmas in the source
  } else {
    Rose_STL_Container<SgNode*> pragmas = NodeQuery::querySubTree(project, V_SgPragma);
    for(Rose_STL_Container<SgNode*>::iterator p=pragmas.begin(); p!=pragmas.end(); p++) {
      SgPragma* pragma = isSgPragma(*p);
      assert(pragma);

      //cout << "pragma: "<< pragma->get_pragma() << endl;
      sregex pragmaLine = *_s >> as_xpr("fuse") >> *_s >> (s1=+~_n);
      smatch what;

      if(regex_match(pragma->get_pragma(), what, pragmaLine)) {
        assert(what.size()==2);
        fuseCmd.append(what[1]);
      }
    }
  }

  lcComposer = as_xpr(icase("loosechain")) | icase("lc");
  lpComposer = as_xpr(icase("loosepar"))   | icase("lp");
  composer = by_ref(lcComposer) | by_ref(lpComposer);
  //composer = as_xpr(icase("loosechain")) | icase("lc") | icase("loosepar") | icase("lp");
  
  cpAnalysis  = as_xpr(icase("constantpropagationanalysis")) | icase("constantpropagation") | icase("cp");
  ldAnalysis  = as_xpr(icase("livedeadmemanalysis"))         | icase("livedead")            | icase("ld");
  oaAnalysis  = as_xpr(icase("livedeadmemanalysis"))         | icase("orthoarray")          | icase("oa");
  ccsAnalysis = as_xpr(icase("callctxsensanalysis"))         | icase("callctxsens")         | icase("ccs");
  dpAnalysis  = as_xpr(icase("deadpathelimanalysis"))        | icase("deadpath")            | icase("dp");
  ptAnalysis  = as_xpr(icase("pointstoanalysis"))            | icase("pointsto")            | icase("pt");
  analysis = by_ref(cpAnalysis) | by_ref(ldAnalysis) | by_ref(oaAnalysis) | by_ref(ccsAnalysis) | by_ref(dpAnalysis) | by_ref(ptAnalysis);
  analysisList = '(' >> *_s >> (by_ref(analysis) | by_ref(compSpec)) >> *_s >> (!('(' >> *_s >>  +_w   >> *_s >> ')') ) >>
        *(*_s >> "," >> *_s >> (by_ref(analysis) | by_ref(compSpec)) >> *_s >> (!('(' >> *_s >>  +_w   >> *_s >> ')') ) ) >> *_s >> ')';
  compSpec = *_s >> by_ref(composer) >> *_s >> analysisList >> *_s;

/*
  // Remove leading spaces
  unsigned int startNonSpace=0;
  while(startNonSpace<fuseCmd.length() && (fuseCmd[startNonSpace]==' ' || fuseCmd[startNonSpace]=='\t')) startNonSpace++;
  if(startNonSpace>0) fuseCmd.erase(0, startNonSpace);

  // Remove trailing spaces
  unsigned int endNonSpace=fuseCmd.length()-1;
  while(endNonSpace>=0 && (fuseCmd[endNonSpace]==' ' || fuseCmd[endNonSpace]=='\t')) endNonSpace--;
  if(endNonSpace<fuseCmd.length()-1) fuseCmd.erase(endNonSpace+1);*/

  cout << "fuseCmd = \""<<fuseCmd<<"\"\n";
  
  // If this is a command for the compositional framework
  if(fuseCmd.size()>0) {
    smatch what;
    if(regex_match(fuseCmd, what, compSpec)) {
      //cout << "MATCH composer\n";
      list<ComposedAnalysis*>  mySubAnalyses;
      Composer* rootComposer = NULL;
      output_nested_results::composerType rootComposerType = output_nested_results::unknown;
      
      checkDataflowInfoPass* cdip = new checkDataflowInfoPass();
      
      output_nested_results ons(0, rootComposerType, mySubAnalyses, &rootComposer, cdip);
      std::for_each(what.nested_results().begin(),
                    what.nested_results().end(),
                    ons);
      assert(rootComposer!=NULL);
      ((ChainComposer*)rootComposer)->runAnalysis();
      
      //cout << "rootComposer="<<rootComposer<<" cdip->getNumErrors()="<<cdip->getNumErrors()<<endl;
      if(cdip->getNumErrors() > 0) cout << cdip->getNumErrors() << " Errors Reported!"<<endl;
      else                         cout << "PASS"<<endl;

    } else
      cout << "FAIL composer\n";
  }
  
  printf("==========  E  N  D  ==========\n");
  
  return 0;
}

