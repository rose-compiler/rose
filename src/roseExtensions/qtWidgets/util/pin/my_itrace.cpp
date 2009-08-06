#include "pin.H"

#include <algorithm>
#include <map>
#include <ostream>
#include <iostream>
#include <fstream>

using namespace std;

// Map stores for each address how often it was executed
map<unsigned long long, unsigned long> instMap;

// This function is called before every instruction is executed
// and prints the IP
void addToMap (void *ip) 
{ 
    unsigned long long addr = reinterpret_cast<unsigned long long>(ip);
    instMap[addr]++;
}

// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)
{
    // Insert a call to printip before every instruction, and pass it the IP
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)addToMap, IARG_INST_PTR, IARG_END);
}

// This function is called when the application exits
void Fini(INT32 code, void *v)
{
    ofstream fStream ("itrace.pin");
    

    map<unsigned long long,unsigned long>::iterator it = instMap.begin();
    for(; it != instMap.end(); ++it)
        fStream << hex << it->first << "\t" << dec << it->second << endl;
    
}

// argc, argv are the entire command line, including pin -t <toolname> -- ...
int main(int argc, char * argv[])
{    
    // Initialize pin
    PIN_Init(argc, argv);

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
