#if 0
This is a bug relating to the reintroduction of preprocessor directives,
and the fact that these can currently not be put in the middle of an
expression:
/home/saebjornsen1/ROSE-October/ROSE-ROSE-Compile/ROSE/src/frontend/EDG/EDG_3.3/src/float_pt.c
line 317:
  } 
  if (
#if TARG_HAS_IEEE_FLOATING_POINT
      /* Don't test NaNs and Infinities. */
      is_finite(temp) &&
#endif /* TARG_H */

this gives:

  }
  if (((long_double_is_finite(temp))) && (((temp >=
0.0000000000000000000)?(temp > host_fp_flt_max):(temp <
(-host_fp_flt_max)))))
#if TARG_HAS_IEEE_FLOATING_POINT

/* Don't test NaNs and Infinities. */
// #endif /* TAR */

seems like maybe we have to work on putting prerpocessor directives in the
middle of an expression, and find out how robust we can make it.

Andreas
#endif
#endif


bool foo()
   {
     if (
#if 1
      /* Don't test NaNs and Infinities. */
          foo() ||
#endif
          true)
     {
        int x;
     }

     return true;
  }
