#include "CppRuntimeSystem.h"
#include <iostream>
#include <cassert>
#include <cstdlib>

using namespace std;

ostream & out = cout;


#define TEST_INIT( MSG)                               \
    out << "-- " << (MSG) << endl ;                   \
    bool errorFound=false;                            \
    RuntimeSystem * rs = RuntimeSystem::instance();   \
    rs->checkpoint(SourcePosition( (MSG), __LINE__, __LINE__ ));              \


#define TEST_CATCH( TYPE)                                          \
    catch (RuntimeViolation & e)  {                                \
        if( e.getType() != ( TYPE) ) {                             \
            out << "Wrong error detected: "                        \
                << e.getShortDesc() << " instead of "              \
                << RuntimeViolation::RuntimeViolation(TYPE)        \
                .getShortDesc()                                    \
                << " : " <<  __LINE__                              \
                << endl;                                           \
            exit(1);                                               \
        }                                                          \
        errorFound=true;                                           \
    }                                                              \
    if(!errorFound)                                                \
    {                                                              \
      out   << "Failed to detect error "                           \
            << RuntimeViolation::RuntimeViolation(TYPE)            \
                .getShortDesc()                                    \
                << " : " <<  __LINE__                              \
            << endl;                                               \
      exit(1);                                                     \
    }                                                              \
    errorFound=false;                                              \



#define CLEANUP  { rs->doProgramExitChecks();  rs->clearStatus(); }

// ------------------- cstdlib string test macros ----------------------------

// each test must define TRY_BODY, as a call to the method being tested, e.g.
//      rs->check_strlen(  (const char*) str)
#define TRY_BODY


#define DEST_SOURCE                                                 \
    char *dest = NULL, *source = NULL;
#define STR1_STR2                                                   \
    char *str1 = NULL, *str2 = NULL;
#define TEST_INIT_STRING( ... )                                     \
    size_t N = 9;                                                   \
    size_t SZ = (N + 1) * sizeof(char);                             \
    __VA_ARGS__;

// Tests that try body will complain if var isn't allocated or if it isn't
// initialized.
//
// As a side effect, this will also initialize var.
#define TEST_STRING_READ( var)                                      \
    /* haven't allocated var yet */                                 \
    try { TRY_BODY }                                                \
    TEST_CATCH( RuntimeViolation::INVALID_READ);                    \
                                                                    \
    var = (char*) malloc( SZ);                                      \
    strcpy( var, "         ");                                      \
    var[ N] = ' ';                                                  \
    rs->createMemory( (addr_type) var, SZ);                         \
                                                                    \
    /* haven't initialized var yet */                               \
    try { TRY_BODY }                                                \
    TEST_CATCH( RuntimeViolation::INVALID_READ);                    \
                                                                    \
    rs->checkMemWrite( (addr_type) var, SZ);                        

// Does TEST_STRING_READ, but also checks that try body complains if var doesn't
// have a null terminator in allocated memory.
#define TEST_STRING_READ_TERMINATED( var)                           \
    TEST_STRING_READ( var )                                         \
                                                                    \
    /* no null-terminator in var */                                 \
    try { TRY_BODY }                                                \
    TEST_CATCH( RuntimeViolation::INVALID_READ);                    \
    var[ N] = '\0';

#define TEST_STRING_WRITE( var, badsz, oksz)                        \
    /* haven't allocated var yet */                                 \
    try { TRY_BODY }                                                \
    TEST_CATCH( RuntimeViolation::INVALID_WRITE);                   \
    var = (char*) malloc( badsz );                                  \
    rs->createMemory( (addr_type) var, badsz );                     \
    /* var isn't large enough */                                    \
    try { TRY_BODY }                                                \
    TEST_CATCH( RuntimeViolation::INVALID_WRITE);                   \
                                                                    \
    rs->freeMemory( (addr_type) var);                               \
    var = (char*) realloc( var, oksz );                             \
    rs->createMemory( (addr_type) var, oksz );                      

