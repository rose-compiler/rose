#include "AnnotIO.h"
#include "CommandOptions.h"
#include <string.h>

using namespace std;

namespace annotation{
static string buffer = "";
static unsigned index = 0;
};

bool DebugAnnotRead()
{
  static int debugread = 0;
  if (debugread == 0) {
    if ( CmdOptions::GetInstance()->HasOption("-debugannotread"))
        debugread = 1;
     else
         debugread = -1;
  }
  return debugread == 1;
}

bool DebugAnnot()
{
  static int debug = 0;
  if (debug == 0) {
    if ( CmdOptions::GetInstance()->HasOption("-debugannot"))
        debug = 1;
     else
         debug = -1;
  }
  return debug == 1;
}
bool is_space( char c)
{
  return c == ' ' || c == '\t' || c == '\n';
}

bool is_num( char c)
{
  return c >= '0' && c <= '9';
}

bool is_id( char c)
{
  return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_operator( char c)
{
  string op = "<>+-*/&.$";
  char *p = strchr(op.c_str(), c);
  if (p == 0)
      return 0;
  return p - op.c_str() + 1;
}

char getch( istream& in)
 { 
    if (annotation::buffer != "") {
       char c = annotation::buffer[annotation::index++];
       if (annotation::index >= annotation::buffer.size()) {
           annotation::buffer = "";
           annotation::index = 0;
       }
       return c;
    }
    else {
      char c = in.get(); 
      if (DebugAnnotRead())
         cerr << c;
      return c;
    }
 }

char peek_ch(istream& in)
{
      char c = peek_next(in); 
      while (is_space(c)) {
        getch(in);
        c = peek_next(in);
      }
      if (in.eof() || c == EOF)
        getch(in);
      return c;
}

char peek_next(istream& in)
{
   if (annotation::buffer != "") 
      return annotation::buffer[annotation::index];
    else 
      return in.peek();
}

string peek_next(istream& in, unsigned size)
{
   unsigned bufsize = annotation::buffer.size();
   for ( ; bufsize < annotation::index + size; ++bufsize) {
      char c = in.get();
      annotation::buffer.push_back(c);
      if (DebugAnnotRead())
         cerr << c;
   }
   return annotation::buffer.substr(annotation::index,size);
}

char read_ch( istream& in )
{
  char c = 0;
  do { c = getch(in); } while (in.good() && is_space(c));
  if (!in.good())
      return 0;
  return c;
}

void read_ch( istream& in, char c)
{
  char c1 = read_ch(in);
  if (c1 != c) {
     ReadError m("read char error: expecting '");
     m.msg.push_back(c);
     m.msg = m.msg + "' instead of '";
     m.msg.push_back(c1);
     m.msg.push_back('\'');
     throw m;
  }
}

string read_id( istream& in )
{
  char c = peek_ch(in);
  string buf = "";
  while (in.good() && (is_id(c) || is_num(c)) ) {
    c = getch(in);
    buf.push_back(c);
    c = peek_next(in);
  }
  return buf;
}

void read_id( istream& in, const string& s)
{
  string r = read_id(in);
  if (r != s) 
     throw ReadError("read identifier error: expecting '" + s + "' instead of '" + r + "'");
}

string peek_id( istream& in)
{
  if (annotation::buffer == "") {
     annotation::buffer = read_id(in);
     annotation::index = 0; 
  } 
  return (annotation::index == 0)? annotation::buffer 
            : annotation::buffer.substr(annotation::index, 
                                        annotation::buffer.size() - annotation::index);
}

string read_num( istream& in )
{
  char c = read_ch(in);
  if (!in.good())
      return "";
  if (!is_num(c)) 
    throw ReadError("read number error: expecting numerics instead of " + c );  
  string buf = "";
  buf.push_back(c);
  for (;;) {
    c = peek_next(in);
    if ( !is_num(c) && c != '.') 
      break;
    getch(in);
    buf.push_back(c);
  }
  return buf;
}

string read_operator( istream& in )
{
  char c = peek_ch(in);
  string buf = "";
  while (in.good() && is_operator(c)) {
    read_ch(in,c);
    buf.push_back(c);
    c = peek_next(in);
  }
  return buf;
}

