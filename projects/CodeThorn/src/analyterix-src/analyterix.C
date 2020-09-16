// Author: Markus Schordan, 2013, 2014.

#include "rose.h"

#include "CommandLineOptions.h"
#ifdef USE_SAWYER_COMMANDLINE
#include "Sawyer/CommandLineBoost.h"
#else
#include <boost/program_options.hpp>
#endif

#include <iostream>
#include "inliner.h"
#include "VariableIdMapping.h"

#include "shared-src/ProgramStats.h"

#include "Labeler.h"
#include "CFAnalysis.h"
#include "RDLattice.h"
#include "DFAnalysisBase.h"
#include "RDAnalysis.h"
#include "RoseRDAnalysis.h"
#include "LVAnalysis.h"
#include "IntervalAnalysis.h"
#include "RDAstAttribute.h"
#include "AstAnnotator.h"
#include "AnalysisAstAnnotator.h"
#include "DataDependenceVisualizer.h"
#include "Miscellaneous.h"
#include "Miscellaneous2.h"
#include "AstUtility.h"
#include "AbstractValue.h"
#include "SgNodeHelper.h"
#include "DFAstAttributeConversion.h"
#include "FIConstAnalysis.h"
#include "FIPointerAnalysis.h"
#include <boost/foreach.hpp>

#include "addressTakenAnalysis.h"
#include "defUseQuery.h"
#include "TimeMeasurement.h"
#include "AstUtility.h"
#include "AliasAnalysis.h"

#include "AstTerm.h"

#include <vector>
#include <set>
#include <list>
#include <string>

#include "limits.h"
#include <cmath>
#include "assert.h"

#include "FunctionIdMapping.h"
// ROSE analyses
#include "VariableRenaming.h"

// temporary
#include "IntervalTransferFunctions.h"

#include "CodeThornException.h"
#include "CodeThornException.h"
#include "DeadCodeAnalysis.h"
#include "Normalization.h"
#include "CodeThornLib.h"

using namespace std;
using namespace CodeThorn;
using namespace DFAstAttributeConversion;
using namespace AstUtility;

#include "PropertyValueTable.h"

string option_prefix;
bool option_stats=false;
bool option_generalanalysis=false;
bool option_rose_rd_analysis=false;
bool option_fi_constanalysis=false;
const char* csvConstResultFileName=0;
const char* csvAddressTakenResultFileName=0;
const char* csvDeadCodeUnreachableFileName = 0;
const char* csvDeadCodeDeadStoreFileName = 0;
string option_start_function="main";
bool option_rd_analysis=false;
bool option_ud_analysis=false;
bool option_lv_analysis=false;
bool option_interval_analysis=false;
bool option_check_static_array_bounds=false;
bool option_at_analysis=false;
bool option_trace=false;
bool option_optimize_icfg=false;
bool option_csv_stable=false;
bool option_no_topological_sort=false;
bool option_annotate_source_code=false;
bool option_ignore_unknown_functions=false;
bool option_inlining=false;
bool option_normalize=false;
bool option_show_source_code=false;
bool option_show_path=true;
//boost::program_options::variables_map args;

void writeFile(std::string filename, std::string data) {
  std::ofstream myfile;
  myfile.open(filename.c_str(),std::ios::out);
  myfile << data;
  myfile.close();
}

void generateRessourceUsageVis(RDAnalysis* rdAnalyzer) {
  cout << "INFO: computing program statistics."<<endl;
  CodeThorn::ProgramStatistics ps(rdAnalyzer->getVariableIdMapping(),
                       rdAnalyzer->getLabeler(), 
                       rdAnalyzer->getFlow(),
                       "ud-analysis");
  ps.computeStatistics();
  //ps.printStatistics();
  cout << "INFO: generating resource usage visualization."<<endl;
  ps.setGenerateWithSource(false);
  ps.generateResourceUsageICFGDotFile("resourceusageicfg.dot");
  rdAnalyzer->getFlow()->resetDotOptions();
}

