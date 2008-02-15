#include <general.h>
#include <AnnotCollect.h>
#include <CommandOptions.h>
#include <string.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

ReadAnnotation* ReadAnnotation::inst = 0;

ReadAnnotation* ReadAnnotation::get_inst()
{
  if (inst == 0)
     inst = new ReadAnnotation();
  return inst;
}

void ReadAnnotation::read()
    {
       unsigned int p = CmdOptions::GetInstance()->HasOption("-annot");
       if (p == 0) return; // Option -annot not found
       --p; // Adjust for one-based indexing from HasOption
       const vector<string>& opts = CmdOptions::GetInstance()->GetOptions();
       while (p != opts.size()) {
	  ++p;
	  if (p == opts.size()) break;
          string p1 = opts[p];
	  {
	    ifstream f(p1.c_str());
	    read(f);
	  }
	  while (true) {
	    ++p;
	    if (p == opts.size() || opts[p] == "-annot") break;
	  }
       }
    }
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

