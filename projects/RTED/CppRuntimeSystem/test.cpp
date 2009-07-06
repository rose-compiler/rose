#include "CppRuntimeSystem.h"
#include <iostream>
#include <cassert>

using namespace std;

ostream & out = cout;


#define TEST_INIT( MSG)                               \
    out << "-- " << (MSG) << endl ;                   \
    bool errorFound=false;                            \
    RuntimeSystem * rs = RuntimeSystem::instance();   \
    rs->checkpoint(SourcePosition(MSG));              \


#define TEST_CATCH( TYPE)                                          \
    catch (RuntimeViolation & e)  {                                \
        if( e.getType() != ( TYPE) ) {                             \
            out << "Wrong error detected:"<<e.getType() << endl;   \
            exit(1);                                               \
        }                                                          \
        errorFound=true;                                           \
    }                                                              \
    if(!errorFound)                                                \
    {                                                              \
      out << "Failed to detect error" << endl;                     \
      exit(1);                                                     \
    }                                                              \
    errorFound=false;                                              \



#define CLEANUP  { rs->doProgramExitChecks();  rs->clearStatus(); }


// -------------------------------------- Memory Checker ------------------------------------------

void testSuccessfulMallocFree()
{
    TEST_INIT("Testing Successful Malloc Free");

    rs->createMemory(42,10);

    rs->log() << "After creation" << endl;
    rs->printMemStatus();
    rs->freeMemory(42);

    rs->log() << "After free" << endl;
    rs->printMemStatus();

    CLEANUP
}

void testFreeInsideBlock()
{
    TEST_INIT("Testing Invalid Free in allocated Block");

    rs->createMemory(42,10);

    try  {  rs->freeMemory(44); }
    TEST_CATCH(RuntimeViolation::INVALID_FREE);

    //free the right one
    rs->freeMemory(42);

    CLEANUP


}

void testInvalidFree()
{
    TEST_INIT("Testing invalid Free outside allocated Block");

    rs->createMemory(42,10);

    try  {  rs->freeMemory(500);   }
    TEST_CATCH(RuntimeViolation::INVALID_FREE)

    //free the right one
    rs->freeMemory(42);

    CLEANUP
}

void testDoubleFree()
{
    TEST_INIT("Testing Double Free");

    rs->createMemory(42,10);
    rs->freeMemory(42);

    try  {  rs->freeMemory(42);  }
    TEST_CATCH(RuntimeViolation::INVALID_FREE)

    CLEANUP
}

void testDoubleAllocation()
{
    TEST_INIT("Testing Double Allocation");


    rs->createMemory(42,10);

    try{  rs->createMemory(45,10);  }
    TEST_CATCH(RuntimeViolation::DOUBLE_ALLOCATION)


    rs->freeMemory(42);

    CLEANUP
}


void testMemoryLeaks()
{
    TEST_INIT("Testing detection of memory leaks");

    rs->createMemory(42,10);
    rs->createMemory(60,10);
    rs->createMemory(0,10);

    rs->freeMemory(60);

    try{ rs->doProgramExitChecks(); }
    TEST_CATCH(RuntimeViolation::MEMORY_LEAK)

    rs->freeMemory(0);
    rs->freeMemory(42);

    CLEANUP
}

void testEmptyAllocation()
{
    TEST_INIT("Testing detection of empty allocation");

    try { rs->createMemory(12,0); }
    TEST_CATCH(RuntimeViolation::EMPTY_ALLOCATION)

    CLEANUP
}


void testMemAccess()
{
    TEST_INIT("Testing memory access checks");

    rs->createMemory(0,10);

    rs->checkMemWrite(9,1);
    try { rs->checkMemWrite(9,2); }
    TEST_CATCH(RuntimeViolation::INVALID_WRITE)

    try { rs->checkMemRead(0,4); }
    TEST_CATCH(RuntimeViolation::INVALID_READ)

    rs->checkMemWrite(0,5);
    rs->checkMemRead(3,2);


    rs->freeMemory(0);


    CLEANUP

}





// -------------------------------------- Stack and Variables Tests ------------------------------------------


/*
void testStack()
{
    addr_type addr=0;

    RuntimeSystem * rs = RuntimeSystem::instance();
    rs->createVariable(addr+=4,"GlobalVar1","MangledGlobal1","SgInt");
    rs->createVariable(addr+=4,"GlobalVar2","MangledGlobal2","SgDouble");

    cout << endl << endl << "After Globals" << endl;

    rs->printStack(cout);
    rs->printMemStatus(cout);

    rs->beginScope("Function1");
    rs->createVariable(addr+=4,"Function1Var1","Mangled","SgInt");
    rs->createVariable(addr+=4,"Fucntion1Var2","Mangled","SgDouble");

    rs->checkMemWrite(2348080,4);
    rs->checkMemRead(addr-4,4);

    cout << endl << endl << "After Function1" << endl;
    rs->printStack(cout);
    rs->printMemStatus(cout);


    rs->endScope();
    cout << endl << endl << "After return of function" << endl;
    rs->printStack(cout);
    rs->printMemStatus(cout);
}
*/


// -------------------------------------- File Monitoring Tests ------------------------------------------


void testFileDoubleClose()
{
    TEST_INIT("Testing double file close");

    rs->registerFileOpen((FILE*)42,"MyFileName.txt",READ);
    rs->registerFileClose((FILE*)42);

    try { rs->registerFileClose((FILE*)42); }
    TEST_CATCH(RuntimeViolation::INVALID_FILE_CLOSE)

    CLEANUP
}


void testFileDoubleOpen()
{
    TEST_INIT("Testing double file open");

    rs->registerFileOpen((FILE*)42,"MyFileName.txt",READ);
    try {  rs->registerFileOpen((FILE*)42,"Other.txt",READ); }
    TEST_CATCH(RuntimeViolation::DOUBLE_FILE_OPEN)

    rs->registerFileClose((FILE*)42);

    CLEANUP
}

void testFileInvalidClose()
{
    TEST_INIT("Testing invalid file close");

    rs->registerFileOpen((FILE*)42,"MyFileName.txt",READ);
    try {  rs->registerFileClose((FILE*)43); }
    TEST_CATCH(RuntimeViolation::INVALID_FILE_CLOSE)


    rs->registerFileClose((FILE*)42);

    CLEANUP
}

void testFileUnclosed()
{
    TEST_INIT("Testing detection of unclosed files");

    rs->registerFileOpen((FILE*)42,"MyFileName.txt",READ);
    try {  rs->doProgramExitChecks(); }
    TEST_CATCH(RuntimeViolation::UNCLOSED_FILES)

    rs->registerFileClose((FILE*)42);

    CLEANUP
}


int main(int argc, char ** argv)
{
    RuntimeSystem * rs = RuntimeSystem::instance();
    rs->setOutputFile("test_output.txt");

    testSuccessfulMallocFree();
    testFreeInsideBlock();
    testInvalidFree();
    testDoubleFree();
    testDoubleAllocation();
    testMemoryLeaks();
    testEmptyAllocation();
    testMemAccess();

    testFileDoubleClose();
    testFileDoubleOpen();
    testFileInvalidClose();
    testFileUnclosed();

    return 0;
}
