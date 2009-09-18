#include <rose.h>
#include <interp_smt.h>

using namespace std;
using namespace Interp;
using namespace Interp::smtBV;
using namespace smtlib;
using namespace smtlib::QF_BV;

int main(int argc, char **argv)
   {
     vector<string> argvList(argv, argv+argc);
     bool expectAssertionFailure = CommandlineProcessing::isOption(argvList, "-interp:", "expectAssertionFailure", true);
     try
        {
          SMTInterpretation interp;
          interp.parseCommandLine(argvList);

          SgProject *prj = frontend(argvList);
          SgSourceFile *file = isSgSourceFile((*prj)[0]);
          ROSE_ASSERT(file != NULL);
          SgGlobal *global = file->get_globalScope();
          SgFunctionSymbol *testSym = global->lookup_function_symbol("test");

          StackFrameP head(new SMTStackFrame(&interp, testSym));
          ValueP x (new BVValue(bvbaseP(new bvname(Bits32, "x")), PTemp, head));
          ValueP rv = head->interpFunction(vector<ValueP>(1, x));
          cout << "Returned " << (rv.get() ? rv->show() : "<<nothing>>") << endl;
        }
     catch (InterpError &ie)
        {
          if (expectAssertionFailure && ie.err.find("Assertion failed") != string::npos)
             {
               cout << "Expected interpreter error occurred: " << ie.err << endl;
               ie.dumpCallStack(cout);
               return 0;
             }
          cerr << "Interpreter error occurred: " << ie.err << endl;
          ie.dumpCallStack(cerr);
          return 1;
        }
     catch (solveerror &se)
        {
          cerr << "Solver error occurred: " << se.err << endl;
          return 1;
        }
     if (expectAssertionFailure)
        {
          cerr << "Test expected assertion failure, but no assertion failed.  Exiting with failure..." << endl;
          return 1;
        }
     else
          return 0;
   }

