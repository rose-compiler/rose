#include "sage3basic.h"

#include "PragmaHandler.h"
#include "SgNodeHelper.h"
#include "Analyzer.h"
#include "Specialization.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;
using namespace CodeThorn;

size_t PragmaHandler::handlePragmas(SgProject* sageProject, CodeThorn::Analyzer* analyzer) {
  size_t num=extractAndParsePragmas(sageProject, analyzer);
  performSpecialization(sageProject, analyzer);
  return num;
}

// MS 2015: this function is implemented with boost tokenizers. If more features are added
// it's certainly better to replace it with a parser.
size_t PragmaHandler::extractAndParsePragmas(SgNode* root, CodeThorn::Analyzer* analyzer) {
  SgNodeHelper::PragmaList pragmaList=SgNodeHelper::collectPragmaLines("verify",root);
  if(size_t numPragmas=pragmaList.size()>0) {
    cout<<"STATUS: found "<<numPragmas<<" verify pragmas."<<endl;
    ROSE_ASSERT(numPragmas==1);
    SgNodeHelper::PragmaList::iterator i=pragmaList.begin();
    std::pair<std::string, SgNode*> p=*i;
    cout<<"STATUS: Verifying pragma: "<<p.first<<endl;

    SgNode* associatedNode=p.second;
    if(SgFunctionDeclaration* funDecl=isSgFunctionDeclaration(associatedNode)) {
      option_specialize_fun_name=SgNodeHelper::getFunctionName(funDecl);
    } else {
      cerr<<"Error: assure is associated with non function-declaration ("<<associatedNode->class_name()<<")"<<endl;
      exit(1);
    }
    // TODO:  determine function name
    //option_specialize_fun_name="kernel_jacobi_2d_imper";
    
    string text=p.first;
    char_separator<char> sep(" ");
    tokenizer< char_separator<char> > tokens(text, sep);

    int tokenCount=0;
    BOOST_FOREACH (const string& t, tokens) {
      if(tokenCount==0) {
        // check annotation
        if(t!="dataracefree") {
          cerr<<"Error: unknown annotation ("<<t<<")"<<endl;
          exit(1);
        } else {
          cout<<"INFO: verifying function "<<option_specialize_fun_name<<" to be data race free."<<endl;
        }
      } else {
        string text2=t;
        char_separator<char> sep2("==");
        tokenizer< char_separator<char> > tokens2(text2, sep2);
        tokenizer< char_separator<char> >::iterator tok_iter=tokens2.begin();
        string varName=*tok_iter++;
        string varValue=*tok_iter++;
        int varValueInt;
        istringstream(varValue) >> varValueInt;
    
        int paramNr=tokenCount-1;
        option_specialize_fun_param_list.push_back(paramNr);
        option_specialize_fun_const_list.push_back(varValueInt);

        if(tok_iter!=tokens.end()) {
          cerr<<"Error: parsing error in var-value pair: "<<text2<<endl;
          exit(1);
        }
        //cout<<"INFO: var-value:"<<varName<<":"<<varValue<<endl;
      }
      //cout << "TOKEN:"<<t << "." << endl;
      tokenCount++;
    }

    //option_specialize_fun_param_list.push_back(1);
    //option_specialize_fun_const_list.push_back(16);
    analyzer->setSkipUnknownFunctionCalls(true);
    analyzer->setSkipArrayAccesses(true);
    return numPragmas;
  }
  return 0;
}

void PragmaHandler::performSpecialization(SgProject* sageProject, CodeThorn::Analyzer* analyzer) {
  //TODO1: refactor into separate function
  int numSubst=0;
  if(option_specialize_fun_name!="")
  {
    Specialization speci;
    cout<<"STATUS: specializing function: "<<option_specialize_fun_name<<endl;

    string funNameToFind=option_specialize_fun_name;

    for(size_t i=0;i<option_specialize_fun_param_list.size();i++) {
      int param=option_specialize_fun_param_list[i];
      int constInt=option_specialize_fun_const_list[i];
      numSubst+=speci.specializeFunction(sageProject,funNameToFind, param, constInt, analyzer->getVariableIdMapping());
    }
    cout<<"STATUS: specialization: number of variable-uses replaced with constant: "<<numSubst<<endl;
    //root=speci.getSpecializedFunctionRootNode();
    //sageProject->unparse(0,0);
    //exit(0);
  }
}
