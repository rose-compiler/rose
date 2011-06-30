#include "MpiDeterminismAnalysis.h"

#include <iostream>
#include <algorithm>
#include <numeric>

using std::max;

static struct MpiRecvFunction {
  const char *name;
  // The position of the source and tag arguments;
  int sourcePos;
  int tagPos;
} mpiRecvFunctions[] = {
     {"MPI_Recv", 3, 4}
     ,{"MPI_Irecv", 3, 4}
     ,{"MPI_Recv_init", 3, 4}
     ,{"MPI_Sendrecv", 8, 9}
     ,{"MPI_Sendrecv_replace", 5, 6}
};

MpiDeterminism combine(MpiDeterminism d1, MpiDeterminism d2)
{
  MpiDeterminism d;
  d.source = max(d1.source, d2.source);
  d.tag = max(d1.tag, d2.tag);
  d.functions = max(d1.functions, d2.functions);
  return d;
}

MpiDeterminism MpiDeterminismAnalysis::evaluateSynthesizedAttribute(SgNode *n, SynthesizedAttributesList childAttribs)
{
  MpiDeterminism children = std::accumulate(childAttribs.begin(), childAttribs.end(), MpiDeterminism(), combine);

  SgFunctionCallExp *fncall = dynamic_cast<SgFunctionCallExp *>(n);

  return fncall ? combine(checkCall(fncall), children) : children;
}

void MpiDeterminismAnalysis::bump(DeterminismState &target, DeterminismState value)
{
  target = max(target, value);
}

MpiDeterminism MpiDeterminismAnalysis::checkCall(SgFunctionCallExp *fncall)
{
  MpiDeterminism d;
  SgFunctionDeclaration *fn = fncall->getAssociatedFunctionDeclaration();
  SgName fnname = fn->get_name();

  for (int i = 0; i < sizeof(mpiRecvFunctions)/sizeof(mpiRecvFunctions[0]); ++i) {
    if (fnname == mpiRecvFunctions[i].name) {
      SgExpressionPtrList args = fncall->get_args()->get_expressions();

      // Source argument testing
      SgExpression *sourceArg = args[mpiRecvFunctions[i].sourcePos];
      SgVarRefExp *sourceVar = dynamic_cast<SgVarRefExp *>(sourceArg);
      SgIntVal *sourceInt = dynamic_cast<SgIntVal *>(sourceArg);
      if (sourceVar) {
	SgName name = sourceVar->get_symbol()->get_name();
	bump(d.source, name == "MPI_ANY_SOURCE" ? NONDETERMINISTIC : QUESTIONABLE);
      } else if (sourceInt) {
	if (sourceInt < 0) {
	  std::cerr << "Negative literal source rank seen - not using the hacked mpi.h?" << std::endl;
	  bump(d.source, QUESTIONABLE);
	}
      } else {
	bump(d.source, QUESTIONABLE);
      }

      // Tag argument testing
      SgExpression *tagArg = args[mpiRecvFunctions[i].tagPos];
      SgVarRefExp *tagVar = dynamic_cast<SgVarRefExp *>(tagArg);
      SgIntVal *tagInt = dynamic_cast<SgIntVal *>(tagArg);
      if (tagVar) {
	SgName name = tagVar->get_symbol()->get_name();
	bump(d.tag, name == "MPI_ANY_TAG" ? NONDETERMINISTIC : QUESTIONABLE);
      } else if (tagInt) {
	if (tagInt < 0) {
	  std::cerr << "Negative literal tag seen - not using the hacked mpi.h?" << std::endl;
	  bump(d.tag, QUESTIONABLE);
	}
      } else {
	bump(d.tag, QUESTIONABLE);
      }
      
    }
  }

  if (fnname == "MPI_Waitany" || fnname == "MPI_Testany" || fnname == "MPI_Waitsome" || fnname == "MPI_Testsome")
    bump(d.functions, NONDETERMINISTIC);

  return d;
}
