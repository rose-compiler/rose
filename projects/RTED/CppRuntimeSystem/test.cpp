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
    rs->createStackMemory(42,sizeof(int),"SgTypeInt");
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
        "SgTypeInt"
    );
    rs->endScope();

    CLEANUP
}

// Tests that an implicit scope exists, i.e. main's scope.  Calling
// createVariable without ever calling beginScope or endScope should not result
// in memory errors.addr_type
void testImplicitScope()
{
    TEST_INIT("Testing that an implicit scope exists for globals/main")

    rs->createVariable(
        (addr_type) 4,
        "my_var",
        "mangled_my_var",
        "SgTypeInt"
    );

    CLEANUP
}


// -------------------------------------- Pointer Tracking Tests ------------------------------------------

void testLostMemRegion()
{
    //
    TEST_INIT("Testing detection of lost mem-regions");
    rs->createMemory(10,2*sizeof(int));
    rs->createMemory(18,2*sizeof(int));

    addr_type addr=100;
    int ptrSize = sizeof(void*);
    rs->beginScope("Scope1");
        rs->createVariable(addr+=ptrSize,"p1_to_10","mangled_p1_to_10","SgPointerType","SgTypeInt");
        rs->registerPointerChange("mangled_p1_to_10",10);

        rs->createVariable(addr+=ptrSize,"p1_to_18","mangled_p1_to_18","SgPointerType","SgTypeInt");
        rs->registerPointerChange("mangled_p1_to_18",18);


        rs->beginScope("Scope2");
            rs->createVariable(addr+=ptrSize,"p2_to_10","mangled_p2_to_10","SgPointerType","SgTypeInt");
            rs->registerPointerChange("mangled_p2_to_10",10);
        rs->endScope();

        /*
        rs->freeMemory(10);
        rs->freeMemory(18);
        rs->registerPointerChange("p1_to_10",NULL);
        rs->registerPointerChange("p1_to_20",NULL);
        */
        try{ rs->registerPointerChange("mangled_p1_to_10",NULL); }
        TEST_CATCH(RuntimeViolation::MEM_WITHOUT_POINTER)

        try{ rs->registerPointerChange("mangled_p1_to_18",NULL); }
        TEST_CATCH(RuntimeViolation::MEM_WITHOUT_POINTER)

    rs->endScope();

    rs->freeMemory(10);
    rs->freeMemory(18);

    CLEANUP
}

void testPointerChanged()
{
    TEST_INIT("Pointer Tracking test: Pointer changes chunks");

    rs->createMemory(10,2*sizeof(int));
    rs->createMemory(18,2*sizeof(int));


    // Case1: change of allocation chunk
    addr_type addr=100;
    int ptrSize = sizeof(void*);
    rs->beginScope("Scope1");
        rs->createVariable(addr+=ptrSize,"p1_to_10","mangled_p1_to_10","SgPointerType","SgTypeInt");
        rs->registerPointerChange("mangled_p1_to_10",10);

        rs->createVariable(addr+=ptrSize,"p2_to_10","mangled_p2_to_10","SgPointerType","SgTypeInt");
        rs->registerPointerChange("mangled_p2_to_10",10);

        rs->createVariable(addr+=ptrSize,"p1_to_18","mangled_p1_to_18","SgPointerType","SgTypeInt");
        rs->registerPointerChange("mangled_p1_to_18",18);

        try{ rs->registerPointerChange("mangled_p1_to_10",18,true); }
        TEST_CATCH(RuntimeViolation::POINTER_CHANGED_MEMAREA )

        rs->registerPointerChange("mangled_p1_to_18",18+sizeof(int));

        rs->freeMemory(10);
        rs->freeMemory(18);
    rs->endScope();


    // Case2: change of "type-chunk"
    rs->beginScope("Scope2");
        struct A { int arr[10]; int behindArr; };
        TypeSystem * ts = rs->getTypeSystem();
        RsClassType * typeA = new RsClassType("A",sizeof(A));
        typeA->addMember("arr",ts->getArrayType("SgTypeInt",10), offsetof(A,arr));
        typeA->addMember("behindArr",ts->getTypeInfo("SgTypeInt"),offsetof(A,behindArr));
        assert(typeA->isComplete());
        ts->registerType(typeA);

        // Create an instance of A on stack
        rs->createVariable(0x42,"instanceOfA","mangled","A");

        rs->createVariable(0x100,"intPtr","mangled_intPtr","SgPointerType","SgTypeInt");
        rs->registerPointerChange("mangled_intPtr",0x42);

        try{ rs->registerPointerChange("mangled_intPtr",0x42 + 10*sizeof(int),true); }
        TEST_CATCH(RuntimeViolation::POINTER_CHANGED_MEMAREA )

    rs->endScope();



    CLEANUP
}

