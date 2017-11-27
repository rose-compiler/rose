/* Type representing exception flags.  */
// typedef unsigned short int fexcept_t;

/* Type representing floating-point environment.  This structure
   corresponds to the layout of the block written by the `fstenv'
   instruction and has additional fields for the contents of the MXCSR
   register as written by the `stmxcsr' instruction.  */
#if 0
typedef struct
  {
    unsigned short int __control_word;
    unsigned short int __unused1;
    unsigned short int __status_word;
    unsigned short int __unused2;
    unsigned short int __tags;
    unsigned short int __unused3;
    unsigned int __eip;
    unsigned short int __cs_selector;
 // unsigned int __opcode:11;
 // unsigned int __unused4:5;
    unsigned int __opcode;
    unsigned int __unused4;
    unsigned int __data_offset;
    unsigned short int __data_selector;
    unsigned short int __unused5;
#ifdef __x86_64__
    unsigned int __mxcsr;
#endif
  }
xxx_fenv_t;
#else
// DQ (4/16/2017): This appears to be failing in the name qualification, so rewrite it as need to debug this.
#if 1
struct rose_fenv_tmp_struct
  {
    unsigned short int __control_word;
    unsigned short int __unused1;
    unsigned short int __status_word;
    unsigned short int __unused2;
    unsigned short int __tags;
    unsigned short int __unused3;
    unsigned int __eip;
    unsigned short int __cs_selector;
 // unsigned int __opcode:11;
 // unsigned int __unused4:5;
    unsigned int __opcode;
    unsigned int __unused4;
    unsigned int __data_offset;
    unsigned short int __data_selector;
    unsigned short int __unused5;
#ifdef __x86_64__
    unsigned int __mxcsr;
#endif
};

typedef rose_fenv_tmp_struct xxx_fenv_t;
#else
typedef unsigned    long xxx_fenv_t;
#endif
#endif

extern int feclearexcept (int __excepts) throw();
// extern int fegetexceptflag (fexcept_t *__flagp, int __excepts) throw();
extern int fegetexceptflag (unsigned short int *__flagp, int __excepts) throw();
extern int feraiseexcept (int __excepts) throw();
// extern int fesetexceptflag (const fexcept_t *__flagp, int __excepts) throw();
extern int fesetexceptflag (const unsigned short int *__flagp, int __excepts) throw();
extern int fetestexcept (int __excepts) throw();
extern int fegetround (void) throw();
extern int fesetround (int __rounding_direction) throw();
extern int fegetenv (xxx_fenv_t *__envp) throw();
extern int feholdexcept (xxx_fenv_t *__envp) throw();
extern int fesetenv (const xxx_fenv_t *__envp) throw();
extern int feupdateenv (const xxx_fenv_t *__envp) throw();
extern int feenableexcept (int __excepts) throw();
extern int fedisableexcept (int __excepts) throw();
extern int fegetexcept (void) throw();