void checkStaticArrayBounds(SgProject* root, CodeThorn::IntervalAnalysis* intervalAnalysis) {
  cout<<"STATUS: checking static array bounds."<<endl;
  int issuesFound=0;
  CodeThorn::Labeler* labeler=intervalAnalysis->getLabeler();
  for(Labeler::iterator j=labeler->begin();j!=labeler->end();++j) {
    SgNode* node=labeler->getNode(*j);
    std::string lineCol=SgNodeHelper::sourceLineColumnToString(node);
    if(isSgStatement(node)&&!isSgFunctionDefinition(node)&&!isSgBasicBlock(node)) {
      RoseAst ast(node);
      for(RoseAst::iterator i=ast.begin();i!=ast.end();i++) {
        if(SgPntrArrRefExp* arrRefExp=isSgPntrArrRefExp(*i)) {
          SgNode* lhs=SgNodeHelper::getLhs(arrRefExp);
          SgNode* rhs=SgNodeHelper::getRhs(arrRefExp);
          // go to the right most expression of "x->y->...->z[i]"
          while(isSgArrowExp(lhs)) {
            lhs=SgNodeHelper::getRhs(isSgArrowExp(lhs));
          }
          SgVarRefExp* arrayVar=isSgVarRefExp(lhs);
          SgVarRefExp* indexVar=isSgVarRefExp(rhs);
          if(arrayVar&&indexVar) {
            VariableIdMapping* variableIdMapping=intervalAnalysis->getVariableIdMapping();
            VariableId arrayVarId=variableIdMapping->variableId(arrayVar);
            VariableId indexVarId=variableIdMapping->variableId(indexVar);
            IntervalPropertyState* intervalPropertyState=dynamic_cast<IntervalPropertyState*>(intervalAnalysis->getPreInfo(*j));
            ROSE_ASSERT(intervalPropertyState);
            if(!variableIdMapping->hasArrayType(arrayVarId)) {
              cerr<<"Internal error: determined array variable, but it is not registered as array variable."<<endl;
              exit(1);
            }
            size_t arraySize=variableIdMapping->getNumberOfElements(arrayVarId);
            if(intervalPropertyState->variableExists(indexVarId)) {
              NumberIntervalLattice indexVariableInterval=intervalPropertyState->getVariable(indexVarId);
              if(indexVariableInterval.isTop()
                 ||indexVariableInterval.isLowInf()
                 ||indexVariableInterval.getLow()<0
                 ||indexVariableInterval.isHighInf()
                 ||indexVariableInterval.getHigh()>(arraySize-1)) {
                cout<<"DETECTED: array out of bounds access: "<<lineCol
                    <<": "<<node->unparseToString()
                    <<" ("
                    <<variableIdMapping->variableName(indexVarId)
                    <<" in "<<indexVariableInterval.toString()
                    <<" accessing array '"<<variableIdMapping->variableName(arrayVarId)<<"'"
                    <<" of size "<<arraySize
                    <<")"
                    <<endl;
                issuesFound++;
              }
            } else if(intervalPropertyState->isBot()) {
              //cout<<"ANALYSIS: not reachable: "<<node->unparseToString()<<endl;
              // nothing to do
            } else {
              cout<<"Error: variable "<<indexVarId.toString()<<" does not exist in property state."<<endl;
              exit(1);
            }
          } else {
            //cerr<<"WARNING: Unsupported array access expression: ";
            //cerr<<AstTerm::astTermWithNullValuesToString(arrRefExp)<<endl;
          }
        }
      }
    }
  }
  if(issuesFound==0) {
    cout<<"PASS: No out of bounds accesses on static arrays in this program."<<endl;
  }
}

// schroder3 (2016-07-05): Attaches the given comment to the preprocessing info of the given node. The comment
//  is inserted without appending comment characters ("//", "/*", "*/") and should therefore contain these
//  characters. The argument for the second parameter specifies whether the comment should be attached before or
//  after the given node.
void insertComment(std::string comment, PreprocessingInfo::RelativePositionType posSpecifier, SgLocatedNode* node) {
  ROSE_ASSERT(posSpecifier==PreprocessingInfo::before || posSpecifier==PreprocessingInfo::after);
  PreprocessingInfo* commentInfo =
    new PreprocessingInfo(PreprocessingInfo::CplusplusStyleComment,
                          comment,
                          "user-generated",0, 0, 0,
                          posSpecifier // e.g. PreprocessingInfo::before
                          );
  node->addToAttachedPreprocessingInfo(commentInfo);
}

// schroder3 (2016-07-12): Returns the mangled stable/portable scope of the given statement.
//  FIXME: There are some places (see comment below) in the rose mangling which add the address
//  of the AST node to the mangled name. Due to this, this function currently does not return
//  a stable/portable mangled scope in all cases.
string getScopeAsMangledStableString(SgLocatedNode* stmt) {
  SgNode* parent = stmt;
  // Go up in the AST and look for the closest scope of the given statement:
  while((parent = parent->get_parent())) {
    SgStatement* declScope = 0;
    // Look for a FunctionParameterList or a ScopeStatement:
    if(SgFunctionParameterList* functionParams = isSgFunctionParameterList(parent)) {
      // Special case: Function parameter: The scope is
      //  the corresponding function definition/declaration:
      //  Function declaration is enough, because we only need the function
      //  name and types to create the mangled scope.
      declScope = isSgFunctionDeclaration(functionParams->get_parent());
      ROSE_ASSERT(declScope);
    }
    else if(SgScopeStatement* scope = isSgScopeStatement(parent)) {
      declScope = scope;
    }

    if(declScope) {
      // Found the scope of the given statement.

      // In theory it should work by using
      //   return mangleQualifiersToString(declScope);
      //  but there are the following problems in functions that get called by mangleQualifiersToString(...):
      //   1) SgFunctionDeclaration::get_mangled_name() mangles every function with the name "main" (even
      //      those that are not in the global scope) as
      //       main_<address of the AST node>
      //      .
      //   2) SgTemplateArgument::get_mangled_name(...) adds the address of a AST node to the mangled
      //      name if the template argument is a type and it's parent is a SgTemplateInstantiationDecl.
      //  Especially because of the address we can not use this as a portable scope representation.
      //
      // Workaround for 1): Replace the name of every function definition/declaration that has the name "main" and that
      //  is a direct or indirect scope parent of declScope by a temporary name that is different from "main".
      //  Then use mangleQualifiersToString(...) to mangle the scope. Finally, replace occurrences of the
      //  temporary name in the mangled-scope by "main".
      //
      // Workaround for 2): Currently none.

      // Workaround for 1):
      string tempName = string("(]"); // Something that does not appear in a function or operator name.
      SgName tempSgName = SgName(tempName);
      SgName mainSgName = SgName("main");
      vector<SgFunctionDeclaration*> main_func_decls;
      SgStatement* scopeParent = declScope;
      // Collect all functions that have "main" as their name and replace their name
      //  by the temporary name:
      while((scopeParent = scopeParent->get_scope()) && !isSgGlobal(scopeParent)) {
        SgFunctionDefinition* funcDef = isSgFunctionDefinition(scopeParent);
        if(SgFunctionDeclaration* funcDecl = funcDef ? funcDef->get_declaration() :0) {
          if(funcDecl->get_name() == tempSgName) {
            // There is a function whose name contains tempName. The mangled scope
            //  will probably be wrong:
            throw CodeThorn::Exception("Found function whose name contains the reserved text \"" + tempName + "\". "
                                   "Mangling of scope is not possible.");
          }
          else if(funcDecl->get_name() == mainSgName) {
            main_func_decls.push_back(funcDecl);
            funcDecl->set_name(tempSgName);
          }
        }
      }

      // Create the mangled-scope:
      string mangled_scope;
      if(SgFunctionDeclaration* fundDecl = isSgFunctionDeclaration(declScope)) {
        // Special case: A function decl node is not a scope statement:
        mangled_scope = fundDecl->get_mangled_name();
      }
      else if(SgScopeStatement* scope = isSgScopeStatement(declScope)) {
        mangled_scope = mangleQualifiersToString(scope);
      }
      else {
        ROSE_ASSERT(false);
      }

      // Replace occurrences of the temporary name in the mangled-scope
      //  by "main" and restore the previous name ("main") of the functions:
      for(vector<SgFunctionDeclaration*>::const_iterator i = main_func_decls.begin();
          i != main_func_decls.end(); ++i
      ) {
          (*i)->set_name(mainSgName);
          size_t start = mangled_scope.find(tempName);
          // TODO: Functions and local classes (and more?) are mangled as L0R, L1R, ... and not with their
          //  scope. Because of that, there is no corresponding temporary name in
          //  the mangled-scope sometimes:
          if(start != string::npos) {
            mangled_scope.replace(start, tempName.length(), string("main"));
          }
      }
      if(mangled_scope.find(tempName) != string::npos) {
        // There is a function whose name contains tempName. Because we abort above if there is such a function
        //  this should not happen.
        ROSE_ASSERT(false);
      }
      return mangled_scope;
    }
  }
  return string("");
}

