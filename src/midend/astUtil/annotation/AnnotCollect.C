

#include "AnnotCollect.h"
#include "CommandOptions.h"

using namespace std;

ReadAnnotation* ReadAnnotation::inst = 0;

ReadAnnotation* ReadAnnotation::get_inst()
{
  if (inst == 0)
     inst = new ReadAnnotation();
  return inst;
}
//! Process command line and call read(istream & in)
void ReadAnnotation::read()
    {
       const vector<string>& opts = CmdOptions::GetInstance()->opts;
       for (size_t i = 0; i < opts.size(); ++i) {
         if (opts[i] == "-annot") {
           assert (i + 1 < opts.size());
           string p1 = opts[i + 1];
           ifstream is(p1.c_str());
           read(is);
           ++i;
         }
       }
    }
//! Read the annotation file and recognize type and operator annotation collections    
void ReadAnnotation:: read( istream & in)
{
  while (in.good()) {
    string buf = read_id(in);
    
    if (buf == "class") {
       typeInfo.read(in);
    }
    else if (buf == "operator") {
        opInfo.read(in);
    }
    else if (!in.good())
      break;
    else if (in.good()) {
      ReadError m("non-recognizable annotation: " + buf);
      cerr << m.msg << endl;
      throw m;
    }
  }
}

