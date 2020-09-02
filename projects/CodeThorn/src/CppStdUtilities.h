#ifndef CPP_STD_UTILITIES_H
#define CPP_STD_UTILITIES_H

#include <string>
#include <vector>
#include <iostream>

namespace CppStdUtilities {

  // these utilities only use C++11 standard features. There are no other dependences.

  typedef std::vector<std::string> DataFileVector;

  // Read data file and add the lines in the provided vector. Chops off a newline if it is
  // present at the end of a line.
  // Returns true if reading the file was successful, otherwise false.
  bool readDataFile(std::string fileName, DataFileVector& dfv);

  // write the string 'data' to file with name 'filename'. It opens and closes the file.
  // if the file can be written the function returns true, otherwise false.
  bool writeFile(std::string filename, std::string data);

  // append the string 'data' to file with name 'filename'. It opens and closes the file.
  // if data can be written to the file  the function returns true, otherwise false.
  bool appendFile(std::string filename, std::string data);

  // is true if string 'prefix' is a prefix of string 's'.
  // e.g. "cc" is a prefix of "cccbba"
  bool isPrefix(std::string prefix, std::string s);
  
  // is true if string 'postfix' is a postfix of string 's'.
  // e.g. "bba" is a postfix of "cccbba"
  bool isPostfix(std::string const &postfix, std::string const &s);

  // splits a string by commas into a vector of strings (C++11, requires at least gcc 4.9)
  std::vector<std::string> splitByComma(const std::string& input);

    // splits a string by tabs into a vector of strings (C++11, requires at least gcc 4.9)
  std::vector<std::string> splitByTab(const std::string& input);

  // splits a string by spaces into a vector of strings. Consecutive
  // spaces are considered as one separator. e.g. "a  b  c" is split
  // into a vector of 3 strings.
  std::vector<std::string> splitBySpaces(const std::string& input);

  // splits a given string 'input' into a vector of strings, using the
  // regular expression 'regex' (e.g. std::regex re(",");
  // splitByRegex("a,b,c",re) is split into a vector of 3 strings)
  std::vector<std::string> splitByRegex(const std::string& input, const std::string& regex);

  // prints an arbitrary container, elements must provide '<<' operator
  template<class Container>
    void printContainer(const Container& s, const char* seperator = " ", std::ostream& output = std::cout) {
    for(typename Container::const_iterator iter = s.begin();
        iter != s.end();
        iter++) {
      if(iter!=s.begin()) {
        output << seperator;
      }
      output << *iter;
    }
    output << std::endl;
  }
}

#endif
