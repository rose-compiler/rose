// This is a C language test code that can be compiled using the C++11 mode.
// It is important because we sometimes want to compile C application (or 
// the C files of a larger C++ application) with C++ modes to support use of
// tools that are designed for C++ (e.g. GoogleTest, which is designed to 
// work with C++11.

#ifdef __cplusplus
extern "C" {
#endif

void foobar (void);

#ifdef __cplusplus
}
#endif

void foobar (void)
   {
   }
