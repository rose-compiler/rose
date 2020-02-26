/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "sage3basic.h" // included for the ROSE_ASSERT macro

#include "Miscellaneous.h"
#include "CodeThornCommandLineOptions.h"
#include "CodeThornLib.h"
#include <cctype>
#include <iomanip>
#include "CodeThornException.h"

//BOOST includes
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/regex.hpp"
#include "boost/regex.hpp"
#include "boost/lexical_cast.hpp"

using namespace std;

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

string CodeThorn::int_to_string(int x) {
  stringstream ss;
  ss << x;
  return ss.str();
}

pair<int,int> CodeThorn::parseCsvIntPair(string toParse) {
  vector<string> values; 
  boost::split(values, toParse, boost::is_any_of(",")); 
  ROSE_ASSERT(values.size() == 2); // needs to be a pair
  int valOne = boost::lexical_cast<int>(values[0]);
  int valTwo = boost::lexical_cast<int>(values[1]);
  return pair<int,int>(valOne, valTwo);
}

string CodeThorn::color(string name) {
#ifndef CT_IGNORE_COLORS_BOOLOPTIONS
  if(!args.getBool("colors")) 
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
    throw CodeThorn::Exception("Error: unknown color code.");
}

string CodeThorn::readableruntime(double timeInMilliSeconds) {
  stringstream s;
  double time=timeInMilliSeconds;
  s << std::fixed << std::setprecision(2); // 2 digits past decimal point.
  if(time<1000.0) {
    s<<time<<" ms";
    return s.str();
  } else {
    time=time/1000;
  }
  if(time<60) {
    s<<time<<" secs"; 
    return s.str();
  } else {
    time=time/60;
  }
  if(time<60) {
    s<<time<<" mins"; 
    return s.str();
  } else {
    time=time/60;
  }
  if(time<24) {
    s<<time<<" hours"; 
    return s.str();
  } else {
    time=time/24;
  }
  if(time<31) {
    s<<time<<" days"; 
    return s.str();
  } else {
    time=time/(((double)(365*3+366))/12*4);
  }
  s<<time<<" months"; 
  return s.str();
}

long CodeThorn::getPhysicalMemorySize() {
  long physicalMemoryUsedUnix = -1;
#if defined(__unix__) || defined(__unix) || defined(unix)
  long residentSetSize = -1;
  FILE* statm = NULL;
  if ((statm = fopen( "/proc/self/statm", "r" )) != NULL) {
    if (fscanf( statm, "%*s%ld", &residentSetSize ) == 1) {
      physicalMemoryUsedUnix = residentSetSize * sysconf(_SC_PAGESIZE);
    }
  }
  fclose(statm);
#endif
  if (physicalMemoryUsedUnix == -1) {
    cerr << "ERROR: Physical memory consumption could not be determined." << endl;
    ROSE_ASSERT(0);
  }
  return physicalMemoryUsedUnix;
}

