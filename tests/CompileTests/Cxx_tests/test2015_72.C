 
 


 




 

 



 

 
 



 

 




 

 

 
 

 




 
 
 

 

 
 


 




 

 
 





 
 
 










 
 



















 
 
double      __builtin_copysign  (double __builtin__x, double __builtin__y);
float       __builtin_copysignf (float __builtin__x, float __builtin__y);
long double __builtin_copysignl (long double __builtin__x, long double __builtin__y);








 



 
float       __builtin_acosf  (float __builtin__x);
long double __builtin_acosl  (long double __builtin__x);
float       __builtin_asinf  (float __builtin__x);
long double __builtin_asinl  (long double __builtin__x);
float       __builtin_atanf  (float __builtin__x);
long double __builtin_atanl  (long double __builtin__x);
float       __builtin_atan2f (float __builtin__x,float __builtin__y);
long double __builtin_atan2l (long double __builtin__x,long double __builtin__y);
float       __builtin_ceilf  (float __builtin__x);
long double __builtin_ceill  (long double __builtin__x);
float       __builtin_coshf  (float __builtin__x);
long double __builtin_coshl  (long double __builtin__x);
float       __builtin_floorf (float __builtin__x);
long double __builtin_floorl (long double __builtin__x);
float       __builtin_fmodf  (float __builtin__x,float __builtin__y);
long double __builtin_fmodl  (long double __builtin__x,long double __builtin__y);
float       __builtin_frexpf (float __builtin__x,int *__builtin__y);
long double __builtin_frexpl (long double __builtin__x,int *__builtin__y);
float       __builtin_ldexpf (float __builtin__x,float __builtin__y);
long double __builtin_ldexpl (long double __builtin__x,long double __builtin__y);
float       __builtin_log10f (float __builtin__x);
long double __builtin_log10l (long double __builtin__x);
float       __builtin_modff  (float __builtin__x,float *__builtin__y);
long double __builtin_modfl  (long double __builtin__x,long double *__builtin__y);
float       __builtin_powf   (float __builtin__x,float __builtin__y);
long double __builtin_powl   (long double __builtin__x,long double __builtin__y);
float       __builtin_sinhf  (float __builtin__x);
long double __builtin_sinhl  (long double __builtin__x);
float       __builtin_tanf   (float __builtin__x);
long double __builtin_tanl   (long double __builtin__x);
float       __builtin_tanhf  (float __builtin__x);
long double __builtin_tanhl  (long double __builtin__x);

 
long double __builtin_powil  (long double __builtin__x, int __builtin__i);
double      __builtin_powi   (double __builtin__x, int __builtin__i);
float       __builtin_powif  (float __builtin__x, int __builtin__i);





 











 






 
char *      __builtin_strchr (const char *__builtin__s, int __builtin__c);
char *      __builtin_strrchr(const char *__builtin__s, int __builtin__c);
char *      __builtin_strpbrk(const char *__builtin__s, const char *__builtin__accept);
char *      __builtin_strstr (const char *__builtin__haystack, const char *__builtin__needle);
float       __builtin_nansf  (const char *__builtin__x);
double      __builtin_nans   (const char *__builtin__x);
long double __builtin_nansl  (const char *__builtin__x);
double      __builtin_fabs   (double      __builtin__x);
float       __builtin_fabsf  (float       __builtin__x);
long double __builtin_fabsl  (long double __builtin__x);
float       __builtin_cosf   (float       __builtin__x);
long double __builtin_cosl   (long double __builtin__x);
float       __builtin_sinf   (float       __builtin__x);
long double __builtin_sinl   (long double __builtin__x);
float       __builtin_sqrtf  (float       __builtin__x);
long double __builtin_sqrtl  (long double __builtin__x);

 

int __builtin_isgreater(double x, double y);
int __builtin_isgreaterequal(double x, double y);
int __builtin_isless(double x, double y);
int __builtin_ia32_bsrsi(int x);
unsigned long long __builtin_ia32_rdpmc(int x);
unsigned long long __builtin_ia32_rdtsc(void);
unsigned long long __builtin_ia32_rdtscp(unsigned int *__A);
unsigned char      __builtin_ia32_rolqi(unsigned char __X, int __C);
unsigned short     __builtin_ia32_rolhi(unsigned short __X, int __C);
unsigned char      __builtin_ia32_rorqi(unsigned char __X, int __C);
unsigned short     __builtin_ia32_rorhi(unsigned short __X, int __C);
void               __builtin_ia32_pause();
int                __builtin_ia32_bsrdi(long long __X);
long long          __builtin_bswap64(long long __X);

void               __builtin_ia32_fxsave(void *__P);
void               __builtin_ia32_fxrstor(void *__P);
void               __builtin_ia32_fxsave64(void *__P);
void               __builtin_ia32_fxrstor64(void *__P);
unsigned char      __builtin_ia32_addcarryx_u32(unsigned char __CF, unsigned int __X, unsigned int __Y, unsigned int *__P);
unsigned char      __builtin_ia32_addcarryx_u64(unsigned char __CF, unsigned long __X, unsigned long __Y, unsigned long long *__P);








 

 
void * __builtin_return_address (unsigned int level);
void * __builtin_frame_address (unsigned int level);













 

 
 
 


 

 




 




 

long        __builtin_expect (long __builtin__exp, long __builtin__c);
void        __builtin_prefetch (const void *__builtin__addr, ...);
double      __builtin_huge_val (void);
float       __builtin_huge_valf (void);
long double __builtin_huge_vall (void);
double      __builtin_inf (void);
float       __builtin_inff (void);
long double __builtin_infl (void);
double      __builtin_nan (const char *__builtin__str);
float       __builtin_nanf (const char *__builtin__str);
long double __builtin_nanl (const char *__builtin__str);
double      __builtin_nans (const char *__builtin__str);
float       __builtin_nansf (const char *__builtin__str);
long double __builtin_nansl (const char *__builtin__str);


 
int __builtin_clz (unsigned int __builtin__x);
int __builtin_ctz (unsigned int __builtin__x);
int __builtin_popcount (unsigned int __builtin__x);
int __builtin_parity (unsigned int __builtin__x);
int __builtin_ffsl (unsigned long __builtin__x);
int __builtin_clzl (unsigned long __builtin__x);
int __builtin_ctzl (unsigned long __builtin__x);
int __builtin_popcountl (unsigned long __builtin__x);
int __builtin_parityl (unsigned long __builtin__x);
int __builtin_ffsll (unsigned long long __builtin__x);
int __builtin_clzll (unsigned long long __builtin__x);
int __builtin_ctzll (unsigned long long __builtin__x);
int __builtin_popcountll (unsigned long long __builtin__x);
int __builtin_parityll (unsigned long long __builtin__x);
double      __builtin_powi (double __builtin__x, int __builtin__y);
float       __builtin_powif (float __builtin__x, int __builtin__y);
long double __builtin_powil (long double __builtin__x, int __builtin__y);















 

 

 
 
 



 











 





















































 

 
int __builtin_abs (int __builtin__x);








 









 
 




























 











 
 
 
 


 



 
 


 
 

 
 

 
 
 

 


 


 
 







 
 
 



 



 







 

 
void __builtin_ia32_emms();
int __builtin_ia32_vec_init_v2si(int,int);
int __builtin_ia32_vec_ext_v2si(int,int);
int __builtin_ia32_packsswb(short,short);
int __builtin_ia32_packssdw(int,int);
int __builtin_ia32_packuswb(short,short);
int __builtin_ia32_punpckhbw(short,short);
int __builtin_ia32_punpckhwd(short,short);
int __builtin_ia32_punpckhdq(int,int);
int __builtin_ia32_punpcklbw(char,char);
int __builtin_ia32_punpcklwd(short,short);
int __builtin_ia32_punpckldq(int,int);
int __builtin_ia32_paddb(char,char);
int __builtin_ia32_paddw(int,int);
int __builtin_ia32_paddd(short,short);
int __builtin_ia32_paddq(long long,long long);
int __builtin_ia32_paddsb(char,char);
int __builtin_ia32_paddsw(int,int);
int __builtin_ia32_paddusb(char,char);
int __builtin_ia32_paddusw(int,int);
int __builtin_ia32_psubb(char,char);
int __builtin_ia32_psubw(int,int);
int __builtin_ia32_psubd(int,int);
int __builtin_ia32_psubq(long long,long long);
int __builtin_ia32_psubsb(char,char);
int __builtin_ia32_psubsw(short,short);
int __builtin_ia32_psubusb(char,char);
int __builtin_ia32_psubusw(int,int);
int __builtin_ia32_pmaddwd(short,short);
int __builtin_ia32_pmulhw(short,short);
int __builtin_ia32_pmullw(short,short);
int __builtin_ia32_psllw(short,long long);
int __builtin_ia32_pslld(int,long long);
int __builtin_ia32_psllq(long long, long long);
int __builtin_ia32_psraw(short, long long);
int __builtin_ia32_psrad(short, long long);
int __builtin_ia32_psrlw(short,long long);
int __builtin_ia32_psrld(short, long long);
int __builtin_ia32_psrlq(long long, long long);
int __builtin_ia32_pand(int,int);
int __builtin_ia32_pandn(int,int);
int __builtin_ia32_por(int,int);
int __builtin_ia32_pxor(int,int);
int __builtin_ia32_pcmpeqb(char,char);
int __builtin_ia32_pcmpgtb(char,char);
int __builtin_ia32_pcmpeqw(short,short);
int __builtin_ia32_pcmpgtw(short,short);
int __builtin_ia32_pcmpeqd(int,int);
int __builtin_ia32_pcmpgtd(int,int);
int __builtin_ia32_vec_init_v2si(int,int);
int __builtin_ia32_vec_init_v4hi(short,short,short,short);
int __builtin_ia32_vec_init_v8qi(char,char,char,char,char,char,char,char);

 
int __builtin_ia32_addss(float,float);
int __builtin_ia32_subss(float,float);
int __builtin_ia32_mulss(float,float);
int __builtin_ia32_divss(float,float);
int __builtin_ia32_sqrtss(float);
int __builtin_ia32_rcpss(float);
int __builtin_ia32_rsqrtss(float);
int __builtin_ia32_minss(float,float);
int __builtin_ia32_maxss(float,float);
int __builtin_ia32_addps(float,float);
int __builtin_ia32_subps(float,float);
int __builtin_ia32_mulps(float,float);
int __builtin_ia32_divps(float,float);
int __builtin_ia32_sqrtps(float);
int __builtin_ia32_rcpps(float);
int __builtin_ia32_rsqrtps(float);
int __builtin_ia32_minps(float,float);
int __builtin_ia32_maxps(float,float);
int __builtin_ia32_andps(float,float);
int __builtin_ia32_andnps(float,float);
int __builtin_ia32_orps(float,float);
int __builtin_ia32_xorps(float,float);
int __builtin_ia32_cmpeqss(float,float);
int __builtin_ia32_cmpltss(float,float);
int __builtin_ia32_cmpless(float,float);
int __builtin_ia32_cmpltss(float,float);
int __builtin_ia32_movss(float,float);
int __builtin_ia32_cmpless(float,float);
int __builtin_ia32_cmpneqss(float,float);
int __builtin_ia32_cmpnltss(float,float);
int __builtin_ia32_cmpnless(float,float);
int __builtin_ia32_cmpordss(float,float);
int __builtin_ia32_cmpunordss(float,float);
int __builtin_ia32_cmpeqps(float,float);
int __builtin_ia32_cmpltps(float,float);
int __builtin_ia32_cmpleps(float,float);
int __builtin_ia32_cmpgtps(float,float);
int __builtin_ia32_cmpgeps(float,float);
int __builtin_ia32_cmpneqps(float,float);
int __builtin_ia32_cmpnltps(float,float);
int __builtin_ia32_cmpnleps(float,float);
int __builtin_ia32_cmpngtps(float,float);
int __builtin_ia32_cmpngeps(float,float);
int __builtin_ia32_cmpordps(float,float);
int __builtin_ia32_cmpunordps(float,float);
int __builtin_ia32_comieq(float,float);
int __builtin_ia32_comilt(float,float);
int __builtin_ia32_comile(float,float);
int __builtin_ia32_comigt(float,float);
int __builtin_ia32_comige(float,float);
int __builtin_ia32_comineq(float,float);
int __builtin_ia32_ucomieq(float,float);
int __builtin_ia32_ucomilt(float,float);
int __builtin_ia32_ucomile(float,float);
int __builtin_ia32_ucomigt(float,float);
int __builtin_ia32_ucomige(float,float);
int __builtin_ia32_ucomineq(float,float);
int __builtin_ia32_cvtss2si(float);
int __builtin_ia32_cvtss2si64(float);
int __builtin_ia32_cvtps2pi(float);
int __builtin_ia32_cvttss2si(float);
int __builtin_ia32_cvttss2si64(float);
int __builtin_ia32_cvttps2pi(float);
int __builtin_ia32_cvtsi2ss(float,float);
int __builtin_ia32_cvtsi642ss(float,float);
int __builtin_ia32_cvtsi642ss(float,float);
int __builtin_ia32_cvtpi2ps(float,float);
int __builtin_ia32_cvtpi2ps(float,float);
int __builtin_ia32_movlhps(float,float);
int __builtin_ia32_cvtpi2ps(float,float);
int __builtin_ia32_movlhps(float,float);
int __builtin_ia32_cvtpi2ps(float,float);
int __builtin_ia32_movlhps(float,float);
int __builtin_ia32_movhlps(float,float);
int __builtin_ia32_cvtps2pi(float);
int __builtin_ia32_unpckhps(float,float);
int __builtin_ia32_unpcklps(float,float);
int __builtin_ia32_loadhps(float,int*);
int __builtin_ia32_storehps(int*,float);
int __builtin_ia32_movhlps(float,float);
int __builtin_ia32_movlhps(float,float);
int __builtin_ia32_loadlps(float,int*);
int __builtin_ia32_storelps(int*,float);
int __builtin_ia32_movmskps(float);
int __builtin_ia32_stmxcsr();
int __builtin_ia32_ldmxcsr(int);
int __builtin_ia32_loadups(float const*);
int __builtin_ia32_shufps(short,short,int);
int __builtin_ia32_vec_ext_v4sf(float,float);
int __builtin_ia32_vec_ext_v4sf(float,float);
int __builtin_ia32_storeups(float*,short);
int __builtin_ia32_pmaxsw(float,float);
int __builtin_ia32_pmaxub(float,float);
int __builtin_ia32_pminsw(float,float);
int __builtin_ia32_pminub(float,float);
int __builtin_ia32_pmovmskb(char);
int __builtin_ia32_pmulhuw(float,float);
int __builtin_ia32_maskmovq(char,char,char*);
int __builtin_ia32_pavgb(float,float);
int __builtin_ia32_pavgw(float,float);
int __builtin_ia32_psadbw(float,float);
int __builtin_ia32_movntq(unsigned long long*,unsigned long long);
int __builtin_ia32_movntps(float*,float);
int __builtin_ia32_sfence();


 
int __builtin_ia32_movsd(double,double);
int __builtin_ia32_loadupd(double const *);
int __builtin_ia32_shufpd(double,double,int);
int __builtin_ia32_storeupd(double*,double);
int __builtin_ia32_vec_ext_v2df(double,int);
 
int __builtin_ia32_vec_ext_v4si(int,int);
int __builtin_ia32_vec_ext_v2di(long long,int);
int __builtin_ia32_addpd(double,double);
int __builtin_ia32_addsd(double,double);
int __builtin_ia32_subpd(double,double);
int __builtin_ia32_subsd(double,double);
int __builtin_ia32_mulpd(double,double);
int __builtin_ia32_mulsd(double,double);
int __builtin_ia32_divpd(double,double);
int __builtin_ia32_divsd(double,double);
int __builtin_ia32_sqrtpd(double);
int __builtin_ia32_sqrtsd(double);
int __builtin_ia32_minpd(double,double);
int __builtin_ia32_minsd(double,double);
int __builtin_ia32_maxpd(double,double);
int __builtin_ia32_maxsd(double,double);
int __builtin_ia32_andpd(double,double);
int __builtin_ia32_andnpd(double,double);
int __builtin_ia32_orpd(double,double);
int __builtin_ia32_xorpd(double,double);
int __builtin_ia32_cmpeqpd(double,double);
int __builtin_ia32_cmpltpd(double,double);
int __builtin_ia32_cmplepd(double,double);
int __builtin_ia32_cmpgtpd(double,double);
int __builtin_ia32_cmpgepd(double,double);
int __builtin_ia32_cmpneqpd(double,double);
int __builtin_ia32_cmpnltpd(double,double);
int __builtin_ia32_cmpnlepd(double,double);
int __builtin_ia32_cmpngtpd(double,double);
int __builtin_ia32_cmpngepd(double,double);
int __builtin_ia32_cmpordpd(double,double);
int __builtin_ia32_cmpunordpd(double,double);
int __builtin_ia32_cmpeqsd(double,double);
int __builtin_ia32_cmpltsd(double,double);
int __builtin_ia32_cmplesd(double,double);
int __builtin_ia32_cmpltsd(double,double);
int __builtin_ia32_cmplesd(double,double);
int __builtin_ia32_cmpneqsd(double,double);
int __builtin_ia32_cmpnltsd(double,double);
int __builtin_ia32_cmpnlesd(double,double);
int __builtin_ia32_cmpordsd(double,double);
int __builtin_ia32_cmpunordsd(double,double);
int __builtin_ia32_comisdeq(double,double);
int __builtin_ia32_comisdlt(double,double);
int __builtin_ia32_comisdle(double,double);
int __builtin_ia32_comisdgt(double,double);
int __builtin_ia32_comisdge(double,double);
int __builtin_ia32_comisdg(double,double);
int __builtin_ia32_comisdneq(double,double);
int __builtin_ia32_ucomisdeq(double,double);
int __builtin_ia32_ucomisdlt(double,double);
int __builtin_ia32_ucomisdle(double,double);
int __builtin_ia32_ucomisdgt(double,double);
int __builtin_ia32_ucomisdge(double,double);
int __builtin_ia32_ucomisdneq(double,double);
int __builtin_ia32_loaddqu(char const*);
int __builtin_ia32_storedqu(char*, char);
int __builtin_ia32_cvtdq2pd(int);
int __builtin_ia32_cvtdq2ps(int);
int __builtin_ia32_cvtpd2dq(double);
int __builtin_ia32_cvtpd2pi(double);
int __builtin_ia32_cvtpd2ps(double);
int __builtin_ia32_cvttpd2dq(double);
int __builtin_ia32_cvttpd2pi(double);
int __builtin_ia32_cvtpi2pd(int);
int __builtin_ia32_cvtps2dq(double);
int __builtin_ia32_cvttps2dq(double);
int __builtin_ia32_cvtps2pd(float);
int __builtin_ia32_cvtsd2si(double);
int __builtin_ia32_cvtsd2si64(double);
int __builtin_ia32_cvtsd2si64(double);
int __builtin_ia32_cvttsd2si(double);
int __builtin_ia32_cvttsd2si64(double);
int __builtin_ia32_cvtsd2ss(int,int);
int __builtin_ia32_cvtsi2sd(int,int);
int __builtin_ia32_cvtsi642sd(int,int);
int __builtin_ia32_cvtsi642sd(int,int);
int __builtin_ia32_cvtss2sd(int,int);
int __builtin_ia32_unpcklpd(int,int);
int __builtin_ia32_unpckhpd(double,double);
int __builtin_ia32_loadhpd(double, double const *);
int __builtin_ia32_loadlpd(double, double const *);
int __builtin_ia32_movmskpd(double);
int __builtin_ia32_packsswb128(short,short);
int __builtin_ia32_packssdw128(short,short);
int __builtin_ia32_packuswb128(short,short);
int __builtin_ia32_punpckhbw128(short,short);
int __builtin_ia32_punpckhwd128(int,int);
int __builtin_ia32_punpckhdq128(int,int);
int __builtin_ia32_punpckhqdq128(int,int);
int __builtin_ia32_punpcklbw128(int,int);
int __builtin_ia32_punpcklwd128(int,int);
int __builtin_ia32_punpckldq128(int,int);
int __builtin_ia32_punpcklqdq128(int,int);
int __builtin_ia32_paddb128(long long,long long);
int __builtin_ia32_paddw128(short,short);
int __builtin_ia32_paddd128(int,int);
int __builtin_ia32_paddq128(long long,long long);
int __builtin_ia32_paddsb128(long long,long long);
int __builtin_ia32_paddsw128(short,short);
int __builtin_ia32_paddusb128(char,char);
int __builtin_ia32_paddusw128(short,short);
int __builtin_ia32_psubb128(char,char);
int __builtin_ia32_psubw128(short,short);
int __builtin_ia32_psubd128(int,int);
int __builtin_ia32_psubq128(double,double);
int __builtin_ia32_psubsb128(char,char);
int __builtin_ia32_psubsw128(short,short);
int __builtin_ia32_psubusb128(char,char);
int __builtin_ia32_psubusw128(short,short);
int __builtin_ia32_pmaddwd128(short,short);
int __builtin_ia32_pmulhw128(short,short);
int __builtin_ia32_pmullw128(short,short);
int __builtin_ia32_pmuludq(int,int);
int __builtin_ia32_pmuludq128(int,int);
int __builtin_ia32_psllwi128(short,short);
int __builtin_ia32_pslldi128(int,int);
int __builtin_ia32_psllqi128(int,int);
int __builtin_ia32_psrawi128(short,short);
int __builtin_ia32_psradi128(short,short);
int __builtin_ia32_psrlwi128(short,short);
int __builtin_ia32_psrldi128(short,short);
int __builtin_ia32_psrlqi128(short,short);
int __builtin_ia32_psllw128(short,short);
int __builtin_ia32_pslld128(short,short);
int __builtin_ia32_psllq128(short,short);
int __builtin_ia32_psraw128(short,short);
int __builtin_ia32_psrad128(short,short);
int __builtin_ia32_psrlw128(short,short);
int __builtin_ia32_psrld128(short,short);
int __builtin_ia32_psrlq128(short,short);
int __builtin_ia32_pand128(int,int);
int __builtin_ia32_pandn128(int,int);
int __builtin_ia32_por128(int,int);
int __builtin_ia32_pxor128(int,int);
int __builtin_ia32_pcmpeqb128(char,char);
int __builtin_ia32_pcmpeqw128(short,short);
int __builtin_ia32_pcmpeqd128(short,short);
int __builtin_ia32_pcmpgtb128(char,char);
int __builtin_ia32_pcmpgtw128(short,short);
int __builtin_ia32_pcmpgtd128(int,int);
int __builtin_ia32_pcmpgtb128(char,char);
int __builtin_ia32_pcmpgtw128(short,short);
int __builtin_ia32_pmaxsw128(short,short);
int __builtin_ia32_pmaxub128(char,char);
int __builtin_ia32_pminsw128(short,short);
int __builtin_ia32_pminub128(char,char);
int __builtin_ia32_pmovmskb128(char);
int __builtin_ia32_pmulhuw128(short,short);
int __builtin_ia32_maskmovdqu(char,char,char*);
int __builtin_ia32_pavgb128(char,char);
int __builtin_ia32_pavgw128(short,short);
int __builtin_ia32_psadbw128(char,char);
int __builtin_ia32_movnti(int*,int);
int __builtin_ia32_movntdq(long long*,long long);
int __builtin_ia32_movntpd(double*,double);
int __builtin_ia32_clflush(void const *);
int __builtin_ia32_lfence();
int __builtin_ia32_mfence();


int __builtin_ia32_psllwi(short,int);
int __builtin_ia32_pslldi(int,int);
int __builtin_ia32_psllqi(long long,int);
int __builtin_ia32_psrawi(short,int);
int __builtin_ia32_psradi(int,int);
int __builtin_ia32_psrlwi(int,int);
int __builtin_ia32_psrldi(int,int);
int __builtin_ia32_psrlqi(long long,int);









int __builtin_bswap32 (int x);
 

 




























 


#pragma GCC system_header






































 



 


#pragma GCC system_header






 






 

















































 




 


 


 



























 
























































































 
namespace std
{
  typedef unsigned long 	size_t;
  typedef long	ptrdiff_t;

  typedef decltype(nullptr)	nullptr_t;
}


























































































 






















 











































































 


 


 

 







 


 

 


 

 

 

 




 


 



















 















 








 

 


 

 







 


 


 

 











 


















 


 



 

 



 





 



 


 


 


 






 


 


 


 


 
 










 







 



 



 



 

 



 

 






 




 


 


 

 


 









 



 




 

 



 


 


 



 

 



 


 





 


 





 






































 
























 
 

 

 

 

 

 

 

 

 

 

 

 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 

 
 

 

 

 

 

 

 

 

 

 
 

 

 

 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 

 
 

 

 

 

 

 

 
 

 

 

 

 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 

 
 

 

 

 
 

 
 

 

 

 

 

 

 

 

 

 

 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 

 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 


 

 
 

 

 

 

 

 

 

 
 

 
 

 
 

 
 

 
 

 

 
 

 

 
 


 

 

 

 

 
 

 

 
 

 
 

 

 

 

 

 
 

 

 
 

 
 


 



 



 


 


 


 


 


 


 


 

 

 

 


 

 

 

 

 

 

 

 
 

 

 
 


 

 
 

 

 
 

 
 

 

 

 

 






























































































 

































 





namespace std 
{


  
  void
  __throw_bad_exception(void) __attribute__((__noreturn__));

  
  void
  __throw_bad_alloc(void) __attribute__((__noreturn__));

  
  void
  __throw_bad_cast(void) __attribute__((__noreturn__));

  void
  __throw_bad_typeid(void) __attribute__((__noreturn__));

  
  void
  __throw_logic_error(const char*) __attribute__((__noreturn__));

  void
  __throw_domain_error(const char*) __attribute__((__noreturn__));

  void
  __throw_invalid_argument(const char*) __attribute__((__noreturn__));

  void
  __throw_length_error(const char*) __attribute__((__noreturn__));

  void
  __throw_out_of_range(const char*) __attribute__((__noreturn__));

  void
  __throw_runtime_error(const char*) __attribute__((__noreturn__));

  void
  __throw_range_error(const char*) __attribute__((__noreturn__));

  void
  __throw_overflow_error(const char*) __attribute__((__noreturn__));

  void
  __throw_underflow_error(const char*) __attribute__((__noreturn__));

  
  void
  __throw_ios_failure(const char*) __attribute__((__noreturn__));

  void
  __throw_system_error(int) __attribute__((__noreturn__));

  void
  __throw_future_error(int) __attribute__((__noreturn__));

  
  void
  __throw_bad_function_call() __attribute__((__noreturn__));


} 




























 




#pragma GCC system_header































namespace __gnu_cxx 
{


  template<typename _Iterator, typename _Container>
    class __normal_iterator;


} 

namespace std 
{


  struct __true_type { };
  struct __false_type { };

  template<bool>
    struct __truth_type
    { typedef __false_type __type; };

  template<>
    struct __truth_type<true>
    { typedef __true_type __type; };

  
  
  template<class _Sp, class _Tp>
    struct __traitor
    {
      enum { __value = bool(_Sp::__value) || bool(_Tp::__value) };
      typedef typename __truth_type<__value>::__type __type;
    };

  
  template<typename, typename>
    struct __are_same
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  template<typename _Tp>
    struct __are_same<_Tp, _Tp>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  
  template<typename _Tp>
    struct __is_void
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  template<>
    struct __is_void<void>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  
  
  
  template<typename _Tp>
    struct __is_integer
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  
  
  
  template<>
    struct __is_integer<bool>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<char>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<signed char>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<unsigned char>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<wchar_t>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<char16_t>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<char32_t>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<short>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<unsigned short>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<int>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<unsigned int>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<long>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<unsigned long>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<long long>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<unsigned long long>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  
  
  
  template<typename _Tp>
    struct __is_floating
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  
  template<>
    struct __is_floating<float>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_floating<double>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_floating<long double>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  
  
  
  template<typename _Tp>
    struct __is_pointer
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  template<typename _Tp>
    struct __is_pointer<_Tp*>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  
  
  
  template<typename _Tp>
    struct __is_normal_iterator
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  template<typename _Iterator, typename _Container>
    struct __is_normal_iterator< __gnu_cxx::__normal_iterator<_Iterator,
							      _Container> >
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  
  
  
  template<typename _Tp>
    struct __is_arithmetic
    : public __traitor<__is_integer<_Tp>, __is_floating<_Tp> >
    { };

  
  
  
  template<typename _Tp>
    struct __is_fundamental
    : public __traitor<__is_void<_Tp>, __is_arithmetic<_Tp> >
    { };

  
  
  
  template<typename _Tp>
    struct __is_scalar
    : public __traitor<__is_arithmetic<_Tp>, __is_pointer<_Tp> >
    { };

  
  
  
  template<typename _Tp>
    struct __is_char
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  template<>
    struct __is_char<char>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_char<wchar_t>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<typename _Tp>
    struct __is_byte
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  template<>
    struct __is_byte<char>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_byte<signed char>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_byte<unsigned char>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  
  
  
  template<typename _Tp>
    struct __is_move_iterator
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  template<typename _Iterator>
    class move_iterator;

  template<typename _Iterator>
    struct __is_move_iterator< move_iterator<_Iterator> >
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };


} 



























 


#pragma GCC system_header


namespace __gnu_cxx 
{


  
  template<bool, typename>
    struct __enable_if 
    { };

  template<typename _Tp>
    struct __enable_if<true, _Tp>
    { typedef _Tp __type; };


  
  template<bool _Cond, typename _Iftrue, typename _Iffalse>
    struct __conditional_type
    { typedef _Iftrue __type; };

  template<typename _Iftrue, typename _Iffalse>
    struct __conditional_type<false, _Iftrue, _Iffalse>
    { typedef _Iffalse __type; };


  
  template<typename _Tp>
    struct __add_unsigned
    { 
    private:
      typedef __enable_if<std::__is_integer<_Tp>::__value, _Tp> __if_type;
      
    public:
      typedef typename __if_type::__type __type; 
    };

  template<>
    struct __add_unsigned<char>
    { typedef unsigned char __type; };

  template<>
    struct __add_unsigned<signed char>
    { typedef unsigned char __type; };

  template<>
    struct __add_unsigned<short>
    { typedef unsigned short __type; };

  template<>
    struct __add_unsigned<int>
    { typedef unsigned int __type; };

  template<>
    struct __add_unsigned<long>
    { typedef unsigned long __type; };

  template<>
    struct __add_unsigned<long long>
    { typedef unsigned long long __type; };

  
  template<>
    struct __add_unsigned<bool>;

  template<>
    struct __add_unsigned<wchar_t>;


  
  template<typename _Tp>
    struct __remove_unsigned
    { 
    private:
      typedef __enable_if<std::__is_integer<_Tp>::__value, _Tp> __if_type;
      
    public:
      typedef typename __if_type::__type __type; 
    };

  template<>
    struct __remove_unsigned<char>
    { typedef signed char __type; };

  template<>
    struct __remove_unsigned<unsigned char>
    { typedef signed char __type; };

  template<>
    struct __remove_unsigned<unsigned short>
    { typedef short __type; };

  template<>
    struct __remove_unsigned<unsigned int>
    { typedef int __type; };

  template<>
    struct __remove_unsigned<unsigned long>
    { typedef long __type; };

  template<>
    struct __remove_unsigned<unsigned long long>
    { typedef long long __type; };

  
  template<>
    struct __remove_unsigned<bool>;

  template<>
    struct __remove_unsigned<wchar_t>;


  
  template<typename _Type>
    inline bool
    __is_null_pointer(_Type* __ptr)
    { return __ptr == 0; }

  template<typename _Type>
    inline bool
    __is_null_pointer(_Type)
    { return false; }


  
  template<typename _Tp, bool = std::__is_integer<_Tp>::__value>
    struct __promote
    { typedef double __type; };

  
  
  
  template<typename _Tp>
    struct __promote<_Tp, false>
    { };

  template<>
    struct __promote<long double>
    { typedef long double __type; };

  template<>
    struct __promote<double>
    { typedef double __type; };

  template<>
    struct __promote<float>
    { typedef float __type; };

  template<typename _Tp, typename _Up,
           typename _Tp2 = typename __promote<_Tp>::__type,
           typename _Up2 = typename __promote<_Up>::__type>
    struct __promote_2
    {
      typedef __typeof__(_Tp2() + _Up2()) __type;
    };

  template<typename _Tp, typename _Up, typename _Vp,
           typename _Tp2 = typename __promote<_Tp>::__type,
           typename _Up2 = typename __promote<_Up>::__type,
           typename _Vp2 = typename __promote<_Vp>::__type>
    struct __promote_3
    {
      typedef __typeof__(_Tp2() + _Up2() + _Vp2()) __type;
    };

  template<typename _Tp, typename _Up, typename _Vp, typename _Wp,
           typename _Tp2 = typename __promote<_Tp>::__type,
           typename _Up2 = typename __promote<_Up>::__type,
           typename _Vp2 = typename __promote<_Vp>::__type,
           typename _Wp2 = typename __promote<_Wp>::__type>
    struct __promote_4
    {
      typedef __typeof__(_Tp2() + _Up2() + _Vp2() + _Wp2()) __type;
    };


} 



























 


#pragma GCC system_header


namespace __gnu_cxx 
{


  
  



  template<typename _Value>
    struct __numeric_traits_integer
    {
      
      static const _Value __min = (((_Value)(-1) < 0) ? (_Value)1 << (sizeof(_Value) * 8 - ((_Value)(-1) < 0)) : (_Value)0);
      static const _Value __max = (((_Value)(-1) < 0) ? (((((_Value)1 << ((sizeof(_Value) * 8 - ((_Value)(-1) < 0)) - 1)) - 1) << 1) + 1) : ~(_Value)0);

      
      
      static const bool __is_signed = ((_Value)(-1) < 0);
      static const int __digits = (sizeof(_Value) * 8 - ((_Value)(-1) < 0));      
    };

  template<typename _Value>
    const _Value __numeric_traits_integer<_Value>::__min;

  template<typename _Value>
    const _Value __numeric_traits_integer<_Value>::__max;

  template<typename _Value>
    const bool __numeric_traits_integer<_Value>::__is_signed;

  template<typename _Value>
    const int __numeric_traits_integer<_Value>::__digits;






  template<typename _Value>
    struct __numeric_traits_floating
    {
      
      static const int __max_digits10 = (2 + (std::__are_same<_Value, float> ::__value ? 24 : std::__are_same<_Value, double> ::__value ? 53 : 64) * 643L / 2136);

      
      static const bool __is_signed = true;
      static const int __digits10 = (std::__are_same<_Value, float> ::__value ? 6 : std::__are_same<_Value, double> ::__value ? 15 : 18);
      static const int __max_exponent10 = (std::__are_same<_Value, float> ::__value ? 38 : std::__are_same<_Value, double> ::__value ? 308 : 4932);
    };

  template<typename _Value>
    const int __numeric_traits_floating<_Value>::__max_digits10;

  template<typename _Value>
    const bool __numeric_traits_floating<_Value>::__is_signed;

  template<typename _Value>
    const int __numeric_traits_floating<_Value>::__digits10;

  template<typename _Value>
    const int __numeric_traits_floating<_Value>::__max_exponent10;

  template<typename _Value>
    struct __numeric_traits
    : public __conditional_type<std::__is_integer<_Value>::__value,
				__numeric_traits_integer<_Value>,
				__numeric_traits_floating<_Value> >::__type
    { };


} 


















































 




 





























 





























 


#pragma GCC system_header














namespace std 
{


  
  


 
  template<typename _Tp>
    inline _Tp*
    __addressof(_Tp& __r) noexcept
    {
      return reinterpret_cast<_Tp*>
	(&const_cast<char&>(reinterpret_cast<const volatile char&>(__r)));
    }


} 



























 


#pragma GCC system_header



namespace std 
{


  








 

  
  template<typename _Tp, _Tp __v>
    struct integral_constant
    {
      static constexpr _Tp                  value = __v;
      typedef _Tp                           value_type;
      typedef integral_constant<_Tp, __v>   type;
      constexpr operator value_type() { return value; }
    };
  
  template<typename _Tp, _Tp __v>
    constexpr _Tp integral_constant<_Tp, __v>::value;

  
  typedef integral_constant<bool, true>     true_type;

  
  typedef integral_constant<bool, false>    false_type;

  

  template<bool, typename, typename>
    struct conditional;

  template<typename...>
    struct __or_;

  template<>
    struct __or_<>
    : public false_type
    { };

  template<typename _B1>
    struct __or_<_B1>
    : public _B1
    { };

  template<typename _B1, typename _B2>
    struct __or_<_B1, _B2>
    : public conditional<_B1::value, _B1, _B2>::type
    { };

  template<typename _B1, typename _B2, typename _B3, typename... _Bn>
    struct __or_<_B1, _B2, _B3, _Bn...>
    : public conditional<_B1::value, _B1, __or_<_B2, _B3, _Bn...>>::type
    { };

  template<typename...>
    struct __and_;

  template<>
    struct __and_<>
    : public true_type
    { };

  template<typename _B1>
    struct __and_<_B1>
    : public _B1
    { };

  template<typename _B1, typename _B2>
    struct __and_<_B1, _B2>
    : public conditional<_B1::value, _B2, _B1>::type
    { };

  template<typename _B1, typename _B2, typename _B3, typename... _Bn>
    struct __and_<_B1, _B2, _B3, _Bn...>
    : public conditional<_B1::value, __and_<_B2, _B3, _Bn...>, _B1>::type
    { };

  template<typename _Pp>
    struct __not_
    : public integral_constant<bool, !_Pp::value>
    { };

  struct __sfinae_types
  {
    typedef char __one;
    typedef struct { char __arr[2]; } __two;
  };

  
  
  
  

  template<typename _Tp>
    struct __success_type
    { typedef _Tp type; };

  struct __failure_type
  { };

  

  template<typename>
    struct remove_cv;

  template<typename>
    struct __is_void_helper
    : public false_type { };

  template<>
    struct __is_void_helper<void>
    : public true_type { };

  
  template<typename _Tp>
    struct is_void
    : public integral_constant<bool, (__is_void_helper<typename
				      remove_cv<_Tp>::type>::value)>
    { };

  template<typename>
    struct __is_integral_helper
    : public false_type { };

  template<>
    struct __is_integral_helper<bool>
    : public true_type { };
  
  template<>
    struct __is_integral_helper<char>
    : public true_type { };

  template<>
    struct __is_integral_helper<signed char>
    : public true_type { };

  template<>
    struct __is_integral_helper<unsigned char>
    : public true_type { };

  template<>
    struct __is_integral_helper<wchar_t>
    : public true_type { };

  template<>
    struct __is_integral_helper<char16_t>
    : public true_type { };

  template<>
    struct __is_integral_helper<char32_t>
    : public true_type { };

  template<>
    struct __is_integral_helper<short>
    : public true_type { };

  template<>
    struct __is_integral_helper<unsigned short>
    : public true_type { };

  template<>
    struct __is_integral_helper<int>
    : public true_type { };

  template<>
    struct __is_integral_helper<unsigned int>
    : public true_type { };

  template<>
    struct __is_integral_helper<long>
    : public true_type { };

  template<>
    struct __is_integral_helper<unsigned long>
    : public true_type { };

  template<>
    struct __is_integral_helper<long long>
    : public true_type { };

  template<>
    struct __is_integral_helper<unsigned long long>
    : public true_type { };

  template<>
    struct __is_integral_helper<__int128>
    : public true_type { };

  template<>
    struct __is_integral_helper<unsigned __int128>
    : public true_type { };

  
  template<typename _Tp>
    struct is_integral
    : public integral_constant<bool, (__is_integral_helper<typename
				      remove_cv<_Tp>::type>::value)>
    { };

  template<typename>
    struct __is_floating_point_helper
    : public false_type { };

  template<>
    struct __is_floating_point_helper<float>
    : public true_type { };

  template<>
    struct __is_floating_point_helper<double>
    : public true_type { };

  template<>
    struct __is_floating_point_helper<long double>
    : public true_type { };


  
  template<typename _Tp>
    struct is_floating_point
    : public integral_constant<bool, (__is_floating_point_helper<typename
				      remove_cv<_Tp>::type>::value)>
    { };

  
  template<typename>
    struct is_array
    : public false_type { };

  template<typename _Tp, std::size_t _Size>
    struct is_array<_Tp[_Size]>
    : public true_type { };

  template<typename _Tp>
    struct is_array<_Tp[]>
    : public true_type { };

  template<typename>
    struct __is_pointer_helper
    : public false_type { };

  template<typename _Tp>
    struct __is_pointer_helper<_Tp*>
    : public true_type { };

  
  template<typename _Tp>
    struct is_pointer
    : public integral_constant<bool, (__is_pointer_helper<typename
				      remove_cv<_Tp>::type>::value)>
    { };

  
  template<typename>
    struct is_lvalue_reference
    : public false_type { };

  template<typename _Tp>
    struct is_lvalue_reference<_Tp&>
    : public true_type { };

  
  template<typename>
    struct is_rvalue_reference
    : public false_type { };

  template<typename _Tp>
    struct is_rvalue_reference<_Tp&&>
    : public true_type { };

  template<typename>
    struct is_function;

  template<typename>
    struct __is_member_object_pointer_helper
    : public false_type { };

  template<typename _Tp, typename _Cp>
    struct __is_member_object_pointer_helper<_Tp _Cp::*>
    : public integral_constant<bool, !is_function<_Tp>::value> { };

  
  template<typename _Tp>
    struct is_member_object_pointer
    : public integral_constant<bool, (__is_member_object_pointer_helper<
				      typename remove_cv<_Tp>::type>::value)>
    { };

  template<typename>
    struct __is_member_function_pointer_helper
    : public false_type { };

  template<typename _Tp, typename _Cp>
    struct __is_member_function_pointer_helper<_Tp _Cp::*>
    : public integral_constant<bool, is_function<_Tp>::value> { };

  
  template<typename _Tp>
    struct is_member_function_pointer
    : public integral_constant<bool, (__is_member_function_pointer_helper<
				      typename remove_cv<_Tp>::type>::value)>
    { };

  
  template<typename _Tp>
    struct is_enum
    : public integral_constant<bool, __is_enum(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_union
    : public integral_constant<bool, __is_union(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_class
    : public integral_constant<bool, __is_class(_Tp)>
    { };

  
  template<typename>
    struct is_function
    : public false_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...)>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......)>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) const>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) const>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) volatile>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) volatile>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) const volatile>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) const volatile>
    : public true_type { };

  template<typename>
    struct __is_nullptr_t_helper
    : public false_type { };

  template<>
    struct __is_nullptr_t_helper<std::nullptr_t>
    : public true_type { };

  
  template<typename _Tp>
    struct __is_nullptr_t
    : public integral_constant<bool, (__is_nullptr_t_helper<typename
				      remove_cv<_Tp>::type>::value)>
    { };

  

  
  template<typename _Tp>
    struct is_reference
    : public __or_<is_lvalue_reference<_Tp>,
                   is_rvalue_reference<_Tp>>::type
    { };

  
  template<typename _Tp>
    struct is_arithmetic
    : public __or_<is_integral<_Tp>, is_floating_point<_Tp>>::type
    { };

  
  template<typename _Tp>
    struct is_fundamental
    : public __or_<is_arithmetic<_Tp>, is_void<_Tp>, __is_nullptr_t<_Tp>>::type
    { };

  
  template<typename _Tp>
    struct is_object
    : public __not_<__or_<is_function<_Tp>, is_reference<_Tp>,
                          is_void<_Tp>>>::type
    { };

#if 0
  template<typename>
    struct is_member_pointer;

  
  template<typename _Tp>
    struct is_scalar
    : public __or_<is_arithmetic<_Tp>, is_enum<_Tp>, is_pointer<_Tp>,
                   is_member_pointer<_Tp>, __is_nullptr_t<_Tp>>::type
    { };

  
  template<typename _Tp>
    struct is_compound
    : public integral_constant<bool, !is_fundamental<_Tp>::value> { };

  template<typename _Tp>
    struct __is_member_pointer_helper
    : public false_type { };

  template<typename _Tp, typename _Cp>
    struct __is_member_pointer_helper<_Tp _Cp::*>
    : public true_type { };

  
  template<typename _Tp>
    struct is_member_pointer
    : public integral_constant<bool, (__is_member_pointer_helper<
				      typename remove_cv<_Tp>::type>::value)>
    { };

  

  
  template<typename>
    struct is_const
    : public false_type { };

  template<typename _Tp>
    struct is_const<_Tp const>
    : public true_type { };
  
  
  template<typename>
    struct is_volatile
    : public false_type { };

  template<typename _Tp>
    struct is_volatile<_Tp volatile>
    : public true_type { };

  
  template<typename _Tp>
    struct is_trivial
    : public integral_constant<bool, __is_trivial(_Tp)>
    { };

  

  
  template<typename _Tp>
    struct is_standard_layout
    : public integral_constant<bool, __is_standard_layout(_Tp)>
    { };

  
  
  template<typename _Tp>
    struct is_pod
    : public integral_constant<bool, __is_pod(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_literal_type
    : public integral_constant<bool, __is_literal_type(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_empty
    : public integral_constant<bool, __is_empty(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_polymorphic
    : public integral_constant<bool, __is_polymorphic(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_abstract
    : public integral_constant<bool, __is_abstract(_Tp)>
    { };

  template<typename _Tp,
	   bool = is_integral<_Tp>::value,
	   bool = is_floating_point<_Tp>::value>
    struct __is_signed_helper
    : public false_type { };

  template<typename _Tp>
    struct __is_signed_helper<_Tp, false, true>
    : public true_type { };

  template<typename _Tp>
    struct __is_signed_helper<_Tp, true, false>
    : public integral_constant<bool, static_cast<bool>(_Tp(-1) < _Tp(0))>
    { };

  
  template<typename _Tp>
    struct is_signed
    : public integral_constant<bool, __is_signed_helper<_Tp>::value>
    { };

  
  template<typename _Tp>
    struct is_unsigned
    : public __and_<is_arithmetic<_Tp>, __not_<is_signed<_Tp>>>::type
    { };


  

  template<typename>
    struct add_rvalue_reference;

  


 
  template<typename _Tp>
    typename add_rvalue_reference<_Tp>::type declval() noexcept;

  template<typename, unsigned = 0>
    struct extent;

  template<typename>
    struct remove_all_extents;

  template<typename _Tp>
    struct __is_array_known_bounds
    : public integral_constant<bool, (extent<_Tp>::value > 0)>
    { };

  template<typename _Tp>
    struct __is_array_unknown_bounds
    : public __and_<is_array<_Tp>, __not_<extent<_Tp>>>::type
    { };
    
  
  
  
  
  
  struct __do_is_destructible_impl
  {
    template<typename _Tp, typename = decltype(declval<_Tp&>().~_Tp())>
      static true_type __test(int);

    template<typename>
      static false_type __test(...);
  };

  template<typename _Tp>
    struct __is_destructible_impl
    : public __do_is_destructible_impl
    {
      typedef decltype(__test<_Tp>(0)) type;
    };

  template<typename _Tp,
           bool = __or_<is_void<_Tp>,
                        __is_array_unknown_bounds<_Tp>,
                        is_function<_Tp>>::value,
           bool = __or_<is_reference<_Tp>, is_scalar<_Tp>>::value>
    struct __is_destructible_safe;

  template<typename _Tp>
    struct __is_destructible_safe<_Tp, false, false>
    : public __is_destructible_impl<typename
               remove_all_extents<_Tp>::type>::type
    { };

  template<typename _Tp>
    struct __is_destructible_safe<_Tp, true, false>
    : public false_type { };

  template<typename _Tp>
    struct __is_destructible_safe<_Tp, false, true>
    : public true_type { };

  
  template<typename _Tp>
    struct is_destructible
    : public integral_constant<bool, (__is_destructible_safe<_Tp>::value)>
    { };

  
  
  
  
  struct __do_is_nt_destructible_impl
  {
    template<typename _Tp>
      static integral_constant<bool, noexcept(declval<_Tp&>().~_Tp())>
        __test(int);

    template<typename>
      static false_type __test(...);
  };

  template<typename _Tp>
    struct __is_nt_destructible_impl
    : public __do_is_nt_destructible_impl
    {
      typedef decltype(__test<_Tp>(0)) type;
    };

  template<typename _Tp,
           bool = __or_<is_void<_Tp>,
                        __is_array_unknown_bounds<_Tp>,
                        is_function<_Tp>>::value,
           bool = __or_<is_reference<_Tp>, is_scalar<_Tp>>::value>
    struct __is_nt_destructible_safe;

  template<typename _Tp>
    struct __is_nt_destructible_safe<_Tp, false, false>
    : public __is_nt_destructible_impl<typename
               remove_all_extents<_Tp>::type>::type
    { };

  template<typename _Tp>
    struct __is_nt_destructible_safe<_Tp, true, false>
    : public false_type { };

  template<typename _Tp>
    struct __is_nt_destructible_safe<_Tp, false, true>
    : public true_type { };

  
  template<typename _Tp>
    struct is_nothrow_destructible
    : public integral_constant<bool, (__is_nt_destructible_safe<_Tp>::value)>
    { };

  struct __do_is_default_constructible_impl
  {
    template<typename _Tp, typename = decltype(_Tp())>
      static true_type __test(int);

    template<typename>
      static false_type __test(...);
  };

  template<typename _Tp>
    struct __is_default_constructible_impl
    : public __do_is_default_constructible_impl
    {
      typedef decltype(__test<_Tp>(0)) type;
    };

  template<typename _Tp>
    struct __is_default_constructible_atom
    : public __and_<__not_<is_void<_Tp>>,
                    __is_default_constructible_impl<_Tp>>::type
    { };

  template<typename _Tp, bool = is_array<_Tp>::value>
    struct __is_default_constructible_safe;

  
  
  
  
  
  template<typename _Tp>
    struct __is_default_constructible_safe<_Tp, true>
    : public __and_<__is_array_known_bounds<_Tp>,
		    __is_default_constructible_atom<typename
                      remove_all_extents<_Tp>::type>>::type
    { };

  template<typename _Tp>
    struct __is_default_constructible_safe<_Tp, false>
    : public __is_default_constructible_atom<_Tp>::type
    { };

  
  template<typename _Tp>
    struct is_default_constructible
    : public integral_constant<bool, (__is_default_constructible_safe<
				      _Tp>::value)>
    { };


  

  
  
  
  
  
  
  
  
  
  struct __do_is_static_castable_impl
  {
    template<typename _From, typename _To, typename
             = decltype(static_cast<_To>(declval<_From>()))>
      static true_type __test(int);

    template<typename, typename>
      static false_type __test(...);
  };

  template<typename _From, typename _To>
    struct __is_static_castable_impl
    : public __do_is_static_castable_impl
    {
      typedef decltype(__test<_From, _To>(0)) type;
    };

  template<typename _From, typename _To>
    struct __is_static_castable_safe
    : public __is_static_castable_impl<_From, _To>::type
    { };

  
  template<typename _From, typename _To>
    struct __is_static_castable
    : public integral_constant<bool, (__is_static_castable_safe<
				      _From, _To>::value)>
    { };

  
  
  
  
  
  struct __do_is_direct_constructible_impl
  {
    template<typename _Tp, typename _Arg, typename
	     = decltype(::new _Tp(declval<_Arg>()))>
      static true_type __test(int);

    template<typename, typename>
      static false_type __test(...);
  };

  template<typename _Tp, typename _Arg>
    struct __is_direct_constructible_impl
    : public __do_is_direct_constructible_impl
    {
      typedef decltype(__test<_Tp, _Arg>(0)) type;
    };

  template<typename _Tp, typename _Arg>
    struct __is_direct_constructible_new_safe
    : public __and_<is_destructible<_Tp>,
                    __is_direct_constructible_impl<_Tp, _Arg>>::type
    { };

  template<typename, typename>
    struct is_same;

  template<typename, typename>
    struct is_base_of;

  template<typename>
    struct remove_reference;

  template<typename _From, typename _To, bool
           = __not_<__or_<is_void<_From>, 
                          is_function<_From>>>::value>
    struct __is_base_to_derived_ref;

  
  
  template<typename _From, typename _To>
    struct __is_base_to_derived_ref<_From, _To, true>
    {
      typedef typename remove_cv<typename remove_reference<_From
        >::type>::type __src_t;
      typedef typename remove_cv<typename remove_reference<_To
        >::type>::type __dst_t;
      typedef __and_<__not_<is_same<__src_t, __dst_t>>,
		     is_base_of<__src_t, __dst_t>> type;
      static constexpr bool value = type::value;
    };

  template<typename _From, typename _To>
    struct __is_base_to_derived_ref<_From, _To, false>
    : public false_type
    { };

  template<typename _From, typename _To, bool
           = __and_<is_lvalue_reference<_From>,
                    is_rvalue_reference<_To>>::value>
    struct __is_lvalue_to_rvalue_ref;

  
  
  template<typename _From, typename _To>
    struct __is_lvalue_to_rvalue_ref<_From, _To, true>
    {
      typedef typename remove_cv<typename remove_reference<
        _From>::type>::type __src_t;
      typedef typename remove_cv<typename remove_reference<
        _To>::type>::type __dst_t;
      typedef __and_<__not_<is_function<__src_t>>, 
        __or_<is_same<__src_t, __dst_t>,
		    is_base_of<__dst_t, __src_t>>> type;
      static constexpr bool value = type::value;
    };

  template<typename _From, typename _To>
    struct __is_lvalue_to_rvalue_ref<_From, _To, false>
    : public false_type
    { };

  
  
  
  
  
  
  template<typename _Tp, typename _Arg>
    struct __is_direct_constructible_ref_cast
    : public __and_<__is_static_castable<_Arg, _Tp>,
                    __not_<__or_<__is_base_to_derived_ref<_Arg, _Tp>,
                                 __is_lvalue_to_rvalue_ref<_Arg, _Tp>
                   >>>::type
    { };

  template<typename _Tp, typename _Arg>
    struct __is_direct_constructible_new
    : public conditional<is_reference<_Tp>::value,
			 __is_direct_constructible_ref_cast<_Tp, _Arg>,
			 __is_direct_constructible_new_safe<_Tp, _Arg>
			 >::type
    { };

  template<typename _Tp, typename _Arg>
    struct __is_direct_constructible
    : public integral_constant<bool, (__is_direct_constructible_new<
				      _Tp, _Arg>::value)>
    { };

  
  
  
  
  
  struct __do_is_nary_constructible_impl
  {
    template<typename _Tp, typename... _Args, typename
             = decltype(_Tp(declval<_Args>()...))>
      static true_type __test(int);

    template<typename, typename...>
      static false_type __test(...);
  };

  template<typename _Tp, typename... _Args>
    struct __is_nary_constructible_impl
    : public __do_is_nary_constructible_impl
    {
      typedef decltype(__test<_Tp, _Args...>(0)) type;
    };

  template<typename _Tp, typename... _Args>
    struct __is_nary_constructible
    : public __is_nary_constructible_impl<_Tp, _Args...>::type
    {
      static_assert(sizeof...(_Args) > 1,
                    "Only useful for > 1 arguments");
    };

  template<typename _Tp, typename... _Args>
    struct __is_constructible_impl
    : public __is_nary_constructible<_Tp, _Args...>
    { };

  template<typename _Tp, typename _Arg>
    struct __is_constructible_impl<_Tp, _Arg>
    : public __is_direct_constructible<_Tp, _Arg>
    { };

  template<typename _Tp>
    struct __is_constructible_impl<_Tp>
    : public is_default_constructible<_Tp>
    { };

  
  template<typename _Tp, typename... _Args>
    struct is_constructible
    : public integral_constant<bool, (__is_constructible_impl<_Tp,
				      _Args...>::value)>
    { };

  template<typename _Tp, bool = is_void<_Tp>::value>
    struct __is_copy_constructible_impl;

  template<typename _Tp>
    struct __is_copy_constructible_impl<_Tp, true>
    : public false_type { };

  template<typename _Tp>
    struct __is_copy_constructible_impl<_Tp, false>
    : public is_constructible<_Tp, const _Tp&>
    { };

  
  template<typename _Tp>
    struct is_copy_constructible
    : public __is_copy_constructible_impl<_Tp>
    { };

  template<typename _Tp, bool = is_void<_Tp>::value>
    struct __is_move_constructible_impl;

  template<typename _Tp>
    struct __is_move_constructible_impl<_Tp, true>
    : public false_type { };

  template<typename _Tp>
    struct __is_move_constructible_impl<_Tp, false>
    : public is_constructible<_Tp, _Tp&&>
    { };

  
  template<typename _Tp>
    struct is_move_constructible
    : public __is_move_constructible_impl<_Tp>
    { };

  template<typename _Tp>
    struct __is_nt_default_constructible_atom
    : public integral_constant<bool, noexcept(_Tp())>
    { };

  template<typename _Tp, bool = is_array<_Tp>::value>
    struct __is_nt_default_constructible_impl;

  template<typename _Tp>
    struct __is_nt_default_constructible_impl<_Tp, true>
    : public __and_<__is_array_known_bounds<_Tp>,
		    __is_nt_default_constructible_atom<typename
                      remove_all_extents<_Tp>::type>>::type
    { };

  template<typename _Tp>
    struct __is_nt_default_constructible_impl<_Tp, false>
    : public __is_nt_default_constructible_atom<_Tp>
    { };

  
  template<typename _Tp>
    struct is_nothrow_default_constructible
    : public __and_<is_default_constructible<_Tp>,
                    __is_nt_default_constructible_impl<_Tp>>::type
    { };

  template<typename _Tp, typename... _Args>
    struct __is_nt_constructible_impl
    : public integral_constant<bool, noexcept(_Tp(declval<_Args>()...))>
    { };

  template<typename _Tp, typename _Arg>
    struct __is_nt_constructible_impl<_Tp, _Arg>
    : public integral_constant<bool,
                               noexcept(static_cast<_Tp>(declval<_Arg>()))>
    { };

  template<typename _Tp>
    struct __is_nt_constructible_impl<_Tp>
    : public is_nothrow_default_constructible<_Tp>
    { };

  
  template<typename _Tp, typename... _Args>
    struct is_nothrow_constructible
    : public __and_<is_constructible<_Tp, _Args...>,
		    __is_nt_constructible_impl<_Tp, _Args...>>::type
    { };

  template<typename _Tp, bool = is_void<_Tp>::value>
    struct __is_nothrow_copy_constructible_impl;

  template<typename _Tp>
    struct __is_nothrow_copy_constructible_impl<_Tp, true>
    : public false_type { };

  template<typename _Tp>
    struct __is_nothrow_copy_constructible_impl<_Tp, false>
    : public is_nothrow_constructible<_Tp, const _Tp&>
    { };

  
  template<typename _Tp>
    struct is_nothrow_copy_constructible
    : public __is_nothrow_copy_constructible_impl<_Tp>
    { };

  template<typename _Tp, bool = is_void<_Tp>::value>
    struct __is_nothrow_move_constructible_impl;

  template<typename _Tp>
    struct __is_nothrow_move_constructible_impl<_Tp, true>
    : public false_type { };

  template<typename _Tp>
    struct __is_nothrow_move_constructible_impl<_Tp, false>
    : public is_nothrow_constructible<_Tp, _Tp&&>
    { };

  
  template<typename _Tp>
    struct is_nothrow_move_constructible
    : public __is_nothrow_move_constructible_impl<_Tp>
    { };

  template<typename _Tp, typename _Up>
    class __is_assignable_helper
    : public __sfinae_types
    {
      template<typename _Tp1, typename _Up1>
        static decltype(declval<_Tp1>() = declval<_Up1>(), __one())
	__test(int);

      template<typename, typename>
        static __two __test(...);

    public:
      static constexpr bool value = sizeof(__test<_Tp, _Up>(0)) == 1;
    };

  
  template<typename _Tp, typename _Up>
    struct is_assignable
    : public integral_constant<bool,
                               __is_assignable_helper<_Tp, _Up>::value>
    { };

  template<typename _Tp, bool = is_void<_Tp>::value>
    struct __is_copy_assignable_impl;

  template<typename _Tp>
    struct __is_copy_assignable_impl<_Tp, true>
    : public false_type { };

  template<typename _Tp>
    struct __is_copy_assignable_impl<_Tp, false>
    : public is_assignable<_Tp&, const _Tp&>
    { };

  
  template<typename _Tp>
    struct is_copy_assignable
    : public __is_copy_assignable_impl<_Tp>
    { };

  template<typename _Tp, bool = is_void<_Tp>::value>
    struct __is_move_assignable_impl;

  template<typename _Tp>
    struct __is_move_assignable_impl<_Tp, true>
    : public false_type { };

  template<typename _Tp>
    struct __is_move_assignable_impl<_Tp, false>
    : public is_assignable<_Tp&, _Tp&&>
    { };

  
  template<typename _Tp>
    struct is_move_assignable
    : public __is_move_assignable_impl<_Tp>
    { };

  template<typename _Tp, typename _Up>
    struct __is_nt_assignable_impl
    : public integral_constant<bool, noexcept(declval<_Tp>() = declval<_Up>())>
    { };

  
  template<typename _Tp, typename _Up>
    struct is_nothrow_assignable
    : public __and_<is_assignable<_Tp, _Up>,
		    __is_nt_assignable_impl<_Tp, _Up>>::type
    { };

  template<typename _Tp, bool = is_void<_Tp>::value>
    struct __is_nt_copy_assignable_impl;

  template<typename _Tp>
    struct __is_nt_copy_assignable_impl<_Tp, true>
    : public false_type { };

  template<typename _Tp>
    struct __is_nt_copy_assignable_impl<_Tp, false>
    : public is_nothrow_assignable<_Tp&, const _Tp&>
    { };

  
  template<typename _Tp>
    struct is_nothrow_copy_assignable
    : public __is_nt_copy_assignable_impl<_Tp>
    { };

  template<typename _Tp, bool = is_void<_Tp>::value>
    struct __is_nt_move_assignable_impl;

  template<typename _Tp>
    struct __is_nt_move_assignable_impl<_Tp, true>
    : public false_type { };

  template<typename _Tp>
    struct __is_nt_move_assignable_impl<_Tp, false>
    : public is_nothrow_assignable<_Tp&, _Tp&&>
    { };

  
  template<typename _Tp>
    struct is_nothrow_move_assignable
    : public __is_nt_move_assignable_impl<_Tp>
    { };

  
  
  

  

  

  

  

  

  
  template<typename _Tp>
    struct is_trivially_destructible
    : public __and_<is_destructible<_Tp>, integral_constant<bool,
			      __has_trivial_destructor(_Tp)>>::type
    { };

  
  template<typename _Tp>
    struct has_trivial_default_constructor
    : public integral_constant<bool, __has_trivial_constructor(_Tp)>
    { };

  
  template<typename _Tp>
    struct has_trivial_copy_constructor
    : public integral_constant<bool, __has_trivial_copy(_Tp)>
    { };

  
  template<typename _Tp>
    struct has_trivial_copy_assign
    : public integral_constant<bool, __has_trivial_assign(_Tp)>
    { };

  
  template<typename _Tp>
    struct has_virtual_destructor
    : public integral_constant<bool, __has_virtual_destructor(_Tp)>
    { };

  
  

  
  template<typename _Tp>
    struct alignment_of
    : public integral_constant<std::size_t, __alignof__(_Tp)> { };
  
  
  template<typename>
    struct rank
    : public integral_constant<std::size_t, 0> { };
   
  template<typename _Tp, std::size_t _Size>
    struct rank<_Tp[_Size]>
    : public integral_constant<std::size_t, 1 + rank<_Tp>::value> { };

  template<typename _Tp>
    struct rank<_Tp[]>
    : public integral_constant<std::size_t, 1 + rank<_Tp>::value> { };

  
  template<typename, unsigned _Uint>
    struct extent
    : public integral_constant<std::size_t, 0> { };
  
  template<typename _Tp, unsigned _Uint, std::size_t _Size>
    struct extent<_Tp[_Size], _Uint>
    : public integral_constant<std::size_t,
			       _Uint == 0 ? _Size : extent<_Tp,
							   _Uint - 1>::value>
    { };

  template<typename _Tp, unsigned _Uint>
    struct extent<_Tp[], _Uint>
    : public integral_constant<std::size_t,
			       _Uint == 0 ? 0 : extent<_Tp,
						       _Uint - 1>::value>
    { };


  

  
  template<typename, typename>
    struct is_same
    : public false_type { };

  template<typename _Tp>
    struct is_same<_Tp, _Tp>
    : public true_type { };

  
  template<typename _Base, typename _Derived>
    struct is_base_of
    : public integral_constant<bool, __is_base_of(_Base, _Derived)>
    { };

  template<typename _From, typename _To,
           bool = __or_<is_void<_From>, is_function<_To>,
                        is_array<_To>>::value>
    struct __is_convertible_helper
    { static constexpr bool value = is_void<_To>::value; };

  template<typename _From, typename _To>
    class __is_convertible_helper<_From, _To, false>
    : public __sfinae_types
    {
      template<typename _To1>
        static void __test_aux(_To1);

      template<typename _From1, typename _To1>
        static decltype(__test_aux<_To1>(std::declval<_From1>()), __one())
	__test(int);

      template<typename, typename>
        static __two __test(...);

    public:
      static constexpr bool value = sizeof(__test<_From, _To>(0)) == 1;
    };

  
  template<typename _From, typename _To>
    struct is_convertible
    : public integral_constant<bool,
			       __is_convertible_helper<_From, _To>::value>
    { };


  

  
  template<typename _Tp>
    struct remove_const
    { typedef _Tp     type; };

  template<typename _Tp>
    struct remove_const<_Tp const>
    { typedef _Tp     type; };
  
  
  template<typename _Tp>
    struct remove_volatile
    { typedef _Tp     type; };

  template<typename _Tp>
    struct remove_volatile<_Tp volatile>
    { typedef _Tp     type; };
  
  
  template<typename _Tp>
    struct remove_cv
    {
      typedef typename
      remove_const<typename remove_volatile<_Tp>::type>::type     type;
    };
  
  
  template<typename _Tp>
    struct add_const
    { typedef _Tp const     type; };
   
  
  template<typename _Tp>
    struct add_volatile
    { typedef _Tp volatile     type; };
  
  
  template<typename _Tp>
    struct add_cv
    {
      typedef typename
      add_const<typename add_volatile<_Tp>::type>::type     type;
    };


  

  
  template<typename _Tp>
    struct remove_reference
    { typedef _Tp   type; };

  template<typename _Tp>
    struct remove_reference<_Tp&>
    { typedef _Tp   type; };

  template<typename _Tp>
    struct remove_reference<_Tp&&>
    { typedef _Tp   type; };

  template<typename _Tp,
	   bool = __and_<__not_<is_reference<_Tp>>,
                         __not_<is_void<_Tp>>>::value,
	   bool = is_rvalue_reference<_Tp>::value>
    struct __add_lvalue_reference_helper
    { typedef _Tp   type; };

  template<typename _Tp>
    struct __add_lvalue_reference_helper<_Tp, true, false>
    { typedef _Tp&   type; };

  template<typename _Tp>
    struct __add_lvalue_reference_helper<_Tp, false, true>
    { typedef typename remove_reference<_Tp>::type&   type; };

  
  template<typename _Tp>
    struct add_lvalue_reference
    : public __add_lvalue_reference_helper<_Tp>
    { };

  template<typename _Tp,
           bool = __and_<__not_<is_reference<_Tp>>,
                         __not_<is_void<_Tp>>>::value>
    struct __add_rvalue_reference_helper
    { typedef _Tp   type; };

  template<typename _Tp>
    struct __add_rvalue_reference_helper<_Tp, true>
    { typedef _Tp&&   type; };

  
  template<typename _Tp>
    struct add_rvalue_reference
    : public __add_rvalue_reference_helper<_Tp>
    { };


  

  
  template<typename _Unqualified, bool _IsConst, bool _IsVol>
    struct __cv_selector;

  template<typename _Unqualified>
    struct __cv_selector<_Unqualified, false, false>
    { typedef _Unqualified __type; };

  template<typename _Unqualified>
    struct __cv_selector<_Unqualified, false, true>
    { typedef volatile _Unqualified __type; };

  template<typename _Unqualified>
    struct __cv_selector<_Unqualified, true, false>
    { typedef const _Unqualified __type; };

  template<typename _Unqualified>
    struct __cv_selector<_Unqualified, true, true>
    { typedef const volatile _Unqualified __type; };

  template<typename _Qualified, typename _Unqualified,
	   bool _IsConst = is_const<_Qualified>::value,
	   bool _IsVol = is_volatile<_Qualified>::value>
    class __match_cv_qualifiers
    {
      typedef __cv_selector<_Unqualified, _IsConst, _IsVol> __match;

    public:
      typedef typename __match::__type __type; 
    };

  
  template<typename _Tp>
    struct __make_unsigned
    { typedef _Tp __type; };

  template<>
    struct __make_unsigned<char>
    { typedef unsigned char __type; };

  template<>
    struct __make_unsigned<signed char>
    { typedef unsigned char __type; };

  template<>
    struct __make_unsigned<short>
    { typedef unsigned short __type; };

  template<>
    struct __make_unsigned<int>
    { typedef unsigned int __type; };

  template<>
    struct __make_unsigned<long>
    { typedef unsigned long __type; };

  template<>
    struct __make_unsigned<long long>
    { typedef unsigned long long __type; };

  template<>
    struct __make_unsigned<__int128>
    { typedef unsigned __int128 __type; };

  
  template<typename _Tp, 
	   bool _IsInt = is_integral<_Tp>::value,
	   bool _IsEnum = is_enum<_Tp>::value>
    class __make_unsigned_selector;

  template<typename _Tp>
    class __make_unsigned_selector<_Tp, true, false>
    {
      typedef __make_unsigned<typename remove_cv<_Tp>::type> __unsignedt;
      typedef typename __unsignedt::__type __unsigned_type;
      typedef __match_cv_qualifiers<_Tp, __unsigned_type> __cv_unsigned;

    public:
      typedef typename __cv_unsigned::__type __type;
    };

  template<typename _Tp>
    class __make_unsigned_selector<_Tp, false, true>
    {
      
      typedef unsigned char __smallest;
      static const bool __b0 = sizeof(_Tp) <= sizeof(__smallest);
      static const bool __b1 = sizeof(_Tp) <= sizeof(unsigned short);
      static const bool __b2 = sizeof(_Tp) <= sizeof(unsigned int);
      typedef conditional<__b2, unsigned int, unsigned long> __cond2;
      typedef typename __cond2::type __cond2_type;
      typedef conditional<__b1, unsigned short, __cond2_type> __cond1;
      typedef typename __cond1::type __cond1_type;

    public:
      typedef typename conditional<__b0, __smallest, __cond1_type>::type __type;
    };

  
  
  
  
  template<typename _Tp>
    struct make_unsigned 
    { typedef typename __make_unsigned_selector<_Tp>::__type type; };

  
  template<>
    struct make_unsigned<bool>;


  
  template<typename _Tp>
    struct __make_signed
    { typedef _Tp __type; };

  template<>
    struct __make_signed<char>
    { typedef signed char __type; };

  template<>
    struct __make_signed<unsigned char>
    { typedef signed char __type; };

  template<>
    struct __make_signed<unsigned short>
    { typedef signed short __type; };

  template<>
    struct __make_signed<unsigned int>
    { typedef signed int __type; };

  template<>
    struct __make_signed<unsigned long>
    { typedef signed long __type; };

  template<>
    struct __make_signed<unsigned long long>
    { typedef signed long long __type; };

  template<>
    struct __make_signed<unsigned __int128>
    { typedef __int128 __type; };

  
  template<typename _Tp, 
	   bool _IsInt = is_integral<_Tp>::value,
	   bool _IsEnum = is_enum<_Tp>::value>
    class __make_signed_selector;

  template<typename _Tp>
    class __make_signed_selector<_Tp, true, false>
    {
      typedef __make_signed<typename remove_cv<_Tp>::type> __signedt;
      typedef typename __signedt::__type __signed_type;
      typedef __match_cv_qualifiers<_Tp, __signed_type> __cv_signed;

    public:
      typedef typename __cv_signed::__type __type;
    };

  template<typename _Tp>
    class __make_signed_selector<_Tp, false, true>
    {
      
      typedef signed char __smallest;
      static const bool __b0 = sizeof(_Tp) <= sizeof(__smallest);
      static const bool __b1 = sizeof(_Tp) <= sizeof(signed short);
      static const bool __b2 = sizeof(_Tp) <= sizeof(signed int);
      typedef conditional<__b2, signed int, signed long> __cond2;
      typedef typename __cond2::type __cond2_type;
      typedef conditional<__b1, signed short, __cond2_type> __cond1;
      typedef typename __cond1::type __cond1_type;

    public:
      typedef typename conditional<__b0, __smallest, __cond1_type>::type __type;
    };

  
  
  
  
  template<typename _Tp>
    struct make_signed 
    { typedef typename __make_signed_selector<_Tp>::__type type; };

  
  template<>
    struct make_signed<bool>;


  

  
  template<typename _Tp>
    struct remove_extent
    { typedef _Tp     type; };

  template<typename _Tp, std::size_t _Size>
    struct remove_extent<_Tp[_Size]>
    { typedef _Tp     type; };

  template<typename _Tp>
    struct remove_extent<_Tp[]>
    { typedef _Tp     type; };

  
  template<typename _Tp>
    struct remove_all_extents
    { typedef _Tp     type; };

  template<typename _Tp, std::size_t _Size>
    struct remove_all_extents<_Tp[_Size]>
    { typedef typename remove_all_extents<_Tp>::type     type; };

  template<typename _Tp>
    struct remove_all_extents<_Tp[]>
    { typedef typename remove_all_extents<_Tp>::type     type; };


  

  template<typename _Tp, typename>
    struct __remove_pointer_helper
    { typedef _Tp     type; };

  template<typename _Tp, typename _Up>
    struct __remove_pointer_helper<_Tp, _Up*>
    { typedef _Up     type; };

  
  template<typename _Tp>
    struct remove_pointer
    : public __remove_pointer_helper<_Tp, typename remove_cv<_Tp>::type>
    { };

  
  template<typename _Tp>
    struct add_pointer
    { typedef typename remove_reference<_Tp>::type*     type; };


  template<std::size_t _Len>
    struct __aligned_storage_msa
    { 
      union __type
      {
	unsigned char __data[_Len];
	struct __attribute__((__aligned__)) { } __align; 
      };
    };

  








 
  template<std::size_t _Len, std::size_t _Align =
	   __alignof__(typename __aligned_storage_msa<_Len>::__type)>
    struct aligned_storage
    { 
      union type
      {
	unsigned char __data[_Len];
	struct __attribute__((__aligned__((_Align)))) { } __align; 
      };
    };


  
  
  template<typename _Up, 
	   bool _IsArray = is_array<_Up>::value,
	   bool _IsFunction = is_function<_Up>::value> 
    struct __decay_selector;

  
  template<typename _Up> 
    struct __decay_selector<_Up, false, false>
    { typedef typename remove_cv<_Up>::type __type; };

  template<typename _Up> 
    struct __decay_selector<_Up, true, false>
    { typedef typename remove_extent<_Up>::type* __type; };

  template<typename _Up> 
    struct __decay_selector<_Up, false, true>
    { typedef typename add_pointer<_Up>::type __type; };

  
  template<typename _Tp> 
    class decay 
    { 
      typedef typename remove_reference<_Tp>::type __remove_type;

    public:
      typedef typename __decay_selector<__remove_type>::__type type;
    };

  template<typename _Tp>
    class reference_wrapper;

  
  template<typename _Tp>
    struct __strip_reference_wrapper
    {
      typedef _Tp __type;
    };

  template<typename _Tp>
    struct __strip_reference_wrapper<reference_wrapper<_Tp> >
    {
      typedef _Tp& __type;
    };

  template<typename _Tp>
    struct __strip_reference_wrapper<const reference_wrapper<_Tp> >
    {
      typedef _Tp& __type;
    };

  template<typename _Tp>
    struct __decay_and_strip
    {
      typedef typename __strip_reference_wrapper<
	typename decay<_Tp>::type>::__type __type;
    };


  
  
  template<bool, typename _Tp = void>
    struct enable_if 
    { };

  
  template<typename _Tp>
    struct enable_if<true, _Tp>
    { typedef _Tp type; };

  template<typename... _Cond>
    using _Require = typename enable_if<__and_<_Cond...>::value>::type;

  
  
  template<bool _Cond, typename _Iftrue, typename _Iffalse>
    struct conditional
    { typedef _Iftrue type; };

  
  template<typename _Iftrue, typename _Iffalse>
    struct conditional<false, _Iftrue, _Iffalse>
    { typedef _Iffalse type; };

  
  template<typename... _Tp>
    struct common_type;

  

  struct __do_common_type_impl
  {
    template<typename _Tp, typename _Up>
      static __success_type<typename decay<decltype
			    (true ? std::declval<_Tp>()
			     : std::declval<_Up>())>::type> _S_test(int);

    template<typename, typename>
      static __failure_type _S_test(...);
  };

  template<typename _Tp, typename _Up>
    struct __common_type_impl
    : private __do_common_type_impl
    {
      typedef decltype(_S_test<_Tp, _Up>(0)) type;
    };

  struct __do_member_type_wrapper
  {
    template<typename _Tp>
      static __success_type<typename _Tp::type> _S_test(int);

    template<typename>
      static __failure_type _S_test(...);
  };

  template<typename _Tp>
    struct __member_type_wrapper
    : private __do_member_type_wrapper
    {
      typedef decltype(_S_test<_Tp>(0)) type;
    };

  template<typename _CTp, typename... _Args>
    struct __expanded_common_type_wrapper
    {
      typedef common_type<typename _CTp::type, _Args...> type;
    };

  template<typename... _Args>
    struct __expanded_common_type_wrapper<__failure_type, _Args...>
    { typedef __failure_type type; };

  template<typename _Tp>
    struct common_type<_Tp>
    { typedef typename decay<_Tp>::type type; };

  template<typename _Tp, typename _Up>
    struct common_type<_Tp, _Up>
    : public __common_type_impl<_Tp, _Up>::type
    { };

  template<typename _Tp, typename _Up, typename... _Vp>
    struct common_type<_Tp, _Up, _Vp...>
    : public __expanded_common_type_wrapper<typename __member_type_wrapper<
               common_type<_Tp, _Up>>::type, _Vp...>::type
    { };

  
  template<typename _Tp>
    struct underlying_type
    {
      typedef __underlying_type(_Tp) type;
    };

  template<typename _Tp>
    struct __declval_protector
    {
      static const bool __stop = false;
      static typename add_rvalue_reference<_Tp>::type __delegate();
    };

  template<typename _Tp>
    inline typename add_rvalue_reference<_Tp>::type
    declval() noexcept
    {
      static_assert(__declval_protector<_Tp>::__stop,
		    "declval() must not be used!");
      return __declval_protector<_Tp>::__delegate();
    }

  
  template<typename _Signature>
    class result_of;

  

  
  struct __result_of_memfun_ref_impl
  {
    template<typename _Fp, typename _Tp1, typename... _Args>
      static __success_type<decltype(
      (std::declval<_Tp1>().*std::declval<_Fp>())(std::declval<_Args>()...)
      )> _S_test(int);

    template<typename...>
      static __failure_type _S_test(...);
  };

  template<typename _MemPtr, typename _Arg, typename... _Args>
    struct __result_of_memfun_ref
    : private __result_of_memfun_ref_impl
    {
      typedef decltype(_S_test<_MemPtr, _Arg, _Args...>(0)) type;
    };

  
  struct __result_of_memfun_deref_impl
  {
    template<typename _Fp, typename _Tp1, typename... _Args>
      static __success_type<decltype(
      ((*std::declval<_Tp1>()).*std::declval<_Fp>())(std::declval<_Args>()...)
      )> _S_test(int);

    template<typename...>
      static __failure_type _S_test(...);
  };

  template<typename _MemPtr, typename _Arg, typename... _Args>
    struct __result_of_memfun_deref
    : private __result_of_memfun_deref_impl
    {
      typedef decltype(_S_test<_MemPtr, _Arg, _Args...>(0)) type;
    };

  
  struct __result_of_memobj_ref_impl
  {
    template<typename _Fp, typename _Tp1>
      static __success_type<decltype(
      std::declval<_Tp1>().*std::declval<_Fp>()
      )> _S_test(int);

    template<typename, typename>
      static __failure_type _S_test(...);
  };

  template<typename _MemPtr, typename _Arg>
    struct __result_of_memobj_ref
    : private __result_of_memobj_ref_impl
    {
      typedef decltype(_S_test<_MemPtr, _Arg>(0)) type;
    };

  
  struct __result_of_memobj_deref_impl
  {
    template<typename _Fp, typename _Tp1>
      static __success_type<decltype(
      (*std::declval<_Tp1>()).*std::declval<_Fp>()
      )> _S_test(int);

    template<typename, typename>
      static __failure_type _S_test(...);
  };

  template<typename _MemPtr, typename _Arg>
    struct __result_of_memobj_deref
    : private __result_of_memobj_deref_impl
    {
      typedef decltype(_S_test<_MemPtr, _Arg>(0)) type;
    };

  template<typename _MemPtr, typename _Arg>
    struct __result_of_memobj;

  template<typename _Res, typename _Class, typename _Arg>
    struct __result_of_memobj<_Res _Class::*, _Arg>
    {
      typedef typename remove_cv<typename remove_reference<
        _Arg>::type>::type _Argval;
      typedef _Res _Class::* _MemPtr;
      typedef typename conditional<__or_<is_same<_Argval, _Class>,
        is_base_of<_Class, _Argval>>::value,
        __result_of_memobj_ref<_MemPtr, _Arg>,
        __result_of_memobj_deref<_MemPtr, _Arg>
      >::type::type type;
    };

  template<typename _MemPtr, typename _Arg, typename... _Args>
    struct __result_of_memfun;

  template<typename _Res, typename _Class, typename _Arg, typename... _Args>
    struct __result_of_memfun<_Res _Class::*, _Arg, _Args...>
    {
      typedef typename remove_cv<typename remove_reference<
        _Arg>::type>::type _Argval;
      typedef _Res _Class::* _MemPtr;
      typedef typename conditional<__or_<is_same<_Argval, _Class>,
        is_base_of<_Class, _Argval>>::value,
        __result_of_memfun_ref<_MemPtr, _Arg, _Args...>,
        __result_of_memfun_deref<_MemPtr, _Arg, _Args...>
      >::type::type type;
    };

  template<bool, bool, typename _Functor, typename... _ArgTypes>
    struct __result_of_impl
    {
      typedef __failure_type type;
    };

  template<typename _MemPtr, typename _Arg>
    struct __result_of_impl<true, false, _MemPtr, _Arg>
    : public __result_of_memobj<typename decay<_MemPtr>::type, _Arg>
    { };

  template<typename _MemPtr, typename _Arg, typename... _Args>
    struct __result_of_impl<false, true, _MemPtr, _Arg, _Args...>
    : public __result_of_memfun<typename decay<_MemPtr>::type, _Arg, _Args...>
    { };

  
  struct __result_of_other_impl
  {
    template<typename _Fn, typename... _Args>
      static __success_type<decltype(
      std::declval<_Fn>()(std::declval<_Args>()...)
      )> _S_test(int);

    template<typename...>
      static __failure_type _S_test(...);
  };

  template<typename _Functor, typename... _ArgTypes>
    struct __result_of_impl<false, false, _Functor, _ArgTypes...>
    : private __result_of_other_impl
    {
      typedef decltype(_S_test<_Functor, _ArgTypes...>(0)) type;
    };

  template<typename _Functor, typename... _ArgTypes>
    struct result_of<_Functor(_ArgTypes...)>
    : public __result_of_impl<
        is_member_object_pointer<
          typename remove_reference<_Functor>::type
        >::value,
        is_member_function_pointer<
          typename remove_reference<_Functor>::type
        >::value,
	    _Functor, _ArgTypes...
      >::type
    { };

  
	
  


 


} 



namespace std 
{


  


 

  




 
  template<typename _Tp>
    constexpr _Tp&&
    forward(typename std::remove_reference<_Tp>::type& __t) noexcept
    { return static_cast<_Tp&&>(__t); }

  




 
  template<typename _Tp>
    constexpr _Tp&&
    forward(typename std::remove_reference<_Tp>::type&& __t) noexcept
    {
      static_assert(!std::is_lvalue_reference<_Tp>::value, "template argument"
		    " substituting _Tp is an lvalue reference type");
      return static_cast<_Tp&&>(__t);
    }

  



 
  template<typename _Tp>
    constexpr typename std::remove_reference<_Tp>::type&&
    move(_Tp&& __t) noexcept
    { return static_cast<typename std::remove_reference<_Tp>::type&&>(__t); }


  template<typename _Tp>
    struct __move_if_noexcept_cond
    : public __and_<__not_<is_nothrow_move_constructible<_Tp>>,
                    is_copy_constructible<_Tp>>::type { };

  






 
  template<typename _Tp>
    inline constexpr typename
    conditional<__move_if_noexcept_cond<_Tp>::value, const _Tp&, _Tp&&>::type
    move_if_noexcept(_Tp& __x) noexcept
    { return std::move(__x); }

  

  





 
  template<typename _Tp>
    inline _Tp*
    addressof(_Tp& __r) noexcept
    { return std::__addressof(__r); }

  

} 


namespace std 
{


  


 

  




 
  template<typename _Tp>
    inline void
    swap(_Tp& __a, _Tp& __b)
    noexcept(__and_<is_nothrow_move_constructible<_Tp>,
	            is_nothrow_move_assignable<_Tp>>::value)
    {
      
      

      _Tp __tmp = std::move(__a);
      __a = std::move(__b);
      __b = std::move(__tmp);
    }

  
  
  
  template<typename _Tp, size_t _Nm>
    inline void
    swap(_Tp (&__a)[_Nm], _Tp (&__b)[_Nm])
    noexcept(noexcept(swap(*__a, *__b)))
    {
      for (size_t __n = 0; __n < _Nm; ++__n)
	swap(__a[__n], __b[__n]);
    }

  

} 



namespace std 
{


  


 

  
  struct piecewise_construct_t { };

  
  constexpr piecewise_construct_t piecewise_construct = piecewise_construct_t();

  
  template<typename...>
    class tuple;

  template<std::size_t...>
    struct _Index_tuple;

 




 
  template<class _T1, class _T2>
    struct pair
    {
      typedef _T1 first_type;    
      typedef _T2 second_type;   

      _T1 first;                 
      _T2 second;                

      
      
      
 
      constexpr pair()
      : first(), second() { }

       
      constexpr pair(const _T1& __a, const _T2& __b)
      : first(__a), second(__b) { }

       
      template<class _U1, class _U2, class = typename
	       enable_if<__and_<is_convertible<const _U1&, _T1>,
				is_convertible<const _U2&, _T2>>::value>::type>
	constexpr pair(const pair<_U1, _U2>& __p)
	: first(__p.first), second(__p.second) { }

      constexpr pair(const pair&) = default;
      constexpr pair(pair&&) = default;

      
      template<class _U1, class = typename
	       enable_if<is_convertible<_U1, _T1>::value>::type>
	constexpr pair(_U1&& __x, const _T2& __y)
	: first(std::forward<_U1>(__x)), second(__y) { }

      template<class _U2, class = typename
	       enable_if<is_convertible<_U2, _T2>::value>::type>
	constexpr pair(const _T1& __x, _U2&& __y)
	: first(__x), second(std::forward<_U2>(__y)) { }

      template<class _U1, class _U2, class = typename
	       enable_if<__and_<is_convertible<_U1, _T1>,
				is_convertible<_U2, _T2>>::value>::type>
	constexpr pair(_U1&& __x, _U2&& __y)
	: first(std::forward<_U1>(__x)), second(std::forward<_U2>(__y)) { }

      template<class _U1, class _U2, class = typename
	       enable_if<__and_<is_convertible<_U1, _T1>,
				is_convertible<_U2, _T2>>::value>::type>
	constexpr pair(pair<_U1, _U2>&& __p)
	: first(std::forward<_U1>(__p.first)),
	  second(std::forward<_U2>(__p.second)) { }

      template<typename... _Args1, typename... _Args2>
        pair(piecewise_construct_t, tuple<_Args1...>, tuple<_Args2...>);

      pair&
      operator=(const pair& __p)
      {
	first = __p.first;
	second = __p.second;
	return *this;
      }

      pair&
      operator=(pair&& __p)
      noexcept(__and_<is_nothrow_move_assignable<_T1>,
	              is_nothrow_move_assignable<_T2>>::value)
      {
	first = std::forward<first_type>(__p.first);
	second = std::forward<second_type>(__p.second);
	return *this;
      }

      template<class _U1, class _U2>
	pair&
	operator=(const pair<_U1, _U2>& __p)
	{
	  first = __p.first;
	  second = __p.second;
	  return *this;
	}

      template<class _U1, class _U2>
	pair&
	operator=(pair<_U1, _U2>&& __p)
	{
	  first = std::forward<_U1>(__p.first);
	  second = std::forward<_U2>(__p.second);
	  return *this;
	}

      void
      swap(pair& __p)
      noexcept(noexcept(swap(first, __p.first))
	       && noexcept(swap(second, __p.second)))
      {
	using std::swap;
	swap(first, __p.first);
	swap(second, __p.second);
      }

    private:
      template<typename... _Args1, std::size_t... _Indexes1,
               typename... _Args2, std::size_t... _Indexes2>
        pair(tuple<_Args1...>&, tuple<_Args2...>&,
             _Index_tuple<_Indexes1...>, _Index_tuple<_Indexes2...>);
    };

  
  template<class _T1, class _T2>
    inline constexpr bool
    operator==(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return __x.first == __y.first && __x.second == __y.second; }

  
  template<class _T1, class _T2>
    inline constexpr bool
    operator<(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return __x.first < __y.first
	     || (!(__y.first < __x.first) && __x.second < __y.second); }

  
  template<class _T1, class _T2>
    inline constexpr bool
    operator!=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return !(__x == __y); }

  
  template<class _T1, class _T2>
    inline constexpr bool
    operator>(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return __y < __x; }

  
  template<class _T1, class _T2>
    inline constexpr bool
    operator<=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return !(__y < __x); }

  
  template<class _T1, class _T2>
    inline constexpr bool
    operator>=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return !(__x < __y); }

  
  
  
  template<class _T1, class _T2>
    inline void
    swap(pair<_T1, _T2>& __x, pair<_T1, _T2>& __y)
    noexcept(noexcept(__x.swap(__y)))
    { __x.swap(__y); }

  








 
  
  
  
  template<class _T1, class _T2>
    constexpr pair<typename __decay_and_strip<_T1>::__type,
                   typename __decay_and_strip<_T2>::__type>
    make_pair(_T1&& __x, _T2&& __y)
    {
      typedef typename __decay_and_strip<_T1>::__type __ds_type1;
      typedef typename __decay_and_strip<_T2>::__type __ds_type2;
      typedef pair<__ds_type1, __ds_type2> 	      __pair_type;
      return __pair_type(std::forward<_T1>(__x), std::forward<_T2>(__y));
    }

  


} 

















































 







 


#pragma GCC system_header



namespace std 
{


  


 
  

  





 
  
  
  struct input_iterator_tag { };

  
  struct output_iterator_tag { };

  
  struct forward_iterator_tag : public input_iterator_tag { };

  
  
  struct bidirectional_iterator_tag : public forward_iterator_tag { };

  
  
  struct random_access_iterator_tag : public bidirectional_iterator_tag { };
  

  








 
  template<typename _Category, typename _Tp, typename _Distance = ptrdiff_t,
           typename _Pointer = _Tp*, typename _Reference = _Tp&>
    struct iterator
    {
      
      typedef _Category  iterator_category;
      
      typedef _Tp        value_type;
      
      typedef _Distance  difference_type;
      
      typedef _Pointer   pointer;
      
      typedef _Reference reference;
    };

  






 

template<typename _Tp> class __has_iterator_category_helper : __sfinae_types { template<typename _Up> struct _Wrap_type { }; template<typename _Up> static __one __test(_Wrap_type<typename _Up::iterator_category> *); template<typename _Up> static __two __test(...); public: static constexpr bool value = sizeof(__test<_Tp>(0)) == 1; }; template<typename _Tp> struct __has_iterator_category : integral_constant<bool, __has_iterator_category_helper <typename remove_cv<_Tp> ::type> ::value> { };

  template<typename _Iterator,
	   bool = __has_iterator_category<_Iterator>::value>
    struct __iterator_traits { };

  template<typename _Iterator>
    struct __iterator_traits<_Iterator, true>
    {
      typedef typename _Iterator::iterator_category iterator_category;
      typedef typename _Iterator::value_type        value_type;
      typedef typename _Iterator::difference_type   difference_type;
      typedef typename _Iterator::pointer           pointer;
      typedef typename _Iterator::reference         reference;
    };

  template<typename _Iterator>
    struct iterator_traits
    : public __iterator_traits<_Iterator> { };

  
  template<typename _Tp>
    struct iterator_traits<_Tp*>
    {
      typedef random_access_iterator_tag iterator_category;
      typedef _Tp                         value_type;
      typedef ptrdiff_t                   difference_type;
      typedef _Tp*                        pointer;
      typedef _Tp&                        reference;
    };

  
  template<typename _Tp>
    struct iterator_traits<const _Tp*>
    {
      typedef random_access_iterator_tag iterator_category;
      typedef _Tp                         value_type;
      typedef ptrdiff_t                   difference_type;
      typedef const _Tp*                  pointer;
      typedef const _Tp&                  reference;
    };

  


 
  template<typename _Iter>
    inline typename iterator_traits<_Iter>::iterator_category
    __iterator_category(const _Iter&)
    { return typename iterator_traits<_Iter>::iterator_category(); }

  

  
  
  template<typename _Iterator, bool _HasBase>
    struct _Iter_base
    {
      typedef _Iterator iterator_type;
      static iterator_type _S_base(_Iterator __it)
      { return __it; }
    };

  template<typename _Iterator>
    struct _Iter_base<_Iterator, true>
    {
      typedef typename _Iterator::iterator_type iterator_type;
      static iterator_type _S_base(_Iterator __it)
      { return __it.base(); }
    };

  template<typename _InIter>
    using _RequireInputIter = typename
      enable_if<is_convertible<typename
		iterator_traits<_InIter>::iterator_category,
			       input_iterator_tag>::value>::type;


} 


















































 







 


#pragma GCC system_header



























 








 






 
namespace std
{ 
  namespace __debug { } 
}



 
namespace __gnu_debug
{
  using namespace std::__debug;
}





namespace std 
{


  template<typename _InputIterator>
    inline typename iterator_traits<_InputIterator>::difference_type
    __distance(_InputIterator __first, _InputIterator __last,
               input_iterator_tag)
    {
      
      

      typename iterator_traits<_InputIterator>::difference_type __n = 0;
      while (__first != __last)
	{
	  ++__first;
	  ++__n;
	}
      return __n;
    }

  template<typename _RandomAccessIterator>
    inline typename iterator_traits<_RandomAccessIterator>::difference_type
    __distance(_RandomAccessIterator __first, _RandomAccessIterator __last,
               random_access_iterator_tag)
    {
      
      
      return __last - __first;
    }

  











 
  template<typename _InputIterator>
    inline typename iterator_traits<_InputIterator>::difference_type
    distance(_InputIterator __first, _InputIterator __last)
    {
      
      return std::__distance(__first, __last,
			     std::__iterator_category(__first));
    }

  template<typename _InputIterator, typename _Distance>
    inline void
    __advance(_InputIterator& __i, _Distance __n, input_iterator_tag)
    {
      
      
      ;
      while (__n--)
	++__i;
    }

  template<typename _BidirectionalIterator, typename _Distance>
    inline void
    __advance(_BidirectionalIterator& __i, _Distance __n,
	      bidirectional_iterator_tag)
    {
      
      
      if (__n > 0)
        while (__n--)
	  ++__i;
      else
        while (__n++)
	  --__i;
    }

  template<typename _RandomAccessIterator, typename _Distance>
    inline void
    __advance(_RandomAccessIterator& __i, _Distance __n,
              random_access_iterator_tag)
    {
      
      
      __i += __n;
    }

  










 
  template<typename _InputIterator, typename _Distance>
    inline void
    advance(_InputIterator& __i, _Distance __n)
    {
      
      typename iterator_traits<_InputIterator>::difference_type __d = __n;
      std::__advance(__i, __d, std::__iterator_category(__i));
    }


  template<typename _ForwardIterator>
    inline _ForwardIterator
    next(_ForwardIterator __x, typename
	 iterator_traits<_ForwardIterator>::difference_type __n = 1)
    {
      std::advance(__x, __n);
      return __x;
    }

  template<typename _BidirectionalIterator>
    inline _BidirectionalIterator
    prev(_BidirectionalIterator __x, typename
	 iterator_traits<_BidirectionalIterator>::difference_type __n = 1) 
    {
      std::advance(__x, -__n);
      return __x;
    }



} 

















































 








 



namespace std 
{


  


 

  
  
















 
  template<typename _Iterator>
    class reverse_iterator
    : public iterator<typename iterator_traits<_Iterator>::iterator_category,
		      typename iterator_traits<_Iterator>::value_type,
		      typename iterator_traits<_Iterator>::difference_type,
		      typename iterator_traits<_Iterator>::pointer,
                      typename iterator_traits<_Iterator>::reference>
    {
    protected:
      _Iterator current;

      typedef iterator_traits<_Iterator>		__traits_type;

    public:
      typedef _Iterator					iterator_type;
      typedef typename __traits_type::difference_type	difference_type;
      typedef typename __traits_type::pointer		pointer;
      typedef typename __traits_type::reference		reference;

      


 
      
      
      reverse_iterator() : current() { }

      

 
      explicit
      reverse_iterator(iterator_type __x) : current(__x) { }

      

 
      reverse_iterator(const reverse_iterator& __x)
      : current(__x.current) { }

      


 
      template<typename _Iter>
        reverse_iterator(const reverse_iterator<_Iter>& __x)
	: current(__x.base()) { }

      

 
      iterator_type
      base() const
      { return current; }

      








 
      reference
      operator*() const
      {
	_Iterator __tmp = current;
	return *--__tmp;
      }

      



 
      pointer
      operator->() const
      { return &(operator*()); }

      



 
      reverse_iterator&
      operator++()
      {
	--current;
	return *this;
      }

      



 
      reverse_iterator
      operator++(int)
      {
	reverse_iterator __tmp = *this;
	--current;
	return __tmp;
      }

      



 
      reverse_iterator&
      operator--()
      {
	++current;
	return *this;
      }

      



 
      reverse_iterator
      operator--(int)
      {
	reverse_iterator __tmp = *this;
	++current;
	return __tmp;
      }

      



 
      reverse_iterator
      operator+(difference_type __n) const
      { return reverse_iterator(current - __n); }

      




 
      reverse_iterator&
      operator+=(difference_type __n)
      {
	current -= __n;
	return *this;
      }

      



 
      reverse_iterator
      operator-(difference_type __n) const
      { return reverse_iterator(current + __n); }

      




 
      reverse_iterator&
      operator-=(difference_type __n)
      {
	current += __n;
	return *this;
      }

      



 
      reference
      operator[](difference_type __n) const
      { return *(*this + __n); }
    };

  
  







 
  template<typename _Iterator>
    inline bool
    operator==(const reverse_iterator<_Iterator>& __x,
	       const reverse_iterator<_Iterator>& __y)
    { return __x.base() == __y.base(); }

  template<typename _Iterator>
    inline bool
    operator<(const reverse_iterator<_Iterator>& __x,
	      const reverse_iterator<_Iterator>& __y)
    { return __y.base() < __x.base(); }

  template<typename _Iterator>
    inline bool
    operator!=(const reverse_iterator<_Iterator>& __x,
	       const reverse_iterator<_Iterator>& __y)
    { return !(__x == __y); }

  template<typename _Iterator>
    inline bool
    operator>(const reverse_iterator<_Iterator>& __x,
	      const reverse_iterator<_Iterator>& __y)
    { return __y < __x; }

  template<typename _Iterator>
    inline bool
    operator<=(const reverse_iterator<_Iterator>& __x,
	       const reverse_iterator<_Iterator>& __y)
    { return !(__y < __x); }

  template<typename _Iterator>
    inline bool
    operator>=(const reverse_iterator<_Iterator>& __x,
	       const reverse_iterator<_Iterator>& __y)
    { return !(__x < __y); }

  template<typename _Iterator>
    inline typename reverse_iterator<_Iterator>::difference_type
    operator-(const reverse_iterator<_Iterator>& __x,
	      const reverse_iterator<_Iterator>& __y)
    { return __y.base() - __x.base(); }

  template<typename _Iterator>
    inline reverse_iterator<_Iterator>
    operator+(typename reverse_iterator<_Iterator>::difference_type __n,
	      const reverse_iterator<_Iterator>& __x)
    { return reverse_iterator<_Iterator>(__x.base() - __n); }

  
  
  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator==(const reverse_iterator<_IteratorL>& __x,
	       const reverse_iterator<_IteratorR>& __y)
    { return __x.base() == __y.base(); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator<(const reverse_iterator<_IteratorL>& __x,
	      const reverse_iterator<_IteratorR>& __y)
    { return __y.base() < __x.base(); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator!=(const reverse_iterator<_IteratorL>& __x,
	       const reverse_iterator<_IteratorR>& __y)
    { return !(__x == __y); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator>(const reverse_iterator<_IteratorL>& __x,
	      const reverse_iterator<_IteratorR>& __y)
    { return __y < __x; }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator<=(const reverse_iterator<_IteratorL>& __x,
	       const reverse_iterator<_IteratorR>& __y)
    { return !(__y < __x); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator>=(const reverse_iterator<_IteratorL>& __x,
	       const reverse_iterator<_IteratorR>& __y)
    { return !(__x < __y); }

  template<typename _IteratorL, typename _IteratorR>
    
    inline auto
    operator-(const reverse_iterator<_IteratorL>& __x,
	      const reverse_iterator<_IteratorR>& __y)
    -> decltype(__y.base() - __x.base())
    { return __y.base() - __x.base(); }
  

  
  








 
  template<typename _Container>
    class back_insert_iterator
    : public iterator<output_iterator_tag, void, void, void, void>
    {
    protected:
      _Container* container;

    public:
      
      typedef _Container          container_type;

      
      explicit
      back_insert_iterator(_Container& __x) : container(&__x) { }

      









 
      back_insert_iterator&
      operator=(const typename _Container::value_type& __value)
      {
	container->push_back(__value);
	return *this;
      }

      back_insert_iterator&
      operator=(typename _Container::value_type&& __value)
      {
	container->push_back(std::move(__value));
	return *this;
      }

      
      back_insert_iterator&
      operator*()
      { return *this; }

      
      back_insert_iterator&
      operator++()
      { return *this; }

      
      back_insert_iterator
      operator++(int)
      { return *this; }
    };

  









 
  template<typename _Container>
    inline back_insert_iterator<_Container>
    back_inserter(_Container& __x)
    { return back_insert_iterator<_Container>(__x); }

  








 
  template<typename _Container>
    class front_insert_iterator
    : public iterator<output_iterator_tag, void, void, void, void>
    {
    protected:
      _Container* container;

    public:
      
      typedef _Container          container_type;

      
      explicit front_insert_iterator(_Container& __x) : container(&__x) { }

      









 
      front_insert_iterator&
      operator=(const typename _Container::value_type& __value)
      {
	container->push_front(__value);
	return *this;
      }

      front_insert_iterator&
      operator=(typename _Container::value_type&& __value)
      {
	container->push_front(std::move(__value));
	return *this;
      }

      
      front_insert_iterator&
      operator*()
      { return *this; }

      
      front_insert_iterator&
      operator++()
      { return *this; }

      
      front_insert_iterator
      operator++(int)
      { return *this; }
    };

  









 
  template<typename _Container>
    inline front_insert_iterator<_Container>
    front_inserter(_Container& __x)
    { return front_insert_iterator<_Container>(__x); }

  












 
  template<typename _Container>
    class insert_iterator
    : public iterator<output_iterator_tag, void, void, void, void>
    {
    protected:
      _Container* container;
      typename _Container::iterator iter;

    public:
      
      typedef _Container          container_type;

      


 
      insert_iterator(_Container& __x, typename _Container::iterator __i)
      : container(&__x), iter(__i) {}

      





















 
      insert_iterator&
      operator=(const typename _Container::value_type& __value)
      {
	iter = container->insert(iter, __value);
	++iter;
	return *this;
      }

      insert_iterator&
      operator=(typename _Container::value_type&& __value)
      {
	iter = container->insert(iter, std::move(__value));
	++iter;
	return *this;
      }

      
      insert_iterator&
      operator*()
      { return *this; }

      
      insert_iterator&
      operator++()
      { return *this; }

      
      insert_iterator&
      operator++(int)
      { return *this; }
    };

  









 
  template<typename _Container, typename _Iterator>
    inline insert_iterator<_Container>
    inserter(_Container& __x, _Iterator __i)
    {
      return insert_iterator<_Container>(__x,
					 typename _Container::iterator(__i));
    }

  


} 

namespace __gnu_cxx 
{


  
  
  
  
  
  
  
  using std::iterator_traits;
  using std::iterator;
  template<typename _Iterator, typename _Container>
    class __normal_iterator
    {
    protected:
      _Iterator _M_current;

      typedef iterator_traits<_Iterator>		__traits_type;

    public:
      typedef _Iterator					iterator_type;
      typedef typename __traits_type::iterator_category iterator_category;
      typedef typename __traits_type::value_type  	value_type;
      typedef typename __traits_type::difference_type 	difference_type;
      typedef typename __traits_type::reference 	reference;
      typedef typename __traits_type::pointer   	pointer;

      constexpr __normal_iterator() : _M_current(_Iterator()) { }

      explicit
      __normal_iterator(const _Iterator& __i) : _M_current(__i) { }

      
      template<typename _Iter>
        __normal_iterator(const __normal_iterator<_Iter,
			  typename __enable_if<
      	       (std::__are_same<_Iter, typename _Container::pointer>::__value),
		      _Container>::__type>& __i)
        : _M_current(__i.base()) { }

      
      reference
      operator*() const
      { return *_M_current; }

      pointer
      operator->() const
      { return _M_current; }

      __normal_iterator&
      operator++()
      {
	++_M_current;
	return *this;
      }

      __normal_iterator
      operator++(int)
      { return __normal_iterator(_M_current++); }

      
      __normal_iterator&
      operator--()
      {
	--_M_current;
	return *this;
      }

      __normal_iterator
      operator--(int)
      { return __normal_iterator(_M_current--); }

      
      reference
      operator[](const difference_type& __n) const
      { return _M_current[__n]; }

      __normal_iterator&
      operator+=(const difference_type& __n)
      { _M_current += __n; return *this; }

      __normal_iterator
      operator+(const difference_type& __n) const
      { return __normal_iterator(_M_current + __n); }

      __normal_iterator&
      operator-=(const difference_type& __n)
      { _M_current -= __n; return *this; }

      __normal_iterator
      operator-(const difference_type& __n) const
      { return __normal_iterator(_M_current - __n); }

      const _Iterator&
      base() const
      { return _M_current; }
    };

  
  
  
  
  
  
  

  
  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator==(const __normal_iterator<_IteratorL, _Container>& __lhs,
	       const __normal_iterator<_IteratorR, _Container>& __rhs)
    { return __lhs.base() == __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator==(const __normal_iterator<_Iterator, _Container>& __lhs,
	       const __normal_iterator<_Iterator, _Container>& __rhs)
    { return __lhs.base() == __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator!=(const __normal_iterator<_IteratorL, _Container>& __lhs,
	       const __normal_iterator<_IteratorR, _Container>& __rhs)
    { return __lhs.base() != __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator!=(const __normal_iterator<_Iterator, _Container>& __lhs,
	       const __normal_iterator<_Iterator, _Container>& __rhs)
    { return __lhs.base() != __rhs.base(); }

  
  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator<(const __normal_iterator<_IteratorL, _Container>& __lhs,
	      const __normal_iterator<_IteratorR, _Container>& __rhs)
    { return __lhs.base() < __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator<(const __normal_iterator<_Iterator, _Container>& __lhs,
	      const __normal_iterator<_Iterator, _Container>& __rhs)
    { return __lhs.base() < __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator>(const __normal_iterator<_IteratorL, _Container>& __lhs,
	      const __normal_iterator<_IteratorR, _Container>& __rhs)
    { return __lhs.base() > __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator>(const __normal_iterator<_Iterator, _Container>& __lhs,
	      const __normal_iterator<_Iterator, _Container>& __rhs)
    { return __lhs.base() > __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator<=(const __normal_iterator<_IteratorL, _Container>& __lhs,
	       const __normal_iterator<_IteratorR, _Container>& __rhs)
    { return __lhs.base() <= __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator<=(const __normal_iterator<_Iterator, _Container>& __lhs,
	       const __normal_iterator<_Iterator, _Container>& __rhs)
    { return __lhs.base() <= __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator>=(const __normal_iterator<_IteratorL, _Container>& __lhs,
	       const __normal_iterator<_IteratorR, _Container>& __rhs)
    { return __lhs.base() >= __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator>=(const __normal_iterator<_Iterator, _Container>& __lhs,
	       const __normal_iterator<_Iterator, _Container>& __rhs)
    { return __lhs.base() >= __rhs.base(); }

  
  
  
  
  template<typename _IteratorL, typename _IteratorR, typename _Container>
    
    inline auto
    operator-(const __normal_iterator<_IteratorL, _Container>& __lhs,
	      const __normal_iterator<_IteratorR, _Container>& __rhs)
    -> decltype(__lhs.base() - __rhs.base())
    { return __lhs.base() - __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline typename __normal_iterator<_Iterator, _Container>::difference_type
    operator-(const __normal_iterator<_Iterator, _Container>& __lhs,
	      const __normal_iterator<_Iterator, _Container>& __rhs)
    { return __lhs.base() - __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline __normal_iterator<_Iterator, _Container>
    operator+(typename __normal_iterator<_Iterator, _Container>::difference_type
	      __n, const __normal_iterator<_Iterator, _Container>& __i)
    { return __normal_iterator<_Iterator, _Container>(__i.base() + __n); }


} 


namespace std 
{


  


 

  
  






 
  template<typename _Iterator>
    class move_iterator
    {
    protected:
      _Iterator _M_current;

      typedef iterator_traits<_Iterator>		__traits_type;

    public:
      typedef _Iterator					iterator_type;
      typedef typename __traits_type::iterator_category iterator_category;
      typedef typename __traits_type::value_type  	value_type;
      typedef typename __traits_type::difference_type	difference_type;
      
      typedef _Iterator					pointer;
      typedef value_type&&				reference;

      move_iterator()
      : _M_current() { }

      explicit
      move_iterator(iterator_type __i)
      : _M_current(__i) { }

      template<typename _Iter>
	move_iterator(const move_iterator<_Iter>& __i)
	: _M_current(__i.base()) { }

      iterator_type
      base() const
      { return _M_current; }

      reference
      operator*() const
      { return std::move(*_M_current); }

      pointer
      operator->() const
      { return _M_current; }

      move_iterator&
      operator++()
      {
	++_M_current;
	return *this;
      }

      move_iterator
      operator++(int)
      {
	move_iterator __tmp = *this;
	++_M_current;
	return __tmp;
      }

      move_iterator&
      operator--()
      {
	--_M_current;
	return *this;
      }

      move_iterator
      operator--(int)
      {
	move_iterator __tmp = *this;
	--_M_current;
	return __tmp;
      }

      move_iterator
      operator+(difference_type __n) const
      { return move_iterator(_M_current + __n); }

      move_iterator&
      operator+=(difference_type __n)
      {
	_M_current += __n;
	return *this;
      }

      move_iterator
      operator-(difference_type __n) const
      { return move_iterator(_M_current - __n); }
    
      move_iterator&
      operator-=(difference_type __n)
      { 
	_M_current -= __n;
	return *this;
      }

      reference
      operator[](difference_type __n) const
      { return std::move(_M_current[__n]); }
    };

  
  
  
  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator==(const move_iterator<_IteratorL>& __x,
	       const move_iterator<_IteratorR>& __y)
    { return __x.base() == __y.base(); }

  template<typename _Iterator>
    inline bool
    operator==(const move_iterator<_Iterator>& __x,
	       const move_iterator<_Iterator>& __y)
    { return __x.base() == __y.base(); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator!=(const move_iterator<_IteratorL>& __x,
	       const move_iterator<_IteratorR>& __y)
    { return !(__x == __y); }

  template<typename _Iterator>
    inline bool
    operator!=(const move_iterator<_Iterator>& __x,
	       const move_iterator<_Iterator>& __y)
    { return !(__x == __y); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator<(const move_iterator<_IteratorL>& __x,
	      const move_iterator<_IteratorR>& __y)
    { return __x.base() < __y.base(); }

  template<typename _Iterator>
    inline bool
    operator<(const move_iterator<_Iterator>& __x,
	      const move_iterator<_Iterator>& __y)
    { return __x.base() < __y.base(); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator<=(const move_iterator<_IteratorL>& __x,
	       const move_iterator<_IteratorR>& __y)
    { return !(__y < __x); }

  template<typename _Iterator>
    inline bool
    operator<=(const move_iterator<_Iterator>& __x,
	       const move_iterator<_Iterator>& __y)
    { return !(__y < __x); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator>(const move_iterator<_IteratorL>& __x,
	      const move_iterator<_IteratorR>& __y)
    { return __y < __x; }

  template<typename _Iterator>
    inline bool
    operator>(const move_iterator<_Iterator>& __x,
	      const move_iterator<_Iterator>& __y)
    { return __y < __x; }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator>=(const move_iterator<_IteratorL>& __x,
	       const move_iterator<_IteratorR>& __y)
    { return !(__x < __y); }

  template<typename _Iterator>
    inline bool
    operator>=(const move_iterator<_Iterator>& __x,
	       const move_iterator<_Iterator>& __y)
    { return !(__x < __y); }

  
  template<typename _IteratorL, typename _IteratorR>
    inline auto
    operator-(const move_iterator<_IteratorL>& __x,
	      const move_iterator<_IteratorR>& __y)
    -> decltype(__x.base() - __y.base())
    { return __x.base() - __y.base(); }

  template<typename _Iterator>
    inline auto
    operator-(const move_iterator<_Iterator>& __x,
	      const move_iterator<_Iterator>& __y)
    -> decltype(__x.base() - __y.base())
    { return __x.base() - __y.base(); }

  template<typename _Iterator>
    inline move_iterator<_Iterator>
    operator+(typename move_iterator<_Iterator>::difference_type __n,
	      const move_iterator<_Iterator>& __x)
    { return __x + __n; }

  template<typename _Iterator>
    inline move_iterator<_Iterator>
    make_move_iterator(_Iterator __i)
    { return move_iterator<_Iterator>(__i); }

  template<typename _Iterator, typename _ReturnType
    = typename conditional<__move_if_noexcept_cond
      <typename iterator_traits<_Iterator>::value_type>::value,
                _Iterator, move_iterator<_Iterator>>::type>
    inline _ReturnType
    __make_move_if_noexcept_iterator(_Iterator __i)
    { return _ReturnType(__i); }

  


} 



namespace std 
{



  








 
  template<typename _ForwardIterator1, typename _ForwardIterator2>
    inline void
    iter_swap(_ForwardIterator1 __a, _ForwardIterator2 __b)
    {
      
      
      

      swap(*__a, *__b);
    }

  










 
  template<typename _ForwardIterator1, typename _ForwardIterator2>
    _ForwardIterator2
    swap_ranges(_ForwardIterator1 __first1, _ForwardIterator1 __last1,
		_ForwardIterator2 __first2)
    {
      
      
      
      ;

      for (; __first1 != __last1; ++__first1, ++__first2)
	std::iter_swap(__first1, __first2);
      return __first2;
    }

  









 
  template<typename _Tp>
    inline const _Tp&
    min(const _Tp& __a, const _Tp& __b)
    {
      
      
      
      if (__b < __a)
	return __b;
      return __a;
    }

  









 
  template<typename _Tp>
    inline const _Tp&
    max(const _Tp& __a, const _Tp& __b)
    {
      
      
      
      if (__a < __b)
	return __b;
      return __a;
    }

  









 
  template<typename _Tp, typename _Compare>
    inline const _Tp&
    min(const _Tp& __a, const _Tp& __b, _Compare __comp)
    {
      
      if (__comp(__b, __a))
	return __b;
      return __a;
    }

  









 
  template<typename _Tp, typename _Compare>
    inline const _Tp&
    max(const _Tp& __a, const _Tp& __b, _Compare __comp)
    {
      
      if (__comp(__a, __b))
	return __b;
      return __a;
    }

  
  
  template<typename _Iterator>
    struct _Niter_base
    : _Iter_base<_Iterator, __is_normal_iterator<_Iterator>::__value>
    { };

  template<typename _Iterator>
    inline typename _Niter_base<_Iterator>::iterator_type
    __niter_base(_Iterator __it)
    { return std::_Niter_base<_Iterator>::_S_base(__it); }

  
  template<typename _Iterator>
    struct _Miter_base
    : _Iter_base<_Iterator, __is_move_iterator<_Iterator>::__value>
    { };

  template<typename _Iterator>
    inline typename _Miter_base<_Iterator>::iterator_type
    __miter_base(_Iterator __it)
    { return std::_Miter_base<_Iterator>::_S_base(__it); }

  
  
  
  
  

  template<bool, bool, typename>
    struct __copy_move
    {
      template<typename _II, typename _OI>
        static _OI
        __copy_m(_II __first, _II __last, _OI __result)
        {
	  for (; __first != __last; ++__result, ++__first)
	    *__result = *__first;
	  return __result;
	}
    };

  template<typename _Category>
    struct __copy_move<true, false, _Category>
    {
      template<typename _II, typename _OI>
        static _OI
        __copy_m(_II __first, _II __last, _OI __result)
        {
	  for (; __first != __last; ++__result, ++__first)
	    *__result = std::move(*__first);
	  return __result;
	}
    };

  template<>
    struct __copy_move<false, false, random_access_iterator_tag>
    {
      template<typename _II, typename _OI>
        static _OI
        __copy_m(_II __first, _II __last, _OI __result)
        { 
	  typedef typename iterator_traits<_II>::difference_type _Distance;
	  for(_Distance __n = __last - __first; __n > 0; --__n)
	    {
	      *__result = *__first;
	      ++__first;
	      ++__result;
	    }
	  return __result;
	}
    };

  template<>
    struct __copy_move<true, false, random_access_iterator_tag>
    {
      template<typename _II, typename _OI>
        static _OI
        __copy_m(_II __first, _II __last, _OI __result)
        { 
	  typedef typename iterator_traits<_II>::difference_type _Distance;
	  for(_Distance __n = __last - __first; __n > 0; --__n)
	    {
	      *__result = std::move(*__first);
	      ++__first;
	      ++__result;
	    }
	  return __result;
	}
    };

  template<bool _IsMove>
    struct __copy_move<_IsMove, true, random_access_iterator_tag>
    {
      template<typename _Tp>
        static _Tp*
        __copy_m(const _Tp* __first, const _Tp* __last, _Tp* __result)
        {
	  const ptrdiff_t _Num = __last - __first;
	  if (_Num)
	    __builtin_memmove(__result, __first, sizeof(_Tp) * _Num);
	  return __result + _Num;
	}
    };

  template<bool _IsMove, typename _II, typename _OI>
    inline _OI
    __copy_move_a(_II __first, _II __last, _OI __result)
    {
      typedef typename iterator_traits<_II>::value_type _ValueTypeI;
      typedef typename iterator_traits<_OI>::value_type _ValueTypeO;
      typedef typename iterator_traits<_II>::iterator_category _Category;
      const bool __simple = (__is_trivial(_ValueTypeI)
	                     && __is_pointer<_II>::__value
	                     && __is_pointer<_OI>::__value
			     && __are_same<_ValueTypeI, _ValueTypeO>::__value);

      return std::__copy_move<_IsMove, __simple,
	                      _Category>::__copy_m(__first, __last, __result);
    }

  
  
  template<typename _CharT>
    struct char_traits;

  template<typename _CharT, typename _Traits>
    class istreambuf_iterator;

  template<typename _CharT, typename _Traits>
    class ostreambuf_iterator;

  template<bool _IsMove, typename _CharT>
    typename __gnu_cxx::__enable_if<__is_char<_CharT>::__value, 
	     ostreambuf_iterator<_CharT, char_traits<_CharT> > >::__type
    __copy_move_a2(_CharT*, _CharT*,
		   ostreambuf_iterator<_CharT, char_traits<_CharT> >);

  template<bool _IsMove, typename _CharT>
    typename __gnu_cxx::__enable_if<__is_char<_CharT>::__value, 
	     ostreambuf_iterator<_CharT, char_traits<_CharT> > >::__type
    __copy_move_a2(const _CharT*, const _CharT*,
		   ostreambuf_iterator<_CharT, char_traits<_CharT> >);

  template<bool _IsMove, typename _CharT>
    typename __gnu_cxx::__enable_if<__is_char<_CharT>::__value,
				    _CharT*>::__type
    __copy_move_a2(istreambuf_iterator<_CharT, char_traits<_CharT> >,
		   istreambuf_iterator<_CharT, char_traits<_CharT> >, _CharT*);

  template<bool _IsMove, typename _II, typename _OI>
    inline _OI
    __copy_move_a2(_II __first, _II __last, _OI __result)
    {
      return _OI(std::__copy_move_a<_IsMove>(std::__niter_base(__first),
					     std::__niter_base(__last),
					     std::__niter_base(__result)));
    }

  















 
  template<typename _II, typename _OI>
    inline _OI
    copy(_II __first, _II __last, _OI __result)
    {
      
      
      
      ;

      return (std::__copy_move_a2<__is_move_iterator<_II>::__value>
	      (std::__miter_base(__first), std::__miter_base(__last),
	       __result));
    }

  















 
  template<typename _II, typename _OI>
    inline _OI
    move(_II __first, _II __last, _OI __result)
    {
      
      
      
      ;

      return std::__copy_move_a2<true>(std::__miter_base(__first),
				       std::__miter_base(__last), __result);
    }


  template<bool, bool, typename>
    struct __copy_move_backward
    {
      template<typename _BI1, typename _BI2>
        static _BI2
        __copy_move_b(_BI1 __first, _BI1 __last, _BI2 __result)
        {
	  while (__first != __last)
	    *--__result = *--__last;
	  return __result;
	}
    };

  template<typename _Category>
    struct __copy_move_backward<true, false, _Category>
    {
      template<typename _BI1, typename _BI2>
        static _BI2
        __copy_move_b(_BI1 __first, _BI1 __last, _BI2 __result)
        {
	  while (__first != __last)
	    *--__result = std::move(*--__last);
	  return __result;
	}
    };

  template<>
    struct __copy_move_backward<false, false, random_access_iterator_tag>
    {
      template<typename _BI1, typename _BI2>
        static _BI2
        __copy_move_b(_BI1 __first, _BI1 __last, _BI2 __result)
        {
	  typename iterator_traits<_BI1>::difference_type __n;
	  for (__n = __last - __first; __n > 0; --__n)
	    *--__result = *--__last;
	  return __result;
	}
    };

  template<>
    struct __copy_move_backward<true, false, random_access_iterator_tag>
    {
      template<typename _BI1, typename _BI2>
        static _BI2
        __copy_move_b(_BI1 __first, _BI1 __last, _BI2 __result)
        {
	  typename iterator_traits<_BI1>::difference_type __n;
	  for (__n = __last - __first; __n > 0; --__n)
	    *--__result = std::move(*--__last);
	  return __result;
	}
    };

  template<bool _IsMove>
    struct __copy_move_backward<_IsMove, true, random_access_iterator_tag>
    {
      template<typename _Tp>
        static _Tp*
        __copy_move_b(const _Tp* __first, const _Tp* __last, _Tp* __result)
        {
	  const ptrdiff_t _Num = __last - __first;
	  if (_Num)
	    __builtin_memmove(__result - _Num, __first, sizeof(_Tp) * _Num);
	  return __result - _Num;
	}
    };

  template<bool _IsMove, typename _BI1, typename _BI2>
    inline _BI2
    __copy_move_backward_a(_BI1 __first, _BI1 __last, _BI2 __result)
    {
      typedef typename iterator_traits<_BI1>::value_type _ValueType1;
      typedef typename iterator_traits<_BI2>::value_type _ValueType2;
      typedef typename iterator_traits<_BI1>::iterator_category _Category;
      const bool __simple = (__is_trivial(_ValueType1)
	                     && __is_pointer<_BI1>::__value
	                     && __is_pointer<_BI2>::__value
			     && __are_same<_ValueType1, _ValueType2>::__value);

      return std::__copy_move_backward<_IsMove, __simple,
	                               _Category>::__copy_move_b(__first,
								 __last,
								 __result);
    }

  template<bool _IsMove, typename _BI1, typename _BI2>
    inline _BI2
    __copy_move_backward_a2(_BI1 __first, _BI1 __last, _BI2 __result)
    {
      return _BI2(std::__copy_move_backward_a<_IsMove>
		  (std::__niter_base(__first), std::__niter_base(__last),
		   std::__niter_base(__result)));
    }

  
















 
  template<typename _BI1, typename _BI2>
    inline _BI2
    copy_backward(_BI1 __first, _BI1 __last, _BI2 __result)
    {
      
      
      
      
      ;

      return (std::__copy_move_backward_a2<__is_move_iterator<_BI1>::__value>
	      (std::__miter_base(__first), std::__miter_base(__last),
	       __result));
    }

  
















 
  template<typename _BI1, typename _BI2>
    inline _BI2
    move_backward(_BI1 __first, _BI1 __last, _BI2 __result)
    {
      
      
      
      
      ;

      return std::__copy_move_backward_a2<true>(std::__miter_base(__first),
						std::__miter_base(__last),
						__result);
    }


  template<typename _ForwardIterator, typename _Tp>
    inline typename
    __gnu_cxx::__enable_if<!__is_scalar<_Tp>::__value, void>::__type
    __fill_a(_ForwardIterator __first, _ForwardIterator __last,
 	     const _Tp& __value)
    {
      for (; __first != __last; ++__first)
	*__first = __value;
    }
    
  template<typename _ForwardIterator, typename _Tp>
    inline typename
    __gnu_cxx::__enable_if<__is_scalar<_Tp>::__value, void>::__type
    __fill_a(_ForwardIterator __first, _ForwardIterator __last,
	     const _Tp& __value)
    {
      const _Tp __tmp = __value;
      for (; __first != __last; ++__first)
	*__first = __tmp;
    }

  
  template<typename _Tp>
    inline typename
    __gnu_cxx::__enable_if<__is_byte<_Tp>::__value, void>::__type
    __fill_a(_Tp* __first, _Tp* __last, const _Tp& __c)
    {
      const _Tp __tmp = __c;
      __builtin_memset(__first, static_cast<unsigned char>(__tmp),
		       __last - __first);
    }

  










 
  template<typename _ForwardIterator, typename _Tp>
    inline void
    fill(_ForwardIterator __first, _ForwardIterator __last, const _Tp& __value)
    {
      
      
      ;

      std::__fill_a(std::__niter_base(__first), std::__niter_base(__last),
		    __value);
    }

  template<typename _OutputIterator, typename _Size, typename _Tp>
    inline typename
    __gnu_cxx::__enable_if<!__is_scalar<_Tp>::__value, _OutputIterator>::__type
    __fill_n_a(_OutputIterator __first, _Size __n, const _Tp& __value)
    {
      for (__decltype(__n + 0) __niter = __n;
	   __niter > 0; --__niter, ++__first)
	*__first = __value;
      return __first;
    }

  template<typename _OutputIterator, typename _Size, typename _Tp>
    inline typename
    __gnu_cxx::__enable_if<__is_scalar<_Tp>::__value, _OutputIterator>::__type
    __fill_n_a(_OutputIterator __first, _Size __n, const _Tp& __value)
    {
      const _Tp __tmp = __value;
      for (__decltype(__n + 0) __niter = __n;
	   __niter > 0; --__niter, ++__first)
	*__first = __tmp;
      return __first;
    }

  template<typename _Size, typename _Tp>
    inline typename
    __gnu_cxx::__enable_if<__is_byte<_Tp>::__value, _Tp*>::__type
    __fill_n_a(_Tp* __first, _Size __n, const _Tp& __c)
    {
      std::__fill_a(__first, __first + __n, __c);
      return __first + __n;
    }

  













 
  template<typename _OI, typename _Size, typename _Tp>
    inline _OI
    fill_n(_OI __first, _Size __n, const _Tp& __value)
    {
      
      

      return _OI(std::__fill_n_a(std::__niter_base(__first), __n, __value));
    }

  template<bool _BoolType>
    struct __equal
    {
      template<typename _II1, typename _II2>
        static bool
        equal(_II1 __first1, _II1 __last1, _II2 __first2)
        {
	  for (; __first1 != __last1; ++__first1, ++__first2)
	    if (!(*__first1 == *__first2))
	      return false;
	  return true;
	}
    };

  template<>
    struct __equal<true>
    {
      template<typename _Tp>
        static bool
        equal(const _Tp* __first1, const _Tp* __last1, const _Tp* __first2)
        {
	  return !__builtin_memcmp(__first1, __first2, sizeof(_Tp)
				   * (__last1 - __first1));
	}
    };

  template<typename _II1, typename _II2>
    inline bool
    __equal_aux(_II1 __first1, _II1 __last1, _II2 __first2)
    {
      typedef typename iterator_traits<_II1>::value_type _ValueType1;
      typedef typename iterator_traits<_II2>::value_type _ValueType2;
      const bool __simple = ((__is_integer<_ValueType1>::__value
			      || __is_pointer<_ValueType1>::__value)
	                     && __is_pointer<_II1>::__value
	                     && __is_pointer<_II2>::__value
			     && __are_same<_ValueType1, _ValueType2>::__value);

      return std::__equal<__simple>::equal(__first1, __last1, __first2);
    }


  template<typename, typename>
    struct __lc_rai
    {
      template<typename _II1, typename _II2>
        static _II1
        __newlast1(_II1, _II1 __last1, _II2, _II2)
        { return __last1; }

      template<typename _II>
        static bool
        __cnd2(_II __first, _II __last)
        { return __first != __last; }
    };

  template<>
    struct __lc_rai<random_access_iterator_tag, random_access_iterator_tag>
    {
      template<typename _RAI1, typename _RAI2>
        static _RAI1
        __newlast1(_RAI1 __first1, _RAI1 __last1,
		   _RAI2 __first2, _RAI2 __last2)
        {
	  const typename iterator_traits<_RAI1>::difference_type
	    __diff1 = __last1 - __first1;
	  const typename iterator_traits<_RAI2>::difference_type
	    __diff2 = __last2 - __first2;
	  return __diff2 < __diff1 ? __first1 + __diff2 : __last1;
	}

      template<typename _RAI>
        static bool
        __cnd2(_RAI, _RAI)
        { return true; }
    };

  template<bool _BoolType>
    struct __lexicographical_compare
    {
      template<typename _II1, typename _II2>
        static bool __lc(_II1, _II1, _II2, _II2);
    };

  template<bool _BoolType>
    template<typename _II1, typename _II2>
      bool
      __lexicographical_compare<_BoolType>::
      __lc(_II1 __first1, _II1 __last1, _II2 __first2, _II2 __last2)
      {
	typedef typename iterator_traits<_II1>::iterator_category _Category1;
	typedef typename iterator_traits<_II2>::iterator_category _Category2;
	typedef std::__lc_rai<_Category1, _Category2> 	__rai_type;
	
	__last1 = __rai_type::__newlast1(__first1, __last1,
					 __first2, __last2);
	for (; __first1 != __last1 && __rai_type::__cnd2(__first2, __last2);
	     ++__first1, ++__first2)
	  {
	    if (*__first1 < *__first2)
	      return true;
	    if (*__first2 < *__first1)
	      return false;
	  }
	return __first1 == __last1 && __first2 != __last2;
      }

  template<>
    struct __lexicographical_compare<true>
    {
      template<typename _Tp, typename _Up>
        static bool
        __lc(const _Tp* __first1, const _Tp* __last1,
	     const _Up* __first2, const _Up* __last2)
	{
	  const size_t __len1 = __last1 - __first1;
	  const size_t __len2 = __last2 - __first2;
	  const int __result = __builtin_memcmp(__first1, __first2,
						std::min(__len1, __len2));
	  return __result != 0 ? __result < 0 : __len1 < __len2;
	}
    };

  template<typename _II1, typename _II2>
    inline bool
    __lexicographical_compare_aux(_II1 __first1, _II1 __last1,
				  _II2 __first2, _II2 __last2)
    {
      typedef typename iterator_traits<_II1>::value_type _ValueType1;
      typedef typename iterator_traits<_II2>::value_type _ValueType2;
      const bool __simple =
	(__is_byte<_ValueType1>::__value && __is_byte<_ValueType2>::__value
	 && !__gnu_cxx::__numeric_traits<_ValueType1>::__is_signed
	 && !__gnu_cxx::__numeric_traits<_ValueType2>::__is_signed
	 && __is_pointer<_II1>::__value
	 && __is_pointer<_II2>::__value);

      return std::__lexicographical_compare<__simple>::__lc(__first1, __last1,
							    __first2, __last2);
    }

  









 
  template<typename _ForwardIterator, typename _Tp>
    _ForwardIterator
    lower_bound(_ForwardIterator __first, _ForwardIterator __last,
		const _Tp& __val)
    {
      typedef typename iterator_traits<_ForwardIterator>::difference_type
	_DistanceType;

      
      
      
      ;

      _DistanceType __len = std::distance(__first, __last);

      while (__len > 0)
	{
	  _DistanceType __half = __len >> 1;
	  _ForwardIterator __middle = __first;
	  std::advance(__middle, __half);
	  if (*__middle < __val)
	    {
	      __first = __middle;
	      ++__first;
	      __len = __len - __half - 1;
	    }
	  else
	    __len = __half;
	}
      return __first;
    }

  
  
  inline constexpr int
  __lg(int __n)
  { return sizeof(int) * 8  - 1 - __builtin_clz(__n); }

  inline constexpr unsigned
  __lg(unsigned __n)
  { return sizeof(int) * 8  - 1 - __builtin_clz(__n); }

  inline constexpr long
  __lg(long __n)
  { return sizeof(long) * 8 - 1 - __builtin_clzl(__n); }

  inline constexpr unsigned long
  __lg(unsigned long __n)
  { return sizeof(long) * 8 - 1 - __builtin_clzl(__n); }

  inline constexpr long long
  __lg(long long __n)
  { return sizeof(long long) * 8 - 1 - __builtin_clzll(__n); }

  inline constexpr unsigned long long
  __lg(unsigned long long __n)
  { return sizeof(long long) * 8 - 1 - __builtin_clzll(__n); }





  










 
  template<typename _II1, typename _II2>
    inline bool
    equal(_II1 __first1, _II1 __last1, _II2 __first2)
    {
      
      
      
      
      ;

      return std::__equal_aux(std::__niter_base(__first1),
			      std::__niter_base(__last1),
			      std::__niter_base(__first2));
    }

  













 
  template<typename _IIter1, typename _IIter2, typename _BinaryPredicate>
    inline bool
    equal(_IIter1 __first1, _IIter1 __last1,
	  _IIter2 __first2, _BinaryPredicate __binary_pred)
    {
      
      
      
      ;

      for (; __first1 != __last1; ++__first1, ++__first2)
	if (!bool(__binary_pred(*__first1, *__first2)))
	  return false;
      return true;
    }

  













 
  template<typename _II1, typename _II2>
    inline bool
    lexicographical_compare(_II1 __first1, _II1 __last1,
			    _II2 __first2, _II2 __last2)
    {
      
      
      
      
      ;
      ;

      return std::__lexicographical_compare_aux(std::__niter_base(__first1),
						std::__niter_base(__last1),
						std::__niter_base(__first2),
						std::__niter_base(__last2));
    }

  











 
  template<typename _II1, typename _II2, typename _Compare>
    bool
    lexicographical_compare(_II1 __first1, _II1 __last1,
			    _II2 __first2, _II2 __last2, _Compare __comp)
    {
      typedef typename iterator_traits<_II1>::iterator_category _Category1;
      typedef typename iterator_traits<_II2>::iterator_category _Category2;
      typedef std::__lc_rai<_Category1, _Category2> 	__rai_type;

      
      
      
      ;
      ;

      __last1 = __rai_type::__newlast1(__first1, __last1, __first2, __last2);
      for (; __first1 != __last1 && __rai_type::__cnd2(__first2, __last2);
	   ++__first1, ++__first2)
	{
	  if (__comp(*__first1, *__first2))
	    return true;
	  if (__comp(*__first2, *__first1))
	    return false;
	}
      return __first1 == __last1 && __first2 != __last2;
    }

  











 
  template<typename _InputIterator1, typename _InputIterator2>
    pair<_InputIterator1, _InputIterator2>
    mismatch(_InputIterator1 __first1, _InputIterator1 __last1,
	     _InputIterator2 __first2)
    {
      
      
      
      
      ;

      while (__first1 != __last1 && *__first1 == *__first2)
        {
	  ++__first1;
	  ++__first2;
        }
      return pair<_InputIterator1, _InputIterator2>(__first1, __first2);
    }

  














 
  template<typename _InputIterator1, typename _InputIterator2,
	   typename _BinaryPredicate>
    pair<_InputIterator1, _InputIterator2>
    mismatch(_InputIterator1 __first1, _InputIterator1 __last1,
	     _InputIterator2 __first2, _BinaryPredicate __binary_pred)
    {
      
      
      
      ;

      while (__first1 != __last1 && bool(__binary_pred(*__first1, *__first2)))
        {
	  ++__first1;
	  ++__first2;
        }
      return pair<_InputIterator1, _InputIterator2>(__first1, __first2);
    }


} 








































 




 





























 




























 

































 


#pragma GCC system_header




























 


#pragma GCC system_header

#pragma GCC visibility push(default)




























 


#pragma GCC system_header




 







 





extern "C++" {

namespace std
{
  





 

  






 
  class exception
  {
  public:
    exception() noexcept { }
    virtual ~exception() noexcept;

    
 
    virtual const char* what() const noexcept;
  };

  
 
  class bad_exception : public exception
  {
  public:
    bad_exception() noexcept { }

    
    
    virtual ~bad_exception() noexcept;

    
    virtual const char* what() const noexcept;
  };

  
  typedef void (*terminate_handler) ();

  
  typedef void (*unexpected_handler) ();

  
  terminate_handler set_terminate(terminate_handler) noexcept;

  
 
  void terminate() noexcept __attribute__ ((__noreturn__));

  
  unexpected_handler set_unexpected(unexpected_handler) noexcept;

  
 
  void unexpected() __attribute__ ((__noreturn__));

  









 
  bool uncaught_exception() noexcept __attribute__ ((__pure__));

  
} 

namespace __gnu_cxx
{


  














 
  void __verbose_terminate_handler();


} 

} 

#pragma GCC visibility pop





























 


#pragma GCC visibility push(default)



extern "C++" {

namespace std 
{
  class type_info;

  


 
  namespace __exception_ptr
  {
    class exception_ptr;
  }

  using __exception_ptr::exception_ptr;

  


 
  exception_ptr current_exception() noexcept;

  
  void rethrow_exception(exception_ptr) __attribute__ ((__noreturn__));

  namespace __exception_ptr
  {
    


 
    class exception_ptr
    {
      void* _M_exception_object;

      explicit exception_ptr(void* __e) noexcept;

      void _M_addref() noexcept;
      void _M_release() noexcept;

      void *_M_get() const noexcept __attribute__ ((__pure__));

      friend exception_ptr std::current_exception() noexcept;
      friend void std::rethrow_exception(exception_ptr);

    public:
      exception_ptr() noexcept;

      exception_ptr(const exception_ptr&) noexcept;

      exception_ptr(nullptr_t) noexcept
      : _M_exception_object(0)
      { }

      exception_ptr(exception_ptr&& __o) noexcept
      : _M_exception_object(__o._M_exception_object)
      { __o._M_exception_object = 0; }


      exception_ptr& 
      operator=(const exception_ptr&) noexcept;

      exception_ptr& 
      operator=(exception_ptr&& __o) noexcept
      {
        exception_ptr(static_cast<exception_ptr&&>(__o)).swap(*this);
        return *this;
      }

      ~exception_ptr() noexcept;

      void 
      swap(exception_ptr&) noexcept;


      explicit operator bool() const
      { return _M_exception_object; }

      friend bool 
      operator==(const exception_ptr&, const exception_ptr&)
	noexcept __attribute__ ((__pure__));

      const class std::type_info*
      __cxa_exception_type() const noexcept
	__attribute__ ((__pure__));
    };

    bool 
    operator==(const exception_ptr&, const exception_ptr&)
      noexcept __attribute__ ((__pure__));

    bool 
    operator!=(const exception_ptr&, const exception_ptr&)
      noexcept __attribute__ ((__pure__));

    inline void
    swap(exception_ptr& __lhs, exception_ptr& __rhs)
    { __lhs.swap(__rhs); }

  } 


  
  template<typename _Ex>
    exception_ptr 
    copy_exception(_Ex __ex) noexcept
    {
      try
	{
	  throw __ex;
	}
      catch(...)
	{
	  return current_exception();
	}
    }

  
  
  
  template<typename _Ex>
    exception_ptr 
    make_exception_ptr(_Ex __ex) noexcept
    { return std::copy_exception<_Ex>(__ex); }

  
} 

} 

#pragma GCC visibility pop




























 


#pragma GCC visibility push(default)




extern "C++" {

namespace std
{
  


 

  
  class nested_exception
  {
    exception_ptr _M_ptr;

  public:
    nested_exception() noexcept : _M_ptr(current_exception()) { }

    nested_exception(const nested_exception&) = default;

    nested_exception& operator=(const nested_exception&) = default;

    virtual ~nested_exception() noexcept;

    void
    rethrow_nested() const __attribute__ ((__noreturn__))
    { rethrow_exception(_M_ptr); }

    exception_ptr
    nested_ptr() const
    { return _M_ptr; }
  };

  template<typename _Except>
    struct _Nested_exception : public _Except, public nested_exception
    {
      explicit _Nested_exception(_Except&& __ex)
      : _Except(static_cast<_Except&&>(__ex))
      { }
    };

  template<typename _Ex>
    struct __get_nested_helper
    {
      static const nested_exception*
      _S_get(const _Ex& __ex)
      { return dynamic_cast<const nested_exception*>(&__ex); }
    };

  template<typename _Ex>
    struct __get_nested_helper<_Ex*>
    {
      static const nested_exception*
      _S_get(const _Ex* __ex)
      { return dynamic_cast<const nested_exception*>(__ex); }
    };

  template<typename _Ex>
    inline const nested_exception*
    __get_nested_exception(const _Ex& __ex)
    { return __get_nested_helper<_Ex>::_S_get(__ex); }

  template<typename _Ex>
    void
    __throw_with_nested(_Ex&&, const nested_exception* = 0)
    __attribute__ ((__noreturn__));

  template<typename _Ex>
    void
    __throw_with_nested(_Ex&&, ...) __attribute__ ((__noreturn__));

  
  
  
  template<typename _Ex>
    inline void
    __throw_with_nested(_Ex&& __ex, const nested_exception*)
    { throw __ex; }

  template<typename _Ex>
    inline void
    __throw_with_nested(_Ex&& __ex, ...)
    { throw _Nested_exception<_Ex>(static_cast<_Ex&&>(__ex)); }
  
  template<typename _Ex>
    void
    throw_with_nested(_Ex __ex) __attribute__ ((__noreturn__));

  
  
  template<typename _Ex>
    inline void
    throw_with_nested(_Ex __ex)
    {
      if (__get_nested_exception(__ex))
        throw __ex;
      __throw_with_nested(static_cast<_Ex&&>(__ex), &__ex);
    }

  
  template<typename _Ex>
    inline void
    rethrow_if_nested(const _Ex& __ex)
    {
      if (const nested_exception* __nested = __get_nested_exception(__ex))
        __nested->rethrow_nested();
    }

  
  inline void
  rethrow_if_nested(const nested_exception& __ex)
  { __ex.rethrow_nested(); }

  
} 

} 


#pragma GCC visibility pop



#pragma GCC visibility push(default)

extern "C++" {

namespace std 
{
  




 
  class bad_alloc : public exception 
  {
  public:
    bad_alloc() throw() { }

    
    
    virtual ~bad_alloc() throw();

    
    virtual const char* what() const throw();
  };

  struct nothrow_t { };

  extern const nothrow_t nothrow;

  
 
  typedef void (*new_handler)();

  
  
  new_handler set_new_handler(new_handler) throw();
} 











 
void* operator new(std::size_t) 
  __attribute__((__externally_visible__));
void* operator new[](std::size_t) 
  __attribute__((__externally_visible__));
void operator delete(void*) noexcept
  __attribute__((__externally_visible__));
void operator delete[](void*) noexcept
  __attribute__((__externally_visible__));
void* operator new(std::size_t, const std::nothrow_t&) noexcept
  __attribute__((__externally_visible__));
void* operator new[](std::size_t, const std::nothrow_t&) noexcept
  __attribute__((__externally_visible__));
void operator delete(void*, const std::nothrow_t&) noexcept
  __attribute__((__externally_visible__));
void operator delete[](void*, const std::nothrow_t&) noexcept
  __attribute__((__externally_visible__));


inline void* operator new(std::size_t, void* __p) noexcept
{ return __p; }
inline void* operator new[](std::size_t, void* __p) noexcept
{ return __p; }


inline void operator delete  (void*, void*) noexcept { }
inline void operator delete[](void*, void*) noexcept { }

} 

#pragma GCC visibility pop


namespace __gnu_cxx 
{


  using std::size_t;
  using std::ptrdiff_t;

  








 
  template<typename _Tp>
    class new_allocator
    {
    public:
      typedef size_t     size_type;
      typedef ptrdiff_t  difference_type;
      typedef _Tp*       pointer;
      typedef const _Tp* const_pointer;
      typedef _Tp&       reference;
      typedef const _Tp& const_reference;
      typedef _Tp        value_type;

      template<typename _Tp1>
        struct rebind
        { typedef new_allocator<_Tp1> other; };

      
      
      typedef std::true_type propagate_on_container_move_assignment;

      new_allocator() noexcept { }

      new_allocator(const new_allocator&) noexcept { }

      template<typename _Tp1>
        new_allocator(const new_allocator<_Tp1>&) noexcept { }

      ~new_allocator() noexcept { }

      pointer
      address(reference __x) const noexcept
      { return std::__addressof(__x); }

      const_pointer
      address(const_reference __x) const noexcept
      { return std::__addressof(__x); }

      
      
      pointer
      allocate(size_type __n, const void* = 0)
      { 
	if (__n > this->max_size())
	  std::__throw_bad_alloc();

	return static_cast<_Tp*>(::operator new(__n * sizeof(_Tp)));
      }

      
      void
      deallocate(pointer __p, size_type)
      { ::operator delete(__p); }

      size_type
      max_size() const noexcept
      { return size_t(-1) / sizeof(_Tp); }

      template<typename _Up, typename... _Args>
        void
        construct(_Up* __p, _Args&&... __args)
	{ ::new((void *)__p) _Up(std::forward<_Args>(__args)...); }

      template<typename _Up>
        void 
        destroy(_Up* __p) { __p->~_Up(); }
    };

  template<typename _Tp>
    inline bool
    operator==(const new_allocator<_Tp>&, const new_allocator<_Tp>&)
    { return true; }
  
  template<typename _Tp>
    inline bool
    operator!=(const new_allocator<_Tp>&, const new_allocator<_Tp>&)
    { return false; }


} 


namespace std
{
  







 
  template<typename _Tp>
    using __allocator_base = __gnu_cxx::new_allocator<_Tp>;
}




































 




 


#pragma GCC system_header


namespace std 
{


  






 

  template<typename>
    class allocator;

  template<>
    class allocator<void>;

  
  template<typename, typename>
    struct uses_allocator;

  


} 


namespace std 
{


  


 

  
  template<>
    class allocator<void>
    {
    public:
      typedef size_t      size_type;
      typedef ptrdiff_t   difference_type;
      typedef void*       pointer;
      typedef const void* const_pointer;
      typedef void        value_type;

      template<typename _Tp1>
        struct rebind
        { typedef allocator<_Tp1> other; };

      
      
      typedef true_type propagate_on_container_move_assignment;
    };

  






 
  template<typename _Tp>
    class allocator: public __allocator_base<_Tp>
    {
   public:
      typedef size_t     size_type;
      typedef ptrdiff_t  difference_type;
      typedef _Tp*       pointer;
      typedef const _Tp* const_pointer;
      typedef _Tp&       reference;
      typedef const _Tp& const_reference;
      typedef _Tp        value_type;

      template<typename _Tp1>
        struct rebind
        { typedef allocator<_Tp1> other; };

      
      
      typedef true_type propagate_on_container_move_assignment;

      allocator() throw() { }

      allocator(const allocator& __a) throw()
      : __allocator_base<_Tp>(__a) { }

      template<typename _Tp1>
        allocator(const allocator<_Tp1>&) throw() { }

      ~allocator() throw() { }

      
    };

  template<typename _T1, typename _T2>
    inline bool
    operator==(const allocator<_T1>&, const allocator<_T2>&)
    { return true; }

  template<typename _Tp>
    inline bool
    operator==(const allocator<_Tp>&, const allocator<_Tp>&)
    { return true; }

  template<typename _T1, typename _T2>
    inline bool
    operator!=(const allocator<_T1>&, const allocator<_T2>&)
    { return false; }

  template<typename _Tp>
    inline bool
    operator!=(const allocator<_Tp>&, const allocator<_Tp>&)
    { return false; }

  

  
  
  extern template class allocator<char>;
  extern template class allocator<wchar_t>;

  

  
  template<typename _Alloc, bool = __is_empty(_Alloc)>
    struct __alloc_swap
    { static void _S_do_it(_Alloc&, _Alloc&) { } };

  template<typename _Alloc>
    struct __alloc_swap<_Alloc, false>
    {
      static void
      _S_do_it(_Alloc& __one, _Alloc& __two)
      {
	
	if (__one != __two)
	  swap(__one, __two);
      }
    };

  
  template<typename _Alloc, bool = __is_empty(_Alloc)>
    struct __alloc_neq
    {
      static bool
      _S_do_it(const _Alloc&, const _Alloc&)
      { return false; }
    };

  template<typename _Alloc>
    struct __alloc_neq<_Alloc, false>
    {
      static bool
      _S_do_it(const _Alloc& __one, const _Alloc& __two)
      { return __one != __two; }
    };

  template<typename _Tp, bool
    = __or_<is_copy_constructible<typename _Tp::value_type>,
            is_nothrow_move_constructible<typename _Tp::value_type>>::value>
    struct __shrink_to_fit_aux
    { static bool _S_do_it(_Tp&) { return false; } };

  template<typename _Tp>
    struct __shrink_to_fit_aux<_Tp, true>
    {
      static bool
      _S_do_it(_Tp& __c)
      {
	try
	  {
	    _Tp(__make_move_if_noexcept_iterator(__c.begin()),
		__make_move_if_noexcept_iterator(__c.end()),
		__c.get_allocator()).swap(__c);
	    return true;
	  }
	catch(...)
	  { return false; }
      }
    };


} 

















































 




 




























 


#pragma GCC system_header




























 






























 




namespace std 
{


template<typename _Tp> class __has_element_type_helper : __sfinae_types { template<typename _Up> struct _Wrap_type { }; template<typename _Up> static __one __test(_Wrap_type<typename _Up::element_type> *); template<typename _Up> static __two __test(...); public: static constexpr bool value = sizeof(__test<_Tp>(0)) == 1; }; template<typename _Tp> struct __has_element_type : integral_constant<bool, __has_element_type_helper <typename remove_cv<_Tp> ::type> ::value> { };
template<typename _Tp> class __has_difference_type_helper : __sfinae_types { template<typename _Up> struct _Wrap_type { }; template<typename _Up> static __one __test(_Wrap_type<typename _Up::difference_type> *); template<typename _Up> static __two __test(...); public: static constexpr bool value = sizeof(__test<_Tp>(0)) == 1; }; template<typename _Tp> struct __has_difference_type : integral_constant<bool, __has_difference_type_helper <typename remove_cv<_Tp> ::type> ::value> { };

  template<typename _Tp, bool = __has_element_type<_Tp>::value>
    struct __ptrtr_elt_type;

  template<typename _Tp>
    struct __ptrtr_elt_type<_Tp, true>
    {
      typedef typename _Tp::element_type __type;
    };

  template<template<typename, typename...> class _SomePtr, typename _Tp,
            typename... _Args>
    struct __ptrtr_elt_type<_SomePtr<_Tp, _Args...>, false>
    {
      typedef _Tp __type;
    };

  template<typename _Tp, bool = __has_difference_type<_Tp>::value>
    struct __ptrtr_diff_type
    {
      typedef typename _Tp::difference_type __type;
    };

  template<typename _Tp>
    struct __ptrtr_diff_type<_Tp, false>
    {
      typedef ptrdiff_t __type;
    };

  template<typename _Ptr, typename _Up>
    class __ptrtr_rebind_helper
    {
      template<typename _Ptr2, typename _Up2>
	static constexpr bool
       	_S_chk(typename _Ptr2::template rebind<_Up2>*)
       	{ return true; }

      template<typename, typename>
        static constexpr bool
       	_S_chk(...)
       	{ return false; }

    public:
      static const bool __value = _S_chk<_Ptr, _Up>(nullptr);
    };

  template<typename _Ptr, typename _Up>
    const bool __ptrtr_rebind_helper<_Ptr, _Up>::__value;

  template<typename _Tp, typename _Up,
           bool = __ptrtr_rebind_helper<_Tp, _Up>::__value>
    struct __ptrtr_rebind;

  template<typename _Tp, typename _Up>
    struct __ptrtr_rebind<_Tp, _Up, true>
    {
      typedef typename _Tp::template rebind<_Up> __type;
    };

  template<template<typename, typename...> class _SomePtr, typename _Up,
            typename _Tp, typename... _Args>
    struct __ptrtr_rebind<_SomePtr<_Tp, _Args...>, _Up, false>
    {
      typedef _SomePtr<_Up, _Args...> __type;
    };

  template<typename _Tp, typename = typename remove_cv<_Tp>::type>
    struct __ptrtr_not_void
    {
      typedef _Tp __type;
    };

  template<typename _Tp>
    struct __ptrtr_not_void<_Tp, void>
    {
      struct __type { };
    };

  template<typename _Ptr>
    class __ptrtr_pointer_to
    {
      typedef typename __ptrtr_elt_type<_Ptr>::__type   __orig_type;
      typedef typename __ptrtr_not_void<__orig_type>::__type __element_type;

    public:
      static _Ptr pointer_to(__element_type& __e)
      { return _Ptr::pointer_to(__e); }
    };

  


 
  template<typename _Ptr>
    struct pointer_traits : __ptrtr_pointer_to<_Ptr>
    {
      
      typedef _Ptr                                      pointer;
      
      typedef typename __ptrtr_elt_type<_Ptr>::__type   element_type;
      
      typedef typename __ptrtr_diff_type<_Ptr>::__type  difference_type;

      template<typename _Up>
        using rebind = typename __ptrtr_rebind<_Ptr, _Up>::__type;
    };

  


 
  template<typename _Tp>
    struct pointer_traits<_Tp*>
    {
      
      typedef _Tp* pointer;
      
      typedef _Tp  element_type;
      
      typedef ptrdiff_t difference_type;

      template<typename _Up>
        using rebind = _Up*;

      



 
      static pointer
      pointer_to(typename __ptrtr_not_void<element_type>::__type& __r) noexcept
      { return std::addressof(__r); }
    };


} 



namespace std 
{


  template<typename _Alloc, typename _Tp>
    class __alloctr_rebind_helper
    {
      template<typename _Alloc2, typename _Tp2>
	static constexpr bool
       	_S_chk(typename _Alloc2::template rebind<_Tp2>::other*)
	{ return true; }

      template<typename, typename>
        static constexpr bool
       	_S_chk(...)
       	{ return false; }

    public:
      static const bool __value = _S_chk<_Alloc, _Tp>(nullptr);
    };

  template<typename _Alloc, typename _Tp>
    const bool __alloctr_rebind_helper<_Alloc, _Tp>::__value;

  template<typename _Alloc, typename _Tp,
           bool = __alloctr_rebind_helper<_Alloc, _Tp>::__value>
    struct __alloctr_rebind;

  template<typename _Alloc, typename _Tp>
    struct __alloctr_rebind<_Alloc, _Tp, true>
    {
      typedef typename _Alloc::template rebind<_Tp>::other __type;
    };

  template<template<typename, typename...> class _Alloc, typename _Tp,
            typename _Up, typename... _Args>
    struct __alloctr_rebind<_Alloc<_Up, _Args...>, _Tp, false>
    {
      typedef _Alloc<_Tp, _Args...> __type;
    };

  


 
  template<typename _Alloc>
    struct allocator_traits
    {
      
      typedef _Alloc allocator_type;
      
      typedef typename _Alloc::value_type value_type;


private: template<typename _Tp> static typename _Tp::pointer _S_pointer_helper(_Tp*); static value_type* _S_pointer_helper(...); typedef decltype(_S_pointer_helper((_Alloc*)0)) __pointer; public:

      



 
      typedef __pointer pointer;

private: template<typename _Tp> static typename _Tp::const_pointer _S_const_pointer_helper(_Tp*); static typename pointer_traits<pointer> ::template rebind<const value_type> _S_const_pointer_helper(...); typedef decltype(_S_const_pointer_helper((_Alloc*)0)) __const_pointer; public:

      




 
      typedef __const_pointer const_pointer;

private: template<typename _Tp> static typename _Tp::void_pointer _S_void_pointer_helper(_Tp*); static typename pointer_traits<pointer> ::template rebind<void> _S_void_pointer_helper(...); typedef decltype(_S_void_pointer_helper((_Alloc*)0)) __void_pointer; public:

      




 
      typedef __void_pointer void_pointer;

private: template<typename _Tp> static typename _Tp::const_void_pointer _S_const_void_pointer_helper(_Tp*); static typename pointer_traits<pointer> ::template rebind<const void> _S_const_void_pointer_helper(...); typedef decltype(_S_const_void_pointer_helper((_Alloc*)0)) __const_void_pointer; public:

      




 
      typedef __const_void_pointer const_void_pointer;

private: template<typename _Tp> static typename _Tp::difference_type _S_difference_type_helper(_Tp*); static typename pointer_traits<pointer> ::difference_type _S_difference_type_helper(...); typedef decltype(_S_difference_type_helper((_Alloc*)0)) __difference_type; public:

      




 
      typedef __difference_type difference_type;

private: template<typename _Tp> static typename _Tp::size_type _S_size_type_helper(_Tp*); static typename make_unsigned<difference_type> ::type _S_size_type_helper(...); typedef decltype(_S_size_type_helper((_Alloc*)0)) __size_type; public:

      




 
      typedef __size_type size_type;

private: template<typename _Tp> static typename _Tp::propagate_on_container_copy_assignment _S_propagate_on_container_copy_assignment_helper(_Tp*); static false_type _S_propagate_on_container_copy_assignment_helper(...); typedef decltype(_S_propagate_on_container_copy_assignment_helper((_Alloc*)0)) __propagate_on_container_copy_assignment; public:

      




 
      typedef __propagate_on_container_copy_assignment
       	propagate_on_container_copy_assignment;

private: template<typename _Tp> static typename _Tp::propagate_on_container_move_assignment _S_propagate_on_container_move_assignment_helper(_Tp*); static false_type _S_propagate_on_container_move_assignment_helper(...); typedef decltype(_S_propagate_on_container_move_assignment_helper((_Alloc*)0)) __propagate_on_container_move_assignment; public:

      




 
      typedef __propagate_on_container_move_assignment
       	propagate_on_container_move_assignment;

private: template<typename _Tp> static typename _Tp::propagate_on_container_swap _S_propagate_on_container_swap_helper(_Tp*); static false_type _S_propagate_on_container_swap_helper(...); typedef decltype(_S_propagate_on_container_swap_helper((_Alloc*)0)) __propagate_on_container_swap; public:

      




 
      typedef __propagate_on_container_swap propagate_on_container_swap;


      template<typename _Tp>
        using rebind_alloc = typename __alloctr_rebind<_Alloc, _Tp>::__type;
      template<typename _Tp>
        using rebind_traits = allocator_traits<rebind_alloc<_Tp>>;

    private:
      template<typename _Alloc2>
	struct __allocate_helper
	{
	  template<typename _Alloc3,
	    typename = decltype(std::declval<_Alloc3*>()->allocate(
		  std::declval<size_type>(),
		  std::declval<const_void_pointer>()))>
	    static true_type __test(int);

	  template<typename>
	    static false_type __test(...);

	  typedef decltype(__test<_Alloc>(0)) type;
	  static const bool value = type::value;
	};

      template<typename _Alloc2>
	static typename
       	enable_if<__allocate_helper<_Alloc2>::value, pointer>::type
       	_S_allocate(_Alloc2& __a, size_type __n, const_void_pointer __hint)
	{ return __a.allocate(__n, __hint); }

      template<typename _Alloc2>
	static typename
       	enable_if<!__allocate_helper<_Alloc2>::value, pointer>::type
       	_S_allocate(_Alloc2& __a, size_type __n, ...)
	{ return __a.allocate(__n); }

      template<typename _Tp, typename... _Args>
	struct __construct_helper
	{
	  template<typename _Alloc2,
	    typename = decltype(std::declval<_Alloc2*>()->construct(
		  std::declval<_Tp*>(), std::declval<_Args>()...))>
	    static true_type __test(int);

	  template<typename>
	    static false_type __test(...);

	  typedef decltype(__test<_Alloc>(0)) type;
	  static const bool value = type::value;
	};

      template<typename _Tp, typename... _Args>
	static typename
       	enable_if<__construct_helper<_Tp, _Args...>::value, void>::type
       	_S_construct(_Alloc& __a, _Tp* __p, _Args&&... __args)
	{ __a.construct(__p, std::forward<_Args>(__args)...); }

      template<typename _Tp, typename... _Args>
	static typename
	enable_if<__and_<__not_<__construct_helper<_Tp, _Args...>>,
			 is_constructible<_Tp, _Args...>>::value, void>::type
       	_S_construct(_Alloc&, _Tp* __p, _Args&&... __args)
	{ ::new((void*)__p) _Tp(std::forward<_Args>(__args)...); }

      template<typename _Tp>
	struct __destroy_helper
	{
	  template<typename _Alloc2,
	    typename = decltype(std::declval<_Alloc2*>()->destroy(
		  std::declval<_Tp*>()))>
	    static true_type __test(int);

	  template<typename>
	    static false_type __test(...);

	  typedef decltype(__test<_Alloc>(0)) type;
	  static const bool value = type::value;
	};

      template<typename _Tp>
	static typename enable_if<__destroy_helper<_Tp>::value, void>::type
       	_S_destroy(_Alloc& __a, _Tp* __p)
	{ __a.destroy(__p); }

      template<typename _Tp>
	static typename enable_if<!__destroy_helper<_Tp>::value, void>::type
       	_S_destroy(_Alloc&, _Tp* __p)
	{ __p->~_Tp(); }

      template<typename _Alloc2>
	struct __maxsize_helper
	{
	  template<typename _Alloc3,
	    typename = decltype(std::declval<_Alloc3*>()->max_size())>
	    static true_type __test(int);

	  template<typename>
	    static false_type __test(...);

	  typedef decltype(__test<_Alloc2>(0)) type;
	  static const bool value = type::value;
	};

      template<typename _Alloc2>
	static typename
       	enable_if<__maxsize_helper<_Alloc2>::value, size_type>::type
       	_S_max_size(_Alloc2& __a)
	{ return __a.max_size(); }

      template<typename _Alloc2>
	static typename
       	enable_if<!__maxsize_helper<_Alloc2>::value, size_type>::type
	_S_max_size(_Alloc2&)
	{ return __gnu_cxx::__numeric_traits<size_type>::__max; }

      template<typename _Alloc2>
	struct __select_helper
	{
	  template<typename _Alloc3, typename
	    = decltype(std::declval<_Alloc3*>()
		->select_on_container_copy_construction())>
	    static true_type __test(int);

	  template<typename>
	    static false_type __test(...);

	  typedef decltype(__test<_Alloc2>(0)) type;
	  static const bool value = type::value;
	};
      template<typename _Alloc2>
	static typename
       	enable_if<__select_helper<_Alloc2>::value, _Alloc2>::type
       	_S_select(_Alloc2& __a)
	{ return __a.select_on_container_copy_construction(); }

      template<typename _Alloc2>
	static typename
       	enable_if<!__select_helper<_Alloc2>::value, _Alloc2>::type
       	_S_select(_Alloc2& __a)
	{ return __a; }

    public:

      





 
      static pointer
      allocate(_Alloc& __a, size_type __n)
      { return __a.allocate(__n); }

      









 
      static pointer
      allocate(_Alloc& __a, size_type __n, const_void_pointer __hint)
      { return _S_allocate(__a, __n, __hint); }

      






 
      static void deallocate(_Alloc& __a, pointer __p, size_type __n)
      { __a.deallocate(__p, __n); }

      









 
      template<typename _Tp, typename... _Args>
	static auto construct(_Alloc& __a, _Tp* __p, _Args&&... __args)
	-> decltype(_S_construct(__a, __p, std::forward<_Args>(__args)...))
	{ _S_construct(__a, __p, std::forward<_Args>(__args)...); }

      






 
      template <class _Tp>
	static void destroy(_Alloc& __a, _Tp* __p)
	{ _S_destroy(__a, __p); }

      






 
      static size_type max_size(const _Alloc& __a)
      { return _S_max_size(__a); }

      






 
      static _Alloc
      select_on_container_copy_construction(const _Alloc& __rhs)
      { return _S_select(__rhs); }
    };

  template<typename _Alloc>
  template<typename _Alloc2>
    const bool allocator_traits<_Alloc>::__allocate_helper<_Alloc2>::value;

  template<typename _Alloc>
  template<typename _Tp, typename... _Args>
    const bool
    allocator_traits<_Alloc>::__construct_helper<_Tp, _Args...>::value;

  template<typename _Alloc>
  template<typename _Tp>
    const bool allocator_traits<_Alloc>::__destroy_helper<_Tp>::value;

  template<typename _Alloc>
  template<typename _Alloc2>
    const bool allocator_traits<_Alloc>::__maxsize_helper<_Alloc2>::value;

  template<typename _Alloc>
  template<typename _Alloc2>
    const bool allocator_traits<_Alloc>::__select_helper<_Alloc2>::value;

  template<typename _Alloc>
    inline void
    __do_alloc_on_copy(_Alloc& __one, const _Alloc& __two, true_type)
    { __one = __two; }

  template<typename _Alloc>
    inline void
    __do_alloc_on_copy(_Alloc&, const _Alloc&, false_type)
    { }

  template<typename _Alloc>
    inline void __alloc_on_copy(_Alloc& __one, const _Alloc& __two)
    {
      typedef allocator_traits<_Alloc> __traits;
      typedef typename __traits::propagate_on_container_copy_assignment __pocca;
      __do_alloc_on_copy(__one, __two, __pocca());
    }

  template<typename _Alloc>
    inline _Alloc __alloc_on_copy(const _Alloc& __a)
    {
      typedef allocator_traits<_Alloc> __traits;
      return __traits::select_on_container_copy_construction(__a);
    }

  template<typename _Alloc>
    inline void __do_alloc_on_move(_Alloc& __one, _Alloc& __two, true_type)
    { __one = std::move(__two); }

  template<typename _Alloc>
    inline void __do_alloc_on_move(_Alloc&, _Alloc&, false_type)
    { }

  template<typename _Alloc>
    inline void __alloc_on_move(_Alloc& __one, _Alloc& __two)
    {
      typedef allocator_traits<_Alloc> __traits;
      typedef typename __traits::propagate_on_container_move_assignment __pocma;
      __do_alloc_on_move(__one, __two, __pocma());
    }

  template<typename _Alloc>
    inline void __do_alloc_on_swap(_Alloc& __one, _Alloc& __two, true_type)
    {
      using std::swap;
      swap(__one, __two);
    }

  template<typename _Alloc>
    inline void __do_alloc_on_swap(_Alloc&, _Alloc&, false_type)
    { }

  template<typename _Alloc>
    inline void __alloc_on_swap(_Alloc& __one, _Alloc& __two)
    {
      typedef allocator_traits<_Alloc> __traits;
      typedef typename __traits::propagate_on_container_swap __pocs;
      __do_alloc_on_swap(__one, __two, __pocs());
    }

  template<typename _Alloc>
    class __is_copy_insertable_impl
    {
      typedef allocator_traits<_Alloc> _Traits;

      template<typename _Up, typename
	       = decltype(_Traits::construct(std::declval<_Alloc&>(),
					     std::declval<_Up*>(),
					     std::declval<const _Up&>()))>
	static true_type
	_M_select(int);

      template<typename _Up>
	static false_type
	_M_select(...);

    public:
      typedef decltype(_M_select<typename _Alloc::value_type>(0)) type;
    };

  
  template<typename _Alloc>
    struct __is_copy_insertable
    : __is_copy_insertable_impl<_Alloc>::type
    { };

  
  template<typename _Tp>
    struct __is_copy_insertable<allocator<_Tp>>
    : is_copy_constructible<_Tp>
    { };

  
  template<bool> struct __allow_copy_cons { };

  
  template<>
    struct __allow_copy_cons<false>
    {
      __allow_copy_cons() = default;
      __allow_copy_cons(const __allow_copy_cons&) = delete;
      __allow_copy_cons(__allow_copy_cons&&) = default;
      __allow_copy_cons& operator=(const __allow_copy_cons&) = default;
      __allow_copy_cons& operator=(__allow_copy_cons&&) = default;
    };

  template<typename _Alloc>
    using __check_copy_constructible
      = __allow_copy_cons<__is_copy_insertable<_Alloc>::value>;


} 


namespace __gnu_cxx 
{


  template<typename _Alloc>
    struct __allocator_always_compares_equal
    { static const bool value = false; };

  template<typename _Alloc>
    const bool __allocator_always_compares_equal<_Alloc>::value;

  template<typename _Tp>
    struct __allocator_always_compares_equal<std::allocator<_Tp>>
    { static const bool value = true; };

  template<typename _Tp>
    const bool __allocator_always_compares_equal<std::allocator<_Tp>>::value;

  template<typename, typename> struct array_allocator;

  template<typename _Tp, typename _Array>
    struct __allocator_always_compares_equal<array_allocator<_Tp, _Array>>
    { static const bool value = true; };

  template<typename _Tp, typename _Array>
    const bool
    __allocator_always_compares_equal<array_allocator<_Tp, _Array>>::value;

  template<typename> struct bitmap_allocator;

  template<typename _Tp>
    struct __allocator_always_compares_equal<bitmap_allocator<_Tp>>
    { static const bool value = true; };

  template<typename _Tp>
    const bool __allocator_always_compares_equal<bitmap_allocator<_Tp>>::value;

  template<typename> struct malloc_allocator;

  template<typename _Tp>
    struct __allocator_always_compares_equal<malloc_allocator<_Tp>>
    { static const bool value = true; };

  template<typename _Tp>
    const bool __allocator_always_compares_equal<malloc_allocator<_Tp>>::value;

  template<typename> struct mt_allocator;

  template<typename _Tp>
    struct __allocator_always_compares_equal<mt_allocator<_Tp>>
    { static const bool value = true; };

  template<typename _Tp>
    const bool __allocator_always_compares_equal<mt_allocator<_Tp>>::value;

  template<typename> struct new_allocator;

  template<typename _Tp>
    struct __allocator_always_compares_equal<new_allocator<_Tp>>
    { static const bool value = true; };

  template<typename _Tp>
    const bool __allocator_always_compares_equal<new_allocator<_Tp>>::value;

  template<typename> struct pool_allocator;

  template<typename _Tp>
    struct __allocator_always_compares_equal<pool_allocator<_Tp>>
    { static const bool value = true; };

  template<typename _Tp>
    const bool __allocator_always_compares_equal<pool_allocator<_Tp>>::value;




 
template<typename _Alloc>
  struct __alloc_traits
  : std::allocator_traits<_Alloc>
  {
    typedef _Alloc allocator_type;
    typedef std::allocator_traits<_Alloc>           _Base_type;
    typedef typename _Base_type::value_type         value_type;
    typedef typename _Base_type::pointer            pointer;
    typedef typename _Base_type::const_pointer      const_pointer;
    typedef typename _Base_type::size_type          size_type;
    typedef typename _Base_type::difference_type    difference_type;
    
    typedef value_type&                             reference;
    typedef const value_type&                       const_reference;
    using _Base_type::allocate;
    using _Base_type::deallocate;
    using _Base_type::construct;
    using _Base_type::destroy;
    using _Base_type::max_size;

  private:
    template<typename _Ptr>
      struct __is_custom_pointer
      : std::integral_constant<bool, std::is_same<pointer, _Ptr>::value
                                     && !std::is_pointer<_Ptr>::value>
      { };

  public:
    
    template<typename _Ptr, typename... _Args>
      static typename std::enable_if<__is_custom_pointer<_Ptr>::value>::type
      construct(_Alloc& __a, _Ptr __p, _Args&&... __args)
      {
	_Base_type::construct(__a, std::addressof(*__p),
			      std::forward<_Args>(__args)...);
      }

    
    template<typename _Ptr>
      static typename std::enable_if<__is_custom_pointer<_Ptr>::value>::type
      destroy(_Alloc& __a, _Ptr __p)
      { _Base_type::destroy(__a, std::addressof(*__p)); }

    static _Alloc _S_select_on_copy(const _Alloc& __a)
    { return _Base_type::select_on_container_copy_construction(__a); }

    static void _S_on_swap(_Alloc& __a, _Alloc& __b)
    { std::__alloc_on_swap(__a, __b); }

    static constexpr bool _S_propagate_on_copy_assign()
    { return _Base_type::propagate_on_container_copy_assignment::value; }

    static constexpr bool _S_propagate_on_move_assign()
    { return _Base_type::propagate_on_container_move_assignment::value; }

    static constexpr bool _S_propagate_on_swap()
    { return _Base_type::propagate_on_container_swap::value; }

    static constexpr bool _S_always_equal()
    { return __allocator_always_compares_equal<_Alloc>::value; }

    static constexpr bool _S_nothrow_move()
    { return _S_propagate_on_move_assign() || _S_always_equal(); }

    static constexpr bool _S_nothrow_swap()
    {
      using std::swap;
      return !_S_propagate_on_swap()
       	|| noexcept(swap(std::declval<_Alloc&>(), std::declval<_Alloc&>()));
    }

    template<typename _Tp>
      struct rebind
      { typedef typename _Base_type::template rebind_alloc<_Tp> other; };
  };


} 


namespace std 
{


  


 
  template<typename _T1, typename... _Args>
    inline void
    _Construct(_T1* __p, _Args&&... __args)
    { ::new(static_cast<void*>(__p)) _T1(std::forward<_Args>(__args)...); }

  

 
  template<typename _Tp>
    inline void
    _Destroy(_Tp* __pointer)
    { __pointer->~_Tp(); }

  template<bool>
    struct _Destroy_aux
    {
      template<typename _ForwardIterator>
        static void
        __destroy(_ForwardIterator __first, _ForwardIterator __last)
	{
	  for (; __first != __last; ++__first)
	    std::_Destroy(std::__addressof(*__first));
	}
    };

  template<>
    struct _Destroy_aux<true>
    {
      template<typename _ForwardIterator>
        static void
        __destroy(_ForwardIterator, _ForwardIterator) { }
    };

  



 
  template<typename _ForwardIterator>
    inline void
    _Destroy(_ForwardIterator __first, _ForwardIterator __last)
    {
      typedef typename iterator_traits<_ForwardIterator>::value_type
                       _Value_type;
      std::_Destroy_aux<__has_trivial_destructor(_Value_type)>::
	__destroy(__first, __last);
    }

  



 

  template<typename _ForwardIterator, typename _Allocator>
    void
    _Destroy(_ForwardIterator __first, _ForwardIterator __last,
	     _Allocator& __alloc)
    {
      typedef __gnu_cxx::__alloc_traits<_Allocator> __traits;
      for (; __first != __last; ++__first)
	__traits::destroy(__alloc, std::__addressof(*__first));
    }

  template<typename _ForwardIterator, typename _Tp>
    inline void
    _Destroy(_ForwardIterator __first, _ForwardIterator __last,
	     allocator<_Tp>&)
    {
      _Destroy(__first, __last);
    }


} 


















































 




 


namespace std 
{


  template<bool _TrivialValueTypes>
    struct __uninitialized_copy
    {
      template<typename _InputIterator, typename _ForwardIterator>
        static _ForwardIterator
        __uninit_copy(_InputIterator __first, _InputIterator __last,
		      _ForwardIterator __result)
        {
	  _ForwardIterator __cur = __result;
	  try
	    {
	      for (; __first != __last; ++__first, ++__cur)
		std::_Construct(std::__addressof(*__cur), *__first);
	      return __cur;
	    }
	  catch(...)
	    {
	      std::_Destroy(__result, __cur);
	      throw;
	    }
	}
    };

  template<>
    struct __uninitialized_copy<true>
    {
      template<typename _InputIterator, typename _ForwardIterator>
        static _ForwardIterator
        __uninit_copy(_InputIterator __first, _InputIterator __last,
		      _ForwardIterator __result)
        { return std::copy(__first, __last, __result); }
    };

  







 
  template<typename _InputIterator, typename _ForwardIterator>
    inline _ForwardIterator
    uninitialized_copy(_InputIterator __first, _InputIterator __last,
		       _ForwardIterator __result)
    {
      typedef typename iterator_traits<_InputIterator>::value_type
	_ValueType1;
      typedef typename iterator_traits<_ForwardIterator>::value_type
	_ValueType2;

      return std::__uninitialized_copy<(__is_trivial(_ValueType1)
					&& __is_trivial(_ValueType2))>::
	__uninit_copy(__first, __last, __result);
    }


  template<bool _TrivialValueType>
    struct __uninitialized_fill
    {
      template<typename _ForwardIterator, typename _Tp>
        static void
        __uninit_fill(_ForwardIterator __first, _ForwardIterator __last,
		      const _Tp& __x)
        {
	  _ForwardIterator __cur = __first;
	  try
	    {
	      for (; __cur != __last; ++__cur)
		std::_Construct(std::__addressof(*__cur), __x);
	    }
	  catch(...)
	    {
	      std::_Destroy(__first, __cur);
	      throw;
	    }
	}
    };

  template<>
    struct __uninitialized_fill<true>
    {
      template<typename _ForwardIterator, typename _Tp>
        static void
        __uninit_fill(_ForwardIterator __first, _ForwardIterator __last,
		      const _Tp& __x)
        { std::fill(__first, __last, __x); }
    };

  







 
  template<typename _ForwardIterator, typename _Tp>
    inline void
    uninitialized_fill(_ForwardIterator __first, _ForwardIterator __last,
		       const _Tp& __x)
    {
      typedef typename iterator_traits<_ForwardIterator>::value_type
	_ValueType;

      std::__uninitialized_fill<__is_trivial(_ValueType)>::
	__uninit_fill(__first, __last, __x);
    }


  template<bool _TrivialValueType>
    struct __uninitialized_fill_n
    {
      template<typename _ForwardIterator, typename _Size, typename _Tp>
        static void
        __uninit_fill_n(_ForwardIterator __first, _Size __n,
			const _Tp& __x)
        {
	  _ForwardIterator __cur = __first;
	  try
	    {
	      for (; __n > 0; --__n, ++__cur)
		std::_Construct(std::__addressof(*__cur), __x);
	    }
	  catch(...)
	    {
	      std::_Destroy(__first, __cur);
	      throw;
	    }
	}
    };

  template<>
    struct __uninitialized_fill_n<true>
    {
      template<typename _ForwardIterator, typename _Size, typename _Tp>
        static void
        __uninit_fill_n(_ForwardIterator __first, _Size __n,
			const _Tp& __x)
        { std::fill_n(__first, __n, __x); }
    };

  







 
  template<typename _ForwardIterator, typename _Size, typename _Tp>
    inline void
    uninitialized_fill_n(_ForwardIterator __first, _Size __n, const _Tp& __x)
    {
      typedef typename iterator_traits<_ForwardIterator>::value_type
	_ValueType;

      std::__uninitialized_fill_n<__is_trivial(_ValueType)>::
	__uninit_fill_n(__first, __n, __x);
    }

  
  
  
  
  

  template<typename _InputIterator, typename _ForwardIterator,
	   typename _Allocator>
    _ForwardIterator
    __uninitialized_copy_a(_InputIterator __first, _InputIterator __last,
			   _ForwardIterator __result, _Allocator& __alloc)
    {
      _ForwardIterator __cur = __result;
      try
	{
	  typedef __gnu_cxx::__alloc_traits<_Allocator> __traits;
	  for (; __first != __last; ++__first, ++__cur)
	    __traits::construct(__alloc, std::__addressof(*__cur), *__first);
	  return __cur;
	}
      catch(...)
	{
	  std::_Destroy(__result, __cur, __alloc);
	  throw;
	}
    }

  template<typename _InputIterator, typename _ForwardIterator, typename _Tp>
    inline _ForwardIterator
    __uninitialized_copy_a(_InputIterator __first, _InputIterator __last,
			   _ForwardIterator __result, allocator<_Tp>&)
    { return std::uninitialized_copy(__first, __last, __result); }

  template<typename _InputIterator, typename _ForwardIterator,
	   typename _Allocator>
    inline _ForwardIterator
    __uninitialized_move_a(_InputIterator __first, _InputIterator __last,
			   _ForwardIterator __result, _Allocator& __alloc)
    {
      return std::__uninitialized_copy_a(std::make_move_iterator(__first),
					 std::make_move_iterator(__last),
					 __result, __alloc);
    }

  template<typename _InputIterator, typename _ForwardIterator,
	   typename _Allocator>
    inline _ForwardIterator
    __uninitialized_move_if_noexcept_a(_InputIterator __first,
				       _InputIterator __last,
				       _ForwardIterator __result,
				       _Allocator& __alloc)
    {
      return std::__uninitialized_copy_a
	(std::__make_move_if_noexcept_iterator(__first),
	 std::__make_move_if_noexcept_iterator(__last), __result, __alloc);
    }

  template<typename _ForwardIterator, typename _Tp, typename _Allocator>
    void
    __uninitialized_fill_a(_ForwardIterator __first, _ForwardIterator __last,
			   const _Tp& __x, _Allocator& __alloc)
    {
      _ForwardIterator __cur = __first;
      try
	{
	  typedef __gnu_cxx::__alloc_traits<_Allocator> __traits;
	  for (; __cur != __last; ++__cur)
	    __traits::construct(__alloc, std::__addressof(*__cur), __x);
	}
      catch(...)
	{
	  std::_Destroy(__first, __cur, __alloc);
	  throw;
	}
    }

  template<typename _ForwardIterator, typename _Tp, typename _Tp2>
    inline void
    __uninitialized_fill_a(_ForwardIterator __first, _ForwardIterator __last,
			   const _Tp& __x, allocator<_Tp2>&)
    { std::uninitialized_fill(__first, __last, __x); }

  template<typename _ForwardIterator, typename _Size, typename _Tp,
	   typename _Allocator>
    void
    __uninitialized_fill_n_a(_ForwardIterator __first, _Size __n, 
			     const _Tp& __x, _Allocator& __alloc)
    {
      _ForwardIterator __cur = __first;
      try
	{
	  typedef __gnu_cxx::__alloc_traits<_Allocator> __traits;
	  for (; __n > 0; --__n, ++__cur)
	    __traits::construct(__alloc, std::__addressof(*__cur), __x);
	}
      catch(...)
	{
	  std::_Destroy(__first, __cur, __alloc);
	  throw;
	}
    }

  template<typename _ForwardIterator, typename _Size, typename _Tp,
	   typename _Tp2>
    inline void
    __uninitialized_fill_n_a(_ForwardIterator __first, _Size __n, 
			     const _Tp& __x, allocator<_Tp2>&)
    { std::uninitialized_fill_n(__first, __n, __x); }


  
  
  
  

  
  
  
  
  template<typename _InputIterator1, typename _InputIterator2,
	   typename _ForwardIterator, typename _Allocator>
    inline _ForwardIterator
    __uninitialized_copy_move(_InputIterator1 __first1,
			      _InputIterator1 __last1,
			      _InputIterator2 __first2,
			      _InputIterator2 __last2,
			      _ForwardIterator __result,
			      _Allocator& __alloc)
    {
      _ForwardIterator __mid = std::__uninitialized_copy_a(__first1, __last1,
							   __result,
							   __alloc);
      try
	{
	  return std::__uninitialized_move_a(__first2, __last2, __mid, __alloc);
	}
      catch(...)
	{
	  std::_Destroy(__result, __mid, __alloc);
	  throw;
	}
    }

  
  
  
  
  template<typename _InputIterator1, typename _InputIterator2,
	   typename _ForwardIterator, typename _Allocator>
    inline _ForwardIterator
    __uninitialized_move_copy(_InputIterator1 __first1,
			      _InputIterator1 __last1,
			      _InputIterator2 __first2,
			      _InputIterator2 __last2,
			      _ForwardIterator __result,
			      _Allocator& __alloc)
    {
      _ForwardIterator __mid = std::__uninitialized_move_a(__first1, __last1,
							   __result,
							   __alloc);
      try
	{
	  return std::__uninitialized_copy_a(__first2, __last2, __mid, __alloc);
	}
      catch(...)
	{
	  std::_Destroy(__result, __mid, __alloc);
	  throw;
	}
    }
  
  
  
  
  template<typename _ForwardIterator, typename _Tp, typename _InputIterator,
	   typename _Allocator>
    inline _ForwardIterator
    __uninitialized_fill_move(_ForwardIterator __result, _ForwardIterator __mid,
			      const _Tp& __x, _InputIterator __first,
			      _InputIterator __last, _Allocator& __alloc)
    {
      std::__uninitialized_fill_a(__result, __mid, __x, __alloc);
      try
	{
	  return std::__uninitialized_move_a(__first, __last, __mid, __alloc);
	}
      catch(...)
	{
	  std::_Destroy(__result, __mid, __alloc);
	  throw;
	}
    }

  
  
  
  template<typename _InputIterator, typename _ForwardIterator, typename _Tp,
	   typename _Allocator>
    inline void
    __uninitialized_move_fill(_InputIterator __first1, _InputIterator __last1,
			      _ForwardIterator __first2,
			      _ForwardIterator __last2, const _Tp& __x,
			      _Allocator& __alloc)
    {
      _ForwardIterator __mid2 = std::__uninitialized_move_a(__first1, __last1,
							    __first2,
							    __alloc);
      try
	{
	  std::__uninitialized_fill_a(__mid2, __last2, __x, __alloc);
	}
      catch(...)
	{
	  std::_Destroy(__first2, __mid2, __alloc);
	  throw;
	}
    }

  
  

  template<bool _TrivialValueType>
    struct __uninitialized_default_1
    {
      template<typename _ForwardIterator>
        static void
        __uninit_default(_ForwardIterator __first, _ForwardIterator __last)
        {
	  _ForwardIterator __cur = __first;
	  try
	    {
	      for (; __cur != __last; ++__cur)
		std::_Construct(std::__addressof(*__cur));
	    }
	  catch(...)
	    {
	      std::_Destroy(__first, __cur);
	      throw;
	    }
	}
    };

  template<>
    struct __uninitialized_default_1<true>
    {
      template<typename _ForwardIterator>
        static void
        __uninit_default(_ForwardIterator __first, _ForwardIterator __last)
        {
	  typedef typename iterator_traits<_ForwardIterator>::value_type
	    _ValueType;

	  std::fill(__first, __last, _ValueType());
	}
    };

  template<bool _TrivialValueType>
    struct __uninitialized_default_n_1
    {
      template<typename _ForwardIterator, typename _Size>
        static void
        __uninit_default_n(_ForwardIterator __first, _Size __n)
        {
	  _ForwardIterator __cur = __first;
	  try
	    {
	      for (; __n > 0; --__n, ++__cur)
		std::_Construct(std::__addressof(*__cur));
	    }
	  catch(...)
	    {
	      std::_Destroy(__first, __cur);
	      throw;
	    }
	}
    };

  template<>
    struct __uninitialized_default_n_1<true>
    {
      template<typename _ForwardIterator, typename _Size>
        static void
        __uninit_default_n(_ForwardIterator __first, _Size __n)
        {
	  typedef typename iterator_traits<_ForwardIterator>::value_type
	    _ValueType;

	  std::fill_n(__first, __n, _ValueType());
	}
    };

  
  
  
  template<typename _ForwardIterator>
    inline void
    __uninitialized_default(_ForwardIterator __first,
			    _ForwardIterator __last)
    {
      typedef typename iterator_traits<_ForwardIterator>::value_type
	_ValueType;

      std::__uninitialized_default_1<__is_trivial(_ValueType)>::
	__uninit_default(__first, __last);
    }

  
  
  template<typename _ForwardIterator, typename _Size>
    inline void
    __uninitialized_default_n(_ForwardIterator __first, _Size __n)
    {
      typedef typename iterator_traits<_ForwardIterator>::value_type
	_ValueType;

      std::__uninitialized_default_n_1<__is_trivial(_ValueType)>::
	__uninit_default_n(__first, __n);
    }


  
  
  
  template<typename _ForwardIterator, typename _Allocator>
    void
    __uninitialized_default_a(_ForwardIterator __first,
			      _ForwardIterator __last,
			      _Allocator& __alloc)
    {
      _ForwardIterator __cur = __first;
      try
	{
	  typedef __gnu_cxx::__alloc_traits<_Allocator> __traits;
	  for (; __cur != __last; ++__cur)
	    __traits::construct(__alloc, std::__addressof(*__cur));
	}
      catch(...)
	{
	  std::_Destroy(__first, __cur, __alloc);
	  throw;
	}
    }

  template<typename _ForwardIterator, typename _Tp>
    inline void
    __uninitialized_default_a(_ForwardIterator __first,
			      _ForwardIterator __last,
			      allocator<_Tp>&)
    { std::__uninitialized_default(__first, __last); }


  
  
  
  template<typename _ForwardIterator, typename _Size, typename _Allocator>
    void
    __uninitialized_default_n_a(_ForwardIterator __first, _Size __n, 
				_Allocator& __alloc)
    {
      _ForwardIterator __cur = __first;
      try
	{
	  typedef __gnu_cxx::__alloc_traits<_Allocator> __traits;
	  for (; __n > 0; --__n, ++__cur)
	    __traits::construct(__alloc, std::__addressof(*__cur));
	}
      catch(...)
	{
	  std::_Destroy(__first, __cur, __alloc);
	  throw;
	}
    }

  template<typename _ForwardIterator, typename _Size, typename _Tp>
    inline void
    __uninitialized_default_n_a(_ForwardIterator __first, _Size __n, 
				allocator<_Tp>&)
    { std::__uninitialized_default_n(__first, __n); }


  template<typename _InputIterator, typename _Size,
	   typename _ForwardIterator>
    _ForwardIterator
    __uninitialized_copy_n(_InputIterator __first, _Size __n,
			   _ForwardIterator __result, input_iterator_tag)
    {
      _ForwardIterator __cur = __result;
      try
	{
	  for (; __n > 0; --__n, ++__first, ++__cur)
	    std::_Construct(std::__addressof(*__cur), *__first);
	  return __cur;
	}
      catch(...)
	{
	  std::_Destroy(__result, __cur);
	  throw;
	}
    }

  template<typename _RandomAccessIterator, typename _Size,
	   typename _ForwardIterator>
    inline _ForwardIterator
    __uninitialized_copy_n(_RandomAccessIterator __first, _Size __n,
			   _ForwardIterator __result,
			   random_access_iterator_tag)
    { return std::uninitialized_copy(__first, __first + __n, __result); }

  







 
  template<typename _InputIterator, typename _Size, typename _ForwardIterator>
    inline _ForwardIterator
    uninitialized_copy_n(_InputIterator __first, _Size __n,
			 _ForwardIterator __result)
    { return std::__uninitialized_copy_n(__first, __n, __result,
					 std::__iterator_category(__first)); }


} 

















































 




 



namespace std 
{


  















 
  template<typename _Tp>
    pair<_Tp*, ptrdiff_t>
    get_temporary_buffer(ptrdiff_t __len) noexcept
    {
      const ptrdiff_t __max =
	__gnu_cxx::__numeric_traits<ptrdiff_t>::__max / sizeof(_Tp);
      if (__len > __max)
	__len = __max;
      
      while (__len > 0) 
	{
	  _Tp* __tmp = static_cast<_Tp*>(::operator new(__len * sizeof(_Tp), 
							std::nothrow));
	  if (__tmp != 0)
	    return std::pair<_Tp*, ptrdiff_t>(__tmp, __len);
	  __len /= 2;
	}
      return std::pair<_Tp*, ptrdiff_t>(static_cast<_Tp*>(0), 0);
    }

  





 
  template<typename _Tp>
    inline void
    return_temporary_buffer(_Tp* __p)
    { ::operator delete(__p, std::nothrow); }


  



 
  template<typename _ForwardIterator, typename _Tp>
    class _Temporary_buffer
    {
      
      

    public:
      typedef _Tp         value_type;
      typedef value_type* pointer;
      typedef pointer     iterator;
      typedef ptrdiff_t   size_type;

    protected:
      size_type  _M_original_len;
      size_type  _M_len;
      pointer    _M_buffer;

    public:
      
      size_type
      size() const
      { return _M_len; }

      
      size_type
      requested_size() const
      { return _M_original_len; }

      
      iterator
      begin()
      { return _M_buffer; }

      
      iterator
      end()
      { return _M_buffer + _M_len; }

      


 
      _Temporary_buffer(_ForwardIterator __first, _ForwardIterator __last);

      ~_Temporary_buffer()
      {
	std::_Destroy(_M_buffer, _M_buffer + _M_len);
	std::return_temporary_buffer(_M_buffer);
      }

    private:
      
      _Temporary_buffer(const _Temporary_buffer&);

      void
      operator=(const _Temporary_buffer&);
    };


  template<bool>
    struct __uninitialized_construct_buf_dispatch
    {
      template<typename _Pointer, typename _ForwardIterator>
        static void
        __ucr(_Pointer __first, _Pointer __last,
	      _ForwardIterator __seed)
        {
	  if(__first == __last)
	    return;

	  _Pointer __cur = __first;
	  try
	    {
	      std::_Construct(std::__addressof(*__first),
			      std::move(*__seed));
	      _Pointer __prev = __cur;
	      ++__cur;
	      for(; __cur != __last; ++__cur, ++__prev)
		std::_Construct(std::__addressof(*__cur),
				std::move(*__prev));
	      *__seed = std::move(*__prev);
	    }
	  catch(...)
	    {
	      std::_Destroy(__first, __cur);
	      throw;
	    }
	}
    };

  template<>
    struct __uninitialized_construct_buf_dispatch<true>
    {
      template<typename _Pointer, typename _ForwardIterator>
        static void
        __ucr(_Pointer, _Pointer, _ForwardIterator) { }
    };

  
  
  
  
  
  
  
  
  
  
  template<typename _Pointer, typename _ForwardIterator>
    inline void
    __uninitialized_construct_buf(_Pointer __first, _Pointer __last,
				  _ForwardIterator __seed)
    {
      typedef typename std::iterator_traits<_Pointer>::value_type
	_ValueType;

      std::__uninitialized_construct_buf_dispatch<
        __has_trivial_constructor(_ValueType)>::
	  __ucr(__first, __last, __seed);
    }

  template<typename _ForwardIterator, typename _Tp>
    _Temporary_buffer<_ForwardIterator, _Tp>::
    _Temporary_buffer(_ForwardIterator __first, _ForwardIterator __last)
    : _M_original_len(std::distance(__first, __last)),
      _M_len(0), _M_buffer(0)
    {
      try
	{
	  std::pair<pointer, size_type> __p(std::get_temporary_buffer<
					    value_type>(_M_original_len));
	  _M_buffer = __p.first;
	  _M_len = __p.second;
	  if (_M_buffer)
	    std::__uninitialized_construct_buf(_M_buffer, _M_buffer + _M_len,
					       __first);
	}
      catch(...)
	{
	  std::return_temporary_buffer(_M_buffer);
	  _M_buffer = 0;
	  _M_len = 0;
	  throw;
	}
    }


} 


















































 




 


namespace std 
{


  


 
  template <class _OutputIterator, class _Tp>
    class raw_storage_iterator
    : public iterator<output_iterator_tag, void, void, void, void>
    {
    protected:
      _OutputIterator _M_iter;

    public:
      explicit
      raw_storage_iterator(_OutputIterator __x)
      : _M_iter(__x) {}

      raw_storage_iterator&
      operator*() { return *this; }

      raw_storage_iterator&
      operator=(const _Tp& __element)
      {
	std::_Construct(std::__addressof(*_M_iter), __element);
	return *this;
      }

      raw_storage_iterator<_OutputIterator, _Tp>&
      operator++()
      {
	++_M_iter;
	return *this;
      }

      raw_storage_iterator<_OutputIterator, _Tp>
      operator++(int)
      {
	raw_storage_iterator<_OutputIterator, _Tp> __tmp = *this;
	++_M_iter;
	return __tmp;
      }
    };


} 




























 


#pragma GCC system_header




























 


#pragma GCC system_header


namespace std
{


  
  
  
  
  
  size_t
  _Hash_bytes(const void* __ptr, size_t __len, size_t __seed);

  
  
  
  
  size_t
  _Fnv_hash_bytes(const void* __ptr, size_t __len, size_t __seed);


} 


#pragma GCC visibility push(default)

extern "C++" {

namespace __cxxabiv1
{
  class __class_type_info;
} 























namespace std
{
  




 
  class type_info
  {
  public:
    


 
    virtual ~type_info();

    
 
    const char* name() const noexcept
    { return __name[0] == '*' ? __name + 1 : __name; }

    
    
    
    bool before(const type_info& __arg) const noexcept;
    bool operator==(const type_info& __arg) const noexcept;
    bool operator!=(const type_info& __arg) const noexcept
    { return !operator==(__arg); }

    size_t hash_code() const noexcept
    {
      return _Hash_bytes(name(), __builtin_strlen(name()),
			 static_cast<size_t>(0xc70f6907UL));
    }

    
    virtual bool __is_pointer_p() const;

    
    virtual bool __is_function_p() const;

    
    
    
    
    
    
    virtual bool __do_catch(const type_info *__thr_type, void **__thr_obj,
			    unsigned __outer) const;

    
    virtual bool __do_upcast(const __cxxabiv1::__class_type_info *__target,
			     void **__obj_ptr) const;

  protected:
    const char *__name;

    explicit type_info(const char *__n): __name(__n) { }

  private:
    
    type_info& operator=(const type_info&);
    type_info(const type_info&);
  };

  




 
  class bad_cast : public exception
  {
  public:
    bad_cast() noexcept { }

    
    
    virtual ~bad_cast() noexcept;

    
    virtual const char* what() const noexcept;
  };

  


 
  class bad_typeid : public exception
  {
  public:
    bad_typeid () noexcept { }

    
    
    virtual ~bad_typeid() noexcept;

    
    virtual const char* what() const noexcept;
  };
} 

} 

#pragma GCC visibility pop



























 






#pragma GCC system_header




























 






#pragma GCC system_header


namespace std 
{


  



 

  template<class _CharT>
    struct char_traits;

  template<typename _CharT, typename _Traits = char_traits<_CharT>,
           typename _Alloc = allocator<_CharT> >
    class basic_string;

  template<> struct char_traits<char>;

  
  typedef basic_string<char>    string;   

  template<> struct char_traits<wchar_t>;

  
  typedef basic_string<wchar_t> wstring;   


  template<> struct char_traits<char16_t>;
  template<> struct char_traits<char32_t>;

  
  typedef basic_string<char16_t> u16string; 

  
  typedef basic_string<char32_t> u32string; 

   


} 




























 







#pragma GCC system_header

































 





#pragma GCC system_header

















 




 



 
















 



 





 
struct _IO_FILE;


 
typedef struct _IO_FILE FILE;






 
typedef struct _IO_FILE __FILE;




 





















 



 


 

typedef __builtin_va_list __gnuc_va_list;


 


















 


 


 



 





















 



 



 


 


 
 




 


 






 





 

 


 

 


 
typedef unsigned long size_t;





 


 


typedef unsigned int wint_t;


 
 
 


 





 

 

 

 
typedef struct
{
  int __count;
  union
  {
    unsigned int __wch;
    char __wchb[4];
  } __value;		 
} __mbstate_t;



 


 
typedef __mbstate_t mbstate_t;




 



 


extern "C" {



 
struct tm;



 




 
extern wchar_t *wcscpy (wchar_t * __dest,
			const wchar_t * __src) throw ();
 
extern wchar_t *wcsncpy (wchar_t * __dest,
			 const wchar_t * __src, size_t __n)
     throw ();

 
extern wchar_t *wcscat (wchar_t * __dest,
			const wchar_t * __src) throw ();
 
extern wchar_t *wcsncat (wchar_t * __dest,
			 const wchar_t * __src, size_t __n)
     throw ();

 
extern int wcscmp (const wchar_t *__s1, const wchar_t *__s2)
     throw () __attribute__ ((__pure__));
 
extern int wcsncmp (const wchar_t *__s1, const wchar_t *__s2, size_t __n)
     throw () __attribute__ ((__pure__));


 
extern int wcscasecmp (const wchar_t *__s1, const wchar_t *__s2) throw ();

 
extern int wcsncasecmp (const wchar_t *__s1, const wchar_t *__s2,
			size_t __n) throw ();


 

















 





 
typedef struct __locale_struct
{
   
  struct __locale_data *__locales[13];  

   
  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;

   
  const char *__names[13];
} *__locale_t;

 
typedef __locale_t locale_t;


extern int wcscasecmp_l (const wchar_t *__s1, const wchar_t *__s2,
			 __locale_t __loc) throw ();

extern int wcsncasecmp_l (const wchar_t *__s1, const wchar_t *__s2,
			  size_t __n, __locale_t __loc) throw ();



 
extern int wcscoll (const wchar_t *__s1, const wchar_t *__s2) throw ();


 
extern size_t wcsxfrm (wchar_t * __s1,
		       const wchar_t * __s2, size_t __n) throw ();



 


 
extern int wcscoll_l (const wchar_t *__s1, const wchar_t *__s2,
		      __locale_t __loc) throw ();



 
extern size_t wcsxfrm_l (wchar_t *__s1, const wchar_t *__s2,
			 size_t __n, __locale_t __loc) throw ();

 
extern wchar_t *wcsdup (const wchar_t *__s) throw () __attribute__ ((__malloc__));


 
extern "C++" wchar_t *wcschr (wchar_t *__wcs, wchar_t __wc)
     throw () __asm ("wcschr") __attribute__ ((__pure__));
extern "C++" const wchar_t *wcschr (const wchar_t *__wcs, wchar_t __wc)
     throw () __asm ("wcschr") __attribute__ ((__pure__));
 
extern "C++" wchar_t *wcsrchr (wchar_t *__wcs, wchar_t __wc)
     throw () __asm ("wcsrchr") __attribute__ ((__pure__));
extern "C++" const wchar_t *wcsrchr (const wchar_t *__wcs, wchar_t __wc)
     throw () __asm ("wcsrchr") __attribute__ ((__pure__));



 
extern wchar_t *wcschrnul (const wchar_t *__s, wchar_t __wc)
     throw () __attribute__ ((__pure__));



 
extern size_t wcscspn (const wchar_t *__wcs, const wchar_t *__reject)
     throw () __attribute__ ((__pure__));

 
extern size_t wcsspn (const wchar_t *__wcs, const wchar_t *__accept)
     throw () __attribute__ ((__pure__));
 
extern "C++" wchar_t *wcspbrk (wchar_t *__wcs, const wchar_t *__accept)
     throw () __asm ("wcspbrk") __attribute__ ((__pure__));
extern "C++" const wchar_t *wcspbrk (const wchar_t *__wcs,
				     const wchar_t *__accept)
     throw () __asm ("wcspbrk") __attribute__ ((__pure__));
 
extern "C++" wchar_t *wcsstr (wchar_t *__haystack, const wchar_t *__needle)
     throw () __asm ("wcsstr") __attribute__ ((__pure__));
extern "C++" const wchar_t *wcsstr (const wchar_t *__haystack,
				    const wchar_t *__needle)
     throw () __asm ("wcsstr") __attribute__ ((__pure__));

 
extern wchar_t *wcstok (wchar_t * __s,
			const wchar_t * __delim,
			wchar_t ** __ptr) throw ();

 
extern size_t wcslen (const wchar_t *__s) throw () __attribute__ ((__pure__));


 
extern "C++" wchar_t *wcswcs (wchar_t *__haystack, const wchar_t *__needle)
     throw () __asm ("wcswcs") __attribute__ ((__pure__));
extern "C++" const wchar_t *wcswcs (const wchar_t *__haystack,
				    const wchar_t *__needle)
     throw () __asm ("wcswcs") __attribute__ ((__pure__));

 
extern size_t wcsnlen (const wchar_t *__s, size_t __maxlen)
     throw () __attribute__ ((__pure__));



 
extern "C++" wchar_t *wmemchr (wchar_t *__s, wchar_t __c, size_t __n)
     throw () __asm ("wmemchr") __attribute__ ((__pure__));
extern "C++" const wchar_t *wmemchr (const wchar_t *__s, wchar_t __c,
				     size_t __n)
     throw () __asm ("wmemchr") __attribute__ ((__pure__));

 
extern int wmemcmp (const wchar_t *__s1, const wchar_t *__s2, size_t __n)
     throw () __attribute__ ((__pure__));

 
extern wchar_t *wmemcpy (wchar_t * __s1,
			 const wchar_t * __s2, size_t __n) throw ();


 
extern wchar_t *wmemmove (wchar_t *__s1, const wchar_t *__s2, size_t __n)
     throw ();

 
extern wchar_t *wmemset (wchar_t *__s, wchar_t __c, size_t __n) throw ();



 
extern wchar_t *wmempcpy (wchar_t * __s1,
			  const wchar_t * __s2, size_t __n)
     throw ();




 
extern wint_t btowc (int __c) throw ();


 
extern int wctob (wint_t __c) throw ();


 
extern int mbsinit (const mbstate_t *__ps) throw () __attribute__ ((__pure__));


 
extern size_t mbrtowc (wchar_t * __pwc,
		       const char * __s, size_t __n,
		       mbstate_t * __p) throw ();

 
extern size_t wcrtomb (char * __s, wchar_t __wc,
		       mbstate_t * __ps) throw ();

 
extern size_t __mbrlen (const char * __s, size_t __n,
			mbstate_t * __ps) throw ();
extern size_t mbrlen (const char * __s, size_t __n,
		      mbstate_t * __ps) throw ();


 



 
extern wint_t __btowc_alias (int __c) __asm ("btowc");
extern inline __attribute__ ((__gnu_inline__)) wint_t
__attribute__ ((__leaf__)) btowc (int __c) throw ()
{ return (__builtin_constant_p (__c) && __c >= '\0' && __c <= '\x7f'
	  ? (wint_t) __c : __btowc_alias (__c)); }

extern int __wctob_alias (wint_t __c) __asm ("wctob");
extern inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__leaf__)) wctob (wint_t __wc) throw ()
{ return (__builtin_constant_p (__wc) && __wc >= L'\0' && __wc <= L'\x7f'
	  ? (int) __wc : __wctob_alias (__wc)); }

extern inline __attribute__ ((__gnu_inline__)) size_t
__attribute__ ((__leaf__)) mbrlen (const char *__s, size_t __n, mbstate_t *__ps) throw ()
{ return (__ps != __null
	  ? mbrtowc (__null, __s, __n, __ps) : __mbrlen (__s, __n, __null)); }



 
extern size_t mbsrtowcs (wchar_t * __dst,
			 const char ** __src, size_t __len,
			 mbstate_t * __ps) throw ();


 
extern size_t wcsrtombs (char * __dst,
			 const wchar_t ** __src, size_t __len,
			 mbstate_t * __ps) throw ();




 
extern size_t mbsnrtowcs (wchar_t * __dst,
			  const char ** __src, size_t __nmc,
			  size_t __len, mbstate_t * __ps) throw ();


 
extern size_t wcsnrtombs (char * __dst,
			  const wchar_t ** __src,
			  size_t __nwc, size_t __len,
			  mbstate_t * __ps) throw ();


 
 
extern int wcwidth (wchar_t __c) throw ();


 
extern int wcswidth (const wchar_t *__s, size_t __n) throw ();




 
extern double wcstod (const wchar_t * __nptr,
		      wchar_t ** __endptr) throw ();



 
extern float wcstof (const wchar_t * __nptr,
		     wchar_t ** __endptr) throw ();
extern long double wcstold (const wchar_t * __nptr,
			    wchar_t ** __endptr) throw ();





 
extern long int wcstol (const wchar_t * __nptr,
			wchar_t ** __endptr, int __base) throw ();


 
extern unsigned long int wcstoul (const wchar_t * __nptr,
				  wchar_t ** __endptr, int __base)
     throw ();




 
__extension__
extern long long int wcstoll (const wchar_t * __nptr,
			      wchar_t ** __endptr, int __base)
     throw ();


 
__extension__
extern unsigned long long int wcstoull (const wchar_t * __nptr,
					wchar_t ** __endptr,
					int __base) throw ();



 
__extension__
extern long long int wcstoq (const wchar_t * __nptr,
			     wchar_t ** __endptr, int __base)
     throw ();


 
__extension__
extern unsigned long long int wcstouq (const wchar_t * __nptr,
				       wchar_t ** __endptr,
				       int __base) throw ();










 


 


 
extern long int wcstol_l (const wchar_t * __nptr,
			  wchar_t ** __endptr, int __base,
			  __locale_t __loc) throw ();

extern unsigned long int wcstoul_l (const wchar_t * __nptr,
				    wchar_t ** __endptr,
				    int __base, __locale_t __loc) throw ();

__extension__
extern long long int wcstoll_l (const wchar_t * __nptr,
				wchar_t ** __endptr,
				int __base, __locale_t __loc) throw ();

__extension__
extern unsigned long long int wcstoull_l (const wchar_t * __nptr,
					  wchar_t ** __endptr,
					  int __base, __locale_t __loc)
     throw ();

extern double wcstod_l (const wchar_t * __nptr,
			wchar_t ** __endptr, __locale_t __loc)
     throw ();

extern float wcstof_l (const wchar_t * __nptr,
		       wchar_t ** __endptr, __locale_t __loc)
     throw ();

extern long double wcstold_l (const wchar_t * __nptr,
			      wchar_t ** __endptr,
			      __locale_t __loc) throw ();



 
extern wchar_t *wcpcpy (wchar_t * __dest,
			const wchar_t * __src) throw ();


 
extern wchar_t *wcpncpy (wchar_t * __dest,
			 const wchar_t * __src, size_t __n)
     throw ();


 


 
extern __FILE *open_wmemstream (wchar_t **__bufloc, size_t *__sizeloc) throw ();



 
extern int fwide (__FILE *__fp, int __mode) throw ();





 
extern int fwprintf (__FILE * __stream,
		     const wchar_t * __format, ...)
      ;



 
extern int wprintf (const wchar_t * __format, ...)
      ;
 
extern int swprintf (wchar_t * __s, size_t __n,
		     const wchar_t * __format, ...)
     throw ()  ;




 
extern int vfwprintf (__FILE * __s,
		      const wchar_t * __format,
		      __gnuc_va_list __arg)
      ;



 
extern int vwprintf (const wchar_t * __format,
		     __gnuc_va_list __arg)
      ;

 
extern int vswprintf (wchar_t * __s, size_t __n,
		      const wchar_t * __format,
		      __gnuc_va_list __arg)
     throw ()  ;





 
extern int fwscanf (__FILE * __stream,
		    const wchar_t * __format, ...)
      ;



 
extern int wscanf (const wchar_t * __format, ...)
      ;
 
extern int swscanf (const wchar_t * __s,
		    const wchar_t * __format, ...)
     throw ()  ;








 
extern int vfwscanf (__FILE * __s,
		     const wchar_t * __format,
		     __gnuc_va_list __arg)
      ;



 
extern int vwscanf (const wchar_t * __format,
		    __gnuc_va_list __arg)
      ;
 
extern int vswscanf (const wchar_t * __s,
		     const wchar_t * __format,
		     __gnuc_va_list __arg)
     throw ()  ;









 
extern wint_t fgetwc (__FILE *__stream);
extern wint_t getwc (__FILE *__stream);




 
extern wint_t getwchar (void);





 
extern wint_t fputwc (wchar_t __wc, __FILE *__stream);
extern wint_t putwc (wchar_t __wc, __FILE *__stream);




 
extern wint_t putwchar (wchar_t __wc);






 
extern wchar_t *fgetws (wchar_t * __ws, int __n,
			__FILE * __stream);




 
extern int fputws (const wchar_t * __ws,
		   __FILE * __stream);





 
extern wint_t ungetwc (wint_t __wc, __FILE *__stream);









 
extern wint_t getwc_unlocked (__FILE *__stream);
extern wint_t getwchar_unlocked (void);






 
extern wint_t fgetwc_unlocked (__FILE *__stream);






 
extern wint_t fputwc_unlocked (wchar_t __wc, __FILE *__stream);







 
extern wint_t putwc_unlocked (wchar_t __wc, __FILE *__stream);
extern wint_t putwchar_unlocked (wchar_t __wc);







 
extern wchar_t *fgetws_unlocked (wchar_t * __ws, int __n,
				 __FILE * __stream);






 
extern int fputws_unlocked (const wchar_t * __ws,
			    __FILE * __stream);





 
extern size_t wcsftime (wchar_t * __s, size_t __maxsize,
			const wchar_t * __format,
			const struct tm * __tp) throw ();




 
extern size_t wcsftime_l (wchar_t * __s, size_t __maxsize,
			  const wchar_t * __format,
			  const struct tm * __tp,
			  __locale_t __loc) throw ();






 

 


}




 





namespace std
{
  using ::mbstate_t;
} 




namespace std 
{


  using ::wint_t;

  using ::btowc;
  using ::fgetwc;
  using ::fgetws;
  using ::fputwc;
  using ::fputws;
  using ::fwide;
  using ::fwprintf;
  using ::fwscanf;
  using ::getwc;
  using ::getwchar;
  using ::mbrlen;
  using ::mbrtowc;
  using ::mbsinit;
  using ::mbsrtowcs;
  using ::putwc;
  using ::putwchar;
  using ::swprintf;
  using ::swscanf;
  using ::ungetwc;
  using ::vfwprintf;
  using ::vfwscanf;
  using ::vswprintf;
  using ::vswscanf;
  using ::vwprintf;
  using ::vwscanf;
  using ::wcrtomb;
  using ::wcscat;
  using ::wcscmp;
  using ::wcscoll;
  using ::wcscpy;
  using ::wcscspn;
  using ::wcsftime;
  using ::wcslen;
  using ::wcsncat;
  using ::wcsncmp;
  using ::wcsncpy;
  using ::wcsrtombs;
  using ::wcsspn;
  using ::wcstod;
  using ::wcstof;
  using ::wcstok;
  using ::wcstol;
  using ::wcstoul;
  using ::wcsxfrm;
  using ::wctob;
  using ::wmemcmp;
  using ::wmemcpy;
  using ::wmemmove;
  using ::wmemset;
  using ::wprintf;
  using ::wscanf;
  using ::wcschr;
  using ::wcspbrk;
  using ::wcsrchr;
  using ::wcsstr;
  using ::wmemchr;



} 



namespace __gnu_cxx
{
  using ::wcstold;
  using ::wcstoll;
  using ::wcstoull;
} 

namespace std
{
  using ::__gnu_cxx::wcstold;
  using ::__gnu_cxx::wcstoll;
  using ::__gnu_cxx::wcstoull;
} 





namespace std
{
  using std::wcstof;
  using std::vfwscanf;
  using std::vswscanf;
  using std::vwscanf;

  using std::wcstold;
  using std::wcstoll;
  using std::wcstoull;
} 








namespace std 
{


  
  
  
  
  
  

  






   
  typedef long          streamoff;

  
  typedef ptrdiff_t	streamsize; 

  









 
  template<typename _StateT>
    class fpos
    {
    private:
      streamoff	                _M_off;
      _StateT			_M_state;

    public:
      
      
      
      
      fpos()
      : _M_off(0), _M_state() { }

      
      
      
      
      
      
      
      fpos(streamoff __off)
      : _M_off(__off), _M_state() { }

      
      operator streamoff() const { return _M_off; }

      
      void
      state(_StateT __st)
      { _M_state = __st; }

      
      _StateT
      state() const
      { return _M_state; }

      
      
      
      
      fpos&
      operator+=(streamoff __off)
      {
	_M_off += __off;
	return *this;
      }

      
      
      
      
      fpos&
      operator-=(streamoff __off)
      {
	_M_off -= __off;
	return *this;
      }

      
      
      
      
      
      
      fpos
      operator+(streamoff __off) const
      {
	fpos __pos(*this);
	__pos += __off;
	return __pos;
      }

      
      
      
      
      
      
      fpos
      operator-(streamoff __off) const
      {
	fpos __pos(*this);
	__pos -= __off;
	return __pos;
      }

      
      
      
      
      
      streamoff
      operator-(const fpos& __other) const
      { return _M_off - __other._M_off; }
    };

  
  
  
  
  
  template<typename _StateT>
    inline bool
    operator==(const fpos<_StateT>& __lhs, const fpos<_StateT>& __rhs)
    { return streamoff(__lhs) == streamoff(__rhs); }

  template<typename _StateT>
    inline bool
    operator!=(const fpos<_StateT>& __lhs, const fpos<_StateT>& __rhs)
    { return streamoff(__lhs) != streamoff(__rhs); }

  
  
  
  
  typedef fpos<mbstate_t> streampos;
  
  typedef fpos<mbstate_t> wstreampos;

  
  typedef fpos<mbstate_t> u16streampos;
  
  typedef fpos<mbstate_t> u32streampos;


} 


namespace std 
{


  


























 
  class ios_base;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_ios;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_streambuf;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_istream;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_ostream;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_iostream;

  template<typename _CharT, typename _Traits = char_traits<_CharT>,
	    typename _Alloc = allocator<_CharT> >
    class basic_stringbuf;

  template<typename _CharT, typename _Traits = char_traits<_CharT>,
	   typename _Alloc = allocator<_CharT> >
    class basic_istringstream;

  template<typename _CharT, typename _Traits = char_traits<_CharT>,
	   typename _Alloc = allocator<_CharT> >
    class basic_ostringstream;

  template<typename _CharT, typename _Traits = char_traits<_CharT>,
	   typename _Alloc = allocator<_CharT> >
    class basic_stringstream;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_filebuf;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_ifstream;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_ofstream;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class basic_fstream;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class istreambuf_iterator;

  template<typename _CharT, typename _Traits = char_traits<_CharT> >
    class ostreambuf_iterator;


  
  typedef basic_ios<char> 		ios; 

  
  typedef basic_streambuf<char> 	streambuf;

  
  typedef basic_istream<char> 		istream;

  
  typedef basic_ostream<char> 		ostream;

  
  typedef basic_iostream<char> 		iostream;

  
  typedef basic_stringbuf<char> 	stringbuf;

  
  typedef basic_istringstream<char> 	istringstream;

  
  typedef basic_ostringstream<char> 	ostringstream;

  
  typedef basic_stringstream<char> 	stringstream;

  
  typedef basic_filebuf<char> 		filebuf;

  
  typedef basic_ifstream<char> 		ifstream;

  
  typedef basic_ofstream<char> 		ofstream;

  
  typedef basic_fstream<char> 		fstream;

  
  typedef basic_ios<wchar_t> 		wios;

  
  typedef basic_streambuf<wchar_t> 	wstreambuf;

  
  typedef basic_istream<wchar_t> 	wistream;

  
  typedef basic_ostream<wchar_t> 	wostream;

  
  typedef basic_iostream<wchar_t> 	wiostream;

  
  typedef basic_stringbuf<wchar_t> 	wstringbuf;

  
  typedef basic_istringstream<wchar_t> 	wistringstream;

  
  typedef basic_ostringstream<wchar_t> 	wostringstream;

  
  typedef basic_stringstream<wchar_t> 	wstringstream;

  
  typedef basic_filebuf<wchar_t> 	wfilebuf;

  
  typedef basic_ifstream<wchar_t> 	wifstream;

  
  typedef basic_ofstream<wchar_t> 	wofstream;

  
  typedef basic_fstream<wchar_t> 	wfstream;
   


} 



























 


#pragma GCC system_header

 
 





















 


#pragma GCC visibility push(default)





































































































 

 
 





















 



 

















 

















 









 


 
 




 




 
















 



















 



 


 


 

 
typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;

 
typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;
typedef signed long int __int64_t;
typedef unsigned long int __uint64_t;

 
typedef long int __quad_t;
typedef unsigned long int __u_quad_t;





























 

 
















 




 

 




 

 

 




typedef unsigned long int __dev_t;	 
typedef unsigned int __uid_t;	 
typedef unsigned int __gid_t;	 
typedef unsigned long int __ino_t;	 
typedef unsigned long int __ino64_t;	 
typedef unsigned int __mode_t;	 
typedef unsigned long int __nlink_t;	 
typedef long int __off_t;	 
typedef long int __off64_t;	 
typedef int __pid_t;	 
typedef struct { int __val[2]; } __fsid_t;	 
typedef long int __clock_t;	 
typedef unsigned long int __rlim_t;	 
typedef unsigned long int __rlim64_t;	 
typedef unsigned int __id_t;		 
typedef long int __time_t;	 
typedef unsigned int __useconds_t;  
typedef long int __suseconds_t;  

typedef int __daddr_t;	 
typedef int __key_t;	 

 
typedef int __clockid_t;

 
typedef void * __timer_t;

 
typedef long int __blksize_t;

 

 
typedef long int __blkcnt_t;
typedef long int __blkcnt64_t;

 
typedef unsigned long int __fsblkcnt_t;
typedef unsigned long int __fsblkcnt64_t;

 
typedef unsigned long int __fsfilcnt_t;
typedef unsigned long int __fsfilcnt64_t;

 
typedef long int __fsword_t;

typedef long int __ssize_t;  

 
typedef long int __syscall_slong_t;
 
typedef unsigned long int __syscall_ulong_t;


 
typedef __off64_t __loff_t;	 
typedef __quad_t *__qaddr_t;
typedef char *__caddr_t;

 
typedef long int __intptr_t;

 
typedef unsigned int __socklen_t;



 


 

 

 
















 



 

static inline unsigned int
__bswap_32 (unsigned int __bsx)
{
  return __builtin_bswap32 (__bsx);
}


 

static inline __uint64_t
__bswap_64 (__uint64_t __bsx)
{
  return __builtin_bswap64 (__bsx);
}























 



 






















 



 



 


 


 
 




 


 






 





 

 


 

 


 





 


 



 
 
 


 



















 



 








 
typedef __time_t time_t;










 
struct timespec
  {
    __time_t tv_sec;		 
    __syscall_slong_t tv_nsec;	 
  };





typedef __pid_t pid_t;


 


















 




 


 

 
struct sched_param
  {
    int __sched_priority;
  };

extern "C" {

 
extern int clone (int (*__fn) (void *__arg), void *__child_stack,
		  int __flags, void *__arg, ...) throw ();

 
extern int unshare (int __flags) throw ();

 
extern int sched_getcpu (void) throw ();

 
extern int setns (int __fd, int __nstype) throw ();


}


 
struct __sched_param
  {
    int __sched_priority;
  };


 

 
typedef unsigned long int __cpu_mask;

 

 
typedef struct
{
  __cpu_mask __bits[1024 / (8 * sizeof (__cpu_mask))];
} cpu_set_t;

 





extern "C" {

extern int __sched_cpucount (size_t __setsize, const cpu_set_t *__setp)
  throw ();
extern cpu_set_t *__sched_cpualloc (size_t __count) throw () ;
extern void __sched_cpufree (cpu_set_t *__set) throw ();

}

 


extern "C" {

 
extern int sched_setparam (__pid_t __pid, const struct sched_param *__param)
     throw ();

 
extern int sched_getparam (__pid_t __pid, struct sched_param *__param) throw ();

 
extern int sched_setscheduler (__pid_t __pid, int __policy,
			       const struct sched_param *__param) throw ();

 
extern int sched_getscheduler (__pid_t __pid) throw ();

 
extern int sched_yield (void) throw ();

 
extern int sched_get_priority_max (int __algorithm) throw ();

 
extern int sched_get_priority_min (int __algorithm) throw ();

 
extern int sched_rr_get_interval (__pid_t __pid, struct timespec *__t) throw ();


 






 
extern int sched_setaffinity (__pid_t __pid, size_t __cpusetsize,
			      const cpu_set_t *__cpuset) throw ();

 
extern int sched_getaffinity (__pid_t __pid, size_t __cpusetsize,
			      cpu_set_t *__cpuset) throw ();

}
















 



 



extern "C" {


 





















 



 



 


 


 
 




 


 






 





 

 


 

 


 





 


 



 
 
 


 





 
















 



 



 
struct timeval
  {
    __time_t tv_sec;		 
    __suseconds_t tv_usec;	 
  };




 


 


 
 
 
 
 
 
 
 
 
 

 
















 



 

struct timex
{
  unsigned int modes;		 
  __syscall_slong_t offset;	 
  __syscall_slong_t freq;	 
  __syscall_slong_t maxerror;	 
  __syscall_slong_t esterror;	 
  int status;			 
  __syscall_slong_t constant;	 
  __syscall_slong_t precision;	 
  __syscall_slong_t tolerance;	 
  struct timeval time;		 
  __syscall_slong_t tick;	 
  __syscall_slong_t ppsfreq;	 
  __syscall_slong_t jitter;	 
  int shift;			 
  __syscall_slong_t stabil;	 
  __syscall_slong_t jitcnt;	 
  __syscall_slong_t calcnt;	 
  __syscall_slong_t errcnt;	 
  __syscall_slong_t stbcnt;	 

  int tai;			 

   
  int  :32; int  :32; int  :32; int  :32;
  int  :32; int  :32; int  :32; int  :32;
  int  :32; int  :32; int  :32;
};

 

 


 




 


extern "C" {

 
extern int clock_adjtime (__clockid_t __clock_id, struct timex *__utx) throw ();

}



 





 
typedef __clock_t clock_t;







 
typedef __clockid_t clockid_t;




 
typedef __timer_t timer_t;






 
struct tm
{
  int tm_sec;			 
  int tm_min;			 
  int tm_hour;			 
  int tm_mday;			 
  int tm_mon;			 
  int tm_year;			 
  int tm_wday;			 
  int tm_yday;			 
  int tm_isdst;			 

  long int tm_gmtoff;		 
  const char *tm_zone;		 
};




 
struct itimerspec
  {
    struct timespec it_interval;
    struct timespec it_value;
  };

 
struct sigevent;




 




 
extern clock_t clock (void) throw ();

 
extern time_t time (time_t *__timer) throw ();

 
extern double difftime (time_t __time1, time_t __time0)
     throw () __attribute__ ((__const__));

 
extern time_t mktime (struct tm *__tp) throw ();




 
extern size_t strftime (char * __s, size_t __maxsize,
			const char * __format,
			const struct tm * __tp) throw ();



 
extern char *strptime (const char * __s,
		       const char * __fmt, struct tm *__tp)
     throw ();


 

extern size_t strftime_l (char * __s, size_t __maxsize,
			  const char * __format,
			  const struct tm * __tp,
			  __locale_t __loc) throw ();

extern char *strptime_l (const char * __s,
			 const char * __fmt, struct tm *__tp,
			 __locale_t __loc) throw ();




 
extern struct tm *gmtime (const time_t *__timer) throw ();


 
extern struct tm *localtime (const time_t *__timer) throw ();



 
extern struct tm *gmtime_r (const time_t * __timer,
			    struct tm * __tp) throw ();


 
extern struct tm *localtime_r (const time_t * __timer,
			       struct tm * __tp) throw ();



 
extern char *asctime (const struct tm *__tp) throw ();

 
extern char *ctime (const time_t *__timer) throw ();


 


 
extern char *asctime_r (const struct tm * __tp,
			char * __buf) throw ();

 
extern char *ctime_r (const time_t * __timer,
		      char * __buf) throw ();


 
extern char *__tzname[2];	 
extern int __daylight;		 
extern long int __timezone;	 


 
extern char *tzname[2];


 
extern void tzset (void) throw ();

extern int daylight;
extern long int timezone;


 
extern int stime (const time_t *__when) throw ();



 



 

 
extern time_t timegm (struct tm *__tp) throw ();

 
extern time_t timelocal (struct tm *__tp) throw ();

 
extern int dysize (int __year) throw ()  __attribute__ ((__const__));





 
extern int nanosleep (const struct timespec *__requested_time,
		      struct timespec *__remaining);


 
extern int clock_getres (clockid_t __clock_id, struct timespec *__res) throw ();

 
extern int clock_gettime (clockid_t __clock_id, struct timespec *__tp) throw ();

 
extern int clock_settime (clockid_t __clock_id, const struct timespec *__tp)
     throw ();




 
extern int clock_nanosleep (clockid_t __clock_id, int __flags,
			    const struct timespec *__req,
			    struct timespec *__rem);

 
extern int clock_getcpuclockid (pid_t __pid, clockid_t *__clock_id) throw ();


 
extern int timer_create (clockid_t __clock_id,
			 struct sigevent * __evp,
			 timer_t * __timerid) throw ();

 
extern int timer_delete (timer_t __timerid) throw ();

 
extern int timer_settime (timer_t __timerid, int __flags,
			  const struct itimerspec * __value,
			  struct itimerspec * __ovalue) throw ();

 
extern int timer_gettime (timer_t __timerid, struct itimerspec *__value)
     throw ();

 
extern int timer_getoverrun (timer_t __timerid) throw ();


 
extern int timespec_get (struct timespec *__ts, int __base)
     throw () __attribute__ ((__nonnull__ (1)));












 
extern int getdate_err;







 
extern struct tm *getdate (const char *__string);










 
extern int getdate_r (const char * __string,
		      struct tm * __resbufp);

}


















 


 


 




 
typedef unsigned long int pthread_t;


union pthread_attr_t
{
  char __size[56];
  long int __align;
};
typedef union pthread_attr_t pthread_attr_t;


typedef struct __pthread_internal_list
{
  struct __pthread_internal_list *__prev;
  struct __pthread_internal_list *__next;
} __pthread_list_t;



 
typedef union
{
  struct __pthread_mutex_s
  {
    int __lock;
    unsigned int __count;
    int __owner;
    unsigned int __nusers;
    
 
    int __kind;
    int __spins;
    __pthread_list_t __list;
  } __data;
  char __size[40];
  long int __align;
} pthread_mutex_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_mutexattr_t;



 
typedef union
{
  struct
  {
    int __lock;
    unsigned int __futex;
    __extension__ unsigned long long int __total_seq;
    __extension__ unsigned long long int __wakeup_seq;
    __extension__ unsigned long long int __woken_seq;
    void *__mutex;
    unsigned int __nwaiters;
    unsigned int __broadcast_seq;
  } __data;
  char __size[48];
  __extension__ long long int __align;
} pthread_cond_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_condattr_t;


 
typedef unsigned int pthread_key_t;


 
typedef int pthread_once_t;



 
typedef union
{
  struct
  {
    int __lock;
    unsigned int __nr_readers;
    unsigned int __readers_wakeup;
    unsigned int __writer_wakeup;
    unsigned int __nr_readers_queued;
    unsigned int __nr_writers_queued;
    int __writer;
    int __shared;
    unsigned long int __pad1;
    unsigned long int __pad2;
    
 
    unsigned int __flags;
  } __data;
  char __size[56];
  long int __align;
} pthread_rwlock_t;

typedef union
{
  char __size[8];
  long int __align;
} pthread_rwlockattr_t;


 
typedef volatile int pthread_spinlock_t;



 
typedef union
{
  char __size[32];
  long int __align;
} pthread_barrier_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_barrierattr_t;


















 

 


 


 


typedef long int __jmp_buf[8];


 


 


 
enum
{
  PTHREAD_CREATE_JOINABLE,
  PTHREAD_CREATE_DETACHED
};


 
enum
{
  PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_ADAPTIVE_NP
  ,
  PTHREAD_MUTEX_NORMAL = PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE = PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
   
  , PTHREAD_MUTEX_FAST_NP = PTHREAD_MUTEX_TIMED_NP
};


 
enum
{
  PTHREAD_MUTEX_STALLED,
  PTHREAD_MUTEX_STALLED_NP = PTHREAD_MUTEX_STALLED,
  PTHREAD_MUTEX_ROBUST,
  PTHREAD_MUTEX_ROBUST_NP = PTHREAD_MUTEX_ROBUST
};


 
enum
{
  PTHREAD_PRIO_NONE,
  PTHREAD_PRIO_INHERIT,
  PTHREAD_PRIO_PROTECT
};


 


 
enum
{
  PTHREAD_RWLOCK_PREFER_READER_NP,
  PTHREAD_RWLOCK_PREFER_WRITER_NP,
  PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP,
  PTHREAD_RWLOCK_DEFAULT_NP = PTHREAD_RWLOCK_PREFER_READER_NP
};



 

 


 
enum
{
  PTHREAD_INHERIT_SCHED,
  PTHREAD_EXPLICIT_SCHED
};


 
enum
{
  PTHREAD_SCOPE_SYSTEM,
  PTHREAD_SCOPE_PROCESS
};


 
enum
{
  PTHREAD_PROCESS_PRIVATE,
  PTHREAD_PROCESS_SHARED
};



 


 
struct _pthread_cleanup_buffer
{
  void (*__routine) (void *);              
  void *__arg;                             
  int __canceltype;                        
  struct _pthread_cleanup_buffer *__prev;  
};

 
enum
{
  PTHREAD_CANCEL_ENABLE,
  PTHREAD_CANCEL_DISABLE
};
enum
{
  PTHREAD_CANCEL_DEFERRED,
  PTHREAD_CANCEL_ASYNCHRONOUS
};


 




 


extern "C" {



 
extern int pthread_create (pthread_t * __newthread,
			   const pthread_attr_t * __attr,
			   void *(*__start_routine) (void *),
			   void * __arg) throw () __attribute__ ((__nonnull__ (1, 3)));




 
extern void pthread_exit (void *__retval) __attribute__ ((__noreturn__));






 
extern int pthread_join (pthread_t __th, void **__thread_return);


 
extern int pthread_tryjoin_np (pthread_t __th, void **__thread_return) throw ();






 
extern int pthread_timedjoin_np (pthread_t __th, void **__thread_return,
				 const struct timespec *__abstime);




 
extern int pthread_detach (pthread_t __th) throw ();


 
extern pthread_t pthread_self (void) throw () __attribute__ ((__const__));

 
extern int pthread_equal (pthread_t __thread1, pthread_t __thread2)
  throw () __attribute__ ((__const__));


 



 
extern int pthread_attr_init (pthread_attr_t *__attr) throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_attr_destroy (pthread_attr_t *__attr)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_attr_getdetachstate (const pthread_attr_t *__attr,
					int *__detachstate)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_attr_setdetachstate (pthread_attr_t *__attr,
					int __detachstate)
     throw () __attribute__ ((__nonnull__ (1)));


 
extern int pthread_attr_getguardsize (const pthread_attr_t *__attr,
				      size_t *__guardsize)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_attr_setguardsize (pthread_attr_t *__attr,
				      size_t __guardsize)
     throw () __attribute__ ((__nonnull__ (1)));


 
extern int pthread_attr_getschedparam (const pthread_attr_t * __attr,
				       struct sched_param * __param)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_attr_setschedparam (pthread_attr_t * __attr,
				       const struct sched_param *
				       __param) throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_attr_getschedpolicy (const pthread_attr_t *
					__attr, int * __policy)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_attr_setschedpolicy (pthread_attr_t *__attr, int __policy)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_attr_getinheritsched (const pthread_attr_t *
					 __attr, int * __inherit)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_attr_setinheritsched (pthread_attr_t *__attr,
					 int __inherit)
     throw () __attribute__ ((__nonnull__ (1)));


 
extern int pthread_attr_getscope (const pthread_attr_t * __attr,
				  int * __scope)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_attr_setscope (pthread_attr_t *__attr, int __scope)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_attr_getstackaddr (const pthread_attr_t *
				      __attr, void ** __stackaddr)
     throw () __attribute__ ((__nonnull__ (1, 2))) __attribute__ ((__deprecated__));




 
extern int pthread_attr_setstackaddr (pthread_attr_t *__attr,
				      void *__stackaddr)
     throw () __attribute__ ((__nonnull__ (1))) __attribute__ ((__deprecated__));

 
extern int pthread_attr_getstacksize (const pthread_attr_t *
				      __attr, size_t * __stacksize)
     throw () __attribute__ ((__nonnull__ (1, 2)));



 
extern int pthread_attr_setstacksize (pthread_attr_t *__attr,
				      size_t __stacksize)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_attr_getstack (const pthread_attr_t * __attr,
				  void ** __stackaddr,
				  size_t * __stacksize)
     throw () __attribute__ ((__nonnull__ (1, 2, 3)));



 
extern int pthread_attr_setstack (pthread_attr_t *__attr, void *__stackaddr,
				  size_t __stacksize) throw () __attribute__ ((__nonnull__ (1)));


 
extern int pthread_attr_setaffinity_np (pthread_attr_t *__attr,
					size_t __cpusetsize,
					const cpu_set_t *__cpuset)
     throw () __attribute__ ((__nonnull__ (1, 3)));


 
extern int pthread_attr_getaffinity_np (const pthread_attr_t *__attr,
					size_t __cpusetsize,
					cpu_set_t *__cpuset)
     throw () __attribute__ ((__nonnull__ (1, 3)));




 
extern int pthread_getattr_np (pthread_t __th, pthread_attr_t *__attr)
     throw () __attribute__ ((__nonnull__ (2)));


 


 
extern int pthread_setschedparam (pthread_t __target_thread, int __policy,
				  const struct sched_param *__param)
     throw () __attribute__ ((__nonnull__ (3)));

 
extern int pthread_getschedparam (pthread_t __target_thread,
				  int * __policy,
				  struct sched_param * __param)
     throw () __attribute__ ((__nonnull__ (2, 3)));

 
extern int pthread_setschedprio (pthread_t __target_thread, int __prio)
     throw ();


 
extern int pthread_getname_np (pthread_t __target_thread, char *__buf,
			       size_t __buflen)
     throw () __attribute__ ((__nonnull__ (2)));

 
extern int pthread_setname_np (pthread_t __target_thread, const char *__name)
     throw () __attribute__ ((__nonnull__ (2)));


 
extern int pthread_getconcurrency (void) throw ();

 
extern int pthread_setconcurrency (int __level) throw ();




 
extern int pthread_yield (void) throw ();



 
extern int pthread_setaffinity_np (pthread_t __th, size_t __cpusetsize,
				   const cpu_set_t *__cpuset)
     throw () __attribute__ ((__nonnull__ (3)));

 
extern int pthread_getaffinity_np (pthread_t __th, size_t __cpusetsize,
				   cpu_set_t *__cpuset)
     throw () __attribute__ ((__nonnull__ (3)));


 







 
extern int pthread_once (pthread_once_t *__once_control,
			 void (*__init_routine) (void)) __attribute__ ((__nonnull__ (1, 2)));






 


 
extern int pthread_setcancelstate (int __state, int *__oldstate);


 
extern int pthread_setcanceltype (int __type, int *__oldtype);

 
extern int pthread_cancel (pthread_t __th);



 
extern void pthread_testcancel (void);


 

typedef struct
{
  struct
  {
    __jmp_buf __cancel_jmp_buf;
    int __mask_was_saved;
  } __cancel_jmp_buf[1];
  void *__pad[4];
} __pthread_unwind_buf_t __attribute__ ((__aligned__));

 


 
struct __pthread_cleanup_frame
{
  void (*__cancel_routine) (void *);
  void *__cancel_arg;
  int __do_it;
  int __cancel_type;
};

 
class __pthread_cleanup_class
{
  void (*__cancel_routine) (void *);
  void *__cancel_arg;
  int __do_it;
  int __cancel_type;

 public:
  __pthread_cleanup_class (void (*__fct) (void *), void *__arg)
    : __cancel_routine (__fct), __cancel_arg (__arg), __do_it (1) { }
  ~__pthread_cleanup_class () { if (__do_it) __cancel_routine (__cancel_arg); }
  void __setdoit (int __newval) { __do_it = __newval; }
  void __defer () { pthread_setcanceltype (PTHREAD_CANCEL_DEFERRED,
					   &__cancel_type); }
  void __restore () const { pthread_setcanceltype (__cancel_type, 0); }
};







 


 



 



 

 
struct __jmp_buf_tag;
extern int __sigsetjmp (struct __jmp_buf_tag *__env, int __savemask) throw ();


 

 
extern int pthread_mutex_init (pthread_mutex_t *__mutex,
			       const pthread_mutexattr_t *__mutexattr)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_mutex_destroy (pthread_mutex_t *__mutex)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_mutex_trylock (pthread_mutex_t *__mutex)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_mutex_lock (pthread_mutex_t *__mutex)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_mutex_timedlock (pthread_mutex_t * __mutex,
				    const struct timespec *
				    __abstime) throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_mutex_unlock (pthread_mutex_t *__mutex)
     throw () __attribute__ ((__nonnull__ (1)));


 
extern int pthread_mutex_getprioceiling (const pthread_mutex_t *
					  __mutex,
					 int * __prioceiling)
     throw () __attribute__ ((__nonnull__ (1, 2)));


 
extern int pthread_mutex_setprioceiling (pthread_mutex_t * __mutex,
					 int __prioceiling,
					 int * __old_ceiling)
     throw () __attribute__ ((__nonnull__ (1, 3)));


 
extern int pthread_mutex_consistent (pthread_mutex_t *__mutex)
     throw () __attribute__ ((__nonnull__ (1)));
extern int pthread_mutex_consistent_np (pthread_mutex_t *__mutex)
     throw () __attribute__ ((__nonnull__ (1)));


 


 
extern int pthread_mutexattr_init (pthread_mutexattr_t *__attr)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_mutexattr_destroy (pthread_mutexattr_t *__attr)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_mutexattr_getpshared (const pthread_mutexattr_t *
					  __attr,
					 int * __pshared)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_mutexattr_setpshared (pthread_mutexattr_t *__attr,
					 int __pshared)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_mutexattr_gettype (const pthread_mutexattr_t *
				      __attr, int * __kind)
     throw () __attribute__ ((__nonnull__ (1, 2)));



 
extern int pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_mutexattr_getprotocol (const pthread_mutexattr_t *
					   __attr,
					  int * __protocol)
     throw () __attribute__ ((__nonnull__ (1, 2)));


 
extern int pthread_mutexattr_setprotocol (pthread_mutexattr_t *__attr,
					  int __protocol)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_mutexattr_getprioceiling (const pthread_mutexattr_t *
					      __attr,
					     int * __prioceiling)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_mutexattr_setprioceiling (pthread_mutexattr_t *__attr,
					     int __prioceiling)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_mutexattr_getrobust (const pthread_mutexattr_t *__attr,
					int *__robustness)
     throw () __attribute__ ((__nonnull__ (1, 2)));
extern int pthread_mutexattr_getrobust_np (const pthread_mutexattr_t *__attr,
					   int *__robustness)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_mutexattr_setrobust (pthread_mutexattr_t *__attr,
					int __robustness)
     throw () __attribute__ ((__nonnull__ (1)));
extern int pthread_mutexattr_setrobust_np (pthread_mutexattr_t *__attr,
					   int __robustness)
     throw () __attribute__ ((__nonnull__ (1)));


 


 
extern int pthread_rwlock_init (pthread_rwlock_t * __rwlock,
				const pthread_rwlockattr_t *
				__attr) throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_rwlock_destroy (pthread_rwlock_t *__rwlock)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_rwlock_rdlock (pthread_rwlock_t *__rwlock)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_rwlock_tryrdlock (pthread_rwlock_t *__rwlock)
  throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_rwlock_timedrdlock (pthread_rwlock_t * __rwlock,
				       const struct timespec *
				       __abstime) throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_rwlock_wrlock (pthread_rwlock_t *__rwlock)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_rwlock_trywrlock (pthread_rwlock_t *__rwlock)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_rwlock_timedwrlock (pthread_rwlock_t * __rwlock,
				       const struct timespec *
				       __abstime) throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_rwlock_unlock (pthread_rwlock_t *__rwlock)
     throw () __attribute__ ((__nonnull__ (1)));


 

 
extern int pthread_rwlockattr_init (pthread_rwlockattr_t *__attr)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_rwlockattr_destroy (pthread_rwlockattr_t *__attr)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_rwlockattr_getpshared (const pthread_rwlockattr_t *
					   __attr,
					  int * __pshared)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_rwlockattr_setpshared (pthread_rwlockattr_t *__attr,
					  int __pshared)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_rwlockattr_getkind_np (const pthread_rwlockattr_t *
					   __attr,
					  int * __pref)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_rwlockattr_setkind_np (pthread_rwlockattr_t *__attr,
					  int __pref) throw () __attribute__ ((__nonnull__ (1)));


 


 
extern int pthread_cond_init (pthread_cond_t * __cond,
			      const pthread_condattr_t * __cond_attr)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_cond_destroy (pthread_cond_t *__cond)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_cond_signal (pthread_cond_t *__cond)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_cond_broadcast (pthread_cond_t *__cond)
     throw () __attribute__ ((__nonnull__ (1)));





 
extern int pthread_cond_wait (pthread_cond_t * __cond,
			      pthread_mutex_t * __mutex)
     __attribute__ ((__nonnull__ (1, 2)));







 
extern int pthread_cond_timedwait (pthread_cond_t * __cond,
				   pthread_mutex_t * __mutex,
				   const struct timespec * __abstime)
     __attribute__ ((__nonnull__ (1, 2, 3)));

 

 
extern int pthread_condattr_init (pthread_condattr_t *__attr)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_condattr_destroy (pthread_condattr_t *__attr)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_condattr_getpshared (const pthread_condattr_t *
					 __attr,
					int * __pshared)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_condattr_setpshared (pthread_condattr_t *__attr,
					int __pshared) throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_condattr_getclock (const pthread_condattr_t *
				       __attr,
				      __clockid_t * __clock_id)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_condattr_setclock (pthread_condattr_t *__attr,
				      __clockid_t __clock_id)
     throw () __attribute__ ((__nonnull__ (1)));


 


 
extern int pthread_spin_init (pthread_spinlock_t *__lock, int __pshared)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_spin_destroy (pthread_spinlock_t *__lock)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_spin_lock (pthread_spinlock_t *__lock)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_spin_trylock (pthread_spinlock_t *__lock)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_spin_unlock (pthread_spinlock_t *__lock)
     throw () __attribute__ ((__nonnull__ (1)));


 


 
extern int pthread_barrier_init (pthread_barrier_t * __barrier,
				 const pthread_barrierattr_t *
				 __attr, unsigned int __count)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_barrier_destroy (pthread_barrier_t *__barrier)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_barrier_wait (pthread_barrier_t *__barrier)
     throw () __attribute__ ((__nonnull__ (1)));


 
extern int pthread_barrierattr_init (pthread_barrierattr_t *__attr)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_barrierattr_destroy (pthread_barrierattr_t *__attr)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_barrierattr_getpshared (const pthread_barrierattr_t *
					    __attr,
					   int * __pshared)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int pthread_barrierattr_setpshared (pthread_barrierattr_t *__attr,
					   int __pshared)
     throw () __attribute__ ((__nonnull__ (1)));


 






 
extern int pthread_key_create (pthread_key_t *__key,
			       void (*__destr_function) (void *))
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int pthread_key_delete (pthread_key_t __key) throw ();

 
extern void *pthread_getspecific (pthread_key_t __key) throw ();

 
extern int pthread_setspecific (pthread_key_t __key,
				const void *__pointer) throw () ;


 
extern int pthread_getcpuclockid (pthread_t __thread_id,
				  __clockid_t *__clock_id)
     throw () __attribute__ ((__nonnull__ (2)));











 

extern int pthread_atfork (void (*__prepare) (void),
			   void (*__parent) (void),
			   void (*__child) (void)) throw ();


 
extern inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__leaf__)) pthread_equal (pthread_t __thread1, pthread_t __thread2) throw ()
{
  return __thread1 == __thread2;
}

}



typedef pthread_t __gthread_t;
typedef pthread_key_t __gthread_key_t;
typedef pthread_once_t __gthread_once_t;
typedef pthread_mutex_t __gthread_mutex_t;
typedef pthread_mutex_t __gthread_recursive_mutex_t;
typedef pthread_cond_t __gthread_cond_t;
typedef struct timespec __gthread_time_t;


 




 



















































 


static inline int
__gthread_active_p (void)
{
  return 1;
}




static inline int
__gthread_create (__gthread_t *__threadid, void *(*__func) (void*),
		  void *__args)
{
  return pthread_create (__threadid, __null, __func, __args);
}

static inline int
__gthread_join (__gthread_t __threadid, void **__value_ptr)
{
  return pthread_join (__threadid, __value_ptr);
}

static inline int
__gthread_detach (__gthread_t __threadid)
{
  return pthread_detach (__threadid);
}

static inline int
__gthread_equal (__gthread_t __t1, __gthread_t __t2)
{
  return pthread_equal (__t1, __t2);
}

static inline __gthread_t
__gthread_self (void)
{
  return pthread_self ();
}

static inline int
__gthread_yield (void)
{
  return sched_yield ();
}

static inline int
__gthread_once (__gthread_once_t *__once, void (*__func) (void))
{
  if (__gthread_active_p ())
    return pthread_once (__once, __func);
  else
    return -1;
}

static inline int
__gthread_key_create (__gthread_key_t *__key, void (*__dtor) (void *))
{
  return pthread_key_create (__key, __dtor);
}

static inline int
__gthread_key_delete (__gthread_key_t __key)
{
  return pthread_key_delete (__key);
}

static inline void *
__gthread_getspecific (__gthread_key_t __key)
{
  return pthread_getspecific (__key);
}

static inline int
__gthread_setspecific (__gthread_key_t __key, const void *__ptr)
{
  return pthread_setspecific (__key, __ptr);
}

static inline void
__gthread_mutex_init_function (__gthread_mutex_t *__mutex)
{
  if (__gthread_active_p ())
    pthread_mutex_init (__mutex, __null);
}

static inline int
__gthread_mutex_destroy (__gthread_mutex_t *__mutex)
{
  if (__gthread_active_p ())
    return pthread_mutex_destroy (__mutex);
  else
    return 0;
}

static inline int
__gthread_mutex_lock (__gthread_mutex_t *__mutex)
{
  if (__gthread_active_p ())
    return pthread_mutex_lock (__mutex);
  else
    return 0;
}

static inline int
__gthread_mutex_trylock (__gthread_mutex_t *__mutex)
{
  if (__gthread_active_p ())
    return pthread_mutex_trylock (__mutex);
  else
    return 0;
}

static inline int
__gthread_mutex_timedlock (__gthread_mutex_t *__mutex,
			   const __gthread_time_t *__abs_timeout)
{
  if (__gthread_active_p ())
    return pthread_mutex_timedlock (__mutex, __abs_timeout);
  else
    return 0;
}

static inline int
__gthread_mutex_unlock (__gthread_mutex_t *__mutex)
{
  if (__gthread_active_p ())
    return pthread_mutex_unlock (__mutex);
  else
    return 0;
}


static inline int
__gthread_recursive_mutex_lock (__gthread_recursive_mutex_t *__mutex)
{
  return __gthread_mutex_lock (__mutex);
}

static inline int
__gthread_recursive_mutex_trylock (__gthread_recursive_mutex_t *__mutex)
{
  return __gthread_mutex_trylock (__mutex);
}

static inline int
__gthread_recursive_mutex_timedlock (__gthread_recursive_mutex_t *__mutex,
				     const __gthread_time_t *__abs_timeout)
{
  return __gthread_mutex_timedlock (__mutex, __abs_timeout);
}

static inline int
__gthread_recursive_mutex_unlock (__gthread_recursive_mutex_t *__mutex)
{
  return __gthread_mutex_unlock (__mutex);
}

static inline int
__gthread_recursive_mutex_destroy (__gthread_recursive_mutex_t *__mutex)
{
  return __gthread_mutex_destroy (__mutex);
}


static inline int
__gthread_cond_broadcast (__gthread_cond_t *__cond)
{
  return pthread_cond_broadcast (__cond);
}

static inline int
__gthread_cond_signal (__gthread_cond_t *__cond)
{
  return pthread_cond_signal (__cond);
}

static inline int
__gthread_cond_wait (__gthread_cond_t *__cond, __gthread_mutex_t *__mutex)
{
  return pthread_cond_wait (__cond, __mutex);
}

static inline int
__gthread_cond_timedwait (__gthread_cond_t *__cond, __gthread_mutex_t *__mutex,
			  const __gthread_time_t *__abs_timeout)
{
  return pthread_cond_timedwait (__cond, __mutex, __abs_timeout);
}

static inline int
__gthread_cond_wait_recursive (__gthread_cond_t *__cond,
			       __gthread_recursive_mutex_t *__mutex)
{
  return __gthread_cond_wait (__cond, __mutex);
}

static inline int
__gthread_cond_destroy (__gthread_cond_t* __cond)
{
  return pthread_cond_destroy (__cond);
}



#pragma GCC visibility pop



























 


typedef int _Atomic_word;















namespace __gnu_cxx 
{


  
  
  
  
  _Atomic_word
  __attribute__ ((__unused__))
  __exchange_and_add(volatile _Atomic_word*, int) throw ();

  void
  __attribute__ ((__unused__))
  __atomic_add(volatile _Atomic_word*, int) throw ();

  static inline _Atomic_word
  __exchange_and_add_single(_Atomic_word* __mem, int __val)
  {
    _Atomic_word __result = *__mem;
    *__mem += __val;
    return __result;
  }

  static inline void
  __atomic_add_single(_Atomic_word* __mem, int __val)
  { *__mem += __val; }

  static inline _Atomic_word
  __attribute__ ((__unused__))
  __exchange_and_add_dispatch(_Atomic_word* __mem, int __val)
  {
    if (__gthread_active_p())
      return __exchange_and_add(__mem, __val);
    else
      return __exchange_and_add_single(__mem, __val);
  }

  static inline void
  __attribute__ ((__unused__))
  __atomic_add_dispatch(_Atomic_word* __mem, int __val)
  {
    if (__gthread_active_p())
      __atomic_add(__mem, __val);
    else
      __atomic_add_single(__mem, __val);
  }


} 































 


#pragma GCC system_header


namespace __gnu_cxx 
{


  
  
  
  
  
  enum _Lock_policy { _S_single, _S_mutex, _S_atomic }; 

  
  
  static const _Lock_policy __default_lock_policy = 
  _S_atomic;

  
  
  class __concurrence_lock_error : public std::exception
  {
  public:
    virtual char const*
    what() const throw()
    { return "__gnu_cxx::__concurrence_lock_error"; }
  };

  class __concurrence_unlock_error : public std::exception
  {
  public:
    virtual char const*
    what() const throw()
    { return "__gnu_cxx::__concurrence_unlock_error"; }
  };

  class __concurrence_broadcast_error : public std::exception
  {
  public:
    virtual char const*
    what() const throw()
    { return "__gnu_cxx::__concurrence_broadcast_error"; }
  };

  class __concurrence_wait_error : public std::exception
  {
  public:
    virtual char const*
    what() const throw()
    { return "__gnu_cxx::__concurrence_wait_error"; }
  };

  
  inline void
  __throw_concurrence_lock_error()
  { (throw (__concurrence_lock_error())); }

  inline void
  __throw_concurrence_unlock_error()
  { (throw (__concurrence_unlock_error())); }

  inline void
  __throw_concurrence_broadcast_error()
  { (throw (__concurrence_broadcast_error())); }

  inline void
  __throw_concurrence_wait_error()
  { (throw (__concurrence_wait_error())); }
 
  class __mutex 
  {
  private:
    __gthread_mutex_t _M_mutex = { { 0, 0, 0, 0, 0, 0, { 0, 0 } } };

    __mutex(const __mutex&);
    __mutex& operator=(const __mutex&);

  public:
    __mutex() 
    { 
    }


    void lock()
    {
      if (__gthread_active_p())
	{
	  if (__gthread_mutex_lock(&_M_mutex) != 0)
	    __throw_concurrence_lock_error();
	}
    }
    
    void unlock()
    {
      if (__gthread_active_p())
	{
	  if (__gthread_mutex_unlock(&_M_mutex) != 0)
	    __throw_concurrence_unlock_error();
	}
    }

    __gthread_mutex_t* gthread_mutex(void)
      { return &_M_mutex; }
  };

  class __recursive_mutex 
  {
  private:
    __gthread_recursive_mutex_t _M_mutex = { { 0, 0, 0, 0, PTHREAD_MUTEX_RECURSIVE_NP, 0, { 0, 0 } } };

    __recursive_mutex(const __recursive_mutex&);
    __recursive_mutex& operator=(const __recursive_mutex&);

  public:
    __recursive_mutex() 
    { 
    }


    void lock()
    { 
      if (__gthread_active_p())
	{
	  if (__gthread_recursive_mutex_lock(&_M_mutex) != 0)
	    __throw_concurrence_lock_error();
	}
    }
    
    void unlock()
    { 
      if (__gthread_active_p())
	{
	  if (__gthread_recursive_mutex_unlock(&_M_mutex) != 0)
	    __throw_concurrence_unlock_error();
	}
    }

    __gthread_recursive_mutex_t* gthread_recursive_mutex(void)
    { return &_M_mutex; }
  };

  
  
  
  class __scoped_lock
  {
  public:
    typedef __mutex __mutex_type;

  private:
    __mutex_type& _M_device;

    __scoped_lock(const __scoped_lock&);
    __scoped_lock& operator=(const __scoped_lock&);

  public:
    explicit __scoped_lock(__mutex_type& __name) : _M_device(__name)
    { _M_device.lock(); }

    ~__scoped_lock() throw()
    { _M_device.unlock(); }
  };

  class __cond
  {
  private:
    __gthread_cond_t _M_cond = { { 0, 0, 0, 0, 0, (void *) 0, 0, 0 } };

    __cond(const __cond&);
    __cond& operator=(const __cond&);

  public:
    __cond() 
    { 
    }


    void broadcast()
    {
      if (__gthread_active_p())
	{
	  if (__gthread_cond_broadcast(&_M_cond) != 0)
	    __throw_concurrence_broadcast_error();
	}
    }

    void wait(__mutex *mutex)
    {
      {
	  if (__gthread_cond_wait(&_M_cond, mutex->gthread_mutex()) != 0)
	    __throw_concurrence_wait_error();
      }
    }

    void wait_recursive(__recursive_mutex *mutex)
    {
      {
	  if (__gthread_cond_wait_recursive(&_M_cond,
					    mutex->gthread_recursive_mutex())
	      != 0)
	    __throw_concurrence_wait_error();
      }
    }
  };


} 

















































 




 


namespace std 
{


  
  































 
  

 
  template<typename _Arg, typename _Result>
    struct unary_function
    {
      
      typedef _Arg 	argument_type;   

      
      typedef _Result 	result_type;  
    };

  

 
  template<typename _Arg1, typename _Arg2, typename _Result>
    struct binary_function
    {
      
      typedef _Arg1 	first_argument_type; 

      
      typedef _Arg2 	second_argument_type;

      
      typedef _Result 	result_type;
    };
   

  
  








 
  
  template<typename _Tp>
    struct plus : public binary_function<_Tp, _Tp, _Tp>
    {
      _Tp
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x + __y; }
    };

  
  template<typename _Tp>
    struct minus : public binary_function<_Tp, _Tp, _Tp>
    {
      _Tp
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x - __y; }
    };

  
  template<typename _Tp>
    struct multiplies : public binary_function<_Tp, _Tp, _Tp>
    {
      _Tp
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x * __y; }
    };

  
  template<typename _Tp>
    struct divides : public binary_function<_Tp, _Tp, _Tp>
    {
      _Tp
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x / __y; }
    };

  
  template<typename _Tp>
    struct modulus : public binary_function<_Tp, _Tp, _Tp>
    {
      _Tp
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x % __y; }
    };

  
  template<typename _Tp>
    struct negate : public unary_function<_Tp, _Tp>
    {
      _Tp
      operator()(const _Tp& __x) const
      { return -__x; }
    };
   

  
  






 
  
  template<typename _Tp>
    struct equal_to : public binary_function<_Tp, _Tp, bool>
    {
      bool
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x == __y; }
    };

  
  template<typename _Tp>
    struct not_equal_to : public binary_function<_Tp, _Tp, bool>
    {
      bool
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x != __y; }
    };

  
  template<typename _Tp>
    struct greater : public binary_function<_Tp, _Tp, bool>
    {
      bool
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x > __y; }
    };

  
  template<typename _Tp>
    struct less : public binary_function<_Tp, _Tp, bool>
    {
      bool
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x < __y; }
    };

  
  template<typename _Tp>
    struct greater_equal : public binary_function<_Tp, _Tp, bool>
    {
      bool
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x >= __y; }
    };

  
  template<typename _Tp>
    struct less_equal : public binary_function<_Tp, _Tp, bool>
    {
      bool
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x <= __y; }
    };
   

  
  






 
  
  template<typename _Tp>
    struct logical_and : public binary_function<_Tp, _Tp, bool>
    {
      bool
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x && __y; }
    };

  
  template<typename _Tp>
    struct logical_or : public binary_function<_Tp, _Tp, bool>
    {
      bool
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x || __y; }
    };

  
  template<typename _Tp>
    struct logical_not : public unary_function<_Tp, bool>
    {
      bool
      operator()(const _Tp& __x) const
      { return !__x; }
    };
   

  
  
  template<typename _Tp>
    struct bit_and : public binary_function<_Tp, _Tp, _Tp>
    {
      _Tp
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x & __y; }
    };

  template<typename _Tp>
    struct bit_or : public binary_function<_Tp, _Tp, _Tp>
    {
      _Tp
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x | __y; }
    };

  template<typename _Tp>
    struct bit_xor : public binary_function<_Tp, _Tp, _Tp>
    {
      _Tp
      operator()(const _Tp& __x, const _Tp& __y) const
      { return __x ^ __y; }
    };

  
  


























 
  
  template<typename _Predicate>
    class unary_negate
    : public unary_function<typename _Predicate::argument_type, bool>
    {
    protected:
      _Predicate _M_pred;

    public:
      explicit
      unary_negate(const _Predicate& __x) : _M_pred(__x) { }

      bool
      operator()(const typename _Predicate::argument_type& __x) const
      { return !_M_pred(__x); }
    };

  
  template<typename _Predicate>
    inline unary_negate<_Predicate>
    not1(const _Predicate& __pred)
    { return unary_negate<_Predicate>(__pred); }

  
  template<typename _Predicate>
    class binary_negate
    : public binary_function<typename _Predicate::first_argument_type,
			     typename _Predicate::second_argument_type, bool>
    {
    protected:
      _Predicate _M_pred;

    public:
      explicit
      binary_negate(const _Predicate& __x) : _M_pred(__x) { }

      bool
      operator()(const typename _Predicate::first_argument_type& __x,
		 const typename _Predicate::second_argument_type& __y) const
      { return !_M_pred(__x, __y); }
    };

  
  template<typename _Predicate>
    inline binary_negate<_Predicate>
    not2(const _Predicate& __pred)
    { return binary_negate<_Predicate>(__pred); }
   

  
  



















 
  
  template<typename _Arg, typename _Result>
    class pointer_to_unary_function : public unary_function<_Arg, _Result>
    {
    protected:
      _Result (*_M_ptr)(_Arg);

    public:
      pointer_to_unary_function() { }

      explicit
      pointer_to_unary_function(_Result (*__x)(_Arg))
      : _M_ptr(__x) { }

      _Result
      operator()(_Arg __x) const
      { return _M_ptr(__x); }
    };

  
  template<typename _Arg, typename _Result>
    inline pointer_to_unary_function<_Arg, _Result>
    ptr_fun(_Result (*__x)(_Arg))
    { return pointer_to_unary_function<_Arg, _Result>(__x); }

  
  template<typename _Arg1, typename _Arg2, typename _Result>
    class pointer_to_binary_function
    : public binary_function<_Arg1, _Arg2, _Result>
    {
    protected:
      _Result (*_M_ptr)(_Arg1, _Arg2);

    public:
      pointer_to_binary_function() { }

      explicit
      pointer_to_binary_function(_Result (*__x)(_Arg1, _Arg2))
      : _M_ptr(__x) { }

      _Result
      operator()(_Arg1 __x, _Arg2 __y) const
      { return _M_ptr(__x, __y); }
    };

  
  template<typename _Arg1, typename _Arg2, typename _Result>
    inline pointer_to_binary_function<_Arg1, _Arg2, _Result>
    ptr_fun(_Result (*__x)(_Arg1, _Arg2))
    { return pointer_to_binary_function<_Arg1, _Arg2, _Result>(__x); }
   

  template<typename _Tp>
    struct _Identity
    : public unary_function<_Tp,_Tp>
    {
      _Tp&
      operator()(_Tp& __x) const
      { return __x; }

      const _Tp&
      operator()(const _Tp& __x) const
      { return __x; }
    };

  template<typename _Pair>
    struct _Select1st
    : public unary_function<_Pair, typename _Pair::first_type>
    {
      typename _Pair::first_type&
      operator()(_Pair& __x) const
      { return __x.first; }

      const typename _Pair::first_type&
      operator()(const _Pair& __x) const
      { return __x.first; }

      template<typename _Pair2>
        typename _Pair2::first_type&
        operator()(_Pair2& __x) const
        { return __x.first; }

      template<typename _Pair2>
        const typename _Pair2::first_type&
        operator()(const _Pair2& __x) const
        { return __x.first; }
    };

  template<typename _Pair>
    struct _Select2nd
    : public unary_function<_Pair, typename _Pair::second_type>
    {
      typename _Pair::second_type&
      operator()(_Pair& __x) const
      { return __x.second; }

      const typename _Pair::second_type&
      operator()(const _Pair& __x) const
      { return __x.second; }
    };

  
  













 
  
  
  template<typename _Ret, typename _Tp>
    class mem_fun_t : public unary_function<_Tp*, _Ret>
    {
    public:
      explicit
      mem_fun_t(_Ret (_Tp::*__pf)())
      : _M_f(__pf) { }

      _Ret
      operator()(_Tp* __p) const
      { return (__p->*_M_f)(); }

    private:
      _Ret (_Tp::*_M_f)();
    };

  
  
  template<typename _Ret, typename _Tp>
    class const_mem_fun_t : public unary_function<const _Tp*, _Ret>
    {
    public:
      explicit
      const_mem_fun_t(_Ret (_Tp::*__pf)() const)
      : _M_f(__pf) { }

      _Ret
      operator()(const _Tp* __p) const
      { return (__p->*_M_f)(); }

    private:
      _Ret (_Tp::*_M_f)() const;
    };

  
  
  template<typename _Ret, typename _Tp>
    class mem_fun_ref_t : public unary_function<_Tp, _Ret>
    {
    public:
      explicit
      mem_fun_ref_t(_Ret (_Tp::*__pf)())
      : _M_f(__pf) { }

      _Ret
      operator()(_Tp& __r) const
      { return (__r.*_M_f)(); }

    private:
      _Ret (_Tp::*_M_f)();
  };

  
  
  template<typename _Ret, typename _Tp>
    class const_mem_fun_ref_t : public unary_function<_Tp, _Ret>
    {
    public:
      explicit
      const_mem_fun_ref_t(_Ret (_Tp::*__pf)() const)
      : _M_f(__pf) { }

      _Ret
      operator()(const _Tp& __r) const
      { return (__r.*_M_f)(); }

    private:
      _Ret (_Tp::*_M_f)() const;
    };

  
  
  template<typename _Ret, typename _Tp, typename _Arg>
    class mem_fun1_t : public binary_function<_Tp*, _Arg, _Ret>
    {
    public:
      explicit
      mem_fun1_t(_Ret (_Tp::*__pf)(_Arg))
      : _M_f(__pf) { }

      _Ret
      operator()(_Tp* __p, _Arg __x) const
      { return (__p->*_M_f)(__x); }

    private:
      _Ret (_Tp::*_M_f)(_Arg);
    };

  
  
  template<typename _Ret, typename _Tp, typename _Arg>
    class const_mem_fun1_t : public binary_function<const _Tp*, _Arg, _Ret>
    {
    public:
      explicit
      const_mem_fun1_t(_Ret (_Tp::*__pf)(_Arg) const)
      : _M_f(__pf) { }

      _Ret
      operator()(const _Tp* __p, _Arg __x) const
      { return (__p->*_M_f)(__x); }

    private:
      _Ret (_Tp::*_M_f)(_Arg) const;
    };

  
  
  template<typename _Ret, typename _Tp, typename _Arg>
    class mem_fun1_ref_t : public binary_function<_Tp, _Arg, _Ret>
    {
    public:
      explicit
      mem_fun1_ref_t(_Ret (_Tp::*__pf)(_Arg))
      : _M_f(__pf) { }

      _Ret
      operator()(_Tp& __r, _Arg __x) const
      { return (__r.*_M_f)(__x); }

    private:
      _Ret (_Tp::*_M_f)(_Arg);
    };

  
  
  template<typename _Ret, typename _Tp, typename _Arg>
    class const_mem_fun1_ref_t : public binary_function<_Tp, _Arg, _Ret>
    {
    public:
      explicit
      const_mem_fun1_ref_t(_Ret (_Tp::*__pf)(_Arg) const)
      : _M_f(__pf) { }

      _Ret
      operator()(const _Tp& __r, _Arg __x) const
      { return (__r.*_M_f)(__x); }

    private:
      _Ret (_Tp::*_M_f)(_Arg) const;
    };

  
  
  template<typename _Ret, typename _Tp>
    inline mem_fun_t<_Ret, _Tp>
    mem_fun(_Ret (_Tp::*__f)())
    { return mem_fun_t<_Ret, _Tp>(__f); }

  template<typename _Ret, typename _Tp>
    inline const_mem_fun_t<_Ret, _Tp>
    mem_fun(_Ret (_Tp::*__f)() const)
    { return const_mem_fun_t<_Ret, _Tp>(__f); }

  template<typename _Ret, typename _Tp>
    inline mem_fun_ref_t<_Ret, _Tp>
    mem_fun_ref(_Ret (_Tp::*__f)())
    { return mem_fun_ref_t<_Ret, _Tp>(__f); }

  template<typename _Ret, typename _Tp>
    inline const_mem_fun_ref_t<_Ret, _Tp>
    mem_fun_ref(_Ret (_Tp::*__f)() const)
    { return const_mem_fun_ref_t<_Ret, _Tp>(__f); }

  template<typename _Ret, typename _Tp, typename _Arg>
    inline mem_fun1_t<_Ret, _Tp, _Arg>
    mem_fun(_Ret (_Tp::*__f)(_Arg))
    { return mem_fun1_t<_Ret, _Tp, _Arg>(__f); }

  template<typename _Ret, typename _Tp, typename _Arg>
    inline const_mem_fun1_t<_Ret, _Tp, _Arg>
    mem_fun(_Ret (_Tp::*__f)(_Arg) const)
    { return const_mem_fun1_t<_Ret, _Tp, _Arg>(__f); }

  template<typename _Ret, typename _Tp, typename _Arg>
    inline mem_fun1_ref_t<_Ret, _Tp, _Arg>
    mem_fun_ref(_Ret (_Tp::*__f)(_Arg))
    { return mem_fun1_ref_t<_Ret, _Tp, _Arg>(__f); }

  template<typename _Ret, typename _Tp, typename _Arg>
    inline const_mem_fun1_ref_t<_Ret, _Tp, _Arg>
    mem_fun_ref(_Ret (_Tp::*__f)(_Arg) const)
    { return const_mem_fun1_ref_t<_Ret, _Tp, _Arg>(__f); }

   


} 

















































 




 


namespace std 
{


  
  




































 
  
  template<typename _Operation>
    class binder1st
    : public unary_function<typename _Operation::second_argument_type,
			    typename _Operation::result_type>
    {
    protected:
      _Operation op;
      typename _Operation::first_argument_type value;

    public:
      binder1st(const _Operation& __x,
		const typename _Operation::first_argument_type& __y)
      : op(__x), value(__y) { }

      typename _Operation::result_type
      operator()(const typename _Operation::second_argument_type& __x) const
      { return op(value, __x); }

      
      
      typename _Operation::result_type
      operator()(typename _Operation::second_argument_type& __x) const
      { return op(value, __x); }
    } __attribute__ ((__deprecated__));

  
  template<typename _Operation, typename _Tp>
    inline binder1st<_Operation>
    bind1st(const _Operation& __fn, const _Tp& __x)
    {
      typedef typename _Operation::first_argument_type _Arg1_type;
      return binder1st<_Operation>(__fn, _Arg1_type(__x));
    }

  
  template<typename _Operation>
    class binder2nd
    : public unary_function<typename _Operation::first_argument_type,
			    typename _Operation::result_type>
    {
    protected:
      _Operation op;
      typename _Operation::second_argument_type value;

    public:
      binder2nd(const _Operation& __x,
		const typename _Operation::second_argument_type& __y)
      : op(__x), value(__y) { }

      typename _Operation::result_type
      operator()(const typename _Operation::first_argument_type& __x) const
      { return op(__x, value); }

      
      
      typename _Operation::result_type
      operator()(typename _Operation::first_argument_type& __x) const
      { return op(__x, value); }
    } __attribute__ ((__deprecated__));

  
  template<typename _Operation, typename _Tp>
    inline binder2nd<_Operation>
    bind2nd(const _Operation& __fn, const _Tp& __x)
    {
      typedef typename _Operation::second_argument_type _Arg2_type;
      return binder2nd<_Operation>(__fn, _Arg2_type(__x));
    } 
   


} 





























namespace std 
{


  
  struct allocator_arg_t { };

  constexpr allocator_arg_t allocator_arg = allocator_arg_t();

template<typename _Tp> class __has_allocator_type_helper : __sfinae_types { template<typename _Up> struct _Wrap_type { }; template<typename _Up> static __one __test(_Wrap_type<typename _Up::allocator_type> *); template<typename _Up> static __two __test(...); public: static constexpr bool value = sizeof(__test<_Tp>(0)) == 1; }; template<typename _Tp> struct __has_allocator_type : integral_constant<bool, __has_allocator_type_helper <typename remove_cv<_Tp> ::type> ::value> { };

  template<typename _Tp, typename _Alloc,
	   bool = __has_allocator_type<_Tp>::value>
    struct __uses_allocator_helper
    : public false_type { };

  template<typename _Tp, typename _Alloc>
    struct __uses_allocator_helper<_Tp, _Alloc, true>
    : public integral_constant<bool, is_convertible<_Alloc,
				     typename _Tp::allocator_type>::value>
    { };

  
  template<typename _Tp, typename _Alloc>
    struct uses_allocator
    : public integral_constant<bool,
			       __uses_allocator_helper<_Tp, _Alloc>::value>
    { };

  template<typename _Tp, typename _Alloc, typename... _Args>
    struct __uses_allocator_arg
    : is_constructible<_Tp, _Alloc, _Args...>
    { static_assert( uses_allocator<_Tp, _Alloc>::value, "uses allocator" ); };

  struct __uses_alloc_base { };
  struct __uses_alloc0 : __uses_alloc_base
  { struct _Anything { _Anything(...) { } } _M_a; };
  template<typename _Alloc>
    struct __uses_alloc1 : __uses_alloc_base { const _Alloc* _M_a; };
  template<typename _Alloc>
    struct __uses_alloc2 : __uses_alloc_base { const _Alloc* _M_a; };

  template<bool, typename _Alloc, typename... _Args>
    struct __uses_alloc;

  template<typename _Tp, typename _Alloc, typename... _Args>
    struct __uses_alloc<true, _Tp, _Alloc, _Args...>
    : conditional<
        is_constructible<_Tp, allocator_arg_t, _Alloc, _Args...>::value,
        __uses_alloc1<_Alloc>,
       	__uses_alloc2<_Alloc>>::type
    { };

  template<typename _Tp, typename _Alloc, typename... _Args>
    struct __uses_alloc<false, _Tp, _Alloc, _Args...>
    : __uses_alloc0 { };

  template<typename _Tp, typename _Alloc, typename... _Args>
    struct __uses_alloc_impl
    : __uses_alloc<uses_allocator<_Tp, _Alloc>::value, _Tp,  _Alloc, _Args...>
    { };

  template<typename _Tp, typename _Alloc, typename... _Args>
    __uses_alloc_impl<_Tp, _Alloc, _Args...>
    __use_alloc(const _Alloc& __a)
    {
      __uses_alloc_impl<_Tp, _Alloc, _Args...> __ret;
      __ret._M_a = &__a;
      return __ret;
    }


} 





































 



 


#pragma GCC system_header





























 


#pragma GCC system_header


















































 



 


#pragma GCC system_header







 

















































 












 


namespace std 
{
  namespace rel_ops
  {
  

    

 

    






 
    template <class _Tp>
      inline bool
      operator!=(const _Tp& __x, const _Tp& __y)
      { return !(__x == __y); }

    






 
    template <class _Tp>
      inline bool
      operator>(const _Tp& __x, const _Tp& __y)
      { return __y < __x; }

    






 
    template <class _Tp>
      inline bool
      operator<=(const _Tp& __x, const _Tp& __y)
      { return !(__y < __x); }

    






 
    template <class _Tp>
      inline bool
      operator>=(const _Tp& __x, const _Tp& __y)
      { return !(__x < __y); }

  
  } 

} 





























 


#pragma GCC system_header


#pragma GCC visibility push(default)


namespace std
{
  
  template<class _E>
    class initializer_list
    {
    public:
      typedef _E 		value_type;
      typedef const _E& 	reference;
      typedef const _E& 	const_reference;
      typedef size_t 		size_type;
      typedef const _E* 	iterator;
      typedef const _E* 	const_iterator;

    private:
      iterator			_M_array;
      size_type			_M_len;

      
      constexpr initializer_list(const_iterator __a, size_type __l)
      : _M_array(__a), _M_len(__l) { }

    public:
      constexpr initializer_list() noexcept
      : _M_array(0), _M_len(0) { }

      
      constexpr size_type
      size() const noexcept { return _M_len; }

      
      constexpr const_iterator
      begin() const noexcept { return _M_array; }

      
      constexpr const_iterator
      end() const noexcept { return begin() + size(); }
    };

  



 
  template<class _Tp>
    constexpr const _Tp*
    begin(initializer_list<_Tp> __ils) noexcept
    { return __ils.begin(); }

  



 
  template<class _Tp>
    constexpr const _Tp*
    end(initializer_list<_Tp> __ils) noexcept
    { return __ils.end(); }
}

#pragma GCC visibility pop



namespace std 
{


  template<class _Tp>
    class tuple_size;

  template<std::size_t _Int, class _Tp>
    class tuple_element;

   
  template<class _Tp1, class _Tp2>
    struct tuple_size<std::pair<_Tp1, _Tp2>>
    : public integral_constant<std::size_t, 2> { };

  template<class _Tp1, class _Tp2>
    struct tuple_element<0, std::pair<_Tp1, _Tp2>>
    { typedef _Tp1 type; };
 
  template<class _Tp1, class _Tp2>
    struct tuple_element<1, std::pair<_Tp1, _Tp2>>
    { typedef _Tp2 type; };

  template<std::size_t _Int>
    struct __pair_get;

  template<>
    struct __pair_get<0>
    {
      template<typename _Tp1, typename _Tp2>
        static constexpr _Tp1&
        __get(std::pair<_Tp1, _Tp2>& __pair) noexcept
        { return __pair.first; }

      template<typename _Tp1, typename _Tp2>
        static constexpr _Tp1&&
        __move_get(std::pair<_Tp1, _Tp2>&& __pair) noexcept
        { return std::forward<_Tp1>(__pair.first); }

      template<typename _Tp1, typename _Tp2>
        static constexpr const _Tp1&
        __const_get(const std::pair<_Tp1, _Tp2>& __pair) noexcept
        { return __pair.first; }
    };

  template<>
    struct __pair_get<1>
    {
      template<typename _Tp1, typename _Tp2>
        static constexpr _Tp2&
        __get(std::pair<_Tp1, _Tp2>& __pair) noexcept
        { return __pair.second; }

      template<typename _Tp1, typename _Tp2>
        static constexpr _Tp2&&
        __move_get(std::pair<_Tp1, _Tp2>&& __pair) noexcept
        { return std::forward<_Tp2>(__pair.second); }

      template<typename _Tp1, typename _Tp2>
        static constexpr const _Tp2&
        __const_get(const std::pair<_Tp1, _Tp2>& __pair) noexcept
        { return __pair.second; }
    };

  template<std::size_t _Int, class _Tp1, class _Tp2>
    constexpr typename tuple_element<_Int, std::pair<_Tp1, _Tp2>>::type&
    get(std::pair<_Tp1, _Tp2>& __in) noexcept
    { return __pair_get<_Int>::__get(__in); }

  template<std::size_t _Int, class _Tp1, class _Tp2>
    constexpr typename tuple_element<_Int, std::pair<_Tp1, _Tp2>>::type&&
    get(std::pair<_Tp1, _Tp2>&& __in) noexcept
    { return __pair_get<_Int>::__move_get(std::move(__in)); }

  template<std::size_t _Int, class _Tp1, class _Tp2>
    constexpr const typename tuple_element<_Int, std::pair<_Tp1, _Tp2>>::type&
    get(const std::pair<_Tp1, _Tp2>& __in) noexcept
    { return __pair_get<_Int>::__const_get(__in); }


} 




























 


#pragma GCC system_header




























 






#pragma GCC system_header



























 






#pragma GCC system_header




























 






#pragma GCC system_header

































 





#pragma GCC system_header

















 




 



 


namespace __gnu_cxx 
{


  








 
  template<typename _CharT>
    struct _Char_types
    {
      typedef unsigned long   int_type;
      typedef std::streampos  pos_type;
      typedef std::streamoff  off_type;
      typedef std::mbstate_t  state_type;
    };


  













 
  template<typename _CharT>
    struct char_traits
    {
      typedef _CharT                                    char_type;
      typedef typename _Char_types<_CharT>::int_type    int_type;
      typedef typename _Char_types<_CharT>::pos_type    pos_type;
      typedef typename _Char_types<_CharT>::off_type    off_type;
      typedef typename _Char_types<_CharT>::state_type  state_type;

      static void
      assign(char_type& __c1, const char_type& __c2)
      { __c1 = __c2; }

      static constexpr bool
      eq(const char_type& __c1, const char_type& __c2)
      { return __c1 == __c2; }

      static constexpr bool
      lt(const char_type& __c1, const char_type& __c2)
      { return __c1 < __c2; }

      static int
      compare(const char_type* __s1, const char_type* __s2, std::size_t __n);

      static std::size_t
      length(const char_type* __s);

      static const char_type*
      find(const char_type* __s, std::size_t __n, const char_type& __a);

      static char_type*
      move(char_type* __s1, const char_type* __s2, std::size_t __n);

      static char_type*
      copy(char_type* __s1, const char_type* __s2, std::size_t __n);

      static char_type*
      assign(char_type* __s, std::size_t __n, char_type __a);

      static constexpr char_type
      to_char_type(const int_type& __c)
      { return static_cast<char_type>(__c); }

      static constexpr int_type
      to_int_type(const char_type& __c)
      { return static_cast<int_type>(__c); }

      static constexpr bool
      eq_int_type(const int_type& __c1, const int_type& __c2)
      { return __c1 == __c2; }

      static constexpr int_type
      eof()
      { return static_cast<int_type>(-1); }

      static constexpr int_type
      not_eof(const int_type& __c)
      { return !eq_int_type(__c, eof()) ? __c : to_int_type(char_type()); }
    };

  template<typename _CharT>
    int
    char_traits<_CharT>::
    compare(const char_type* __s1, const char_type* __s2, std::size_t __n)
    {
      for (std::size_t __i = 0; __i < __n; ++__i)
	if (lt(__s1[__i], __s2[__i]))
	  return -1;
	else if (lt(__s2[__i], __s1[__i]))
	  return 1;
      return 0;
    }

  template<typename _CharT>
    std::size_t
    char_traits<_CharT>::
    length(const char_type* __p)
    {
      std::size_t __i = 0;
      while (!eq(__p[__i], char_type()))
        ++__i;
      return __i;
    }

  template<typename _CharT>
    const typename char_traits<_CharT>::char_type*
    char_traits<_CharT>::
    find(const char_type* __s, std::size_t __n, const char_type& __a)
    {
      for (std::size_t __i = 0; __i < __n; ++__i)
        if (eq(__s[__i], __a))
          return __s + __i;
      return 0;
    }

  template<typename _CharT>
    typename char_traits<_CharT>::char_type*
    char_traits<_CharT>::
    move(char_type* __s1, const char_type* __s2, std::size_t __n)
    {
      return static_cast<_CharT*>(__builtin_memmove(__s1, __s2,
						    __n * sizeof(char_type)));
    }

  template<typename _CharT>
    typename char_traits<_CharT>::char_type*
    char_traits<_CharT>::
    copy(char_type* __s1, const char_type* __s2, std::size_t __n)
    {
      
      std::copy(__s2, __s2 + __n, __s1);
      return __s1;
    }

  template<typename _CharT>
    typename char_traits<_CharT>::char_type*
    char_traits<_CharT>::
    assign(char_type* __s, std::size_t __n, char_type __a)
    {
      
      std::fill_n(__s, __n, __a);
      return __s;
    }


} 

namespace std 
{


  
  










 
  template<class _CharT>
    struct char_traits : public __gnu_cxx::char_traits<_CharT>
    { };


  
  template<>
    struct char_traits<char>
    {
      typedef char              char_type;
      typedef int               int_type;
      typedef streampos         pos_type;
      typedef streamoff         off_type;
      typedef mbstate_t         state_type;

      static void
      assign(char_type& __c1, const char_type& __c2) noexcept
      { __c1 = __c2; }

      static constexpr bool
      eq(const char_type& __c1, const char_type& __c2) noexcept
      { return __c1 == __c2; }

      static constexpr bool
      lt(const char_type& __c1, const char_type& __c2) noexcept
      { return __c1 < __c2; }

      static int
      compare(const char_type* __s1, const char_type* __s2, size_t __n)
      { return __builtin_memcmp(__s1, __s2, __n); }

      static size_t
      length(const char_type* __s)
      { return __builtin_strlen(__s); }

      static const char_type*
      find(const char_type* __s, size_t __n, const char_type& __a)
      { return static_cast<const char_type*>(__builtin_memchr(__s, __a, __n)); }

      static char_type*
      move(char_type* __s1, const char_type* __s2, size_t __n)
      { return static_cast<char_type*>(__builtin_memmove(__s1, __s2, __n)); }

      static char_type*
      copy(char_type* __s1, const char_type* __s2, size_t __n)
      { return static_cast<char_type*>(__builtin_memcpy(__s1, __s2, __n)); }

      static char_type*
      assign(char_type* __s, size_t __n, char_type __a)
      { return static_cast<char_type*>(__builtin_memset(__s, __a, __n)); }

      static constexpr char_type
      to_char_type(const int_type& __c) noexcept
      { return static_cast<char_type>(__c); }

      
      
      static constexpr int_type
      to_int_type(const char_type& __c) noexcept
      { return static_cast<int_type>(static_cast<unsigned char>(__c)); }

      static constexpr bool
      eq_int_type(const int_type& __c1, const int_type& __c2) noexcept
      { return __c1 == __c2; }

      static constexpr int_type
      eof() noexcept
      { return static_cast<int_type>(-1); }

      static constexpr int_type
      not_eof(const int_type& __c) noexcept
      { return (__c == eof()) ? 0 : __c; }
  };


  
  template<>
    struct char_traits<wchar_t>
    {
      typedef wchar_t           char_type;
      typedef wint_t            int_type;
      typedef streamoff         off_type;
      typedef wstreampos        pos_type;
      typedef mbstate_t         state_type;

      static void
      assign(char_type& __c1, const char_type& __c2) noexcept
      { __c1 = __c2; }

      static constexpr bool
      eq(const char_type& __c1, const char_type& __c2) noexcept
      { return __c1 == __c2; }

      static constexpr bool
      lt(const char_type& __c1, const char_type& __c2) noexcept
      { return __c1 < __c2; }

      static int
      compare(const char_type* __s1, const char_type* __s2, size_t __n)
      { return wmemcmp(__s1, __s2, __n); }

      static size_t
      length(const char_type* __s)
      { return wcslen(__s); }

      static const char_type*
      find(const char_type* __s, size_t __n, const char_type& __a)
      { return wmemchr(__s, __a, __n); }

      static char_type*
      move(char_type* __s1, const char_type* __s2, size_t __n)
      { return wmemmove(__s1, __s2, __n); }

      static char_type*
      copy(char_type* __s1, const char_type* __s2, size_t __n)
      { return wmemcpy(__s1, __s2, __n); }

      static char_type*
      assign(char_type* __s, size_t __n, char_type __a)
      { return wmemset(__s, __a, __n); }

      static constexpr char_type
      to_char_type(const int_type& __c) noexcept
      { return char_type(__c); }

      static constexpr int_type
      to_int_type(const char_type& __c) noexcept
      { return int_type(__c); }

      static constexpr bool
      eq_int_type(const int_type& __c1, const int_type& __c2) noexcept
      { return __c1 == __c2; }

      static constexpr int_type
      eof() noexcept
      { return static_cast<int_type>((0xffffffffu)); }

      static constexpr int_type
      not_eof(const int_type& __c) noexcept
      { return eq_int_type(__c, eof()) ? 0 : __c; }
  };


} 




























 


#pragma GCC system_header


















 



 


 


 

 

 

 
typedef signed char		int8_t;
typedef short int		int16_t;
typedef int			int32_t;
typedef long int		int64_t;

 
typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long int	uint64_t;


 

 
typedef signed char		int_least8_t;
typedef short int		int_least16_t;
typedef int			int_least32_t;
typedef long int		int_least64_t;

 
typedef unsigned char		uint_least8_t;
typedef unsigned short int	uint_least16_t;
typedef unsigned int		uint_least32_t;
typedef unsigned long int	uint_least64_t;


 

 
typedef signed char		int_fast8_t;
typedef long int		int_fast16_t;
typedef long int		int_fast32_t;
typedef long int		int_fast64_t;

 
typedef unsigned char		uint_fast8_t;
typedef unsigned long int	uint_fast16_t;
typedef unsigned long int	uint_fast32_t;
typedef unsigned long int	uint_fast64_t;


 
typedef long int		intptr_t;
typedef unsigned long int	uintptr_t;


 
typedef long int		intmax_t;
typedef unsigned long int	uintmax_t;



 


 

 
 

 


 
 

 


 
 

 


 


 
 

 


 

 

 

 

 

 




 

 

 

 




namespace std
{
  using ::int8_t;
  using ::int16_t;
  using ::int32_t;
  using ::int64_t;

  using ::int_fast8_t;
  using ::int_fast16_t;
  using ::int_fast32_t;
  using ::int_fast64_t;

  using ::int_least8_t;
  using ::int_least16_t;
  using ::int_least32_t;
  using ::int_least64_t;

  using ::intmax_t;
  using ::intptr_t;
  
  using ::uint8_t;
  using ::uint16_t;
  using ::uint32_t;
  using ::uint64_t;

  using ::uint_fast8_t;
  using ::uint_fast16_t;
  using ::uint_fast32_t;
  using ::uint_fast64_t;

  using ::uint_least8_t;
  using ::uint_least16_t;
  using ::uint_least32_t;
  using ::uint_least64_t;

  using ::uintmax_t;
  using ::uintptr_t;
} 




namespace std 
{


  template<>
    struct char_traits<char16_t>
    {
      typedef char16_t          char_type;
      typedef uint_least16_t    int_type;
      typedef streamoff         off_type;
      typedef u16streampos      pos_type;
      typedef mbstate_t         state_type;

      static void
      assign(char_type& __c1, const char_type& __c2) noexcept
      { __c1 = __c2; }

      static constexpr bool
      eq(const char_type& __c1, const char_type& __c2) noexcept
      { return __c1 == __c2; }

      static constexpr bool
      lt(const char_type& __c1, const char_type& __c2) noexcept
      { return __c1 < __c2; }

      static int
      compare(const char_type* __s1, const char_type* __s2, size_t __n)
      {
	for (size_t __i = 0; __i < __n; ++__i)
	  if (lt(__s1[__i], __s2[__i]))
	    return -1;
	  else if (lt(__s2[__i], __s1[__i]))
	    return 1;
	return 0;
      }

      static size_t
      length(const char_type* __s)
      {
	size_t __i = 0;
	while (!eq(__s[__i], char_type()))
	  ++__i;
	return __i;
      }

      static const char_type*
      find(const char_type* __s, size_t __n, const char_type& __a)
      {
	for (size_t __i = 0; __i < __n; ++__i)
	  if (eq(__s[__i], __a))
	    return __s + __i;
	return 0;
      }

      static char_type*
      move(char_type* __s1, const char_type* __s2, size_t __n)
      {
	return (static_cast<char_type*>
		(__builtin_memmove(__s1, __s2, __n * sizeof(char_type))));
      }

      static char_type*
      copy(char_type* __s1, const char_type* __s2, size_t __n)
      {
	return (static_cast<char_type*>
		(__builtin_memcpy(__s1, __s2, __n * sizeof(char_type))));
      }

      static char_type*
      assign(char_type* __s, size_t __n, char_type __a)
      {
	for (size_t __i = 0; __i < __n; ++__i)
	  assign(__s[__i], __a);
	return __s;
      }

      static constexpr char_type
      to_char_type(const int_type& __c) noexcept
      { return char_type(__c); }

      static constexpr int_type
      to_int_type(const char_type& __c) noexcept
      { return int_type(__c); }

      static constexpr bool
      eq_int_type(const int_type& __c1, const int_type& __c2) noexcept
      { return __c1 == __c2; }

      static constexpr int_type
      eof() noexcept
      { return static_cast<int_type>(-1); }

      static constexpr int_type
      not_eof(const int_type& __c) noexcept
      { return eq_int_type(__c, eof()) ? 0 : __c; }
    };

  template<>
    struct char_traits<char32_t>
    {
      typedef char32_t          char_type;
      typedef uint_least32_t    int_type;
      typedef streamoff         off_type;
      typedef u32streampos      pos_type;
      typedef mbstate_t         state_type;

      static void
      assign(char_type& __c1, const char_type& __c2) noexcept
      { __c1 = __c2; }

      static constexpr bool
      eq(const char_type& __c1, const char_type& __c2) noexcept
      { return __c1 == __c2; }

      static constexpr bool
      lt(const char_type& __c1, const char_type& __c2) noexcept
      { return __c1 < __c2; }

      static int
      compare(const char_type* __s1, const char_type* __s2, size_t __n)
      {
	for (size_t __i = 0; __i < __n; ++__i)
	  if (lt(__s1[__i], __s2[__i]))
	    return -1;
	  else if (lt(__s2[__i], __s1[__i]))
	    return 1;
	return 0;
      }

      static size_t
      length(const char_type* __s)
      {
	size_t __i = 0;
	while (!eq(__s[__i], char_type()))
	  ++__i;
	return __i;
      }

      static const char_type*
      find(const char_type* __s, size_t __n, const char_type& __a)
      {
	for (size_t __i = 0; __i < __n; ++__i)
	  if (eq(__s[__i], __a))
	    return __s + __i;
	return 0;
      }

      static char_type*
      move(char_type* __s1, const char_type* __s2, size_t __n)
      {
	return (static_cast<char_type*>
		(__builtin_memmove(__s1, __s2, __n * sizeof(char_type))));
      }

      static char_type*
      copy(char_type* __s1, const char_type* __s2, size_t __n)
      { 
	return (static_cast<char_type*>
		(__builtin_memcpy(__s1, __s2, __n * sizeof(char_type))));
      }

      static char_type*
      assign(char_type* __s, size_t __n, char_type __a)
      {
	for (size_t __i = 0; __i < __n; ++__i)
	  assign(__s[__i], __a);
	return __s;
      }

      static constexpr char_type
      to_char_type(const int_type& __c) noexcept
      { return char_type(__c); }

      static constexpr int_type
      to_int_type(const char_type& __c) noexcept
      { return int_type(__c); }

      static constexpr bool
      eq_int_type(const int_type& __c1, const int_type& __c2) noexcept
      { return __c1 == __c2; }

      static constexpr int_type
      eof() noexcept
      { return static_cast<int_type>(-1); }

      static constexpr int_type
      not_eof(const int_type& __c) noexcept
      { return eq_int_type(__c, eof()) ? 0 : __c; }
    };


} 





























 






#pragma GCC system_header




























 








#pragma GCC system_header

































 





#pragma GCC system_header

















 



 
























 



 



 


 


 
 




 


 






 





 

 


 

 


 





 


 



 
 
 


 



















 





extern "C" {


 




 
struct lconv
{
   

  char *decimal_point;		 
  char *thousands_sep;		 
  



 
  char *grouping;

   

  
 
  char *int_curr_symbol;
  char *currency_symbol;	 
  char *mon_decimal_point;	 
  char *mon_thousands_sep;	 
  char *mon_grouping;		 
  char *positive_sign;		 
  char *negative_sign;		 
  char int_frac_digits;		 
  char frac_digits;		 
   
  char p_cs_precedes;
   
  char p_sep_by_space;
   
  char n_cs_precedes;
   
  char n_sep_by_space;
  




 
  char p_sign_posn;
  char n_sign_posn;
   
  char int_p_cs_precedes;
   
  char int_p_sep_by_space;
   
  char int_n_cs_precedes;
   
  char int_n_sep_by_space;
  




 
  char int_p_sign_posn;
  char int_n_sign_posn;
};


 
extern char *setlocale (int __category, const char *__locale) throw ();

 
extern struct lconv *localeconv (void) throw ();













 

 




 
extern __locale_t newlocale (int __category_mask, const char *__locale,
			     __locale_t __base) throw ();




 


 
extern __locale_t duplocale (__locale_t __dataset) throw ();


 
extern void freelocale (__locale_t __dataset) throw ();





 
extern __locale_t uselocale (__locale_t __dataset) throw ();


 


}





namespace std
{
  using ::lconv;
  using ::setlocale;
  using ::localeconv;
} 




namespace __gnu_cxx 
{


  extern "C" __typeof(uselocale) __uselocale;


} 

namespace std 
{


  typedef __locale_t		__c_locale;

  
  
  
  
  inline int
  __convert_from_v(const __c_locale& __cloc __attribute__ ((__unused__)),
		   char* __out,
		   const int __size __attribute__ ((__unused__)),
		   const char* __fmt, ...)
  {
    __c_locale __old = __gnu_cxx::__uselocale(__cloc);

    __builtin_va_list __args;
    __builtin_va_start(__args, __fmt);

    const int __ret = __builtin_vsnprintf(__out, __size, __fmt, __args);

    __builtin_va_end(__args);

    __gnu_cxx::__uselocale(__old);
    return __ret;
  }


} 

































 





#pragma GCC system_header

















 



 



extern "C" {







 


enum
{
  _ISupper = ((0) < 8 ? ((1 << (0)) << 8) : ((1 << (0)) >> 8)),	 
  _ISlower = ((1) < 8 ? ((1 << (1)) << 8) : ((1 << (1)) >> 8)),	 
  _ISalpha = ((2) < 8 ? ((1 << (2)) << 8) : ((1 << (2)) >> 8)),	 
  _ISdigit = ((3) < 8 ? ((1 << (3)) << 8) : ((1 << (3)) >> 8)),	 
  _ISxdigit = ((4) < 8 ? ((1 << (4)) << 8) : ((1 << (4)) >> 8)),	 
  _ISspace = ((5) < 8 ? ((1 << (5)) << 8) : ((1 << (5)) >> 8)),	 
  _ISprint = ((6) < 8 ? ((1 << (6)) << 8) : ((1 << (6)) >> 8)),	 
  _ISgraph = ((7) < 8 ? ((1 << (7)) << 8) : ((1 << (7)) >> 8)),	 
  _ISblank = ((8) < 8 ? ((1 << (8)) << 8) : ((1 << (8)) >> 8)),	 
  _IScntrl = ((9) < 8 ? ((1 << (9)) << 8) : ((1 << (9)) >> 8)),	 
  _ISpunct = ((10) < 8 ? ((1 << (10)) << 8) : ((1 << (10)) >> 8)),	 
  _ISalnum = ((11) < 8 ? ((1 << (11)) << 8) : ((1 << (11)) >> 8))	 
};
















 
extern const unsigned short int **__ctype_b_loc (void)
     throw () __attribute__ ((__const__));
extern const __int32_t **__ctype_tolower_loc (void)
     throw () __attribute__ ((__const__));
extern const __int32_t **__ctype_toupper_loc (void)
     throw () __attribute__ ((__const__));










 
extern int isalnum (int) throw ();
extern int isalpha (int) throw ();
extern int iscntrl (int) throw ();
extern int isdigit (int) throw ();
extern int islower (int) throw ();
extern int isgraph (int) throw ();
extern int isprint (int) throw ();
extern int ispunct (int) throw ();
extern int isspace (int) throw ();
extern int isupper (int) throw ();
extern int isxdigit (int) throw ();


 
extern int tolower (int __c) throw ();

 
extern int toupper (int __c) throw ();




 


extern int isblank (int) throw ();



 
extern int isctype (int __c, int __mask) throw ();



 
extern int isascii (int __c) throw ();


 
extern int toascii (int __c) throw ();


 
extern int _toupper (int) throw ();
extern int _tolower (int) throw ();

 












 


 


 





 
extern int isalnum_l (int, __locale_t) throw ();
extern int isalpha_l (int, __locale_t) throw ();
extern int iscntrl_l (int, __locale_t) throw ();
extern int isdigit_l (int, __locale_t) throw ();
extern int islower_l (int, __locale_t) throw ();
extern int isgraph_l (int, __locale_t) throw ();
extern int isprint_l (int, __locale_t) throw ();
extern int ispunct_l (int, __locale_t) throw ();
extern int isspace_l (int, __locale_t) throw ();
extern int isupper_l (int, __locale_t) throw ();
extern int isxdigit_l (int, __locale_t) throw ();

extern int isblank_l (int, __locale_t) throw ();


 
extern int __tolower_l (int __c, __locale_t __l) throw ();
extern int tolower_l (int __c, __locale_t __l) throw ();

 
extern int __toupper_l (int __c, __locale_t __l) throw ();
extern int toupper_l (int __c, __locale_t __l) throw ();





}





namespace std
{
  using ::isalnum;
  using ::isalpha;
  using ::iscntrl;
  using ::isdigit;
  using ::isgraph;
  using ::islower;
  using ::isprint;
  using ::ispunct;
  using ::isspace;
  using ::isupper;
  using ::isxdigit;
  using ::tolower;
  using ::toupper;
} 




namespace std
{
  using ::isblank;
} 




namespace std 
{


  



 

  
  class locale;

  template<typename _Facet>
    bool
    has_facet(const locale&) throw();

  template<typename _Facet>
    const _Facet&
    use_facet(const locale&);

  
  template<typename _CharT>
    bool
    isspace(_CharT, const locale&);

  template<typename _CharT>
    bool
    isprint(_CharT, const locale&);

  template<typename _CharT>
    bool
    iscntrl(_CharT, const locale&);

  template<typename _CharT>
    bool
    isupper(_CharT, const locale&);

  template<typename _CharT>
    bool
    islower(_CharT, const locale&);

  template<typename _CharT>
    bool
    isalpha(_CharT, const locale&);

  template<typename _CharT>
    bool
    isdigit(_CharT, const locale&);

  template<typename _CharT>
    bool
    ispunct(_CharT, const locale&);

  template<typename _CharT>
    bool
    isxdigit(_CharT, const locale&);

  template<typename _CharT>
    bool
    isalnum(_CharT, const locale&);

  template<typename _CharT>
    bool
    isgraph(_CharT, const locale&);

  template<typename _CharT>
    _CharT
    toupper(_CharT, const locale&);

  template<typename _CharT>
    _CharT
    tolower(_CharT, const locale&);

  
  class ctype_base;
  template<typename _CharT>
    class ctype;
  template<> class ctype<char>;
  template<> class ctype<wchar_t>;
  template<typename _CharT>
    class ctype_byname;
  

  class codecvt_base;
  template<typename _InternT, typename _ExternT, typename _StateT>
    class codecvt;
  template<> class codecvt<char, char, mbstate_t>;
  template<> class codecvt<wchar_t, char, mbstate_t>;
  template<typename _InternT, typename _ExternT, typename _StateT>
    class codecvt_byname;

  

  template<typename _CharT, typename _InIter = istreambuf_iterator<_CharT> >
    class num_get;
  template<typename _CharT, typename _OutIter = ostreambuf_iterator<_CharT> >
    class num_put;

  template<typename _CharT> class numpunct;
  template<typename _CharT> class numpunct_byname;

  
  template<typename _CharT>
    class collate;
  template<typename _CharT> class
    collate_byname;

  
  class time_base;
  template<typename _CharT, typename _InIter =  istreambuf_iterator<_CharT> >
    class time_get;
  template<typename _CharT, typename _InIter =  istreambuf_iterator<_CharT> >
    class time_get_byname;
  template<typename _CharT, typename _OutIter = ostreambuf_iterator<_CharT> >
    class time_put;
  template<typename _CharT, typename _OutIter = ostreambuf_iterator<_CharT> >
    class time_put_byname;

  
  class money_base;

  template<typename _CharT, typename _InIter =  istreambuf_iterator<_CharT> >
    class money_get;
  template<typename _CharT, typename _OutIter = ostreambuf_iterator<_CharT> >
    class money_put;

  template<typename _CharT, bool _Intl = false>
    class moneypunct;
  template<typename _CharT, bool _Intl = false>
    class moneypunct_byname;

  
  class messages_base;
  template<typename _CharT>
    class messages;
  template<typename _CharT>
    class messages_byname;


} 




























 


#pragma GCC system_header


  


























 


#pragma GCC system_header

#pragma GCC visibility push(default)

namespace __cxxabiv1
{  
  





 
  class __forced_unwind
  {
    virtual ~__forced_unwind() throw();

    
    virtual void __pure_dummy() = 0; 
  };
}

#pragma GCC visibility pop


namespace std 
{


  template<typename _CharT, typename _Traits>
    inline void
    __ostream_write(basic_ostream<_CharT, _Traits>& __out,
		    const _CharT* __s, streamsize __n)
    {
      typedef basic_ostream<_CharT, _Traits>       __ostream_type;      
      typedef typename __ostream_type::ios_base    __ios_base;

      const streamsize __put = __out.rdbuf()->sputn(__s, __n);
      if (__put != __n)
	__out.setstate(__ios_base::badbit);
    }

  template<typename _CharT, typename _Traits>
    inline void
    __ostream_fill(basic_ostream<_CharT, _Traits>& __out, streamsize __n)
    {
      typedef basic_ostream<_CharT, _Traits>       __ostream_type;      
      typedef typename __ostream_type::ios_base    __ios_base;

      const _CharT __c = __out.fill();
      for (; __n > 0; --__n)
	{
	  const typename _Traits::int_type __put = __out.rdbuf()->sputc(__c);
	  if (_Traits::eq_int_type(__put, _Traits::eof()))
	    {
	      __out.setstate(__ios_base::badbit);
	      break;
	    }
	}
    }

  template<typename _CharT, typename _Traits>
    basic_ostream<_CharT, _Traits>&
    __ostream_insert(basic_ostream<_CharT, _Traits>& __out,
		     const _CharT* __s, streamsize __n)
    {
      typedef basic_ostream<_CharT, _Traits>       __ostream_type;
      typedef typename __ostream_type::ios_base    __ios_base;

      typename __ostream_type::sentry __cerb(__out);
      if (__cerb)
	{
	  try
	    {
	      const streamsize __w = __out.width();
	      if (__w > __n)
		{
		  const bool __left = ((__out.flags()
					& __ios_base::adjustfield)
				       == __ios_base::left);
		  if (!__left)
		    __ostream_fill(__out, __w - __n);
		  if (__out.good())
		    __ostream_write(__out, __s, __n);
		  if (__left && __out.good())
		    __ostream_fill(__out, __w - __n);
		}
	      else
		__ostream_write(__out, __s, __n);
	      __out.width(0);
	    }
	  catch(__cxxabiv1::__forced_unwind&)
	    {
	      __out._M_setstate(__ios_base::badbit);
	      throw;
	    }
	  catch(...)
	    { __out._M_setstate(__ios_base::badbit); }
	}
      return __out;
    }

  
  
  extern template ostream& __ostream_insert(ostream&, const char*, streamsize);

  extern template wostream& __ostream_insert(wostream&, const wchar_t*,
					     streamsize);


} 




























 


#pragma GCC system_header


namespace std 
{


  



 
  template<class _Container>
    inline auto
    begin(_Container& __cont) -> decltype(__cont.begin())
    { return __cont.begin(); }

  



 
  template<class _Container>
    inline auto
    begin(const _Container& __cont) -> decltype(__cont.begin())
    { return __cont.begin(); }

  



 
  template<class _Container>
    inline auto
    end(_Container& __cont) -> decltype(__cont.end())
    { return __cont.end(); }

  



 
  template<class _Container>
    inline auto
    end(const _Container& __cont) -> decltype(__cont.end())
    { return __cont.end(); }

  


 
  template<class _Tp, size_t _Nm>
    inline _Tp*
    begin(_Tp (&__arr)[_Nm])
    { return __arr; }

  



 
  template<class _Tp, size_t _Nm>
    inline _Tp*
    end(_Tp (&__arr)[_Nm])
    { return __arr + _Nm; }


} 





























 






#pragma GCC system_header


namespace std 
{


  



























































 
  
  template<typename _CharT, typename _Traits, typename _Alloc>
    class basic_string
    {
      typedef typename _Alloc::template rebind<_CharT>::other _CharT_alloc_type;

      
    public:
      typedef _Traits					    traits_type;
      typedef typename _Traits::char_type		    value_type;
      typedef _Alloc					    allocator_type;
      typedef typename _CharT_alloc_type::size_type	    size_type;
      typedef typename _CharT_alloc_type::difference_type   difference_type;
      typedef typename _CharT_alloc_type::reference	    reference;
      typedef typename _CharT_alloc_type::const_reference   const_reference;
      typedef typename _CharT_alloc_type::pointer	    pointer;
      typedef typename _CharT_alloc_type::const_pointer	    const_pointer;
      typedef __gnu_cxx::__normal_iterator<pointer, basic_string>  iterator;
      typedef __gnu_cxx::__normal_iterator<const_pointer, basic_string>
                                                            const_iterator;
      typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;
      typedef std::reverse_iterator<iterator>		    reverse_iterator;

    private:
      
      
      
      
      
      
      
      
      
      
      
      
      

      struct _Rep_base
      {
	size_type		_M_length;
	size_type		_M_capacity;
	_Atomic_word		_M_refcount;
      };

      struct _Rep : _Rep_base
      {
	
	typedef typename _Alloc::template rebind<char>::other _Raw_bytes_alloc;

	

	
	
	
	
	
	
	
	
	
	
	
	static const size_type	_S_max_size;
	static const _CharT	_S_terminal;

	
        
        static size_type _S_empty_rep_storage[];

        static _Rep&
        _S_empty_rep()
        { 
	  
	  
	  
	  void* __p = reinterpret_cast<void*>(&_S_empty_rep_storage);
	  return *reinterpret_cast<_Rep*>(__p);
	}

        bool
	_M_is_leaked() const
        { return this->_M_refcount < 0; }

        bool
	_M_is_shared() const
        { return this->_M_refcount > 0; }

        void
	_M_set_leaked()
        { this->_M_refcount = -1; }

        void
	_M_set_sharable()
        { this->_M_refcount = 0; }

	void
	_M_set_length_and_sharable(size_type __n)
	{
	  if (__builtin_expect(this != &_S_empty_rep(), false))
	    {
	      this->_M_set_sharable();  
	      this->_M_length = __n;
	      traits_type::assign(this->_M_refdata()[__n], _S_terminal);
	      
	      
	    }
	}

	_CharT*
	_M_refdata() throw()
	{ return reinterpret_cast<_CharT*>(this + 1); }

	_CharT*
	_M_grab(const _Alloc& __alloc1, const _Alloc& __alloc2)
	{
	  return (!_M_is_leaked() && __alloc1 == __alloc2)
	          ? _M_refcopy() : _M_clone(__alloc1);
	}

	
	static _Rep*
	_S_create(size_type, size_type, const _Alloc&);

	void
	_M_dispose(const _Alloc& __a)
	{
	  if (__builtin_expect(this != &_S_empty_rep(), false))
	    {
	      
	      ;
	      if (__gnu_cxx::__exchange_and_add_dispatch(&this->_M_refcount,
							 -1) <= 0)
		{
		  ;
		  _M_destroy(__a);
		}
	    }
	}  

	void
	_M_destroy(const _Alloc&) throw();

	_CharT*
	_M_refcopy() throw()
	{
	  if (__builtin_expect(this != &_S_empty_rep(), false))
            __gnu_cxx::__atomic_add_dispatch(&this->_M_refcount, 1);
	  return _M_refdata();
	}  

	_CharT*
	_M_clone(const _Alloc&, size_type __res = 0);
      };

      
      struct _Alloc_hider : _Alloc
      {
	_Alloc_hider(_CharT* __dat, const _Alloc& __a)
	: _Alloc(__a), _M_p(__dat) { }

	_CharT* _M_p; 
      };

    public:
      
      
      
      
      static const size_type	npos = static_cast<size_type>(-1);

    private:
      
      mutable _Alloc_hider	_M_dataplus;

      _CharT*
      _M_data() const
      { return  _M_dataplus._M_p; }

      _CharT*
      _M_data(_CharT* __p)
      { return (_M_dataplus._M_p = __p); }

      _Rep*
      _M_rep() const
      { return &((reinterpret_cast<_Rep*> (_M_data()))[-1]); }

      
      
      iterator
      _M_ibegin() const
      { return iterator(_M_data()); }

      iterator
      _M_iend() const
      { return iterator(_M_data() + this->size()); }

      void
      _M_leak()    
      {
	if (!_M_rep()->_M_is_leaked())
	  _M_leak_hard();
      }

      size_type
      _M_check(size_type __pos, const char* __s) const
      {
	if (__pos > this->size())
	  __throw_out_of_range((__s));
	return __pos;
      }

      void
      _M_check_length(size_type __n1, size_type __n2, const char* __s) const
      {
	if (this->max_size() - (this->size() - __n1) < __n2)
	  __throw_length_error((__s));
      }

      
      size_type
      _M_limit(size_type __pos, size_type __off) const
      {
	const bool __testoff =  __off < this->size() - __pos;
	return __testoff ? __off : this->size() - __pos;
      }

      
      bool
      _M_disjunct(const _CharT* __s) const
      {
	return (less<const _CharT*>()(__s, _M_data())
		|| less<const _CharT*>()(_M_data() + this->size(), __s));
      }

      
      
      static void
      _M_copy(_CharT* __d, const _CharT* __s, size_type __n)
      {
	if (__n == 1)
	  traits_type::assign(*__d, *__s);
	else
	  traits_type::copy(__d, __s, __n);
      }

      static void
      _M_move(_CharT* __d, const _CharT* __s, size_type __n)
      {
	if (__n == 1)
	  traits_type::assign(*__d, *__s);
	else
	  traits_type::move(__d, __s, __n);	  
      }

      static void
      _M_assign(_CharT* __d, size_type __n, _CharT __c)
      {
	if (__n == 1)
	  traits_type::assign(*__d, __c);
	else
	  traits_type::assign(__d, __n, __c);	  
      }

      
      
      template<class _Iterator>
        static void
        _S_copy_chars(_CharT* __p, _Iterator __k1, _Iterator __k2)
        {
	  for (; __k1 != __k2; ++__k1, ++__p)
	    traits_type::assign(*__p, *__k1); 
	}

      static void
      _S_copy_chars(_CharT* __p, iterator __k1, iterator __k2)
      { _S_copy_chars(__p, __k1.base(), __k2.base()); }

      static void
      _S_copy_chars(_CharT* __p, const_iterator __k1, const_iterator __k2)
      { _S_copy_chars(__p, __k1.base(), __k2.base()); }

      static void
      _S_copy_chars(_CharT* __p, _CharT* __k1, _CharT* __k2)
      { _M_copy(__p, __k1, __k2 - __k1); }

      static void
      _S_copy_chars(_CharT* __p, const _CharT* __k1, const _CharT* __k2)
      { _M_copy(__p, __k1, __k2 - __k1); }

      static int
      _S_compare(size_type __n1, size_type __n2)
      {
	const difference_type __d = difference_type(__n1 - __n2);

	if (__d > __gnu_cxx::__numeric_traits<int>::__max)
	  return __gnu_cxx::__numeric_traits<int>::__max;
	else if (__d < __gnu_cxx::__numeric_traits<int>::__min)
	  return __gnu_cxx::__numeric_traits<int>::__min;
	else
	  return int(__d);
      }

      void
      _M_mutate(size_type __pos, size_type __len1, size_type __len2);

      void
      _M_leak_hard();

      static _Rep&
      _S_empty_rep()
      { return _Rep::_S_empty_rep(); }

    public:
      
      
      

      

 
      basic_string()
      : _M_dataplus(_S_empty_rep()._M_refdata(), _Alloc()) { }

      

 
      explicit
      basic_string(const _Alloc& __a);

      
      


 
      basic_string(const basic_string& __str);
      




 
      basic_string(const basic_string& __str, size_type __pos,
		   size_type __n = npos);
      





 
      basic_string(const basic_string& __str, size_type __pos,
		   size_type __n, const _Alloc& __a);

      







 
      basic_string(const _CharT* __s, size_type __n,
		   const _Alloc& __a = _Alloc());
      



 
      basic_string(const _CharT* __s, const _Alloc& __a = _Alloc());
      




 
      basic_string(size_type __n, _CharT __c, const _Alloc& __a = _Alloc());

      





 
      basic_string(basic_string&& __str) noexcept
      : _M_dataplus(__str._M_dataplus)
      {
	__str._M_data(_S_empty_rep()._M_refdata());
      }

      



 
      basic_string(initializer_list<_CharT> __l, const _Alloc& __a = _Alloc());

      




 
      template<class _InputIterator>
        basic_string(_InputIterator __beg, _InputIterator __end,
		     const _Alloc& __a = _Alloc());

      

 
      ~basic_string() noexcept
      { _M_rep()->_M_dispose(this->get_allocator()); }

      


 
      basic_string&
      operator=(const basic_string& __str) 
      { return this->assign(__str); }

      


 
      basic_string&
      operator=(const _CharT* __s) 
      { return this->assign(__s); }

      





 
      basic_string&
      operator=(_CharT __c) 
      { 
	this->assign(1, __c); 
	return *this;
      }

      





 
      basic_string&
      operator=(basic_string&& __str)
      {
	
	this->swap(__str);
	return *this;
      }

      


 
      basic_string&
      operator=(initializer_list<_CharT> __l)
      {
	this->assign(__l.begin(), __l.size());
	return *this;
      }

      
      


 
      iterator
      begin() noexcept
      {
	_M_leak();
	return iterator(_M_data());
      }

      


 
      const_iterator
      begin() const noexcept
      { return const_iterator(_M_data()); }

      


 
      iterator
      end() noexcept
      {
	_M_leak();
	return iterator(_M_data() + this->size());
      }

      


 
      const_iterator
      end() const noexcept
      { return const_iterator(_M_data() + this->size()); }

      



 
      reverse_iterator
      rbegin() noexcept
      { return reverse_iterator(this->end()); }

      



 
      const_reverse_iterator
      rbegin() const noexcept
      { return const_reverse_iterator(this->end()); }

      



 
      reverse_iterator
      rend() noexcept
      { return reverse_iterator(this->begin()); }

      



 
      const_reverse_iterator
      rend() const noexcept
      { return const_reverse_iterator(this->begin()); }

      


 
      const_iterator
      cbegin() const noexcept
      { return const_iterator(this->_M_data()); }

      


 
      const_iterator
      cend() const noexcept
      { return const_iterator(this->_M_data() + this->size()); }

      



 
      const_reverse_iterator
      crbegin() const noexcept
      { return const_reverse_iterator(this->end()); }

      



 
      const_reverse_iterator
      crend() const noexcept
      { return const_reverse_iterator(this->begin()); }

    public:
      
      
      
      size_type
      size() const noexcept
      { return _M_rep()->_M_length; }

      
      
      size_type
      length() const noexcept
      { return _M_rep()->_M_length; }

      
      size_type
      max_size() const noexcept
      { return _Rep::_S_max_size; }

      








 
      void
      resize(size_type __n, _CharT __c);

      








 
      void
      resize(size_type __n)
      { this->resize(__n, _CharT()); }

      
      void
      shrink_to_fit()
      {
	if (capacity() > size())
	  {
	    try
	      { reserve(0); }
	    catch(...)
	      { }
	  }
      }

      


 
      size_type
      capacity() const noexcept
      { return _M_rep()->_M_capacity; }

      















 
      void
      reserve(size_type __res_arg = 0);

      

 
      void
      clear() noexcept
      { _M_mutate(0, this->size(), 0); }

      


 
      bool
      empty() const noexcept
      { return this->size() == 0; }

      
      








 
      const_reference
      operator[] (size_type __pos) const
      {
	;
	return _M_data()[__pos];
      }

      








 
      reference
      operator[](size_type __pos)
      {
        
	;
        
	;
	_M_leak();
	return _M_data()[__pos];
      }

      








 
      const_reference
      at(size_type __n) const
      {
	if (__n >= this->size())
	  __throw_out_of_range(("basic_string::at"));
	return _M_data()[__n];
      }

      









 
      reference
      at(size_type __n)
      {
	if (__n >= size())
	  __throw_out_of_range(("basic_string::at"));
	_M_leak();
	return _M_data()[__n];
      }

      


 
      reference
      front()
      { return operator[](0); }

      


 
      const_reference
      front() const
      { return operator[](0); }

      


 
      reference
      back()
      { return operator[](this->size() - 1); }

      


 
      const_reference
      back() const
      { return operator[](this->size() - 1); }

      
      



 
      basic_string&
      operator+=(const basic_string& __str)
      { return this->append(__str); }

      



 
      basic_string&
      operator+=(const _CharT* __s)
      { return this->append(__s); }

      



 
      basic_string&
      operator+=(_CharT __c)
      { 
	this->push_back(__c);
	return *this;
      }

      



 
      basic_string&
      operator+=(initializer_list<_CharT> __l)
      { return this->append(__l.begin(), __l.size()); }

      



 
      basic_string&
      append(const basic_string& __str);

      











 
      basic_string&
      append(const basic_string& __str, size_type __pos, size_type __n);

      




 
      basic_string&
      append(const _CharT* __s, size_type __n);

      



 
      basic_string&
      append(const _CharT* __s)
      {
	;
	return this->append(__s, traits_type::length(__s));
      }

      






 
      basic_string&
      append(size_type __n, _CharT __c);

      



 
      basic_string&
      append(initializer_list<_CharT> __l)
      { return this->append(__l.begin(), __l.size()); }

      






 
      template<class _InputIterator>
        basic_string&
        append(_InputIterator __first, _InputIterator __last)
        { return this->replace(_M_iend(), _M_iend(), __first, __last); }

      


 
      void
      push_back(_CharT __c)
      { 
	const size_type __len = 1 + this->size();
	if (__len > this->capacity() || _M_rep()->_M_is_shared())
	  this->reserve(__len);
	traits_type::assign(_M_data()[this->size()], __c);
	_M_rep()->_M_set_length_and_sharable(__len);
      }

      



 
      basic_string&
      assign(const basic_string& __str);

      






 
      basic_string&
      assign(basic_string&& __str)
      {
	this->swap(__str);
	return *this;
      }

      











 
      basic_string&
      assign(const basic_string& __str, size_type __pos, size_type __n)
      { return this->assign(__str._M_data()
			    + __str._M_check(__pos, "basic_string::assign"),
			    __str._M_limit(__pos, __n)); }

      








 
      basic_string&
      assign(const _CharT* __s, size_type __n);

      







 
      basic_string&
      assign(const _CharT* __s)
      {
	;
	return this->assign(__s, traits_type::length(__s));
      }

      







 
      basic_string&
      assign(size_type __n, _CharT __c)
      { return _M_replace_aux(size_type(0), this->size(), __n, __c); }

      






 
      template<class _InputIterator>
        basic_string&
        assign(_InputIterator __first, _InputIterator __last)
        { return this->replace(_M_ibegin(), _M_iend(), __first, __last); }

      



 
      basic_string&
      assign(initializer_list<_CharT> __l)
      { return this->assign(__l.begin(), __l.size()); }

      











 
      void
      insert(iterator __p, size_type __n, _CharT __c)
      {	this->replace(__p, __p, __n, __c);  }

      










 
      template<class _InputIterator>
        void
        insert(iterator __p, _InputIterator __beg, _InputIterator __end)
        { this->replace(__p, __p, __beg, __end); }

      




 
      void
      insert(iterator __p, initializer_list<_CharT> __l)
      {
	;
	this->insert(__p - _M_ibegin(), __l.begin(), __l.size());
      }

      










 
      basic_string&
      insert(size_type __pos1, const basic_string& __str)
      { return this->insert(__pos1, __str, size_type(0), __str.size()); }

      
















 
      basic_string&
      insert(size_type __pos1, const basic_string& __str,
	     size_type __pos2, size_type __n)
      { return this->insert(__pos1, __str._M_data()
			    + __str._M_check(__pos2, "basic_string::insert"),
			    __str._M_limit(__pos2, __n)); }

      














 
      basic_string&
      insert(size_type __pos, const _CharT* __s, size_type __n);

      













 
      basic_string&
      insert(size_type __pos, const _CharT* __s)
      {
	;
	return this->insert(__pos, __s, traits_type::length(__s));
      }

      














 
      basic_string&
      insert(size_type __pos, size_type __n, _CharT __c)
      { return _M_replace_aux(_M_check(__pos, "basic_string::insert"),
			      size_type(0), __n, __c); }

      











 
      iterator
      insert(iterator __p, _CharT __c)
      {
	;
	const size_type __pos = __p - _M_ibegin();
	_M_replace_aux(__pos, size_type(0), size_type(1), __c);
	_M_rep()->_M_set_leaked();
	return iterator(_M_data() + __pos);
      }

      













 
      basic_string&
      erase(size_type __pos = 0, size_type __n = npos)
      { 
	_M_mutate(_M_check(__pos, "basic_string::erase"),
		  _M_limit(__pos, __n), size_type(0));
	return *this;
      }

      






 
      iterator
      erase(iterator __position)
      {
	;
	const size_type __pos = __position - _M_ibegin();
	_M_mutate(__pos, size_type(1), size_type(0));
	_M_rep()->_M_set_leaked();
	return iterator(_M_data() + __pos);
      }

      







 
      iterator
      erase(iterator __first, iterator __last);
 
      



 
      void
      pop_back()
      { erase(size()-1, 1); }

      















 
      basic_string&
      replace(size_type __pos, size_type __n, const basic_string& __str)
      { return this->replace(__pos, __n, __str._M_data(), __str.size()); }

      
















 
      basic_string&
      replace(size_type __pos1, size_type __n1, const basic_string& __str,
	      size_type __pos2, size_type __n2)
      { return this->replace(__pos1, __n1, __str._M_data()
			     + __str._M_check(__pos2, "basic_string::replace"),
			     __str._M_limit(__pos2, __n2)); }

      
















 
      basic_string&
      replace(size_type __pos, size_type __n1, const _CharT* __s,
	      size_type __n2);

      














 
      basic_string&
      replace(size_type __pos, size_type __n1, const _CharT* __s)
      {
	;
	return this->replace(__pos, __n1, __s, traits_type::length(__s));
      }

      















 
      basic_string&
      replace(size_type __pos, size_type __n1, size_type __n2, _CharT __c)
      { return _M_replace_aux(_M_check(__pos, "basic_string::replace"),
			      _M_limit(__pos, __n1), __n2, __c); }

      











 
      basic_string&
      replace(iterator __i1, iterator __i2, const basic_string& __str)
      { return this->replace(__i1, __i2, __str._M_data(), __str.size()); }

      













 
      basic_string&
      replace(iterator __i1, iterator __i2, const _CharT* __s, size_type __n)
      {
	;
	return this->replace(__i1 - _M_ibegin(), __i2 - __i1, __s, __n);
      }

      











 
      basic_string&
      replace(iterator __i1, iterator __i2, const _CharT* __s)
      {
	;
	return this->replace(__i1, __i2, __s, traits_type::length(__s));
      }

      












 
      basic_string&
      replace(iterator __i1, iterator __i2, size_type __n, _CharT __c)
      {
	;
	return _M_replace_aux(__i1 - _M_ibegin(), __i2 - __i1, __n, __c);
      }

      













 
      template<class _InputIterator>
        basic_string&
        replace(iterator __i1, iterator __i2,
		_InputIterator __k1, _InputIterator __k2)
        {
	  ;
	  ;
	  typedef typename std::__is_integer<_InputIterator>::__type _Integral;
	  return _M_replace_dispatch(__i1, __i2, __k1, __k2, _Integral());
	}

      
      
      basic_string&
      replace(iterator __i1, iterator __i2, _CharT* __k1, _CharT* __k2)
      {
	;
	;
	return this->replace(__i1 - _M_ibegin(), __i2 - __i1,
			     __k1, __k2 - __k1);
      }

      basic_string&
      replace(iterator __i1, iterator __i2,
	      const _CharT* __k1, const _CharT* __k2)
      {
	;
	;
	return this->replace(__i1 - _M_ibegin(), __i2 - __i1,
			     __k1, __k2 - __k1);
      }

      basic_string&
      replace(iterator __i1, iterator __i2, iterator __k1, iterator __k2)
      {
	;
	;
	return this->replace(__i1 - _M_ibegin(), __i2 - __i1,
			     __k1.base(), __k2 - __k1);
      }

      basic_string&
      replace(iterator __i1, iterator __i2,
	      const_iterator __k1, const_iterator __k2)
      {
	;
	;
	return this->replace(__i1 - _M_ibegin(), __i2 - __i1,
			     __k1.base(), __k2 - __k1);
      }
      
      












 
      basic_string& replace(iterator __i1, iterator __i2,
			    initializer_list<_CharT> __l)
      { return this->replace(__i1, __i2, __l.begin(), __l.end()); }

    private:
      template<class _Integer>
	basic_string&
	_M_replace_dispatch(iterator __i1, iterator __i2, _Integer __n,
			    _Integer __val, __true_type)
        { return _M_replace_aux(__i1 - _M_ibegin(), __i2 - __i1, __n, __val); }

      template<class _InputIterator>
	basic_string&
	_M_replace_dispatch(iterator __i1, iterator __i2, _InputIterator __k1,
			    _InputIterator __k2, __false_type);

      basic_string&
      _M_replace_aux(size_type __pos1, size_type __n1, size_type __n2,
		     _CharT __c);

      basic_string&
      _M_replace_safe(size_type __pos1, size_type __n1, const _CharT* __s,
		      size_type __n2);

      
      
      template<class _InIterator>
        static _CharT*
        _S_construct_aux(_InIterator __beg, _InIterator __end,
			 const _Alloc& __a, __false_type)
	{
          typedef typename iterator_traits<_InIterator>::iterator_category _Tag;
          return _S_construct(__beg, __end, __a, _Tag());
	}

      
      
      template<class _Integer>
        static _CharT*
        _S_construct_aux(_Integer __beg, _Integer __end,
			 const _Alloc& __a, __true_type)
        { return _S_construct_aux_2(static_cast<size_type>(__beg),
				    __end, __a); }

      static _CharT*
      _S_construct_aux_2(size_type __req, _CharT __c, const _Alloc& __a)
      { return _S_construct(__req, __c, __a); }

      template<class _InIterator>
        static _CharT*
        _S_construct(_InIterator __beg, _InIterator __end, const _Alloc& __a)
	{
	  typedef typename std::__is_integer<_InIterator>::__type _Integral;
	  return _S_construct_aux(__beg, __end, __a, _Integral());
        }

      
      template<class _InIterator>
        static _CharT*
         _S_construct(_InIterator __beg, _InIterator __end, const _Alloc& __a,
		      input_iterator_tag);

      
      
      template<class _FwdIterator>
        static _CharT*
        _S_construct(_FwdIterator __beg, _FwdIterator __end, const _Alloc& __a,
		     forward_iterator_tag);

      static _CharT*
      _S_construct(size_type __req, _CharT __c, const _Alloc& __a);

    public:

      










 
      size_type
      copy(_CharT* __s, size_type __n, size_type __pos = 0) const;

      





 
      void
      swap(basic_string& __s);

      
      




 
      const _CharT*
      c_str() const noexcept
      { return _M_data(); }

      




 
      const _CharT*
      data() const noexcept
      { return _M_data(); }

      

 
      allocator_type
      get_allocator() const noexcept
      { return _M_dataplus; }

      










 
      size_type
      find(const _CharT* __s, size_type __pos, size_type __n) const;

      








 
      size_type
      find(const basic_string& __str, size_type __pos = 0) const
	noexcept
      { return this->find(__str.data(), __pos, __str.size()); }

      








 
      size_type
      find(const _CharT* __s, size_type __pos = 0) const
      {
	;
	return this->find(__s, __pos, traits_type::length(__s));
      }

      








 
      size_type
      find(_CharT __c, size_type __pos = 0) const noexcept;

      








 
      size_type
      rfind(const basic_string& __str, size_type __pos = npos) const
	noexcept
      { return this->rfind(__str.data(), __pos, __str.size()); }

      










 
      size_type
      rfind(const _CharT* __s, size_type __pos, size_type __n) const;

      








 
      size_type
      rfind(const _CharT* __s, size_type __pos = npos) const
      {
	;
	return this->rfind(__s, __pos, traits_type::length(__s));
      }

      








 
      size_type
      rfind(_CharT __c, size_type __pos = npos) const noexcept;

      









 
      size_type
      find_first_of(const basic_string& __str, size_type __pos = 0) const
	noexcept
      { return this->find_first_of(__str.data(), __pos, __str.size()); }

      










 
      size_type
      find_first_of(const _CharT* __s, size_type __pos, size_type __n) const;

      








 
      size_type
      find_first_of(const _CharT* __s, size_type __pos = 0) const
      {
	;
	return this->find_first_of(__s, __pos, traits_type::length(__s));
      }

      










 
      size_type
      find_first_of(_CharT __c, size_type __pos = 0) const noexcept
      { return this->find(__c, __pos); }

      









 
      size_type
      find_last_of(const basic_string& __str, size_type __pos = npos) const
	noexcept
      { return this->find_last_of(__str.data(), __pos, __str.size()); }

      










 
      size_type
      find_last_of(const _CharT* __s, size_type __pos, size_type __n) const;

      








 
      size_type
      find_last_of(const _CharT* __s, size_type __pos = npos) const
      {
	;
	return this->find_last_of(__s, __pos, traits_type::length(__s));
      }

      










 
      size_type
      find_last_of(_CharT __c, size_type __pos = npos) const noexcept
      { return this->rfind(__c, __pos); }

      








 
      size_type
      find_first_not_of(const basic_string& __str, size_type __pos = 0) const
	noexcept
      { return this->find_first_not_of(__str.data(), __pos, __str.size()); }

      










 
      size_type
      find_first_not_of(const _CharT* __s, size_type __pos,
			size_type __n) const;

      








 
      size_type
      find_first_not_of(const _CharT* __s, size_type __pos = 0) const
      {
	;
	return this->find_first_not_of(__s, __pos, traits_type::length(__s));
      }

      








 
      size_type
      find_first_not_of(_CharT __c, size_type __pos = 0) const
	noexcept;

      









 
      size_type
      find_last_not_of(const basic_string& __str, size_type __pos = npos) const
	noexcept
      { return this->find_last_not_of(__str.data(), __pos, __str.size()); }

      










 
      size_type
      find_last_not_of(const _CharT* __s, size_type __pos,
		       size_type __n) const;
      









 
      size_type
      find_last_not_of(const _CharT* __s, size_type __pos = npos) const
      {
	;
	return this->find_last_not_of(__s, __pos, traits_type::length(__s));
      }

      








 
      size_type
      find_last_not_of(_CharT __c, size_type __pos = npos) const
	noexcept;

      










 
      basic_string
      substr(size_type __pos = 0, size_type __n = npos) const
      { return basic_string(*this,
			    _M_check(__pos, "basic_string::substr"), __n); }

      












 
      int
      compare(const basic_string& __str) const
      {
	const size_type __size = this->size();
	const size_type __osize = __str.size();
	const size_type __len = std::min(__size, __osize);

	int __r = traits_type::compare(_M_data(), __str.data(), __len);
	if (!__r)
	  __r = _S_compare(__size, __osize);
	return __r;
      }

      

















 
      int
      compare(size_type __pos, size_type __n, const basic_string& __str) const;

      





















 
      int
      compare(size_type __pos1, size_type __n1, const basic_string& __str,
	      size_type __pos2, size_type __n2) const;

      












 
      int
      compare(const _CharT* __s) const;

      
      
      

















 
      int
      compare(size_type __pos, size_type __n1, const _CharT* __s) const;

      






















 
      int
      compare(size_type __pos, size_type __n1, const _CharT* __s,
	      size_type __n2) const;
  };

  
  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>
    operator+(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	      const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    {
      basic_string<_CharT, _Traits, _Alloc> __str(__lhs);
      __str.append(__rhs);
      return __str;
    }

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT,_Traits,_Alloc>
    operator+(const _CharT* __lhs,
	      const basic_string<_CharT,_Traits,_Alloc>& __rhs);

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT,_Traits,_Alloc>
    operator+(_CharT __lhs, const basic_string<_CharT,_Traits,_Alloc>& __rhs);

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline basic_string<_CharT, _Traits, _Alloc>
    operator+(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	     const _CharT* __rhs)
    {
      basic_string<_CharT, _Traits, _Alloc> __str(__lhs);
      __str.append(__rhs);
      return __str;
    }

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline basic_string<_CharT, _Traits, _Alloc>
    operator+(const basic_string<_CharT, _Traits, _Alloc>& __lhs, _CharT __rhs)
    {
      typedef basic_string<_CharT, _Traits, _Alloc>	__string_type;
      typedef typename __string_type::size_type		__size_type;
      __string_type __str(__lhs);
      __str.append(__size_type(1), __rhs);
      return __str;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    inline basic_string<_CharT, _Traits, _Alloc>
    operator+(basic_string<_CharT, _Traits, _Alloc>&& __lhs,
	      const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    { return std::move(__lhs.append(__rhs)); }

  template<typename _CharT, typename _Traits, typename _Alloc>
    inline basic_string<_CharT, _Traits, _Alloc>
    operator+(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	      basic_string<_CharT, _Traits, _Alloc>&& __rhs)
    { return std::move(__rhs.insert(0, __lhs)); }

  template<typename _CharT, typename _Traits, typename _Alloc>
    inline basic_string<_CharT, _Traits, _Alloc>
    operator+(basic_string<_CharT, _Traits, _Alloc>&& __lhs,
	      basic_string<_CharT, _Traits, _Alloc>&& __rhs)
    {
      const auto __size = __lhs.size() + __rhs.size();
      const bool __cond = (__size > __lhs.capacity()
			   && __size <= __rhs.capacity());
      return __cond ? std::move(__rhs.insert(0, __lhs))
	            : std::move(__lhs.append(__rhs));
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    inline basic_string<_CharT, _Traits, _Alloc>
    operator+(const _CharT* __lhs,
	      basic_string<_CharT, _Traits, _Alloc>&& __rhs)
    { return std::move(__rhs.insert(0, __lhs)); }

  template<typename _CharT, typename _Traits, typename _Alloc>
    inline basic_string<_CharT, _Traits, _Alloc>
    operator+(_CharT __lhs,
	      basic_string<_CharT, _Traits, _Alloc>&& __rhs)
    { return std::move(__rhs.insert(0, 1, __lhs)); }

  template<typename _CharT, typename _Traits, typename _Alloc>
    inline basic_string<_CharT, _Traits, _Alloc>
    operator+(basic_string<_CharT, _Traits, _Alloc>&& __lhs,
	      const _CharT* __rhs)
    { return std::move(__lhs.append(__rhs)); }

  template<typename _CharT, typename _Traits, typename _Alloc>
    inline basic_string<_CharT, _Traits, _Alloc>
    operator+(basic_string<_CharT, _Traits, _Alloc>&& __lhs,
	      _CharT __rhs)
    { return std::move(__lhs.append(1, __rhs)); }

  
  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator==(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	       const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    { return __lhs.compare(__rhs) == 0; }

  template<typename _CharT>
    inline
    typename __gnu_cxx::__enable_if<__is_char<_CharT>::__value, bool>::__type
    operator==(const basic_string<_CharT>& __lhs,
	       const basic_string<_CharT>& __rhs)
    { return (__lhs.size() == __rhs.size()
	      && !std::char_traits<_CharT>::compare(__lhs.data(), __rhs.data(),
						    __lhs.size())); }

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator==(const _CharT* __lhs,
	       const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    { return __rhs.compare(__lhs) == 0; }

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator==(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	       const _CharT* __rhs)
    { return __lhs.compare(__rhs) == 0; }

  
  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator!=(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	       const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    { return !(__lhs == __rhs); }

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator!=(const _CharT* __lhs,
	       const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    { return !(__lhs == __rhs); }

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator!=(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	       const _CharT* __rhs)
    { return !(__lhs == __rhs); }

  
  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator<(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	      const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    { return __lhs.compare(__rhs) < 0; }

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator<(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	      const _CharT* __rhs)
    { return __lhs.compare(__rhs) < 0; }

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator<(const _CharT* __lhs,
	      const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    { return __rhs.compare(__lhs) > 0; }

  
  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator>(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	      const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    { return __lhs.compare(__rhs) > 0; }

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator>(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	      const _CharT* __rhs)
    { return __lhs.compare(__rhs) > 0; }

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator>(const _CharT* __lhs,
	      const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    { return __rhs.compare(__lhs) < 0; }

  
  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator<=(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	       const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    { return __lhs.compare(__rhs) <= 0; }

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator<=(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	       const _CharT* __rhs)
    { return __lhs.compare(__rhs) <= 0; }

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator<=(const _CharT* __lhs,
	       const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    { return __rhs.compare(__lhs) >= 0; }

  
  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator>=(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	       const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    { return __lhs.compare(__rhs) >= 0; }

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator>=(const basic_string<_CharT, _Traits, _Alloc>& __lhs,
	       const _CharT* __rhs)
    { return __lhs.compare(__rhs) >= 0; }

  




 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline bool
    operator>=(const _CharT* __lhs,
	     const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    { return __rhs.compare(__lhs) <= 0; }

  





 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline void
    swap(basic_string<_CharT, _Traits, _Alloc>& __lhs,
	 basic_string<_CharT, _Traits, _Alloc>& __rhs)
    { __lhs.swap(__rhs); }

  










 
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_istream<_CharT, _Traits>&
    operator>>(basic_istream<_CharT, _Traits>& __is,
	       basic_string<_CharT, _Traits, _Alloc>& __str);

  template<>
    basic_istream<char>&
    operator>>(basic_istream<char>& __is, basic_string<char>& __str);

  







 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline basic_ostream<_CharT, _Traits>&
    operator<<(basic_ostream<_CharT, _Traits>& __os,
	       const basic_string<_CharT, _Traits, _Alloc>& __str)
    {
      
      
      return __ostream_insert(__os, __str.data(), __str.size());
    }

  











 
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_istream<_CharT, _Traits>&
    getline(basic_istream<_CharT, _Traits>& __is,
	    basic_string<_CharT, _Traits, _Alloc>& __str, _CharT __delim);

  










 
  template<typename _CharT, typename _Traits, typename _Alloc>
    inline basic_istream<_CharT, _Traits>&
    getline(basic_istream<_CharT, _Traits>& __is,
	    basic_string<_CharT, _Traits, _Alloc>& __str)
    { return getline(__is, __str, __is.widen('\n')); }

  template<>
    basic_istream<char>&
    getline(basic_istream<char>& __in, basic_string<char>& __str,
	    char __delim);

  template<>
    basic_istream<wchar_t>&
    getline(basic_istream<wchar_t>& __in, basic_string<wchar_t>& __str,
	    wchar_t __delim);


} 




























 


#pragma GCC system_header


































 





#pragma GCC system_header



















 



 



 





















 



 



 


 


 
 




 


 






 





 

 


 

 


 





 


 



 
 
 


 




extern "C" {


 
















 



 

 

















 



 


 

 

 

 

 

 


 

 

 




union wait
  {
    int w_status;
    struct
      {
	unsigned int __w_termsig:7;  
	unsigned int __w_coredump:1;  
	unsigned int __w_retcode:8;  
	unsigned int:16;
      } __wait_terminated;
    struct
      {
	unsigned int __w_stopval:8;  
	unsigned int __w_stopsig:8;  
	unsigned int:16;
      } __wait_stopped;
  };





 





 



 


 
typedef struct
  {
    int quot;			 
    int rem;			 
  } div_t;

 
typedef struct
  {
    long int quot;		 
    long int rem;		 
  } ldiv_t;



 
__extension__ typedef struct
  {
    long long int quot;		 
    long long int rem;		 
  } lldiv_t;



 



 


 
extern size_t __ctype_get_mb_cur_max (void) throw () ;



 
extern double atof (const char *__nptr)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;
 
extern int atoi (const char *__nptr)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;
 
extern long int atol (const char *__nptr)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;



 
__extension__ extern long long int atoll (const char *__nptr)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;



 
extern double strtod (const char * __nptr,
		      char ** __endptr)
     throw () __attribute__ ((__nonnull__ (1)));



 
extern float strtof (const char * __nptr,
		     char ** __endptr) throw () __attribute__ ((__nonnull__ (1)));

extern long double strtold (const char * __nptr,
			    char ** __endptr)
     throw () __attribute__ ((__nonnull__ (1)));



 
extern long int strtol (const char * __nptr,
			char ** __endptr, int __base)
     throw () __attribute__ ((__nonnull__ (1)));
 
extern unsigned long int strtoul (const char * __nptr,
				  char ** __endptr, int __base)
     throw () __attribute__ ((__nonnull__ (1)));


 
__extension__
extern long long int strtoq (const char * __nptr,
			     char ** __endptr, int __base)
     throw () __attribute__ ((__nonnull__ (1)));
 
__extension__
extern unsigned long long int strtouq (const char * __nptr,
				       char ** __endptr, int __base)
     throw () __attribute__ ((__nonnull__ (1)));


 
__extension__
extern long long int strtoll (const char * __nptr,
			      char ** __endptr, int __base)
     throw () __attribute__ ((__nonnull__ (1)));
 
__extension__
extern unsigned long long int strtoull (const char * __nptr,
					char ** __endptr, int __base)
     throw () __attribute__ ((__nonnull__ (1)));












 


 


 
extern long int strtol_l (const char * __nptr,
			  char ** __endptr, int __base,
			  __locale_t __loc) throw () __attribute__ ((__nonnull__ (1, 4)));

extern unsigned long int strtoul_l (const char * __nptr,
				    char ** __endptr,
				    int __base, __locale_t __loc)
     throw () __attribute__ ((__nonnull__ (1, 4)));

__extension__
extern long long int strtoll_l (const char * __nptr,
				char ** __endptr, int __base,
				__locale_t __loc)
     throw () __attribute__ ((__nonnull__ (1, 4)));

__extension__
extern unsigned long long int strtoull_l (const char * __nptr,
					  char ** __endptr,
					  int __base, __locale_t __loc)
     throw () __attribute__ ((__nonnull__ (1, 4)));

extern double strtod_l (const char * __nptr,
			char ** __endptr, __locale_t __loc)
     throw () __attribute__ ((__nonnull__ (1, 3)));

extern float strtof_l (const char * __nptr,
		       char ** __endptr, __locale_t __loc)
     throw () __attribute__ ((__nonnull__ (1, 3)));

extern long double strtold_l (const char * __nptr,
			      char ** __endptr,
			      __locale_t __loc)
     throw () __attribute__ ((__nonnull__ (1, 3)));



extern inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__leaf__)) atoi (const char *__nptr) throw ()
{
  return (int) strtol (__nptr, (char **) __null, 10);
}
extern inline __attribute__ ((__gnu_inline__)) long int
__attribute__ ((__leaf__)) atol (const char *__nptr) throw ()
{
  return strtol (__nptr, (char **) __null, 10);
}



__extension__ extern inline __attribute__ ((__gnu_inline__)) long long int
__attribute__ ((__leaf__)) atoll (const char *__nptr) throw ()
{
  return strtoll (__nptr, (char **) __null, 10);
}





 
extern char *l64a (long int __n) throw () ;

 
extern long int a64l (const char *__s)
     throw () __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1))) ;

















 



 



extern "C" {


typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;

typedef __loff_t loff_t;

typedef __ino_t ino_t;
typedef __ino64_t ino64_t;

typedef __dev_t dev_t;

typedef __gid_t gid_t;

typedef __mode_t mode_t;

typedef __nlink_t nlink_t;

typedef __uid_t uid_t;

typedef __off_t off_t;
typedef __off64_t off64_t;


typedef __id_t id_t;

typedef __ssize_t ssize_t;

typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;

typedef __key_t key_t;


typedef __useconds_t useconds_t;
typedef __suseconds_t suseconds_t;






















 



 



 


 


 
 




 


 






 





 

 


 

 


 





 


 



 
 
 


 




 
typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;

 


 


typedef unsigned int u_int8_t __attribute__ ((__mode__ (__QI__)));
typedef unsigned int u_int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int u_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int u_int64_t __attribute__ ((__mode__ (__DI__)));

typedef int register_t __attribute__ ((__mode__ (__word__)));



 


 

 
















 

 



 

 















 


 


 







 

















 


typedef int __sig_atomic_t;

 

typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
  } __sigset_t;







 


typedef __sigset_t sigset_t;

 
















 



 






 
typedef long int __fd_mask;

 
 

 
typedef struct
  {
    
 
    __fd_mask fds_bits[1024 / (8 * (int) sizeof (__fd_mask))];
  } fd_set;

 

 
typedef __fd_mask fd_mask;

 


 


extern "C" {








 
extern int select (int __nfds, fd_set * __readfds,
		   fd_set * __writefds,
		   fd_set * __exceptfds,
		   struct timeval * __timeout);






 
extern int pselect (int __nfds, fd_set * __readfds,
		    fd_set * __writefds,
		    fd_set * __exceptfds,
		    const struct timespec * __timeout,
		    const __sigset_t * __sigmask);


 

}


 

















 





 
extern "C" {

__extension__
extern unsigned int gnu_dev_major (unsigned long long int __dev)
     throw () __attribute__ ((__const__));
__extension__
extern unsigned int gnu_dev_minor (unsigned long long int __dev)
     throw () __attribute__ ((__const__));
__extension__
extern unsigned long long int gnu_dev_makedev (unsigned int __major,
					       unsigned int __minor)
     throw () __attribute__ ((__const__));

__extension__ extern inline __attribute__ ((__gnu_inline__)) __attribute__ ((__const__)) unsigned int
__attribute__ ((__leaf__)) gnu_dev_major (unsigned long long int __dev) throw ()
{
  return ((__dev >> 8) & 0xfff) | ((unsigned int) (__dev >> 32) & ~0xfff);
}

__extension__ extern inline __attribute__ ((__gnu_inline__)) __attribute__ ((__const__)) unsigned int
__attribute__ ((__leaf__)) gnu_dev_minor (unsigned long long int __dev) throw ()
{
  return (__dev & 0xff) | ((unsigned int) (__dev >> 12) & ~0xff);
}

__extension__ extern inline __attribute__ ((__gnu_inline__)) __attribute__ ((__const__)) unsigned long long int
__attribute__ ((__leaf__)) gnu_dev_makedev (unsigned int __major, unsigned int __minor) throw ()
{
  return ((__minor & 0xff) | ((__major & 0xfff) << 8)
	  | (((unsigned long long int) (__minor & ~0xff)) << 12)
	  | (((unsigned long long int) (__major & ~0xfff)) << 32));
}
}

 



typedef __blksize_t blksize_t;

 
typedef __blkcnt_t blkcnt_t;	  
typedef __fsblkcnt_t fsblkcnt_t;  
typedef __fsfilcnt_t fsfilcnt_t;  

typedef __blkcnt64_t blkcnt64_t;      
typedef __fsblkcnt64_t fsblkcnt64_t;  
typedef __fsfilcnt64_t fsfilcnt64_t;  


 

}





 
 
extern long int random (void) throw ();

 
extern void srandom (unsigned int __seed) throw ();




 
extern char *initstate (unsigned int __seed, char *__statebuf,
			size_t __statelen) throw () __attribute__ ((__nonnull__ (2)));


 
extern char *setstate (char *__statebuf) throw () __attribute__ ((__nonnull__ (1)));




 

struct random_data
  {
    int32_t *fptr;		 
    int32_t *rptr;		 
    int32_t *state;		 
    int rand_type;		 
    int rand_deg;		 
    int rand_sep;		 
    int32_t *end_ptr;		 
  };

extern int random_r (struct random_data * __buf,
		     int32_t * __result) throw () __attribute__ ((__nonnull__ (1, 2)));

extern int srandom_r (unsigned int __seed, struct random_data *__buf)
     throw () __attribute__ ((__nonnull__ (2)));

extern int initstate_r (unsigned int __seed, char * __statebuf,
			size_t __statelen,
			struct random_data * __buf)
     throw () __attribute__ ((__nonnull__ (2, 4)));

extern int setstate_r (char * __statebuf,
		       struct random_data * __buf)
     throw () __attribute__ ((__nonnull__ (1, 2)));



 
extern int rand (void) throw ();
 
extern void srand (unsigned int __seed) throw ();


 
extern int rand_r (unsigned int *__seed) throw ();


 

 
extern double drand48 (void) throw ();
extern double erand48 (unsigned short int __xsubi[3]) throw () __attribute__ ((__nonnull__ (1)));

 
extern long int lrand48 (void) throw ();
extern long int nrand48 (unsigned short int __xsubi[3])
     throw () __attribute__ ((__nonnull__ (1)));

 
extern long int mrand48 (void) throw ();
extern long int jrand48 (unsigned short int __xsubi[3])
     throw () __attribute__ ((__nonnull__ (1)));

 
extern void srand48 (long int __seedval) throw ();
extern unsigned short int *seed48 (unsigned short int __seed16v[3])
     throw () __attribute__ ((__nonnull__ (1)));
extern void lcong48 (unsigned short int __param[7]) throw () __attribute__ ((__nonnull__ (1)));



 
struct drand48_data
  {
    unsigned short int __x[3];	 
    unsigned short int __old_x[3];  
    unsigned short int __c;	 
    unsigned short int __init;	 
    unsigned long long int __a;	 
  };

 
extern int drand48_r (struct drand48_data * __buffer,
		      double * __result) throw () __attribute__ ((__nonnull__ (1, 2)));
extern int erand48_r (unsigned short int __xsubi[3],
		      struct drand48_data * __buffer,
		      double * __result) throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int lrand48_r (struct drand48_data * __buffer,
		      long int * __result)
     throw () __attribute__ ((__nonnull__ (1, 2)));
extern int nrand48_r (unsigned short int __xsubi[3],
		      struct drand48_data * __buffer,
		      long int * __result)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int mrand48_r (struct drand48_data * __buffer,
		      long int * __result)
     throw () __attribute__ ((__nonnull__ (1, 2)));
extern int jrand48_r (unsigned short int __xsubi[3],
		      struct drand48_data * __buffer,
		      long int * __result)
     throw () __attribute__ ((__nonnull__ (1, 2)));

 
extern int srand48_r (long int __seedval, struct drand48_data *__buffer)
     throw () __attribute__ ((__nonnull__ (2)));

extern int seed48_r (unsigned short int __seed16v[3],
		     struct drand48_data *__buffer) throw () __attribute__ ((__nonnull__ (1, 2)));

extern int lcong48_r (unsigned short int __param[7],
		      struct drand48_data *__buffer)
     throw () __attribute__ ((__nonnull__ (1, 2)));



 
extern void *malloc (size_t __size) throw () __attribute__ ((__malloc__)) ;
 
extern void *calloc (size_t __nmemb, size_t __size)
     throw () __attribute__ ((__malloc__)) ;




 


 
extern void *realloc (void *__ptr, size_t __size)
     throw () __attribute__ ((__warn_unused_result__));
 
extern void free (void *__ptr) throw ();


 
extern void cfree (void *__ptr) throw ();
















 
























 



 



 


 


 
 




 


 






 





 

 


 

 


 





 


 



 
 
 


 




extern "C" {

 

 
extern void *alloca (size_t __size) throw ();


}


 
extern void *valloc (size_t __size) throw () __attribute__ ((__malloc__)) ;

 
extern int posix_memalign (void **__memptr, size_t __alignment, size_t __size)
     throw () __attribute__ ((__nonnull__ (1))) ;

 
extern void *aligned_alloc (size_t __alignment, size_t __size)
     throw ()  __attribute__ ((__malloc__, __alloc_size__ (2)));


 
extern void abort (void) throw () __attribute__ ((__noreturn__));


 
extern int atexit (void (*__func) (void)) throw () __attribute__ ((__nonnull__ (1)));

 
extern "C++" int at_quick_exit (void (*__func) (void))
     throw () __asm ("at_quick_exit") __attribute__ ((__nonnull__ (1)));



 
extern int on_exit (void (*__func) (int __status, void *__arg), void *__arg)
     throw () __attribute__ ((__nonnull__ (1)));




 
extern void exit (int __status) throw () __attribute__ ((__noreturn__));



 
extern void quick_exit (int __status) throw () __attribute__ ((__noreturn__));




 
extern void _Exit (int __status) throw () __attribute__ ((__noreturn__));




 
extern char *getenv (const char *__name) throw () __attribute__ ((__nonnull__ (1))) ;



 
extern char *secure_getenv (const char *__name)
     throw () __attribute__ ((__nonnull__ (1))) ;

 

 
extern int putenv (char *__string) throw () __attribute__ ((__nonnull__ (1)));


 
extern int setenv (const char *__name, const char *__value, int __replace)
     throw () __attribute__ ((__nonnull__ (2)));

 
extern int unsetenv (const char *__name) throw () __attribute__ ((__nonnull__ (1)));



 
extern int clearenv (void) throw ();






 
extern char *mktemp (char *__template) throw () __attribute__ ((__nonnull__ (1)));








 
extern int mkstemp (char *__template) __attribute__ ((__nonnull__ (1))) ;
extern int mkstemp64 (char *__template) __attribute__ ((__nonnull__ (1))) ;






 
extern int mkstemps (char *__template, int __suffixlen) __attribute__ ((__nonnull__ (1))) ;
extern int mkstemps64 (char *__template, int __suffixlen)
     __attribute__ ((__nonnull__ (1))) ;





 
extern char *mkdtemp (char *__template) throw () __attribute__ ((__nonnull__ (1))) ;






 
extern int mkostemp (char *__template, int __flags) __attribute__ ((__nonnull__ (1))) ;
extern int mkostemp64 (char *__template, int __flags) __attribute__ ((__nonnull__ (1))) ;






 
extern int mkostemps (char *__template, int __suffixlen, int __flags)
     __attribute__ ((__nonnull__ (1))) ;
extern int mkostemps64 (char *__template, int __suffixlen, int __flags)
     __attribute__ ((__nonnull__ (1))) ;






 
extern int system (const char *__command) ;




 
extern char *canonicalize_file_name (const char *__name)
     throw () __attribute__ ((__nonnull__ (1))) ;





 
extern char *realpath (const char * __name,
		       char * __resolved) throw () ;


 
typedef int (*__compar_fn_t) (const void *, const void *);

typedef __compar_fn_t comparison_fn_t;
typedef int (*__compar_d_fn_t) (const void *, const void *, void *);



 
extern void *bsearch (const void *__key, const void *__base,
		      size_t __nmemb, size_t __size, __compar_fn_t __compar)
     __attribute__ ((__nonnull__ (1, 2, 5))) ;


 
extern void qsort (void *__base, size_t __nmemb, size_t __size,
		   __compar_fn_t __compar) __attribute__ ((__nonnull__ (1, 4)));
extern void qsort_r (void *__base, size_t __nmemb, size_t __size,
		     __compar_d_fn_t __compar, void *__arg)
  __attribute__ ((__nonnull__ (1, 4)));


 
extern int abs (int __x) throw () __attribute__ ((__const__)) ;
extern long int labs (long int __x) throw () __attribute__ ((__const__)) ;


__extension__ extern long long int llabs (long long int __x)
     throw () __attribute__ ((__const__)) ;




 
 
extern div_t div (int __numer, int __denom)
     throw () __attribute__ ((__const__)) ;
extern ldiv_t ldiv (long int __numer, long int __denom)
     throw () __attribute__ ((__const__)) ;



__extension__ extern lldiv_t lldiv (long long int __numer,
				    long long int __denom)
     throw () __attribute__ ((__const__)) ;




 



 
extern char *ecvt (double __value, int __ndigit, int * __decpt,
		   int * __sign) throw () __attribute__ ((__nonnull__ (3, 4))) ;



 
extern char *fcvt (double __value, int __ndigit, int * __decpt,
		   int * __sign) throw () __attribute__ ((__nonnull__ (3, 4))) ;



 
extern char *gcvt (double __value, int __ndigit, char *__buf)
     throw () __attribute__ ((__nonnull__ (3))) ;


 
extern char *qecvt (long double __value, int __ndigit,
		    int * __decpt, int * __sign)
     throw () __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qfcvt (long double __value, int __ndigit,
		    int * __decpt, int * __sign)
     throw () __attribute__ ((__nonnull__ (3, 4))) ;
extern char *qgcvt (long double __value, int __ndigit, char *__buf)
     throw () __attribute__ ((__nonnull__ (3))) ;



 
extern int ecvt_r (double __value, int __ndigit, int * __decpt,
		   int * __sign, char * __buf,
		   size_t __len) throw () __attribute__ ((__nonnull__ (3, 4, 5)));
extern int fcvt_r (double __value, int __ndigit, int * __decpt,
		   int * __sign, char * __buf,
		   size_t __len) throw () __attribute__ ((__nonnull__ (3, 4, 5)));

extern int qecvt_r (long double __value, int __ndigit,
		    int * __decpt, int * __sign,
		    char * __buf, size_t __len)
     throw () __attribute__ ((__nonnull__ (3, 4, 5)));
extern int qfcvt_r (long double __value, int __ndigit,
		    int * __decpt, int * __sign,
		    char * __buf, size_t __len)
     throw () __attribute__ ((__nonnull__ (3, 4, 5)));




 
extern int mblen (const char *__s, size_t __n) throw () ;

 
extern int mbtowc (wchar_t * __pwc,
		   const char * __s, size_t __n) throw () ;

 
extern int wctomb (char *__s, wchar_t __wchar) throw () ;


 
extern size_t mbstowcs (wchar_t *  __pwcs,
			const char * __s, size_t __n) throw ();
 
extern size_t wcstombs (char * __s,
			const wchar_t * __pwcs, size_t __n)
     throw ();






 
extern int rpmatch (const char *__response) throw () __attribute__ ((__nonnull__ (1))) ;







 
extern int getsubopt (char ** __optionp,
		      char *const * __tokens,
		      char ** __valuep)
     throw () __attribute__ ((__nonnull__ (1, 2, 3))) ;


 
extern void setkey (const char *__key) throw () __attribute__ ((__nonnull__ (1)));


 

 
extern int posix_openpt (int __oflag) ;


 

 
extern int grantpt (int __fd) throw ();


 
extern int unlockpt (int __fd) throw ();



 
extern char *ptsname (int __fd) throw () ;



 
extern int ptsname_r (int __fd, char *__buf, size_t __buflen)
     throw () __attribute__ ((__nonnull__ (2)));

 
extern int getpt (void);



 
extern int getloadavg (double __loadavg[], int __nelem)
     throw () __attribute__ ((__nonnull__ (1)));

















 



extern inline __attribute__ ((__gnu_inline__)) double
__attribute__ ((__leaf__)) atof (const char *__nptr) throw ()
{
  return strtod (__nptr, (char **) __null);
}


 


}




namespace std 
{


  using ::div_t;
  using ::ldiv_t;

  using ::abort;
  using ::abs;
  using ::atexit;
  using ::at_quick_exit;
  using ::atof;
  using ::atoi;
  using ::atol;
  using ::bsearch;
  using ::calloc;
  using ::div;
  using ::exit;
  using ::free;
  using ::getenv;
  using ::labs;
  using ::ldiv;
  using ::malloc;
  using ::mblen;
  using ::mbstowcs;
  using ::mbtowc;
  using ::qsort;
  using ::quick_exit;
  using ::rand;
  using ::realloc;
  using ::srand;
  using ::strtod;
  using ::strtol;
  using ::strtoul;
  using ::system;
  using ::wcstombs;
  using ::wctomb;

  inline long
  abs(long __i) { return __builtin_labs(__i); }

  inline ldiv_t
  div(long __i, long __j) { return ldiv(__i, __j); }

  inline long long
  abs(long long __x) { return __builtin_llabs (__x); }

  inline __int128
  abs(__int128 __x) { return __x >= 0 ? __x : -__x; }


} 



namespace __gnu_cxx 
{


  using ::lldiv_t;
  using ::_Exit;

  using ::llabs;

  inline lldiv_t
  div(long long __n, long long __d)
  { lldiv_t __q; __q.quot = __n / __d; __q.rem = __n % __d; return __q; }

  using ::lldiv;

  using ::atoll;
  using ::strtoll;
  using ::strtoull;
  using ::strtof;
  using ::strtold;


} 

namespace std
{
  using ::__gnu_cxx::lldiv_t;
  using ::__gnu_cxx::_Exit;
  using ::__gnu_cxx::llabs;
  using ::__gnu_cxx::div;
  using ::__gnu_cxx::lldiv;
  using ::__gnu_cxx::atoll;
  using ::__gnu_cxx::strtof;
  using ::__gnu_cxx::strtoll;
  using ::__gnu_cxx::strtoull;
  using ::__gnu_cxx::strtold;
} 



































 





#pragma GCC system_header

















 




 



 

































 





#pragma GCC system_header

















 



 



extern "C" {






















 



 



 


 


 
 




 


 






 





 

 


 

 


 





 


 



 
 
 


 





































 



 


 






















 



 



 


 


 
 




 


 






 





 

 


 

 


 





 


 



 
 
 


 


















 




 



 
typedef struct
{
  __off_t __pos;
  __mbstate_t __state;
} _G_fpos_t;
typedef struct
{
  __off64_t __pos;
  __mbstate_t __state;
} _G_fpos64_t;


 



 


 

 





















 



 


 



 










 



 


struct _IO_jump_t;  struct _IO_FILE;

 
typedef void _IO_lock_t;


 

struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;
  
 
   
  int _pos;
};

 
enum __codecvt_result
{
  __codecvt_ok,
  __codecvt_partial,
  __codecvt_error,
  __codecvt_noconv
};


struct _IO_FILE {
  int _flags;		 

   
   
  char* _IO_read_ptr;	 
  char* _IO_read_end;	 
  char* _IO_read_base;	 
  char* _IO_write_base;	 
  char* _IO_write_ptr;	 
  char* _IO_write_end;	 
  char* _IO_buf_base;	 
  char* _IO_buf_end;	 
   
  char *_IO_save_base;  
  char *_IO_backup_base;   
  char *_IO_save_end;  

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;
  int _flags2;
  __off_t _old_offset;  

   
  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];

   

  _IO_lock_t *_lock;
  __off64_t _offset;
  void *__pad1;
  void *__pad2;
  void *__pad3;
  void *__pad4;
  size_t __pad5;
  int _mode;
   
  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];
};


struct _IO_FILE_plus;

extern struct _IO_FILE_plus _IO_2_1_stdin_;
extern struct _IO_FILE_plus _IO_2_1_stdout_;
extern struct _IO_FILE_plus _IO_2_1_stderr_;


 


 
typedef __ssize_t __io_read_fn (void *__cookie, char *__buf, size_t __nbytes);






 
typedef __ssize_t __io_write_fn (void *__cookie, const char *__buf,
				 size_t __n);






 
typedef int __io_seek_fn (void *__cookie, __off64_t *__pos, int __w);

 
typedef int __io_close_fn (void *__cookie);


 
typedef __io_read_fn cookie_read_function_t;
typedef __io_write_fn cookie_write_function_t;
typedef __io_seek_fn cookie_seek_function_t;
typedef __io_close_fn cookie_close_function_t;

 
typedef struct
{
  __io_read_fn *read;		 
  __io_write_fn *write;		 
  __io_seek_fn *seek;		 
  __io_close_fn *close;		 
} _IO_cookie_io_functions_t;
typedef _IO_cookie_io_functions_t cookie_io_functions_t;

struct _IO_cookie_file;

 
extern void _IO_cookie_init (struct _IO_cookie_file *__cfile, int __read_write,
			     void *__cookie, _IO_cookie_io_functions_t __fns);


extern "C" {

extern int __underflow (_IO_FILE *);
extern int __uflow (_IO_FILE *);
extern int __overflow (_IO_FILE *, int);





extern int _IO_getc (_IO_FILE *__fp);
extern int _IO_putc (int __c, _IO_FILE *__fp);
extern int _IO_feof (_IO_FILE *__fp) throw ();
extern int _IO_ferror (_IO_FILE *__fp) throw ();

extern int _IO_peekc_locked (_IO_FILE *__fp);

 

extern void _IO_flockfile (_IO_FILE *) throw ();
extern void _IO_funlockfile (_IO_FILE *) throw ();
extern int _IO_ftrylockfile (_IO_FILE *) throw ();


extern int _IO_vfscanf (_IO_FILE * , const char * ,
			__gnuc_va_list, int *);
extern int _IO_vfprintf (_IO_FILE *, const char *,
			 __gnuc_va_list);
extern __ssize_t _IO_padn (_IO_FILE *, int, __ssize_t);
extern size_t _IO_sgetn (_IO_FILE *, void *, size_t);

extern __off64_t _IO_seekoff (_IO_FILE *, __off64_t, int, int);
extern __off64_t _IO_seekpos (_IO_FILE *, __off64_t, int);

extern void _IO_free_backup_area (_IO_FILE *) throw ();



}


typedef __gnuc_va_list va_list;



 

typedef _G_fpos_t fpos_t;

typedef _G_fpos64_t fpos64_t;

 


 



 



 


 










 















 







 
extern struct _IO_FILE *stdin;		 
extern struct _IO_FILE *stdout;		 
extern struct _IO_FILE *stderr;		 
 


 
extern int remove (const char *__filename) throw ();
 
extern int rename (const char *__old, const char *__new) throw ();


 
extern int renameat (int __oldfd, const char *__old, int __newfd,
		     const char *__new) throw ();





 
extern FILE *tmpfile (void) ;

extern FILE *tmpfile64 (void) ;

 
extern char *tmpnam (char *__s) throw () ;



 
extern char *tmpnam_r (char *__s) throw () ;








 
extern char *tempnam (const char *__dir, const char *__pfx)
     throw () __attribute__ ((__malloc__)) ;






 
extern int fclose (FILE *__stream);



 
extern int fflush (FILE *__stream);







 
extern int fflush_unlocked (FILE *__stream);






 
extern int fcloseall (void);






 
extern FILE *fopen (const char * __filename,
		    const char * __modes) ;



 
extern FILE *freopen (const char * __filename,
		      const char * __modes,
		      FILE * __stream) ;

extern FILE *fopen64 (const char * __filename,
		      const char * __modes) ;
extern FILE *freopen64 (const char * __filename,
			const char * __modes,
			FILE * __stream) ;

 
extern FILE *fdopen (int __fd, const char *__modes) throw () ;


 
extern FILE *fopencookie (void * __magic_cookie,
			  const char * __modes,
			  _IO_cookie_io_functions_t __io_funcs) throw () ;

 
extern FILE *fmemopen (void *__s, size_t __len, const char *__modes)
  throw () ;



 
extern FILE *open_memstream (char **__bufloc, size_t *__sizeloc) throw () ;




 
extern void setbuf (FILE * __stream, char * __buf) throw ();


 
extern int setvbuf (FILE * __stream, char * __buf,
		    int __modes, size_t __n) throw ();



 
extern void setbuffer (FILE * __stream, char * __buf,
		       size_t __size) throw ();

 
extern void setlinebuf (FILE *__stream) throw ();






 
extern int fprintf (FILE * __stream,
		    const char * __format, ...);



 
extern int printf (const char * __format, ...);
 
extern int sprintf (char * __s,
		    const char * __format, ...) throw ();




 
extern int vfprintf (FILE * __s, const char * __format,
		     __gnuc_va_list __arg);



 
extern int vprintf (const char * __format, __gnuc_va_list __arg);
 
extern int vsprintf (char * __s, const char * __format,
		     __gnuc_va_list __arg) throw ();



 
extern int snprintf (char * __s, size_t __maxlen,
		     const char * __format, ...)
     throw () __attribute__ ((__format__ (__printf__, 3, 4)));

extern int vsnprintf (char * __s, size_t __maxlen,
		      const char * __format, __gnuc_va_list __arg)
     throw () __attribute__ ((__format__ (__printf__, 3, 0)));



 
extern int vasprintf (char ** __ptr, const char * __f,
		      __gnuc_va_list __arg)
     throw () __attribute__ ((__format__ (__printf__, 2, 0))) ;
extern int __asprintf (char ** __ptr,
		       const char * __fmt, ...)
     throw () __attribute__ ((__format__ (__printf__, 2, 3))) ;
extern int asprintf (char ** __ptr,
		     const char * __fmt, ...)
     throw () __attribute__ ((__format__ (__printf__, 2, 3))) ;

 
extern int vdprintf (int __fd, const char * __fmt,
		     __gnuc_va_list __arg)
     __attribute__ ((__format__ (__printf__, 2, 0)));
extern int dprintf (int __fd, const char * __fmt, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));






 
extern int fscanf (FILE * __stream,
		   const char * __format, ...) ;



 
extern int scanf (const char * __format, ...) ;
 
extern int sscanf (const char * __s,
		   const char * __format, ...) throw ();








 
extern int vfscanf (FILE * __s, const char * __format,
		    __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 2, 0))) ;




 
extern int vscanf (const char * __format, __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 1, 0))) ;

 
extern int vsscanf (const char * __s,
		    const char * __format, __gnuc_va_list __arg)
     throw () __attribute__ ((__format__ (__scanf__, 2, 0)));









 
extern int fgetc (FILE *__stream);
extern int getc (FILE *__stream);




 
extern int getchar (void);



 




 
extern int getc_unlocked (FILE *__stream);
extern int getchar_unlocked (void);






 
extern int fgetc_unlocked (FILE *__stream);









 
extern int fputc (int __c, FILE *__stream);
extern int putc (int __c, FILE *__stream);




 
extern int putchar (int __c);



 






 
extern int fputc_unlocked (int __c, FILE *__stream);




 
extern int putc_unlocked (int __c, FILE *__stream);
extern int putchar_unlocked (int __c);


 
extern int getw (FILE *__stream);

 
extern int putw (int __w, FILE *__stream);






 
extern char *fgets (char * __s, int __n, FILE * __stream)
     ;











 
extern char *gets (char *__s)  __attribute__ ((__deprecated__));







 
extern char *fgets_unlocked (char * __s, int __n,
			     FILE * __stream) ;











 
extern __ssize_t __getdelim (char ** __lineptr,
			       size_t * __n, int __delimiter,
			       FILE * __stream) ;
extern __ssize_t getdelim (char ** __lineptr,
			     size_t * __n, int __delimiter,
			     FILE * __stream) ;






 
extern __ssize_t getline (char ** __lineptr,
			    size_t * __n,
			    FILE * __stream) ;






 
extern int fputs (const char * __s, FILE * __stream);




 
extern int puts (const char *__s);





 
extern int ungetc (int __c, FILE *__stream);





 
extern size_t fread (void * __ptr, size_t __size,
		     size_t __n, FILE * __stream) ;



 
extern size_t fwrite (const void * __ptr, size_t __size,
		      size_t __n, FILE * __s);







 
extern int fputs_unlocked (const char * __s,
			   FILE * __stream);






 
extern size_t fread_unlocked (void * __ptr, size_t __size,
			      size_t __n, FILE * __stream) ;
extern size_t fwrite_unlocked (const void * __ptr, size_t __size,
			       size_t __n, FILE * __stream);






 
extern int fseek (FILE *__stream, long int __off, int __whence);



 
extern long int ftell (FILE *__stream) ;



 
extern void rewind (FILE *__stream);





 




 
extern int fseeko (FILE *__stream, __off_t __off, int __whence);



 
extern __off_t ftello (FILE *__stream) ;





 
extern int fgetpos (FILE * __stream, fpos_t * __pos);



 
extern int fsetpos (FILE *__stream, const fpos_t *__pos);


extern int fseeko64 (FILE *__stream, __off64_t __off, int __whence);
extern __off64_t ftello64 (FILE *__stream) ;
extern int fgetpos64 (FILE * __stream, fpos64_t * __pos);
extern int fsetpos64 (FILE *__stream, const fpos64_t *__pos);


 
extern void clearerr (FILE *__stream) throw ();
 
extern int feof (FILE *__stream) throw () ;
 
extern int ferror (FILE *__stream) throw () ;


 
extern void clearerr_unlocked (FILE *__stream) throw ();
extern int feof_unlocked (FILE *__stream) throw () ;
extern int ferror_unlocked (FILE *__stream) throw () ;






 
extern void perror (const char *__s);





 
















 


 

extern int sys_nerr;
extern const char *const sys_errlist[];
extern int _sys_nerr;
extern const char *const _sys_errlist[];


 
extern int fileno (FILE *__stream) throw () ;

 
extern int fileno_unlocked (FILE *__stream) throw () ;





 
extern FILE *popen (const char *__command, const char *__modes) ;




 
extern int pclose (FILE *__stream);


 
extern char *ctermid (char *__s) throw ();


 
extern char *cuserid (char *__s);


struct obstack;			 

 
extern int obstack_printf (struct obstack * __obstack,
			   const char * __format, ...)
     throw () __attribute__ ((__format__ (__printf__, 2, 3)));
extern int obstack_vprintf (struct obstack * __obstack,
			    const char * __format,
			    __gnuc_va_list __args)
     throw () __attribute__ ((__format__ (__printf__, 2, 0)));


 

 
extern void flockfile (FILE *__stream) throw ();


 
extern int ftrylockfile (FILE *__stream) throw () ;

 
extern void funlockfile (FILE *__stream) throw ();



 
















 





 
 
extern inline __attribute__ ((__gnu_inline__)) int
vprintf (const char * __fmt, __gnuc_va_list __arg)
{
  return vfprintf (stdout, __fmt, __arg);
}

 
extern inline __attribute__ ((__gnu_inline__)) int
getchar (void)
{
  return _IO_getc (stdin);
}


 
extern inline __attribute__ ((__gnu_inline__)) int
fgetc_unlocked (FILE *__fp)
{
  return (__builtin_expect (((__fp)->_IO_read_ptr >= (__fp)->_IO_read_end), 0) ? __uflow (__fp) : *(unsigned char *) (__fp)->_IO_read_ptr++);
}


 
extern inline __attribute__ ((__gnu_inline__)) int
getc_unlocked (FILE *__fp)
{
  return (__builtin_expect (((__fp)->_IO_read_ptr >= (__fp)->_IO_read_end), 0) ? __uflow (__fp) : *(unsigned char *) (__fp)->_IO_read_ptr++);
}

 
extern inline __attribute__ ((__gnu_inline__)) int
getchar_unlocked (void)
{
  return (__builtin_expect (((stdin)->_IO_read_ptr >= (stdin)->_IO_read_end), 0) ? __uflow (stdin) : *(unsigned char *) (stdin)->_IO_read_ptr++);
}


 
extern inline __attribute__ ((__gnu_inline__)) int
putchar (int __c)
{
  return _IO_putc (__c, stdout);
}


 
extern inline __attribute__ ((__gnu_inline__)) int
fputc_unlocked (int __c, FILE *__stream)
{
  return (__builtin_expect (((__stream)->_IO_write_ptr >= (__stream)->_IO_write_end), 0) ? __overflow (__stream, (unsigned char) (__c)) : (unsigned char) (*(__stream)->_IO_write_ptr++ = (__c)));
}


 
extern inline __attribute__ ((__gnu_inline__)) int
putc_unlocked (int __c, FILE *__stream)
{
  return (__builtin_expect (((__stream)->_IO_write_ptr >= (__stream)->_IO_write_end), 0) ? __overflow (__stream, (unsigned char) (__c)) : (unsigned char) (*(__stream)->_IO_write_ptr++ = (__c)));
}

 
extern inline __attribute__ ((__gnu_inline__)) int
putchar_unlocked (int __c)
{
  return (__builtin_expect (((stdout)->_IO_write_ptr >= (stdout)->_IO_write_end), 0) ? __overflow (stdout, (unsigned char) (__c)) : (unsigned char) (*(stdout)->_IO_write_ptr++ = (__c)));
}


 
extern inline __attribute__ ((__gnu_inline__)) __ssize_t
getline (char **__lineptr, size_t *__n, FILE *__stream)
{
  return __getdelim (__lineptr, __n, '\n', __stream);
}


 
extern inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__leaf__)) feof_unlocked (FILE *__stream) throw ()
{
  return (((__stream)->_flags & 0x10) != 0);
}

 
extern inline __attribute__ ((__gnu_inline__)) int
__attribute__ ((__leaf__)) ferror_unlocked (FILE *__stream) throw ()
{
  return (((__stream)->_flags & 0x20) != 0);
}




 

}







namespace std
{
  using ::FILE;
  using ::fpos_t;

  using ::clearerr;
  using ::fclose;
  using ::feof;
  using ::ferror;
  using ::fflush;
  using ::fgetc;
  using ::fgetpos;
  using ::fgets;
  using ::fopen;
  using ::fprintf;
  using ::fputc;
  using ::fputs;
  using ::fread;
  using ::freopen;
  using ::fscanf;
  using ::fseek;
  using ::fsetpos;
  using ::ftell;
  using ::fwrite;
  using ::getc;
  using ::getchar;
  using ::gets;
  using ::perror;
  using ::printf;
  using ::putc;
  using ::putchar;
  using ::puts;
  using ::remove;
  using ::rename;
  using ::rewind;
  using ::scanf;
  using ::setbuf;
  using ::setvbuf;
  using ::sprintf;
  using ::sscanf;
  using ::tmpfile;
  using ::tmpnam;
  using ::ungetc;
  using ::vfprintf;
  using ::vprintf;
  using ::vsprintf;
} 



namespace __gnu_cxx
{

  using ::snprintf;
  using ::vfscanf;
  using ::vscanf;
  using ::vsnprintf;
  using ::vsscanf;
} 

namespace std
{
  using ::__gnu_cxx::snprintf;
  using ::__gnu_cxx::vfscanf;
  using ::__gnu_cxx::vscanf;
  using ::__gnu_cxx::vsnprintf;
  using ::__gnu_cxx::vsscanf;
} 


































 





#pragma GCC system_header
















 



 



 

extern "C" {


 
















 










 




 

 

 



 
extern int *__errno_location (void) throw () __attribute__ ((__const__));

 







 




 
extern char *program_invocation_name, *program_invocation_short_name;

}





 
typedef int error_t;





namespace __gnu_cxx 
{


  
  template<typename _TRet, typename _Ret = _TRet, typename _CharT,
	   typename... _Base>
    _Ret
    __stoa(_TRet (*__convf) (const _CharT*, _CharT**, _Base...),
	   const char* __name, const _CharT* __str, std::size_t* __idx,
	   _Base... __base)
    {
      _Ret __ret;

      _CharT* __endptr;
      (*__errno_location ()) = 0;
      const _TRet __tmp = __convf(__str, &__endptr, __base...);

      if (__endptr == __str)
	std::__throw_invalid_argument(__name);
      else if ((*__errno_location ()) == 34
	       || (std::__are_same<_Ret, int>::__value
		   && (__tmp < __numeric_traits<int>::__min
		       || __tmp > __numeric_traits<int>::__max)))
	std::__throw_out_of_range(__name);
      else
	__ret = __tmp;

      if (__idx)
	*__idx = __endptr - __str;

      return __ret;
    }

  
  template<typename _String, typename _CharT = typename _String::value_type>
    _String
    __to_xstring(int (*__convf) (_CharT*, std::size_t, const _CharT*,
				 __builtin_va_list), std::size_t __n,
		 const _CharT* __fmt, ...)
    {
      
      
      _CharT* __s = static_cast<_CharT*>(__builtin_alloca(sizeof(_CharT)
							  * __n));

      __builtin_va_list __args;
      __builtin_va_start(__args, __fmt);

      const int __len = __convf(__s, __n, __fmt, __args);

      __builtin_va_end(__args);

      return _String(__s, __s + __len);
    }


} 



namespace std 
{


  
  inline int
  stoi(const string& __str, size_t* __idx = 0, int __base = 10)
  { return __gnu_cxx::__stoa<long, int>(&std::strtol, "stoi", __str.c_str(),
					__idx, __base); }

  inline long
  stol(const string& __str, size_t* __idx = 0, int __base = 10)
  { return __gnu_cxx::__stoa(&std::strtol, "stol", __str.c_str(),
			     __idx, __base); }

  inline unsigned long
  stoul(const string& __str, size_t* __idx = 0, int __base = 10)
  { return __gnu_cxx::__stoa(&std::strtoul, "stoul", __str.c_str(),
			     __idx, __base); }

  inline long long
  stoll(const string& __str, size_t* __idx = 0, int __base = 10)
  { return __gnu_cxx::__stoa(&std::strtoll, "stoll", __str.c_str(),
			     __idx, __base); }

  inline unsigned long long
  stoull(const string& __str, size_t* __idx = 0, int __base = 10)
  { return __gnu_cxx::__stoa(&std::strtoull, "stoull", __str.c_str(),
			     __idx, __base); }

  
  inline float
  stof(const string& __str, size_t* __idx = 0)
  { return __gnu_cxx::__stoa(&std::strtof, "stof", __str.c_str(), __idx); }

  inline double
  stod(const string& __str, size_t* __idx = 0)
  { return __gnu_cxx::__stoa(&std::strtod, "stod", __str.c_str(), __idx); }

  inline long double
  stold(const string& __str, size_t* __idx = 0)
  { return __gnu_cxx::__stoa(&std::strtold, "stold", __str.c_str(), __idx); }

  

  
  inline string
  to_string(int __val)
  { return __gnu_cxx::__to_xstring<string>(&std::vsnprintf, 4 * sizeof(int),
					   "%d", __val); }

  inline string
  to_string(unsigned __val)
  { return __gnu_cxx::__to_xstring<string>(&std::vsnprintf,
					   4 * sizeof(unsigned),
					   "%u", __val); }

  inline string
  to_string(long __val)
  { return __gnu_cxx::__to_xstring<string>(&std::vsnprintf, 4 * sizeof(long),
					   "%ld", __val); }

  inline string
  to_string(unsigned long __val)
  { return __gnu_cxx::__to_xstring<string>(&std::vsnprintf,
					   4 * sizeof(unsigned long),
					   "%lu", __val); }

  inline string
  to_string(long long __val)
  { return __gnu_cxx::__to_xstring<string>(&std::vsnprintf,
					   4 * sizeof(long long),
					   "%lld", __val); }

  inline string
  to_string(unsigned long long __val)
  { return __gnu_cxx::__to_xstring<string>(&std::vsnprintf,
					   4 * sizeof(unsigned long long),
					   "%llu", __val); }

  inline string
  to_string(float __val)
  {
    const int __n = 
      __gnu_cxx::__numeric_traits<float>::__max_exponent10 + 20;
    return __gnu_cxx::__to_xstring<string>(&std::vsnprintf, __n,
					   "%f", __val);
  }

  inline string
  to_string(double __val)
  {
    const int __n = 
      __gnu_cxx::__numeric_traits<double>::__max_exponent10 + 20;
    return __gnu_cxx::__to_xstring<string>(&std::vsnprintf, __n,
					   "%f", __val);
  }

  inline string
  to_string(long double __val)
  {
    const int __n = 
      __gnu_cxx::__numeric_traits<long double>::__max_exponent10 + 20;
    return __gnu_cxx::__to_xstring<string>(&std::vsnprintf, __n,
					   "%Lf", __val);
  }

  inline int 
  stoi(const wstring& __str, size_t* __idx = 0, int __base = 10)
  { return __gnu_cxx::__stoa<long, int>(&std::wcstol, "stoi", __str.c_str(),
					__idx, __base); }

  inline long 
  stol(const wstring& __str, size_t* __idx = 0, int __base = 10)
  { return __gnu_cxx::__stoa(&std::wcstol, "stol", __str.c_str(),
			     __idx, __base); }

  inline unsigned long
  stoul(const wstring& __str, size_t* __idx = 0, int __base = 10)
  { return __gnu_cxx::__stoa(&std::wcstoul, "stoul", __str.c_str(),
			     __idx, __base); }

  inline long long
  stoll(const wstring& __str, size_t* __idx = 0, int __base = 10)
  { return __gnu_cxx::__stoa(&std::wcstoll, "stoll", __str.c_str(),
			     __idx, __base); }

  inline unsigned long long
  stoull(const wstring& __str, size_t* __idx = 0, int __base = 10)
  { return __gnu_cxx::__stoa(&std::wcstoull, "stoull", __str.c_str(),
			     __idx, __base); }

  
  inline float
  stof(const wstring& __str, size_t* __idx = 0)
  { return __gnu_cxx::__stoa(&std::wcstof, "stof", __str.c_str(), __idx); }

  inline double
  stod(const wstring& __str, size_t* __idx = 0)
  { return __gnu_cxx::__stoa(&std::wcstod, "stod", __str.c_str(), __idx); }

  inline long double
  stold(const wstring& __str, size_t* __idx = 0)
  { return __gnu_cxx::__stoa(&std::wcstold, "stold", __str.c_str(), __idx); }

  
  inline wstring
  to_wstring(int __val)
  { return __gnu_cxx::__to_xstring<wstring>(&std::vswprintf, 4 * sizeof(int),
					    L"%d", __val); }

  inline wstring
  to_wstring(unsigned __val)
  { return __gnu_cxx::__to_xstring<wstring>(&std::vswprintf,
					    4 * sizeof(unsigned),
					    L"%u", __val); }

  inline wstring
  to_wstring(long __val)
  { return __gnu_cxx::__to_xstring<wstring>(&std::vswprintf, 4 * sizeof(long),
					    L"%ld", __val); }

  inline wstring
  to_wstring(unsigned long __val)
  { return __gnu_cxx::__to_xstring<wstring>(&std::vswprintf,
					    4 * sizeof(unsigned long),
					    L"%lu", __val); }

  inline wstring
  to_wstring(long long __val)
  { return __gnu_cxx::__to_xstring<wstring>(&std::vswprintf,
					    4 * sizeof(long long),
					    L"%lld", __val); }

  inline wstring
  to_wstring(unsigned long long __val)
  { return __gnu_cxx::__to_xstring<wstring>(&std::vswprintf,
					    4 * sizeof(unsigned long long),
					    L"%llu", __val); }

  inline wstring
  to_wstring(float __val)
  {
    const int __n =
      __gnu_cxx::__numeric_traits<float>::__max_exponent10 + 20;
    return __gnu_cxx::__to_xstring<wstring>(&std::vswprintf, __n,
					    L"%f", __val);
  }

  inline wstring
  to_wstring(double __val)
  {
    const int __n =
      __gnu_cxx::__numeric_traits<double>::__max_exponent10 + 20;
    return __gnu_cxx::__to_xstring<wstring>(&std::vswprintf, __n,
					    L"%f", __val);
  }

  inline wstring
  to_wstring(long double __val)
  {
    const int __n =
      __gnu_cxx::__numeric_traits<long double>::__max_exponent10 + 20;
    return __gnu_cxx::__to_xstring<wstring>(&std::vswprintf, __n,
					    L"%Lf", __val);
  }


} 






























 


#pragma GCC system_header


namespace std 
{


  





 

  template<typename _Result, typename _Arg>
    struct __hash_base
    {
      typedef _Result     result_type;
      typedef _Arg      argument_type;
    };

  
  template<typename _Tp>
    struct hash;

  
  template<typename _Tp>
    struct hash<_Tp*> : public __hash_base<size_t, _Tp*>
    {
      size_t
      operator()(_Tp* __p) const noexcept
      { return reinterpret_cast<size_t>(__p); }
    };

  

  
  template< > struct hash<bool> : public __hash_base<size_t, bool> { size_t operator()(bool __val) const noexcept { return static_cast<size_t>(__val); } };

  
  template< > struct hash<char> : public __hash_base<size_t, char> { size_t operator()(char __val) const noexcept { return static_cast<size_t>(__val); } };

  
  template< > struct hash<signed char> : public __hash_base<size_t, signed char> { size_t operator()(signed char __val) const noexcept { return static_cast<size_t>(__val); } };

  
  template< > struct hash<unsigned char> : public __hash_base<size_t, unsigned char> { size_t operator()(unsigned char __val) const noexcept { return static_cast<size_t>(__val); } };

  
  template< > struct hash<wchar_t> : public __hash_base<size_t, wchar_t> { size_t operator()(wchar_t __val) const noexcept { return static_cast<size_t>(__val); } };

  
  template< > struct hash<char16_t> : public __hash_base<size_t, char16_t> { size_t operator()(char16_t __val) const noexcept { return static_cast<size_t>(__val); } };

  
  template< > struct hash<char32_t> : public __hash_base<size_t, char32_t> { size_t operator()(char32_t __val) const noexcept { return static_cast<size_t>(__val); } };

  
  template< > struct hash<short> : public __hash_base<size_t, short> { size_t operator()(short __val) const noexcept { return static_cast<size_t>(__val); } };

  
  template< > struct hash<int> : public __hash_base<size_t, int> { size_t operator()(int __val) const noexcept { return static_cast<size_t>(__val); } };

  
  template< > struct hash<long> : public __hash_base<size_t, long> { size_t operator()(long __val) const noexcept { return static_cast<size_t>(__val); } };

  
  template< > struct hash<long long> : public __hash_base<size_t, long long> { size_t operator()(long long __val) const noexcept { return static_cast<size_t>(__val); } };

  
  template< > struct hash<unsigned short> : public __hash_base<size_t, unsigned short> { size_t operator()(unsigned short __val) const noexcept { return static_cast<size_t>(__val); } };

  
  template< > struct hash<unsigned int> : public __hash_base<size_t, unsigned int> { size_t operator()(unsigned int __val) const noexcept { return static_cast<size_t>(__val); } };

  
  template< > struct hash<unsigned long> : public __hash_base<size_t, unsigned long> { size_t operator()(unsigned long __val) const noexcept { return static_cast<size_t>(__val); } };

  
  template< > struct hash<unsigned long long> : public __hash_base<size_t, unsigned long long> { size_t operator()(unsigned long long __val) const noexcept { return static_cast<size_t>(__val); } };


  struct _Hash_impl
  {
    static size_t
    hash(const void* __ptr, size_t __clength,
	 size_t __seed = static_cast<size_t>(0xc70f6907UL))
    { return _Hash_bytes(__ptr, __clength, __seed); }

    template<typename _Tp>
      static size_t
      hash(const _Tp& __val)
      { return hash(&__val, sizeof(__val)); }

    template<typename _Tp>
      static size_t
      __hash_combine(const _Tp& __val, size_t __hash)
      { return hash(&__val, sizeof(__val), __hash); }
  };

  struct _Fnv_hash_impl
  {
    static size_t
    hash(const void* __ptr, size_t __clength,
	 size_t __seed = static_cast<size_t>(2166136261UL))
    { return _Fnv_hash_bytes(__ptr, __clength, __seed); }

    template<typename _Tp>
      static size_t
      hash(const _Tp& __val)
      { return hash(&__val, sizeof(__val)); }

    template<typename _Tp>
      static size_t
      __hash_combine(const _Tp& __val, size_t __hash)
      { return hash(&__val, sizeof(__val), __hash); }
  };

  
  template<>
    struct hash<float> : public __hash_base<size_t, float>
    {
      size_t
      operator()(float __val) const noexcept
      {
	
	return __val != 0.0f ? std::_Hash_impl::hash(__val) : 0;
      }
    };

  
  template<>
    struct hash<double> : public __hash_base<size_t, double>
    {
      size_t
      operator()(double __val) const noexcept
      {
	
	return __val != 0.0 ? std::_Hash_impl::hash(__val) : 0;
      }
    };

  
  template<>
    struct hash<long double>
    : public __hash_base<size_t, long double>
    {
      __attribute__ ((__pure__)) size_t
      operator()(long double __val) const noexcept;
    };

  

  
  
  
  
  template<typename _Hash>
    struct __is_fast_hash : public std::true_type
    { };

  template<>
    struct __is_fast_hash<hash<long double>> : public std::false_type
    { };


} 


namespace std 
{


  

  
  template<>
    struct hash<string>
    : public __hash_base<size_t, string>
    {
      size_t
      operator()(const string& __s) const noexcept
      { return std::_Hash_impl::hash(__s.data(), __s.length()); }
    };

  template<>
    struct __is_fast_hash<hash<string>> : std::false_type
    { };

  
  template<>
    struct hash<wstring>
    : public __hash_base<size_t, wstring>
    {
      size_t
      operator()(const wstring& __s) const noexcept
      { return std::_Hash_impl::hash(__s.data(),
                                     __s.length() * sizeof(wchar_t)); }
    };

  template<>
    struct __is_fast_hash<hash<wstring>> : std::false_type
    { };

  
  template<>
    struct hash<u16string>
    : public __hash_base<size_t, u16string>
    {
      size_t
      operator()(const u16string& __s) const noexcept
      { return std::_Hash_impl::hash(__s.data(),
                                     __s.length() * sizeof(char16_t)); }
    };

  template<>
    struct __is_fast_hash<hash<u16string>> : std::false_type
    { };

  
  template<>
    struct hash<u32string>
    : public __hash_base<size_t, u32string>
    {
      size_t
      operator()(const u32string& __s) const noexcept
      { return std::_Hash_impl::hash(__s.data(),
                                     __s.length() * sizeof(char32_t)); }
    };

  template<>
    struct __is_fast_hash<hash<u32string>> : std::false_type
    { };


} 





























 









#pragma GCC system_header


namespace std 
{


  template<typename _CharT, typename _Traits, typename _Alloc>
    const typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    _Rep::_S_max_size = (((npos - sizeof(_Rep_base))/sizeof(_CharT)) - 1) / 4;

  template<typename _CharT, typename _Traits, typename _Alloc>
    const _CharT
    basic_string<_CharT, _Traits, _Alloc>::
    _Rep::_S_terminal = _CharT();

  template<typename _CharT, typename _Traits, typename _Alloc>
    const typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::npos;

  
  
  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::_Rep::_S_empty_rep_storage[
    (sizeof(_Rep_base) + sizeof(_CharT) + sizeof(size_type) - 1) /
      sizeof(size_type)];

  
  
  
  
  template<typename _CharT, typename _Traits, typename _Alloc>
    template<typename _InIterator>
      _CharT*
      basic_string<_CharT, _Traits, _Alloc>::
      _S_construct(_InIterator __beg, _InIterator __end, const _Alloc& __a,
		   input_iterator_tag)
      {
	if (__beg == __end && __a == _Alloc())
	  return _S_empty_rep()._M_refdata();
	
	_CharT __buf[128];
	size_type __len = 0;
	while (__beg != __end && __len < sizeof(__buf) / sizeof(_CharT))
	  {
	    __buf[__len++] = *__beg;
	    ++__beg;
	  }
	_Rep* __r = _Rep::_S_create(__len, size_type(0), __a);
	_M_copy(__r->_M_refdata(), __buf, __len);
	try
	  {
	    while (__beg != __end)
	      {
		if (__len == __r->_M_capacity)
		  {
		    
		    _Rep* __another = _Rep::_S_create(__len + 1, __len, __a);
		    _M_copy(__another->_M_refdata(), __r->_M_refdata(), __len);
		    __r->_M_destroy(__a);
		    __r = __another;
		  }
		__r->_M_refdata()[__len++] = *__beg;
		++__beg;
	      }
	  }
	catch(...)
	  {
	    __r->_M_destroy(__a);
	    throw;
	  }
	__r->_M_set_length_and_sharable(__len);
	return __r->_M_refdata();
      }

  template<typename _CharT, typename _Traits, typename _Alloc>
    template <typename _InIterator>
      _CharT*
      basic_string<_CharT, _Traits, _Alloc>::
      _S_construct(_InIterator __beg, _InIterator __end, const _Alloc& __a,
		   forward_iterator_tag)
      {
	if (__beg == __end && __a == _Alloc())
	  return _S_empty_rep()._M_refdata();
	
	if (__gnu_cxx::__is_null_pointer(__beg) && __beg != __end)
	  __throw_logic_error(("basic_string::_S_construct null not valid"));

	const size_type __dnew = static_cast<size_type>(std::distance(__beg,
								      __end));
	
	_Rep* __r = _Rep::_S_create(__dnew, size_type(0), __a);
	try
	  { _S_copy_chars(__r->_M_refdata(), __beg, __end); }
	catch(...)
	  {
	    __r->_M_destroy(__a);
	    throw;
	  }
	__r->_M_set_length_and_sharable(__dnew);
	return __r->_M_refdata();
      }

  template<typename _CharT, typename _Traits, typename _Alloc>
    _CharT*
    basic_string<_CharT, _Traits, _Alloc>::
    _S_construct(size_type __n, _CharT __c, const _Alloc& __a)
    {
      if (__n == 0 && __a == _Alloc())
	return _S_empty_rep()._M_refdata();
      
      _Rep* __r = _Rep::_S_create(__n, size_type(0), __a);
      if (__n)
	_M_assign(__r->_M_refdata(), __n, __c);

      __r->_M_set_length_and_sharable(__n);
      return __r->_M_refdata();
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(const basic_string& __str)
    : _M_dataplus(__str._M_rep()->_M_grab(_Alloc(__str.get_allocator()),
					  __str.get_allocator()),
		  __str.get_allocator())
    { }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(const _Alloc& __a)
    : _M_dataplus(_S_construct(size_type(), _CharT(), __a), __a)
    { }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(const basic_string& __str, size_type __pos, size_type __n)
    : _M_dataplus(_S_construct(__str._M_data()
			       + __str._M_check(__pos,
						"basic_string::basic_string"),
			       __str._M_data() + __str._M_limit(__pos, __n)
			       + __pos, _Alloc()), _Alloc())
    { }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(const basic_string& __str, size_type __pos,
		 size_type __n, const _Alloc& __a)
    : _M_dataplus(_S_construct(__str._M_data()
			       + __str._M_check(__pos,
						"basic_string::basic_string"),
			       __str._M_data() + __str._M_limit(__pos, __n)
			       + __pos, __a), __a)
    { }

  
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(const _CharT* __s, size_type __n, const _Alloc& __a)
    : _M_dataplus(_S_construct(__s, __s + __n, __a), __a)
    { }

  
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(const _CharT* __s, const _Alloc& __a)
    : _M_dataplus(_S_construct(__s, __s ? __s + traits_type::length(__s) :
			       __s + npos, __a), __a)
    { }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(size_type __n, _CharT __c, const _Alloc& __a)
    : _M_dataplus(_S_construct(__n, __c, __a), __a)
    { }

  
  template<typename _CharT, typename _Traits, typename _Alloc>
    template<typename _InputIterator>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(_InputIterator __beg, _InputIterator __end, const _Alloc& __a)
    : _M_dataplus(_S_construct(__beg, __end, __a), __a)
    { }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>::
    basic_string(initializer_list<_CharT> __l, const _Alloc& __a)
    : _M_dataplus(_S_construct(__l.begin(), __l.end(), __a), __a)
    { }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    assign(const basic_string& __str)
    {
      if (_M_rep() != __str._M_rep())
	{
	  
	  const allocator_type __a = this->get_allocator();
	  _CharT* __tmp = __str._M_rep()->_M_grab(__a, __str.get_allocator());
	  _M_rep()->_M_dispose(__a);
	  _M_data(__tmp);
	}
      return *this;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    assign(const _CharT* __s, size_type __n)
    {
      ;
      _M_check_length(this->size(), __n, "basic_string::assign");
      if (_M_disjunct(__s) || _M_rep()->_M_is_shared())
	return _M_replace_safe(size_type(0), this->size(), __s, __n);
      else
	{
	  
	  const size_type __pos = __s - _M_data();
	  if (__pos >= __n)
	    _M_copy(_M_data(), __s, __n);
	  else if (__pos)
	    _M_move(_M_data(), __s, __n);
	  _M_rep()->_M_set_length_and_sharable(__n);
	  return *this;
	}
     }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    append(size_type __n, _CharT __c)
    {
      if (__n)
	{
	  _M_check_length(size_type(0), __n, "basic_string::append");	  
	  const size_type __len = __n + this->size();
	  if (__len > this->capacity() || _M_rep()->_M_is_shared())
	    this->reserve(__len);
	  _M_assign(_M_data() + this->size(), __n, __c);
	  _M_rep()->_M_set_length_and_sharable(__len);
	}
      return *this;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    append(const _CharT* __s, size_type __n)
    {
      ;
      if (__n)
	{
	  _M_check_length(size_type(0), __n, "basic_string::append");
	  const size_type __len = __n + this->size();
	  if (__len > this->capacity() || _M_rep()->_M_is_shared())
	    {
	      if (_M_disjunct(__s))
		this->reserve(__len);
	      else
		{
		  const size_type __off = __s - _M_data();
		  this->reserve(__len);
		  __s = _M_data() + __off;
		}
	    }
	  _M_copy(_M_data() + this->size(), __s, __n);
	  _M_rep()->_M_set_length_and_sharable(__len);
	}
      return *this;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    append(const basic_string& __str)
    {
      const size_type __size = __str.size();
      if (__size)
	{
	  const size_type __len = __size + this->size();
	  if (__len > this->capacity() || _M_rep()->_M_is_shared())
	    this->reserve(__len);
	  _M_copy(_M_data() + this->size(), __str._M_data(), __size);
	  _M_rep()->_M_set_length_and_sharable(__len);
	}
      return *this;
    }    

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    append(const basic_string& __str, size_type __pos, size_type __n)
    {
      __str._M_check(__pos, "basic_string::append");
      __n = __str._M_limit(__pos, __n);
      if (__n)
	{
	  const size_type __len = __n + this->size();
	  if (__len > this->capacity() || _M_rep()->_M_is_shared())
	    this->reserve(__len);
	  _M_copy(_M_data() + this->size(), __str._M_data() + __pos, __n);
	  _M_rep()->_M_set_length_and_sharable(__len);	  
	}
      return *this;
    }

   template<typename _CharT, typename _Traits, typename _Alloc>
     basic_string<_CharT, _Traits, _Alloc>&
     basic_string<_CharT, _Traits, _Alloc>::
     insert(size_type __pos, const _CharT* __s, size_type __n)
     {
       ;
       _M_check(__pos, "basic_string::insert");
       _M_check_length(size_type(0), __n, "basic_string::insert");
       if (_M_disjunct(__s) || _M_rep()->_M_is_shared())
         return _M_replace_safe(__pos, size_type(0), __s, __n);
       else
         {
           
           const size_type __off = __s - _M_data();
           _M_mutate(__pos, 0, __n);
           __s = _M_data() + __off;
           _CharT* __p = _M_data() + __pos;
           if (__s  + __n <= __p)
             _M_copy(__p, __s, __n);
           else if (__s >= __p)
             _M_copy(__p, __s + __n, __n);
           else
             {
	       const size_type __nleft = __p - __s;
               _M_copy(__p, __s, __nleft);
               _M_copy(__p + __nleft, __p + __n, __n - __nleft);
             }
           return *this;
         }
     }

   template<typename _CharT, typename _Traits, typename _Alloc>
     typename basic_string<_CharT, _Traits, _Alloc>::iterator
     basic_string<_CharT, _Traits, _Alloc>::
     erase(iterator __first, iterator __last)
     {
       ;

       
       
       
       const size_type __size = __last - __first;
       if (__size)
	 {
	   const size_type __pos = __first - _M_ibegin();
	   _M_mutate(__pos, __size, size_type(0));
	   _M_rep()->_M_set_leaked();
	   return iterator(_M_data() + __pos);
	 }
       else
	 return __first;
     }

   template<typename _CharT, typename _Traits, typename _Alloc>
     basic_string<_CharT, _Traits, _Alloc>&
     basic_string<_CharT, _Traits, _Alloc>::
     replace(size_type __pos, size_type __n1, const _CharT* __s,
	     size_type __n2)
     {
       ;
       _M_check(__pos, "basic_string::replace");
       __n1 = _M_limit(__pos, __n1);
       _M_check_length(__n1, __n2, "basic_string::replace");
       bool __left;
       if (_M_disjunct(__s) || _M_rep()->_M_is_shared())
         return _M_replace_safe(__pos, __n1, __s, __n2);
       else if ((__left = __s + __n2 <= _M_data() + __pos)
		|| _M_data() + __pos + __n1 <= __s)
	 {
	   
	   size_type __off = __s - _M_data();
	   __left ? __off : (__off += __n2 - __n1);
	   _M_mutate(__pos, __n1, __n2);
	   _M_copy(_M_data() + __pos, _M_data() + __off, __n2);
	   return *this;
	 }
       else
	 {
	   
	   const basic_string __tmp(__s, __n2);
	   return _M_replace_safe(__pos, __n1, __tmp._M_data(), __n2);
	 }
     }

  template<typename _CharT, typename _Traits, typename _Alloc>
    void
    basic_string<_CharT, _Traits, _Alloc>::_Rep::
    _M_destroy(const _Alloc& __a) throw ()
    {
      const size_type __size = sizeof(_Rep_base) +
	                       (this->_M_capacity + 1) * sizeof(_CharT);
      _Raw_bytes_alloc(__a).deallocate(reinterpret_cast<char*>(this), __size);
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    void
    basic_string<_CharT, _Traits, _Alloc>::
    _M_leak_hard()
    {
      if (_M_rep() == &_S_empty_rep())
	return;
      if (_M_rep()->_M_is_shared())
	_M_mutate(0, 0, 0);
      _M_rep()->_M_set_leaked();
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    void
    basic_string<_CharT, _Traits, _Alloc>::
    _M_mutate(size_type __pos, size_type __len1, size_type __len2)
    {
      const size_type __old_size = this->size();
      const size_type __new_size = __old_size + __len2 - __len1;
      const size_type __how_much = __old_size - __pos - __len1;

      if (__new_size > this->capacity() || _M_rep()->_M_is_shared())
	{
	  
	  const allocator_type __a = get_allocator();
	  _Rep* __r = _Rep::_S_create(__new_size, this->capacity(), __a);

	  if (__pos)
	    _M_copy(__r->_M_refdata(), _M_data(), __pos);
	  if (__how_much)
	    _M_copy(__r->_M_refdata() + __pos + __len2,
		    _M_data() + __pos + __len1, __how_much);

	  _M_rep()->_M_dispose(__a);
	  _M_data(__r->_M_refdata());
	}
      else if (__how_much && __len1 != __len2)
	{
	  
	  _M_move(_M_data() + __pos + __len2,
		  _M_data() + __pos + __len1, __how_much);
	}
      _M_rep()->_M_set_length_and_sharable(__new_size);
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    void
    basic_string<_CharT, _Traits, _Alloc>::
    reserve(size_type __res)
    {
      if (__res != this->capacity() || _M_rep()->_M_is_shared())
        {
	  
	  if (__res < this->size())
	    __res = this->size();
	  const allocator_type __a = get_allocator();
	  _CharT* __tmp = _M_rep()->_M_clone(__a, __res - this->size());
	  _M_rep()->_M_dispose(__a);
	  _M_data(__tmp);
        }
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    void
    basic_string<_CharT, _Traits, _Alloc>::
    swap(basic_string& __s)
    {
      if (_M_rep()->_M_is_leaked())
	_M_rep()->_M_set_sharable();
      if (__s._M_rep()->_M_is_leaked())
	__s._M_rep()->_M_set_sharable();
      if (this->get_allocator() == __s.get_allocator())
	{
	  _CharT* __tmp = _M_data();
	  _M_data(__s._M_data());
	  __s._M_data(__tmp);
	}
      
      else
	{
	  const basic_string __tmp1(_M_ibegin(), _M_iend(),
				    __s.get_allocator());
	  const basic_string __tmp2(__s._M_ibegin(), __s._M_iend(),
				    this->get_allocator());
	  *this = __tmp2;
	  __s = __tmp1;
	}
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::_Rep*
    basic_string<_CharT, _Traits, _Alloc>::_Rep::
    _S_create(size_type __capacity, size_type __old_capacity,
	      const _Alloc& __alloc)
    {
      
      
      if (__capacity > _S_max_size)
	__throw_length_error(("basic_string::_S_create"));

      
      
      

      
      
      
      
      
      
      
      
      

      
      
      
      
      
      
      
      
      
      const size_type __pagesize = 4096;
      const size_type __malloc_header_size = 4 * sizeof(void*);

      
      
      
      
      
      
      if (__capacity > __old_capacity && __capacity < 2 * __old_capacity)
	__capacity = 2 * __old_capacity;

      
      
      
      size_type __size = (__capacity + 1) * sizeof(_CharT) + sizeof(_Rep);

      const size_type __adj_size = __size + __malloc_header_size;
      if (__adj_size > __pagesize && __capacity > __old_capacity)
	{
	  const size_type __extra = __pagesize - __adj_size % __pagesize;
	  __capacity += __extra / sizeof(_CharT);
	  
	  if (__capacity > _S_max_size)
	    __capacity = _S_max_size;
	  __size = (__capacity + 1) * sizeof(_CharT) + sizeof(_Rep);
	}

      
      
      void* __place = _Raw_bytes_alloc(__alloc).allocate(__size);
      _Rep *__p = new (__place) _Rep;
      __p->_M_capacity = __capacity;
      
      
      
      
      
      
      
      __p->_M_set_sharable();
      return __p;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    _CharT*
    basic_string<_CharT, _Traits, _Alloc>::_Rep::
    _M_clone(const _Alloc& __alloc, size_type __res)
    {
      
      const size_type __requested_cap = this->_M_length + __res;
      _Rep* __r = _Rep::_S_create(__requested_cap, this->_M_capacity,
				  __alloc);
      if (this->_M_length)
	_M_copy(__r->_M_refdata(), _M_refdata(), this->_M_length);

      __r->_M_set_length_and_sharable(this->_M_length);
      return __r->_M_refdata();
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    void
    basic_string<_CharT, _Traits, _Alloc>::
    resize(size_type __n, _CharT __c)
    {
      const size_type __size = this->size();
      _M_check_length(__size, __n, "basic_string::resize");
      if (__size < __n)
	this->append(__n - __size, __c);
      else if (__n < __size)
	this->erase(__n);
      
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    template<typename _InputIterator>
      basic_string<_CharT, _Traits, _Alloc>&
      basic_string<_CharT, _Traits, _Alloc>::
      _M_replace_dispatch(iterator __i1, iterator __i2, _InputIterator __k1,
			  _InputIterator __k2, __false_type)
      {
	const basic_string __s(__k1, __k2);
	const size_type __n1 = __i2 - __i1;
	_M_check_length(__n1, __s.size(), "basic_string::_M_replace_dispatch");
	return _M_replace_safe(__i1 - _M_ibegin(), __n1, __s._M_data(),
			       __s.size());
      }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    _M_replace_aux(size_type __pos1, size_type __n1, size_type __n2,
		   _CharT __c)
    {
      _M_check_length(__n1, __n2, "basic_string::_M_replace_aux");
      _M_mutate(__pos1, __n1, __n2);
      if (__n2)
	_M_assign(_M_data() + __pos1, __n2, __c);
      return *this;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>&
    basic_string<_CharT, _Traits, _Alloc>::
    _M_replace_safe(size_type __pos1, size_type __n1, const _CharT* __s,
		    size_type __n2)
    {
      _M_mutate(__pos1, __n1, __n2);
      if (__n2)
	_M_copy(_M_data() + __pos1, __s, __n2);
      return *this;
    }
   
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>
    operator+(const _CharT* __lhs,
	      const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    {
      ;
      typedef basic_string<_CharT, _Traits, _Alloc> __string_type;
      typedef typename __string_type::size_type	  __size_type;
      const __size_type __len = _Traits::length(__lhs);
      __string_type __str;
      __str.reserve(__len + __rhs.size());
      __str.append(__lhs, __len);
      __str.append(__rhs);
      return __str;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_string<_CharT, _Traits, _Alloc>
    operator+(_CharT __lhs, const basic_string<_CharT, _Traits, _Alloc>& __rhs)
    {
      typedef basic_string<_CharT, _Traits, _Alloc> __string_type;
      typedef typename __string_type::size_type	  __size_type;
      __string_type __str;
      const __size_type __len = __rhs.size();
      __str.reserve(__len + 1);
      __str.append(__size_type(1), __lhs);
      __str.append(__rhs);
      return __str;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    copy(_CharT* __s, size_type __n, size_type __pos) const
    {
      _M_check(__pos, "basic_string::copy");
      __n = _M_limit(__pos, __n);
      ;
      if (__n)
	_M_copy(__s, _M_data() + __pos, __n);
      
      return __n;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find(const _CharT* __s, size_type __pos, size_type __n) const
    {
      ;
      const size_type __size = this->size();
      const _CharT* __data = _M_data();

      if (__n == 0)
	return __pos <= __size ? __pos : npos;

      if (__n <= __size)
	{
	  for (; __pos <= __size - __n; ++__pos)
	    if (traits_type::eq(__data[__pos], __s[0])
		&& traits_type::compare(__data + __pos + 1,
					__s + 1, __n - 1) == 0)
	      return __pos;
	}
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find(_CharT __c, size_type __pos) const noexcept
    {
      size_type __ret = npos;
      const size_type __size = this->size();
      if (__pos < __size)
	{
	  const _CharT* __data = _M_data();
	  const size_type __n = __size - __pos;
	  const _CharT* __p = traits_type::find(__data + __pos, __n, __c);
	  if (__p)
	    __ret = __p - __data;
	}
      return __ret;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    rfind(const _CharT* __s, size_type __pos, size_type __n) const
    {
      ;
      const size_type __size = this->size();
      if (__n <= __size)
	{
	  __pos = std::min(size_type(__size - __n), __pos);
	  const _CharT* __data = _M_data();
	  do
	    {
	      if (traits_type::compare(__data + __pos, __s, __n) == 0)
		return __pos;
	    }
	  while (__pos-- > 0);
	}
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    rfind(_CharT __c, size_type __pos) const noexcept
    {
      size_type __size = this->size();
      if (__size)
	{
	  if (--__size > __pos)
	    __size = __pos;
	  for (++__size; __size-- > 0; )
	    if (traits_type::eq(_M_data()[__size], __c))
	      return __size;
	}
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find_first_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
      ;
      for (; __n && __pos < this->size(); ++__pos)
	{
	  const _CharT* __p = traits_type::find(__s, __n, _M_data()[__pos]);
	  if (__p)
	    return __pos;
	}
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find_last_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
      ;
      size_type __size = this->size();
      if (__size && __n)
	{
	  if (--__size > __pos)
	    __size = __pos;
	  do
	    {
	      if (traits_type::find(__s, __n, _M_data()[__size]))
		return __size;
	    }
	  while (__size-- != 0);
	}
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find_first_not_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
      ;
      for (; __pos < this->size(); ++__pos)
	if (!traits_type::find(__s, __n, _M_data()[__pos]))
	  return __pos;
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find_first_not_of(_CharT __c, size_type __pos) const noexcept
    {
      for (; __pos < this->size(); ++__pos)
	if (!traits_type::eq(_M_data()[__pos], __c))
	  return __pos;
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find_last_not_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
      ;
      size_type __size = this->size();
      if (__size)
	{
	  if (--__size > __pos)
	    __size = __pos;
	  do
	    {
	      if (!traits_type::find(__s, __n, _M_data()[__size]))
		return __size;
	    }
	  while (__size--);
	}
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    typename basic_string<_CharT, _Traits, _Alloc>::size_type
    basic_string<_CharT, _Traits, _Alloc>::
    find_last_not_of(_CharT __c, size_type __pos) const noexcept
    {
      size_type __size = this->size();
      if (__size)
	{
	  if (--__size > __pos)
	    __size = __pos;
	  do
	    {
	      if (!traits_type::eq(_M_data()[__size], __c))
		return __size;
	    }
	  while (__size--);
	}
      return npos;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    int
    basic_string<_CharT, _Traits, _Alloc>::
    compare(size_type __pos, size_type __n, const basic_string& __str) const
    {
      _M_check(__pos, "basic_string::compare");
      __n = _M_limit(__pos, __n);
      const size_type __osize = __str.size();
      const size_type __len = std::min(__n, __osize);
      int __r = traits_type::compare(_M_data() + __pos, __str.data(), __len);
      if (!__r)
	__r = _S_compare(__n, __osize);
      return __r;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    int
    basic_string<_CharT, _Traits, _Alloc>::
    compare(size_type __pos1, size_type __n1, const basic_string& __str,
	    size_type __pos2, size_type __n2) const
    {
      _M_check(__pos1, "basic_string::compare");
      __str._M_check(__pos2, "basic_string::compare");
      __n1 = _M_limit(__pos1, __n1);
      __n2 = __str._M_limit(__pos2, __n2);
      const size_type __len = std::min(__n1, __n2);
      int __r = traits_type::compare(_M_data() + __pos1,
				     __str.data() + __pos2, __len);
      if (!__r)
	__r = _S_compare(__n1, __n2);
      return __r;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    int
    basic_string<_CharT, _Traits, _Alloc>::
    compare(const _CharT* __s) const
    {
      ;
      const size_type __size = this->size();
      const size_type __osize = traits_type::length(__s);
      const size_type __len = std::min(__size, __osize);
      int __r = traits_type::compare(_M_data(), __s, __len);
      if (!__r)
	__r = _S_compare(__size, __osize);
      return __r;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    int
    basic_string <_CharT, _Traits, _Alloc>::
    compare(size_type __pos, size_type __n1, const _CharT* __s) const
    {
      ;
      _M_check(__pos, "basic_string::compare");
      __n1 = _M_limit(__pos, __n1);
      const size_type __osize = traits_type::length(__s);
      const size_type __len = std::min(__n1, __osize);
      int __r = traits_type::compare(_M_data() + __pos, __s, __len);
      if (!__r)
	__r = _S_compare(__n1, __osize);
      return __r;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    int
    basic_string <_CharT, _Traits, _Alloc>::
    compare(size_type __pos, size_type __n1, const _CharT* __s,
	    size_type __n2) const
    {
      ;
      _M_check(__pos, "basic_string::compare");
      __n1 = _M_limit(__pos, __n1);
      const size_type __len = std::min(__n1, __n2);
      int __r = traits_type::compare(_M_data() + __pos, __s, __len);
      if (!__r)
	__r = _S_compare(__n1, __n2);
      return __r;
    }

  
  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_istream<_CharT, _Traits>&
    operator>>(basic_istream<_CharT, _Traits>& __in,
	       basic_string<_CharT, _Traits, _Alloc>& __str)
    {
      typedef basic_istream<_CharT, _Traits>		__istream_type;
      typedef basic_string<_CharT, _Traits, _Alloc>	__string_type;
      typedef typename __istream_type::ios_base         __ios_base;
      typedef typename __istream_type::int_type		__int_type;
      typedef typename __string_type::size_type		__size_type;
      typedef ctype<_CharT>				__ctype_type;
      typedef typename __ctype_type::ctype_base         __ctype_base;

      __size_type __extracted = 0;
      typename __ios_base::iostate __err = __ios_base::goodbit;
      typename __istream_type::sentry __cerb(__in, false);
      if (__cerb)
	{
	  try
	    {
	      
	      __str.erase();
	      _CharT __buf[128];
	      __size_type __len = 0;	      
	      const streamsize __w = __in.width();
	      const __size_type __n = __w > 0 ? static_cast<__size_type>(__w)
		                              : __str.max_size();
	      const __ctype_type& __ct = use_facet<__ctype_type>(__in.getloc());
	      const __int_type __eof = _Traits::eof();
	      __int_type __c = __in.rdbuf()->sgetc();

	      while (__extracted < __n
		     && !_Traits::eq_int_type(__c, __eof)
		     && !__ct.is(__ctype_base::space,
				 _Traits::to_char_type(__c)))
		{
		  if (__len == sizeof(__buf) / sizeof(_CharT))
		    {
		      __str.append(__buf, sizeof(__buf) / sizeof(_CharT));
		      __len = 0;
		    }
		  __buf[__len++] = _Traits::to_char_type(__c);
		  ++__extracted;
		  __c = __in.rdbuf()->snextc();
		}
	      __str.append(__buf, __len);

	      if (_Traits::eq_int_type(__c, __eof))
		__err |= __ios_base::eofbit;
	      __in.width(0);
	    }
	  catch(__cxxabiv1::__forced_unwind&)
	    {
	      __in._M_setstate(__ios_base::badbit);
	      throw;
	    }
	  catch(...)
	    {
	      
	      
	      
	      __in._M_setstate(__ios_base::badbit);
	    }
	}
      
      if (!__extracted)
	__err |= __ios_base::failbit;
      if (__err)
	__in.setstate(__err);
      return __in;
    }

  template<typename _CharT, typename _Traits, typename _Alloc>
    basic_istream<_CharT, _Traits>&
    getline(basic_istream<_CharT, _Traits>& __in,
	    basic_string<_CharT, _Traits, _Alloc>& __str, _CharT __delim)
    {
      typedef basic_istream<_CharT, _Traits>		__istream_type;
      typedef basic_string<_CharT, _Traits, _Alloc>	__string_type;
      typedef typename __istream_type::ios_base         __ios_base;
      typedef typename __istream_type::int_type		__int_type;
      typedef typename __string_type::size_type		__size_type;

      __size_type __extracted = 0;
      const __size_type __n = __str.max_size();
      typename __ios_base::iostate __err = __ios_base::goodbit;
      typename __istream_type::sentry __cerb(__in, true);
      if (__cerb)
	{
	  try
	    {
	      __str.erase();
	      const __int_type __idelim = _Traits::to_int_type(__delim);
	      const __int_type __eof = _Traits::eof();
	      __int_type __c = __in.rdbuf()->sgetc();

	      while (__extracted < __n
		     && !_Traits::eq_int_type(__c, __eof)
		     && !_Traits::eq_int_type(__c, __idelim))
		{
		  __str += _Traits::to_char_type(__c);
		  ++__extracted;
		  __c = __in.rdbuf()->snextc();
		}

	      if (_Traits::eq_int_type(__c, __eof))
		__err |= __ios_base::eofbit;
	      else if (_Traits::eq_int_type(__c, __idelim))
		{
		  ++__extracted;		  
		  __in.rdbuf()->sbumpc();
		}
	      else
		__err |= __ios_base::failbit;
	    }
	  catch(__cxxabiv1::__forced_unwind&)
	    {
	      __in._M_setstate(__ios_base::badbit);
	      throw;
	    }
	  catch(...)
	    {
	      
	      
	      
	      __in._M_setstate(__ios_base::badbit);
	    }
	}
      if (!__extracted)
	__err |= __ios_base::failbit;
      if (__err)
	__in.setstate(__err);
      return __in;
    }

  
  
  extern template class basic_string<char>;
  extern template
    basic_istream<char>&
    operator>>(basic_istream<char>&, string&);
  extern template
    basic_ostream<char>&
    operator<<(basic_ostream<char>&, const string&);
  extern template
    basic_istream<char>&
    getline(basic_istream<char>&, string&, char);
  extern template
    basic_istream<char>&
    getline(basic_istream<char>&, string&);

  extern template class basic_string<wchar_t>;
  extern template
    basic_istream<wchar_t>&
    operator>>(basic_istream<wchar_t>&, wstring&);
  extern template
    basic_ostream<wchar_t>&
    operator<<(basic_ostream<wchar_t>&, const wstring&);
  extern template
    basic_istream<wchar_t>&
    getline(basic_istream<wchar_t>&, wstring&, wchar_t);
  extern template
    basic_istream<wchar_t>&
    getline(basic_istream<wchar_t>&, wstring&);


} 



namespace std 
{


  


 

  



 
  class logic_error : public exception 
  {
    string _M_msg;

  public:
     
    explicit 
    logic_error(const string& __arg);

    virtual ~logic_error() noexcept;

    
 
    virtual const char* 
    what() const noexcept;
  };

  
 
  class domain_error : public logic_error 
  {
  public:
    explicit domain_error(const string& __arg);
    virtual ~domain_error() noexcept;
  };

   
  class invalid_argument : public logic_error 
  {
  public:
    explicit invalid_argument(const string& __arg);
    virtual ~invalid_argument() noexcept;
  };

  
 
  class length_error : public logic_error 
  {
  public:
    explicit length_error(const string& __arg);
    virtual ~length_error() noexcept;
  };

  
 
  class out_of_range : public logic_error 
  {
  public:
    explicit out_of_range(const string& __arg);
    virtual ~out_of_range() noexcept;
  };

  



 
  class runtime_error : public exception 
  {
    string _M_msg;

  public:
     
    explicit 
    runtime_error(const string& __arg);

    virtual ~runtime_error() noexcept;

    
 
    virtual const char* 
    what() const noexcept;
  };

   
  class range_error : public runtime_error 
  {
  public:
    explicit range_error(const string& __arg);
    virtual ~range_error() noexcept;
  };

   
  class overflow_error : public runtime_error 
  {
  public:
    explicit overflow_error(const string& __arg);
    virtual ~overflow_error() noexcept;
  };

   
  class underflow_error : public runtime_error 
  {
  public:
    explicit underflow_error(const string& __arg);
    virtual ~underflow_error() noexcept;
  };

  


} 


namespace std 
{


  template<typename _Tp, std::size_t _Nm>
    struct __array_traits
    {
      typedef _Tp _Type[_Nm];

      static constexpr _Tp&
      _S_ref(const _Type& __t, std::size_t __n) noexcept
      { return const_cast<_Tp&>(__t[__n]); }
    };

 template<typename _Tp>
   struct __array_traits<_Tp, 0>
   {
     struct _Type { };

     static constexpr _Tp&
     _S_ref(const _Type&, std::size_t) noexcept
     { return *static_cast<_Tp*>(nullptr); }
   };

  












 
  template<typename _Tp, std::size_t _Nm>
    struct array
    {
      typedef _Tp 	    			      value_type;
      typedef value_type*			      pointer;
      typedef const value_type*                       const_pointer;
      typedef value_type&                   	      reference;
      typedef const value_type&             	      const_reference;
      typedef value_type*          		      iterator;
      typedef const value_type*			      const_iterator;
      typedef std::size_t                    	      size_type;
      typedef std::ptrdiff_t                   	      difference_type;
      typedef std::reverse_iterator<iterator>	      reverse_iterator;
      typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

      
      typedef std::__array_traits<_Tp, _Nm> _AT_Type;
      typename _AT_Type::_Type                         _M_elems;

      

      
      void
      fill(const value_type& __u)
      { std::fill_n(begin(), size(), __u); }

      void
      swap(array& __other)
      noexcept(noexcept(swap(std::declval<_Tp&>(), std::declval<_Tp&>())))
      { std::swap_ranges(begin(), end(), __other.begin()); }

      
      iterator
      begin() noexcept
      { return iterator(data()); }

      const_iterator
      begin() const noexcept
      { return const_iterator(data()); }

      iterator
      end() noexcept
      { return iterator(data() + _Nm); }

      const_iterator
      end() const noexcept
      { return const_iterator(data() + _Nm); }

      reverse_iterator 
      rbegin() noexcept
      { return reverse_iterator(end()); }

      const_reverse_iterator 
      rbegin() const noexcept
      { return const_reverse_iterator(end()); }

      reverse_iterator 
      rend() noexcept
      { return reverse_iterator(begin()); }

      const_reverse_iterator 
      rend() const noexcept
      { return const_reverse_iterator(begin()); }

      const_iterator
      cbegin() const noexcept
      { return const_iterator(data()); }

      const_iterator
      cend() const noexcept
      { return const_iterator(data() + _Nm); }

      const_reverse_iterator 
      crbegin() const noexcept
      { return const_reverse_iterator(end()); }

      const_reverse_iterator 
      crend() const noexcept
      { return const_reverse_iterator(begin()); }

      
      constexpr size_type 
      size() const noexcept { return _Nm; }

      constexpr size_type 
      max_size() const noexcept { return _Nm; }

      constexpr bool 
      empty() const noexcept { return size() == 0; }

      
      reference
      operator[](size_type __n)
      { return _AT_Type::_S_ref(_M_elems, __n); }

      constexpr const_reference
      operator[](size_type __n) const noexcept
      { return _AT_Type::_S_ref(_M_elems, __n); }

      reference
      at(size_type __n)
      {
	if (__n >= _Nm)
	  std::__throw_out_of_range(("array::at"));
	return _AT_Type::_S_ref(_M_elems, __n);
      }

      constexpr const_reference
      at(size_type __n) const
      {
	
	
	return __n < _Nm ? _AT_Type::_S_ref(_M_elems, __n)
	  : (std::__throw_out_of_range(("array::at")),
	     _AT_Type::_S_ref(_M_elems, 0));
      }

      reference 
      front()
      { return *begin(); }

      constexpr const_reference 
      front() const
      { return _AT_Type::_S_ref(_M_elems, 0); }

      reference 
      back()
      { return _Nm ? *(end() - 1) : *end(); }

      constexpr const_reference 
      back() const
      { 
	return _Nm ? _AT_Type::_S_ref(_M_elems, _Nm - 1) 
 	           : _AT_Type::_S_ref(_M_elems, 0);
      }

      pointer
      data() noexcept
      { return std::__addressof(_AT_Type::_S_ref(_M_elems, 0)); }

      const_pointer
      data() const noexcept
      { return std::__addressof(_AT_Type::_S_ref(_M_elems, 0)); }
    };

  
  template<typename _Tp, std::size_t _Nm>
    inline bool 
    operator==(const array<_Tp, _Nm>& __one, const array<_Tp, _Nm>& __two)
    { return std::equal(__one.begin(), __one.end(), __two.begin()); }

  template<typename _Tp, std::size_t _Nm>
    inline bool
    operator!=(const array<_Tp, _Nm>& __one, const array<_Tp, _Nm>& __two)
    { return !(__one == __two); }

  template<typename _Tp, std::size_t _Nm>
    inline bool
    operator<(const array<_Tp, _Nm>& __a, const array<_Tp, _Nm>& __b)
    { 
      return std::lexicographical_compare(__a.begin(), __a.end(),
					  __b.begin(), __b.end()); 
    }

  template<typename _Tp, std::size_t _Nm>
    inline bool
    operator>(const array<_Tp, _Nm>& __one, const array<_Tp, _Nm>& __two)
    { return __two < __one; }

  template<typename _Tp, std::size_t _Nm>
    inline bool
    operator<=(const array<_Tp, _Nm>& __one, const array<_Tp, _Nm>& __two)
    { return !(__one > __two); }

  template<typename _Tp, std::size_t _Nm>
    inline bool
    operator>=(const array<_Tp, _Nm>& __one, const array<_Tp, _Nm>& __two)
    { return !(__one < __two); }

  
  template<typename _Tp, std::size_t _Nm>
    inline void
    swap(array<_Tp, _Nm>& __one, array<_Tp, _Nm>& __two)
    noexcept(noexcept(__one.swap(__two)))
    { __one.swap(__two); }

  template<std::size_t _Int, typename _Tp, std::size_t _Nm>
    constexpr _Tp&
    get(array<_Tp, _Nm>& __arr) noexcept
    {
      static_assert(_Int < _Nm, "index is out of bounds");
      return std::__array_traits<_Tp, _Nm>::
	_S_ref(__arr._M_elems, _Int);
    }

  template<std::size_t _Int, typename _Tp, std::size_t _Nm>
    constexpr _Tp&&
    get(array<_Tp, _Nm>&& __arr) noexcept
    {
      static_assert(_Int < _Nm, "index is out of bounds");
      return std::move(get<_Int>(__arr));
    }

  template<std::size_t _Int, typename _Tp, std::size_t _Nm>
    constexpr const _Tp&
    get(const array<_Tp, _Nm>& __arr) noexcept
    {
      static_assert(_Int < _Nm, "index is out of bounds");
      return std::__array_traits<_Tp, _Nm>::
	_S_ref(__arr._M_elems, _Int);
    }


} 

namespace std 
{


  

  
  template<typename _Tp> 
    class tuple_size;

  template<typename _Tp, std::size_t _Nm>
    struct tuple_size<std::array<_Tp, _Nm>>
    : public integral_constant<std::size_t, _Nm> { };

  
  template<std::size_t _Int, typename _Tp>
    class tuple_element;

  template<std::size_t _Int, typename _Tp, std::size_t _Nm>
    struct tuple_element<_Int, std::array<_Tp, _Nm>>
    {
      static_assert(_Int < _Nm, "index is out of bounds");
      typedef _Tp type;
    };


} 





namespace std 
{


  


 

  
  template<typename _Tp>
    struct __add_c_ref
    { typedef const _Tp& type; };

  template<typename _Tp>
    struct __add_c_ref<_Tp&>
    { typedef _Tp& type; };

  
  template<typename _Tp>
    struct __add_ref
    { typedef _Tp& type; };

  template<typename _Tp>
    struct __add_ref<_Tp&>
    { typedef _Tp& type; };

  
  template<typename _Tp>
    struct __add_r_ref
    { typedef _Tp&& type; };

  template<typename _Tp>
    struct __add_r_ref<_Tp&>
    { typedef _Tp& type; };

  template<std::size_t _Idx, typename _Head, bool _IsEmptyNotFinal>
    struct _Head_base;

  template<std::size_t _Idx, typename _Head>
    struct _Head_base<_Idx, _Head, true>
    : public _Head
    {
      constexpr _Head_base()
      : _Head() { }

      constexpr _Head_base(const _Head& __h)
      : _Head(__h) { }

      template<typename _UHead, typename = typename
	       enable_if<!is_convertible<_UHead,
	                                 __uses_alloc_base>::value>::type>
        constexpr _Head_base(_UHead&& __h)
	: _Head(std::forward<_UHead>(__h)) { }

      _Head_base(__uses_alloc0)
      : _Head() { }

      template<typename _Alloc>
	_Head_base(__uses_alloc1<_Alloc> __a)
	: _Head(allocator_arg, *__a._M_a) { }

      template<typename _Alloc>
	_Head_base(__uses_alloc2<_Alloc> __a)
	: _Head(*__a._M_a) { }

      template<typename _UHead>
	_Head_base(__uses_alloc0, _UHead&& __uhead)
	: _Head(std::forward<_UHead>(__uhead)) { }

      template<typename _Alloc, typename _UHead>
	_Head_base(__uses_alloc1<_Alloc> __a, _UHead&& __uhead)
	: _Head(allocator_arg, *__a._M_a, std::forward<_UHead>(__uhead)) { }

      template<typename _Alloc, typename _UHead>
	_Head_base(__uses_alloc2<_Alloc> __a, _UHead&& __uhead)
	: _Head(std::forward<_UHead>(__uhead), *__a._M_a) { }

      static constexpr _Head&
      _M_head(_Head_base& __b) noexcept { return __b; }

      static constexpr const _Head&
      _M_head(const _Head_base& __b) noexcept { return __b; }
    };

  template<std::size_t _Idx, typename _Head>
    struct _Head_base<_Idx, _Head, false>
    {
      constexpr _Head_base()
      : _M_head_impl() { }

      constexpr _Head_base(const _Head& __h)
      : _M_head_impl(__h) { }

      template<typename _UHead, typename = typename
	       enable_if<!is_convertible<_UHead,
	                                 __uses_alloc_base>::value>::type>
        constexpr _Head_base(_UHead&& __h)
	: _M_head_impl(std::forward<_UHead>(__h)) { }

      _Head_base(__uses_alloc0)
      : _M_head_impl() { }

      template<typename _Alloc>
	_Head_base(__uses_alloc1<_Alloc> __a)
	: _M_head_impl(allocator_arg, *__a._M_a) { }

      template<typename _Alloc>
	_Head_base(__uses_alloc2<_Alloc> __a)
	: _M_head_impl(*__a._M_a) { }

      template<typename _UHead>
	_Head_base(__uses_alloc0, _UHead&& __uhead)
	: _M_head_impl(std::forward<_UHead>(__uhead)) { }

      template<typename _Alloc, typename _UHead>
	_Head_base(__uses_alloc1<_Alloc> __a, _UHead&& __uhead)
	: _M_head_impl(allocator_arg, *__a._M_a, std::forward<_UHead>(__uhead))
	{ }

      template<typename _Alloc, typename _UHead>
	_Head_base(__uses_alloc2<_Alloc> __a, _UHead&& __uhead)
	: _M_head_impl(std::forward<_UHead>(__uhead), *__a._M_a) { }

      static constexpr _Head&
      _M_head(_Head_base& __b) noexcept { return __b._M_head_impl; }

      static constexpr const _Head&
      _M_head(const _Head_base& __b) noexcept { return __b._M_head_impl; }

      _Head _M_head_impl;
    };

  






 
  template<std::size_t _Idx, typename... _Elements>
    struct _Tuple_impl; 

  


 
  template<std::size_t _Idx>
    struct _Tuple_impl<_Idx>
    {
      template<std::size_t, typename...> friend class _Tuple_impl;

      _Tuple_impl() = default;

      template<typename _Alloc>
        _Tuple_impl(allocator_arg_t, const _Alloc&) { }

      template<typename _Alloc>
        _Tuple_impl(allocator_arg_t, const _Alloc&, const _Tuple_impl&) { }

      template<typename _Alloc>
        _Tuple_impl(allocator_arg_t, const _Alloc&, _Tuple_impl&&) { }

    protected:
      void _M_swap(_Tuple_impl&) noexcept {   }
    };

  template<typename _Tp>
    struct __is_empty_non_tuple : is_empty<_Tp> { };

  
  template<typename _El0, typename... _El>
    struct __is_empty_non_tuple<tuple<_El0, _El...>> : false_type { };

  
  template<typename _Tp>
    using __empty_not_final
    = typename conditional<__is_final(_Tp), false_type,
			   __is_empty_non_tuple<_Tp>>::type;

  



 
  template<std::size_t _Idx, typename _Head, typename... _Tail>
    struct _Tuple_impl<_Idx, _Head, _Tail...>
    : public _Tuple_impl<_Idx + 1, _Tail...>,
      private _Head_base<_Idx, _Head, __empty_not_final<_Head>::value>
    {
      template<std::size_t, typename...> friend class _Tuple_impl;

      typedef _Tuple_impl<_Idx + 1, _Tail...> _Inherited;
      typedef _Head_base<_Idx, _Head, __empty_not_final<_Head>::value> _Base;

      static constexpr _Head&  
      _M_head(_Tuple_impl& __t) noexcept { return _Base::_M_head(__t); }

      static constexpr const _Head&
      _M_head(const _Tuple_impl& __t) noexcept { return _Base::_M_head(__t); }

      static constexpr _Inherited&
      _M_tail(_Tuple_impl& __t) noexcept { return __t; }

      static constexpr const _Inherited&
      _M_tail(const _Tuple_impl& __t) noexcept { return __t; }

      constexpr _Tuple_impl()
      : _Inherited(), _Base() { }

      explicit 
      constexpr _Tuple_impl(const _Head& __head, const _Tail&... __tail)
      : _Inherited(__tail...), _Base(__head) { }

      template<typename _UHead, typename... _UTail, typename = typename
               enable_if<sizeof...(_Tail) == sizeof...(_UTail)>::type> 
        explicit
        constexpr _Tuple_impl(_UHead&& __head, _UTail&&... __tail)
	: _Inherited(std::forward<_UTail>(__tail)...),
	  _Base(std::forward<_UHead>(__head)) { }

      constexpr _Tuple_impl(const _Tuple_impl&) = default;

      constexpr
      _Tuple_impl(_Tuple_impl&& __in)
      noexcept(__and_<is_nothrow_move_constructible<_Head>,
	              is_nothrow_move_constructible<_Inherited>>::value)
      : _Inherited(std::move(_M_tail(__in))), 
	_Base(std::forward<_Head>(_M_head(__in))) { }

      template<typename... _UElements>
        constexpr _Tuple_impl(const _Tuple_impl<_Idx, _UElements...>& __in)
	: _Inherited(_Tuple_impl<_Idx, _UElements...>::_M_tail(__in)),
	  _Base(_Tuple_impl<_Idx, _UElements...>::_M_head(__in)) { }

      template<typename _UHead, typename... _UTails>
        constexpr _Tuple_impl(_Tuple_impl<_Idx, _UHead, _UTails...>&& __in)
	: _Inherited(std::move
		     (_Tuple_impl<_Idx, _UHead, _UTails...>::_M_tail(__in))),
	  _Base(std::forward<_UHead>
		(_Tuple_impl<_Idx, _UHead, _UTails...>::_M_head(__in))) { }

      template<typename _Alloc>
	_Tuple_impl(allocator_arg_t __tag, const _Alloc& __a)
	: _Inherited(__tag, __a),
          _Base(__use_alloc<_Head>(__a)) { }

      template<typename _Alloc>
	_Tuple_impl(allocator_arg_t __tag, const _Alloc& __a,
		    const _Head& __head, const _Tail&... __tail)
	: _Inherited(__tag, __a, __tail...),
          _Base(__use_alloc<_Head, _Alloc, _Head>(__a), __head) { }

      template<typename _Alloc, typename _UHead, typename... _UTail,
               typename = typename enable_if<sizeof...(_Tail)
					     == sizeof...(_UTail)>::type>
	_Tuple_impl(allocator_arg_t __tag, const _Alloc& __a,
	            _UHead&& __head, _UTail&&... __tail)
	: _Inherited(__tag, __a, std::forward<_UTail>(__tail)...),
          _Base(__use_alloc<_Head, _Alloc, _UHead>(__a),
	        std::forward<_UHead>(__head)) { }

      template<typename _Alloc>
        _Tuple_impl(allocator_arg_t __tag, const _Alloc& __a,
	            const _Tuple_impl& __in)
	: _Inherited(__tag, __a, _M_tail(__in)), 
          _Base(__use_alloc<_Head, _Alloc, _Head>(__a), _M_head(__in)) { }

      template<typename _Alloc>
	_Tuple_impl(allocator_arg_t __tag, const _Alloc& __a,
	            _Tuple_impl&& __in)
	: _Inherited(__tag, __a, std::move(_M_tail(__in))), 
	  _Base(__use_alloc<_Head, _Alloc, _Head>(__a),
	        std::forward<_Head>(_M_head(__in))) { }

      template<typename _Alloc, typename... _UElements>
	_Tuple_impl(allocator_arg_t __tag, const _Alloc& __a,
	            const _Tuple_impl<_Idx, _UElements...>& __in)
	: _Inherited(__tag, __a,
		     _Tuple_impl<_Idx, _UElements...>::_M_tail(__in)),
	  _Base(__use_alloc<_Head, _Alloc, _Head>(__a),
		_Tuple_impl<_Idx, _UElements...>::_M_head(__in)) { }

      template<typename _Alloc, typename _UHead, typename... _UTails>
	_Tuple_impl(allocator_arg_t __tag, const _Alloc& __a,
	            _Tuple_impl<_Idx, _UHead, _UTails...>&& __in)
	: _Inherited(__tag, __a, std::move
		     (_Tuple_impl<_Idx, _UHead, _UTails...>::_M_tail(__in))),
	  _Base(__use_alloc<_Head, _Alloc, _UHead>(__a),
                std::forward<_UHead>
		(_Tuple_impl<_Idx, _UHead, _UTails...>::_M_head(__in))) { }

      _Tuple_impl&
      operator=(const _Tuple_impl& __in)
      {
	_M_head(*this) = _M_head(__in);
	_M_tail(*this) = _M_tail(__in);
	return *this;
      }

      _Tuple_impl&
      operator=(_Tuple_impl&& __in)
      noexcept(__and_<is_nothrow_move_assignable<_Head>,
	              is_nothrow_move_assignable<_Inherited>>::value)
      {
	_M_head(*this) = std::forward<_Head>(_M_head(__in));
	_M_tail(*this) = std::move(_M_tail(__in));
	return *this;
      }

      template<typename... _UElements>
        _Tuple_impl&
        operator=(const _Tuple_impl<_Idx, _UElements...>& __in)
        {
	  _M_head(*this) = _Tuple_impl<_Idx, _UElements...>::_M_head(__in);
	  _M_tail(*this) = _Tuple_impl<_Idx, _UElements...>::_M_tail(__in);
	  return *this;
	}

      template<typename _UHead, typename... _UTails>
        _Tuple_impl&
        operator=(_Tuple_impl<_Idx, _UHead, _UTails...>&& __in)
        {
	  _M_head(*this) = std::forward<_UHead>
	    (_Tuple_impl<_Idx, _UHead, _UTails...>::_M_head(__in));
	  _M_tail(*this) = std::move
	    (_Tuple_impl<_Idx, _UHead, _UTails...>::_M_tail(__in));
	  return *this;
	}

    protected:
      void
      _M_swap(_Tuple_impl& __in)
      noexcept(noexcept(swap(std::declval<_Head&>(),
			     std::declval<_Head&>()))
	       && noexcept(_M_tail(__in)._M_swap(_M_tail(__in))))
      {
	using std::swap;
	swap(_M_head(*this), _M_head(__in));
	_Inherited::_M_swap(_M_tail(__in));
      }
    };

  
  template<typename... _Elements> 
    class tuple : public _Tuple_impl<0, _Elements...>
    {
      typedef _Tuple_impl<0, _Elements...> _Inherited;

    public:
      constexpr tuple()
      : _Inherited() { }

      explicit
      constexpr tuple(const _Elements&... __elements)
      : _Inherited(__elements...) { }

      template<typename... _UElements, typename = typename
        enable_if<__and_<is_convertible<_UElements,
					_Elements>...>::value>::type>
	explicit
        constexpr tuple(_UElements&&... __elements)
	: _Inherited(std::forward<_UElements>(__elements)...) {	}

      constexpr tuple(const tuple&) = default;

      constexpr tuple(tuple&&) = default; 

      template<typename... _UElements, typename = typename
        enable_if<__and_<is_convertible<const _UElements&,
					_Elements>...>::value>::type>
        constexpr tuple(const tuple<_UElements...>& __in)
        : _Inherited(static_cast<const _Tuple_impl<0, _UElements...>&>(__in))
        { }

      template<typename... _UElements, typename = typename
        enable_if<__and_<is_convertible<_UElements,
					_Elements>...>::value>::type>
        constexpr tuple(tuple<_UElements...>&& __in)
        : _Inherited(static_cast<_Tuple_impl<0, _UElements...>&&>(__in)) { }

      

      template<typename _Alloc>
	tuple(allocator_arg_t __tag, const _Alloc& __a)
	: _Inherited(__tag, __a) { }

      template<typename _Alloc>
	tuple(allocator_arg_t __tag, const _Alloc& __a,
	      const _Elements&... __elements)
	: _Inherited(__tag, __a, __elements...) { }

      template<typename _Alloc, typename... _UElements, typename = typename
	       enable_if<sizeof...(_UElements)
			 == sizeof...(_Elements)>::type>
	tuple(allocator_arg_t __tag, const _Alloc& __a,
	      _UElements&&... __elements)
	: _Inherited(__tag, __a, std::forward<_UElements>(__elements)...)
       	{ }

      template<typename _Alloc>
	tuple(allocator_arg_t __tag, const _Alloc& __a, const tuple& __in)
	: _Inherited(__tag, __a, static_cast<const _Inherited&>(__in)) { }

      template<typename _Alloc>
	tuple(allocator_arg_t __tag, const _Alloc& __a, tuple&& __in)
	: _Inherited(__tag, __a, static_cast<_Inherited&&>(__in)) { }

      template<typename _Alloc, typename... _UElements, typename = typename
	       enable_if<sizeof...(_UElements)
			 == sizeof...(_Elements)>::type>
	tuple(allocator_arg_t __tag, const _Alloc& __a,
	      const tuple<_UElements...>& __in)
	: _Inherited(__tag, __a,
	             static_cast<const _Tuple_impl<0, _UElements...>&>(__in))
	{ }

      template<typename _Alloc, typename... _UElements, typename = typename
	       enable_if<sizeof...(_UElements)
			 == sizeof...(_Elements)>::type>
	tuple(allocator_arg_t __tag, const _Alloc& __a,
	      tuple<_UElements...>&& __in)
	: _Inherited(__tag, __a,
	             static_cast<_Tuple_impl<0, _UElements...>&&>(__in))
	{ }

      tuple&
      operator=(const tuple& __in)
      {
	static_cast<_Inherited&>(*this) = __in;
	return *this;
      }

      tuple&
      operator=(tuple&& __in)
      noexcept(is_nothrow_move_assignable<_Inherited>::value)
      {
	static_cast<_Inherited&>(*this) = std::move(__in);
	return *this;
      }

      template<typename... _UElements, typename = typename
	       enable_if<sizeof...(_UElements)
			 == sizeof...(_Elements)>::type>
        tuple&
        operator=(const tuple<_UElements...>& __in)
        {
	  static_cast<_Inherited&>(*this) = __in;
	  return *this;
	}

      template<typename... _UElements, typename = typename
	       enable_if<sizeof...(_UElements)
			 == sizeof...(_Elements)>::type>
        tuple&
        operator=(tuple<_UElements...>&& __in)
        {
	  static_cast<_Inherited&>(*this) = std::move(__in);
	  return *this;
	}

      void
      swap(tuple& __in)
      noexcept(noexcept(__in._M_swap(__in)))
      { _Inherited::_M_swap(__in); }
    };

  
  template<>  
    class tuple<>
    {
    public:
      void swap(tuple&) noexcept {   }
    };

  
  
  template<typename _T1, typename _T2>
    class tuple<_T1, _T2> : public _Tuple_impl<0, _T1, _T2>
    {
      typedef _Tuple_impl<0, _T1, _T2> _Inherited;

    public:
      constexpr tuple()
      : _Inherited() { }

      explicit
      constexpr tuple(const _T1& __a1, const _T2& __a2)
      : _Inherited(__a1, __a2) { }

      template<typename _U1, typename _U2, typename = typename
	       enable_if<__and_<is_convertible<_U1, _T1>,
				is_convertible<_U2, _T2>>::value>::type>
        explicit
        constexpr tuple(_U1&& __a1, _U2&& __a2)
	: _Inherited(std::forward<_U1>(__a1), std::forward<_U2>(__a2)) { }

      constexpr tuple(const tuple&) = default;

      constexpr tuple(tuple&&) = default;

      template<typename _U1, typename _U2, typename = typename
	enable_if<__and_<is_convertible<const _U1&, _T1>,
			 is_convertible<const _U2&, _T2>>::value>::type>
        constexpr tuple(const tuple<_U1, _U2>& __in)
	: _Inherited(static_cast<const _Tuple_impl<0, _U1, _U2>&>(__in)) { }

      template<typename _U1, typename _U2, typename = typename
	       enable_if<__and_<is_convertible<_U1, _T1>,
				is_convertible<_U2, _T2>>::value>::type>
        constexpr tuple(tuple<_U1, _U2>&& __in)
	: _Inherited(static_cast<_Tuple_impl<0, _U1, _U2>&&>(__in)) { }

      template<typename _U1, typename _U2, typename = typename
	enable_if<__and_<is_convertible<const _U1&, _T1>,
			 is_convertible<const _U2&, _T2>>::value>::type>
        constexpr tuple(const pair<_U1, _U2>& __in)
	: _Inherited(__in.first, __in.second) { }

      template<typename _U1, typename _U2, typename = typename
	       enable_if<__and_<is_convertible<_U1, _T1>,
				is_convertible<_U2, _T2>>::value>::type>
        constexpr tuple(pair<_U1, _U2>&& __in)
	: _Inherited(std::forward<_U1>(__in.first),
		     std::forward<_U2>(__in.second)) { }

      

      template<typename _Alloc>
	tuple(allocator_arg_t __tag, const _Alloc& __a)
	: _Inherited(__tag, __a) { }

      template<typename _Alloc>
	tuple(allocator_arg_t __tag, const _Alloc& __a,
	      const _T1& __a1, const _T2& __a2)
	: _Inherited(__tag, __a, __a1, __a2) { }

      template<typename _Alloc, typename _U1, typename _U2>
	tuple(allocator_arg_t __tag, const _Alloc& __a, _U1&& __a1, _U2&& __a2)
	: _Inherited(__tag, __a, std::forward<_U1>(__a1),
	             std::forward<_U2>(__a2)) { }

      template<typename _Alloc>
	tuple(allocator_arg_t __tag, const _Alloc& __a, const tuple& __in)
	: _Inherited(__tag, __a, static_cast<const _Inherited&>(__in)) { }

      template<typename _Alloc>
	tuple(allocator_arg_t __tag, const _Alloc& __a, tuple&& __in)
	: _Inherited(__tag, __a, static_cast<_Inherited&&>(__in)) { }

      template<typename _Alloc, typename _U1, typename _U2>
	tuple(allocator_arg_t __tag, const _Alloc& __a,
	      const tuple<_U1, _U2>& __in)
	: _Inherited(__tag, __a,
	             static_cast<const _Tuple_impl<0, _U1, _U2>&>(__in))
	{ }

      template<typename _Alloc, typename _U1, typename _U2>
	tuple(allocator_arg_t __tag, const _Alloc& __a, tuple<_U1, _U2>&& __in)
	: _Inherited(__tag, __a, static_cast<_Tuple_impl<0, _U1, _U2>&&>(__in))
	{ }

      template<typename _Alloc, typename _U1, typename _U2>
        tuple(allocator_arg_t __tag, const _Alloc& __a,
	      const pair<_U1, _U2>& __in)
	: _Inherited(__tag, __a, __in.first, __in.second) { }

      template<typename _Alloc, typename _U1, typename _U2>
        tuple(allocator_arg_t __tag, const _Alloc& __a, pair<_U1, _U2>&& __in)
	: _Inherited(__tag, __a, std::forward<_U1>(__in.first),
		     std::forward<_U2>(__in.second)) { }

      tuple&
      operator=(const tuple& __in)
      {
	static_cast<_Inherited&>(*this) = __in;
	return *this;
      }

      tuple&
      operator=(tuple&& __in)
      noexcept(is_nothrow_move_assignable<_Inherited>::value)
      {
	static_cast<_Inherited&>(*this) = std::move(__in);
	return *this;
      }

      template<typename _U1, typename _U2>
        tuple&
        operator=(const tuple<_U1, _U2>& __in)
        {
	  static_cast<_Inherited&>(*this) = __in;
	  return *this;
	}

      template<typename _U1, typename _U2>
        tuple&
        operator=(tuple<_U1, _U2>&& __in)
        {
	  static_cast<_Inherited&>(*this) = std::move(__in);
	  return *this;
	}

      template<typename _U1, typename _U2>
        tuple&
        operator=(const pair<_U1, _U2>& __in)
        {
	  this->_M_head(*this) = __in.first;
	  this->_M_tail(*this)._M_head(*this) = __in.second;
	  return *this;
	}

      template<typename _U1, typename _U2>
        tuple&
        operator=(pair<_U1, _U2>&& __in)
        {
	  this->_M_head(*this) = std::forward<_U1>(__in.first);
	  this->_M_tail(*this)._M_head(*this) = std::forward<_U2>(__in.second);
	  return *this;
	}

      void
      swap(tuple& __in)
      noexcept(noexcept(__in._M_swap(__in)))
      { _Inherited::_M_swap(__in); }
    };


  
  template<std::size_t __i, typename _Tp>
    struct tuple_element;

  


 
  template<std::size_t __i, typename _Head, typename... _Tail>
    struct tuple_element<__i, tuple<_Head, _Tail...> >
    : tuple_element<__i - 1, tuple<_Tail...> > { };

  

 
  template<typename _Head, typename... _Tail>
    struct tuple_element<0, tuple<_Head, _Tail...> >
    {
      typedef _Head type;
    };

  template<std::size_t __i, typename _Tp>
    struct tuple_element<__i, const _Tp>
    {
      typedef typename
      add_const<typename tuple_element<__i, _Tp>::type>::type type;
    };

  template<std::size_t __i, typename _Tp>
    struct tuple_element<__i, volatile _Tp>
    {
      typedef typename
      add_volatile<typename tuple_element<__i, _Tp>::type>::type type;
    };

  template<std::size_t __i, typename _Tp>
    struct tuple_element<__i, const volatile _Tp>
    {
      typedef typename
      add_cv<typename tuple_element<__i, _Tp>::type>::type type;
    };

  
  template<typename _Tp>
    struct tuple_size;

  template<typename _Tp>
    struct tuple_size<const _Tp>
    : public integral_constant<
             typename remove_cv<decltype(tuple_size<_Tp>::value)>::type,
             tuple_size<_Tp>::value> { };

  template<typename _Tp>
    struct tuple_size<volatile _Tp>
    : public integral_constant<
             typename remove_cv<decltype(tuple_size<_Tp>::value)>::type,
             tuple_size<_Tp>::value> { };

  template<typename _Tp>
    struct tuple_size<const volatile _Tp>
    : public integral_constant<
             typename remove_cv<decltype(tuple_size<_Tp>::value)>::type,
             tuple_size<_Tp>::value> { };

  
  template<typename... _Elements>
    struct tuple_size<tuple<_Elements...>>
    : public integral_constant<std::size_t, sizeof...(_Elements)> { };

  template<std::size_t __i, typename _Head, typename... _Tail>
    constexpr typename __add_ref<_Head>::type
    __get_helper(_Tuple_impl<__i, _Head, _Tail...>& __t) noexcept
    { return _Tuple_impl<__i, _Head, _Tail...>::_M_head(__t); }

  template<std::size_t __i, typename _Head, typename... _Tail>
    constexpr typename __add_c_ref<_Head>::type
    __get_helper(const _Tuple_impl<__i, _Head, _Tail...>& __t) noexcept
    { return _Tuple_impl<__i, _Head, _Tail...>::_M_head(__t); }

  
  
  
  template<std::size_t __i, typename... _Elements>
    constexpr typename __add_ref<
                      typename tuple_element<__i, tuple<_Elements...>>::type
                    >::type
    get(tuple<_Elements...>& __t) noexcept
    { return std::__get_helper<__i>(__t); }

  template<std::size_t __i, typename... _Elements>
    constexpr typename __add_c_ref<
                      typename tuple_element<__i, tuple<_Elements...>>::type
                    >::type
    get(const tuple<_Elements...>& __t) noexcept
    { return std::__get_helper<__i>(__t); }

  template<std::size_t __i, typename... _Elements>
    constexpr typename __add_r_ref<
                      typename tuple_element<__i, tuple<_Elements...>>::type
                    >::type
    get(tuple<_Elements...>&& __t) noexcept
    { return std::forward<typename tuple_element<__i,
	tuple<_Elements...>>::type&&>(get<__i>(__t)); }

  
  template<std::size_t __check_equal_size, std::size_t __i, std::size_t __j,
	   typename _Tp, typename _Up>
    struct __tuple_compare;

  template<std::size_t __i, std::size_t __j, typename _Tp, typename _Up>
    struct __tuple_compare<0, __i, __j, _Tp, _Up>
    {
      static constexpr bool 
      __eq(const _Tp& __t, const _Up& __u)
      {
	return (get<__i>(__t) == get<__i>(__u) &&
		__tuple_compare<0, __i + 1, __j, _Tp, _Up>::__eq(__t, __u));
      }
     
      static constexpr bool 
      __less(const _Tp& __t, const _Up& __u)
      {
	return ((get<__i>(__t) < get<__i>(__u))
		|| !(get<__i>(__u) < get<__i>(__t)) &&
		__tuple_compare<0, __i + 1, __j, _Tp, _Up>::__less(__t, __u));
      }
    };

  template<std::size_t __i, typename _Tp, typename _Up>
    struct __tuple_compare<0, __i, __i, _Tp, _Up>
    {
      static constexpr bool 
      __eq(const _Tp&, const _Up&) { return true; }
     
      static constexpr bool 
      __less(const _Tp&, const _Up&) { return false; }
    };

  template<typename... _TElements, typename... _UElements>
    constexpr bool
    operator==(const tuple<_TElements...>& __t,
	       const tuple<_UElements...>& __u)
    {
      typedef tuple<_TElements...> _Tp;
      typedef tuple<_UElements...> _Up;
      return bool(__tuple_compare<tuple_size<_Tp>::value - tuple_size<_Up>::value,
	      0, tuple_size<_Tp>::value, _Tp, _Up>::__eq(__t, __u));
    }

  template<typename... _TElements, typename... _UElements>
    constexpr bool
    operator<(const tuple<_TElements...>& __t,
	      const tuple<_UElements...>& __u)
    {
      typedef tuple<_TElements...> _Tp;
      typedef tuple<_UElements...> _Up;
      return bool(__tuple_compare<tuple_size<_Tp>::value - tuple_size<_Up>::value,
	      0, tuple_size<_Tp>::value, _Tp, _Up>::__less(__t, __u));
    }

  template<typename... _TElements, typename... _UElements>
    inline constexpr bool
    operator!=(const tuple<_TElements...>& __t,
	       const tuple<_UElements...>& __u)
    { return !(__t == __u); }

  template<typename... _TElements, typename... _UElements>
    inline constexpr bool
    operator>(const tuple<_TElements...>& __t,
	      const tuple<_UElements...>& __u)
    { return __u < __t; }

  template<typename... _TElements, typename... _UElements>
    inline constexpr bool
    operator<=(const tuple<_TElements...>& __t,
	       const tuple<_UElements...>& __u)
    { return !(__u < __t); }

  template<typename... _TElements, typename... _UElements>
    inline constexpr bool
    operator>=(const tuple<_TElements...>& __t,
	       const tuple<_UElements...>& __u)
    { return !(__t < __u); }

  
  template<typename... _Elements>
    constexpr tuple<typename __decay_and_strip<_Elements>::__type...>
    make_tuple(_Elements&&... __args)
    {
      typedef tuple<typename __decay_and_strip<_Elements>::__type...>
	__result_type;
      return __result_type(std::forward<_Elements>(__args)...);
    }

  template<typename... _Elements>
    tuple<_Elements&&...>
    forward_as_tuple(_Elements&&... __args) noexcept
    { return tuple<_Elements&&...>(std::forward<_Elements>(__args)...); }

  template<typename>
    struct __is_tuple_like_impl : false_type
    { };

  template<typename... _Tps>
    struct __is_tuple_like_impl<tuple<_Tps...>> : true_type
    { };

  template<typename _T1, typename _T2>
    struct __is_tuple_like_impl<pair<_T1, _T2>> : true_type
    { };

  template<typename _Tp, std::size_t _Nm>
    struct __is_tuple_like_impl<array<_Tp, _Nm>> : true_type
    { };

  
  template<typename _Tp>
    struct __is_tuple_like
    : public __is_tuple_like_impl<typename std::remove_cv
            <typename std::remove_reference<_Tp>::type>::type>::type
    { };

  
  
  template<std::size_t... _Indexes>
    struct _Index_tuple
    {
      typedef _Index_tuple<_Indexes..., sizeof...(_Indexes)> __next;
    };

  
  template<std::size_t _Num>
    struct _Build_index_tuple
    {
      typedef typename _Build_index_tuple<_Num - 1>::__type::__next __type;
    };

  template<>
    struct _Build_index_tuple<0>
    {
      typedef _Index_tuple<> __type;
    };

  template<std::size_t, typename, typename, std::size_t>
    struct __make_tuple_impl;

  template<std::size_t _Idx, typename _Tuple, typename... _Tp,
           std::size_t _Nm>
    struct __make_tuple_impl<_Idx, tuple<_Tp...>, _Tuple, _Nm>
    {
      typedef typename __make_tuple_impl<_Idx + 1, tuple<_Tp...,
	typename std::tuple_element<_Idx, _Tuple>::type>, _Tuple, _Nm>::__type
      __type;
    };

  template<std::size_t _Nm, typename _Tuple, typename... _Tp>
    struct __make_tuple_impl<_Nm, tuple<_Tp...>, _Tuple, _Nm>
    {
      typedef tuple<_Tp...> __type;
    };

  template<typename _Tuple>
    struct __do_make_tuple
    : public __make_tuple_impl<0, tuple<>, _Tuple,
                               std::tuple_size<_Tuple>::value>
    { };

  
  template<typename _Tuple>
    struct __make_tuple
    : public __do_make_tuple<typename std::remove_cv
            <typename std::remove_reference<_Tuple>::type>::type>
    { };

  
  template<typename...>
    struct __combine_tuples;

  template<>
    struct __combine_tuples<>
    {
      typedef tuple<> __type;
    };

  template<typename... _Ts>
    struct __combine_tuples<tuple<_Ts...>>
    {
      typedef tuple<_Ts...> __type;
    };

  template<typename... _T1s, typename... _T2s, typename... _Rem>
    struct __combine_tuples<tuple<_T1s...>, tuple<_T2s...>, _Rem...>
    {
      typedef typename __combine_tuples<tuple<_T1s..., _T2s...>,
					_Rem...>::__type __type;
    };

  
  template<typename... _Tpls>
    struct __tuple_cat_result
    {
      typedef typename __combine_tuples
        <typename __make_tuple<_Tpls>::__type...>::__type __type;
    };

  
  
  template<typename...>
    struct __make_1st_indices;

  template<>
    struct __make_1st_indices<>
    {
      typedef std::_Index_tuple<> __type;
    };

  template<typename _Tp, typename... _Tpls>
    struct __make_1st_indices<_Tp, _Tpls...>
    {
      typedef typename std::_Build_index_tuple<std::tuple_size<
	typename std::remove_reference<_Tp>::type>::value>::__type __type;
    };

  
  
  
  template<typename _Ret, typename _Indices, typename... _Tpls>
    struct __tuple_concater;

  template<typename _Ret, std::size_t... _Is, typename _Tp, typename... _Tpls>
    struct __tuple_concater<_Ret, std::_Index_tuple<_Is...>, _Tp, _Tpls...>
    {
      template<typename... _Us>
        static constexpr _Ret
        _S_do(_Tp&& __tp, _Tpls&&... __tps, _Us&&... __us)
        {
	  typedef typename __make_1st_indices<_Tpls...>::__type __idx;
	  typedef __tuple_concater<_Ret, __idx, _Tpls...>      __next;
	  return __next::_S_do(std::forward<_Tpls>(__tps)...,
			       std::forward<_Us>(__us)...,
			       std::get<_Is>(std::forward<_Tp>(__tp))...);
	}
    };

  template<typename _Ret>
    struct __tuple_concater<_Ret, std::_Index_tuple<>>
    {
      template<typename... _Us>
	static constexpr _Ret
	_S_do(_Us&&... __us)
        {
	  return _Ret(std::forward<_Us>(__us)...);
	}
    };

  
  template<typename... _Tpls, typename = typename
           enable_if<__and_<__is_tuple_like<_Tpls>...>::value>::type>
    constexpr auto
    tuple_cat(_Tpls&&... __tpls)
    -> typename __tuple_cat_result<_Tpls...>::__type
    {
      typedef typename __tuple_cat_result<_Tpls...>::__type __ret;
      typedef typename __make_1st_indices<_Tpls...>::__type __idx;
      typedef __tuple_concater<__ret, __idx, _Tpls...> __concater;
      return __concater::_S_do(std::forward<_Tpls>(__tpls)...);
    }

  
  template<typename... _Elements>
    inline tuple<_Elements&...>
    tie(_Elements&... __args) noexcept
    { return tuple<_Elements&...>(__args...); }

  
  template<typename... _Elements>
    inline void 
    swap(tuple<_Elements...>& __x, tuple<_Elements...>& __y)
    noexcept(noexcept(__x.swap(__y)))
    { __x.swap(__y); }

  
  
  struct _Swallow_assign
  {
    template<class _Tp>
      const _Swallow_assign&
      operator=(const _Tp&) const
      { return *this; }
  };

  const _Swallow_assign ignore{};

  
  template<typename... _Types, typename _Alloc>
    struct uses_allocator<tuple<_Types...>, _Alloc> : true_type { };

  
  template<class _T1, class _T2>
    template<typename... _Args1, typename... _Args2>
      inline
      pair<_T1, _T2>::
      pair(piecewise_construct_t,
	   tuple<_Args1...> __first, tuple<_Args2...> __second)
      : pair(__first, __second,
	     typename _Build_index_tuple<sizeof...(_Args1)>::__type(),
	     typename _Build_index_tuple<sizeof...(_Args2)>::__type())
      { }

  template<class _T1, class _T2>
    template<typename... _Args1, std::size_t... _Indexes1,
             typename... _Args2, std::size_t... _Indexes2>
      inline
      pair<_T1, _T2>::
      pair(tuple<_Args1...>& __tuple1, tuple<_Args2...>& __tuple2,
	   _Index_tuple<_Indexes1...>, _Index_tuple<_Indexes2...>)
      : first(std::forward<_Args1>(std::get<_Indexes1>(__tuple1))...),
        second(std::forward<_Args2>(std::get<_Indexes2>(__tuple2))...)
      { }

  


} 



namespace std 
{


  template<typename _MemberPointer>
    class _Mem_fn;
  template<typename _Tp, typename _Class>
    _Mem_fn<_Tp _Class::*>
    mem_fn(_Tp _Class::*) noexcept;

template<typename _Tp> class __has_result_type_helper : __sfinae_types { template<typename _Up> struct _Wrap_type { }; template<typename _Up> static __one __test(_Wrap_type<typename _Up::result_type> *); template<typename _Up> static __two __test(...); public: static constexpr bool value = sizeof(__test<_Tp>(0)) == 1; }; template<typename _Tp> struct __has_result_type : integral_constant<bool, __has_result_type_helper <typename remove_cv<_Tp> ::type> ::value> { };

  
  template<bool _Has_result_type, typename _Functor>
    struct _Maybe_get_result_type
    { };

  template<typename _Functor>
    struct _Maybe_get_result_type<true, _Functor>
    { typedef typename _Functor::result_type result_type; };

  


 
  template<typename _Functor>
    struct _Weak_result_type_impl
    : _Maybe_get_result_type<__has_result_type<_Functor>::value, _Functor>
    { };

  
  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes...)>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes......)>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes...) const>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes......) const>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes...) volatile>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes......) volatile>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes...) const volatile>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(_ArgTypes......) const volatile>
    { typedef _Res result_type; };

  
  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(&)(_ArgTypes...)>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(&)(_ArgTypes......)>
    { typedef _Res result_type; };

  
  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(*)(_ArgTypes...)>
    { typedef _Res result_type; };

  template<typename _Res, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res(*)(_ArgTypes......)>
    { typedef _Res result_type; };

  
  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes...)>
    { typedef _Res result_type; };

  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes......)>
    { typedef _Res result_type; };

  
  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes...) const>
    { typedef _Res result_type; };

  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes......) const>
    { typedef _Res result_type; };

  
  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes...) volatile>
    { typedef _Res result_type; };

  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes......) volatile>
    { typedef _Res result_type; };

  
  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes...)
				  const volatile>
    { typedef _Res result_type; };

  template<typename _Res, typename _Class, typename... _ArgTypes>
    struct _Weak_result_type_impl<_Res (_Class::*)(_ArgTypes......)
				  const volatile>
    { typedef _Res result_type; };

  


 
  template<typename _Functor>
    struct _Weak_result_type
    : _Weak_result_type_impl<typename remove_cv<_Functor>::type>
    { };

  
  template<typename _Tp>
    struct _Derives_from_unary_function : __sfinae_types
    {
    private:
      template<typename _T1, typename _Res>
	static __one __test(const volatile unary_function<_T1, _Res>*);

      
      
      static __two __test(...);

    public:
      static const bool value = sizeof(__test((_Tp*)0)) == 1;
    };

  
  template<typename _Tp>
    struct _Derives_from_binary_function : __sfinae_types
    {
    private:
      template<typename _T1, typename _T2, typename _Res>
	static __one __test(const volatile binary_function<_T1, _T2, _Res>*);

      
      
      static __two __test(...);

    public:
      static const bool value = sizeof(__test((_Tp*)0)) == 1;
    };

  


 
  template<typename _Functor, typename... _Args>
    inline
    typename enable_if<
	     (!is_member_pointer<_Functor>::value
	      && !is_function<_Functor>::value
	      && !is_function<typename remove_pointer<_Functor>::type>::value),
	     typename result_of<_Functor&(_Args&&...)>::type
	   >::type
    __invoke(_Functor& __f, _Args&&... __args)
    {
      return __f(std::forward<_Args>(__args)...);
    }

  template<typename _Functor, typename... _Args>
    inline
    typename enable_if<
             (is_member_pointer<_Functor>::value
              && !is_function<_Functor>::value
              && !is_function<typename remove_pointer<_Functor>::type>::value),
             typename result_of<_Functor(_Args&&...)>::type
           >::type
    __invoke(_Functor& __f, _Args&&... __args)
    {
      return std::mem_fn(__f)(std::forward<_Args>(__args)...);
    }

  
  template<typename _Functor, typename... _Args>
    inline
    typename enable_if<
	     (is_pointer<_Functor>::value
	      && is_function<typename remove_pointer<_Functor>::type>::value),
	     typename result_of<_Functor(_Args&&...)>::type
	   >::type
    __invoke(_Functor __f, _Args&&... __args)
    {
      return __f(std::forward<_Args>(__args)...);
    }

  



 
  template<bool _Unary, bool _Binary, typename _Tp>
    struct _Reference_wrapper_base_impl;

  
  template<typename _Tp>
    struct _Reference_wrapper_base_impl<false, false, _Tp>
    : _Weak_result_type<_Tp>
    { };

  
  template<typename _Tp>
    struct _Reference_wrapper_base_impl<true, false, _Tp>
    : _Weak_result_type<_Tp>
    {
      typedef typename _Tp::argument_type argument_type;
    };

  
  template<typename _Tp>
    struct _Reference_wrapper_base_impl<false, true, _Tp>
    : _Weak_result_type<_Tp>
    {
      typedef typename _Tp::first_argument_type first_argument_type;
      typedef typename _Tp::second_argument_type second_argument_type;
    };

  
   template<typename _Tp>
    struct _Reference_wrapper_base_impl<true, true, _Tp>
    : _Weak_result_type<_Tp>
    {
      typedef typename _Tp::argument_type argument_type;
      typedef typename _Tp::first_argument_type first_argument_type;
      typedef typename _Tp::second_argument_type second_argument_type;
    };

  template<typename _Tp> class __has_argument_type_helper : __sfinae_types { template<typename _Up> struct _Wrap_type { }; template<typename _Up> static __one __test(_Wrap_type<typename _Up::argument_type> *); template<typename _Up> static __two __test(...); public: static constexpr bool value = sizeof(__test<_Tp>(0)) == 1; }; template<typename _Tp> struct __has_argument_type : integral_constant<bool, __has_argument_type_helper <typename remove_cv<_Tp> ::type> ::value> { };
  template<typename _Tp> class __has_first_argument_type_helper : __sfinae_types { template<typename _Up> struct _Wrap_type { }; template<typename _Up> static __one __test(_Wrap_type<typename _Up::first_argument_type> *); template<typename _Up> static __two __test(...); public: static constexpr bool value = sizeof(__test<_Tp>(0)) == 1; }; template<typename _Tp> struct __has_first_argument_type : integral_constant<bool, __has_first_argument_type_helper <typename remove_cv<_Tp> ::type> ::value> { };
  template<typename _Tp> class __has_second_argument_type_helper : __sfinae_types { template<typename _Up> struct _Wrap_type { }; template<typename _Up> static __one __test(_Wrap_type<typename _Up::second_argument_type> *); template<typename _Up> static __two __test(...); public: static constexpr bool value = sizeof(__test<_Tp>(0)) == 1; }; template<typename _Tp> struct __has_second_argument_type : integral_constant<bool, __has_second_argument_type_helper <typename remove_cv<_Tp> ::type> ::value> { };

  




 
  template<typename _Tp>
    struct _Reference_wrapper_base
    : _Reference_wrapper_base_impl<
      __has_argument_type<_Tp>::value,
      __has_first_argument_type<_Tp>::value
      && __has_second_argument_type<_Tp>::value,
      _Tp>
    { };

  
  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res(_T1)>
    : unary_function<_T1, _Res>
    { };

  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res(_T1) const>
    : unary_function<_T1, _Res>
    { };

  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res(_T1) volatile>
    : unary_function<_T1, _Res>
    { };

  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res(_T1) const volatile>
    : unary_function<_T1, _Res>
    { };

  
  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res(_T1, _T2)>
    : binary_function<_T1, _T2, _Res>
    { };

  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res(_T1, _T2) const>
    : binary_function<_T1, _T2, _Res>
    { };

  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res(_T1, _T2) volatile>
    : binary_function<_T1, _T2, _Res>
    { };

  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res(_T1, _T2) const volatile>
    : binary_function<_T1, _T2, _Res>
    { };

  
  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res(*)(_T1)>
    : unary_function<_T1, _Res>
    { };

  
  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res(*)(_T1, _T2)>
    : binary_function<_T1, _T2, _Res>
    { };

  
  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res (_T1::*)()>
    : unary_function<_T1*, _Res>
    { };

  
  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res (_T1::*)(_T2)>
    : binary_function<_T1*, _T2, _Res>
    { };

  
  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res (_T1::*)() const>
    : unary_function<const _T1*, _Res>
    { };

  
  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res (_T1::*)(_T2) const>
    : binary_function<const _T1*, _T2, _Res>
    { };

  
  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res (_T1::*)() volatile>
    : unary_function<volatile _T1*, _Res>
    { };

  
  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res (_T1::*)(_T2) volatile>
    : binary_function<volatile _T1*, _T2, _Res>
    { };

  
  template<typename _Res, typename _T1>
    struct _Reference_wrapper_base<_Res (_T1::*)() const volatile>
    : unary_function<const volatile _T1*, _Res>
    { };

  
  template<typename _Res, typename _T1, typename _T2>
    struct _Reference_wrapper_base<_Res (_T1::*)(_T2) const volatile>
    : binary_function<const volatile _T1*, _T2, _Res>
    { };

  



 
  template<typename _Tp>
    class reference_wrapper
    : public _Reference_wrapper_base<typename remove_cv<_Tp>::type>
    {
      _Tp* _M_data;

    public:
      typedef _Tp type;

      reference_wrapper(_Tp& __indata) noexcept
      : _M_data(std::__addressof(__indata))
      { }

      reference_wrapper(_Tp&&) = delete;

      reference_wrapper(const reference_wrapper<_Tp>& __inref) noexcept
      : _M_data(__inref._M_data)
      { }

      reference_wrapper&
      operator=(const reference_wrapper<_Tp>& __inref) noexcept
      {
	_M_data = __inref._M_data;
	return *this;
      }

      operator _Tp&() const noexcept
      { return this->get(); }

      _Tp&
      get() const noexcept
      { return *_M_data; }

      template<typename... _Args>
	typename result_of<_Tp&(_Args&&...)>::type
	operator()(_Args&&... __args) const
	{
	  return __invoke(get(), std::forward<_Args>(__args)...);
	}
    };


  
  template<typename _Tp>
    inline reference_wrapper<_Tp>
    ref(_Tp& __t) noexcept
    { return reference_wrapper<_Tp>(__t); }

  
  template<typename _Tp>
    inline reference_wrapper<const _Tp>
    cref(const _Tp& __t) noexcept
    { return reference_wrapper<const _Tp>(__t); }

  template<typename _Tp>
    void ref(const _Tp&&) = delete;

  template<typename _Tp>
    void cref(const _Tp&&) = delete;

  
  template<typename _Tp>
    inline reference_wrapper<_Tp>
    ref(reference_wrapper<_Tp> __t) noexcept
    { return ref(__t.get()); }

  
  template<typename _Tp>
    inline reference_wrapper<const _Tp>
    cref(reference_wrapper<_Tp> __t) noexcept
    { return cref(__t.get()); }

  

  template<typename... _Types>
    struct _Pack : integral_constant<size_t, sizeof...(_Types)>
    { };

  template<typename _From, typename _To, bool = _From::value == _To::value>
    struct _AllConvertible : false_type
    { };

  template<typename... _From, typename... _To>
    struct _AllConvertible<_Pack<_From...>, _Pack<_To...>, true>
    : __and_<is_convertible<_From, _To>...>
    { };

  template<typename _Tp1, typename _Tp2>
    using _NotSame = __not_<is_same<typename std::decay<_Tp1>::type,
				    typename std::decay<_Tp2>::type>>;

  



 
  template<typename _Res, typename... _ArgTypes>
    struct _Maybe_unary_or_binary_function { };

  
  template<typename _Res, typename _T1>
    struct _Maybe_unary_or_binary_function<_Res, _T1>
    : std::unary_function<_T1, _Res> { };

  
  template<typename _Res, typename _T1, typename _T2>
    struct _Maybe_unary_or_binary_function<_Res, _T1, _T2>
    : std::binary_function<_T1, _T2, _Res> { };

  
  template<typename _Res, typename _Class, typename... _ArgTypes>
    class _Mem_fn<_Res (_Class::*)(_ArgTypes...)>
    : public _Maybe_unary_or_binary_function<_Res, _Class*, _ArgTypes...>
    {
      typedef _Res (_Class::*_Functor)(_ArgTypes...);

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __object, const volatile _Class *,
		_Args&&... __args) const
	{
	  return (std::forward<_Tp>(__object).*__pmf)
	    (std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __ptr, const volatile void *, _Args&&... __args) const
	{ return ((*__ptr).*__pmf)(std::forward<_Args>(__args)...); }

      
      template<typename... _Args>
	using _RequireValidArgs
	  = _Require<_AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      
      
      template<typename _Tp, typename... _Args>
	using _RequireValidArgs2
	  = _Require<_NotSame<_Class, _Tp>, _NotSame<_Class*, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      
      
      template<typename _Tp, typename... _Args>
	using _RequireValidArgs3
	  = _Require<is_base_of<_Class, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

    public:
      typedef _Res result_type;

      explicit _Mem_fn(_Functor __pmf) : __pmf(__pmf) { }

      
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(_Class& __object, _Args&&... __args) const
	{ return (__object.*__pmf)(std::forward<_Args>(__args)...); }

      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(_Class&& __object, _Args&&... __args) const
	{
	  return (std::move(__object).*__pmf)(std::forward<_Args>(__args)...);
	}

      
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(_Class* __object, _Args&&... __args) const
	{ return (__object->*__pmf)(std::forward<_Args>(__args)...); }

      
      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs2<_Tp, _Args...>>
	_Res
	operator()(_Tp&& __object, _Args&&... __args) const
	{
	  return _M_call(std::forward<_Tp>(__object), &__object,
	      std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs3<_Tp, _Args...>>
	_Res
	operator()(reference_wrapper<_Tp> __ref, _Args&&... __args) const
	{ return operator()(__ref.get(), std::forward<_Args>(__args)...); }

    private:
      _Functor __pmf;
    };

  
  template<typename _Res, typename _Class, typename... _ArgTypes>
    class _Mem_fn<_Res (_Class::*)(_ArgTypes...) const>
    : public _Maybe_unary_or_binary_function<_Res, const _Class*,
					     _ArgTypes...>
    {
      typedef _Res (_Class::*_Functor)(_ArgTypes...) const;

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __object, const volatile _Class *,
		_Args&&... __args) const
	{
	  return (std::forward<_Tp>(__object).*__pmf)
	    (std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __ptr, const volatile void *, _Args&&... __args) const
	{ return ((*__ptr).*__pmf)(std::forward<_Args>(__args)...); }

      template<typename... _Args>
	using _RequireValidArgs
	  = _Require<_AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      template<typename _Tp, typename... _Args>
	using _RequireValidArgs2
	  = _Require<_NotSame<_Class, _Tp>, _NotSame<const _Class*, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      template<typename _Tp, typename... _Args>
	using _RequireValidArgs3
	  = _Require<is_base_of<_Class, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

    public:
      typedef _Res result_type;

      explicit _Mem_fn(_Functor __pmf) : __pmf(__pmf) { }

      
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(const _Class& __object, _Args&&... __args) const
	{ return (__object.*__pmf)(std::forward<_Args>(__args)...); }

      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(const _Class&& __object, _Args&&... __args) const
	{
	  return (std::move(__object).*__pmf)(std::forward<_Args>(__args)...);
	}

      
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(const _Class* __object, _Args&&... __args) const
	{ return (__object->*__pmf)(std::forward<_Args>(__args)...); }

      
      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs2<_Tp, _Args...>>
	_Res operator()(_Tp&& __object, _Args&&... __args) const
	{
	  return _M_call(std::forward<_Tp>(__object), &__object,
	      std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs3<_Tp, _Args...>>
	_Res
	operator()(reference_wrapper<_Tp> __ref, _Args&&... __args) const
	{ return operator()(__ref.get(), std::forward<_Args>(__args)...); }

    private:
      _Functor __pmf;
    };

  
  template<typename _Res, typename _Class, typename... _ArgTypes>
    class _Mem_fn<_Res (_Class::*)(_ArgTypes...) volatile>
    : public _Maybe_unary_or_binary_function<_Res, volatile _Class*,
					     _ArgTypes...>
    {
      typedef _Res (_Class::*_Functor)(_ArgTypes...) volatile;

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __object, const volatile _Class *,
		_Args&&... __args) const
	{
	  return (std::forward<_Tp>(__object).*__pmf)
	    (std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __ptr, const volatile void *, _Args&&... __args) const
	{ return ((*__ptr).*__pmf)(std::forward<_Args>(__args)...); }

      template<typename... _Args>
	using _RequireValidArgs
	  = _Require<_AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      template<typename _Tp, typename... _Args>
	using _RequireValidArgs2
	  = _Require<_NotSame<_Class, _Tp>, _NotSame<volatile _Class*, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      template<typename _Tp, typename... _Args>
	using _RequireValidArgs3
	  = _Require<is_base_of<_Class, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

    public:
      typedef _Res result_type;

      explicit _Mem_fn(_Functor __pmf) : __pmf(__pmf) { }

      
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(volatile _Class& __object, _Args&&... __args) const
	{ return (__object.*__pmf)(std::forward<_Args>(__args)...); }

      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(volatile _Class&& __object, _Args&&... __args) const
	{
	  return (std::move(__object).*__pmf)(std::forward<_Args>(__args)...);
	}

      
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(volatile _Class* __object, _Args&&... __args) const
	{ return (__object->*__pmf)(std::forward<_Args>(__args)...); }

      
      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs2<_Tp, _Args...>>
	_Res
	operator()(_Tp&& __object, _Args&&... __args) const
	{
	  return _M_call(std::forward<_Tp>(__object), &__object,
	      std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs3<_Tp, _Args...>>
	_Res
	operator()(reference_wrapper<_Tp> __ref, _Args&&... __args) const
	{ return operator()(__ref.get(), std::forward<_Args>(__args)...); }

    private:
      _Functor __pmf;
    };

  
  template<typename _Res, typename _Class, typename... _ArgTypes>
    class _Mem_fn<_Res (_Class::*)(_ArgTypes...) const volatile>
    : public _Maybe_unary_or_binary_function<_Res, const volatile _Class*,
					     _ArgTypes...>
    {
      typedef _Res (_Class::*_Functor)(_ArgTypes...) const volatile;

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __object, const volatile _Class *,
		_Args&&... __args) const
	{
	  return (std::forward<_Tp>(__object).*__pmf)
	    (std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args>
	_Res
	_M_call(_Tp&& __ptr, const volatile void *, _Args&&... __args) const
	{ return ((*__ptr).*__pmf)(std::forward<_Args>(__args)...); }

      template<typename... _Args>
	using _RequireValidArgs
	  = _Require<_AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      template<typename _Tp, typename... _Args>
	using _RequireValidArgs2
	  = _Require<_NotSame<_Class, _Tp>,
		     _NotSame<const volatile _Class*, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

      template<typename _Tp, typename... _Args>
	using _RequireValidArgs3
	  = _Require<is_base_of<_Class, _Tp>,
		     _AllConvertible<_Pack<_Args...>, _Pack<_ArgTypes...>>>;

    public:
      typedef _Res result_type;

      explicit _Mem_fn(_Functor __pmf) : __pmf(__pmf) { }

      
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(const volatile _Class& __object, _Args&&... __args) const
	{ return (__object.*__pmf)(std::forward<_Args>(__args)...); }

      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(const volatile _Class&& __object, _Args&&... __args) const
	{
	  return (std::move(__object).*__pmf)(std::forward<_Args>(__args)...);
	}

      
      template<typename... _Args, typename _Req = _RequireValidArgs<_Args...>>
	_Res
	operator()(const volatile _Class* __object, _Args&&... __args) const
	{ return (__object->*__pmf)(std::forward<_Args>(__args)...); }

      
      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs2<_Tp, _Args...>>
	_Res operator()(_Tp&& __object, _Args&&... __args) const
	{
	  return _M_call(std::forward<_Tp>(__object), &__object,
	      std::forward<_Args>(__args)...);
	}

      template<typename _Tp, typename... _Args,
	       typename _Req = _RequireValidArgs3<_Tp, _Args...>>
	_Res
	operator()(reference_wrapper<_Tp> __ref, _Args&&... __args) const
	{ return operator()(__ref.get(), std::forward<_Args>(__args)...); }

    private:
      _Functor __pmf;
    };


  template<typename _Tp, bool>
    struct _Mem_fn_const_or_non
    {
      typedef const _Tp& type;
    };

  template<typename _Tp>
    struct _Mem_fn_const_or_non<_Tp, false>
    {
      typedef _Tp& type;
    };

  template<typename _Res, typename _Class>
    class _Mem_fn<_Res _Class::*>
    {
      using __pm_type = _Res _Class::*;

      
      
      
      template<typename _Tp>
	auto
	_M_call(_Tp&& __object, const _Class *) const noexcept
	-> decltype(std::forward<_Tp>(__object).*std::declval<__pm_type&>())
	{ return std::forward<_Tp>(__object).*__pm; }

      template<typename _Tp, typename _Up>
	auto
	_M_call(_Tp&& __object, _Up * const *) const noexcept
	-> decltype((*std::forward<_Tp>(__object)).*std::declval<__pm_type&>())
	{ return (*std::forward<_Tp>(__object)).*__pm; }

      template<typename _Tp>
	auto
	_M_call(_Tp&& __ptr, const volatile void*) const
	noexcept(noexcept((*__ptr).*std::declval<__pm_type&>()))
	-> decltype((*__ptr).*std::declval<__pm_type&>())
	{ return (*__ptr).*__pm; }

    public:
      explicit
      _Mem_fn(_Res _Class::*__pm) noexcept : __pm(__pm) { }

      
      _Res&
      operator()(_Class& __object) const noexcept
      { return __object.*__pm; }

      const _Res&
      operator()(const _Class& __object) const noexcept
      { return __object.*__pm; }

      _Res&&
      operator()(_Class&& __object) const noexcept
      { return std::forward<_Class>(__object).*__pm; }

      const _Res&&
      operator()(const _Class&& __object) const noexcept
      { return std::forward<const _Class>(__object).*__pm; }

      
      _Res&
      operator()(_Class* __object) const noexcept
      { return __object->*__pm; }

      const _Res&
      operator()(const _Class* __object) const noexcept
      { return __object->*__pm; }

      
      template<typename _Tp, typename _Req = _Require<_NotSame<_Class*, _Tp>>>
	auto
	operator()(_Tp&& __unknown) const
	noexcept(noexcept(std::declval<_Mem_fn*>()->_M_call
			  (std::forward<_Tp>(__unknown), &__unknown)))
	-> decltype(this->_M_call(std::forward<_Tp>(__unknown), &__unknown))
	{ return _M_call(std::forward<_Tp>(__unknown), &__unknown); }

      template<typename _Tp, typename _Req = _Require<is_base_of<_Class, _Tp>>>
	auto
	operator()(reference_wrapper<_Tp> __ref) const
	noexcept(noexcept(std::declval<_Mem_fn&>()(__ref.get())))
	-> decltype((*this)(__ref.get()))
	{ return (*this)(__ref.get()); }

    private:
      _Res _Class::*__pm;
    };

  
  
  



 
  template<typename _Tp, typename _Class>
    inline _Mem_fn<_Tp _Class::*>
    mem_fn(_Tp _Class::* __pm) noexcept
    {
      return _Mem_fn<_Tp _Class::*>(__pm);
    }

  




 
  template<typename _Tp>
    struct is_bind_expression
    : public false_type { };

  



 
  template<typename _Tp>
    struct is_placeholder
    : public integral_constant<int, 0>
    { };

  

 
  template<int _Num> struct _Placeholder { };

  

  


 
  namespace placeholders
  {
  
  


 
    extern const _Placeholder<1> _1;
    extern const _Placeholder<2> _2;
    extern const _Placeholder<3> _3;
    extern const _Placeholder<4> _4;
    extern const _Placeholder<5> _5;
    extern const _Placeholder<6> _6;
    extern const _Placeholder<7> _7;
    extern const _Placeholder<8> _8;
    extern const _Placeholder<9> _9;
    extern const _Placeholder<10> _10;
    extern const _Placeholder<11> _11;
    extern const _Placeholder<12> _12;
    extern const _Placeholder<13> _13;
    extern const _Placeholder<14> _14;
    extern const _Placeholder<15> _15;
    extern const _Placeholder<16> _16;
    extern const _Placeholder<17> _17;
    extern const _Placeholder<18> _18;
    extern const _Placeholder<19> _19;
    extern const _Placeholder<20> _20;
    extern const _Placeholder<21> _21;
    extern const _Placeholder<22> _22;
    extern const _Placeholder<23> _23;
    extern const _Placeholder<24> _24;
    extern const _Placeholder<25> _25;
    extern const _Placeholder<26> _26;
    extern const _Placeholder<27> _27;
    extern const _Placeholder<28> _28;
    extern const _Placeholder<29> _29;
  
  }

  

  



 
  template<int _Num>
    struct is_placeholder<_Placeholder<_Num> >
    : public integral_constant<int, _Num>
    { };

  template<int _Num>
    struct is_placeholder<const _Placeholder<_Num> >
    : public integral_constant<int, _Num>
    { };

  


 
  struct _No_tuple_element;

  



 
  template<std::size_t __i, typename _Tuple, bool _IsSafe>
    struct _Safe_tuple_element_impl
    : tuple_element<__i, _Tuple> { };

  



 
  template<std::size_t __i, typename _Tuple>
    struct _Safe_tuple_element_impl<__i, _Tuple, false>
    {
      typedef _No_tuple_element type;
    };

  


 
 template<std::size_t __i, typename _Tuple>
   struct _Safe_tuple_element
   : _Safe_tuple_element_impl<__i, _Tuple,
			      (__i < tuple_size<_Tuple>::value)>
   { };

  









 
  template<typename _Arg,
	   bool _IsBindExp = is_bind_expression<_Arg>::value,
	   bool _IsPlaceholder = (is_placeholder<_Arg>::value > 0)>
    class _Mu;

  


 
  template<typename _Tp>
    class _Mu<reference_wrapper<_Tp>, false, false>
    {
    public:
      typedef _Tp& result_type;

      


 
      template<typename _CVRef, typename _Tuple>
	result_type
	operator()(_CVRef& __arg, _Tuple&) const volatile
	{ return __arg.get(); }
    };

  



 
  template<typename _Arg>
    class _Mu<_Arg, true, false>
    {
    public:
      template<typename _CVArg, typename... _Args>
	auto
	operator()(_CVArg& __arg,
		   tuple<_Args...>& __tuple) const volatile
	-> decltype(__arg(declval<_Args>()...))
	{
	  
	  typedef typename _Build_index_tuple<sizeof...(_Args)>::__type
	    _Indexes;
	  return this->__call(__arg, __tuple, _Indexes());
	}

    private:
      
      
      template<typename _CVArg, typename... _Args, std::size_t... _Indexes>
	auto
	__call(_CVArg& __arg, tuple<_Args...>& __tuple,
	       const _Index_tuple<_Indexes...>&) const volatile
	-> decltype(__arg(declval<_Args>()...))
	{
	  return __arg(std::forward<_Args>(get<_Indexes>(__tuple))...);
	}
    };

  



 
  template<typename _Arg>
    class _Mu<_Arg, false, true>
    {
    public:
      template<typename _Signature> class result;

      template<typename _CVMu, typename _CVArg, typename _Tuple>
	class result<_CVMu(_CVArg, _Tuple)>
	{
	  
	  
	  
	  typedef typename _Safe_tuple_element<(is_placeholder<_Arg>::value
						- 1), _Tuple>::type
	    __base_type;

	public:
	  typedef typename add_rvalue_reference<__base_type>::type type;
	};

      template<typename _Tuple>
	typename result<_Mu(_Arg, _Tuple)>::type
	operator()(const volatile _Arg&, _Tuple& __tuple) const volatile
	{
	  return std::forward<typename result<_Mu(_Arg, _Tuple)>::type>(
	      ::std::get<(is_placeholder<_Arg>::value - 1)>(__tuple));
	}
    };

  



 
  template<typename _Arg>
    class _Mu<_Arg, false, false>
    {
    public:
      template<typename _Signature> struct result;

      template<typename _CVMu, typename _CVArg, typename _Tuple>
	struct result<_CVMu(_CVArg, _Tuple)>
	{
	  typedef typename add_lvalue_reference<_CVArg>::type type;
	};

      
      template<typename _CVArg, typename _Tuple>
	_CVArg&&
	operator()(_CVArg&& __arg, _Tuple&) const volatile
	{ return std::forward<_CVArg>(__arg); }
    };

  



 
  template<typename _Tp>
    struct _Maybe_wrap_member_pointer
    {
      typedef _Tp type;

      static const _Tp&
      __do_wrap(const _Tp& __x)
      { return __x; }

      static _Tp&&
      __do_wrap(_Tp&& __x)
      { return static_cast<_Tp&&>(__x); }
    };

  



 
  template<typename _Tp, typename _Class>
    struct _Maybe_wrap_member_pointer<_Tp _Class::*>
    {
      typedef _Mem_fn<_Tp _Class::*> type;

      static type
      __do_wrap(_Tp _Class::* __pm)
      { return type(__pm); }
    };

  
  
  
  
  template<>
    struct _Maybe_wrap_member_pointer<void>
    {
      typedef void type;
    };

  
  template<std::size_t _Ind, typename... _Tp>
    inline auto
    __volget(volatile tuple<_Tp...>& __tuple)
    -> typename tuple_element<_Ind, tuple<_Tp...>>::type volatile&
    { return std::get<_Ind>(const_cast<tuple<_Tp...>&>(__tuple)); }

  
  template<std::size_t _Ind, typename... _Tp>
    inline auto
    __volget(const volatile tuple<_Tp...>& __tuple)
    -> typename tuple_element<_Ind, tuple<_Tp...>>::type const volatile&
    { return std::get<_Ind>(const_cast<const tuple<_Tp...>&>(__tuple)); }

  
  template<typename _Signature>
    struct _Bind;

   template<typename _Functor, typename... _Bound_args>
    class _Bind<_Functor(_Bound_args...)>
    : public _Weak_result_type<_Functor>
    {
      typedef _Bind __self_type;
      typedef typename _Build_index_tuple<sizeof...(_Bound_args)>::__type
	_Bound_indexes;

      _Functor _M_f;
      tuple<_Bound_args...> _M_bound_args;

      
      template<typename _Result, typename... _Args, std::size_t... _Indexes>
	_Result
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>)
	{
	  return _M_f(_Mu<_Bound_args>()
		      (get<_Indexes>(_M_bound_args), __args)...);
	}

      
      template<typename _Result, typename... _Args, std::size_t... _Indexes>
	_Result
	__call_c(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>) const
	{
	  return _M_f(_Mu<_Bound_args>()
		      (get<_Indexes>(_M_bound_args), __args)...);
	}

      
      template<typename _Result, typename... _Args, std::size_t... _Indexes>
	_Result
	__call_v(tuple<_Args...>&& __args,
		 _Index_tuple<_Indexes...>) volatile
	{
	  return _M_f(_Mu<_Bound_args>()
		      (__volget<_Indexes>(_M_bound_args), __args)...);
	}

      
      template<typename _Result, typename... _Args, std::size_t... _Indexes>
	_Result
	__call_c_v(tuple<_Args...>&& __args,
		   _Index_tuple<_Indexes...>) const volatile
	{
	  return _M_f(_Mu<_Bound_args>()
		      (__volget<_Indexes>(_M_bound_args), __args)...);
	}

     public:
      template<typename... _Args>
	explicit _Bind(const _Functor& __f, _Args&&... __args)
	: _M_f(__f), _M_bound_args(std::forward<_Args>(__args)...)
	{ }

      template<typename... _Args>
	explicit _Bind(_Functor&& __f, _Args&&... __args)
	: _M_f(std::move(__f)), _M_bound_args(std::forward<_Args>(__args)...)
	{ }

      _Bind(const _Bind&) = default;

      _Bind(_Bind&& __b)
      : _M_f(std::move(__b._M_f)), _M_bound_args(std::move(__b._M_bound_args))
      { }

      
      template<typename... _Args, typename _Result
	= decltype( std::declval<_Functor>()(
	      _Mu<_Bound_args>()( std::declval<_Bound_args&>(),
				  std::declval<tuple<_Args...>&>() )... ) )>
	_Result
	operator()(_Args&&... __args)
	{
	  return this->__call<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}

      
      template<typename... _Args, typename _Result
	= decltype( std::declval<typename enable_if<(sizeof...(_Args) >= 0),
		       typename add_const<_Functor>::type>::type>()(
	      _Mu<_Bound_args>()( std::declval<const _Bound_args&>(),
				  std::declval<tuple<_Args...>&>() )... ) )>
	_Result
	operator()(_Args&&... __args) const
	{
	  return this->__call_c<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}

      
      template<typename... _Args, typename _Result
	= decltype( std::declval<typename enable_if<(sizeof...(_Args) >= 0),
                       typename add_volatile<_Functor>::type>::type>()(
	      _Mu<_Bound_args>()( std::declval<volatile _Bound_args&>(),
				  std::declval<tuple<_Args...>&>() )... ) )>
	_Result
	operator()(_Args&&... __args) volatile
	{
	  return this->__call_v<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}

      
      template<typename... _Args, typename _Result
	= decltype( std::declval<typename enable_if<(sizeof...(_Args) >= 0),
                       typename add_cv<_Functor>::type>::type>()(
	      _Mu<_Bound_args>()( std::declval<const volatile _Bound_args&>(),
				  std::declval<tuple<_Args...>&>() )... ) )>
	_Result
	operator()(_Args&&... __args) const volatile
	{
	  return this->__call_c_v<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}
    };

  
  template<typename _Result, typename _Signature>
    struct _Bind_result;

  template<typename _Result, typename _Functor, typename... _Bound_args>
    class _Bind_result<_Result, _Functor(_Bound_args...)>
    {
      typedef _Bind_result __self_type;
      typedef typename _Build_index_tuple<sizeof...(_Bound_args)>::__type
	_Bound_indexes;

      _Functor _M_f;
      tuple<_Bound_args...> _M_bound_args;

      
      template<typename _Res>
	struct __enable_if_void : enable_if<is_void<_Res>::value, int> { };
      template<typename _Res>
	struct __disable_if_void : enable_if<!is_void<_Res>::value, int> { };

      
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	_Result
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>,
	    typename __disable_if_void<_Res>::type = 0)
	{
	  return _M_f(_Mu<_Bound_args>()
		      (get<_Indexes>(_M_bound_args), __args)...);
	}

      
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	void
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>,
	    typename __enable_if_void<_Res>::type = 0)
	{
	  _M_f(_Mu<_Bound_args>()
	       (get<_Indexes>(_M_bound_args), __args)...);
	}

      
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	_Result
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>,
	    typename __disable_if_void<_Res>::type = 0) const
	{
	  return _M_f(_Mu<_Bound_args>()
		      (get<_Indexes>(_M_bound_args), __args)...);
	}

      
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	void
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>,
	    typename __enable_if_void<_Res>::type = 0) const
	{
	  _M_f(_Mu<_Bound_args>()
	       (get<_Indexes>(_M_bound_args),  __args)...);
	}

      
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	_Result
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>,
	    typename __disable_if_void<_Res>::type = 0) volatile
	{
	  return _M_f(_Mu<_Bound_args>()
		      (__volget<_Indexes>(_M_bound_args), __args)...);
	}

      
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	void
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>,
	    typename __enable_if_void<_Res>::type = 0) volatile
	{
	  _M_f(_Mu<_Bound_args>()
	       (__volget<_Indexes>(_M_bound_args), __args)...);
	}

      
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	_Result
	__call(tuple<_Args...>&& __args, _Index_tuple<_Indexes...>,
	    typename __disable_if_void<_Res>::type = 0) const volatile
	{
	  return _M_f(_Mu<_Bound_args>()
		      (__volget<_Indexes>(_M_bound_args), __args)...);
	}

      
      template<typename _Res, typename... _Args, std::size_t... _Indexes>
	void
	__call(tuple<_Args...>&& __args,
	       _Index_tuple<_Indexes...>,
	    typename __enable_if_void<_Res>::type = 0) const volatile
	{
	  _M_f(_Mu<_Bound_args>()
	       (__volget<_Indexes>(_M_bound_args), __args)...);
	}

    public:
      typedef _Result result_type;

      template<typename... _Args>
	explicit _Bind_result(const _Functor& __f, _Args&&... __args)
	: _M_f(__f), _M_bound_args(std::forward<_Args>(__args)...)
	{ }

      template<typename... _Args>
	explicit _Bind_result(_Functor&& __f, _Args&&... __args)
	: _M_f(std::move(__f)), _M_bound_args(std::forward<_Args>(__args)...)
	{ }

      _Bind_result(const _Bind_result&) = default;

      _Bind_result(_Bind_result&& __b)
      : _M_f(std::move(__b._M_f)), _M_bound_args(std::move(__b._M_bound_args))
      { }

      
      template<typename... _Args>
	result_type
	operator()(_Args&&... __args)
	{
	  return this->__call<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}

      
      template<typename... _Args>
	result_type
	operator()(_Args&&... __args) const
	{
	  return this->__call<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}

      
      template<typename... _Args>
	result_type
	operator()(_Args&&... __args) volatile
	{
	  return this->__call<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}

      
      template<typename... _Args>
	result_type
	operator()(_Args&&... __args) const volatile
	{
	  return this->__call<_Result>(
	      std::forward_as_tuple(std::forward<_Args>(__args)...),
	      _Bound_indexes());
	}
    };

  


 
  template<typename _Signature>
    struct is_bind_expression<_Bind<_Signature> >
    : public true_type { };

  


 
  template<typename _Signature>
    struct is_bind_expression<const _Bind<_Signature> >
    : public true_type { };

  


 
  template<typename _Signature>
    struct is_bind_expression<volatile _Bind<_Signature> >
    : public true_type { };

  


 
  template<typename _Signature>
    struct is_bind_expression<const volatile _Bind<_Signature>>
    : public true_type { };

  


 
  template<typename _Result, typename _Signature>
    struct is_bind_expression<_Bind_result<_Result, _Signature>>
    : public true_type { };

  


 
  template<typename _Result, typename _Signature>
    struct is_bind_expression<const _Bind_result<_Result, _Signature>>
    : public true_type { };

  


 
  template<typename _Result, typename _Signature>
    struct is_bind_expression<volatile _Bind_result<_Result, _Signature>>
    : public true_type { };

  


 
  template<typename _Result, typename _Signature>
    struct is_bind_expression<const volatile _Bind_result<_Result, _Signature>>
    : public true_type { };

  
  
  
  template<typename _Tp, typename _Tp2 = typename decay<_Tp>::type>
    using __is_socketlike = __or_<is_integral<_Tp2>, is_enum<_Tp2>>;

  template<bool _SocketLike, typename _Func, typename... _BoundArgs>
    struct _Bind_helper
    {
      typedef _Maybe_wrap_member_pointer<typename decay<_Func>::type>
	__maybe_type;
      typedef typename __maybe_type::type __func_type;
      typedef _Bind<__func_type(typename decay<_BoundArgs>::type...)> type;
    };

  
  
  
  template<typename _Func, typename... _BoundArgs>
    struct _Bind_helper<true, _Func, _BoundArgs...>
    { };

  


 
  template<typename _Func, typename... _BoundArgs>
    inline typename
    _Bind_helper<__is_socketlike<_Func>::value, _Func, _BoundArgs...>::type
    bind(_Func&& __f, _BoundArgs&&... __args)
    {
      typedef _Bind_helper<false, _Func, _BoundArgs...> __helper_type;
      typedef typename __helper_type::__maybe_type __maybe_type;
      typedef typename __helper_type::type __result_type;
      return __result_type(__maybe_type::__do_wrap(std::forward<_Func>(__f)),
			   std::forward<_BoundArgs>(__args)...);
    }

  template<typename _Result, typename _Func, typename... _BoundArgs>
    struct _Bindres_helper
    {
      typedef _Maybe_wrap_member_pointer<typename decay<_Func>::type>
	__maybe_type;
      typedef typename __maybe_type::type __functor_type;
      typedef _Bind_result<_Result,
			   __functor_type(typename decay<_BoundArgs>::type...)>
	type;
    };

  


 
  template<typename _Result, typename _Func, typename... _BoundArgs>
    inline
    typename _Bindres_helper<_Result, _Func, _BoundArgs...>::type
    bind(_Func&& __f, _BoundArgs&&... __args)
    {
      typedef _Bindres_helper<_Result, _Func, _BoundArgs...> __helper_type;
      typedef typename __helper_type::__maybe_type __maybe_type;
      typedef typename __helper_type::type __result_type;
      return __result_type(__maybe_type::__do_wrap(std::forward<_Func>(__f)),
			   std::forward<_BoundArgs>(__args)...);
    }

  template<typename _Signature>
    struct _Bind_simple;

  template<typename _Callable, typename... _Args>
    struct _Bind_simple<_Callable(_Args...)>
    {
      typedef typename result_of<_Callable(_Args...)>::type result_type;

      template<typename... _Args2, typename = typename
               enable_if< sizeof...(_Args) == sizeof...(_Args2)>::type>
        explicit
        _Bind_simple(const _Callable& __callable, _Args2&&... __args)
        : _M_bound(__callable, std::forward<_Args2>(__args)...)
        { }

      template<typename... _Args2, typename = typename
               enable_if< sizeof...(_Args) == sizeof...(_Args2)>::type>
        explicit
        _Bind_simple(_Callable&& __callable, _Args2&&... __args)
        : _M_bound(std::move(__callable), std::forward<_Args2>(__args)...)
        { }

      _Bind_simple(const _Bind_simple&) = default;
      _Bind_simple(_Bind_simple&&) = default;

      result_type
      operator()()
      {
        typedef typename _Build_index_tuple<sizeof...(_Args)>::__type _Indices;
        return _M_invoke(_Indices());
      }

    private:

      template<std::size_t... _Indices>
        typename result_of<_Callable(_Args...)>::type
        _M_invoke(_Index_tuple<_Indices...>)
        {
	  
	  
          return std::forward<_Callable>(std::get<0>(_M_bound))(
              std::forward<_Args>(std::get<_Indices+1>(_M_bound))...);
        }

      std::tuple<_Callable, _Args...> _M_bound;
    };

  template<typename _Func, typename... _BoundArgs>
    struct _Bind_simple_helper
    {
      typedef _Maybe_wrap_member_pointer<typename decay<_Func>::type>
        __maybe_type;
      typedef typename __maybe_type::type __func_type;
      typedef _Bind_simple<__func_type(typename decay<_BoundArgs>::type...)>
       	__type;
    };

  
  
  template<typename _Callable, typename... _Args>
    typename _Bind_simple_helper<_Callable, _Args...>::__type
    __bind_simple(_Callable&& __callable, _Args&&... __args)
    {
      typedef _Bind_simple_helper<_Callable, _Args...> __helper_type;
      typedef typename __helper_type::__maybe_type __maybe_type;
      typedef typename __helper_type::__type __result_type;
      return __result_type(
          __maybe_type::__do_wrap( std::forward<_Callable>(__callable)),
          std::forward<_Args>(__args)...);
    }

  



 
  class bad_function_call : public std::exception
  {
  public:
    virtual ~bad_function_call() noexcept;

    const char* what() const noexcept;
  };

  



 
  template<typename _Tp>
    struct __is_location_invariant
    : integral_constant<bool, (is_pointer<_Tp>::value
			       || is_member_pointer<_Tp>::value)>
    { };

  class _Undefined_class;

  union _Nocopy_types
  {
    void*       _M_object;
    const void* _M_const_object;
    void (*_M_function_pointer)();
    void (_Undefined_class::*_M_member_pointer)();
  };

  union _Any_data
  {
    void*       _M_access()       { return &_M_pod_data[0]; }
    const void* _M_access() const { return &_M_pod_data[0]; }

    template<typename _Tp>
      _Tp&
      _M_access()
      { return *static_cast<_Tp*>(_M_access()); }

    template<typename _Tp>
      const _Tp&
      _M_access() const
      { return *static_cast<const _Tp*>(_M_access()); }

    _Nocopy_types _M_unused;
    char _M_pod_data[sizeof(_Nocopy_types)];
  };

  enum _Manager_operation
  {
    __get_type_info,
    __get_functor_ptr,
    __clone_functor,
    __destroy_functor
  };

  
  
  template<typename _Tp>
    struct _Simple_type_wrapper
    {
      _Simple_type_wrapper(_Tp __value) : __value(__value) { }

      _Tp __value;
    };

  template<typename _Tp>
    struct __is_location_invariant<_Simple_type_wrapper<_Tp> >
    : __is_location_invariant<_Tp>
    { };

  
  
  template<typename _Functor>
    inline _Functor&
    __callable_functor(_Functor& __f)
    { return __f; }

  template<typename _Member, typename _Class>
    inline _Mem_fn<_Member _Class::*>
    __callable_functor(_Member _Class::* &__p)
    { return std::mem_fn(__p); }

  template<typename _Member, typename _Class>
    inline _Mem_fn<_Member _Class::*>
    __callable_functor(_Member _Class::* const &__p)
    { return std::mem_fn(__p); }

  template<typename _Member, typename _Class>
    inline _Mem_fn<_Member _Class::*>
    __callable_functor(_Member _Class::* volatile &__p)
    { return std::mem_fn(__p); }

  template<typename _Member, typename _Class>
    inline _Mem_fn<_Member _Class::*>
    __callable_functor(_Member _Class::* const volatile &__p)
    { return std::mem_fn(__p); }

  template<typename _Signature>
    class function;

  
  class _Function_base
  {
  public:
    static const std::size_t _M_max_size = sizeof(_Nocopy_types);
    static const std::size_t _M_max_align = __alignof__(_Nocopy_types);

    template<typename _Functor>
      class _Base_manager
      {
      protected:
	static const bool __stored_locally =
	(__is_location_invariant<_Functor>::value
	 && sizeof(_Functor) <= _M_max_size
	 && __alignof__(_Functor) <= _M_max_align
	 && (_M_max_align % __alignof__(_Functor) == 0));

	typedef integral_constant<bool, __stored_locally> _Local_storage;

	
	static _Functor*
	_M_get_pointer(const _Any_data& __source)
	{
	  const _Functor* __ptr =
	    __stored_locally? std::__addressof(__source._M_access<_Functor>())
	      : __source._M_access<_Functor*>();
	  return const_cast<_Functor*>(__ptr);
	}

	
	
	static void
	_M_clone(_Any_data& __dest, const _Any_data& __source, true_type)
	{
	  new (__dest._M_access()) _Functor(__source._M_access<_Functor>());
	}

	
	
	static void
	_M_clone(_Any_data& __dest, const _Any_data& __source, false_type)
	{
	  __dest._M_access<_Functor*>() =
	    new _Functor(*__source._M_access<_Functor*>());
	}

	
	
	static void
	_M_destroy(_Any_data& __victim, true_type)
	{
	  __victim._M_access<_Functor>().~_Functor();
	}

	
	static void
	_M_destroy(_Any_data& __victim, false_type)
	{
	  delete __victim._M_access<_Functor*>();
	}

      public:
	static bool
	_M_manager(_Any_data& __dest, const _Any_data& __source,
		   _Manager_operation __op)
	{
	  switch (__op)
	    {
	    case __get_type_info:
	      __dest._M_access<const type_info*>() = &typeid(_Functor);
	      break;
	    case __get_functor_ptr:
	      __dest._M_access<_Functor*>() = _M_get_pointer(__source);
	      break;

	    case __clone_functor:
	      _M_clone(__dest, __source, _Local_storage());
	      break;

	    case __destroy_functor:
	      _M_destroy(__dest, _Local_storage());
	      break;
	    }
	  return false;
	}

	static void
	_M_init_functor(_Any_data& __functor, _Functor&& __f)
	{ _M_init_functor(__functor, std::move(__f), _Local_storage()); }

	template<typename _Signature>
	  static bool
	  _M_not_empty_function(const function<_Signature>& __f)
	  { return static_cast<bool>(__f); }

	template<typename _Tp>
	  static bool
	  _M_not_empty_function(_Tp* const& __fp)
	  { return __fp; }

	template<typename _Class, typename _Tp>
	  static bool
	  _M_not_empty_function(_Tp _Class::* const& __mp)
	  { return __mp; }

	template<typename _Tp>
	  static bool
	  _M_not_empty_function(const _Tp&)
	  { return true; }

      private:
	static void
	_M_init_functor(_Any_data& __functor, _Functor&& __f, true_type)
	{ new (__functor._M_access()) _Functor(std::move(__f)); }

	static void
	_M_init_functor(_Any_data& __functor, _Functor&& __f, false_type)
	{ __functor._M_access<_Functor*>() = new _Functor(std::move(__f)); }
      };

    template<typename _Functor>
      class _Ref_manager : public _Base_manager<_Functor*>
      {
	typedef _Function_base::_Base_manager<_Functor*> _Base;

      public:
	static bool
	_M_manager(_Any_data& __dest, const _Any_data& __source,
		   _Manager_operation __op)
	{
	  switch (__op)
	    {
	    case __get_type_info:
	      __dest._M_access<const type_info*>() = &typeid(_Functor);
	      break;
	    case __get_functor_ptr:
	      __dest._M_access<_Functor*>() = *_Base::_M_get_pointer(__source);
	      return is_const<_Functor>::value;
	      break;

	    default:
	      _Base::_M_manager(__dest, __source, __op);
	    }
	  return false;
	}

	static void
	_M_init_functor(_Any_data& __functor, reference_wrapper<_Functor> __f)
	{
	  _Base::_M_init_functor(__functor, std::__addressof(__f.get()));
	}
      };

    _Function_base() : _M_manager(0) { }

    ~_Function_base()
    {
      if (_M_manager)
	_M_manager(_M_functor, _M_functor, __destroy_functor);
    }


    bool _M_empty() const { return !_M_manager; }

    typedef bool (*_Manager_type)(_Any_data&, const _Any_data&,
				  _Manager_operation);

    _Any_data     _M_functor;
    _Manager_type _M_manager;
  };

  template<typename _Signature, typename _Functor>
    class _Function_handler;

  template<typename _Res, typename _Functor, typename... _ArgTypes>
    class _Function_handler<_Res(_ArgTypes...), _Functor>
    : public _Function_base::_Base_manager<_Functor>
    {
      typedef _Function_base::_Base_manager<_Functor> _Base;

    public:
      static _Res
      _M_invoke(const _Any_data& __functor, _ArgTypes... __args)
      {
	return (*_Base::_M_get_pointer(__functor))(
	    std::forward<_ArgTypes>(__args)...);
      }
    };

  template<typename _Functor, typename... _ArgTypes>
    class _Function_handler<void(_ArgTypes...), _Functor>
    : public _Function_base::_Base_manager<_Functor>
    {
      typedef _Function_base::_Base_manager<_Functor> _Base;

     public:
      static void
      _M_invoke(const _Any_data& __functor, _ArgTypes... __args)
      {
	(*_Base::_M_get_pointer(__functor))(
	    std::forward<_ArgTypes>(__args)...);
      }
    };

  template<typename _Res, typename _Functor, typename... _ArgTypes>
    class _Function_handler<_Res(_ArgTypes...), reference_wrapper<_Functor> >
    : public _Function_base::_Ref_manager<_Functor>
    {
      typedef _Function_base::_Ref_manager<_Functor> _Base;

     public:
      static _Res
      _M_invoke(const _Any_data& __functor, _ArgTypes... __args)
      {
	return __callable_functor(**_Base::_M_get_pointer(__functor))(
	      std::forward<_ArgTypes>(__args)...);
      }
    };

  template<typename _Functor, typename... _ArgTypes>
    class _Function_handler<void(_ArgTypes...), reference_wrapper<_Functor> >
    : public _Function_base::_Ref_manager<_Functor>
    {
      typedef _Function_base::_Ref_manager<_Functor> _Base;

     public:
      static void
      _M_invoke(const _Any_data& __functor, _ArgTypes... __args)
      {
	__callable_functor(**_Base::_M_get_pointer(__functor))(
	    std::forward<_ArgTypes>(__args)...);
      }
    };

  template<typename _Class, typename _Member, typename _Res,
	   typename... _ArgTypes>
    class _Function_handler<_Res(_ArgTypes...), _Member _Class::*>
    : public _Function_handler<void(_ArgTypes...), _Member _Class::*>
    {
      typedef _Function_handler<void(_ArgTypes...), _Member _Class::*>
	_Base;

     public:
      static _Res
      _M_invoke(const _Any_data& __functor, _ArgTypes... __args)
      {
	return std::mem_fn(_Base::_M_get_pointer(__functor)->__value)(
	    std::forward<_ArgTypes>(__args)...);
      }
    };

  template<typename _Class, typename _Member, typename... _ArgTypes>
    class _Function_handler<void(_ArgTypes...), _Member _Class::*>
    : public _Function_base::_Base_manager<
		 _Simple_type_wrapper< _Member _Class::* > >
    {
      typedef _Member _Class::* _Functor;
      typedef _Simple_type_wrapper<_Functor> _Wrapper;
      typedef _Function_base::_Base_manager<_Wrapper> _Base;

    public:
      static bool
      _M_manager(_Any_data& __dest, const _Any_data& __source,
		 _Manager_operation __op)
      {
	switch (__op)
	  {
	  case __get_type_info:
	    __dest._M_access<const type_info*>() = &typeid(_Functor);
	    break;
	  case __get_functor_ptr:
	    __dest._M_access<_Functor*>() =
	      &_Base::_M_get_pointer(__source)->__value;
	    break;

	  default:
	    _Base::_M_manager(__dest, __source, __op);
	  }
	return false;
      }

      static void
      _M_invoke(const _Any_data& __functor, _ArgTypes... __args)
      {
	std::mem_fn(_Base::_M_get_pointer(__functor)->__value)(
	    std::forward<_ArgTypes>(__args)...);
      }
    };

  template<typename _From, typename _To>
    using __check_func_return_type
      = __or_<is_void<_To>, is_convertible<_From, _To>>;

  




 
  template<typename _Res, typename... _ArgTypes>
    class function<_Res(_ArgTypes...)>
    : public _Maybe_unary_or_binary_function<_Res, _ArgTypes...>,
      private _Function_base
    {
      typedef _Res _Signature_type(_ArgTypes...);

      template<typename _Functor>
	using _Invoke = decltype(__callable_functor(std::declval<_Functor&>())
				 (std::declval<_ArgTypes>()...) );

      
      
      template<typename _Tp>
	using _NotSelf = __not_<is_same<_Tp, function>>;

      template<typename _Functor>
	using _Callable
	  = __and_<_NotSelf<_Functor>,
		   __check_func_return_type<_Invoke<_Functor>, _Res>>;

      template<typename _Cond, typename _Tp>
	using _Requires = typename enable_if<_Cond::value, _Tp>::type;

    public:
      typedef _Res result_type;

      

      


 
      function() noexcept
      : _Function_base() { }

      


 
      function(nullptr_t) noexcept
      : _Function_base() { }

      






 
      function(const function& __x);

      





 
      function(function&& __x) : _Function_base()
      {
	__x.swap(*this);
      }

      

      














 
      template<typename _Functor,
	       typename = _Requires<_Callable<_Functor>, void>>
	function(_Functor);

      










 
      function&
      operator=(const function& __x)
      {
	function(__x).swap(*this);
	return *this;
      }

      









 
      function&
      operator=(function&& __x)
      {
	function(std::move(__x)).swap(*this);
	return *this;
      }

      





 
      function&
      operator=(nullptr_t)
      {
	if (_M_manager)
	  {
	    _M_manager(_M_functor, _M_functor, __destroy_functor);
	    _M_manager = 0;
	    _M_invoker = 0;
	  }
	return *this;
      }

      














 
      template<typename _Functor>
	_Requires<_Callable<typename decay<_Functor>::type>, function&>
	operator=(_Functor&& __f)
	{
	  function(std::forward<_Functor>(__f)).swap(*this);
	  return *this;
	}

      
      template<typename _Functor>
	function&
	operator=(reference_wrapper<_Functor> __f) noexcept
	{
	  function(__f).swap(*this);
	  return *this;
	}

      

      





 
      void swap(function& __x)
      {
	std::swap(_M_functor, __x._M_functor);
	std::swap(_M_manager, __x._M_manager);
	std::swap(_M_invoker, __x._M_invoker);
      }

      
      







 

      

      






 
      explicit operator bool() const noexcept
      { return !_M_empty(); }

      

      






 
      _Res operator()(_ArgTypes... __args) const;

      
      







 
      const type_info& target_type() const noexcept;

      







 
      template<typename _Functor>       _Functor* target() noexcept;

      
      template<typename _Functor> const _Functor* target() const noexcept;

    private:
      typedef _Res (*_Invoker_type)(const _Any_data&, _ArgTypes...);
      _Invoker_type _M_invoker;
  };

  
  template<typename _Res, typename... _ArgTypes>
    function<_Res(_ArgTypes...)>::
    function(const function& __x)
    : _Function_base()
    {
      if (static_cast<bool>(__x))
	{
	  _M_invoker = __x._M_invoker;
	  _M_manager = __x._M_manager;
	  __x._M_manager(_M_functor, __x._M_functor, __clone_functor);
	}
    }

  template<typename _Res, typename... _ArgTypes>
    template<typename _Functor, typename>
      function<_Res(_ArgTypes...)>::
      function(_Functor __f)
      : _Function_base()
      {
	typedef _Function_handler<_Signature_type, _Functor> _My_handler;

	if (_My_handler::_M_not_empty_function(__f))
	  {
	    _My_handler::_M_init_functor(_M_functor, std::move(__f));
	    _M_invoker = &_My_handler::_M_invoke;
	    _M_manager = &_My_handler::_M_manager;
	  }
      }

  template<typename _Res, typename... _ArgTypes>
    _Res
    function<_Res(_ArgTypes...)>::
    operator()(_ArgTypes... __args) const
    {
      if (_M_empty())
	__throw_bad_function_call();
      return _M_invoker(_M_functor, std::forward<_ArgTypes>(__args)...);
    }

  template<typename _Res, typename... _ArgTypes>
    const type_info&
    function<_Res(_ArgTypes...)>::
    target_type() const noexcept
    {
      if (_M_manager)
	{
	  _Any_data __typeinfo_result;
	  _M_manager(__typeinfo_result, _M_functor, __get_type_info);
	  return *__typeinfo_result._M_access<const type_info*>();
	}
      else
	return typeid(void);
    }

  template<typename _Res, typename... _ArgTypes>
    template<typename _Functor>
      _Functor*
      function<_Res(_ArgTypes...)>::
      target() noexcept
      {
	if (typeid(_Functor) == target_type() && _M_manager)
	  {
	    _Any_data __ptr;
	    if (_M_manager(__ptr, _M_functor, __get_functor_ptr)
		&& !is_const<_Functor>::value)
	      return 0;
	    else
	      return __ptr._M_access<_Functor*>();
	  }
	else
	  return 0;
      }

  template<typename _Res, typename... _ArgTypes>
    template<typename _Functor>
      const _Functor*
      function<_Res(_ArgTypes...)>::
      target() const noexcept
      {
	if (typeid(_Functor) == target_type() && _M_manager)
	  {
	    _Any_data __ptr;
	    _M_manager(__ptr, _M_functor, __get_functor_ptr);
	    return __ptr._M_access<const _Functor*>();
	  }
	else
	  return 0;
      }

  

  





 
  template<typename _Res, typename... _Args>
    inline bool
    operator==(const function<_Res(_Args...)>& __f, nullptr_t) noexcept
    { return !static_cast<bool>(__f); }

  
  template<typename _Res, typename... _Args>
    inline bool
    operator==(nullptr_t, const function<_Res(_Args...)>& __f) noexcept
    { return !static_cast<bool>(__f); }

  





 
  template<typename _Res, typename... _Args>
    inline bool
    operator!=(const function<_Res(_Args...)>& __f, nullptr_t) noexcept
    { return static_cast<bool>(__f); }

  
  template<typename _Res, typename... _Args>
    inline bool
    operator!=(nullptr_t, const function<_Res(_Args...)>& __f) noexcept
    { return static_cast<bool>(__f); }

  

  



 
  template<typename _Res, typename... _Args>
    inline void
    swap(function<_Res(_Args...)>& __x, function<_Res(_Args...)>& __y)
    { __x.swap(__y); }


} 





























 



namespace std 
{


  


 

  template<typename> class auto_ptr;

  
  template<typename _Tp>
    struct default_delete
    {
      constexpr default_delete() noexcept = default;

      template<typename _Up, typename = typename
	       enable_if<is_convertible<_Up*, _Tp*>::value>::type>
        default_delete(const default_delete<_Up>&) noexcept { }

      void
      operator()(_Tp* __ptr) const
      {
	static_assert(sizeof(_Tp)>0,
		      "can't delete pointer to incomplete type");
	delete __ptr;
      }
    };

  
  
  
  template<typename _Tp>
    struct default_delete<_Tp[]>
    {
    private:
      template<typename _Up>
	using __remove_cv = typename remove_cv<_Up>::type;

      
      template<typename _Up>
	using __is_derived_Tp
	  = __and_< is_base_of<_Tp, _Up>,
		    __not_<is_same<__remove_cv<_Tp>, __remove_cv<_Up>>> >;

    public:
      constexpr default_delete() noexcept = default;

      template<typename _Up, typename = typename
	       enable_if<!__is_derived_Tp<_Up>::value>::type>
        default_delete(const default_delete<_Up[]>&) noexcept { }

      void
      operator()(_Tp* __ptr) const
      {
	static_assert(sizeof(_Tp)>0,
		      "can't delete pointer to incomplete type");
	delete [] __ptr;
      }

      template<typename _Up>
	typename enable_if<__is_derived_Tp<_Up>::value>::type
	operator()(_Up*) const = delete;
    };

  
  template <typename _Tp, typename _Dp = default_delete<_Tp> >
    class unique_ptr
    {
      
      class _Pointer
      {
	template<typename _Up>
	  static typename _Up::pointer __test(typename _Up::pointer*);

	template<typename _Up>
	  static _Tp* __test(...);

	typedef typename remove_reference<_Dp>::type _Del;

      public:
	typedef decltype(__test<_Del>(0)) type;
      };

      typedef std::tuple<typename _Pointer::type, _Dp>  __tuple_type;
      __tuple_type                                      _M_t;

    public:
      typedef typename _Pointer::type   pointer;
      typedef _Tp                       element_type;
      typedef _Dp                       deleter_type;

      
      constexpr unique_ptr() noexcept
      : _M_t()
      { static_assert(!is_pointer<deleter_type>::value,
		     "constructed with null function pointer deleter"); }

      explicit
      unique_ptr(pointer __p) noexcept
      : _M_t(__p, deleter_type())
      { static_assert(!is_pointer<deleter_type>::value,
		     "constructed with null function pointer deleter"); }

      unique_ptr(pointer __p,
	  typename conditional<is_reference<deleter_type>::value,
	    deleter_type, const deleter_type&>::type __d) noexcept
      : _M_t(__p, __d) { }

      unique_ptr(pointer __p,
	  typename remove_reference<deleter_type>::type&& __d) noexcept
      : _M_t(std::move(__p), std::move(__d))
      { static_assert(!std::is_reference<deleter_type>::value,
		      "rvalue deleter bound to reference"); }

      constexpr unique_ptr(nullptr_t) noexcept : unique_ptr() { }

      
      unique_ptr(unique_ptr&& __u) noexcept
      : _M_t(__u.release(), std::forward<deleter_type>(__u.get_deleter())) { }

      template<typename _Up, typename _Ep, typename = _Require<
	       is_convertible<typename unique_ptr<_Up, _Ep>::pointer, pointer>,
	       __not_<is_array<_Up>>,
	       typename conditional<is_reference<_Dp>::value,
				    is_same<_Ep, _Dp>,
				    is_convertible<_Ep, _Dp>>::type>>
	unique_ptr(unique_ptr<_Up, _Ep>&& __u) noexcept
	: _M_t(__u.release(), std::forward<_Ep>(__u.get_deleter()))
	{ }

      template<typename _Up, typename = _Require<
	       is_convertible<_Up*, _Tp*>, is_same<_Dp, default_delete<_Tp>>>>
	unique_ptr(auto_ptr<_Up>&& __u) noexcept;

      
      ~unique_ptr() noexcept
      {
	auto& __ptr = std::get<0>(_M_t);
	if (__ptr != nullptr)
	  get_deleter()(__ptr);
	__ptr = pointer();
      }

      
      unique_ptr&
      operator=(unique_ptr&& __u) noexcept
      {
	reset(__u.release());
	get_deleter() = std::forward<deleter_type>(__u.get_deleter());
	return *this;
      }

      template<typename _Up, typename _Ep>
	typename enable_if< __and_<
	  is_convertible<typename unique_ptr<_Up, _Ep>::pointer, pointer>,
	  __not_<is_array<_Up>>
	  >::value,
	  unique_ptr&>::type
	operator=(unique_ptr<_Up, _Ep>&& __u) noexcept
	{
	  reset(__u.release());
	  get_deleter() = std::forward<_Ep>(__u.get_deleter());
	  return *this;
	}

      unique_ptr&
      operator=(nullptr_t) noexcept
      {
	reset();
	return *this;
      }

      
      typename add_lvalue_reference<element_type>::type
      operator*() const
      {
	;
	return *get();
      }

      pointer
      operator->() const noexcept
      {
	;
	return get();
      }

      pointer
      get() const noexcept
      { return std::get<0>(_M_t); }

      deleter_type&
      get_deleter() noexcept
      { return std::get<1>(_M_t); }

      const deleter_type&
      get_deleter() const noexcept
      { return std::get<1>(_M_t); }

      explicit operator bool() const noexcept
      { return get() == pointer() ? false : true; }

      
      pointer
      release() noexcept
      {
	pointer __p = get();
	std::get<0>(_M_t) = pointer();
	return __p;
      }

      void
      reset(pointer __p = pointer()) noexcept
      {
	using std::swap;
	swap(std::get<0>(_M_t), __p);
	if (__p != pointer())
	  get_deleter()(__p);
      }

      void
      swap(unique_ptr& __u) noexcept
      {
	using std::swap;
	swap(_M_t, __u._M_t);
      }

      
      unique_ptr(const unique_ptr&) = delete;
      unique_ptr& operator=(const unique_ptr&) = delete;
  };

  
  
  
  
  template<typename _Tp, typename _Dp>
    class unique_ptr<_Tp[], _Dp>
    {
      
      class _Pointer
      {
	template<typename _Up>
	  static typename _Up::pointer __test(typename _Up::pointer*);

	template<typename _Up>
	  static _Tp* __test(...);

	typedef typename remove_reference<_Dp>::type _Del;

      public:
	typedef decltype(__test<_Del>(0)) type;
      };

      typedef std::tuple<typename _Pointer::type, _Dp>  __tuple_type;
      __tuple_type                                      _M_t;

      template<typename _Up>
	using __remove_cv = typename remove_cv<_Up>::type;

      
      template<typename _Up>
	using __is_derived_Tp
	  = __and_< is_base_of<_Tp, _Up>,
		    __not_<is_same<__remove_cv<_Tp>, __remove_cv<_Up>>> >;

      template<typename _Up, typename _Ep,
	       typename _Tp_pointer = typename _Pointer::type,
	       typename _Up_pointer = typename unique_ptr<_Up, _Ep>::pointer>
	using __safe_conversion = __and_<
	    is_convertible<_Up_pointer, _Tp_pointer>,
	    is_array<_Up>,
	    __or_<__not_<is_pointer<_Up_pointer>>,
		  __not_<is_pointer<_Tp_pointer>>,
		  __not_<__is_derived_Tp<typename remove_extent<_Up>::type>>
	    >
	  >;

    public:
      typedef typename _Pointer::type	pointer;
      typedef _Tp		 	element_type;
      typedef _Dp                       deleter_type;

      
      constexpr unique_ptr() noexcept
      : _M_t()
      { static_assert(!std::is_pointer<deleter_type>::value,
		      "constructed with null function pointer deleter"); }

      explicit
      unique_ptr(pointer __p) noexcept
      : _M_t(__p, deleter_type())
      { static_assert(!is_pointer<deleter_type>::value,
		      "constructed with null function pointer deleter"); }

      template<typename _Up, typename = _Require<is_pointer<pointer>,
	       is_convertible<_Up*, pointer>, __is_derived_Tp<_Up>>>
	explicit
	unique_ptr(_Up* __p) = delete;

      unique_ptr(pointer __p,
	  typename conditional<is_reference<deleter_type>::value,
	      deleter_type, const deleter_type&>::type __d) noexcept
      : _M_t(__p, __d) { }

      unique_ptr(pointer __p, typename
		 remove_reference<deleter_type>::type&& __d) noexcept
      : _M_t(std::move(__p), std::move(__d))
      { static_assert(!is_reference<deleter_type>::value,
		      "rvalue deleter bound to reference"); }

      
      unique_ptr(unique_ptr&& __u) noexcept
      : _M_t(__u.release(), std::forward<deleter_type>(__u.get_deleter())) { }

      constexpr unique_ptr(nullptr_t) noexcept : unique_ptr() { }

      template<typename _Up, typename _Ep,
	       typename = _Require<__safe_conversion<_Up, _Ep>,
		 typename conditional<is_reference<_Dp>::value,
				      is_same<_Ep, _Dp>,
				      is_convertible<_Ep, _Dp>>::type
	       >>
	unique_ptr(unique_ptr<_Up, _Ep>&& __u) noexcept
	: _M_t(__u.release(), std::forward<_Ep>(__u.get_deleter()))
	{ }

      
      ~unique_ptr()
      {
	auto& __ptr = std::get<0>(_M_t);
	if (__ptr != nullptr)
	  get_deleter()(__ptr);
	__ptr = pointer();
      }

      
      unique_ptr&
      operator=(unique_ptr&& __u) noexcept
      {
	reset(__u.release());
	get_deleter() = std::forward<deleter_type>(__u.get_deleter());
	return *this;
      }

      template<typename _Up, typename _Ep>
	typename
	enable_if<__safe_conversion<_Up, _Ep>::value, unique_ptr&>::type
	operator=(unique_ptr<_Up, _Ep>&& __u) noexcept
	{
	  reset(__u.release());
	  get_deleter() = std::forward<_Ep>(__u.get_deleter());
	  return *this;
	}

      unique_ptr&
      operator=(nullptr_t) noexcept
      {
	reset();
	return *this;
      }

      
      typename std::add_lvalue_reference<element_type>::type
      operator[](size_t __i) const
      {
	;
	return get()[__i];
      }

      pointer
      get() const noexcept
      { return std::get<0>(_M_t); }

      deleter_type&
      get_deleter() noexcept
      { return std::get<1>(_M_t); }

      const deleter_type&
      get_deleter() const noexcept
      { return std::get<1>(_M_t); }

      explicit operator bool() const noexcept
      { return get() == pointer() ? false : true; }

      
      pointer
      release() noexcept
      {
	pointer __p = get();
	std::get<0>(_M_t) = pointer();
	return __p;
      }

      void
      reset() noexcept
      { reset(pointer()); }

      void
      reset(pointer __p) noexcept
      {
	using std::swap;
	swap(std::get<0>(_M_t), __p);
	if (__p != nullptr)
	  get_deleter()(__p);
      }

      template<typename _Up, typename = _Require<is_pointer<pointer>,
	       is_convertible<_Up*, pointer>, __is_derived_Tp<_Up>>>
	void reset(_Up*) = delete;

      void
      swap(unique_ptr& __u) noexcept
      {
	using std::swap;
	swap(_M_t, __u._M_t);
      }

      
      unique_ptr(const unique_ptr&) = delete;
      unique_ptr& operator=(const unique_ptr&) = delete;

      
      template<typename _Up, typename = _Require<is_pointer<pointer>,
	       is_convertible<_Up*, pointer>, __is_derived_Tp<_Up>>>
	unique_ptr(_Up*, typename
		   conditional<is_reference<deleter_type>::value,
		   deleter_type, const deleter_type&>::type) = delete;

      template<typename _Up, typename = _Require<is_pointer<pointer>,
	       is_convertible<_Up*, pointer>, __is_derived_Tp<_Up>>>
	unique_ptr(_Up*, typename
		   remove_reference<deleter_type>::type&&) = delete;
    };

  template<typename _Tp, typename _Dp>
    inline void
    swap(unique_ptr<_Tp, _Dp>& __x,
	 unique_ptr<_Tp, _Dp>& __y) noexcept
    { __x.swap(__y); }

  template<typename _Tp, typename _Dp,
	   typename _Up, typename _Ep>
    inline bool
    operator==(const unique_ptr<_Tp, _Dp>& __x,
	       const unique_ptr<_Up, _Ep>& __y)
    { return __x.get() == __y.get(); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator==(const unique_ptr<_Tp, _Dp>& __x, nullptr_t) noexcept
    { return !__x; }

  template<typename _Tp, typename _Dp>
    inline bool
    operator==(nullptr_t, const unique_ptr<_Tp, _Dp>& __x) noexcept
    { return !__x; }

  template<typename _Tp, typename _Dp,
	   typename _Up, typename _Ep>
    inline bool
    operator!=(const unique_ptr<_Tp, _Dp>& __x,
	       const unique_ptr<_Up, _Ep>& __y)
    { return __x.get() != __y.get(); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator!=(const unique_ptr<_Tp, _Dp>& __x, nullptr_t) noexcept
    { return (bool)__x; }

  template<typename _Tp, typename _Dp>
    inline bool
    operator!=(nullptr_t, const unique_ptr<_Tp, _Dp>& __x) noexcept
    { return (bool)__x; }

  template<typename _Tp, typename _Dp,
	   typename _Up, typename _Ep>
    inline bool
    operator<(const unique_ptr<_Tp, _Dp>& __x,
	      const unique_ptr<_Up, _Ep>& __y)
    {
      typedef typename
	std::common_type<typename unique_ptr<_Tp, _Dp>::pointer,
	                 typename unique_ptr<_Up, _Ep>::pointer>::type _CT;
      return std::less<_CT>()(__x.get(), __y.get());
    }

  template<typename _Tp, typename _Dp>
    inline bool
    operator<(const unique_ptr<_Tp, _Dp>& __x, nullptr_t)
    { return std::less<typename unique_ptr<_Tp, _Dp>::pointer>()(__x.get(),
								 nullptr); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator<(nullptr_t, const unique_ptr<_Tp, _Dp>& __x)
    { return std::less<typename unique_ptr<_Tp, _Dp>::pointer>()(nullptr,
								 __x.get()); }

  template<typename _Tp, typename _Dp,
	   typename _Up, typename _Ep>
    inline bool
    operator<=(const unique_ptr<_Tp, _Dp>& __x,
	       const unique_ptr<_Up, _Ep>& __y)
    { return !(__y < __x); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator<=(const unique_ptr<_Tp, _Dp>& __x, nullptr_t)
    { return !(nullptr < __x); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator<=(nullptr_t, const unique_ptr<_Tp, _Dp>& __x)
    { return !(__x < nullptr); }

  template<typename _Tp, typename _Dp,
	   typename _Up, typename _Ep>
    inline bool
    operator>(const unique_ptr<_Tp, _Dp>& __x,
	      const unique_ptr<_Up, _Ep>& __y)
    { return (__y < __x); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator>(const unique_ptr<_Tp, _Dp>& __x, nullptr_t)
    { return std::less<typename unique_ptr<_Tp, _Dp>::pointer>()(nullptr,
								 __x.get()); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator>(nullptr_t, const unique_ptr<_Tp, _Dp>& __x)
    { return std::less<typename unique_ptr<_Tp, _Dp>::pointer>()(__x.get(),
								 nullptr); }

  template<typename _Tp, typename _Dp,
	   typename _Up, typename _Ep>
    inline bool
    operator>=(const unique_ptr<_Tp, _Dp>& __x,
	       const unique_ptr<_Up, _Ep>& __y)
    { return !(__x < __y); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator>=(const unique_ptr<_Tp, _Dp>& __x, nullptr_t)
    { return !(__x < nullptr); }

  template<typename _Tp, typename _Dp>
    inline bool
    operator>=(nullptr_t, const unique_ptr<_Tp, _Dp>& __x)
    { return !(nullptr < __x); }

  
  template<typename _Tp, typename _Dp>
    struct hash<unique_ptr<_Tp, _Dp>>
    : public __hash_base<size_t, unique_ptr<_Tp, _Dp>>
    {
      size_t
      operator()(const unique_ptr<_Tp, _Dp>& __u) const noexcept
      {
	typedef unique_ptr<_Tp, _Dp> _UP;
	return std::hash<typename _UP::pointer>()(__u.get());
      }
    };

  


} 















































 
















































 


namespace std 
{


  template<typename> class auto_ptr;

 


 
  class bad_weak_ptr : public std::exception
  {
  public:
    virtual char const*
    what() const noexcept;

    virtual ~bad_weak_ptr() noexcept;    
  };

  
  inline void
  __throw_bad_weak_ptr()
  { (throw (bad_weak_ptr())); }

  using __gnu_cxx::_Lock_policy;
  using __gnu_cxx::__default_lock_policy;
  using __gnu_cxx::_S_single;
  using __gnu_cxx::_S_mutex;
  using __gnu_cxx::_S_atomic;

  
  template<_Lock_policy _Lp>
    class _Mutex_base
    {
    protected:
      
      enum { _S_need_barriers = 0 };
    };

  template<>
    class _Mutex_base<_S_mutex>
    : public __gnu_cxx::__mutex
    {
    protected:
      
      
      
      enum { _S_need_barriers = 1 };
    };

  template<_Lock_policy _Lp = __default_lock_policy>
    class _Sp_counted_base
    : public _Mutex_base<_Lp>
    {
    public:  
      _Sp_counted_base() noexcept
      : _M_use_count(1), _M_weak_count(1) { }
      
      virtual
      ~_Sp_counted_base() noexcept
      { }
  
      
      
      virtual void
      _M_dispose() noexcept = 0;
      
      
      virtual void
      _M_destroy() noexcept
      { delete this; }
      
      virtual void*
      _M_get_deleter(const std::type_info&) = 0;

      void
      _M_add_ref_copy()
      { __gnu_cxx::__atomic_add_dispatch(&_M_use_count, 1); }
  
      void
      _M_add_ref_lock();
      
      void
      _M_release() noexcept
      {
        
        ;
	if (__gnu_cxx::__exchange_and_add_dispatch(&_M_use_count, -1) == 1)
	  {
            ;
	    _M_dispose();
	    
	    
	    
	    
	    if (_Mutex_base<_Lp>::_S_need_barriers)
	      {
	        __asm __volatile ("" :: : "memory");
	        __asm __volatile ("" :: : "memory");
	      }

            
            ;
	    if (__gnu_cxx::__exchange_and_add_dispatch(&_M_weak_count,
						       -1) == 1)
              {
                ;
	        _M_destroy();
              }
	  }
      }
  
      void
      _M_weak_add_ref() noexcept
      { __gnu_cxx::__atomic_add_dispatch(&_M_weak_count, 1); }

      void
      _M_weak_release() noexcept
      {
        
        ;
	if (__gnu_cxx::__exchange_and_add_dispatch(&_M_weak_count, -1) == 1)
	  {
            ;
	    if (_Mutex_base<_Lp>::_S_need_barriers)
	      {
	        
	        
	        __asm __volatile ("" :: : "memory");
	        __asm __volatile ("" :: : "memory");
	      }
	    _M_destroy();
	  }
      }
  
      long
      _M_get_use_count() const noexcept
      {
        
        
        return __atomic_load_n(&_M_use_count, 0);
      }

    private:  
      _Sp_counted_base(_Sp_counted_base const&) = delete;
      _Sp_counted_base& operator=(_Sp_counted_base const&) = delete;

      _Atomic_word  _M_use_count;     
      _Atomic_word  _M_weak_count;    
    };

  template<>
    inline void
    _Sp_counted_base<_S_single>::
    _M_add_ref_lock()
    {
      if (__gnu_cxx::__exchange_and_add_dispatch(&_M_use_count, 1) == 0)
	{
	  _M_use_count = 0;
	  __throw_bad_weak_ptr();
	}
    }

  template<>
    inline void
    _Sp_counted_base<_S_mutex>::
    _M_add_ref_lock()
    {
      __gnu_cxx::__scoped_lock sentry(*this);
      if (__gnu_cxx::__exchange_and_add_dispatch(&_M_use_count, 1) == 0)
	{
	  _M_use_count = 0;
	  __throw_bad_weak_ptr();
	}
    }

  template<> 
    inline void
    _Sp_counted_base<_S_atomic>::
    _M_add_ref_lock()
    {
      
      _Atomic_word __count = _M_get_use_count();
      do
	{
	  if (__count == 0)
	    __throw_bad_weak_ptr();
	  
	  
	}
      while (!__atomic_compare_exchange_n(&_M_use_count, &__count, __count + 1,
					  true, 4, 
					  0));
    }


  
  template<typename _Tp, _Lock_policy _Lp = __default_lock_policy>
    class __shared_ptr;

  template<typename _Tp, _Lock_policy _Lp = __default_lock_policy>
    class __weak_ptr;

  template<typename _Tp, _Lock_policy _Lp = __default_lock_policy>
    class __enable_shared_from_this;

  template<typename _Tp>
    class shared_ptr;

  template<typename _Tp>
    class weak_ptr;

  template<typename _Tp>
    struct owner_less;

  template<typename _Tp>
    class enable_shared_from_this;

  template<_Lock_policy _Lp = __default_lock_policy>
    class __weak_count;

  template<_Lock_policy _Lp = __default_lock_policy>
    class __shared_count;


  
  template<typename _Ptr, _Lock_policy _Lp>
    class _Sp_counted_ptr final : public _Sp_counted_base<_Lp>
    {
    public:
      explicit
      _Sp_counted_ptr(_Ptr __p)
      : _M_ptr(__p) { }

      virtual void
      _M_dispose() noexcept
      { delete _M_ptr; }

      virtual void
      _M_destroy() noexcept
      { delete this; }

      virtual void*
      _M_get_deleter(const std::type_info&)
      { return 0; }

      _Sp_counted_ptr(const _Sp_counted_ptr&) = delete;
      _Sp_counted_ptr& operator=(const _Sp_counted_ptr&) = delete;

    protected:
      _Ptr             _M_ptr;  
    };

  template<>
    inline void
    _Sp_counted_ptr<nullptr_t, _S_single>::_M_dispose() noexcept { }

  template<>
    inline void
    _Sp_counted_ptr<nullptr_t, _S_mutex>::_M_dispose() noexcept { }

  template<>
    inline void
    _Sp_counted_ptr<nullptr_t, _S_atomic>::_M_dispose() noexcept { }

  
  template<typename _Ptr, typename _Deleter, typename _Alloc, _Lock_policy _Lp>
    class _Sp_counted_deleter final : public _Sp_counted_base<_Lp>
    {
      
      
      
      struct _My_Deleter
      : public _Alloc           
      {
	_Deleter _M_del;        
	_My_Deleter(_Deleter __d, const _Alloc& __a)
	: _Alloc(__a), _M_del(__d) { }
      };

    public:
      
      _Sp_counted_deleter(_Ptr __p, _Deleter __d)
      : _M_ptr(__p), _M_del(__d, _Alloc()) { }

      
      _Sp_counted_deleter(_Ptr __p, _Deleter __d, const _Alloc& __a)
      : _M_ptr(__p), _M_del(__d, __a) { }

      ~_Sp_counted_deleter() noexcept { }

      virtual void
      _M_dispose() noexcept
      { _M_del._M_del(_M_ptr); }

      virtual void
      _M_destroy() noexcept
      {
	typedef typename allocator_traits<_Alloc>::template
	  rebind_traits<_Sp_counted_deleter> _Alloc_traits;
	typename _Alloc_traits::allocator_type __a(_M_del);
	_Alloc_traits::destroy(__a, this);
	_Alloc_traits::deallocate(__a, this, 1);
      }

      virtual void*
      _M_get_deleter(const std::type_info& __ti)
      {
        return __ti == typeid(_Deleter) ? &_M_del._M_del : 0;
      }

    protected:
      _Ptr             _M_ptr;  
      _My_Deleter      _M_del;  
    };

  

  struct _Sp_make_shared_tag { };

  template<typename _Tp, typename _Alloc, _Lock_policy _Lp>
    class _Sp_counted_ptr_inplace final : public _Sp_counted_base<_Lp>
    {
      
      
      
      struct _Impl
      : public _Alloc           
      {
	_Impl(_Alloc __a) : _Alloc(__a), _M_ptr() { }
	_Tp* _M_ptr;
      };

    public:
      template<typename... _Args>
	_Sp_counted_ptr_inplace(_Alloc __a, _Args&&... __args)
	: _M_impl(__a)
	{
	  _M_impl._M_ptr = static_cast<_Tp*>(static_cast<void*>(&_M_storage));
	  
	  
	  allocator_traits<_Alloc>::construct(__a, _M_impl._M_ptr,
	      std::forward<_Args>(__args)...); 
	}

      ~_Sp_counted_ptr_inplace() noexcept { }

      virtual void
      _M_dispose() noexcept
      { allocator_traits<_Alloc>::destroy(_M_impl, _M_impl._M_ptr); }

      
      virtual void
      _M_destroy() noexcept
      {
	typedef typename allocator_traits<_Alloc>::template
	  rebind_traits<_Sp_counted_ptr_inplace> _Alloc_traits;
	typename _Alloc_traits::allocator_type __a(_M_impl);
	_Alloc_traits::destroy(__a, this);
	_Alloc_traits::deallocate(__a, this, 1);
      }

      
      virtual void*
      _M_get_deleter(const std::type_info& __ti) noexcept
      {
	return __ti == typeid(_Sp_make_shared_tag)
	       ? static_cast<void*>(&_M_storage)
	       : 0;
      }

    private:
      _Impl _M_impl;
      typename aligned_storage<sizeof(_Tp), alignment_of<_Tp>::value>::type
	_M_storage;
    };

  template<_Lock_policy _Lp>
    class __shared_count
    {
    public:
      constexpr __shared_count() noexcept : _M_pi(0)
      { }

      template<typename _Ptr>
        explicit
	__shared_count(_Ptr __p) : _M_pi(0)
	{
	  try
	    {
	      _M_pi = new _Sp_counted_ptr<_Ptr, _Lp>(__p);
	    }
	  catch(...)
	    {
	      delete __p;
	      throw;
	    }
	}

      template<typename _Ptr, typename _Deleter>
	__shared_count(_Ptr __p, _Deleter __d)
	: __shared_count(__p, std::move(__d), allocator<int>())
	{ }

      template<typename _Ptr, typename _Deleter, typename _Alloc>
	__shared_count(_Ptr __p, _Deleter __d, _Alloc __a) : _M_pi(0)
	{
	  typedef _Sp_counted_deleter<_Ptr, _Deleter, _Alloc, _Lp> _Sp_cd_type;
	  typedef typename allocator_traits<_Alloc>::template
	    rebind_traits<_Sp_cd_type> _Alloc_traits;
	  typename _Alloc_traits::allocator_type __a2(__a);
	  _Sp_cd_type* __mem = 0;
	  try
	    {
	      __mem = _Alloc_traits::allocate(__a2, 1);
	      _Alloc_traits::construct(__a2, __mem,
		  __p, std::move(__d), std::move(__a));
	      _M_pi = __mem;
	    }
	  catch(...)
	    {
	      __d(__p); 
	      if (__mem)
	        _Alloc_traits::deallocate(__a2, __mem, 1);
	      throw;
	    }
	}

      template<typename _Tp, typename _Alloc, typename... _Args>
	__shared_count(_Sp_make_shared_tag, _Tp*, const _Alloc& __a,
		       _Args&&... __args)
	: _M_pi(0)
	{
	  typedef _Sp_counted_ptr_inplace<_Tp, _Alloc, _Lp> _Sp_cp_type;
	  typedef typename allocator_traits<_Alloc>::template
	    rebind_traits<_Sp_cp_type> _Alloc_traits;
	  typename _Alloc_traits::allocator_type __a2(__a);
	  _Sp_cp_type* __mem = _Alloc_traits::allocate(__a2, 1);
	  try
	    {
	      _Alloc_traits::construct(__a2, __mem, std::move(__a),
		    std::forward<_Args>(__args)...);
	      _M_pi = __mem;
	    }
	  catch(...)
	    {
	      _Alloc_traits::deallocate(__a2, __mem, 1);
	      throw;
	    }
	}

      
      template<typename _Tp>
        explicit
	__shared_count(std::auto_ptr<_Tp>&& __r);

      
      template<typename _Tp, typename _Del>
        explicit
	__shared_count(std::unique_ptr<_Tp, _Del>&& __r) : _M_pi(0)
	{
	  using _Ptr = typename unique_ptr<_Tp, _Del>::pointer;
	  using _Del2 = typename conditional<is_reference<_Del>::value,
	      reference_wrapper<typename remove_reference<_Del>::type>,
	      _Del>::type;
	  using _Sp_cd_type
	    = _Sp_counted_deleter<_Ptr, _Del2, allocator<void>, _Lp>;
	  using _Alloc = allocator<_Sp_cd_type>;
	  using _Alloc_traits = allocator_traits<_Alloc>;
	  _Alloc __a;
	  _Sp_cd_type* __mem = _Alloc_traits::allocate(__a, 1);
	  _Alloc_traits::construct(__a, __mem, __r.release(),
				   __r.get_deleter());  
	  _M_pi = __mem;
	}

      
      explicit __shared_count(const __weak_count<_Lp>& __r);

      ~__shared_count() noexcept
      {
	if (_M_pi != nullptr)
	  _M_pi->_M_release();
      }

      __shared_count(const __shared_count& __r) noexcept
      : _M_pi(__r._M_pi)
      {
	if (_M_pi != 0)
	  _M_pi->_M_add_ref_copy();
      }

      __shared_count&
      operator=(const __shared_count& __r) noexcept
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	if (__tmp != _M_pi)
	  {
	    if (__tmp != 0)
	      __tmp->_M_add_ref_copy();
	    if (_M_pi != 0)
	      _M_pi->_M_release();
	    _M_pi = __tmp;
	  }
	return *this;
      }

      void
      _M_swap(__shared_count& __r) noexcept
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	__r._M_pi = _M_pi;
	_M_pi = __tmp;
      }

      long
      _M_get_use_count() const noexcept
      { return _M_pi != 0 ? _M_pi->_M_get_use_count() : 0; }

      bool
      _M_unique() const noexcept
      { return this->_M_get_use_count() == 1; }

      void*
      _M_get_deleter(const std::type_info& __ti) const noexcept
      { return _M_pi ? _M_pi->_M_get_deleter(__ti) : 0; }

      bool
      _M_less(const __shared_count& __rhs) const noexcept
      { return std::less<_Sp_counted_base<_Lp>*>()(this->_M_pi, __rhs._M_pi); }

      bool
      _M_less(const __weak_count<_Lp>& __rhs) const noexcept
      { return std::less<_Sp_counted_base<_Lp>*>()(this->_M_pi, __rhs._M_pi); }

      
      friend inline bool
      operator==(const __shared_count& __a, const __shared_count& __b) noexcept
      { return __a._M_pi == __b._M_pi; }

    private:
      friend class __weak_count<_Lp>;

      _Sp_counted_base<_Lp>*  _M_pi;
    };


  template<_Lock_policy _Lp>
    class __weak_count
    {
    public:
      constexpr __weak_count() noexcept : _M_pi(0)
      { }

      __weak_count(const __shared_count<_Lp>& __r) noexcept
      : _M_pi(__r._M_pi)
      {
	if (_M_pi != 0)
	  _M_pi->_M_weak_add_ref();
      }

      __weak_count(const __weak_count<_Lp>& __r) noexcept
      : _M_pi(__r._M_pi)
      {
	if (_M_pi != 0)
	  _M_pi->_M_weak_add_ref();
      }

      ~__weak_count() noexcept
      {
	if (_M_pi != 0)
	  _M_pi->_M_weak_release();
      }

      __weak_count<_Lp>&
      operator=(const __shared_count<_Lp>& __r) noexcept
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	if (__tmp != 0)
	  __tmp->_M_weak_add_ref();
	if (_M_pi != 0)
	  _M_pi->_M_weak_release();
	_M_pi = __tmp;
	return *this;
      }

      __weak_count<_Lp>&
      operator=(const __weak_count<_Lp>& __r) noexcept
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	if (__tmp != 0)
	  __tmp->_M_weak_add_ref();
	if (_M_pi != 0)
	  _M_pi->_M_weak_release();
	_M_pi = __tmp;
	return *this;
      }

      void
      _M_swap(__weak_count<_Lp>& __r) noexcept
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	__r._M_pi = _M_pi;
	_M_pi = __tmp;
      }

      long
      _M_get_use_count() const noexcept
      { return _M_pi != 0 ? _M_pi->_M_get_use_count() : 0; }

      bool
      _M_less(const __weak_count& __rhs) const noexcept
      { return std::less<_Sp_counted_base<_Lp>*>()(this->_M_pi, __rhs._M_pi); }

      bool
      _M_less(const __shared_count<_Lp>& __rhs) const noexcept
      { return std::less<_Sp_counted_base<_Lp>*>()(this->_M_pi, __rhs._M_pi); }

      
      friend inline bool
      operator==(const __weak_count& __a, const __weak_count& __b) noexcept
      { return __a._M_pi == __b._M_pi; }

    private:
      friend class __shared_count<_Lp>;

      _Sp_counted_base<_Lp>*  _M_pi;
    };

  
  template<_Lock_policy _Lp>
    inline __shared_count<_Lp>:: __shared_count(const __weak_count<_Lp>& __r)
    : _M_pi(__r._M_pi)
    {
      if (_M_pi != 0)
	_M_pi->_M_add_ref_lock();
      else
	__throw_bad_weak_ptr();
    }


  

  
  template<_Lock_policy _Lp, typename _Tp1, typename _Tp2>
    void
    __enable_shared_from_this_helper(const __shared_count<_Lp>&,
				     const __enable_shared_from_this<_Tp1,
				     _Lp>*, const _Tp2*) noexcept;

  
  template<typename _Tp1, typename _Tp2>
    void
    __enable_shared_from_this_helper(const __shared_count<>&,
				     const enable_shared_from_this<_Tp1>*,
				     const _Tp2*) noexcept;

  template<_Lock_policy _Lp>
    inline void
    __enable_shared_from_this_helper(const __shared_count<_Lp>&, ...) noexcept
    { }


  template<typename _Tp, _Lock_policy _Lp>
    class __shared_ptr
    {
    public:
      typedef _Tp   element_type;

      constexpr __shared_ptr() noexcept
      : _M_ptr(0), _M_refcount()
      { }

      template<typename _Tp1>
	explicit __shared_ptr(_Tp1* __p)
        : _M_ptr(__p), _M_refcount(__p)
	{
	  
	  static_assert( sizeof(_Tp1) > 0, "incomplete type" );
	  __enable_shared_from_this_helper(_M_refcount, __p, __p);
	}

      template<typename _Tp1, typename _Deleter>
	__shared_ptr(_Tp1* __p, _Deleter __d)
	: _M_ptr(__p), _M_refcount(__p, __d)
	{
	  
	  
	  __enable_shared_from_this_helper(_M_refcount, __p, __p);
	}

      template<typename _Tp1, typename _Deleter, typename _Alloc>
	__shared_ptr(_Tp1* __p, _Deleter __d, _Alloc __a)
	: _M_ptr(__p), _M_refcount(__p, __d, std::move(__a))
	{
	  
	  
	  __enable_shared_from_this_helper(_M_refcount, __p, __p);
	}

      template<typename _Deleter>
	__shared_ptr(nullptr_t __p, _Deleter __d)
	: _M_ptr(0), _M_refcount(__p, __d)
	{ }

      template<typename _Deleter, typename _Alloc>
        __shared_ptr(nullptr_t __p, _Deleter __d, _Alloc __a)
	: _M_ptr(0), _M_refcount(__p, __d, std::move(__a))
	{ }

      template<typename _Tp1>
	__shared_ptr(const __shared_ptr<_Tp1, _Lp>& __r, _Tp* __p) noexcept
	: _M_ptr(__p), _M_refcount(__r._M_refcount) 
	{ }

      __shared_ptr(const __shared_ptr&) noexcept = default;
      __shared_ptr& operator=(const __shared_ptr&) noexcept = default;
      ~__shared_ptr() = default;

      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	__shared_ptr(const __shared_ptr<_Tp1, _Lp>& __r) noexcept
	: _M_ptr(__r._M_ptr), _M_refcount(__r._M_refcount)
	{ }

      __shared_ptr(__shared_ptr&& __r) noexcept
      : _M_ptr(__r._M_ptr), _M_refcount()
      {
	_M_refcount._M_swap(__r._M_refcount);
	__r._M_ptr = 0;
      }

      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	__shared_ptr(__shared_ptr<_Tp1, _Lp>&& __r) noexcept
	: _M_ptr(__r._M_ptr), _M_refcount()
	{
	  _M_refcount._M_swap(__r._M_refcount);
	  __r._M_ptr = 0;
	}

      template<typename _Tp1>
	explicit __shared_ptr(const __weak_ptr<_Tp1, _Lp>& __r)
	: _M_refcount(__r._M_refcount) 
	{
	  

	  
	  
	  _M_ptr = __r._M_ptr;
	}

      
      template<typename _Tp1, typename _Del>
	__shared_ptr(std::unique_ptr<_Tp1, _Del>&& __r)
	: _M_ptr(__r.get()), _M_refcount()
	{
	  
	  auto __tmp = __r.get();
	  _M_refcount = __shared_count<_Lp>(std::move(__r));
	  __enable_shared_from_this_helper(_M_refcount, __tmp, __tmp);
	}

      
      template<typename _Tp1>
	__shared_ptr(std::auto_ptr<_Tp1>&& __r);

       
      constexpr __shared_ptr(nullptr_t) noexcept
      : _M_ptr(0), _M_refcount()
      { }

      template<typename _Tp1>
	__shared_ptr&
	operator=(const __shared_ptr<_Tp1, _Lp>& __r) noexcept
	{
	  _M_ptr = __r._M_ptr;
	  _M_refcount = __r._M_refcount; 
	  return *this;
	}

      template<typename _Tp1>
	__shared_ptr&
	operator=(std::auto_ptr<_Tp1>&& __r)
	{
	  __shared_ptr(std::move(__r)).swap(*this);
	  return *this;
	}

      __shared_ptr&
      operator=(__shared_ptr&& __r) noexcept
      {
	__shared_ptr(std::move(__r)).swap(*this);
	return *this;
      }

      template<class _Tp1>
	__shared_ptr&
	operator=(__shared_ptr<_Tp1, _Lp>&& __r) noexcept
	{
	  __shared_ptr(std::move(__r)).swap(*this);
	  return *this;
	}

      template<typename _Tp1, typename _Del>
	__shared_ptr&
	operator=(std::unique_ptr<_Tp1, _Del>&& __r)
	{
	  __shared_ptr(std::move(__r)).swap(*this);
	  return *this;
	}

      void
      reset() noexcept
      { __shared_ptr().swap(*this); }

      template<typename _Tp1>
	void
	reset(_Tp1* __p) 
	{
	  
	  ;
	  __shared_ptr(__p).swap(*this);
	}

      template<typename _Tp1, typename _Deleter>
	void
	reset(_Tp1* __p, _Deleter __d)
	{ __shared_ptr(__p, __d).swap(*this); }

      template<typename _Tp1, typename _Deleter, typename _Alloc>
	void
        reset(_Tp1* __p, _Deleter __d, _Alloc __a)
        { __shared_ptr(__p, __d, std::move(__a)).swap(*this); }

      
      typename std::add_lvalue_reference<_Tp>::type
      operator*() const noexcept
      {
	;
	return *_M_ptr;
      }

      _Tp*
      operator->() const noexcept
      {
	;
	return _M_ptr;
      }

      _Tp*
      get() const noexcept
      { return _M_ptr; }

      explicit operator bool() const 
      { return _M_ptr == 0 ? false : true; }

      bool
      unique() const noexcept
      { return _M_refcount._M_unique(); }

      long
      use_count() const noexcept
      { return _M_refcount._M_get_use_count(); }

      void
      swap(__shared_ptr<_Tp, _Lp>& __other) noexcept
      {
	std::swap(_M_ptr, __other._M_ptr);
	_M_refcount._M_swap(__other._M_refcount);
      }

      template<typename _Tp1>
	bool
	owner_before(__shared_ptr<_Tp1, _Lp> const& __rhs) const
	{ return _M_refcount._M_less(__rhs._M_refcount); }

      template<typename _Tp1>
	bool
	owner_before(__weak_ptr<_Tp1, _Lp> const& __rhs) const
	{ return _M_refcount._M_less(__rhs._M_refcount); }

    protected:
      
      template<typename _Alloc, typename... _Args>
	__shared_ptr(_Sp_make_shared_tag __tag, const _Alloc& __a,
		     _Args&&... __args)
	: _M_ptr(), _M_refcount(__tag, (_Tp*)0, __a,
				std::forward<_Args>(__args)...)
	{
	  
	  
	  void* __p = _M_refcount._M_get_deleter(typeid(__tag));
	  _M_ptr = static_cast<_Tp*>(__p);
	  __enable_shared_from_this_helper(_M_refcount, _M_ptr, _M_ptr);
	}

      template<typename _Tp1, _Lock_policy _Lp1, typename _Alloc,
	       typename... _Args>
	friend __shared_ptr<_Tp1, _Lp1>
	__allocate_shared(const _Alloc& __a, _Args&&... __args);

    private:
      void*
      _M_get_deleter(const std::type_info& __ti) const noexcept
      { return _M_refcount._M_get_deleter(__ti); }

      template<typename _Tp1, _Lock_policy _Lp1> friend class __shared_ptr;
      template<typename _Tp1, _Lock_policy _Lp1> friend class __weak_ptr;

      template<typename _Del, typename _Tp1, _Lock_policy _Lp1>
	friend _Del* get_deleter(const __shared_ptr<_Tp1, _Lp1>&) noexcept;

      _Tp*	   	   _M_ptr;         
      __shared_count<_Lp>  _M_refcount;    
    };


  
  template<typename _Tp1, typename _Tp2, _Lock_policy _Lp>
    inline bool
    operator==(const __shared_ptr<_Tp1, _Lp>& __a,
	       const __shared_ptr<_Tp2, _Lp>& __b) noexcept
    { return __a.get() == __b.get(); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator==(const __shared_ptr<_Tp, _Lp>& __a, nullptr_t) noexcept
    { return !__a; }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator==(nullptr_t, const __shared_ptr<_Tp, _Lp>& __a) noexcept
    { return !__a; }

  template<typename _Tp1, typename _Tp2, _Lock_policy _Lp>
    inline bool
    operator!=(const __shared_ptr<_Tp1, _Lp>& __a,
	       const __shared_ptr<_Tp2, _Lp>& __b) noexcept
    { return __a.get() != __b.get(); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator!=(const __shared_ptr<_Tp, _Lp>& __a, nullptr_t) noexcept
    { return (bool)__a; }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator!=(nullptr_t, const __shared_ptr<_Tp, _Lp>& __a) noexcept
    { return (bool)__a; }

  template<typename _Tp1, typename _Tp2, _Lock_policy _Lp>
    inline bool
    operator<(const __shared_ptr<_Tp1, _Lp>& __a,
	      const __shared_ptr<_Tp2, _Lp>& __b) noexcept
    {
      typedef typename std::common_type<_Tp1*, _Tp2*>::type _CT;
      return std::less<_CT>()(__a.get(), __b.get());
    }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator<(const __shared_ptr<_Tp, _Lp>& __a, nullptr_t) noexcept
    { return std::less<_Tp*>()(__a.get(), nullptr); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator<(nullptr_t, const __shared_ptr<_Tp, _Lp>& __a) noexcept
    { return std::less<_Tp*>()(nullptr, __a.get()); }

  template<typename _Tp1, typename _Tp2, _Lock_policy _Lp>
    inline bool
    operator<=(const __shared_ptr<_Tp1, _Lp>& __a,
	       const __shared_ptr<_Tp2, _Lp>& __b) noexcept
    { return !(__b < __a); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator<=(const __shared_ptr<_Tp, _Lp>& __a, nullptr_t) noexcept
    { return !(nullptr < __a); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator<=(nullptr_t, const __shared_ptr<_Tp, _Lp>& __a) noexcept
    { return !(__a < nullptr); }

  template<typename _Tp1, typename _Tp2, _Lock_policy _Lp>
    inline bool
    operator>(const __shared_ptr<_Tp1, _Lp>& __a,
	      const __shared_ptr<_Tp2, _Lp>& __b) noexcept
    { return (__b < __a); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator>(const __shared_ptr<_Tp, _Lp>& __a, nullptr_t) noexcept
    { return std::less<_Tp*>()(nullptr, __a.get()); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator>(nullptr_t, const __shared_ptr<_Tp, _Lp>& __a) noexcept
    { return std::less<_Tp*>()(__a.get(), nullptr); }

  template<typename _Tp1, typename _Tp2, _Lock_policy _Lp>
    inline bool
    operator>=(const __shared_ptr<_Tp1, _Lp>& __a,
	       const __shared_ptr<_Tp2, _Lp>& __b) noexcept
    { return !(__a < __b); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator>=(const __shared_ptr<_Tp, _Lp>& __a, nullptr_t) noexcept
    { return !(__a < nullptr); }

  template<typename _Tp, _Lock_policy _Lp>
    inline bool
    operator>=(nullptr_t, const __shared_ptr<_Tp, _Lp>& __a) noexcept
    { return !(nullptr < __a); }

  template<typename _Sp>
    struct _Sp_less : public binary_function<_Sp, _Sp, bool>
    {
      bool
      operator()(const _Sp& __lhs, const _Sp& __rhs) const noexcept
      {
	typedef typename _Sp::element_type element_type;
	return std::less<element_type*>()(__lhs.get(), __rhs.get());
      }
    };

  template<typename _Tp, _Lock_policy _Lp>
    struct less<__shared_ptr<_Tp, _Lp>>
    : public _Sp_less<__shared_ptr<_Tp, _Lp>>
    { };

  
  template<typename _Tp, _Lock_policy _Lp>
    inline void
    swap(__shared_ptr<_Tp, _Lp>& __a, __shared_ptr<_Tp, _Lp>& __b) noexcept
    { __a.swap(__b); }

  

  
  
  
  
  
  template<typename _Tp, typename _Tp1, _Lock_policy _Lp>
    inline __shared_ptr<_Tp, _Lp>
    static_pointer_cast(const __shared_ptr<_Tp1, _Lp>& __r) noexcept
    { return __shared_ptr<_Tp, _Lp>(__r, static_cast<_Tp*>(__r.get())); }

  
  
  
  
  
  template<typename _Tp, typename _Tp1, _Lock_policy _Lp>
    inline __shared_ptr<_Tp, _Lp>
    const_pointer_cast(const __shared_ptr<_Tp1, _Lp>& __r) noexcept
    { return __shared_ptr<_Tp, _Lp>(__r, const_cast<_Tp*>(__r.get())); }

  
  
  
  
  
  template<typename _Tp, typename _Tp1, _Lock_policy _Lp>
    inline __shared_ptr<_Tp, _Lp>
    dynamic_pointer_cast(const __shared_ptr<_Tp1, _Lp>& __r) noexcept
    {
      if (_Tp* __p = dynamic_cast<_Tp*>(__r.get()))
	return __shared_ptr<_Tp, _Lp>(__r, __p);
      return __shared_ptr<_Tp, _Lp>();
    }


  template<typename _Tp, _Lock_policy _Lp>
    class __weak_ptr
    {
    public:
      typedef _Tp element_type;

      constexpr __weak_ptr() noexcept
      : _M_ptr(0), _M_refcount()
      { }

      __weak_ptr(const __weak_ptr&) noexcept = default;
      __weak_ptr& operator=(const __weak_ptr&) noexcept = default;
      ~__weak_ptr() = default;

      
      
      
      
      
      
      
      
      
      
      
      
      
      
      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	__weak_ptr(const __weak_ptr<_Tp1, _Lp>& __r) noexcept
	: _M_refcount(__r._M_refcount)
        { _M_ptr = __r.lock().get(); }

      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	__weak_ptr(const __shared_ptr<_Tp1, _Lp>& __r) noexcept
	: _M_ptr(__r._M_ptr), _M_refcount(__r._M_refcount)
	{ }

      template<typename _Tp1>
	__weak_ptr&
	operator=(const __weak_ptr<_Tp1, _Lp>& __r) noexcept
	{
	  _M_ptr = __r.lock().get();
	  _M_refcount = __r._M_refcount;
	  return *this;
	}

      template<typename _Tp1>
	__weak_ptr&
	operator=(const __shared_ptr<_Tp1, _Lp>& __r) noexcept
	{
	  _M_ptr = __r._M_ptr;
	  _M_refcount = __r._M_refcount;
	  return *this;
	}

      __shared_ptr<_Tp, _Lp>
      lock() const noexcept
      {
	
	if (expired())
	  return __shared_ptr<element_type, _Lp>();

	try
	  {
	    return __shared_ptr<element_type, _Lp>(*this);
	  }
	catch(const bad_weak_ptr&)
	  {
	    
	    
	    
	    return __shared_ptr<element_type, _Lp>();
	  }

      } 

      long
      use_count() const noexcept
      { return _M_refcount._M_get_use_count(); }

      bool
      expired() const noexcept
      { return _M_refcount._M_get_use_count() == 0; }

      template<typename _Tp1>
	bool
	owner_before(const __shared_ptr<_Tp1, _Lp>& __rhs) const
	{ return _M_refcount._M_less(__rhs._M_refcount); }

      template<typename _Tp1>
	bool
	owner_before(const __weak_ptr<_Tp1, _Lp>& __rhs) const
	{ return _M_refcount._M_less(__rhs._M_refcount); }

      void
      reset() noexcept
      { __weak_ptr().swap(*this); }

      void
      swap(__weak_ptr& __s) noexcept
      {
	std::swap(_M_ptr, __s._M_ptr);
	_M_refcount._M_swap(__s._M_refcount);
      }

    private:
      
      void
      _M_assign(_Tp* __ptr, const __shared_count<_Lp>& __refcount) noexcept
      {
	_M_ptr = __ptr;
	_M_refcount = __refcount;
      }

      template<typename _Tp1, _Lock_policy _Lp1> friend class __shared_ptr;
      template<typename _Tp1, _Lock_policy _Lp1> friend class __weak_ptr;
      friend class __enable_shared_from_this<_Tp, _Lp>;
      friend class enable_shared_from_this<_Tp>;

      _Tp*	 	 _M_ptr;         
      __weak_count<_Lp>  _M_refcount;    
    };

  
  template<typename _Tp, _Lock_policy _Lp>
    inline void
    swap(__weak_ptr<_Tp, _Lp>& __a, __weak_ptr<_Tp, _Lp>& __b) noexcept
    { __a.swap(__b); }

  template<typename _Tp, typename _Tp1>
    struct _Sp_owner_less : public binary_function<_Tp, _Tp, bool>
    {
      bool
      operator()(const _Tp& __lhs, const _Tp& __rhs) const
      { return __lhs.owner_before(__rhs); }

      bool
      operator()(const _Tp& __lhs, const _Tp1& __rhs) const
      { return __lhs.owner_before(__rhs); }

      bool
      operator()(const _Tp1& __lhs, const _Tp& __rhs) const
      { return __lhs.owner_before(__rhs); }
    };

  template<typename _Tp, _Lock_policy _Lp>
    struct owner_less<__shared_ptr<_Tp, _Lp>>
    : public _Sp_owner_less<__shared_ptr<_Tp, _Lp>, __weak_ptr<_Tp, _Lp>>
    { };

  template<typename _Tp, _Lock_policy _Lp>
    struct owner_less<__weak_ptr<_Tp, _Lp>>
    : public _Sp_owner_less<__weak_ptr<_Tp, _Lp>, __shared_ptr<_Tp, _Lp>>
    { };


  template<typename _Tp, _Lock_policy _Lp>
    class __enable_shared_from_this
    {
    protected:
      constexpr __enable_shared_from_this() noexcept { }

      __enable_shared_from_this(const __enable_shared_from_this&) noexcept { }

      __enable_shared_from_this&
      operator=(const __enable_shared_from_this&) noexcept
      { return *this; }

      ~__enable_shared_from_this() { }

    public:
      __shared_ptr<_Tp, _Lp>
      shared_from_this()
      { return __shared_ptr<_Tp, _Lp>(this->_M_weak_this); }

      __shared_ptr<const _Tp, _Lp>
      shared_from_this() const
      { return __shared_ptr<const _Tp, _Lp>(this->_M_weak_this); }

    private:
      template<typename _Tp1>
	void
	_M_weak_assign(_Tp1* __p, const __shared_count<_Lp>& __n) const noexcept
	{ _M_weak_this._M_assign(__p, __n); }

      template<typename _Tp1>
	friend void
	__enable_shared_from_this_helper(const __shared_count<_Lp>& __pn,
					 const __enable_shared_from_this* __pe,
					 const _Tp1* __px) noexcept
	{
	  if (__pe != 0)
	    __pe->_M_weak_assign(const_cast<_Tp1*>(__px), __pn);
	}

      mutable __weak_ptr<_Tp, _Lp>  _M_weak_this;
    };


  template<typename _Tp, _Lock_policy _Lp, typename _Alloc, typename... _Args>
    inline __shared_ptr<_Tp, _Lp>
    __allocate_shared(const _Alloc& __a, _Args&&... __args)
    {
      return __shared_ptr<_Tp, _Lp>(_Sp_make_shared_tag(), __a,
				    std::forward<_Args>(__args)...);
    }

  template<typename _Tp, _Lock_policy _Lp, typename... _Args>
    inline __shared_ptr<_Tp, _Lp>
    __make_shared(_Args&&... __args)
    {
      typedef typename std::remove_const<_Tp>::type _Tp_nc;
      return std::__allocate_shared<_Tp, _Lp>(std::allocator<_Tp_nc>(),
					      std::forward<_Args>(__args)...);
    }

  
  template<typename _Tp, _Lock_policy _Lp>
    struct hash<__shared_ptr<_Tp, _Lp>>
    : public __hash_base<size_t, __shared_ptr<_Tp, _Lp>>
    {
      size_t
      operator()(const __shared_ptr<_Tp, _Lp>& __s) const noexcept
      { return std::hash<_Tp*>()(__s.get()); }
    };


} 


namespace std 
{


  


 

  
  template<typename _Ch, typename _Tr, typename _Tp, _Lock_policy _Lp>
    inline std::basic_ostream<_Ch, _Tr>&
    operator<<(std::basic_ostream<_Ch, _Tr>& __os,
	       const __shared_ptr<_Tp, _Lp>& __p)
    {
      __os << __p.get();
      return __os;
    }

  
  template<typename _Del, typename _Tp, _Lock_policy _Lp>
    inline _Del*
    get_deleter(const __shared_ptr<_Tp, _Lp>& __p) noexcept
    {
      return static_cast<_Del*>(__p._M_get_deleter(typeid(_Del)));
    }


  




 
  template<typename _Tp>
    class shared_ptr : public __shared_ptr<_Tp>
    {
    public:
      


 
      constexpr shared_ptr() noexcept
      : __shared_ptr<_Tp>() { }

      shared_ptr(const shared_ptr&) noexcept = default;

      




 
      template<typename _Tp1>
	explicit shared_ptr(_Tp1* __p)
        : __shared_ptr<_Tp>(__p) { }

      











 
      template<typename _Tp1, typename _Deleter>
	shared_ptr(_Tp1* __p, _Deleter __d)
        : __shared_ptr<_Tp>(__p, __d) { }

      











 
      template<typename _Deleter>
	shared_ptr(nullptr_t __p, _Deleter __d)
        : __shared_ptr<_Tp>(__p, __d) { }

      













 
      template<typename _Tp1, typename _Deleter, typename _Alloc>
	shared_ptr(_Tp1* __p, _Deleter __d, _Alloc __a)
	: __shared_ptr<_Tp>(__p, __d, std::move(__a)) { }

      













 
      template<typename _Deleter, typename _Alloc>
	shared_ptr(nullptr_t __p, _Deleter __d, _Alloc __a)
	: __shared_ptr<_Tp>(__p, __d, std::move(__a)) { }

      

      














 
      template<typename _Tp1>
	shared_ptr(const shared_ptr<_Tp1>& __r, _Tp* __p) noexcept
	: __shared_ptr<_Tp>(__r, __p) { }

      





 
      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	shared_ptr(const shared_ptr<_Tp1>& __r) noexcept
        : __shared_ptr<_Tp>(__r) { }

      



 
      shared_ptr(shared_ptr&& __r) noexcept
      : __shared_ptr<_Tp>(std::move(__r)) { }

      



 
      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	shared_ptr(shared_ptr<_Tp1>&& __r) noexcept
	: __shared_ptr<_Tp>(std::move(__r)) { }

      






 
      template<typename _Tp1>
	explicit shared_ptr(const weak_ptr<_Tp1>& __r)
	: __shared_ptr<_Tp>(__r) { }

      template<typename _Tp1>
	shared_ptr(std::auto_ptr<_Tp1>&& __r);

      template<typename _Tp1, typename _Del>
	shared_ptr(std::unique_ptr<_Tp1, _Del>&& __r)
	: __shared_ptr<_Tp>(std::move(__r)) { }

      



 
      constexpr shared_ptr(nullptr_t __p) noexcept
      : __shared_ptr<_Tp>(__p) { }

      shared_ptr& operator=(const shared_ptr&) noexcept = default;

      template<typename _Tp1>
	shared_ptr&
	operator=(const shared_ptr<_Tp1>& __r) noexcept
	{
	  this->__shared_ptr<_Tp>::operator=(__r);
	  return *this;
	}

      template<typename _Tp1>
	shared_ptr&
	operator=(std::auto_ptr<_Tp1>&& __r)
	{
	  this->__shared_ptr<_Tp>::operator=(std::move(__r));
	  return *this;
	}

      shared_ptr&
      operator=(shared_ptr&& __r) noexcept
      {
	this->__shared_ptr<_Tp>::operator=(std::move(__r));
	return *this;
      }

      template<class _Tp1>
	shared_ptr&
	operator=(shared_ptr<_Tp1>&& __r) noexcept
	{
	  this->__shared_ptr<_Tp>::operator=(std::move(__r));
	  return *this;
	}

      template<typename _Tp1, typename _Del>
	shared_ptr&
	operator=(std::unique_ptr<_Tp1, _Del>&& __r)
	{
	  this->__shared_ptr<_Tp>::operator=(std::move(__r));
	  return *this;
	}

    private:
      
      template<typename _Alloc, typename... _Args>
	shared_ptr(_Sp_make_shared_tag __tag, const _Alloc& __a,
		   _Args&&... __args)
	: __shared_ptr<_Tp>(__tag, __a, std::forward<_Args>(__args)...)
	{ }

      template<typename _Tp1, typename _Alloc, typename... _Args>
	friend shared_ptr<_Tp1>
	allocate_shared(const _Alloc& __a, _Args&&... __args);
    };

  
  template<typename _Tp1, typename _Tp2>
    inline bool
    operator==(const shared_ptr<_Tp1>& __a,
	       const shared_ptr<_Tp2>& __b) noexcept
    { return __a.get() == __b.get(); }

  template<typename _Tp>
    inline bool
    operator==(const shared_ptr<_Tp>& __a, nullptr_t) noexcept
    { return !__a; }

  template<typename _Tp>
    inline bool
    operator==(nullptr_t, const shared_ptr<_Tp>& __a) noexcept
    { return !__a; }

  template<typename _Tp1, typename _Tp2>
    inline bool
    operator!=(const shared_ptr<_Tp1>& __a,
	       const shared_ptr<_Tp2>& __b) noexcept
    { return __a.get() != __b.get(); }

  template<typename _Tp>
    inline bool
    operator!=(const shared_ptr<_Tp>& __a, nullptr_t) noexcept
    { return (bool)__a; }

  template<typename _Tp>
    inline bool
    operator!=(nullptr_t, const shared_ptr<_Tp>& __a) noexcept
    { return (bool)__a; }

  template<typename _Tp1, typename _Tp2>
    inline bool
    operator<(const shared_ptr<_Tp1>& __a,
	      const shared_ptr<_Tp2>& __b) noexcept
    {
      typedef typename std::common_type<_Tp1*, _Tp2*>::type _CT;
      return std::less<_CT>()(__a.get(), __b.get());
    }

  template<typename _Tp>
    inline bool
    operator<(const shared_ptr<_Tp>& __a, nullptr_t) noexcept
    { return std::less<_Tp*>()(__a.get(), nullptr); }

  template<typename _Tp>
    inline bool
    operator<(nullptr_t, const shared_ptr<_Tp>& __a) noexcept
    { return std::less<_Tp*>()(nullptr, __a.get()); }

  template<typename _Tp1, typename _Tp2>
    inline bool
    operator<=(const shared_ptr<_Tp1>& __a,
	       const shared_ptr<_Tp2>& __b) noexcept
    { return !(__b < __a); }

  template<typename _Tp>
    inline bool
    operator<=(const shared_ptr<_Tp>& __a, nullptr_t) noexcept
    { return !(nullptr < __a); }

  template<typename _Tp>
    inline bool
    operator<=(nullptr_t, const shared_ptr<_Tp>& __a) noexcept
    { return !(__a < nullptr); }

  template<typename _Tp1, typename _Tp2>
    inline bool
    operator>(const shared_ptr<_Tp1>& __a,
	      const shared_ptr<_Tp2>& __b) noexcept
    { return (__b < __a); }

  template<typename _Tp>
    inline bool
    operator>(const shared_ptr<_Tp>& __a, nullptr_t) noexcept
    { return std::less<_Tp*>()(nullptr, __a.get()); }

  template<typename _Tp>
    inline bool
    operator>(nullptr_t, const shared_ptr<_Tp>& __a) noexcept
    { return std::less<_Tp*>()(__a.get(), nullptr); }

  template<typename _Tp1, typename _Tp2>
    inline bool
    operator>=(const shared_ptr<_Tp1>& __a,
	       const shared_ptr<_Tp2>& __b) noexcept
    { return !(__a < __b); }

  template<typename _Tp>
    inline bool
    operator>=(const shared_ptr<_Tp>& __a, nullptr_t) noexcept
    { return !(__a < nullptr); }

  template<typename _Tp>
    inline bool
    operator>=(nullptr_t, const shared_ptr<_Tp>& __a) noexcept
    { return !(nullptr < __a); }

  template<typename _Tp>
    struct less<shared_ptr<_Tp>> : public _Sp_less<shared_ptr<_Tp>>
    { };

  
  template<typename _Tp>
    inline void
    swap(shared_ptr<_Tp>& __a, shared_ptr<_Tp>& __b) noexcept
    { __a.swap(__b); }

  
  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    static_pointer_cast(const shared_ptr<_Tp1>& __r) noexcept
    { return shared_ptr<_Tp>(__r, static_cast<_Tp*>(__r.get())); }

  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    const_pointer_cast(const shared_ptr<_Tp1>& __r) noexcept
    { return shared_ptr<_Tp>(__r, const_cast<_Tp*>(__r.get())); }

  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    dynamic_pointer_cast(const shared_ptr<_Tp1>& __r) noexcept
    {
      if (_Tp* __p = dynamic_cast<_Tp*>(__r.get()))
	return shared_ptr<_Tp>(__r, __p);
      return shared_ptr<_Tp>();
    }


  



 
  template<typename _Tp>
    class weak_ptr : public __weak_ptr<_Tp>
    {
    public:
      constexpr weak_ptr() noexcept
      : __weak_ptr<_Tp>() { }

      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	weak_ptr(const weak_ptr<_Tp1>& __r) noexcept
	: __weak_ptr<_Tp>(__r) { }

      template<typename _Tp1, typename = typename
	       std::enable_if<std::is_convertible<_Tp1*, _Tp*>::value>::type>
	weak_ptr(const shared_ptr<_Tp1>& __r) noexcept
	: __weak_ptr<_Tp>(__r) { }

      template<typename _Tp1>
	weak_ptr&
	operator=(const weak_ptr<_Tp1>& __r) noexcept
	{
	  this->__weak_ptr<_Tp>::operator=(__r);
	  return *this;
	}

      template<typename _Tp1>
	weak_ptr&
	operator=(const shared_ptr<_Tp1>& __r) noexcept
	{
	  this->__weak_ptr<_Tp>::operator=(__r);
	  return *this;
	}

      shared_ptr<_Tp>
      lock() const noexcept
      {
	if (this->expired())
	  return shared_ptr<_Tp>();

	try
	  {
	    return shared_ptr<_Tp>(*this);
	  }
	catch(const bad_weak_ptr&)
	  {
	    return shared_ptr<_Tp>();
	  }
      }
    };

  
  template<typename _Tp>
    inline void
    swap(weak_ptr<_Tp>& __a, weak_ptr<_Tp>& __b) noexcept
    { __a.swap(__b); }


  
  template<typename _Tp>
    struct owner_less;

  
  template<typename _Tp>
    struct owner_less<shared_ptr<_Tp>>
    : public _Sp_owner_less<shared_ptr<_Tp>, weak_ptr<_Tp>>
    { };

  
  template<typename _Tp>
    struct owner_less<weak_ptr<_Tp>>
    : public _Sp_owner_less<weak_ptr<_Tp>, shared_ptr<_Tp>>
    { };

  

 
  template<typename _Tp>
    class enable_shared_from_this
    {
    protected:
      constexpr enable_shared_from_this() noexcept { }

      enable_shared_from_this(const enable_shared_from_this&) noexcept { }

      enable_shared_from_this&
      operator=(const enable_shared_from_this&) noexcept
      { return *this; }

      ~enable_shared_from_this() { }

    public:
      shared_ptr<_Tp>
      shared_from_this()
      { return shared_ptr<_Tp>(this->_M_weak_this); }

      shared_ptr<const _Tp>
      shared_from_this() const
      { return shared_ptr<const _Tp>(this->_M_weak_this); }

    private:
      template<typename _Tp1>
	void
	_M_weak_assign(_Tp1* __p, const __shared_count<>& __n) const noexcept
	{ _M_weak_this._M_assign(__p, __n); }

      template<typename _Tp1>
	friend void
	__enable_shared_from_this_helper(const __shared_count<>& __pn,
					 const enable_shared_from_this* __pe,
					 const _Tp1* __px) noexcept
	{
	  if (__pe != 0)
	    __pe->_M_weak_assign(const_cast<_Tp1*>(__px), __pn);
	}

      mutable weak_ptr<_Tp>  _M_weak_this;
    };

  









 
  template<typename _Tp, typename _Alloc, typename... _Args>
    inline shared_ptr<_Tp>
    allocate_shared(const _Alloc& __a, _Args&&... __args)
    {
      return shared_ptr<_Tp>(_Sp_make_shared_tag(), __a,
			     std::forward<_Args>(__args)...);
    }

  





 
  template<typename _Tp, typename... _Args>
    inline shared_ptr<_Tp>
    make_shared(_Args&&... __args)
    {
      typedef typename std::remove_const<_Tp>::type _Tp_nc;
      return std::allocate_shared<_Tp>(std::allocator<_Tp_nc>(),
				       std::forward<_Args>(__args)...);
    }

  
  template<typename _Tp>
    struct hash<shared_ptr<_Tp>>
    : public __hash_base<size_t, shared_ptr<_Tp>>
    {
      size_t
      operator()(const shared_ptr<_Tp>& __s) const noexcept
      { return std::hash<_Tp*>()(__s.get()); }
    };

  


} 




























 



namespace std 
{


  





 
  template<typename _Tp1>
    struct auto_ptr_ref
    {
      _Tp1* _M_ptr;
      
      explicit
      auto_ptr_ref(_Tp1* __p): _M_ptr(__p) { }
    } __attribute__ ((__deprecated__));


  



























 
  template<typename _Tp>
    class auto_ptr
    {
    private:
      _Tp* _M_ptr;
      
    public:
      
      typedef _Tp element_type;
      
      




 
      explicit
      auto_ptr(element_type* __p = 0) throw() : _M_ptr(__p) { }

      





 
      auto_ptr(auto_ptr& __a) throw() : _M_ptr(__a.release()) { }

      








 
      template<typename _Tp1>
        auto_ptr(auto_ptr<_Tp1>& __a) throw() : _M_ptr(__a.release()) { }

      






 
      auto_ptr&
      operator=(auto_ptr& __a) throw()
      {
	reset(__a.release());
	return *this;
      }

      








 
      template<typename _Tp1>
        auto_ptr&
        operator=(auto_ptr<_Tp1>& __a) throw()
        {
	  reset(__a.release());
	  return *this;
	}

      








 
      ~auto_ptr() { delete _M_ptr; }
      
      






 
      element_type&
      operator*() const throw() 
      {
	;
	return *_M_ptr; 
      }
      
      




 
      element_type*
      operator->() const throw() 
      {
	;
	return _M_ptr; 
      }
      
      








 
      element_type*
      get() const throw() { return _M_ptr; }
      
      









 
      element_type*
      release() throw()
      {
	element_type* __tmp = _M_ptr;
	_M_ptr = 0;
	return __tmp;
      }
      
      





 
      void
      reset(element_type* __p = 0) throw()
      {
	if (__p != _M_ptr)
	  {
	    delete _M_ptr;
	    _M_ptr = __p;
	  }
      }
      
      









 
      auto_ptr(auto_ptr_ref<element_type> __ref) throw()
      : _M_ptr(__ref._M_ptr) { }
      
      auto_ptr&
      operator=(auto_ptr_ref<element_type> __ref) throw()
      {
	if (__ref._M_ptr != this->get())
	  {
	    delete _M_ptr;
	    _M_ptr = __ref._M_ptr;
	  }
	return *this;
      }
      
      template<typename _Tp1>
        operator auto_ptr_ref<_Tp1>() throw()
        { return auto_ptr_ref<_Tp1>(this->release()); }

      template<typename _Tp1>
        operator auto_ptr<_Tp1>() throw()
        { return auto_ptr<_Tp1>(this->release()); }
    } __attribute__ ((__deprecated__));

  
  
  template<>
    class auto_ptr<void>
    {
    public:
      typedef void element_type;
    } __attribute__ ((__deprecated__));

  template<_Lock_policy _Lp>
  template<typename _Tp>
    inline
    __shared_count<_Lp>::__shared_count(std::auto_ptr<_Tp>&& __r)
    : _M_pi(new _Sp_counted_ptr<_Tp*, _Lp>(__r.get()))
    { __r.release(); }

  template<typename _Tp, _Lock_policy _Lp>
  template<typename _Tp1>
    inline
    __shared_ptr<_Tp, _Lp>::__shared_ptr(std::auto_ptr<_Tp1>&& __r)
    : _M_ptr(__r.get()), _M_refcount()
    {
      
      static_assert( sizeof(_Tp1) > 0, "incomplete type" );
      _Tp1* __tmp = __r.get();
      _M_refcount = __shared_count<_Lp>(std::move(__r));
      __enable_shared_from_this_helper(_M_refcount, __tmp, __tmp);
    }

  template<typename _Tp>
  template<typename _Tp1>
    inline
    shared_ptr<_Tp>::shared_ptr(std::auto_ptr<_Tp1>&& __r)
    : __shared_ptr<_Tp>(std::move(__r)) { }

  template<typename _Tp, typename _Dp>
  template<typename _Up, typename>
    inline
    unique_ptr<_Tp, _Dp>::unique_ptr(auto_ptr<_Up>&& __u) noexcept
    : _M_t(__u.release(), deleter_type()) { }


} 




























 


#pragma GCC system_header


namespace std 
{
namespace tr1
{


  


 

  struct __sfinae_types
  {
    typedef char __one;
    typedef struct { char __arr[2]; } __two;
  };





  

  
  template<typename _Tp, _Tp __v>
    struct integral_constant
    {
      static const _Tp                      value = __v;
      typedef _Tp                           value_type;
      typedef integral_constant<_Tp, __v>   type;
    };
  
  
  typedef integral_constant<bool, true>     true_type;

  
  typedef integral_constant<bool, false>    false_type;

  template<typename _Tp, _Tp __v>
    const _Tp integral_constant<_Tp, __v>::value;

  
  template<typename>
    struct remove_cv;

  template<typename>
    struct __is_void_helper
    : public false_type { };
  template< > struct __is_void_helper<void> : public integral_constant<bool, true> { };

  

  
  template<typename _Tp>
    struct is_void
    : public integral_constant<bool, (__is_void_helper<typename
				      remove_cv<_Tp>::type>::value)>
    { };

  template<typename>
    struct __is_integral_helper
    : public false_type { };
  template< > struct __is_integral_helper<bool> : public integral_constant<bool, true> { };
  template< > struct __is_integral_helper<char> : public integral_constant<bool, true> { };
  template< > struct __is_integral_helper<signed char> : public integral_constant<bool, true> { };
  template< > struct __is_integral_helper<unsigned char> : public integral_constant<bool, true> { };
  template< > struct __is_integral_helper<wchar_t> : public integral_constant<bool, true> { };
  template< > struct __is_integral_helper<short> : public integral_constant<bool, true> { };
  template< > struct __is_integral_helper<unsigned short> : public integral_constant<bool, true> { };
  template< > struct __is_integral_helper<int> : public integral_constant<bool, true> { };
  template< > struct __is_integral_helper<unsigned int> : public integral_constant<bool, true> { };
  template< > struct __is_integral_helper<long> : public integral_constant<bool, true> { };
  template< > struct __is_integral_helper<unsigned long> : public integral_constant<bool, true> { };
  template< > struct __is_integral_helper<long long> : public integral_constant<bool, true> { };
  template< > struct __is_integral_helper<unsigned long long> : public integral_constant<bool, true> { };

  
  template<typename _Tp>
    struct is_integral
    : public integral_constant<bool, (__is_integral_helper<typename
				      remove_cv<_Tp>::type>::value)>
    { };

  template<typename>
    struct __is_floating_point_helper
    : public false_type { };
  template< > struct __is_floating_point_helper<float> : public integral_constant<bool, true> { };
  template< > struct __is_floating_point_helper<double> : public integral_constant<bool, true> { };
  template< > struct __is_floating_point_helper<long double> : public integral_constant<bool, true> { };

  
  template<typename _Tp>
    struct is_floating_point
    : public integral_constant<bool, (__is_floating_point_helper<typename
				      remove_cv<_Tp>::type>::value)>
    { };

  
  template<typename>
    struct is_array
    : public false_type { };

  template<typename _Tp, std::size_t _Size>
    struct is_array<_Tp[_Size]>
    : public true_type { };

  template<typename _Tp>
    struct is_array<_Tp[]>
    : public true_type { };

  template<typename>
    struct __is_pointer_helper
    : public false_type { };
  template<typename _Tp> struct __is_pointer_helper<_Tp* > : public integral_constant<bool, true> { };

  
  template<typename _Tp>
    struct is_pointer
    : public integral_constant<bool, (__is_pointer_helper<typename
				      remove_cv<_Tp>::type>::value)>
    { };

  
  template<typename _Tp>
    struct is_reference;

  
  template<typename _Tp>
    struct is_function;

  template<typename>
    struct __is_member_object_pointer_helper
    : public false_type { };
  template<typename _Tp, typename _Cp> struct __is_member_object_pointer_helper<_Tp _Cp:: * > : public integral_constant<bool, !is_function<_Tp> ::value> { };

  
  template<typename _Tp>
    struct is_member_object_pointer
    : public integral_constant<bool, (__is_member_object_pointer_helper<
				      typename remove_cv<_Tp>::type>::value)>
    { };

  template<typename>
    struct __is_member_function_pointer_helper
    : public false_type { };
  template<typename _Tp, typename _Cp> struct __is_member_function_pointer_helper<_Tp _Cp:: * > : public integral_constant<bool, is_function<_Tp> ::value> { };

  
  template<typename _Tp>
    struct is_member_function_pointer
    : public integral_constant<bool, (__is_member_function_pointer_helper<
				      typename remove_cv<_Tp>::type>::value)>
    { };

  
  template<typename _Tp>
    struct is_enum
    : public integral_constant<bool, __is_enum(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_union
    : public integral_constant<bool, __is_union(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_class
    : public integral_constant<bool, __is_class(_Tp)>
    { };

  
  template<typename>
    struct is_function
    : public false_type { };
  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...)>
    : public true_type { };
  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......)>
    : public true_type { };
  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) const>
    : public true_type { };
  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) const>
    : public true_type { };
  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) volatile>
    : public true_type { };
  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) volatile>
    : public true_type { };
  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) const volatile>
    : public true_type { };
  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) const volatile>
    : public true_type { };

  
  
  
  template<typename _Tp>
    struct is_arithmetic
    : public integral_constant<bool, (is_integral<_Tp>::value
				      || is_floating_point<_Tp>::value)>
    { };

  
  template<typename _Tp>
    struct is_fundamental
    : public integral_constant<bool, (is_arithmetic<_Tp>::value
				      || is_void<_Tp>::value)>
    { };

  
  template<typename _Tp>
    struct is_object
    : public integral_constant<bool, !(is_function<_Tp>::value
				       || is_reference<_Tp>::value
				       || is_void<_Tp>::value)>
    { };

  
  template<typename _Tp>
    struct is_member_pointer;

  
  template<typename _Tp>
    struct is_scalar
    : public integral_constant<bool, (is_arithmetic<_Tp>::value
				      || is_enum<_Tp>::value
				      || is_pointer<_Tp>::value
				      || is_member_pointer<_Tp>::value)>
    { };

  
  template<typename _Tp>
    struct is_compound
    : public integral_constant<bool, !is_fundamental<_Tp>::value> { };

  
  template<typename _Tp>
    struct __is_member_pointer_helper
    : public false_type { };
  template<typename _Tp, typename _Cp> struct __is_member_pointer_helper<_Tp _Cp:: * > : public integral_constant<bool, true> { };

  template<typename _Tp>
  struct is_member_pointer
    : public integral_constant<bool, (__is_member_pointer_helper<
				      typename remove_cv<_Tp>::type>::value)>
    { };

  
  
  template<typename>
    struct is_const
    : public false_type { };

  template<typename _Tp>
    struct is_const<_Tp const>
    : public true_type { };
  
  
  template<typename>
    struct is_volatile
    : public false_type { };

  template<typename _Tp>
    struct is_volatile<_Tp volatile>
    : public true_type { };

  
  template<typename _Tp>
    struct is_empty
    : public integral_constant<bool, __is_empty(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_polymorphic
    : public integral_constant<bool, __is_polymorphic(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_abstract
    : public integral_constant<bool, __is_abstract(_Tp)>
    { };

  
  template<typename _Tp>
    struct has_virtual_destructor
    : public integral_constant<bool, __has_virtual_destructor(_Tp)>
    { };

  
  template<typename _Tp>
    struct alignment_of
    : public integral_constant<std::size_t, __alignof__(_Tp)> { };
  
  
  template<typename>
    struct rank
    : public integral_constant<std::size_t, 0> { };
   
  template<typename _Tp, std::size_t _Size>
    struct rank<_Tp[_Size]>
    : public integral_constant<std::size_t, 1 + rank<_Tp>::value> { };

  template<typename _Tp>
    struct rank<_Tp[]>
    : public integral_constant<std::size_t, 1 + rank<_Tp>::value> { };

  
  template<typename, unsigned _Uint = 0>
    struct extent
    : public integral_constant<std::size_t, 0> { };
  
  template<typename _Tp, unsigned _Uint, std::size_t _Size>
    struct extent<_Tp[_Size], _Uint>
    : public integral_constant<std::size_t,
			       _Uint == 0 ? _Size : extent<_Tp,
							   _Uint - 1>::value>
    { };

  template<typename _Tp, unsigned _Uint>
    struct extent<_Tp[], _Uint>
    : public integral_constant<std::size_t,
			       _Uint == 0 ? 0 : extent<_Tp,
						       _Uint - 1>::value>
    { };

  

  
  template<typename, typename>
    struct is_same
    : public false_type { };

  template<typename _Tp>
    struct is_same<_Tp, _Tp>
    : public true_type { };

  

  
  template<typename _Tp>
    struct remove_const
    { typedef _Tp     type; };

  template<typename _Tp>
    struct remove_const<_Tp const>
    { typedef _Tp     type; };
  
  
  template<typename _Tp>
    struct remove_volatile
    { typedef _Tp     type; };

  template<typename _Tp>
    struct remove_volatile<_Tp volatile>
    { typedef _Tp     type; };
  
  
  template<typename _Tp>
    struct remove_cv
    {
      typedef typename
      remove_const<typename remove_volatile<_Tp>::type>::type     type;
    };
  
  
  template<typename _Tp>
    struct add_const
    { typedef _Tp const     type; };
   
  
  template<typename _Tp>
    struct add_volatile
    { typedef _Tp volatile     type; };
  
  
  template<typename _Tp>
    struct add_cv
    {
      typedef typename
      add_const<typename add_volatile<_Tp>::type>::type     type;
    };

  

  
  template<typename _Tp>
    struct remove_extent
    { typedef _Tp     type; };

  template<typename _Tp, std::size_t _Size>
    struct remove_extent<_Tp[_Size]>
    { typedef _Tp     type; };

  template<typename _Tp>
    struct remove_extent<_Tp[]>
    { typedef _Tp     type; };

  
  template<typename _Tp>
    struct remove_all_extents
    { typedef _Tp     type; };

  template<typename _Tp, std::size_t _Size>
    struct remove_all_extents<_Tp[_Size]>
    { typedef typename remove_all_extents<_Tp>::type     type; };

  template<typename _Tp>
    struct remove_all_extents<_Tp[]>
    { typedef typename remove_all_extents<_Tp>::type     type; };

  

  template<typename _Tp, typename>
    struct __remove_pointer_helper
    { typedef _Tp     type; };

  template<typename _Tp, typename _Up>
    struct __remove_pointer_helper<_Tp, _Up*>
    { typedef _Up     type; };

  
  template<typename _Tp>
    struct remove_pointer
    : public __remove_pointer_helper<_Tp, typename remove_cv<_Tp>::type>
    { };

  template<typename>
    struct remove_reference;

  
  template<typename _Tp>
    struct add_pointer
    { typedef typename remove_reference<_Tp>::type*     type; };

  template<typename>
    struct is_reference
    : public false_type { };

  template<typename _Tp>
    struct is_reference<_Tp&>
    : public true_type { };

  template<typename _Tp>
    struct is_pod
    : public integral_constant<bool, __is_pod(_Tp) || is_void<_Tp>::value>
    { };

  template<typename _Tp>
    struct has_trivial_constructor
    : public integral_constant<bool, is_pod<_Tp>::value>
    { };

  template<typename _Tp>
    struct has_trivial_copy
    : public integral_constant<bool, is_pod<_Tp>::value>
    { };

  template<typename _Tp>
    struct has_trivial_assign
    : public integral_constant<bool, is_pod<_Tp>::value>
    { };

  template<typename _Tp>
    struct has_trivial_destructor
    : public integral_constant<bool, is_pod<_Tp>::value>
    { };

  template<typename _Tp>
    struct has_nothrow_constructor
    : public integral_constant<bool, is_pod<_Tp>::value>
    { };

  template<typename _Tp>
    struct has_nothrow_copy
    : public integral_constant<bool, is_pod<_Tp>::value>
    { };

  template<typename _Tp>
    struct has_nothrow_assign
    : public integral_constant<bool, is_pod<_Tp>::value>
    { };

  template<typename>
    struct __is_signed_helper
    : public false_type { };
  template< > struct __is_signed_helper<signed char> : public integral_constant<bool, true> { };
  template< > struct __is_signed_helper<short> : public integral_constant<bool, true> { };
  template< > struct __is_signed_helper<int> : public integral_constant<bool, true> { };
  template< > struct __is_signed_helper<long> : public integral_constant<bool, true> { };
  template< > struct __is_signed_helper<long long> : public integral_constant<bool, true> { };

  template<typename _Tp>
    struct is_signed
    : public integral_constant<bool, (__is_signed_helper<typename
				      remove_cv<_Tp>::type>::value)>
    { };

  template<typename>
    struct __is_unsigned_helper
    : public false_type { };
  template< > struct __is_unsigned_helper<unsigned char> : public integral_constant<bool, true> { };
  template< > struct __is_unsigned_helper<unsigned short> : public integral_constant<bool, true> { };
  template< > struct __is_unsigned_helper<unsigned int> : public integral_constant<bool, true> { };
  template< > struct __is_unsigned_helper<unsigned long> : public integral_constant<bool, true> { };
  template< > struct __is_unsigned_helper<unsigned long long> : public integral_constant<bool, true> { };

  template<typename _Tp>
    struct is_unsigned
    : public integral_constant<bool, (__is_unsigned_helper<typename
				      remove_cv<_Tp>::type>::value)>
    { };

  template<typename _Base, typename _Derived>
    struct __is_base_of_helper
    {
      typedef typename remove_cv<_Base>::type    _NoCv_Base;
      typedef typename remove_cv<_Derived>::type _NoCv_Derived;
      static const bool __value = (is_same<_Base, _Derived>::value
				   || (__is_base_of(_Base, _Derived)
				       && !is_same<_NoCv_Base,
				                   _NoCv_Derived>::value));
    };
 
  template<typename _Base, typename _Derived>
    struct is_base_of
    : public integral_constant<bool,
			       __is_base_of_helper<_Base, _Derived>::__value>
    { };

  template<typename _From, typename _To>
    struct __is_convertible_simple
    : public __sfinae_types
    {
    private:
      static __one __test(_To);
      static __two __test(...);
      static _From __makeFrom();
    
    public:
      static const bool __value = sizeof(__test(__makeFrom())) == 1;
    };

  template<typename _Tp>
    struct add_reference;

  template<typename _Tp>
    struct __is_int_or_cref
    {
      typedef typename remove_reference<_Tp>::type __rr_Tp;
      static const bool __value = (is_integral<_Tp>::value
				   || (is_integral<__rr_Tp>::value
				       && is_const<__rr_Tp>::value
				       && !is_volatile<__rr_Tp>::value));
    };

  template<typename _From, typename _To,
	   bool = (is_void<_From>::value || is_void<_To>::value
		   || is_function<_To>::value || is_array<_To>::value
		   
		   || (is_floating_point<typename
		       remove_reference<_From>::type>::value
		       && __is_int_or_cref<_To>::__value))>
    struct __is_convertible_helper
    {
      
      static const bool __value = (__is_convertible_simple<typename
				   add_reference<_From>::type, _To>::__value);
    };

  template<typename _From, typename _To>
    struct __is_convertible_helper<_From, _To, true>
    { static const bool __value = (is_void<_To>::value
				   || (__is_int_or_cref<_To>::__value
				       && !is_void<_From>::value)); };

  template<typename _From, typename _To>
    struct is_convertible
    : public integral_constant<bool,
			       __is_convertible_helper<_From, _To>::__value>
    { };

  
  template<typename _Tp>
    struct remove_reference
    { typedef _Tp     type; };

  template<typename _Tp>
    struct remove_reference<_Tp&>
    { typedef _Tp     type; };

  
  template<typename _Tp, bool = (is_void<_Tp>::value
				 || is_reference<_Tp>::value)>
    struct __add_reference_helper
    { typedef _Tp&    type; };

  template<typename _Tp>
    struct __add_reference_helper<_Tp, true>
    { typedef _Tp     type; };

  template<typename _Tp>
    struct add_reference
    : public __add_reference_helper<_Tp>
    { };

  
  template<std::size_t _Len, std::size_t _Align>
    struct aligned_storage
    { 
      union type
      {
	unsigned char __data[_Len];
	struct __attribute__((__aligned__((_Align)))) { } __align; 
      };
    };


  


}
}















































 


namespace std 
{
namespace tr1
{


 


 
  class bad_weak_ptr : public std::exception
  {
  public:
    virtual char const*
    what() const throw()
    { return "tr1::bad_weak_ptr"; }
  };

  
  inline void
  __throw_bad_weak_ptr()
  { (throw (bad_weak_ptr())); }

  using __gnu_cxx::_Lock_policy;
  using __gnu_cxx::__default_lock_policy;
  using __gnu_cxx::_S_single;
  using __gnu_cxx::_S_mutex;
  using __gnu_cxx::_S_atomic;

  
  template<_Lock_policy _Lp>
    class _Mutex_base
    {
    protected:
      
      enum { _S_need_barriers = 0 };
    };

  template<>
    class _Mutex_base<_S_mutex>
    : public __gnu_cxx::__mutex
    {
    protected:
      
      
      
      enum { _S_need_barriers = 1 };
    };

  template<_Lock_policy _Lp = __default_lock_policy>
    class _Sp_counted_base
    : public _Mutex_base<_Lp>
    {
    public:  
      _Sp_counted_base()
      : _M_use_count(1), _M_weak_count(1) { }
      
      virtual
      ~_Sp_counted_base() 
      { }
  
      
      
      virtual void
      _M_dispose() = 0; 
      
      
      virtual void
      _M_destroy() 
      { delete this; }
      
      virtual void*
      _M_get_deleter(const std::type_info&) = 0;

      void
      _M_add_ref_copy()
      { __gnu_cxx::__atomic_add_dispatch(&_M_use_count, 1); }
  
      void
      _M_add_ref_lock();
      
      void
      _M_release() 
      {
        
        ;
	if (__gnu_cxx::__exchange_and_add_dispatch(&_M_use_count, -1) == 1)
	  {
            ;
	    _M_dispose();
	    
	    
	    
	    
	    if (_Mutex_base<_Lp>::_S_need_barriers)
	      {
	        __asm __volatile ("" :: : "memory");
	        __asm __volatile ("" :: : "memory");
	      }

            
            ;
	    if (__gnu_cxx::__exchange_and_add_dispatch(&_M_weak_count,
						       -1) == 1)
              {
                ;
	        _M_destroy();
              }
	  }
      }
  
      void
      _M_weak_add_ref() 
      { __gnu_cxx::__atomic_add_dispatch(&_M_weak_count, 1); }

      void
      _M_weak_release() 
      {
        
        ;
	if (__gnu_cxx::__exchange_and_add_dispatch(&_M_weak_count, -1) == 1)
	  {
            ;
	    if (_Mutex_base<_Lp>::_S_need_barriers)
	      {
	        
	        
	        __asm __volatile ("" :: : "memory");
	        __asm __volatile ("" :: : "memory");
	      }
	    _M_destroy();
	  }
      }
  
      long
      _M_get_use_count() const 
      {
        
        
        return const_cast<const volatile _Atomic_word&>(_M_use_count);
      }

    private:  
      _Sp_counted_base(_Sp_counted_base const&);
      _Sp_counted_base& operator=(_Sp_counted_base const&);

      _Atomic_word  _M_use_count;     
      _Atomic_word  _M_weak_count;    
    };

  template<>
    inline void
    _Sp_counted_base<_S_single>::
    _M_add_ref_lock()
    {
      if (__gnu_cxx::__exchange_and_add_dispatch(&_M_use_count, 1) == 0)
	{
	  _M_use_count = 0;
	  __throw_bad_weak_ptr();
	}
    }

  template<>
    inline void
    _Sp_counted_base<_S_mutex>::
    _M_add_ref_lock()
    {
      __gnu_cxx::__scoped_lock sentry(*this);
      if (__gnu_cxx::__exchange_and_add_dispatch(&_M_use_count, 1) == 0)
	{
	  _M_use_count = 0;
	  __throw_bad_weak_ptr();
	}
    }

  template<> 
    inline void
    _Sp_counted_base<_S_atomic>::
    _M_add_ref_lock()
    {
      
      _Atomic_word __count = _M_use_count;
      do
	{
	  if (__count == 0)
	    __throw_bad_weak_ptr();
	  
	  
	}
      while (!__atomic_compare_exchange_n(&_M_use_count, &__count, __count + 1,
					  true, 4, 
					  0));
     }

  template<typename _Ptr, typename _Deleter, _Lock_policy _Lp>
    class _Sp_counted_base_impl
    : public _Sp_counted_base<_Lp>
    {
    public:
      
      _Sp_counted_base_impl(_Ptr __p, _Deleter __d)
      : _M_ptr(__p), _M_del(__d) { }
    
      virtual void
      _M_dispose() 
      { _M_del(_M_ptr); }
      
      virtual void*
      _M_get_deleter(const std::type_info& __ti)
      {
        return __ti == typeid(_Deleter) ? &_M_del : 0;
      }
      
    private:
      _Sp_counted_base_impl(const _Sp_counted_base_impl&);
      _Sp_counted_base_impl& operator=(const _Sp_counted_base_impl&);
      
      _Ptr      _M_ptr;  
      _Deleter  _M_del;  
    };

  template<_Lock_policy _Lp = __default_lock_policy>
    class __weak_count;

  template<typename _Tp>
    struct _Sp_deleter
    {
      typedef void result_type;
      typedef _Tp* argument_type;
      void operator()(_Tp* __p) const { delete __p; }
    };

  template<_Lock_policy _Lp = __default_lock_policy>
    class __shared_count
    {
    public: 
      __shared_count()
      : _M_pi(0) 
      { }
  
      template<typename _Ptr>
        __shared_count(_Ptr __p) : _M_pi(0)
        {
	  try
	    {
	      typedef typename std::tr1::remove_pointer<_Ptr>::type _Tp;
	      _M_pi = new _Sp_counted_base_impl<_Ptr, _Sp_deleter<_Tp>, _Lp>(
	          __p, _Sp_deleter<_Tp>());
	    }
	  catch(...)
	    {
	      delete __p;
	      throw;
	    }
	}

      template<typename _Ptr, typename _Deleter>
        __shared_count(_Ptr __p, _Deleter __d) : _M_pi(0)
        {
	  try
	    {
	      _M_pi = new _Sp_counted_base_impl<_Ptr, _Deleter, _Lp>(__p, __d);
	    }
	  catch(...)
	    {
	      __d(__p); 
	      throw;
	    }
	}

      
      template<typename _Tp>
        explicit
        __shared_count(std::auto_ptr<_Tp>& __r)
	: _M_pi(new _Sp_counted_base_impl<_Tp*,
		_Sp_deleter<_Tp>, _Lp >(__r.get(), _Sp_deleter<_Tp>()))
        { __r.release(); }

      
      explicit
      __shared_count(const __weak_count<_Lp>& __r);
  
      ~__shared_count() 
      {
	if (_M_pi != 0)
	  _M_pi->_M_release();
      }
      
      __shared_count(const __shared_count& __r)
      : _M_pi(__r._M_pi) 
      {
	if (_M_pi != 0)
	  _M_pi->_M_add_ref_copy();
      }
  
      __shared_count&
      operator=(const __shared_count& __r) 
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	if (__tmp != _M_pi)
	  {
	    if (__tmp != 0)
	      __tmp->_M_add_ref_copy();
	    if (_M_pi != 0)
	      _M_pi->_M_release();
	    _M_pi = __tmp;
	  }
	return *this;
      }
  
      void
      _M_swap(__shared_count& __r) 
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	__r._M_pi = _M_pi;
	_M_pi = __tmp;
      }
  
      long
      _M_get_use_count() const 
      { return _M_pi != 0 ? _M_pi->_M_get_use_count() : 0; }

      bool
      _M_unique() const 
      { return this->_M_get_use_count() == 1; }
      
      friend inline bool
      operator==(const __shared_count& __a, const __shared_count& __b)
      { return __a._M_pi == __b._M_pi; }
  
      friend inline bool
      operator<(const __shared_count& __a, const __shared_count& __b)
      { return std::less<_Sp_counted_base<_Lp>*>()(__a._M_pi, __b._M_pi); }
  
      void*
      _M_get_deleter(const std::type_info& __ti) const
      { return _M_pi ? _M_pi->_M_get_deleter(__ti) : 0; }

    private:
      friend class __weak_count<_Lp>;

      _Sp_counted_base<_Lp>*  _M_pi;
    };


  template<_Lock_policy _Lp>
    class __weak_count
    {
    public:
      __weak_count()
      : _M_pi(0) 
      { }
  
      __weak_count(const __shared_count<_Lp>& __r)
      : _M_pi(__r._M_pi) 
      {
	if (_M_pi != 0)
	  _M_pi->_M_weak_add_ref();
      }
      
      __weak_count(const __weak_count<_Lp>& __r)
      : _M_pi(__r._M_pi) 
      {
	if (_M_pi != 0)
	  _M_pi->_M_weak_add_ref();
      }
      
      ~__weak_count() 
      {
	if (_M_pi != 0)
	  _M_pi->_M_weak_release();
      }
      
      __weak_count<_Lp>&
      operator=(const __shared_count<_Lp>& __r) 
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	if (__tmp != 0)
	  __tmp->_M_weak_add_ref();
	if (_M_pi != 0)
	  _M_pi->_M_weak_release();
	_M_pi = __tmp;  
	return *this;
      }
      
      __weak_count<_Lp>&
      operator=(const __weak_count<_Lp>& __r) 
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	if (__tmp != 0)
	  __tmp->_M_weak_add_ref();
	if (_M_pi != 0)
	  _M_pi->_M_weak_release();
	_M_pi = __tmp;
	return *this;
      }

      void
      _M_swap(__weak_count<_Lp>& __r) 
      {
	_Sp_counted_base<_Lp>* __tmp = __r._M_pi;
	__r._M_pi = _M_pi;
	_M_pi = __tmp;
      }
  
      long
      _M_get_use_count() const 
      { return _M_pi != 0 ? _M_pi->_M_get_use_count() : 0; }

      friend inline bool
      operator==(const __weak_count<_Lp>& __a, const __weak_count<_Lp>& __b)
      { return __a._M_pi == __b._M_pi; }
      
      friend inline bool
      operator<(const __weak_count<_Lp>& __a, const __weak_count<_Lp>& __b)
      { return std::less<_Sp_counted_base<_Lp>*>()(__a._M_pi, __b._M_pi); }

    private:
      friend class __shared_count<_Lp>;

      _Sp_counted_base<_Lp>*  _M_pi;
    };

  
  template<_Lock_policy _Lp>
    inline
    __shared_count<_Lp>::
    __shared_count(const __weak_count<_Lp>& __r)
    : _M_pi(__r._M_pi)
    {
      if (_M_pi != 0)
	_M_pi->_M_add_ref_lock();
      else
	__throw_bad_weak_ptr();
    }

  
  template<typename _Tp, _Lock_policy _Lp = __default_lock_policy>
    class __shared_ptr;
  
  template<typename _Tp, _Lock_policy _Lp = __default_lock_policy>
    class __weak_ptr;

  template<typename _Tp, _Lock_policy _Lp = __default_lock_policy>
    class __enable_shared_from_this;

  template<typename _Tp>
    class shared_ptr;
  
  template<typename _Tp>
    class weak_ptr;

  template<typename _Tp>
    class enable_shared_from_this;

  

  
  template<_Lock_policy _Lp, typename _Tp1, typename _Tp2>
    void
    __enable_shared_from_this_helper(const __shared_count<_Lp>&,
				     const __enable_shared_from_this<_Tp1,
				     _Lp>*, const _Tp2*);

  
  template<typename _Tp1, typename _Tp2>
    void
    __enable_shared_from_this_helper(const __shared_count<>&,
				     const enable_shared_from_this<_Tp1>*,
				     const _Tp2*);

  template<_Lock_policy _Lp>
    inline void
    __enable_shared_from_this_helper(const __shared_count<_Lp>&, ...)
    { }


  struct __static_cast_tag { };
  struct __const_cast_tag { };
  struct __dynamic_cast_tag { };

  
  
  
  template<typename _Tp, _Lock_policy _Lp>
    class __shared_ptr
    {
    public:
      typedef _Tp   element_type;
      
      __shared_ptr()
      : _M_ptr(0), _M_refcount() 
      { }

      template<typename _Tp1>
        explicit
        __shared_ptr(_Tp1* __p)
	: _M_ptr(__p), _M_refcount(__p)
        {
	  
	  typedef int _IsComplete[sizeof(_Tp1)];
	  __enable_shared_from_this_helper(_M_refcount, __p, __p);
	}

      template<typename _Tp1, typename _Deleter>
        __shared_ptr(_Tp1* __p, _Deleter __d)
        : _M_ptr(__p), _M_refcount(__p, __d)
        {
	  
	  
	  __enable_shared_from_this_helper(_M_refcount, __p, __p);
	}
      
      
      
      template<typename _Tp1>
        __shared_ptr(const __shared_ptr<_Tp1, _Lp>& __r)
	: _M_ptr(__r._M_ptr), _M_refcount(__r._M_refcount) 
        {  }

      template<typename _Tp1>
        explicit
        __shared_ptr(const __weak_ptr<_Tp1, _Lp>& __r)
	: _M_refcount(__r._M_refcount) 
        {
	  
	  
	  
	  _M_ptr = __r._M_ptr;
	}

      
      template<typename _Tp1>
        explicit
        __shared_ptr(std::auto_ptr<_Tp1>& __r)
	: _M_ptr(__r.get()), _M_refcount()
        { 
	  
	  typedef int _IsComplete[sizeof(_Tp1)];
	  _Tp1* __tmp = __r.get();
	  _M_refcount = __shared_count<_Lp>(__r);
	  __enable_shared_from_this_helper(_M_refcount, __tmp, __tmp);
	}


      template<typename _Tp1>
        __shared_ptr(const __shared_ptr<_Tp1, _Lp>& __r, __static_cast_tag)
	: _M_ptr(static_cast<element_type*>(__r._M_ptr)),
	  _M_refcount(__r._M_refcount)
        { }

      template<typename _Tp1>
        __shared_ptr(const __shared_ptr<_Tp1, _Lp>& __r, __const_cast_tag)
	: _M_ptr(const_cast<element_type*>(__r._M_ptr)),
	  _M_refcount(__r._M_refcount)
        { }

      template<typename _Tp1>
        __shared_ptr(const __shared_ptr<_Tp1, _Lp>& __r, __dynamic_cast_tag)
	: _M_ptr(dynamic_cast<element_type*>(__r._M_ptr)),
	  _M_refcount(__r._M_refcount)
        {
	  if (_M_ptr == 0) 
	    _M_refcount = __shared_count<_Lp>();
	}

      template<typename _Tp1>
        __shared_ptr&
        operator=(const __shared_ptr<_Tp1, _Lp>& __r) 
        {
	  _M_ptr = __r._M_ptr;
	  _M_refcount = __r._M_refcount; 
	  return *this;
	}

      template<typename _Tp1>
        __shared_ptr&
        operator=(std::auto_ptr<_Tp1>& __r)
        {
	  __shared_ptr(__r).swap(*this);
	  return *this;
	}

      void
      reset() 
      { __shared_ptr().swap(*this); }

      template<typename _Tp1>
        void
        reset(_Tp1* __p) 
        {
	  
	  ; 
	  __shared_ptr(__p).swap(*this);
	}

      template<typename _Tp1, typename _Deleter>
        void
        reset(_Tp1* __p, _Deleter __d)
        { __shared_ptr(__p, __d).swap(*this); }

      
      typename std::tr1::add_reference<_Tp>::type
      operator*() const 
      {
	;
	return *_M_ptr;
      }

      _Tp*
      operator->() const 
      {
	;
	return _M_ptr;
      }
    
      _Tp*
      get() const 
      { return _M_ptr; }

      
    private:
      typedef _Tp* __shared_ptr::*__unspecified_bool_type;

    public:
      operator __unspecified_bool_type() const 
      { return _M_ptr == 0 ? 0 : &__shared_ptr::_M_ptr; }

      bool
      unique() const 
      { return _M_refcount._M_unique(); }

      long
      use_count() const 
      { return _M_refcount._M_get_use_count(); }

      void
      swap(__shared_ptr<_Tp, _Lp>& __other) 
      {
	std::swap(_M_ptr, __other._M_ptr);
	_M_refcount._M_swap(__other._M_refcount);
      }

    private:
      void*
      _M_get_deleter(const std::type_info& __ti) const
      { return _M_refcount._M_get_deleter(__ti); }

      template<typename _Tp1, _Lock_policy _Lp1>
        bool
        _M_less(const __shared_ptr<_Tp1, _Lp1>& __rhs) const
        { return _M_refcount < __rhs._M_refcount; }

      template<typename _Tp1, _Lock_policy _Lp1> friend class __shared_ptr;
      template<typename _Tp1, _Lock_policy _Lp1> friend class __weak_ptr;

      template<typename _Del, typename _Tp1, _Lock_policy _Lp1>
        friend _Del* get_deleter(const __shared_ptr<_Tp1, _Lp1>&);

      
      template<typename _Tp1>
        friend inline bool
        operator==(const __shared_ptr& __a, const __shared_ptr<_Tp1, _Lp>& __b)
        { return __a.get() == __b.get(); }

      template<typename _Tp1>
        friend inline bool
        operator!=(const __shared_ptr& __a, const __shared_ptr<_Tp1, _Lp>& __b)
        { return __a.get() != __b.get(); }

      template<typename _Tp1>
        friend inline bool
        operator<(const __shared_ptr& __a, const __shared_ptr<_Tp1, _Lp>& __b)
        { return __a._M_less(__b); }

      _Tp*         	   _M_ptr;         
      __shared_count<_Lp>  _M_refcount;    
    };

  
  template<typename _Tp, _Lock_policy _Lp>
    inline void
    swap(__shared_ptr<_Tp, _Lp>& __a, __shared_ptr<_Tp, _Lp>& __b)
    { __a.swap(__b); }

  
  



 
  template<typename _Tp, typename _Tp1, _Lock_policy _Lp>
    inline __shared_ptr<_Tp, _Lp>
    static_pointer_cast(const __shared_ptr<_Tp1, _Lp>& __r)
    { return __shared_ptr<_Tp, _Lp>(__r, __static_cast_tag()); }

  



 
  template<typename _Tp, typename _Tp1, _Lock_policy _Lp>
    inline __shared_ptr<_Tp, _Lp>
    const_pointer_cast(const __shared_ptr<_Tp1, _Lp>& __r)
    { return __shared_ptr<_Tp, _Lp>(__r, __const_cast_tag()); }

  



 
  template<typename _Tp, typename _Tp1, _Lock_policy _Lp>
    inline __shared_ptr<_Tp, _Lp>
    dynamic_pointer_cast(const __shared_ptr<_Tp1, _Lp>& __r)
    { return __shared_ptr<_Tp, _Lp>(__r, __dynamic_cast_tag()); }

  
  template<typename _Ch, typename _Tr, typename _Tp, _Lock_policy _Lp>
    std::basic_ostream<_Ch, _Tr>&
    operator<<(std::basic_ostream<_Ch, _Tr>& __os, 
	       const __shared_ptr<_Tp, _Lp>& __p)
    {
      __os << __p.get();
      return __os;
    }

  
  template<typename _Del, typename _Tp, _Lock_policy _Lp>
    inline _Del*
    get_deleter(const __shared_ptr<_Tp, _Lp>& __p)
    {
      return static_cast<_Del*>(__p._M_get_deleter(typeid(_Del)));
    }


  template<typename _Tp, _Lock_policy _Lp>
    class __weak_ptr
    {
    public:
      typedef _Tp element_type;
      
      __weak_ptr()
      : _M_ptr(0), _M_refcount() 
      { }

      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      template<typename _Tp1>
        __weak_ptr(const __weak_ptr<_Tp1, _Lp>& __r)
	: _M_refcount(__r._M_refcount) 
        {
	  
	  _M_ptr = __r.lock().get();
	}

      template<typename _Tp1>
        __weak_ptr(const __shared_ptr<_Tp1, _Lp>& __r)
	: _M_ptr(__r._M_ptr), _M_refcount(__r._M_refcount) 
        {  }

      template<typename _Tp1>
        __weak_ptr&
        operator=(const __weak_ptr<_Tp1, _Lp>& __r) 
        {
	  _M_ptr = __r.lock().get();
	  _M_refcount = __r._M_refcount;
	  return *this;
	}
      
      template<typename _Tp1>
        __weak_ptr&
        operator=(const __shared_ptr<_Tp1, _Lp>& __r) 
        {
	  _M_ptr = __r._M_ptr;
	  _M_refcount = __r._M_refcount;
	  return *this;
	}

      __shared_ptr<_Tp, _Lp>
      lock() const 
      {
	
	if (expired())
	  return __shared_ptr<element_type, _Lp>();

	try
	  {
	    return __shared_ptr<element_type, _Lp>(*this);
	  }
	catch(const bad_weak_ptr&)
	  {
	    
	    
	    
	    return __shared_ptr<element_type, _Lp>();
	  }
	
      } 

      long
      use_count() const 
      { return _M_refcount._M_get_use_count(); }

      bool
      expired() const 
      { return _M_refcount._M_get_use_count() == 0; }
      
      void
      reset() 
      { __weak_ptr().swap(*this); }

      void
      swap(__weak_ptr& __s) 
      {
	std::swap(_M_ptr, __s._M_ptr);
	_M_refcount._M_swap(__s._M_refcount);
      }

    private:
      
      void
      _M_assign(_Tp* __ptr, const __shared_count<_Lp>& __refcount)
      {
	_M_ptr = __ptr;
	_M_refcount = __refcount;
      }

      template<typename _Tp1>
        bool
        _M_less(const __weak_ptr<_Tp1, _Lp>& __rhs) const
        { return _M_refcount < __rhs._M_refcount; }

      template<typename _Tp1, _Lock_policy _Lp1> friend class __shared_ptr;
      template<typename _Tp1, _Lock_policy _Lp1> friend class __weak_ptr;
      friend class __enable_shared_from_this<_Tp, _Lp>;
      friend class enable_shared_from_this<_Tp>;

      
      template<typename _Tp1>
        friend inline bool
        operator<(const __weak_ptr& __lhs, const __weak_ptr<_Tp1, _Lp>& __rhs)
        { return __lhs._M_less(__rhs); }

      _Tp*       	 _M_ptr;         
      __weak_count<_Lp>  _M_refcount;    
    };

  
  template<typename _Tp, _Lock_policy _Lp>
    inline void
    swap(__weak_ptr<_Tp, _Lp>& __a, __weak_ptr<_Tp, _Lp>& __b)
    { __a.swap(__b); }


  template<typename _Tp, _Lock_policy _Lp>
    class __enable_shared_from_this
    {
    protected:
      __enable_shared_from_this() { }
      
      __enable_shared_from_this(const __enable_shared_from_this&) { }
      
      __enable_shared_from_this&
      operator=(const __enable_shared_from_this&)
      { return *this; }

      ~__enable_shared_from_this() { }
      
    public:
      __shared_ptr<_Tp, _Lp>
      shared_from_this()
      { return __shared_ptr<_Tp, _Lp>(this->_M_weak_this); }

      __shared_ptr<const _Tp, _Lp>
      shared_from_this() const
      { return __shared_ptr<const _Tp, _Lp>(this->_M_weak_this); }

    private:
      template<typename _Tp1>
        void
        _M_weak_assign(_Tp1* __p, const __shared_count<_Lp>& __n) const
        { _M_weak_this._M_assign(__p, __n); }

      template<typename _Tp1>
        friend void
        __enable_shared_from_this_helper(const __shared_count<_Lp>& __pn,
					 const __enable_shared_from_this* __pe,
					 const _Tp1* __px)
        {
	  if (__pe != 0)
	    __pe->_M_weak_assign(const_cast<_Tp1*>(__px), __pn);
	}

      mutable __weak_ptr<_Tp, _Lp>  _M_weak_this;
    };


  
  
  template<typename _Tp>
    class shared_ptr
    : public __shared_ptr<_Tp>
    {
    public:
      shared_ptr()
      : __shared_ptr<_Tp>() { }

      template<typename _Tp1>
        explicit
        shared_ptr(_Tp1* __p)
	: __shared_ptr<_Tp>(__p) { }

      template<typename _Tp1, typename _Deleter>
        shared_ptr(_Tp1* __p, _Deleter __d)
	: __shared_ptr<_Tp>(__p, __d) { }

      template<typename _Tp1>
        shared_ptr(const shared_ptr<_Tp1>& __r)
	: __shared_ptr<_Tp>(__r) { }

      template<typename _Tp1>
        explicit
        shared_ptr(const weak_ptr<_Tp1>& __r)
	: __shared_ptr<_Tp>(__r) { }

      template<typename _Tp1>
        explicit
        shared_ptr(std::auto_ptr<_Tp1>& __r)
	: __shared_ptr<_Tp>(__r) { }

      template<typename _Tp1>
        shared_ptr(const shared_ptr<_Tp1>& __r, __static_cast_tag)
	: __shared_ptr<_Tp>(__r, __static_cast_tag()) { }

      template<typename _Tp1>
        shared_ptr(const shared_ptr<_Tp1>& __r, __const_cast_tag)
	: __shared_ptr<_Tp>(__r, __const_cast_tag()) { }

      template<typename _Tp1>
        shared_ptr(const shared_ptr<_Tp1>& __r, __dynamic_cast_tag)
	: __shared_ptr<_Tp>(__r, __dynamic_cast_tag()) { }

      template<typename _Tp1>
        shared_ptr&
        operator=(const shared_ptr<_Tp1>& __r) 
        {
	  this->__shared_ptr<_Tp>::operator=(__r);
	  return *this;
	}

      template<typename _Tp1>
        shared_ptr&
        operator=(std::auto_ptr<_Tp1>& __r)
        {
	  this->__shared_ptr<_Tp>::operator=(__r);
	  return *this;
	}
    };

  
  template<typename _Tp>
    inline void
    swap(__shared_ptr<_Tp>& __a, __shared_ptr<_Tp>& __b)
    { __a.swap(__b); }

  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    static_pointer_cast(const shared_ptr<_Tp1>& __r)
    { return shared_ptr<_Tp>(__r, __static_cast_tag()); }

  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    const_pointer_cast(const shared_ptr<_Tp1>& __r)
    { return shared_ptr<_Tp>(__r, __const_cast_tag()); }

  template<typename _Tp, typename _Tp1>
    inline shared_ptr<_Tp>
    dynamic_pointer_cast(const shared_ptr<_Tp1>& __r)
    { return shared_ptr<_Tp>(__r, __dynamic_cast_tag()); }


  
  
  template<typename _Tp>
    class weak_ptr
    : public __weak_ptr<_Tp>
    {
    public:
      weak_ptr()
      : __weak_ptr<_Tp>() { }
      
      template<typename _Tp1>
        weak_ptr(const weak_ptr<_Tp1>& __r)
	: __weak_ptr<_Tp>(__r) { }

      template<typename _Tp1>
        weak_ptr(const shared_ptr<_Tp1>& __r)
	: __weak_ptr<_Tp>(__r) { }

      template<typename _Tp1>
        weak_ptr&
        operator=(const weak_ptr<_Tp1>& __r) 
        {
	  this->__weak_ptr<_Tp>::operator=(__r);
	  return *this;
	}

      template<typename _Tp1>
        weak_ptr&
        operator=(const shared_ptr<_Tp1>& __r) 
        {
	  this->__weak_ptr<_Tp>::operator=(__r);
	  return *this;
	}

      shared_ptr<_Tp>
      lock() const 
      {
	if (this->expired())
	  return shared_ptr<_Tp>();

	try
	  {
	    return shared_ptr<_Tp>(*this);
	  }
	catch(const bad_weak_ptr&)
	  {
	    return shared_ptr<_Tp>();
	  }
      }
    };

  template<typename _Tp>
    class enable_shared_from_this
    {
    protected:
      enable_shared_from_this() { }
      
      enable_shared_from_this(const enable_shared_from_this&) { }

      enable_shared_from_this&
      operator=(const enable_shared_from_this&)
      { return *this; }

      ~enable_shared_from_this() { }

    public:
      shared_ptr<_Tp>
      shared_from_this()
      { return shared_ptr<_Tp>(this->_M_weak_this); }

      shared_ptr<const _Tp>
      shared_from_this() const
      { return shared_ptr<const _Tp>(this->_M_weak_this); }

    private:
      template<typename _Tp1>
        void
        _M_weak_assign(_Tp1* __p, const __shared_count<>& __n) const
        { _M_weak_this._M_assign(__p, __n); }

      template<typename _Tp1>
        friend void
        __enable_shared_from_this_helper(const __shared_count<>& __pn,
					 const enable_shared_from_this* __pe,
					 const _Tp1* __px)
        {
	  if (__pe != 0)
	    __pe->_M_weak_assign(const_cast<_Tp1*>(__px), __pn);
	}

      mutable weak_ptr<_Tp>  _M_weak_this;
    };


}
}

#else
}
#endif