void runAnalyses(SgProject* root, Labeler* labeler, VariableIdMapping* variableIdMapping) {
  if(option_fi_constanalysis) {
    FIConstAnalysis fiConstAnalysis(variableIdMapping);
    fiConstAnalysis.runAnalysis(root);
    fiConstAnalysis.attachAstAttributes(labeler,"const-analysis-inout"); // not iolabeler
    if(csvConstResultFileName) {
      cout<<"INFO: generating const CSV file "<<option_prefix+csvConstResultFileName<<endl;
      fiConstAnalysis.writeCvsConstResult(*variableIdMapping, (option_prefix+csvConstResultFileName).c_str());
    }
    if(option_annotate_source_code) {
      cout << "INFO: annotating analysis results as comments."<<endl;
      AstAnnotator ara(labeler);
      ara.annotateAstAttributesAsCommentsBeforeStatements(root, "const-analysis-inout");
      ara.annotateAstAttributesAsCommentsAfterStatements(root, "const-analysis-inout");
    }
  }

  if(option_at_analysis) {
    cout<<"STATUS: running address taken analysis."<<endl;
    cout << "STATUS: computing variable and function mappings."<<endl;
    // compute variableId mappings
    VariableIdMapping variableIdMapping;
    variableIdMapping.computeVariableSymbolMapping(root);
    // Compute function id mappings:
    FunctionIdMapping functionIdMapping;
    functionIdMapping.computeFunctionSymbolMapping(root);

    if(option_trace) {
      std::cout << std::endl << "TRACE: Variable Id Mapping:" << std::endl;
      variableIdMapping.toStream(std::cout);
      std::cout << std::endl << "TRACE: Function Id Mapping:" << std::endl;
      functionIdMapping.toStream(std::cout);
    }

    cout << "STATUS: computing address taken sets."<<endl;
    CodeThorn::FIPointerAnalysis fipa(&variableIdMapping, &functionIdMapping, root);
    fipa.initialize();
    fipa.run();

    //cout << "STATUS: computed address taken sets:"<<endl;
    //fipa.getFIPointerInfo()->printInfoSets();

    bool createCsv = false;
    ofstream addressTakenCsvFile;
    if(csvAddressTakenResultFileName) {
      std::string addressTakenCsvFileName = option_prefix;
      addressTakenCsvFileName += csvAddressTakenResultFileName;
      addressTakenCsvFile.open(addressTakenCsvFileName.c_str());
      createCsv = true;
    }

    cout << "INFO: annotating declarations of address taken variables and functions."<<endl;

    // Annotate declarations/definitions of variables from which the address was taken:
    VariableIdSet addressTakenVariableIds = fipa.getAddressTakenVariables();
    for(VariableIdSet::const_iterator idIter = addressTakenVariableIds.begin(); idIter != addressTakenVariableIds.end(); ++idIter) {
      // Determine the variable declaration/definition:
      SgLocatedNode* decl = variableIdMapping.getVariableDeclaration(*idIter);
      if(!decl) {
        // The current variable is presumably a function parameter: Try to get the initialized name:
        SgVariableSymbol* varSymbol = isSgVariableSymbol(variableIdMapping.getSymbol(*idIter));
        ROSE_ASSERT(varSymbol);
        SgInitializedName* paramDecl = isSgInitializedName(varSymbol->get_declaration());
        // We should not have a real variable declaration for the parameter:
        ROSE_ASSERT(isSgFunctionParameterList(paramDecl->get_declaration()));

        // Use the InitializedName:
        decl = paramDecl;
      }

      if(decl) {
        // Create the comment:
        ostringstream commentStream;
        commentStream << "/* Address of \"" << variableIdMapping.variableName(*idIter) << "\" is "
                      << "presumably taken.*/";

        // Annotate first declaration:
        insertComment(commentStream.str(), PreprocessingInfo::before, decl);
        // TODO: Annotate other declarations too!

        // Annotate definition if available (e.g. not available in case of parameter):
        if(SgDeclarationStatement* variableDeclaration = isSgDeclarationStatement(decl)) {
          if(SgDeclarationStatement* definingDeclaration = variableDeclaration->get_definingDeclaration()) {
            insertComment(commentStream.str(), PreprocessingInfo::before, definingDeclaration);
          }
        }

        if(createCsv) {
          // Write variable info to csv:
          addressTakenCsvFile << VariableId::idKindIndicator << ","
                              // The id of the variable (id codes are influenced by the used system headers
                              //  and are therefore not stable/portable):
                              << (option_csv_stable ? string("<unstable>") : int_to_string((*idIter).getIdCode())) << ","
                              // Name of the variable:
                              << variableIdMapping.variableName(*idIter) << ","

                              // TODO: Mangled scope and type are currently not stable/portable
                              //  (see comments in getScopeAsMangledStableString(...))
                              // Mangled type of the variable (non-mangled type may contain commas (e.g. "A<int,bool>"):
                              << (option_csv_stable ? string("<unstable>") : variableIdMapping.getType(*idIter)->get_mangled().getString()) << ","
                              // Mangled scope of the variable:
                              << (option_csv_stable ? string("<unstable>") : getScopeAsMangledStableString(decl)) << ","

                              // Is the address taken? (currently only address taken variables are output to csv)
                              << "1" << endl;
        }
      }
      else {
        cout << "ERROR: No declaration for " << variableIdMapping.uniqueVariableName(*idIter) << " available." << endl;
        ROSE_ASSERT(false);
      }
    }

    // Annotate declarations and definitions of functions from which the address was taken:
    FunctionIdSet addressTakenFunctionIds = fipa.getAddressTakenFunctions();
    for(FunctionIdSet::const_iterator idIter = addressTakenFunctionIds.begin(); idIter != addressTakenFunctionIds.end(); ++idIter) {

      if(SgFunctionDeclaration* decl = functionIdMapping.getFunctionDeclaration(*idIter)) {
        // Create the comment:
        ostringstream commentStream;
        commentStream << "/* Address of \"" << functionIdMapping.getFunctionNameFromFunctionId(*idIter) << "(...)\" is "
                      << "presumably taken.*/";

        // Annotate first declaration:
        insertComment(commentStream.str(), PreprocessingInfo::before, decl);
        // TODO: Annotate other declarations too!

        // Annotate definition if available:
        if(SgDeclarationStatement* definingDeclaration = decl->get_definingDeclaration()) {
          insertComment(commentStream.str(), PreprocessingInfo::before, definingDeclaration);
        }

        if(createCsv) {
          addressTakenCsvFile << FunctionId::idKindIndicator << ","
                              // The id of the function (id codes are influenced by the used system headers
                              //  and are therefore not stable/portable):
                              << (option_csv_stable ? string("<unstable>") : int_to_string((*idIter).getIdCode())) << ","
                              // Name of the function:
                              << functionIdMapping.getFunctionNameFromFunctionId(*idIter) << ","

                              // TODO: Mangled scope and type are currently not stable/portable
                              //  (see comments in getScopeAsMangledStableString(...))
                              // Mangled type of the function (non-mangled type may contain commas (e.g. "void (A<int,bool>)"):
                              << (option_csv_stable ? string("<unstable>") : functionIdMapping.getTypeFromFunctionId(*idIter)->get_mangled().getString()) << ","
                              // Mangled scope of the function:
                              << (option_csv_stable ? string("<unstable>") :getScopeAsMangledStableString(decl)) << ","

                              // Is the address taken? (currently only address taken functions are output to csv)
                              << "1" << endl;
        }
      }
      else {
        cout << "ERROR: No declaration for " << functionIdMapping.getUniqueShortNameFromFunctionId(*idIter) << " available." << endl;
        ROSE_ASSERT(false);
      }
    }

    if(createCsv) {
      addressTakenCsvFile.close();
    }

#if 0
    VariableIdSet vidset=fipa.getModByPointer();
    cout<<"mod-set: "<<CodeThorn::VariableIdSetPrettyPrint::str(vidset,variableIdMapping)<<endl;
#endif
  }
  
  if(option_interval_analysis) {
    cout << "STATUS: creating interval analyzer."<<endl;
    CodeThorn::IntervalAnalysis* intervalAnalyzer=new CodeThorn::IntervalAnalysis();
    cout << "STATUS: initializing interval analyzer."<<endl;
    intervalAnalyzer->setNoTopologicalSort(option_no_topological_sort);
    intervalAnalyzer->initialize(root);
    cout << "STATUS: running pointer analysis."<<endl;
    ROSE_ASSERT(intervalAnalyzer->getVariableIdMapping());
    CodeThorn::FIPointerAnalysis* fipa=new FIPointerAnalysis(intervalAnalyzer->getVariableIdMapping(), intervalAnalyzer->getFunctionIdMapping(), root);
    fipa->initialize();
    fipa->run();
    intervalAnalyzer->setPointerAnalysis(fipa);
    cout << "STATUS: initializing interval transfer functions."<<endl;
    intervalAnalyzer->initializeTransferFunctions();
    cout << "STATUS: initializing interval global variables."<<endl;
    intervalAnalyzer->initializeGlobalVariables(root);
      
    intervalAnalyzer->setSkipUnknownFunctionCalls(option_ignore_unknown_functions);

    intervalAnalyzer->setSolverTrace(option_trace);
    std::string funtofind=option_start_function;
    RoseAst completeast(root);
    SgFunctionDefinition* startFunRoot=completeast.findFunctionByName(funtofind);
    intervalAnalyzer->determineExtremalLabels(startFunRoot,false);
    intervalAnalyzer->run();

    if(option_annotate_source_code) {
      AnalysisAstAnnotator ara(intervalAnalyzer->getLabeler(),intervalAnalyzer->getVariableIdMapping());
      ara.annotateAnalysisPrePostInfoAsComments(root,"iv-analysis",intervalAnalyzer);
    }

    if(option_check_static_array_bounds) {
      checkStaticArrayBounds(root,intervalAnalyzer);
    }
    // schroder3 (2016-08-08): Generate csv-file containing unreachable statements
    if(csvDeadCodeUnreachableFileName) {
      // Generate file name
      std::string deadCodeCsvFileName = option_prefix+csvDeadCodeUnreachableFileName;
      DeadCodeAnalysis deadCodeAnalysis;
      deadCodeAnalysis.setOptionSourceCode(option_show_source_code);
      deadCodeAnalysis.setOptionFilePath(option_show_path);
      deadCodeAnalysis.writeUnreachableCodeResultFile(intervalAnalyzer,deadCodeCsvFileName);
    }
    delete fipa;
  }

  if(option_lv_analysis) {
    cout << "STATUS: creating LV analysis."<<endl;
    CodeThorn::LVAnalysis* lvAnalysis=new CodeThorn::LVAnalysis();
    cout << "STATUS: initializing LV analysis."<<endl;
    lvAnalysis->setBackwardAnalysis();
    lvAnalysis->setNoTopologicalSort(option_no_topological_sort);
    lvAnalysis->initialize(root);
    cout << "STATUS: running pointer analysis."<<endl;
    ROSE_ASSERT(lvAnalysis->getVariableIdMapping());
    CodeThorn::FIPointerAnalysis* fipa = new FIPointerAnalysis(lvAnalysis->getVariableIdMapping(), lvAnalysis->getFunctionIdMapping(), root);
    fipa->initialize();
    fipa->run();
    lvAnalysis->setPointerAnalysis(fipa);
    cout << "STATUS: initializing LV transfer functions."<<endl;
    lvAnalysis->initializeTransferFunctions();
    cout << "STATUS: initializing LV global variables."<<endl;
    lvAnalysis->initializeGlobalVariables(root);
    lvAnalysis->setSolverTrace(option_trace);
    std::string funtofind=option_start_function;
    RoseAst completeast(root);
    SgFunctionDefinition* startFunRoot=completeast.findFunctionByName(funtofind);
    cout << "generating icfg_backward.dot."<<endl;
    write_file("icfg_backward.dot", lvAnalysis->getFlow()->toDot(lvAnalysis->getLabeler()));

    lvAnalysis->setSkipUnknownFunctionCalls(option_ignore_unknown_functions);

    lvAnalysis->determineExtremalLabels(startFunRoot);
    lvAnalysis->run();
    cout << "INFO: attaching LV-data to AST."<<endl;
#if 0
    lvAnalysis->attachInInfoToAst("lv-analysis-in");
    lvAnalysis->attachOutInfoToAst("lv-analysis-out");
    AstAnnotator ara(lvAnalysis->getLabeler(),lvAnalysis->getVariableIdMapping());
    ara.annotateAstAttributesAsCommentsBeforeStatements(root, "lv-analysis-in");
    ara.annotateAstAttributesAsCommentsAfterStatements(root, "lv-analysis-out");
#else
    if(option_annotate_source_code) {
      AnalysisAstAnnotator ara(lvAnalysis->getLabeler(),lvAnalysis->getVariableIdMapping());
      ara.annotateAnalysisPrePostInfoAsComments(root,"lv-analysis",lvAnalysis);
    }
#endif

    // schroder3 (2016-08-15): Generate csv-file that contains dead assignments/ initializations:
    if(csvDeadCodeDeadStoreFileName) {
      std::string deadCodeCsvFileName = option_prefix+csvDeadCodeDeadStoreFileName;
      DeadCodeAnalysis deadCodeAnalysis;
      deadCodeAnalysis.writeDeadAssignmentResultFile(lvAnalysis,deadCodeCsvFileName);
    }
    delete lvAnalysis;
  }

  if(option_rd_analysis) {
      cout << "STATUS: creating RD analyzer."<<endl;
      CodeThorn::RDAnalysis* rdAnalysis=new CodeThorn::RDAnalysis();
      cout << "STATUS: initializing RD analyzer."<<endl;
      rdAnalysis->setNoTopologicalSort(option_no_topological_sort);
      rdAnalysis->initialize(root);
      cout << "STATUS: initializing RD transfer functions."<<endl;
      rdAnalysis->initializeTransferFunctions();
      cout << "STATUS: initializing RD global variables."<<endl;
      rdAnalysis->initializeGlobalVariables(root);
      rdAnalysis->setSolverTrace(option_trace);
      
      cout << "generating icfg_forward.dot."<<endl;
      write_file("icfg_forward.dot", rdAnalysis->getFlow()->toDot(rdAnalysis->getLabeler()));
    
      std::string funtofind=option_start_function;
      RoseAst completeast(root);
      SgFunctionDefinition* startFunRoot=completeast.findFunctionByName(funtofind);
      rdAnalysis->setSkipUnknownFunctionCalls(option_ignore_unknown_functions);
      rdAnalysis->determineExtremalLabels(startFunRoot);
      rdAnalysis->run();
    
      cout << "INFO: attaching RD-data to AST."<<endl;
      rdAnalysis->attachInInfoToAst("rd-analysis-in");
      rdAnalysis->attachOutInfoToAst("rd-analysis-out");
      //printAttributes<RDAstAttribute>(rdAnalysis->getLabeler(),rdAnalysis->getVariableIdMapping(),"rd-analysis-in");
      cout << "INFO: annotating analysis results as comments."<<endl;
      ROSE_ASSERT(rdAnalysis->getVariableIdMapping());
#if 0
      AstAnnotator ara(rdAnalysis->getLabeler(),rdAnalysis->getVariableIdMapping());
      ara.annotateAstAttributesAsCommentsBeforeStatements(root, "rd-analysis-in");
      ara.annotateAstAttributesAsCommentsAfterStatements(root, "rd-analysis-out");
#else
      if(option_annotate_source_code) {
        AnalysisAstAnnotator ara(rdAnalysis->getLabeler(),rdAnalysis->getVariableIdMapping());
        ara.annotateAnalysisPrePostInfoAsComments(root,"rd-analysis",rdAnalysis);
      }
#endif

#if 0
      cout << "INFO: substituting uses with rhs of defs."<<endl;
      substituteUsesWithAvailableExpRhsOfDef("ud-analysis", root, rdAnalysis->getLabeler(), rdAnalysis->getVariableIdMapping());
#endif
      if(option_ud_analysis) {
        ROSE_ASSERT(option_rd_analysis);
        cout << "INFO: generating and attaching UD-data to AST."<<endl;
        createUDAstAttributeFromRDAttribute(rdAnalysis->getLabeler(),"rd-analysis-in", "ud-analysis");
        Flow* flow=rdAnalysis->getFlow();
        cout<<"Flow label-set size: "<<flow->nodeLabels().size()<<endl;
        //CFAnalysis* cfAnalyzer0=rdAnalysis->getCFAnalyzer();
        //int red=cfAnalyzer0->reduceBlockBeginNodes(*flow);
        //cout<<"INFO: eliminated "<<red<<" block-begin nodes in ICFG."<<endl;
        
#if 0
        cout << "INFO: computing program statistics."<<endl;
        ProgramStatistics ps(rdAnalysis->getVariableIdMapping(),
                             rdAnalysis->getLabeler(), 
                             rdAnalysis->getFlow(),
                             "ud-analysis");
        ps.computeStatistics();
        //ps.printStatistics();
        cout << "INFO: generating resource usage visualization."<<endl;
        ps.setGenerateWithSource(false);
        ps.generateResourceUsageICFGDotFile("resourceusageicfg.dot");
        flow->resetDotOptions();
#endif
        cout << "INFO: generating visualization data."<<endl;
        // generate ICFG visualization
        cout << "generating icfg.dot."<<endl;
        write_file("icfg.dot", flow->toDot(rdAnalysis->getLabeler()));
        
        //  cout << "INFO: generating control dependence graph."<<endl;
        //Flow cdg=rdAnalysis->getCFAnalyzer()->controlDependenceGraph(*flow);

        cout << "generating datadependencegraph.dot."<<endl;
        DataDependenceVisualizer ddvis0(rdAnalysis->getLabeler(),
                                        rdAnalysis->getVariableIdMapping(),
                                        "ud-analysis");
        //printAttributes<UDAstAttribute>(rdAnalysis->getLabeler(),rdAnalysis->getVariableIdMapping(),"ud-analysis");
        //ddvis._showSourceCode=false; // for large programs
        ddvis0.generateDefUseDotGraph(root,"datadependencegraph.dot");
        flow->resetDotOptions();
        
        cout << "generating icfgdatadependencegraph.dot."<<endl;
        DataDependenceVisualizer ddvis1(rdAnalysis->getLabeler(),
                                        rdAnalysis->getVariableIdMapping(),
                                        "ud-analysis");
        ddvis1.includeFlowGraphEdges(flow);
        ddvis1.generateDefUseDotGraph(root,"icfgdatadependencegraph.dot");
        flow->resetDotOptions();
        
        cout << "generating icfgdatadependencegraph_clustered.dot."<<endl;
        DataDependenceVisualizer ddvis2(rdAnalysis->getLabeler(),
                                        rdAnalysis->getVariableIdMapping(),
                                        "ud-analysis");
        ddvis2.generateDotFunctionClusters(root,rdAnalysis->getCFAnalyzer(),"icfgdatadependencegraph_clustered.dot",true);
        
        cout << "generating icfg_clustered.dot."<<endl;
        DataDependenceVisualizer ddvis3(rdAnalysis->getLabeler(),
                                        rdAnalysis->getVariableIdMapping(),
                                        "ud-analysis");
        ddvis3.generateDotFunctionClusters(root,rdAnalysis->getCFAnalyzer(),"icfg_clustered.dot",false);
        
      }
    }
}

