#include <rose.h>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <interp_core.h>
#include <interp_mpi.h>

using namespace std;
using namespace Interp;

namespace Interp {
namespace mpi {

string MPI_InitValue::functionName() const { return "MPI_Init"; }

ValueP MPI_InitValue::call(SgFunctionType *fnType, const vector<ValueP> &args) const
   {
     cerr << "MPI_Init called" << endl;
     return ValueP();
   }

StackFrameP MPIStackFrame::newStackFrame(SgFunctionSymbol *funSym, ValueP thisBinding)
   {
     return StackFrameP(new MPIStackFrame(interp(), funSym, thisBinding));
   }

ValueP MPIStackFrame::evalFunctionRefExp(SgFunctionSymbol *sym)
   {
     if (sym->get_name() == "MPI_Init")
          return ValueP(new MPI_InitValue(PTemp, shared_from_this()));

     return StackFrame::evalFunctionRefExp(sym);
   }

}
}

/*
int main(int argc, char **argv)
   {
     SgProject *prj = frontend(argc, argv);
     SgSourceFile *file = isSgSourceFile((*prj)[0]);
     ROSE_ASSERT(file != NULL);
     SgGlobal *global = file->get_globalScope();
     SgFunctionSymbol *testSym = global->lookup_function_symbol("test");

     Interpretation interp;
     StackFrameP head(new MPIStackFrame(&interp, testSym));
     ValueP x (new IntValue(1, PTemp, head));
     ValueP rv = head->interpFunction(vector<ValueP>(1, x));
     cout << "Returned " << (rv.get() ? rv->show() : "<<nothing>>") << endl;
   }

*/
