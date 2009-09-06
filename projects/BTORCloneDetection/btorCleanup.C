#include "rose.h"
#include "btorProblem.h"
#include <boost/lexical_cast.hpp>

using namespace std;

int main(int argc, char** argv) {
  BtorProblem p = BtorProblem::parse(stdin);
  fprintf(stdout, "%s", p.unparse().c_str());
  return 0;
}
