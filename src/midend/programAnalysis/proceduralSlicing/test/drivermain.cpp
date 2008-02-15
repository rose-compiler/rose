#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sys/time.h>

using namespace std;

// For timing the testing
double gettime(){
    timeval tv;
    gettimeofday(&tv, 0);
    // return time in seconds + time in microseconds/1000000
    return tv.tv_sec + tv.tv_usec / 1000000.;
  }

 
int main(int argc, char* argv[]){

  string srcdir = "inputfiles/";
  int no_files = 13; 
  string file[no_files];
 
  // The input files we test
  file[0] = "break1.c";
  file[1] = "char1.c";
  file[2] = "dowhile1.c";
  file[3] = "for1.c";
  file[4] = "for3.c";
  file[5] = "global1.c"; 
  file[6] = "if1.c";
  file[7] = "nested1.c";
  file[8] = "switch1.c";
  file[9] = "variable1.c";
  file[10] = "variable2.c";
  file[11] = "while1.c";
  file[12] = "while2.c";

  double start = gettime(); // timing the testing process...
  for(int i=0; i<no_files; i++){
    cout << "Testing file: " << srcdir+file[i] << endl;
    string cmd = "./test_slice "+srcdir+file[i];
    if(system(cmd.c_str())){
      cerr << "Failed testing" << srcdir+file[i] << endl;
      return -1;
    }
    else {
      cout << "Finished ok testing file: " << srcdir+file[i] << endl;
    }
  }
  double stop = gettime();
  double time_taken = stop - start;
  char *unit = " seconds";
  if(time_taken > 60){  // convert to minutes
    time_taken = time_taken/60;
    unit= " minutes";
  }
  cout << "Time taken to do the testing of " << no_files << " files is " 
       << time_taken << unit << endl;
  
  return 0;
}
