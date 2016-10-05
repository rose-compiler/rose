// The use of the mutex header file demonstrates where the Intel compiler use (as backend 
// compiler) was not having the correct value of __cplusplus set in the case of C++11.
#include <mutex>
