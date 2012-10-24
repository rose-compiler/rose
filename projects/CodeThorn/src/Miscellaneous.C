/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "Miscellaneous.h"
#include "CommandLineOptions.h"

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

string CodeThorn::color(string name) {
  if(!boolOptions["colors"]) 
	return "";
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

