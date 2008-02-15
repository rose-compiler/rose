#include <vector>
#include <cmath>
#include <iostream>
#include <csignal>
#include <unistd.h>
#include <sys/time.h>
#include <string>
#include <cassert>
#include <dlfcn.h>
#include "matvec.h"

using namespace std;

double normalize(vector<double>& x) {
  double norm2 = 0;
  for (int i = 0; i < x.size(); ++i)
    norm2 += x[i] * x[i];
  double norm = sqrt(norm2);
  for (int i = 0; i < x.size(); ++i)
    x[i] /= norm;
  return norm;
}

double gettime() {
  timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1000000.;
}

void do_benchmark(string func_library, string func_sym,
		  vector<double>& y, const vector<double>& A,
		  const vector<double>& x) {

// This sends the signal to the OS which starts the saving of the checkpoint and kills the program
  kill(getpid(), SIGTSTP); // Checkpoint

// Upon startup the get the name of the new dynamic library to run
  getline(cin, func_library);
  void* func_lib = dlopen(func_library.c_str(), RTLD_LAZY);
  if (!func_lib) {cerr << dlerror() << endl; assert (false);}
  typedef void Func(vector<double>&, const vector<double>&, 
		    const vector<double>&);
  Func* func = (Func*)dlsym(func_lib, func_sym.c_str());
  if (!func) {cerr << dlerror() << endl; assert (false);}
  double start = gettime();
  (*func)(y, A, x);
  double stop = gettime();
  dlclose(func_lib);
  cout << stop - start << endl;
  exit(0);
}

double largest_eigenvalue(const vector<double>& A) {
  // Assumed to be square and fill the entire vector
  int size = (int)sqrt((double)(A.size()));
  vector<double> x(size);
  for (int i = 0; i < size; ++i)
    x[i] = drand48();
  normalize(x);

  double factor;
  for (int iter = 0; iter < 100; ++iter) {
    vector<double> y(size);
// Start of part to handle checkpointing and benchmarking
    if (iter == 10)
      do_benchmark("./matvec.so", "matvec", y, A, x);
// End of part to handle checkpointing and benchmarking
    matvec(y, A, x);
    factor = normalize(y);
    // cout << "In iteration " << iter << ", eigenvalue is " << factor << endl;
    swap(x, y);
  }

  return factor;
}
