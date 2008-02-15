void foo (void)
   {
     enum LocalEnum { VAL_A, VAL_B, VAL_C };
#if __cplusplus
     LocalEnum x = VAL_B;
#else
     enum LocalEnum x = VAL_B;
#endif
   }
