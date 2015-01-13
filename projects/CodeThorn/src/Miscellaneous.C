/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "Miscellaneous.h"
#include "CommandLineOptions.h"
#include <cctype>

void CodeThorn::nocheck(string checkIdentifier, bool checkResult) {
  check(checkIdentifier,checkResult,false);
}

bool checkresult=true; // used by check

void CodeThorn::check(string checkIdentifier, bool checkResult, bool check) {
  static int checkNr=1;
  cout << color("white") << "CHECK "<<checkNr<<": "; 
  if(!check) {
    cout<<color("white")<<"----";
  } else {
    if(checkResult) {
      cout<<color("green")<<"PASS";
    } else {
      cout<<color("red")<<"FAIL";
      checkresult=false;
    }
  }
  cout << " ";
  cout << color("white") << "["<<checkIdentifier << "]";
  checkNr++;
  cout<<color("normal")<<endl;
}

void CodeThorn::write_file(std::string filename, std::string data) {
  std::ofstream myfile;
  myfile.open(filename.c_str(),std::ios::out);
  myfile << data;
  myfile.close();
}

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

string CodeThorn::int_to_string(int x) {
  stringstream ss;
  ss << x;
  return ss.str();
}

string CodeThorn::color(string name) {
#ifndef CT_IGNORE_COLORS_BOOLOPTIONS
  if(!boolOptions["colors"]) 
    return "";
#endif
  string c="\33[";
  if(name=="normal") return c+"0m";
  if(name=="bold") return c+"1m";
  if(name=="bold-off") return c+"22m";
  if(name=="blink") return c+"5m";
  if(name=="blink-off") return c+"25m";
  if(name=="underline") return c+"4m";
  if(name=="default-text-color") return c+"39m";
  if(name=="default-bg-color") return c+"49m";
  bool bgcolor=false;
  string prefix="bg-";
  size_t pos=name.find(prefix);
  if(pos==0) {
    bgcolor=true;
    name=name.substr(prefix.size(),name.size()-prefix.size());
  }
  string colors[]={"black","red","green","yellow","blue","magenta","cyan","white"};
  int i;
  for(i=0;i<8;i++) {
    if(name==colors[i]) {
      break;
    }
  }
  if(i<8) {
    if(bgcolor)
      return c+"4"+int_to_string(i)+"m";
    else
      return c+"3"+int_to_string(i)+"m";
  }
  else
    throw "Error: unknown color code.";
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
    throw "Parser Error.";
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
CodeThorn::Parse::integerSet(string setstring) {
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

