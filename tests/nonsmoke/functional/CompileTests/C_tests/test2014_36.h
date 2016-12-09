
typedef unsigned int UInt;

__attribute__((regparm(3))) 
extern UInt x86g_calculate_eflags_c ( UInt cc_op, UInt cc_dep1, UInt cc_dep2, UInt cc_ndep );