void testInvalidPointerAssign()
{
    TEST_INIT("Testing Invalid Pointer assign");
    rs->beginScope("Scope2");
        // Create an instance of A on stack
        rs->createVariable(0x42,"instanceOfA","mangled","SgTypeDouble");
        rs->createVariable(0x100,"intPtr","mangled_intPtr","SgPointerType","SgTypeInt");
        // Try to access double with an int ptr
        try { rs->registerPointerChange("mangled_intPtr",0x42); }
        TEST_CATCH ( RuntimeViolation::INVALID_TYPE_ACCESS )

    rs->endScope();
    CLEANUP
}

void testPointerTracking()
{
    TEST_INIT("Testing Pointer tracking")
    TypeSystem * ts = rs->getTypeSystem();

    // class A { int arr[2]; int intBehindArr; }
    RsClassType * type = new RsClassType("A",3*sizeof(int));
    type->addMember("arr",ts->getArrayType("SgTypeInt",2));
    type->addMember("intBehindArr",ts->getTypeInfo("SgTypeInt"));
    ts->registerType(type);

    rs->beginScope("TestScope");
    rs->createVariable(42,"instanceOfA","mangled","A");

    rs->createVariable(100,"pointer","mangledPointer","SgPointerType","A");

    //rs->setQtDebuggerEnabled(true);
    //rs->checkpoint(SourcePosition());


    rs->endScope();

    CLEANUP
}


