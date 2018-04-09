#include "CppStdUtilities.h"
#include <fstream>
#include <regex>

using namespace std;

// C++11, requires at least gcc 4.9
namespace CppStdUtilities {
  bool writeFile(std::string filename, std::string data) {
    std::ofstream myfile;
    myfile.open(filename.c_str(),std::ios::out);
    if(myfile.good()) {
      myfile << data;
      myfile.close();
      return true;
    } else {
      return false;
    }
  }

  std::vector<std::string> splitByComma(const string& input) {
    return mysplit(input,",");
  }

  std::vector<std::string> splitBySpaces(const string& input) {
    return mysplit(input,"\\s+");
  }

  std::vector<std::string> mysplit(const string& input, const string& regex) {
    // passing -1 as the submatch index parameter performs splitting
    std::regex re(regex);
    std::sregex_token_iterator first{input.begin(), input.end(), re, -1},last;
    return {first, last};
  }
}
