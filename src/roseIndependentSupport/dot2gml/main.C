/* main.cc */

#include "parse_header.h"
#include <libgen.h>
#include <cstdlib>
using namespace std;

ofstream GML;

// prototype of bison-generated parser function
int yyparse();

int main (int argc, char **argv)
{
  if ((argc > 1) && (freopen(argv[1], "r", stdin) == NULL))
  {
    cerr << argv[0] << ": File " << argv[1] << " cannot be opened.\n";
    exit( 1 );
  }

  if (argc == 3)
    GML.open(argv[2]);
  else
    if (argc == 2)
      {
        std::ostringstream os;
        os << basename(argv[1]) << ".gml";
        GML.open(os.str().c_str());
      }
    else
    {
      cerr << "Usage: parser <input_file> [<output_file>]!";
      exit(1);
    }

  if (!GML)
  {
    cout << "Cannot open output file!";
    exit(1);
  }
  
  yyparse();
  GML.close();

  return 0;
}

