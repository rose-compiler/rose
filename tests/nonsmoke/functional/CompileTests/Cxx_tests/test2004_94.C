// This test code demonstrates where a struct is referenced before being defined
// (swig_type_info is referenced within a typedef).  Yes, this legal code!
// This code comes from a construction of code found in SWIG generated code.

// This is an error! Because swig_type_info is not defined
// swig_type_info* structurePointer2 = 0L;

// This is a typedef that references "swig_type_info" and defines "swig_dycast_func"
// What is interesting is that it also, in some sense, defines "swig_type_info"
// so that subsequent references to the "swig_type_info" type (not requiring 
// a defining declaration) are valid C++ code.

#warning "This is a bug, but I have commented it out!"
// typedef struct swig_type_info *(*swig_dycast_func)(void **);

struct swig_type_info {};

#if 0
// This is a function pointer using the typedef which defines "swig_dycast_func"
swig_dycast_func functionPointer = 0L;

// This is not an error since swig_type_info is defined as a struct within the typedef 
// (even though the typedef defines the function pointer "swig_dycast_func")
swig_type_info* structurePointer = 0L;
#endif

#if 0
// This is the defining declaration of "swig_type_info", which is not really 
// required for the above code to be valid C++ code.
typedef struct swig_type_info {
  const char             *name;                 
  swig_converter_func     converter;
  const char             *str;
  void                   *clientdata;	
  swig_dycast_func        dcast;
  struct swig_type_info  *next;
  struct swig_type_info  *prev;
} swig_type_info;
#endif


