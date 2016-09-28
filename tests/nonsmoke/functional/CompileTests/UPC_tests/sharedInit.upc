shared int global_counter;

#ifdef ROSE_USE_NEW_EDG_INTERFACE
// DQ (2/17/2011): Remove the initializaion since EDG reports it as an error with EDG 4.0.
shared int global_counter2;
#else
shared int global_counter2 = 2;
#endif
