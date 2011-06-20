#include "MpiDeterminismAnalysis.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
  SgProject *project = frontend(argc, argv);

  cout << "You must be using the hacked mpi.h that defines things nicely for this to work!" << endl;

  //  ConstantPropagationAnalysis cp;

  MpiDeterminismAnalysis a;
  MpiDeterminism d = a.traverse(project);

  cout << "Analysis finds that this program is" << endl;
  cout << "\t";
  switch (d.source) {
  case DETERMINISTIC: cout << "deterministic"; break;
  case QUESTIONABLE: cout << "unclear"; break;
  case NONDETERMINISTIC: cout << "NONdeterminisitic"; break;
  }
  cout << " with respect to receive sources." << endl;
  cout << "\t";
  switch (d.tag) {
  case DETERMINISTIC: cout << "deterministic"; break;
  case QUESTIONABLE: cout << "unclear"; break;
  case NONDETERMINISTIC: cout << "NONdeterminisitic"; break;
  }
  cout << " with respect to receive tags." << endl;
  cout << "\t";
  switch (d.functions) {
  case DETERMINISTIC: cout << "deterministic"; break;
  case QUESTIONABLE: cout << "unclear"; break;
  case NONDETERMINISTIC: cout << "NONdeterminisitic"; break;
  }
  cout << " with respect to receive functions called." << endl;

  return 0;
}