#define TEST_STRING_CAT( var, oksz)                                 \
    /* haven't allocated var yet */                                 \
    try { TRY_BODY }                                                \
    TEST_CATCH( RuntimeViolation::INVALID_READ);                    \
    var = (char*) malloc( oksz );                                   \
    rs->createMemory( (addr_type) var, oksz );                      \
    var[ 0 ] = ' ';                                                 \
    var[ 1 ] = '\0';                                                \
    rs->checkMemWrite( (addr_type) var, 2);                         \
    /* var is large enough to hold source, but not large enough */  \
    /* for source to be appended */                                 \
    try { TRY_BODY }                                                \
    TEST_CATCH( RuntimeViolation::INVALID_WRITE);                   \
    var[ 0 ] = '\0';

// Tests that try body complains if either str1 or str2 lacks a null terminator
// in allocated memory, or if either isn't allocated yet.
//
// Tests that try body does not complain when both of these conditions are
// satisfied.
#define TEST_STR1_STR2                                              \
    TEST_INIT_STRING( STR1_STR2)                                    \
                                                                    \
    TEST_STRING_READ_TERMINATED( str1)                              \
    TEST_STRING_READ_TERMINATED( str2)                              \
    TRY_BODY                                                        \
    TEST_STRING_CLEANUP( str1)                                      \
    TEST_STRING_CLEANUP( str2)                                      \
                                                                    \
    TEST_STRING_READ_TERMINATED( str2)                              \
    TEST_STRING_READ_TERMINATED( str1)                              \
    TRY_BODY                                                        \
    TEST_STRING_CLEANUP( str2)                                      \
    TEST_STRING_CLEANUP( str1)                                      

#define TEST_STRING_CLEANUP( var)                                   \
    free( var );                                                    \
    rs->freeMemory( (addr_type) var );                              \
    var = NULL;

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

