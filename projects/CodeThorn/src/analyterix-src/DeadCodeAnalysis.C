
#include "sage3basic.h"
#include "DeadCodeAnalysis.h"
#include "Flow.h"
#include "Miscellaneous2.h"
#include "CppStdUtilities.h"

using namespace std;
using namespace CodeThorn;

void DeadCodeAnalysis::setOptionTrace(bool flag) {
  option_trace=flag;
}

void DeadCodeAnalysis::setOptionSourceCode(bool flag) {
  optionSourceCode=flag;
}

void DeadCodeAnalysis::setOptionSystemHeaders(bool flag) {
  optionSystemHeaders=flag;
}

void DeadCodeAnalysis::setOptionFilePath(bool flag) {
  optionFilePath=flag;
}

void DeadCodeAnalysis::writeUnreachableCodeResultFile(CodeThorn::IntervalAnalysis* intervalAnalyzer,
                                                      string csvDeadCodeUnreachableFileName) {
  ofstream deadCodeCsvFile;
  deadCodeCsvFile.open(csvDeadCodeUnreachableFileName.c_str());
  // Iteratate over all CFG nodes/ labels:
  Flow* flow=intervalAnalyzer->getFlow();
  for(Flow::const_node_iterator i = flow->nodes_begin(); i != flow->nodes_end(); ++i) {
    const Label& label = *i;
    // Do not output a function call twice (only the function call label and not the function call return label):
    if(!intervalAnalyzer->getLabeler()->isFunctionCallReturnLabel(label)) {
      /*const*/ IntervalPropertyState& intervalsLattice = *static_cast<IntervalPropertyState*>(intervalAnalyzer->getPreInfo(label.getId()));
      if(intervalsLattice.isBot()) {
        // Unreachable statement found:
        SgNode* correspondingNode = intervalAnalyzer->getLabeler()->getNode(label);
        ROSE_ASSERT(correspondingNode);
        // Do not output scope statements ({ }, ...)
        if(!isSgScopeStatement(correspondingNode)) {
          Sg_File_Info* fileInfo=correspondingNode->get_file_info();
          int lineNr=fileInfo->get_line();
          int colNr=fileInfo->get_col();
          string fileName=fileInfo->get_filenameString();
          ROSE_ASSERT(isSgLocatedNode(correspondingNode));
          bool generateOutput=(optionSystemHeaders? true: !SageInterface::insideSystemHeader(isSgLocatedNode(correspondingNode)));
          string filename=string(fileInfo->get_filename());
          for (auto headerFileName : excludedHeaders) {
            if(CppStdUtilities::isPostfix(headerFileName,filename)) {
              generateOutput=false;
            }
          }
          if(generateOutput) {
            if(lineNr>0) {
              deadCodeCsvFile << lineNr;
              deadCodeCsvFile <<","<< colNr;  
              if(optionFilePath) {
                deadCodeCsvFile <<","<<fileName;
              } else {
                // determine base file name (without path) of given file name (with path)
                std::string baseFileName = fileName.substr(fileName.find_last_of("/\\")+1);
                deadCodeCsvFile <<","<<baseFileName;
              }
              if(optionSourceCode) {
                deadCodeCsvFile <<","<< CodeThorn::replace_string(correspondingNode->unparseToString(), ",", "/*comma*/");
              }
              deadCodeCsvFile << endl;
            }
          }
        } else {
          //cout<<"DEBUG: EXCLUDING: "<<label.getId()<<" : "<<intervalAnalyzer->getLabeler()->getNode(label)->unparseToString()<<endl;
        }
      }
    } else {
      //cout<<"DEBUG: FUNCTION CALLRETURN LABEL: "<<label.getId()<<" : "<<intervalAnalyzer->getLabeler()->getNode(label)->unparseToString()<<endl;
    }
  }
  deadCodeCsvFile.close();
}

void DeadCodeAnalysis::writeDeadAssignmentResultFile(CodeThorn::LVAnalysis* lvAnalysis,
                                                     string deadCodeCsvFileName) {
  ofstream deadCodeCsvFile;
  deadCodeCsvFile.open(deadCodeCsvFileName.c_str());
  if(option_trace) {
    cout << "TRACE: checking for dead assignments." << endl;
  }
  // Iteratate over all CFG nodes/ labels:
  for(CodeThorn::Flow::const_node_iterator labIter = lvAnalysis->getFlow()->nodes_begin(); labIter != lvAnalysis->getFlow()->nodes_end(); ++labIter) {
    const Label& label = *labIter;
    // Do not output a function call twice (only the function call return label and not the function call label):
    if(!lvAnalysis->getLabeler()->isFunctionCallLabel(label)) {
      /*const*/ SgNode* correspondingNode = lvAnalysis->getLabeler()->getNode(label);
      ROSE_ASSERT(correspondingNode);
      if(/*const*/ SgExprStatement* exprStmt = isSgExprStatement(correspondingNode)) {
        correspondingNode = exprStmt->get_expression();
      }
      /*const*/ SgNode* association = 0;
      // Check if the corresponding node is an assignment or an initialization:
      if(isSgAssignOp(correspondingNode)) {
        association = correspondingNode;
      }
      else if(SgVariableDeclaration* varDecl = isSgVariableDeclaration(correspondingNode)) {
        SgInitializedName* initName = SgNodeHelper::getInitializedNameOfVariableDeclaration(varDecl);
        ROSE_ASSERT(initName);
        // Check whether there is an initialization that can be eliminated (reference initialization can not be eliminated).
        if(!SgNodeHelper::isReferenceType(initName->get_type()) && initName->get_initializer()) {
          association = correspondingNode;
        }
      }
      
      if(association) {
        if(option_trace) {
          cout << endl << "association: " << association->unparseToString() << endl;
        }
        VariableIdSet assignedVars = AstUtility::defVariables(association, 
                                                                            *lvAnalysis->getVariableIdMapping(), 
                                                                            lvAnalysis->getPointerAnalysis());
        /*const*/ LVLattice& liveVarsLattice = *static_cast<LVLattice*>(lvAnalysis->getPreInfo(label.getId()));
        if(option_trace) {
          cout << "live: " << liveVarsLattice.toString(lvAnalysis->getVariableIdMapping()) << endl;
          cout << "assigned: " << endl;
        }
        bool minOneIsLive = false;
        for(VariableIdSet::const_iterator assignedVarIter = assignedVars.begin(); assignedVarIter != assignedVars.end(); ++assignedVarIter) {
          if(option_trace) {
            cout << (*assignedVarIter).toString(*lvAnalysis->getVariableIdMapping()) << endl;
          }
          if(liveVarsLattice.exists(*assignedVarIter)) {
            minOneIsLive = true;
            break;
          }
        }
        if(!minOneIsLive) {
          if(option_trace) {
            cout << "association is dead." << endl;
          }
          // assignment to only dead variables found:
          deadCodeCsvFile << correspondingNode->get_file_info()->get_line()
                          << "," << CodeThorn::replace_string(correspondingNode->unparseToString(), ",", "/*comma*/")
                          << endl;
        }
      }
    }
  }
  deadCodeCsvFile.close();
}
