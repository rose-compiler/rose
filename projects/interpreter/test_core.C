#include <rose.h>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <interp_core.h>

using namespace std;
using namespace Interp;

int main(int argc, char **argv)
   {
     Interpretation interp;
     vector<string> argvList(argv, argv+argc);
     string expectedReturnValue, expectedReturnStr;
     CommandlineProcessing::isOptionWithParameter(argvList, "-interp:", "expectedReturnValue", expectedReturnValue, true);
     CommandlineProcessing::isOptionWithParameter(argvList, "-interp:", "expectedReturnStr", expectedReturnStr, true);
     bool expectUndefinedReturnValue = CommandlineProcessing::isOption(argvList, "-interp:", "expectUndefinedReturnValue", true);
     try
        {
          interp.parseCommandLine(argvList);

          SgProject *prj = frontend(argvList);
          SgFunctionSymbol *testSym = prjFindGlobalFunction(prj, "test");
          ROSE_ASSERT(testSym != NULL);

          StackFrameP head(new StackFrame(&interp, testSym));
          head->initializeGlobals(prj);
          vector<ValueP> args;
          args.push_back(ValueP(new IntValue(1, PTemp, head)));
          ValueP returnStr;
          if (expectedReturnStr != "")
             {
               returnStr = ValueP(new CompoundValue(SgTypeChar::createType(), expectedReturnStr.size()+1, PTemp, head));
               args.push_back(ValueP(new PointerValue(returnStr, PTemp, head)));
             }

          ValueP rv = head->interpFunction(args);
          if (expectUndefinedReturnValue)
             {
               if (rv->prim()->valid())
                  {
                    cerr << "Return value expected to be undefined, got " << rv->show() << endl;
                    return 1;
                  }
             }
          if (expectedReturnValue != "")
             {
               int rvInt = rv->prim()->getConcreteValueInt();
               stringstream ss;
               ss << rvInt;
               if (ss.str() != expectedReturnValue)
                  {
                    cerr << "Return value expected to be " << expectedReturnValue << ", got " << rv->show() << endl;
                    return 1;
                  }
             }
          if (expectedReturnStr != "")
             {
               for (size_t i = 0; i <= expectedReturnStr.size(); ++i)
                  {
                    char expectedChar = i < expectedReturnStr.size() ? expectedReturnStr[i] : 0,
                         computedChar = returnStr->primAtOffset(i)->getConcreteValueChar();
                    if (expectedChar != computedChar)
                       {
                         cerr << "Return string at " << i << " expected to be " << expectedChar+0 << ", got " << computedChar+0 << endl;
                         return 1;
                       }
                  }
             }
          cout << "Returned " << (rv.get() ? rv->show() : "<<nothing>>") << endl;
          return 0;
        }
     catch (InterpError &ie)
        {
          cerr << "Interpreter error occurred: " << ie.err << endl;
          ie.dumpCallStack(cerr);
          return 1;
        }
        return 0;
   }
