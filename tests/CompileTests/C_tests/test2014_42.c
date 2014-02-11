// This appears to be a bug in EDG...

// "extern" placed here (before the attribute) with "void" placed before the attribute in the defining functions, causes the attribute to go undetected.
// extern 
__attribute__((regparm(3))) void x86g_calculate_eflags_c ( int cc_op );

#if 0
// Void placed before the attribute will case it to be hidden when extern is used for the prototype.
void
__attribute__((regparm(3)))
x86g_calculate_eflags_c ( int cc_op )
   {
   }
#endif
