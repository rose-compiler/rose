
// "extern" placed here (before the attribute) and with "void" placed after the attribute, cause the attribute to be attached to the return type of the function.
// Since this is a gnu attribute that applies only to the function, it need to be interpreted to apply to the function independent of its location in the EDG AST.
extern 
__attribute__((regparm(3))) void x86g_calculate_eflags_c ( int cc_op );

#if 1
void
__attribute__((regparm(1)))
   x86g_calculate_eflags_c ( int cc_op )
   {
   }
#endif