int main(int argc, char* argv[]) {
  // required for Sawyer logger streams
  ROSE_INITIALIZE;
  CodeThorn::initDiagnostics();
  //CodeThorn::CommandLineOptions args; must use provided args variable

  try {
    if(argc==1) {
      cout << "Error: wrong command line options."<<endl;
      exit(1);
    }

    // Command line option handling.
#ifdef USE_SAWYER_COMMANDLINE
    namespace po = Sawyer::CommandLine::Boost;
#else
    namespace po = boost::program_options;
#endif

    po::options_description desc
      ("analyterix V0.2\n"
       "Written by Markus Schordan\n"
       "Supported options");
  
    desc.add_options()
      ("help,h", "produce this help message.")
      ("rose-help", "show help for compiler frontend options.")
      ("version,v", "display the version.")
      ("stats", "display code statistics.")
      ("fi-constanalysis", "perform flow-insensitive constant analysis.")
      ("csv-fi-constanalysis",po::value< string >(), "generate csv-file [arg] with const-analysis data.")
      ("rd-analysis", "perform reaching definitions analysis.")
      ("rose-rd-analysis", "perform rose reaching definitions analysis.")
      ("lv-analysis", "perform live variables analysis.")
      ("ud-analysis", "use-def analysis.")
      ("at-analysis", "address-taken analysis.")
      ("annotate", "Annotate source code with analysis results.")
      ("csv-at-analysis",po::value< string >(), "generate csv-file [arg] with address-taken analysis data.")
      ("no-topological-sort", "do not initialize the worklist with topological sorted CFG.")
      ("icfg-dot", "generates the ICFG as dot file.")
      ("optimize-icfg", "prunes conditions with empty blocks, block begin, and block end icfg nodes.")
      ("no-optmize-icfg", "does not optimize icfg.")
      ("interval-analysis", "perform interval analysis.")
      ("csv-deadcode-unreachable", po::value< string >(), "perform interval analysis and generate csv-file [arg] with unreachable code.")
      ("csv-deadcode-deadstore", po::value< string >(), "perform liveness analysis and generate csv-file [arg] with stores to dead variables.")
      ("report-source-code", "report source code in generated csv files.")
      ("report-only-file-name", "report only file name in generated csv files (default: full path).")
      ("trace", "show operations as performed by selected solver.")
      ("check-static-array-bounds", "check static array bounds (uses interval analysis).")
      ("print-varid-mapping", "prints variableIdMapping")
      ("print-varid-mapping-array", "prints variableIdMapping with array element varids.")
      ("print-label-mapping", "prints mapping of labels to statements")
      ("print-inter-flow", "prints inter-procedural information call/entry/exit/callreturn.")
      ("prefix",po::value< string >(), "set prefix for all generated files.")
      ("start-function",po::value< string >(), "set name of function where analysis is supposed to start (default is 'main').")
      ("ignore-unknown-functions","ignore unknown functions (assume those functions are side effect free)")
      ("csv-stable", "only output csv data that is stable/portable across environments.")
      ("normalize-fcalls", "normalize only expressions with function calls [default].")
      ("normalize-all", "normalize program (transform all expressions).")
      ("inline", "inline functions (can increase precision of analysis).")
      ("unparse", "generate source code from internal representation.")
      ;
  //    ("int-option",po::value< int >(),"option info")

#if 0
    po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), args);
    po::notify(args);
