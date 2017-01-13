// This is supported only in GNU versions greater than 4.6.
#if __GNUC__ == 4 && __GNUC_MINOR__ >= 7 || __GNUC__ > 4
  #include<stdalign.h>
#endif

// This type does not appear to be available in gcc so we can't use it.
// max_align_t x;
