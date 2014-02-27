
typedef int Int;
typedef unsigned int UInt;
typedef char HChar;

UInt VG_USERREQ__RUNNING_ON_VALGRIND = 0;

void vgPlain_debugLog ( Int level, const HChar* modulename, const HChar* format, ... )
{
   UInt pid;
   Int indent, depth, i;
// va_list vargs;
// printf_buf buf;

#if 0
   if (level > loglevel)
      return;

   indent = 2*level - 1;
   if (indent < 1) indent = 1;
#endif
#if 0
   buf.n = 0;
   buf.buf[0] = 0;
   pid = local_sys_getpid();
#endif

#if 0
// Where this appears in Valgrind (m_debuglog.c in function name that macro expands to vgPlain_debugLog) the generated code in ROSE does not compile properly.
// However, the similar problem extracted as a test code (this code) does compile fine.  Not clear what the issues is here.
// Unparses to:   
depth = ((unsigned int )((
{
    volatile unsigned long long _zzq_args[6];
    volatile unsigned long long _zzq_result;
    _zzq_args[0] = ((unsigned long long )VG_USERREQ__RUNNING_ON_VALGRIND);
    _zzq_args[1] = ((unsigned long long )0);
    _zzq_args[2] = ((unsigned long long )0);
    _zzq_args[3] = ((unsigned long long )0);
    _zzq_args[4] = ((unsigned long long )0);
    _zzq_args[5] = ((unsigned long long )0);

// Orignal code:
//  __asm__ volatile("roll $3,  %%edi ; roll $13, %%edi\n\t" "roll $29, %%edi ; roll $19, %%edi\n\t" "xchgl %%ebx,%%ebx" : "=d" (_zzq_result) : "a" (&_zzq_args[0]), "0" (0) : "cc", "memory" ); _zzq_result; });
// Generated code:
//  __asm__ volatile ("roll $3,  %%edi ; roll $13, %%edi\n\troll $29, %%edi ; roll $19, %%edi\n\txchgl %%ebx,%%ebx" : "=d" (_zzq_result) : "a" ((&_zzq_args[0])), "0" (0) : "memory");

    __asm__ volatile ("rolq $3,  %%rdi ; rolq $13, %%rdi\n\trolq $61, %%rdi ; rolq $51, %%rdi\n\txchgq %%rbx,%%rbx" : "=d" (_zzq_result) : "a" ((&_zzq_args[0])), "0" (0) : "memory");

    _zzq_result;
  })));
#endif

   depth = (unsigned)__extension__ ({volatile unsigned int _zzq_args[6]; volatile unsigned int _zzq_result; _zzq_args[0] = (unsigned int)(VG_USERREQ__RUNNING_ON_VALGRIND); _zzq_args[1] = (unsigned int)(0); _zzq_args[2] = (unsigned int)(0); _zzq_args[3] = (unsigned int)(0); _zzq_args[4] = (unsigned int)(0); _zzq_args[5] = (unsigned int)(0); 
   __asm__ volatile("roll $3,  %%edi ; roll $13, %%edi\n\t" "roll $29, %%edi ; roll $19, %%edi\n\t" "xchgl %%ebx,%%ebx" : "=d" (_zzq_result) : "a" (&_zzq_args[0]), "0" (0) : "cc", "memory" ); _zzq_result; });

#if 0
   for (i = 0; i < depth; i++) {
      (void)myvprintf_str ( add_to_buf, &buf, 0, 1, ">", ((Bool)0) );
   }

   (void)myvprintf_str ( add_to_buf, &buf, 0, 2, "--", ((Bool)0) );
   (void)myvprintf_int64 ( add_to_buf, &buf, 0, 10, 1, ((Bool)0), (ULong)pid );
   (void)myvprintf_str ( add_to_buf, &buf, 0, 1, ":", ((Bool)0) );
   (void)myvprintf_int64 ( add_to_buf, &buf, 0, 10, 1, ((Bool)0), (ULong)level );
   (void)myvprintf_str ( add_to_buf, &buf, 0, 1, ":", ((Bool)0) );
   (void)myvprintf_str ( add_to_buf, &buf, 0, 8, (HChar*)modulename, ((Bool)0) );
   (void)myvprintf_str ( add_to_buf, &buf, 0, indent, "", ((Bool)0) );

   __builtin_va_start(vargs,format);

   (void) vgPlain_debugLog_vprintf ( add_to_buf, &buf, format, vargs );

   if (buf.n > 0) {
      emit( buf.buf, local_strlen(buf.buf) );
   }

   __builtin_va_end(vargs);
#endif

}