#else
    args.parseAllowUnregistered(argc,argv,desc);
#endif
    if (args.isDefined("help")) {
      cout << "analyterix <filename> [OPTIONS]"<<endl;
      cout << desc << "\n";
      return 0;
    }
    if (args.isDefined("version")) {
      cout << "analyterix version 0.1\n";
      cout << "Written by Markus Schordan 2014\n";
      return 0;
    }
    if (args.isDefined("rose-help")) {
      argv[1] = strdup("--help");
    }
    if (args.isDefined("prefix")) {
      option_prefix=args.getString("prefix").c_str();
    }

    if (args.isDefined("optimize-icfg")) {
      option_optimize_icfg=true;
    }
    if (args.isDefined("no-optimize-icfg")) {
      option_optimize_icfg=false;
    }
    if (args.isDefined("trace")) {
      option_trace=true;
    }
    if(args.isDefined("stats")) {
      option_stats=true;
    }
    if(args.isDefined("start-function")) {
      option_start_function = args.getString("start-function");
    }
    if(args.isDefined("rd-analysis")) {
      option_rd_analysis=true;
    }
    if(args.isDefined("lv-analysis")) {
      option_lv_analysis=true;
    }
    if(args.isDefined("interval-analysis")) {
      option_interval_analysis=true;
    }
    if(args.isDefined("annotate")) {
      option_annotate_source_code=true;
    }
    if(args.isDefined("csv-deadcode-unreachable")) {
      option_interval_analysis = true;
      csvDeadCodeUnreachableFileName = args.getString("csv-deadcode-unreachable").c_str();
    }
    if(args.isDefined("csv-deadcode-deadstore")) {
      option_lv_analysis = true;
      csvDeadCodeDeadStoreFileName = args.getString("csv-deadcode-deadstore").c_str();
    }
    if(args.isDefined("check-static-array-bounds")) {
      option_interval_analysis=true;
      option_check_static_array_bounds=true;
    }
    if(args.isDefined("ud-analysis")) {
      option_rd_analysis=true; // required
      option_ud_analysis=true;
    }
    if(args.isDefined("rose-rd-analysis")) {
      option_rose_rd_analysis=true;
    }
    if(args.isDefined("fi-constanalysis")) {
      option_fi_constanalysis=true;
    }
    if (args.isDefined("csv-fi-constanalysis")) {
      csvConstResultFileName=args.getString("csv-fi-constanalysis").c_str();
      option_fi_constanalysis=true;
    }
    if(args.isDefined("at-analysis")) {
      option_at_analysis=true;
    }
    if (args.isDefined("csv-at-analysis")) {
      csvAddressTakenResultFileName=args.getString("csv-at-analysis").c_str();
      option_at_analysis=true;
    }
    if (args.isDefined("csv-stable")) {
      option_csv_stable=true;
    }
    if (args.isDefined("no-topological-sort")) {
      option_no_topological_sort=true;
    }
    if (args.isDefined("ignore-unknown-functions")) {
      option_ignore_unknown_functions=true;
    }
    if (args.isDefined("report-source-code")) {
      option_show_source_code=true;
    }
    if (args.isDefined("report-only-file-name")) {
      option_show_path=false;
    }

    // clean up string-options in argv
    for (int i=1; i<argc; ++i) {
      if (string(argv[i]) == "--prefix" 
       || string(argv[i]) == "--csv-fi-constanalysis"
       || string(argv[i]) == "--csv-at-analysis"
       || string(argv[i]) == "--csv-deadcode-unreachable"
       || string(argv[i]) == "--csv-deadcode-deadstore"
      ) {
        // do not confuse ROSE frontend
        argv[i] = strdup("");
        assert(i+1<argc);
        argv[i+1] = strdup("");
      }
    }

    cout << "INIT: Parsing and creating AST."<<endl;
    SgProject* root = frontend(argc,argv);

    if(option_trace) {
      cout << "TRACE: AST node count: " << root->numberOfNodesInSubtree() << endl;
    }
    //  AstTests::runAllTests(root);

   if(option_stats) {
      CodeThorn::ProgramStatistics::printBasicCodeInfo(root);
    }

  cout<<"STATUS: computing variableid mapping"<<endl;
  ProgramAbstractionLayer* programAbstractionLayer=new ProgramAbstractionLayer();
  if(args.isDefined("inline")) {
    programAbstractionLayer->setInliningOption(true);
  }
  if(args.isDefined("normalize-fcalls")) {
    programAbstractionLayer->setNormalizationLevel(1);
  }
  if(args.isDefined("normalize-all")) {
    programAbstractionLayer->setNormalizationLevel(2);
  }
  if(programAbstractionLayer->getInliningOption() && programAbstractionLayer->getNormalizationLevel()==0) {
    cerr<<"Error: inlining option selected without option 'normalize'."<<endl;
    exit(1);
  }
  programAbstractionLayer->initialize(root);
  if (args.isDefined("print-varid-mapping-array")) {
    //programAbstractionLayer->getVariableIdMapping()->setModeVariableIdForEachArrayElement(true);
  }

