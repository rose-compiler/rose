#include <rose.h>
#include <interp_extcall.h>

using namespace std;
using namespace Interp;
using namespace Interp::extcall;

int main(int argc, char **argv)
   {
     Interpretation interp;
     try
        {
          vector<string> argvList(argv, argv+argc);
          interp.parseCommandLine(argvList);

          SgProject *prj = frontend(argvList);
          SgFunctionSymbol *testSym = prjFindGlobalFunction(prj, "main");
          ROSE_ASSERT(testSym != NULL);

          vector<void *> libList = buildLibraryList(prj);
          StackFrameP head(new ExternalCallingStackFrame(libList, &interp, testSym));
          head->initializeGlobals(prj);
          ValueP argcVal (new IntValue(0, PTemp, head));
          ValueP argvVal (new PointerValue(ValueP(), PTemp, head));
          vector<ValueP> params;
          params.push_back(argcVal);
          params.push_back(argvVal);
          ValueP rv = head->interpFunction(params);
          cout << "Returned " << (rv.get() ? rv->show() : "<<nothing>>") << endl;
        }
     catch (InterpError &ie)
        {
          cerr << "Interpreter error occurred: " << ie.err << endl;
          ie.dumpCallStack(cerr);
          return 1;
        }
     return 0;
   }

