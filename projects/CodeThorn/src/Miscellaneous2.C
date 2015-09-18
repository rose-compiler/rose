/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "Miscellaneous2.h"
#include <cctype>

using namespace std;

string SPRAY::replace_string(string toModify, string toReplace, string with) {
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
SPRAY::Parse::checkWord(string w,istream& is) {
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
SPRAY::Parse::parseString(string w,istream& is) {
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
    throw "Parser Error.";
  }
}

bool
SPRAY::Parse::integer(istream& is, int& num) {
  if(std::isdigit(is.peek())) {
    is>>num;
    return true;
  } else {
    return false;
  }
}

int
SPRAY::Parse::spaces(istream& is) {
  int num=0;
  while(is.peek()==' ') {
    is.get();
    num++;
  }
  return num;
}

int
SPRAY::Parse::whitespaces(istream& is) {
  int num=0;
  while(std::isspace(is.peek())) {
    is.get();
    num++;
  }
  return num;
}

list<int>
SPRAY::Parse::integerList(string liststring) {
  list<int> intList;
      stringstream ss(liststring);
    if(ss.peek()=='[')
      ss.ignore();
    else
      throw "Error: parse integer-values: wrong input format (at start).";
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
      throw "Error: parse integer-values: wrong input format (at end).";
#endif
    return intList;
}

set<int>
SPRAY::Parse::integerSet(string setstring) {
  set<int> intSet;
      stringstream ss(setstring);
    if(ss.peek()=='{')
      ss.ignore();
    else
      throw "Error: parse integer-values: wrong input format (at start).";
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
      throw "Error: parse integer-values: wrong input format (at end).";
#endif
    return intSet;
}

