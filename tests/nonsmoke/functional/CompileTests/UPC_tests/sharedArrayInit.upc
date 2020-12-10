#if 1
// DQ (2/17/2011): Remove the initializaion since EDG reports it as an error with EDG 4.0.
shared int array [2*THREADS];
#else
shared int array [2*THREADS] = {0,1};
#endif
