#include <rose.h>
#include <string>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <iostream>

#include <sys/stat.h>
#include "boost/filesystem/operations.hpp"

using namespace boost;
using namespace std;
using namespace boost::filesystem;


static string rtedpath;
static vector<string> cdirs;

void
createCStruct() {
  cdirs.push_back("C");
}

int getdir (string dir, vector<string> &files) {
  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(dir.c_str())) == NULL) {
    cout << "Error(" << errno << ") opening " << dir << endl;
    return errno;
  }

  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_type==4)  //8 for file, 4 directory
      if (string(dirp->d_name)!="." &&
	  string(dirp->d_name)!="..")
	files.push_back(string(dirp->d_name));
  }
  closedir(dp);
  return 0;
}


int main(int argc, char** argv) {
  ROSE_ASSERT(argc>1);
  rtedpath=argv[1];
  cerr <<"Running RTED in :" <<rtedpath << endl;
  createCStruct();


  vector<string> files = vector<string>();
  getdir(rtedpath,files);
  cout << " Found : " << files.size() << endl;
  for (unsigned int i = 0;i < files.size();i++) {
    cout << files[i] << endl;
  }
}


