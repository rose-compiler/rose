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

int 
getdir (string dir, map<string, vector<string> > &files) {
  DIR *dp;
  struct dirent *dirp;
  if((dp  = opendir(dir.c_str())) == NULL) {
    cout << "Error(" << errno << ") opening " << dir << endl;
    return errno;
  }

  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_type==4)  //8 for file, 4 directory
      if (string(dirp->d_name)!="." &&
	  string(dirp->d_name)!="..") {
	// for each language (C, C++ and Fortran,
	// find every project within RTED
	string language = dir+"/"+string(dirp->d_name);
	string plain_language = string(dirp->d_name);

	DIR *dp2;
	struct dirent *dirp2;
	if((dp2  = opendir(language.c_str())) == NULL) {
	  cout << "Error(" << errno << ") opening " << language << endl;
	  return errno;
	}
	while ((dirp2 = readdir(dp2)) != NULL) {
	  if (dirp2->d_type==4)  //8 for file, 4 directory
	    if (string(dirp2->d_name)!="." &&
		string(dirp2->d_name)!="..") {
	      string subdir = language + "/"+string(dirp2->d_name);
	      files[plain_language].push_back(subdir);
	    }
	}
	closedir(dp2);
      }
  }
  closedir(dp);
  return 0;
}


int main(int argc, char** argv) {
  ROSE_ASSERT(argc>1);
  rtedpath=argv[1];
  cerr <<"Running RTED in :" <<rtedpath << endl;

  map<string,vector<string> > files;
  getdir(rtedpath,files);
  map<string,vector<string> >::const_iterator it = files.begin();
  for (;it != files.end() ;++it) {
    string lang = it->first;
    vector<string> path = it->second;
    for (int i=0;i<path.size();++i) {
      cout << "Language : " << lang << "  Path : " << path[i] << endl;
    }
  }
}