void testInvalidStackFree()
{
    TEST_INIT("Testing invalid Free outside allocated Block");

    // freeing heap memory should be fine
    rs->createMemory(42,10);
    rs->freeMemory(42);

    // but freeing stack memory is not okay
    rs->createMemory(42,10,true);
    try  {  rs->freeMemory(42);   }
    TEST_CATCH(RuntimeViolation::INVALID_FREE)

    // test cleanup
    rs->freeMemory(42,true);

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

void testFileInvalidAccess()
{
    TEST_INIT("Testing invalid file-access");


    try{ rs->registerFileOpen(NULL,"name",READ ); }
    TEST_CATCH(RuntimeViolation::INVALID_FILE_OPEN)


    FILE * fh = (FILE*)42;
    // some allowed operations
    rs->registerFileOpen(fh,"MyFileName.txt",READ | WRITE);
    rs->checkFileAccess(fh,true);
    rs->checkFileAccess(fh,false);
    rs->registerFileClose(fh);


    // check if illegal write is detected
    rs->registerFileOpen(fh,"MyFileName.txt",READ);
    rs->checkFileAccess(fh,true);
    try {  rs->checkFileAccess(fh,false); } //invalid write
    TEST_CATCH(RuntimeViolation::INVALID_FILE_ACCESS)
    rs->registerFileClose(fh);

    // check if illegal read is detected
    rs->registerFileOpen(fh,"MyFileName.txt",WRITE);
    rs->checkFileAccess(fh,false);
    try {  rs->checkFileAccess(fh,true); } //invalid read
    TEST_CATCH(RuntimeViolation::INVALID_FILE_ACCESS)
    rs->registerFileClose(fh);

    try {  rs->checkFileAccess((FILE*)43,true); }
    TEST_CATCH(RuntimeViolation::INVALID_FILE_ACCESS)

    CLEANUP
}

void testScopeFreesStack()
{
    TEST_INIT("Testing that exiting a scope frees stack variables")

    rs->beginScope("main");
    rs->createVariable(
        (addr_type) 4,
        "my_var",
        "mangled_my_var",
        "SgIntVal",
        sizeof( int)
    );
    rs->endScope();

    CLEANUP
}

// Tests that an implicit scope exists, i.e. main's scope.  Calling
// createVariable without ever calling beginScope or endScope should not result
// in memory errors.
void testImplicitScope()
{
    TEST_INIT("Testing that an implicit scope exists for globals/main")

    rs->createVariable(
        (addr_type) 4,
        "my_var",
        "mangled_my_var",
        "SgIntVal",
        sizeof( int)
    );

    CLEANUP
}


// -------------------------------------- Pointer Tracking Tests ------------------------------------------

void testLostMemRegion()
{
    TEST_INIT("Testing detection of lost mem-regions");
    rs->createMemory(10,5);
    rs->createMemory(20,5);


    addr_type addr=100;
    rs->beginScope("Scope1");
        rs->createVariable(addr+=4,"p1_to_10","mangled","SgPointerType",4);
        rs->createPointer("p1_to_10",10);

        rs->createVariable(addr+=4,"p1_to_20","mangled","SgPointerType",4);
        rs->createPointer("p1_to_20",20);


        rs->beginScope("Scope2");
            rs->createVariable(addr+=4,"p2_to_10","mangled","SgPointerType",4);
            rs->createPointer("p2_to_10",10);
        rs->endScope();

        try{ rs->createPointer("p1_to_10",NULL); }
        TEST_CATCH(RuntimeViolation::MEM_WITHOUT_POINTER)

        try{ rs->createPointer("p1_to_20",NULL); }
        TEST_CATCH(RuntimeViolation::MEM_WITHOUT_POINTER)

    rs->endScope();

    rs->freeMemory(10);
    rs->freeMemory(20);

    CLEANUP
}

void testPointerChanged()
{
    TEST_INIT("Testing detection of lost mem-regions");

    CLEANUP
}

void testInvalidPointerAssign()
{
    TEST_INIT("Testing Invalid Pointer assign");

    CLEANUP
}


// -------------------------------------- CStdLib Tests ------------------------------------------

void test_memcpy()
{
    TEST_INIT("Testing calls to memcpy");

    addr_type address = 0;

    rs->createMemory( address += 4, 16);
    addr_type ptr1 = address;

    rs->createMemory( address += 16, 16);
    rs->checkMemWrite( address, 16);
    addr_type ptr2 = address;


    // 4..20 doesn't overlap 20..36
    rs->check_memcpy( (void*) ptr1, (void*) ptr2, 16);

    // but 4..20 overlaps 16..32
    try { rs->check_memcpy( (void*) ptr1, (void*)(ptr2 - 4), 16);}
    TEST_CATCH( RuntimeViolation::INVALID_MEM_OVERLAP)

    rs->freeMemory( ptr1);
    rs->freeMemory( ptr2);

    CLEANUP
}

void test_memmove()
{
    TEST_INIT("Testing calls to memmove");
    addr_type address = 0;

    rs->createMemory( address += 4, 16);
    addr_type ptr1 = address;

    rs->createMemory( address += 16, 16);
    addr_type ptr2 = address;


    try { rs->check_memmove( (void*) ptr1, (void*)(ptr2 - 4), 16);}
    TEST_CATCH( RuntimeViolation::INVALID_READ)

    rs->checkMemWrite( address, 16);
    rs->check_memmove( (void*) ptr1, (void*) ptr2, 16);

    rs->freeMemory( ptr1);
    rs->freeMemory( ptr2);

    CLEANUP
}

void test_strcpy()
{
    TEST_INIT("Testing calls to strcpy");

    #undef TRY_BODY
    #define TRY_BODY rs->check_strcpy(  dest, (const char*) source);
    TEST_INIT_STRING( DEST_SOURCE)
    TEST_STRING_READ_TERMINATED( source)
    TEST_STRING_WRITE( dest, SZ / 2, SZ)

    // also, it's not legal for the strings to overlap
    try { rs->check_strcpy( source + (N/2), source); }
    TEST_CATCH( RuntimeViolation::INVALID_MEM_OVERLAP);

    TRY_BODY
    TEST_STRING_CLEANUP( dest)
    TEST_STRING_CLEANUP( source)

    CLEANUP
}

void test_strncpy()
{
    TEST_INIT("Testing calls to strncpy");

    #undef TRY_BODY
    #define TRY_BODY rs->check_strncpy(  dest, (const char*) source, N / 2);
    TEST_INIT_STRING( DEST_SOURCE)
    TEST_STRING_READ( source)
    TEST_STRING_WRITE( dest, SZ / 4, SZ / 2)

    // also it's not legal for the strings to overlap
    try { rs->check_strncpy(  source + (N/2),  source, N); }
    TEST_CATCH( RuntimeViolation::INVALID_MEM_OVERLAP);

    TRY_BODY
    TEST_STRING_CLEANUP( dest)
    TEST_STRING_CLEANUP( source)

    CLEANUP
}

void test_strcat()
{
    TEST_INIT("Testing calls to strcat");

    #undef TRY_BODY
    #define TRY_BODY rs->check_strcat(  dest, (const char*) source);
    TEST_INIT_STRING( DEST_SOURCE)
    TEST_STRING_READ_TERMINATED( source)
    TEST_STRING_READ_TERMINATED( dest)
    TEST_STRING_CLEANUP( dest)
    TEST_STRING_CAT( dest, SZ)

    TRY_BODY
    TEST_STRING_CLEANUP( dest)
    TEST_STRING_CLEANUP( source)

    CLEANUP
}

void test_strncat()
{
    TEST_INIT("Testing calls to strncat");

    #undef TRY_BODY
    #define TRY_BODY rs->check_strncat(  dest, (const char*) source, N / 2);
    TEST_INIT_STRING( DEST_SOURCE)
    TEST_STRING_READ( source)
    TEST_STRING_READ( dest)
    TEST_STRING_CLEANUP( dest)
    TEST_STRING_CAT( dest, SZ / 2)

    TRY_BODY
    TEST_STRING_CLEANUP( dest)
    TEST_STRING_CLEANUP( source)

    CLEANUP
}

void test_strchr()
{
    TEST_INIT("Testing calls to strchr");

    #undef TRY_BODY
    #define TRY_BODY rs->check_strchr(  (const char*) str, 'x');
    TEST_INIT_STRING( char *str = NULL)
    TEST_STRING_READ_TERMINATED( str)

    TRY_BODY
    TEST_STRING_CLEANUP( str)

    CLEANUP
}

void test_strpbrk()
{
    TEST_INIT("Testing calls to strpbrk");

    #undef TRY_BODY
    #define TRY_BODY rs->check_strpbrk(  (const char*) str1, (const char*) str2);
    TEST_STR1_STR2

    CLEANUP
}

void test_strspn()
{
    TEST_INIT("Testing calls to strspn");
    
    #undef TRY_BODY
    #define TRY_BODY rs->check_strspn(  (const char*) str1, (const char*) str2);
    TEST_STR1_STR2

    CLEANUP
}

void test_strstr()
{
    TEST_INIT("Testing calls to strstr");

    #undef TRY_BODY
    #define TRY_BODY rs->check_strstr(  (const char*) str1, (const char*) str2);
    TEST_STR1_STR2

    CLEANUP
}

void test_strlen()
{
    TEST_INIT("Testing calls to strlen");

    #undef TRY_BODY
    #define TRY_BODY rs->check_strlen(  (const char*) str);
    TEST_INIT_STRING( char* str = NULL)
    TEST_STRING_READ_TERMINATED( str)

    TRY_BODY
    TEST_STRING_CLEANUP( str)

    CLEANUP
}

void test_meminit_nullterm_included()
{
    TEST_INIT(  "Testing that strcpy et. al set the full destination "
                "initialized, including the null terminator");

    size_t n = 9; // sizeof("a string") + 1 for \0
    char s1[ 9 ];
    char s2[ 9 ] = "a string";
    char s3[ 9 ];

    rs->createMemory( (addr_type) s1, n);
    rs->createMemory( (addr_type) s2, n);
    rs->createMemory( (addr_type) s3, n);

    rs->checkMemWrite( (addr_type) s2, n);

    rs->check_strcpy( (char*) s1, (const char*) s2);
    strcpy( s1, s2);
    rs->check_strcpy( (char*) s3, (const char*) s1);

    rs->freeMemory( (addr_type) s1);
    rs->freeMemory( (addr_type) s2);
    rs->freeMemory( (addr_type) s3);
    CLEANUP
}

void test_range_overlap()
{
    TEST_INIT("Testing that overlap checks cover ranges")

    char* s1;
    char s2[ 20 ];
    char s3[ 9 ] = "a string";


    rs->createMemory( (addr_type) s2, sizeof(s2));
    rs->createMemory( (addr_type) s3, sizeof(s3));
    rs->checkMemWrite( (addr_type) s3, sizeof(s3));
    rs->check_strcpy( s2, s3);

    strcpy( s2, s3);
    s1 = &s2[ 3 ];

    try { rs->check_strcat( s1, s2);}
    TEST_CATCH( RuntimeViolation::INVALID_MEM_OVERLAP)

    rs->check_strcpy( &s2[ 10 ], s3);
    strcpy( &s2[ 10 ], s3);

    // this should be fine -- the cat doesn't reach s2[ 10 ]
    s2[ 0 ] = '\0';
    s1 = &s2[0];
    rs->check_strcat( s1, &s2[ 10 ]);

    s1[ 0 ] = ' ';
    s1[ 3 ] = '\0';

    // now the cat will reach and an overlap occurs
    try { rs->check_strcat( s1, &s2[ 10 ]);}
    TEST_CATCH( RuntimeViolation::INVALID_MEM_OVERLAP)


    rs->freeMemory( (addr_type) s2);
    rs->freeMemory( (addr_type) s3);

    CLEANUP
}



void testTypeSystem()
{
    TEST_INIT("Testing TypeSystem: nested type detection")

    TypeSystem * ts = rs->getTypeSystem();

    /*
     * class A { int a1; char a2, double a3; }
     * class B { A a[10]; char b1; int b2;
     */
    RsClassType * typeA = new RsClassType("A",16);
    typeA->addMember("a1",ts->getTypeInfo("SgTypeInt"),0);
    typeA->addMember("a2",ts->getTypeInfo("SgTypeChar"),4);
    typeA->addMember("a3",ts->getTypeInfo("SgTypeDouble"),8);

    ts->registerType(typeA);

    RsClassType * typeB = new RsClassType("B",10*16 + 8);

    typeB->addMember("a",ts->getArrayType("A",10),0);
    typeB->addMember("b1",ts->getTypeInfo("SgTypeChar"),10*16    );
    typeB->addMember("b2",ts->getTypeInfo("SgTypeInt"), 10*16 + 4);

    ts->registerType(typeB);


    rs->createMemory(42,10000);
    MemoryType * mt = rs->getMemManager()->getMemoryType(42);
    mt->setTypeInfo(0,ts->getTypeInfo("A"));
    mt->setTypeInfo(16,ts->getTypeInfo("B"));
    mt->setTypeInfo(32+4+4,ts->getTypeInfo("SgTypeDouble"));

    //Access to padded area
    try {mt->setTypeInfo(32+4+1,ts->getTypeInfo("SgTypeChar")); }
    TEST_CATCH( RuntimeViolation::INVALID_TYPE_ACCESS)

    //Wrong type
    try {mt->setTypeInfo(0,ts->getTypeInfo("B")); }
    TEST_CATCH( RuntimeViolation::INVALID_TYPE_ACCESS)

    //Wrong basic type
    mt->setTypeInfo(32+4,ts->getTypeInfo("SgTypeChar"));
    try {mt->setTypeInfo(32+4,ts->getTypeInfo("SgTypeInt")); }
    TEST_CATCH( RuntimeViolation::INVALID_TYPE_ACCESS)

    rs->log() << "Type System Status after test" << endl;
    ts->print(rs->log());

    rs->freeMemory(42);

    CLEANUP
}



int main(int argc, char ** argv)
{
    try
    {
        RuntimeSystem * rs = RuntimeSystem::instance();
        rs->setTestingMode(true);
        rs->setOutputFile("test_output.txt");

        testTypeSystem();

        testSuccessfulMallocFree();

        testFreeInsideBlock();
        testInvalidFree();
        testInvalidStackFree();
        testDoubleFree();
        testDoubleAllocation();
        testMemoryLeaks();
        testEmptyAllocation();
        testMemAccess();

        testFileDoubleClose();
        testFileDoubleOpen();
        testFileInvalidClose();
        testFileUnclosed();
        testFileInvalidAccess();

        testScopeFreesStack();
        testImplicitScope();

        //testLostMemRegion();
        //testPointerChanged();
        //testInvalidPointerAssign();

        test_memcpy();
        test_memmove();
        test_strcpy();
        test_strncpy();
        test_strcat();
        test_strncat();
        test_strchr();
        test_strpbrk();
        test_strspn();
        test_strstr();
        test_strlen();

        test_meminit_nullterm_included();
        test_range_overlap();
    }
    catch( RuntimeViolation& e)
    {
        out << "Unexpected Error: " << endl << e;
        exit( 1);
    }

    return 0;
}

// vim:sw=4 ts=4 tw=80 et sta fdm=marker:
