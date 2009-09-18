#include <rose.h>
#include <interp_extcall.h>

using namespace std;
using namespace Interp;
using namespace Interp::extcall;

int main(int argc, char **argv)
   {
     try
        {
          vector<string> argvList(argv, argv+argc);
          Interpretation interp;
          interp.parseCommandLine(argvList);

          SgProject *prj = frontend(argvList);
          SgSourceFile *file = isSgSourceFile((*prj)[0]);
          ROSE_ASSERT(file != NULL);
          SgGlobal *global = file->get_globalScope();
          SgFunctionSymbol *testSym = global->lookup_function_symbol("main");

          vector<void *> libList = buildLibraryList(prj);
          StackFrameP head(new ExternalCallingStackFrame(libList, &interp, testSym));
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

