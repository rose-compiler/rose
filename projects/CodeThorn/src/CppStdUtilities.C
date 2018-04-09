#include "CppStdUtilities.h"
#include <fstream>

using namespace std;

namespace CppStdUtilities {
  void write_file(std::string filename, std::string data) {
    std::ofstream myfile;
    myfile.open(filename.c_str(),std::ios::out);
    myfile << data;
    myfile.close();
  }
}
