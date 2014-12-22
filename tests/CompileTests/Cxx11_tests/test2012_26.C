// Thread-local storage

// In a multi-threaded environment, it is common for every thread to have some unique variables. 
// This already happens for the local variables of a function, but it does not happen for global 
// and static variables.

// A new thread-local storage duration (in addition to the existing static, dynamic and automatic) 
// is indicated by the storage specifier thread_local.

// Any object which could have static storage duration (i.e., lifetime spanning the entire execution 
// of the program) may be given thread-local duration instead. The intent is that like any other 
// static-duration variable, a thread-local object can be initialized using a constructor and 
// destroyed using a destructor.

// #error "NEED AND EXAMPLE OF THIS!"

// Example of thread_local keyword as storage modifier.
thread_local unsigned int rage = 1;

