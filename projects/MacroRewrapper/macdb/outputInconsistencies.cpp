#include "macro.h"
#include <iostream>

using namespace std;

int main (int argc, char *argv[]) {
    if ( argc>2 ) {
        fprintf(stderr, "Usage: %s <database name>\n", argv[0]);
        return 1;
    }

    int dberrcode = 0;
    if ( argc==1 )
        dberrcode = init_macdb(MACDB_NAME);
    else
        dberrcode = init_macdb(argv[1]);
    if ( dberrcode!= 0 ) {
        cerr << "Can't open db '%s'" << (argc==1 ? MACDB_NAME : argv[1]) << endl;
        return 2;
    }
//  cout << "exist? " << exist_macro_def("hello", 40) << endl;
//  vector<string> x;
//  cout << "return code: " << add_macro_def("/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/ROSE/boost-January-2b/projects/MacroRewrapper/test12.C", 1, 9, "temp", "tetsss", x) << endl;

    // output all inconsistencies:
    iterate_macro_defs_calls();
    close_macdb();

  return 0;
}

