#include "CppStdUtilities.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

#if GCC_VERSION >= 40900
#include <regex>
#endif

using namespace std;

// C++11, requires at least gcc 4.9
namespace CppStdUtilities {

  bool readDataFile(std::string fileName, DataFileVector& dfv) {
    string line;
    ifstream infile(fileName);
    if(infile.is_open()) {
      // get all the lines
      while(getline(infile,line)) {
        // remove newline if present at the end of line
        if (line.size()>0 && line.back()=='\n') {
          line.pop_back(); // C++11: str.pop_back();
        }
        dfv.push_back(line);
      }
      infile.close();
      return true;
    } else {
      return false;
    }
  }
  
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

  bool appendFile(std::string filename, std::string data) {
    std::ofstream myfile;
    myfile.open(filename.c_str(),std::ios::app);
    if(myfile.good()) {
      myfile << data;
      myfile.close();
      return true;
    } else {
      return false;
    }
  }

  bool isPrefix(string prefix, string s) {
    // fast and handles also case that s is shorter than prefix
    return s.compare(0,prefix.size(),prefix)==0;
  }

  bool isPostfix(std::string const &postfix, std::string const &s) {
    if (s.length() >= postfix.length()) {
      return (0 == s.compare (s.length() - postfix.length(), postfix.length(), postfix));
    } else {
      return false;
    }
  }

  std::vector<std::string> splitByComma(const string& input) {
    return splitByRegex(input,",");
  }

  std::vector<std::string> splitByTab(const string& input) {
    return splitByRegex(input,"\\t");
  }

  std::vector<std::string> splitBySpaces(const string& input) {
    return splitByRegex(input,"\\s+");
  }

  std::vector<std::string> splitByRegex(const string& input, const string& regex) {
    // passing -1 as the submatch index parameter performs splitting
    std::regex re(regex);
    std::sregex_token_iterator first{input.begin(), input.end(), re, -1},last;
    return {first, last};
  }
}
