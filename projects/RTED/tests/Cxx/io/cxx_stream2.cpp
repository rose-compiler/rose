#include <fstream>

using namespace std;


int main() {

    fstream myfile;

    // Write to uninitialized fstream
    myfile << "Hejsan." << endl;

    // The RTS should abort prior to this
    return 0;
}
