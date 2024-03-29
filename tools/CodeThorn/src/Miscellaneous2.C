#include "Miscellaneous2.h"
#include <cctype>
#include <cstdlib>
#include "CodeThornException.h"

//#include "boost/algorithm/string.hpp"
//#include "boost/algorithm/string/trim.hpp"
//#include "boost/algorithm/string/regex.hpp"
//#include "boost/regex.hpp"
//#include "boost/lexical_cast.hpp"
#include <regex>

using namespace std;

string CodeThorn::replace_string(string toModify, string toReplace, string with) {
  size_t index = 0;
  while (true) {
    /* Locate the substring to replace. */
    index = toModify.find(toReplace, index);
    if (index == string::npos) 
      break;
    /* Make the replacement. */
    toModify.replace(index, toReplace.size(), with);
    
    /* Advance index forward so the next iteration doesn't pick it up as well. */
    index += toReplace.size();
  }
  return toModify;
}

/* returns true if the string w can be parsed on stream is.
   otherwise false.
   returns true for an empty string w.
   istream remains unmodified if string s cannot be parsed.
 */
bool
CodeThorn::Parse::checkWord(string w,istream& is) {
  size_t i;
  for(i=0;i<w.size();i++) {
    if(is.peek()==w[i]) {
      is.get();
    } else {
      break;
    }
  }
  // ensure that the word is followed either by anychar or some char not in [a-zA-Z]
  if(i==w.size() && !std::isalpha(is.peek())) {
    return true;
  }
  if(i==0) return false;
  --i; // was peeked
  // putback all chars that were read
  while(i>0) {
    is.putback(w[i--]);
  }
  is.putback(w[0]); // note: i is unsigned
  return false;
}

/* Consumes input from istream if the string w can be parsed
   otherwise generates an error message and throws exception.
   Returns without performing any action for an empty string w.
*/
void
CodeThorn::Parse::parseString(string w,istream& is) {
  size_t i;
  char c;
  for(i=0;i<w.size();i++) {
    if(is.peek()==w[i]) {
      is >> c;
    } else {
      break;
    }
  }
  // check that string was sucessfully parsed
  if(i==w.size()) {
    return;
  } else {
    cerr<< "Error: parsing of \""<<w<<"\" failed."<<endl;
    string s;
    is>>s;
    cerr<< "Parsed "<<i<<"characters. Remaining input: "<<s<<"..."<<endl;
    throw CodeThorn::Exception("Parser Error.");
  }
}

bool
CodeThorn::Parse::integer(istream& is, int& num) {
  if(std::isdigit(is.peek())) {
    is>>num;
    return true;
  } else {
    return false;
  }
}

int
CodeThorn::Parse::spaces(istream& is) {
  int num=0;
  while(is.peek()==' ') {
    is.get();
    num++;
  }
  return num;
}

int
CodeThorn::Parse::whitespaces(istream& is) {
  int num=0;
  while(std::isspace(is.peek())) {
    is.get();
    num++;
  }
  return num;
}

list<int>
CodeThorn::Parse::integerList(string liststring) {
  list<int> intList;
      stringstream ss(liststring);
    if(ss.peek()=='[')
      ss.ignore();
    else
      throw CodeThorn::Exception("Error: parse integer-values: wrong input format (at start).");
    int i;
    while(ss>>i) {
      //cout << "DEBUG: input-var-string:i:"<<i<<" peek:"<<ss.peek()<<endl;    
      intList.push_back(i);
      if(ss.peek()==','||ss.peek()==' ')
        ss.ignore();
    }
#if 0
    if(ss.peek()==']')
      ss.ignore();
    else
      throw CodeThorn::Exception("Error: parse integer-values: wrong input format (at end).");
#endif
    return intList;
}

list<set<int> >
CodeThorn::Parse::integerSetList(string liststring) {
  list<set<int> > intList;
      stringstream ss(liststring);
    if(ss.peek()=='[')
      ss.ignore();
    else
      throw CodeThorn::Exception("Error: parse integer-values: wrong input format (at start).");
    string set;
    while(ss>>set) {
      //cout << "DEBUG: input-var-string:i:"<<i<<" peek:"<<ss.peek()<<endl;    
      intList.push_back(integerSet(set));
      if(ss.peek()==','||ss.peek()==' ')
        ss.ignore();
    }
#if 0
    if(ss.peek()==']')
      ss.ignore();
    else
      throw CodeThorn::Exception("Error: parse integer-values: wrong input format (at end).");
#endif
    return intList;
}

set<int>
CodeThorn::Parse::integerSet(string setstring) {
  set<int> intSet;
      stringstream ss(setstring);
    if(ss.peek()=='{')
      ss.ignore();
    else
      throw CodeThorn::Exception("Error: parse integer-values: wrong input format (at start).");
    int i;
    while(ss>>i) {
      //cout << "DEBUG: input-var-string:i:"<<i<<" peek:"<<ss.peek()<<endl;    
      intSet.insert(i);
      if(ss.peek()==','||ss.peek()==' ')
        ss.ignore();
    }
#if 0
    if(ss.peek()=='}')
      ss.ignore();
    else
      throw CodeThorn::Exception("Error: parse integer-values: wrong input format (at end).");
#endif
    return intSet;
}

int CodeThorn::randomIntInRange(pair<int,int> range) {
  int rangeLength = range.second - range.first + 1;
  return range.first + (rand() % rangeLength);
}

list<int> CodeThorn::nDifferentRandomIntsInSet(int n, set<int> values) {
  list<int> result;
  list<int> indices = nDifferentRandomIntsInRange(n, pair<int,int>(0, values.size() - 1));
  indices.sort();
  set<int>::iterator iterSet = values.begin();
  list<int>::iterator iterIndices = indices.begin();
  int index = 0;
  while(iterIndices != indices.end()) {
    // move to the next chosen set element (virtual index)
    while(index < *iterIndices) {
      ++iterSet;
      ++index;
    }
    // add set element to the results
    result.push_back(*iterSet);
    ++iterIndices;
  }
  return result;
}

list<int> CodeThorn::nDifferentRandomIntsInRange(int n, pair<int,int> range) {
  list<int> result;
  for (int i = 0; i < n; ++i) {
    int chosen_intermediate = randomIntInRange( pair<int,int>(range.first, (range.second - i)) );
    int chosen_final = chosen_intermediate;
    for (list<int>::const_iterator k=result.begin(); k!=result.end(); ++k) {
      if (*k <= chosen_intermediate) {
	++chosen_final;
      }
    }
    result.push_back(chosen_final);
  }
  return result;
}

std::vector<std::string> CodeThorn::Parse::commandLineArgs(std::string commandLine) {
  vector<std::string> v;	
  //std::split_regex(v, commandLine, std::regex("( )+")); // below code shows how to replace boost split_regex
  std::regex re("( )+");
  sregex_token_iterator it(commandLine.begin(), commandLine.end(), re, -1);
  sregex_token_iterator reg_end;
  for (; it != reg_end; ++it) {
    v.push_back(*it);
  }
  cout<<"Parsing command line: found "<<v.size()<<" arguments."<<endl;
  return v;
}
