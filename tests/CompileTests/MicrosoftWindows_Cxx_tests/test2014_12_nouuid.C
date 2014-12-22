#if 0
    align( # )
    allocate(" segname ")
    appdomain
    code_seg(" segname ")
    deprecated
    dllimport
    dllexport
    jitintrinsic
    naked
    noalias
    noinline
    noreturn
    nothrow
    novtable
    process
    property( {get=get_func_name|,put=put_func_name})
    restrict
    safebuffers
    selectany
    thread
    uuid(" ComObjectGUID ")
#endif

__declspec(align(4))        int a_1;
__declspec(allocate("segname"))     int a_2;

// __declspec(appdomain)    int a_3;
#if 0
class CGlobal 
   {
     public:
          CGlobal(bool bProcess) {}
   };

__declspec(process)   CGlobal process_global   = CGlobal(true);  // applies in C++/CLI mode only
__declspec(appdomain) CGlobal appdomain_global = CGlobal(false); // applies in C++/CLI mode only
#endif

// __declspec(code_seg("segname"))     int a_4;
__declspec(deprecated)   int a_5;
__declspec(dllimport)    int a_6;
__declspec(dllexport)    int a_7;
// __declspec(jitintrinsic) int a_8; (does not apply here)
// __declspec(jitintrinsic) int foo_a_8(); // (does not apply here)
// void __declspec(jitintrinsic) foo_a_8() {} // (does not apply here)
// __declspec(naked)        int a_9;
__declspec( naked ) int func() {}
// __declspec(noalias)      int a_10;
__declspec(noalias) void multiply(float * a, float * b, float * c) {}


__declspec(noinline)     int a_11() {}
__declspec(noreturn)     int a_12() {}
__declspec(nothrow)      int a_13() {}
// __declspec(novtable)     int a_14() {}
struct __declspec(novtable) X { virtual void mf(); };
// __declspec(process)      int a_15;
__declspec(process)

// __declspec(property)     int a_16;
// declspec_property.cpp
struct S {
   int i;
   void putprop(int j) { 
      i = j;
   }

   int getprop() {
      return i;
   }

   __declspec(property(get = getprop, put = putprop)) int the_prop;
};

__declspec(restrict)     int* a_17();

// __declspec(safebuffers)  int a_18;
// compile with: /c /GS
typedef struct {
    int x[20];
} BUFFER;
static int checkBuffers() {
    BUFFER cb;
    // Use the buffer...
    return 0;
};
static __declspec(safebuffers) 
    int noCheckBuffers() {
    BUFFER ncb;
    // Use the buffer...
    return 0;
}
__declspec(selectany)    int a_19;
__declspec(thread)       int a_20;

#if 0
// __declspec(uuid)         int a_21;
__declspec( uuid("ComObjectGUID") ) int a_21;
struct __declspec(uuid("00000000-0000-0000-c000-000000000046")) IUnknown;
struct __declspec(uuid("{00020400-0000-0000-c000-000000000046}")) IDispatch;
#endif
