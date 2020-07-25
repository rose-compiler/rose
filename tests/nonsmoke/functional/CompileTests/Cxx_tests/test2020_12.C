#if _GNUC_ >= 10

// DQ (7/23/2020): This appears to work for EDG 5.0 with GNU 6.1, but 
// fails with EDG 6.0 using GNU 6.1, and it works for EDG 6.0 and GNU 10.1, 
// so I'm not clear on where the boundaries are for when this works.
#if __has_include(<test2020_12.h>)

#endif

#endif
