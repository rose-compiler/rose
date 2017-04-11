
// DQ (4/5/2017): These tests can't be supported using Intel V14
#if !(defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 1400))

// Example of thread_local keyword as storage modifier.
thread_local unsigned int rage = 1;

#endif