#if 0
  IOLabeler* iolabeler=new IOLabeler(root,&variableIdMapping);
  //cout<<"IOLabelling:\n"<<iolabeler->toString()<<endl;
#endif

  if (args.isDefined("print-varid-mapping")||args.isDefined("print-varid-mapping-array")) {
    programAbstractionLayer->getVariableIdMapping()->toStream(cout);
    return 0;
  }

  if(args.isDefined("print-label-mapping")) {
    cout<<(programAbstractionLayer->getLabeler()->toString());
    return 0;
  }

  if(args.isDefined("print-inter-flow")) {
    CFAnalysis* cfAnalysis=new CFAnalysis(programAbstractionLayer->getLabeler());
    Flow flow=cfAnalysis->flow(root);
    if(option_optimize_icfg) {
      cfAnalysis->optimizeFlow(flow);
    }
    InterFlow interFlow=cfAnalysis->interFlow(flow);
    cout<<interFlow.toString()<<endl;
    return 0;
  }

  if(args.isDefined("icfg-dot")) {
    CFAnalysis* cfAnalysis=new CFAnalysis(programAbstractionLayer->getLabeler());
    Flow flow=cfAnalysis->flow(root);
    if(option_optimize_icfg) {
      cfAnalysis->optimizeFlow(flow);
    }
    InterFlow interFlow=cfAnalysis->interFlow(flow);
    // merges interFlow into Flow
    cfAnalysis->intraInterFlow(flow,interFlow);
    cout << "generating icfg.dot."<<endl;
    string dotString=flow.toDot(programAbstractionLayer->getLabeler());
    writeFile("icfg.dot",dotString);

    cout << "generating icfg-clustered.dot."<<endl;
    DataDependenceVisualizer ddvis(programAbstractionLayer->getLabeler(),programAbstractionLayer->getVariableIdMapping(),"none");
    ddvis.generateDotFunctionClusters(root,cfAnalysis,"icfg-clustered.dot",false);

    delete cfAnalysis;
    exit(0);
  }
  runAnalyses(root, programAbstractionLayer->getLabeler(), programAbstractionLayer->getVariableIdMapping());

  if(args.isDefined("unparse")) {
    cout << "INFO: generating source code from internal representation."<<endl;
    root->unparse(0,0);
  }

  if(option_rose_rd_analysis) {
    Experimental::RoseRDAnalysis::generateRoseRDDotFiles(programAbstractionLayer->getLabeler(),root);
  }

  cout<< "STATUS: finished."<<endl;

  // main function try-catch
  } catch(CodeThorn::Exception& e) {
    cerr << "CodeThorn::Exception raised: " << e.what() << endl;
    return 1;
  } catch(std::exception& e) {
    cerr << "std::exception raised: " << e.what() << endl;
    return 1;
  } catch(char* str) {
    cerr << "*Exception raised: " << str << endl;
    return 1;
  } catch(const char* str) {
    cerr << "Exception raised: " << str << endl;
    return 1;
  } catch(string str) {
    cerr << "Exception raised: " << str << endl;
    return 1;
  }
  return 0;
}
