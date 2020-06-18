

#include "AnnotCollect.h"
#include "CommandOptions.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

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
       CmdOptions* inst = CmdOptions::GetInstance();
       std::vector<std::string>::const_iterator p = inst->begin();
       for ( ; p != inst->end(); p++) {
         if ( (*p)  == "-annot") {
           p++;
           string p1 = *p;
           ifstream is(p1.c_str());
           if (!is.is_open())
           {
             cerr<<"Error! Cannot find and open the annotation file "<<p1.c_str()<<endl;
             exit(1);
           }
           read(is);
         }
       }
    }
//! Read the annotation file and recognize type and operator annotation collections
void ReadAnnotation:: read( istream & in)
{
  while (in.good()) {
    peek_ch(in);
    string buf = read_id(in);

    if (buf == "class") {
       typeInfo.read(in);
    }
    else if (buf == "operator") {
        opInfo.read(in);
    }
    else if (buf == "variable") {
        varInfo.read(in);
    }
    else if (buf=="" || !in.good())
      break;
    else if (in.good()) {
      ReadError m("non-recognizable annotation: \"" + buf + "\"");
      cerr << m.msg << endl;
      throw m;
    }
  }
}

