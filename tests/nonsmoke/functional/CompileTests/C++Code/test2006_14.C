#if 0
The following code:
void foo(){
  char * flind;
  strlen(flind);

};
gives the following error when compiled with ROSE:
"../../../../ROSE/src/util/commandlineProcessing/sla.c", line 5: error:
          identifier "strlen" is undefined
    strlen(flind);

It compiles fine with GCC.

#endif

void foo()
   {
     char * flind;
     strlen(flind);
   };
