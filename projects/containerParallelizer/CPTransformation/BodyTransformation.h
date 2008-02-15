#include "BodyTransformationParser.h"
#include "AgProcessing.h"

class BodyTransformation : AgProcessing<BodyTransformationParser> {
 public:
  // note: we override the virtual function evaluate here
  string transform(SgForStatement* astNode, string iVarName, string iContName) {
    assert(astNode);
    SgBasicBlock* astForBodyNode=astNode->get_loop_body();
    assert(astForBodyNode);
    cout << "BODY-TRANSFORMATION:START:STRING:" << astForBodyNode->unparseToString() << endl;
    ScannerType* scanner=new ScannerType(astForBodyNode);
    assert(scanner);
    ErrorType* error=new ErrorType("SourceName",scanner);
    BodyTransformationParser* parser=new BodyTransformationParser(scanner,error);
    parser->iVarName=iVarName;
    parser->iContName=iContName;

    scanner->Reset();
    parser->InitSubLanguage();
    scannerTest(scanner);

    scanner->Reset();
    parser->InitSubLanguage();
    DerefExpAttribute derefExpSpec;
    DerefExpSynAttribute derefExpSyn;
    parser->SgBasicBlockNT(derefExpSpec,derefExpSyn);
    if (error->Errors) {
      cout << "\nPARSE-ERROR: REMAINING INPUT:\n";
      scannerTest(scanner);
      cout << endl;
      exit(EXIT_FAILURE); 
    }
    string ret=astForBodyNode->unparseToCompleteString();
    cout << "BODY-TRANSFORMATION:RETURN:STRING:" << ret << endl;
    cout << "***" << endl;
    return ret;
  }
};