void testArrayAccess()
{
    TEST_INIT("Testing Heap Array")

    addr_type heapAddr =0x42;
    rs->createMemory(heapAddr,10*sizeof(int));
    rs->createMemory(heapAddr+10*sizeof(int),10); //allocate second chunk directly afterwards

    rs->beginScope("Scope");

    addr_type pointerAddr = 0x100;
    rs->createVariable(0x100,"intPointer","mangled_intPointer","SgPointerType","SgTypeInt");
    rs->registerPointerChange(pointerAddr,heapAddr,false);

    //simulate iteration over array
    for(int i=0; i<10 ; i++)
        rs->registerPointerChange(pointerAddr,heapAddr+ i*sizeof(int),true);


    // write in second allocation ( not allowed to changed mem-chunk)
    try { rs->registerPointerChange(pointerAddr,heapAddr+ 10*sizeof(int),true); }
    TEST_CATCH ( RuntimeViolation::POINTER_CHANGED_MEMAREA )

    // write in illegal mem region before
    try { rs->registerPointerChange(pointerAddr,heapAddr - sizeof(int),true); }
    TEST_CATCH ( RuntimeViolation::INVALID_PTR_ASSIGN )

    rs->freeMemory(heapAddr);
    rs->freeMemory(heapAddr+10*sizeof(int));

    rs->endScope();

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

void test_memcpy_strict_overlap()
{
    TEST_INIT(  "Testing that memcpy complains about overlap for allocated "
                "ranges, not merely for the copied ranges.")

    char* s1;
    char* s2 = "here is string 2";


    rs->createMemory( (addr_type) s2, 16 * sizeof( char ));
    rs->checkMemWrite( (addr_type) s2, 16 * sizeof( char ));

    s1 = &s2[ 7 ];

    // even though there's no overlap with the copied ranges, memcpy shouldn't
    // be called when the allocated blocks overlap
    try { rs->check_memcpy( s1, s2, 3);}
    TEST_CATCH( RuntimeViolation::INVALID_MEM_OVERLAP)

    rs->freeMemory( (addr_type) s2);

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



void testTypeSystemDetectNested()
{
    TEST_INIT("Testing TypeSystem: nested type detection")

    TypeSystem * ts = rs->getTypeSystem();

    // Register Struct A
    struct A { int a1; char a2; double a3; };
    RsClassType * typeA = new RsClassType("A",sizeof(A));
    typeA->addMember("a1",ts->getTypeInfo("SgTypeInt"),   offsetof(A,a1));
    typeA->addMember("a2",ts->getTypeInfo("SgTypeChar"),  offsetof(A,a2));
    typeA->addMember("a3",ts->getTypeInfo("SgTypeDouble"),offsetof(A,a3));
    assert(typeA->isComplete());
    ts->registerType(typeA);

    // Register Struct B
    struct B { A arr[10]; char b1; int b2; };
    RsClassType * typeB = new RsClassType("B",sizeof(B));
    typeB->addMember("arr",ts->getArrayType("A",10),    offsetof(B,arr));
    typeB->addMember("b1",ts->getTypeInfo("SgTypeChar"),offsetof(B,b1) );
    typeB->addMember("b2",ts->getTypeInfo("SgTypeInt"), offsetof(B,b2));
    assert(typeB->isComplete());
    ts->registerType(typeB);


    // Create Memory with first an A and then a B
    const addr_type ADDR = 42;
    rs->createMemory(ADDR,sizeof(A)+sizeof(B));
    MemoryType * mt = rs->getMemManager()->getMemoryType(ADDR);
    mt->accessMemWithType(0,ts->getTypeInfo("A"));
    mt->accessMemWithType(sizeof(A),ts->getTypeInfo("B"));
    mt->accessMemWithType(sizeof(A)+offsetof(A,a3),
                          ts->getTypeInfo("SgTypeDouble"));


    //rs->setQtDebuggerEnabled(true);
    //rs->checkpoint(SourcePosition());
    //rs->setQtDebuggerEnabled(false);
    /*
    RsType * type1 = mt->getTypeAt(sizeof(A)+offsetof(B,arr),sizeof(A));
    RsType * type2 = mt->getTypeAt(sizeof(A)+offsetof(B,arr) + 5*sizeof(A)+1,sizeof(A));
    cout << "Type1 " << (type1 ? type1->getName() : "NULL") << endl;
    cout << "Type2 " << (type2 ? type2->getName() : "NULL") << endl;
    */

    //Access to padded area
    try {mt->accessMemWithType(offsetof(A,a2)+1,ts->getTypeInfo("SgTypeChar")); }
    TEST_CATCH( RuntimeViolation::INVALID_TYPE_ACCESS)

    //Wrong type
    try {mt->accessMemWithType(0,ts->getTypeInfo("B")); }
    TEST_CATCH( RuntimeViolation::INVALID_TYPE_ACCESS)

    //Wrong basic type
    mt->accessMemWithType(offsetof(A,a2),ts->getTypeInfo("SgTypeChar"));
    try {mt->accessMemWithType(offsetof(A,a2),ts->getTypeInfo("SgTypeInt")); }
    TEST_CATCH( RuntimeViolation::INVALID_TYPE_ACCESS)

    rs->log() << "Type System Status after test" << endl;
    ts->print(rs->log());

    rs->freeMemory(42);

    CLEANUP
}

void testTypeSystemMerge()
{
    TEST_INIT("Testing TypeSystem: Merging basic types into a struct ")
    TypeSystem * ts = rs->getTypeSystem();

    struct A { int a1; int a2; float a3; };
    RsClassType * typeA = new RsClassType("A",sizeof(A));
    typeA->addMember("a1",ts->getTypeInfo("SgTypeInt"),  offsetof(A,a1));
    typeA->addMember("a2",ts->getTypeInfo("SgTypeInt"),  offsetof(A,a2));
    typeA->addMember("a3",ts->getTypeInfo("SgTypeFloat"),offsetof(A,a3));
    assert(typeA->isComplete());
    ts->registerType(typeA);

    const addr_type ADDR = 42;
    rs->createMemory(ADDR,100);

        MemoryType * mt = rs->getMemManager()->getMemoryType(ADDR);
        //first part in mem is a double
        mt->accessMemWithType(0,ts->getTypeInfo("SgTypeDouble"));

        //then two ints are accessed
        mt->accessMemWithType(sizeof(double)+offsetof(A,a1),ts->getTypeInfo("SgTypeInt"));
        mt->accessMemWithType(sizeof(double)+offsetof(A,a2),ts->getTypeInfo("SgTypeInt") );
        //then the same location is accessed with an struct of two int -> has to merge
        mt->accessMemWithType(sizeof(double),typeA);

        // because of struct access it is known that after the two ints a float follows -> access with int failes
        try {mt->accessMemWithType(sizeof(double)+offsetof(A,a3),ts->getTypeInfo("SgTypeInt")); }
        TEST_CATCH( RuntimeViolation::INVALID_TYPE_ACCESS)




    rs->freeMemory(ADDR);

    CLEANUP
}





int main(int argc, char ** argv)
{
    try
    {
        RuntimeSystem * rs = RuntimeSystem::instance();
        rs->setTestingMode(true);

#if 1
        rs->setOutputFile("test_output.txt");
#else
        testArrayAccess();
        cerr<< endl << "!!!!  Success  !!!! " << endl << endl;
        abort();
#endif
        testTypeSystemDetectNested();
        testTypeSystemMerge();

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

        testLostMemRegion();
        testPointerChanged();
        testInvalidPointerAssign();
        testPointerTracking();
        testArrayAccess();


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

        test_memcpy_strict_overlap();
        test_meminit_nullterm_included();
        test_range_overlap();
    }
    catch( RuntimeViolation& e)
    {
        out << "Unexpected Error: " << endl << e;
        exit( 1);
    }

    int a [10];

    return 0;
}

// vim:sw=4 ts=4 tw=80 et sta fdm=marker:
