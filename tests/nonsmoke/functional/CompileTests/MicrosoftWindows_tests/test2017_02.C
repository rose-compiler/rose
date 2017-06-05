// # ifdef __cplusplus
// extern "C++" int at_quick_exit (void (*__func) (void)) __THROW __asm ("at_quick_exit") __nonnull ((1));

// extern "C++" int at_quick_exit (void (*__func) (void)) __asm ("at_quick_exit") __nonnull ((1));

// extern "C++" int at_quick_exit (void (*__func) (void)) asm ("at_quick_exit") __nonnull ((1));

// extern "C++" int at_quick_exit (void (*__func) (void)) __asm ("at_quick_exit");

// extern "C++" int at_quick_exit (void (*__func) (void)) __asm {"at_quick_exit"};

// DQ (4/23/2017): Ultimately this is the best way to avoid a asm statement like this from being processed in the GNU compiler's stddef.h file.
#ifndef NOCRYPT

// extern "C++" int at_quick_exit (void (*__func) (void)) __THROW __asm ("at_quick_exit") __nonnull ((1));
int at_quick_exit (void) __asm { int 3 };

#endif

