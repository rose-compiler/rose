#include "rose.h"
#include <Rose/CommandLine.h> // Commandline support in librose
#include <Sawyer/CommandLine.h>
using namespace std;
using namespace Sawyer::Message::Common; // if you want unqualified DEBUG, WARN, ERROR, FATAL, etc.
Sawyer::Message::Facility mlog; // a.k.a., Rose::Diagnostics::Facility

bool enable_debug = false;
bool enable_verbose = false;

Sawyer::CommandLine::ParserResult
parseCommandLine(int argc, char *argv[]) {
    using namespace Sawyer::CommandLine;

    SwitchGroup ACATSTrimmer;
    ACATSTrimmer.doc("The following switches are specific to scalarizer.");
  
    bool showRoseHelp = false;
    ACATSTrimmer.insert(Switch("rose:help")
             .longPrefix("-")
             .intrinsicValue(true, showRoseHelp)
             .doc("Show the old-style ROSE help.")); 

    ACATSTrimmer.insert(Switch("debug")
                .intrinsicValue(true, enable_debug)
                .doc("Enable the debugging mode"));

    ACATSTrimmer.insert(Switch("verbose")
                .intrinsicValue(true, enable_verbose)
                .doc("Enable the verbose mode"));

    Parser parser;
    parser
        .purpose("ACATSTrimmer")
        .doc("synopsis", "@prop{programName} [@v{switches}] [@v{files}] ")
        .doc("description",
             "This program trims out the SUPPORT functions for ACATS tests  "
             "to allow Ada2CPP to process. ");

    parser.skippingUnknownSwitches(true);
    return parser.with(Rose::CommandLine::genericSwitches()).with(ACATSTrimmer).parse(argc, argv).apply();
}

stack<SgConcatenationOp*> opStack;
vector<SgConcatenationOp*> removeList;
stack<SgFunctionCallExp*> funcCallStack;
vector<SgFunctionCallExp*> removeFucList;

class ACATSTraversal : public AstSimpleProcessing
{
  public:
    virtual void visit(SgNode* n);
};


void ACATSTraversal::visit(SgNode* n)
{
  //std::cout << n->class_name() << std::endl;
  switch(n->variantT())
  {
    case V_SgConcatenationOp:
      {
        SgConcatenationOp* concatOp = isSgConcatenationOp(n);
        opStack.push(concatOp);
        break;
      }
    case V_SgStringVal:
      {
        std::cout << "String:" << isSgStringVal(n)->get_value() << std::endl;
        break;
      }
    case V_SgFunctionCallExp:
      {
        SgFunctionSymbol* funcSym = isSgFunctionCallExp(n)->getAssociatedFunctionSymbol();
        //std::cout << "funcName:" << isSgFunctionCallExp(n)->getAssociatedFunctionSymbol()->get_name() << std::endl;
        std::string funcName;
        if(funcSym) 
           funcName = funcSym->get_name().getString();
        if(funcName.compare("operator&") == 0)
          funcCallStack.push(isSgFunctionCallExp(n)); 
        break;
      }
    default:
      break;

  }
}

void concatAdaString(SgFunctionCallExp* funcCallExp)
{

  SgExprListExp* exprListExp = funcCallExp->get_args();

  SgExpressionPtrList exprList = exprListExp->get_expressions();

  string newval = "";
  for(auto e:exprList)
  {
    SgStringVal* strVal = isSgStringVal(e);
    if(strVal)
      newval += strVal->get_value();
    else return;
  }
  SgStringVal* newStrVal = SageBuilder::buildStringVal(newval);
  newStrVal->set_stringDelimiter(isSgStringVal(exprList[0])->get_stringDelimiter());
  cout << "Concat new string:" << newval << endl;


    SageInterface::replaceExpression(funcCallExp, newStrVal, false);
//    removeList.push_back(concatOp);
}

void concatAdaString(SgConcatenationOp* concatOp)
{
  SgStringVal* rhs = isSgStringVal(concatOp->get_rhs_operand());
  SgStringVal* lhs = isSgStringVal(concatOp->get_lhs_operand());
  if(rhs != nullptr && lhs != nullptr)
  {
    string rhsval = rhs->get_value();
    string lhsval = lhs->get_value();
    string newval = lhsval + rhsval;
    SgStringVal* newStrVal = SageBuilder::buildStringVal(newval);
    newStrVal->set_stringDelimiter(rhs->get_stringDelimiter());
    cout << "Concat new string:" << newval<< " from " << rhsval << " " << lhsval << endl;


    SageInterface::replaceExpression(concatOp, newStrVal, false);
//    removeList.push_back(concatOp);
  }
  else
  {
    cout << "not both children are string!!" << endl;
  }
  
}


int main( int argc, char * argv[] ){

  ROSE_INITIALIZE;
  Rose::Diagnostics::initAndRegister(&mlog, "ACATSTrimmer");

  Sawyer::CommandLine::ParserResult cmdline = parseCommandLine(argc, argv);
  std::vector<std::string> positionalArgs = cmdline.unparsedArgs();
  positionalArgs.insert(positionalArgs.begin(), argv[0]);

  SgProject* project = frontend(positionalArgs);

  ACATSTraversal acats; 
  acats.traverseInputFiles(project,preorder);

  while(!opStack.empty())
  {
    SgConcatenationOp* concatOp = opStack.top();
    concatAdaString(concatOp);
    opStack.pop();
  }
  while(!funcCallStack.empty())
  {
    SgFunctionCallExp* funcCallExp = funcCallStack.top();
    concatAdaString(funcCallExp);
    funcCallStack.pop();
  }
/*
  for(vector<SgConcatenationOp*>::iterator i= removeList.begin(); i != removeList.end(); ++i)
  {
    SageInterface::deleteAST(*i);
  }
*/
    generateDOT(*project );
  return backend(project);
}
