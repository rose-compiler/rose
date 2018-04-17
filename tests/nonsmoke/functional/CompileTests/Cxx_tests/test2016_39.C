
 
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
 

 


































































































































 











 







 









 



































































































































































































 

































 
























































































 
namespace std
{
  typedef long unsigned int 	size_t;
  typedef long int	ptrdiff_t;

}


























































































 






















 











































































 


 


 

 







 


 

 


 

 

 

 




 


 



















 















 







 


 


 

 











 


















 


 



 

 



 





 



 


 


 


 






 


 


 


 


 
 










 







 



 



 



 

 



 

 






 




 


 


 

 


 









 



 




 

 



 


 


 



 

 



 


 





 


 





 






































 
























 
 

 

 

 

 

 

 

 

 

 

 

 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 

 
 

 

 

 

 

 

 

 

 

 
 

 

 

 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 

 
 

 

 

 

 

 

 
 

 

 

 

 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 

 
 

 

 

 
 

 
 

 

 

 

 

 

 

 

 

 

 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 

 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 
 

 


 

 
 

 

 

 

 

 

 

 
 

 
 

 
 

 
 

 
 

 

 
 

 

 
 


 

 

 

 

 
 

 

 
 

 
 

 

 

 

 

 
 

 

 
 

 
 


 



 



 


 


 


 


 


 


 


 

 

 

 


 

 

 

 

 

 

 

 
 

 

 
 


 

 
 

 

 
 

 
 

 

 

 

 






















































































 



 



 
 


 


 
 




 


 






 





 

 


 
typedef long int ptrdiff_t;

 


 


 
typedef long unsigned int size_t;





 


 



 
 
 


 



 



































      
      
      
      
      



















 



 



extern "C" {


 

 


 

 

 


 


 


 


 

 

 

 

 

 


 

 




















































































 

















 


 

 

 

 

 

 

 

 

 

 


 

 

 

 

 

 

 

 

 

 

 


 

 

 

 

 

 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 















 


 


 

















 










 


 

 



 


 

















 



 


 


 

 
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




typedef __ssize_t ssize_t;






















 



 



 


 


 
 




 


 






 





 

 


 

 


 


 





 


 



 
 
 


 



 






 
typedef __gid_t gid_t;

typedef __uid_t uid_t;

typedef __off_t off_t;
typedef __off64_t off64_t;

typedef __useconds_t useconds_t;

typedef __pid_t pid_t;

typedef __intptr_t intptr_t;

typedef __socklen_t socklen_t;


 

 
extern int access (const char *__name, int __type) throw () __attribute__ ((__nonnull__ (1)));


 
extern int euidaccess (const char *__name, int __type)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern int eaccess (const char *__name, int __type)
     throw () __attribute__ ((__nonnull__ (1)));



 
extern int faccessat (int __fd, const char *__file, int __type, int __flag)
     throw () __attribute__ ((__nonnull__ (2))) ;


 

 






 
extern __off_t lseek (int __fd, __off_t __offset, int __whence) throw ();
extern __off64_t lseek64 (int __fd, __off64_t __offset, int __whence)
     throw ();




 
extern int close (int __fd);





 
extern ssize_t read (int __fd, void *__buf, size_t __nbytes) ;




 
extern ssize_t write (int __fd, const void *__buf, size_t __n) ;






 
extern ssize_t pread (int __fd, void *__buf, size_t __nbytes,
		      __off_t __offset) ;





 
extern ssize_t pwrite (int __fd, const void *__buf, size_t __n,
		       __off_t __offset) ;



 
extern ssize_t pread64 (int __fd, void *__buf, size_t __nbytes,
			__off64_t __offset) ;

 
extern ssize_t pwrite64 (int __fd, const void *__buf, size_t __n,
			 __off64_t __offset) ;




 
extern int pipe (int __pipedes[2]) throw () ;


 
extern int pipe2 (int __pipedes[2], int __flags) throw () ;







 
extern unsigned int alarm (unsigned int __seconds) throw ();










 
extern unsigned int sleep (unsigned int __seconds);




 
extern __useconds_t ualarm (__useconds_t __value, __useconds_t __interval)
     throw ();





 
extern int usleep (__useconds_t __useconds);






 
extern int pause (void);


 
extern int chown (const char *__file, __uid_t __owner, __gid_t __group)
     throw () __attribute__ ((__nonnull__ (1))) ;

 
extern int fchown (int __fd, __uid_t __owner, __gid_t __group) throw () ;



 
extern int lchown (const char *__file, __uid_t __owner, __gid_t __group)
     throw () __attribute__ ((__nonnull__ (1))) ;



 
extern int fchownat (int __fd, const char *__file, __uid_t __owner,
		     __gid_t __group, int __flag)
     throw () __attribute__ ((__nonnull__ (2))) ;

 
extern int chdir (const char *__path) throw () __attribute__ ((__nonnull__ (1))) ;

 
extern int fchdir (int __fd) throw () ;







 
extern char *getcwd (char *__buf, size_t __size) throw () ;



 
extern char *get_current_dir_name (void) throw ();



 
extern char *getwd (char *__buf)
     throw () __attribute__ ((__nonnull__ (1))) __attribute__ ((__deprecated__)) ;


 
extern int dup (int __fd) throw () ;

 
extern int dup2 (int __fd, int __fd2) throw ();


 
extern int dup3 (int __fd, int __fd2, int __flags) throw ();

 
extern char **__environ;
extern char **environ;



 
extern int execve (const char *__path, char *const __argv[],
		   char *const __envp[]) throw () __attribute__ ((__nonnull__ (1, 2)));


 
extern int fexecve (int __fd, char *const __argv[], char *const __envp[])
     throw () __attribute__ ((__nonnull__ (2)));


 
extern int execv (const char *__path, char *const __argv[])
     throw () __attribute__ ((__nonnull__ (1, 2)));


 
extern int execle (const char *__path, const char *__arg, ...)
     throw () __attribute__ ((__nonnull__ (1, 2)));


 
extern int execl (const char *__path, const char *__arg, ...)
     throw () __attribute__ ((__nonnull__ (1, 2)));


 
extern int execvp (const char *__file, char *const __argv[])
     throw () __attribute__ ((__nonnull__ (1, 2)));



 
extern int execlp (const char *__file, const char *__arg, ...)
     throw () __attribute__ ((__nonnull__ (1, 2)));


 
extern int execvpe (const char *__file, char *const __argv[],
		    char *const __envp[])
     throw () __attribute__ ((__nonnull__ (1, 2)));


 
extern int nice (int __inc) throw () ;


 
extern void _exit (int __status) __attribute__ ((__noreturn__));




 

















 


 
enum
  {
    _PC_LINK_MAX,
    _PC_MAX_CANON,
    _PC_MAX_INPUT,
    _PC_NAME_MAX,
    _PC_PATH_MAX,
    _PC_PIPE_BUF,
    _PC_CHOWN_RESTRICTED,
    _PC_NO_TRUNC,
    _PC_VDISABLE,
    _PC_SYNC_IO,
    _PC_ASYNC_IO,
    _PC_PRIO_IO,
    _PC_SOCK_MAXBUF,
    _PC_FILESIZEBITS,
    _PC_REC_INCR_XFER_SIZE,
    _PC_REC_MAX_XFER_SIZE,
    _PC_REC_MIN_XFER_SIZE,
    _PC_REC_XFER_ALIGN,
    _PC_ALLOC_SIZE_MIN,
    _PC_SYMLINK_MAX,
    _PC_2_SYMLINKS
  };

 
enum
  {
    _SC_ARG_MAX,
    _SC_CHILD_MAX,
    _SC_CLK_TCK,
    _SC_NGROUPS_MAX,
    _SC_OPEN_MAX,
    _SC_STREAM_MAX,
    _SC_TZNAME_MAX,
    _SC_JOB_CONTROL,
    _SC_SAVED_IDS,
    _SC_REALTIME_SIGNALS,
    _SC_PRIORITY_SCHEDULING,
    _SC_TIMERS,
    _SC_ASYNCHRONOUS_IO,
    _SC_PRIORITIZED_IO,
    _SC_SYNCHRONIZED_IO,
    _SC_FSYNC,
    _SC_MAPPED_FILES,
    _SC_MEMLOCK,
    _SC_MEMLOCK_RANGE,
    _SC_MEMORY_PROTECTION,
    _SC_MESSAGE_PASSING,
    _SC_SEMAPHORES,
    _SC_SHARED_MEMORY_OBJECTS,
    _SC_AIO_LISTIO_MAX,
    _SC_AIO_MAX,
    _SC_AIO_PRIO_DELTA_MAX,
    _SC_DELAYTIMER_MAX,
    _SC_MQ_OPEN_MAX,
    _SC_MQ_PRIO_MAX,
    _SC_VERSION,
    _SC_PAGESIZE,
    _SC_RTSIG_MAX,
    _SC_SEM_NSEMS_MAX,
    _SC_SEM_VALUE_MAX,
    _SC_SIGQUEUE_MAX,
    _SC_TIMER_MAX,

    
 
    _SC_BC_BASE_MAX,
    _SC_BC_DIM_MAX,
    _SC_BC_SCALE_MAX,
    _SC_BC_STRING_MAX,
    _SC_COLL_WEIGHTS_MAX,
    _SC_EQUIV_CLASS_MAX,
    _SC_EXPR_NEST_MAX,
    _SC_LINE_MAX,
    _SC_RE_DUP_MAX,
    _SC_CHARCLASS_NAME_MAX,

    _SC_2_VERSION,
    _SC_2_C_BIND,
    _SC_2_C_DEV,
    _SC_2_FORT_DEV,
    _SC_2_FORT_RUN,
    _SC_2_SW_DEV,
    _SC_2_LOCALEDEF,

    _SC_PII,
    _SC_PII_XTI,
    _SC_PII_SOCKET,
    _SC_PII_INTERNET,
    _SC_PII_OSI,
    _SC_POLL,
    _SC_SELECT,
    _SC_UIO_MAXIOV,
    _SC_IOV_MAX = _SC_UIO_MAXIOV,
    _SC_PII_INTERNET_STREAM,
    _SC_PII_INTERNET_DGRAM,
    _SC_PII_OSI_COTS,
    _SC_PII_OSI_CLTS,
    _SC_PII_OSI_M,
    _SC_T_IOV_MAX,

     
    _SC_THREADS,
    _SC_THREAD_SAFE_FUNCTIONS,
    _SC_GETGR_R_SIZE_MAX,
    _SC_GETPW_R_SIZE_MAX,
    _SC_LOGIN_NAME_MAX,
    _SC_TTY_NAME_MAX,
    _SC_THREAD_DESTRUCTOR_ITERATIONS,
    _SC_THREAD_KEYS_MAX,
    _SC_THREAD_STACK_MIN,
    _SC_THREAD_THREADS_MAX,
    _SC_THREAD_ATTR_STACKADDR,
    _SC_THREAD_ATTR_STACKSIZE,
    _SC_THREAD_PRIORITY_SCHEDULING,
    _SC_THREAD_PRIO_INHERIT,
    _SC_THREAD_PRIO_PROTECT,
    _SC_THREAD_PROCESS_SHARED,

    _SC_NPROCESSORS_CONF,
    _SC_NPROCESSORS_ONLN,
    _SC_PHYS_PAGES,
    _SC_AVPHYS_PAGES,
    _SC_ATEXIT_MAX,
    _SC_PASS_MAX,

    _SC_XOPEN_VERSION,
    _SC_XOPEN_XCU_VERSION,
    _SC_XOPEN_UNIX,
    _SC_XOPEN_CRYPT,
    _SC_XOPEN_ENH_I18N,
    _SC_XOPEN_SHM,

    _SC_2_CHAR_TERM,
    _SC_2_C_VERSION,
    _SC_2_UPE,

    _SC_XOPEN_XPG2,
    _SC_XOPEN_XPG3,
    _SC_XOPEN_XPG4,

    _SC_CHAR_BIT,
    _SC_CHAR_MAX,
    _SC_CHAR_MIN,
    _SC_INT_MAX,
    _SC_INT_MIN,
    _SC_LONG_BIT,
    _SC_WORD_BIT,
    _SC_MB_LEN_MAX,
    _SC_NZERO,
    _SC_SSIZE_MAX,
    _SC_SCHAR_MAX,
    _SC_SCHAR_MIN,
    _SC_SHRT_MAX,
    _SC_SHRT_MIN,
    _SC_UCHAR_MAX,
    _SC_UINT_MAX,
    _SC_ULONG_MAX,
    _SC_USHRT_MAX,

    _SC_NL_ARGMAX,
    _SC_NL_LANGMAX,
    _SC_NL_MSGMAX,
    _SC_NL_NMAX,
    _SC_NL_SETMAX,
    _SC_NL_TEXTMAX,

    _SC_XBS5_ILP32_OFF32,
    _SC_XBS5_ILP32_OFFBIG,
    _SC_XBS5_LP64_OFF64,
    _SC_XBS5_LPBIG_OFFBIG,

    _SC_XOPEN_LEGACY,
    _SC_XOPEN_REALTIME,
    _SC_XOPEN_REALTIME_THREADS,

    _SC_ADVISORY_INFO,
    _SC_BARRIERS,
    _SC_BASE,
    _SC_C_LANG_SUPPORT,
    _SC_C_LANG_SUPPORT_R,
    _SC_CLOCK_SELECTION,
    _SC_CPUTIME,
    _SC_THREAD_CPUTIME,
    _SC_DEVICE_IO,
    _SC_DEVICE_SPECIFIC,
    _SC_DEVICE_SPECIFIC_R,
    _SC_FD_MGMT,
    _SC_FIFO,
    _SC_PIPE,
    _SC_FILE_ATTRIBUTES,
    _SC_FILE_LOCKING,
    _SC_FILE_SYSTEM,
    _SC_MONOTONIC_CLOCK,
    _SC_MULTI_PROCESS,
    _SC_SINGLE_PROCESS,
    _SC_NETWORKING,
    _SC_READER_WRITER_LOCKS,
    _SC_SPIN_LOCKS,
    _SC_REGEXP,
    _SC_REGEX_VERSION,
    _SC_SHELL,
    _SC_SIGNALS,
    _SC_SPAWN,
    _SC_SPORADIC_SERVER,
    _SC_THREAD_SPORADIC_SERVER,
    _SC_SYSTEM_DATABASE,
    _SC_SYSTEM_DATABASE_R,
    _SC_TIMEOUTS,
    _SC_TYPED_MEMORY_OBJECTS,
    _SC_USER_GROUPS,
    _SC_USER_GROUPS_R,
    _SC_2_PBS,
    _SC_2_PBS_ACCOUNTING,
    _SC_2_PBS_LOCATE,
    _SC_2_PBS_MESSAGE,
    _SC_2_PBS_TRACK,
    _SC_SYMLOOP_MAX,
    _SC_STREAMS,
    _SC_2_PBS_CHECKPOINT,

    _SC_V6_ILP32_OFF32,
    _SC_V6_ILP32_OFFBIG,
    _SC_V6_LP64_OFF64,
    _SC_V6_LPBIG_OFFBIG,

    _SC_HOST_NAME_MAX,
    _SC_TRACE,
    _SC_TRACE_EVENT_FILTER,
    _SC_TRACE_INHERIT,
    _SC_TRACE_LOG,

    _SC_LEVEL1_ICACHE_SIZE,
    _SC_LEVEL1_ICACHE_ASSOC,
    _SC_LEVEL1_ICACHE_LINESIZE,
    _SC_LEVEL1_DCACHE_SIZE,
    _SC_LEVEL1_DCACHE_ASSOC,
    _SC_LEVEL1_DCACHE_LINESIZE,
    _SC_LEVEL2_CACHE_SIZE,
    _SC_LEVEL2_CACHE_ASSOC,
    _SC_LEVEL2_CACHE_LINESIZE,
    _SC_LEVEL3_CACHE_SIZE,
    _SC_LEVEL3_CACHE_ASSOC,
    _SC_LEVEL3_CACHE_LINESIZE,
    _SC_LEVEL4_CACHE_SIZE,
    _SC_LEVEL4_CACHE_ASSOC,
    _SC_LEVEL4_CACHE_LINESIZE,
     

    _SC_IPV6 = _SC_LEVEL1_ICACHE_SIZE + 50,
    _SC_RAW_SOCKETS,

    _SC_V7_ILP32_OFF32,
    _SC_V7_ILP32_OFFBIG,
    _SC_V7_LP64_OFF64,
    _SC_V7_LPBIG_OFFBIG,

    _SC_SS_REPL_MAX,

    _SC_TRACE_EVENT_NAME_MAX,
    _SC_TRACE_NAME_MAX,
    _SC_TRACE_SYS_MAX,
    _SC_TRACE_USER_EVENT_MAX,

    _SC_XOPEN_STREAMS,

    _SC_THREAD_ROBUST_PRIO_INHERIT,
    _SC_THREAD_ROBUST_PRIO_PROTECT
  };

 
enum
  {
    _CS_PATH,			 

    _CS_V6_WIDTH_RESTRICTED_ENVS,

    _CS_GNU_LIBC_VERSION,
    _CS_GNU_LIBPTHREAD_VERSION,

    _CS_V5_WIDTH_RESTRICTED_ENVS,

    _CS_V7_WIDTH_RESTRICTED_ENVS,

    _CS_LFS_CFLAGS = 1000,
    _CS_LFS_LDFLAGS,
    _CS_LFS_LIBS,
    _CS_LFS_LINTFLAGS,
    _CS_LFS64_CFLAGS,
    _CS_LFS64_LDFLAGS,
    _CS_LFS64_LIBS,
    _CS_LFS64_LINTFLAGS,

    _CS_XBS5_ILP32_OFF32_CFLAGS = 1100,
    _CS_XBS5_ILP32_OFF32_LDFLAGS,
    _CS_XBS5_ILP32_OFF32_LIBS,
    _CS_XBS5_ILP32_OFF32_LINTFLAGS,
    _CS_XBS5_ILP32_OFFBIG_CFLAGS,
    _CS_XBS5_ILP32_OFFBIG_LDFLAGS,
    _CS_XBS5_ILP32_OFFBIG_LIBS,
    _CS_XBS5_ILP32_OFFBIG_LINTFLAGS,
    _CS_XBS5_LP64_OFF64_CFLAGS,
    _CS_XBS5_LP64_OFF64_LDFLAGS,
    _CS_XBS5_LP64_OFF64_LIBS,
    _CS_XBS5_LP64_OFF64_LINTFLAGS,
    _CS_XBS5_LPBIG_OFFBIG_CFLAGS,
    _CS_XBS5_LPBIG_OFFBIG_LDFLAGS,
    _CS_XBS5_LPBIG_OFFBIG_LIBS,
    _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS,

    _CS_POSIX_V6_ILP32_OFF32_CFLAGS,
    _CS_POSIX_V6_ILP32_OFF32_LDFLAGS,
    _CS_POSIX_V6_ILP32_OFF32_LIBS,
    _CS_POSIX_V6_ILP32_OFF32_LINTFLAGS,
    _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS,
    _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS,
    _CS_POSIX_V6_ILP32_OFFBIG_LIBS,
    _CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS,
    _CS_POSIX_V6_LP64_OFF64_CFLAGS,
    _CS_POSIX_V6_LP64_OFF64_LDFLAGS,
    _CS_POSIX_V6_LP64_OFF64_LIBS,
    _CS_POSIX_V6_LP64_OFF64_LINTFLAGS,
    _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS,
    _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS,
    _CS_POSIX_V6_LPBIG_OFFBIG_LIBS,
    _CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS,

    _CS_POSIX_V7_ILP32_OFF32_CFLAGS,
    _CS_POSIX_V7_ILP32_OFF32_LDFLAGS,
    _CS_POSIX_V7_ILP32_OFF32_LIBS,
    _CS_POSIX_V7_ILP32_OFF32_LINTFLAGS,
    _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS,
    _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS,
    _CS_POSIX_V7_ILP32_OFFBIG_LIBS,
    _CS_POSIX_V7_ILP32_OFFBIG_LINTFLAGS,
    _CS_POSIX_V7_LP64_OFF64_CFLAGS,
    _CS_POSIX_V7_LP64_OFF64_LDFLAGS,
    _CS_POSIX_V7_LP64_OFF64_LIBS,
    _CS_POSIX_V7_LP64_OFF64_LINTFLAGS,
    _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS,
    _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS,
    _CS_POSIX_V7_LPBIG_OFFBIG_LIBS,
    _CS_POSIX_V7_LPBIG_OFFBIG_LINTFLAGS,

    _CS_V6_ENV,
    _CS_V7_ENV
  };

 
extern long int pathconf (const char *__path, int __name)
     throw () __attribute__ ((__nonnull__ (1)));

 
extern long int fpathconf (int __fd, int __name) throw ();

 
extern long int sysconf (int __name) throw ();

 
extern size_t confstr (int __name, char *__buf, size_t __len) throw ();


 
extern __pid_t getpid (void) throw ();

 
extern __pid_t getppid (void) throw ();


 
extern __pid_t getpgrp (void) throw ();

 
extern __pid_t __getpgid (__pid_t __pid) throw ();
extern __pid_t getpgid (__pid_t __pid) throw ();




 
extern int setpgid (__pid_t __pid, __pid_t __pgid) throw ();









 



 
extern int setpgrp (void) throw ();




 
extern __pid_t setsid (void) throw ();

 
extern __pid_t getsid (__pid_t __pid) throw ();

 
extern __uid_t getuid (void) throw ();

 
extern __uid_t geteuid (void) throw ();

 
extern __gid_t getgid (void) throw ();

 
extern __gid_t getegid (void) throw ();



 
extern int getgroups (int __size, __gid_t __list[]) throw () ;

 
extern int group_member (__gid_t __gid) throw ();




 
extern int setuid (__uid_t __uid) throw () ;


 
extern int setreuid (__uid_t __ruid, __uid_t __euid) throw () ;

 
extern int seteuid (__uid_t __uid) throw () ;




 
extern int setgid (__gid_t __gid) throw () ;


 
extern int setregid (__gid_t __rgid, __gid_t __egid) throw () ;

 
extern int setegid (__gid_t __gid) throw () ;


 
extern int getresuid (__uid_t *__ruid, __uid_t *__euid, __uid_t *__suid)
     throw ();


 
extern int getresgid (__gid_t *__rgid, __gid_t *__egid, __gid_t *__sgid)
     throw ();


 
extern int setresuid (__uid_t __ruid, __uid_t __euid, __uid_t __suid)
     throw () ;


 
extern int setresgid (__gid_t __rgid, __gid_t __egid, __gid_t __sgid)
     throw () ;




 
extern __pid_t fork (void) throw ();




 
extern __pid_t vfork (void) throw ();



 
extern char *ttyname (int __fd) throw ();


 
extern int ttyname_r (int __fd, char *__buf, size_t __buflen)
     throw () __attribute__ ((__nonnull__ (2))) ;


 
extern int isatty (int __fd) throw ();


 
extern int ttyslot (void) throw ();


 
extern int link (const char *__from, const char *__to)
     throw () __attribute__ ((__nonnull__ (1, 2))) ;


 
extern int linkat (int __fromfd, const char *__from, int __tofd,
		   const char *__to, int __flags)
     throw () __attribute__ ((__nonnull__ (2, 4))) ;

 
extern int symlink (const char *__from, const char *__to)
     throw () __attribute__ ((__nonnull__ (1, 2))) ;



 
extern ssize_t readlink (const char * __path,
			 char * __buf, size_t __len)
     throw () __attribute__ ((__nonnull__ (1, 2))) ;

 
extern int symlinkat (const char *__from, int __tofd,
		      const char *__to) throw () __attribute__ ((__nonnull__ (1, 3))) ;

 
extern ssize_t readlinkat (int __fd, const char * __path,
			   char * __buf, size_t __len)
     throw () __attribute__ ((__nonnull__ (2, 3))) ;

 
extern int unlink (const char *__name) throw () __attribute__ ((__nonnull__ (1)));

 
extern int unlinkat (int __fd, const char *__name, int __flag)
     throw () __attribute__ ((__nonnull__ (2)));

 
extern int rmdir (const char *__path) throw () __attribute__ ((__nonnull__ (1)));


 
extern __pid_t tcgetpgrp (int __fd) throw ();

 
extern int tcsetpgrp (int __fd, __pid_t __pgrp_id) throw ();





 
extern char *getlogin (void);





 
extern int getlogin_r (char *__name, size_t __name_len) __attribute__ ((__nonnull__ (1)));

 
extern int setlogin (const char *__name) throw () __attribute__ ((__nonnull__ (1)));




 

















 









 


extern "C" {





 

extern char *optarg;











 

extern int optind;


 

extern int opterr;

 

extern int optopt;

























 



 
extern int getopt (int ___argc, char *const *___argv, const char *__shortopts)
       throw ();



}

 





 
extern int gethostname (char *__name, size_t __len) throw () __attribute__ ((__nonnull__ (1)));



 
extern int sethostname (const char *__name, size_t __len)
     throw () __attribute__ ((__nonnull__ (1))) ;


 
extern int sethostid (long int __id) throw () ;




 
extern int getdomainname (char *__name, size_t __len)
     throw () __attribute__ ((__nonnull__ (1))) ;
extern int setdomainname (const char *__name, size_t __len)
     throw () __attribute__ ((__nonnull__ (1))) ;




 
extern int vhangup (void) throw ();

 
extern int revoke (const char *__file) throw () __attribute__ ((__nonnull__ (1))) ;






 
extern int profil (unsigned short int *__sample_buffer, size_t __size,
		   size_t __offset, unsigned int __scale)
     throw () __attribute__ ((__nonnull__ (1)));




 
extern int acct (const char *__name) throw ();


 
extern char *getusershell (void) throw ();
extern void endusershell (void) throw ();  
extern void setusershell (void) throw ();  




 
extern int daemon (int __nochdir, int __noclose) throw () ;



 
extern int chroot (const char *__path) throw () __attribute__ ((__nonnull__ (1))) ;


 
extern char *getpass (const char *__prompt) __attribute__ ((__nonnull__ (1)));





 
extern int fsync (int __fd);



 
extern int syncfs (int __fd) throw ();



 
extern long int gethostid (void);

 
extern void sync (void) throw ();



 
extern int getpagesize (void)  throw () __attribute__ ((__const__));



 
extern int getdtablesize (void) throw ();




 
extern int truncate (const char *__file, __off_t __length)
     throw () __attribute__ ((__nonnull__ (1))) ;
extern int truncate64 (const char *__file, __off64_t __length)
     throw () __attribute__ ((__nonnull__ (1))) ;



 
extern int ftruncate (int __fd, __off_t __length) throw () ;
extern int ftruncate64 (int __fd, __off64_t __length) throw () ;





 
extern int brk (void *__addr) throw () ;




 
extern void *sbrk (intptr_t __delta) throw ();











 
extern long int syscall (long int __sysno, ...) throw ();





 






 


extern int lockf (int __fd, int __cmd, __off_t __len) ;
extern int lockf64 (int __fd, int __cmd, __off64_t __len) ;




 



 
extern int fdatasync (int __fildes);



 
 
extern char *crypt (const char *__key, const char *__salt)
     throw () __attribute__ ((__nonnull__ (1, 2)));


 
extern void encrypt (char *__block, int __edflag) throw () __attribute__ ((__nonnull__ (1)));





 
extern void swab (const void * __from, void * __to,
		  ssize_t __n) throw () __attribute__ ((__nonnull__ (1, 2)));



 


 

}





























































































































 
























 



 



 





















 



 



 


 


 
 




 


 






 





 

 


 

 


 


 





 


 



 
 
 


 



 





extern "C" {


 
















 



 

 

















 



 


 

 

 

 

 

 


 

 

 


















 









 


 
 




 




 
















 



 


 

 

 
















 



 

static __inline unsigned int
__bswap_32 (unsigned int __bsx)
{
  return __builtin_bswap32 (__bsx);
}


 

static __inline __uint64_t
__bswap_64 (__uint64_t __bsx)
{
  return __builtin_bswap64 (__bsx);
}







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












 


 

















 





 
typedef struct __locale_struct
{
   
  struct __locale_data *__locales[13];  

   
  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;

   
  const char *__names[13];
} *__locale_t;

 
typedef __locale_t locale_t;



 
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


typedef __mode_t mode_t;

typedef __nlink_t nlink_t;




typedef __id_t id_t;


typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;

typedef __key_t key_t;
















 



 







 
typedef __clock_t clock_t;







 
typedef __time_t time_t;






 
typedef __clockid_t clockid_t;




 
typedef __timer_t timer_t;







typedef __suseconds_t suseconds_t;






















 



 



 


 


 
 




 


 






 





 

 


 

 


 


 





 


 



 
 
 


 



 





 
typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;

 


 

typedef int int8_t __attribute__ ((__mode__ (__QI__)));
typedef int int16_t __attribute__ ((__mode__ (__HI__)));
typedef int int32_t __attribute__ ((__mode__ (__SI__)));
typedef int int64_t __attribute__ ((__mode__ (__DI__)));

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

 















 



 












 
struct timespec
  {
    __time_t tv_sec;		 
    __syscall_slong_t tv_nsec;	 
  };




















 



 



 
struct timeval
  {
    __time_t tv_sec;		 
    __suseconds_t tv_usec;	 
  };





 
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

}

 



typedef __blksize_t blksize_t;

 
typedef __blkcnt_t blkcnt_t;	  
typedef __fsblkcnt_t fsblkcnt_t;  
typedef __fsfilcnt_t fsfilcnt_t;  

typedef __blkcnt64_t blkcnt64_t;      
typedef __fsblkcnt64_t fsblkcnt64_t;  
typedef __fsfilcnt64_t fsfilcnt64_t;  


 















 


 


 




 
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

















 



 


}




namespace std 
{


  using ::div_t;
  using ::ldiv_t;

  using ::abort;
  using ::abs;
  using ::atexit;
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








   
   
   










   



















      

      

      

      
      
      
      
      
      
      

      
      

      
      
      
      

      
      
      

      
      
      
      
      














































































































































































































































































namespace boost{
   __extension__ typedef long long long_long_type;
   __extension__ typedef unsigned long long ulong_long_type;
}

namespace boost{
   __extension__ typedef __int128 int128_type;
   __extension__ typedef unsigned __int128 uint128_type;
}






































     



     




























































































































































#pragma once


































































































































 



























 



 



namespace boost {


template <class T> class rv;

namespace move_detail {




struct nat{};




template <class T> struct natify{};




template<bool C, typename T1, typename T2>
struct if_c
{
   typedef T1 type;
};

template<typename T1, typename T2>
struct if_c<false,T1,T2>
{
   typedef T2 type;
};




template<typename T1, typename T2, typename T3>
struct if_
{
   typedef typename if_c<0 != T1::value, T2, T3>::type type;
};


template <bool B, class T = nat>
struct enable_if_c
{
   typedef T type;
};




template <class T>
struct enable_if_c<false, T> {};




template <class Cond, class T = nat>
struct enable_if : public enable_if_c<Cond::value, T> {};




template <class Cond, class T = nat>
struct disable_if : public enable_if_c<!Cond::value, T> {};




template<class T, T v>
struct integral_constant
{
   static const T value = v;
   typedef T value_type;
   typedef integral_constant<T, v> type;
};

typedef integral_constant<bool, true >  true_type;
typedef integral_constant<bool, false > false_type;




template <class T>
struct identity
{
   typedef T type;
};




template<class T>
struct remove_reference
{
   typedef T type;
};

template<class T>
struct remove_reference<T&>
{
   typedef T type;
};


template<class T>
struct remove_reference< rv<T> >
{
   typedef T type;
};

template<class T>
struct remove_reference< rv<T> &>
{
   typedef T type;
};

template<class T>
struct remove_reference< const rv<T> &>
{
   typedef T type;
};






template<class T>
struct add_const
{
   typedef const T type;
};

template<class T>
struct add_const<T&>
{
   typedef const T& type;
};





template<class T>
struct add_lvalue_reference
{
   typedef T& type;
};

template<class T>
struct add_lvalue_reference<T&>
{
   typedef T& type;
};

template<>
struct add_lvalue_reference<void>
{
   typedef void type;
};

template<>
struct add_lvalue_reference<const void>
{
   typedef const void type;
};

template<>
struct add_lvalue_reference<volatile void>
{
   typedef volatile void type;
};

template<>
struct add_lvalue_reference<const volatile void>
{
   typedef const volatile void type;
};

template<class T>
struct add_const_lvalue_reference
{
   typedef typename remove_reference<T>::type         t_unreferenced;
   typedef typename add_const<t_unreferenced>::type   t_unreferenced_const;
   typedef typename add_lvalue_reference
      <t_unreferenced_const>::type                    type;
};





template<class T, class U>
struct is_same
{
   static const bool value = false;
};
 
template<class T>
struct is_same<T, T>
{
   static const bool value = true;
};




template<class T>
struct is_lvalue_reference
{
    static const bool value = false;
};

template<class T>
struct is_lvalue_reference<T&>
{
    static const bool value = true;
};




template<class T>
struct is_class_or_union
{
   struct twochar { char _[2]; };
   template <class U>
   static char is_class_or_union_tester(void(U::*)(void));
   template <class U>
   static twochar is_class_or_union_tester(...);
   static const bool value = sizeof(is_class_or_union_tester<T>(0)) == sizeof(char);
};




template<class T>
struct addr_impl_ref
{
   T & v_;
   inline addr_impl_ref( T & v ): v_( v ) {}
   inline operator T& () const { return v_; }

   private:
   addr_impl_ref & operator=(const addr_impl_ref &);
};

template<class T>
struct addressof_impl
{
   static inline T * f( T & v, long )
   {
      return reinterpret_cast<T*>(
         &const_cast<char&>(reinterpret_cast<const volatile char &>(v)));
   }

   static inline T * f( T * v, int )
   {  return v;  }
};

template<class T>
inline T * addressof( T & v )
{
   return ::boost::move_detail::addressof_impl<T>::f
      ( ::boost::move_detail::addr_impl_ref<T>( v ), 0 );
}




template <class T>
struct has_pointer_type
{
   struct two { char c[2]; };
   template <class U> static two test(...);
   template <class U> static char test(typename U::pointer* = 0);
   static const bool value = sizeof(test<T>(0)) == 1;
};





template <class T, class U>
class is_convertible
{
   typedef typename add_lvalue_reference<T>::type t_reference;
   typedef char true_t;
   class false_t { char dummy[2]; };
   static false_t dispatch(...);
   static true_t  dispatch(U);
   static t_reference       trigger();
   public:
   static const bool value = sizeof(dispatch(trigger())) == sizeof(true_t);
};







template<class T>
struct has_move_emulation_enabled_impl
   : is_convertible< T, ::boost::rv<T>& >
{};

template<class T>
struct has_move_emulation_enabled_impl<T&>
{  static const bool value = false;  };

template<class T>
struct has_move_emulation_enabled_impl< ::boost::rv<T> >
{  static const bool value = false;  };







template <class T>
struct is_rv_impl
{  static const bool value = false;  };

template <class T>
struct is_rv_impl< rv<T> >
{  static const bool value = true;  };

template <class T>
struct is_rv_impl< const rv<T> >
{  static const bool value = true;  };



template< class T >
struct is_rvalue_reference
{  static const bool value = false;  };


template< class T >
struct is_rvalue_reference< boost::rv<T>& >
{  static const bool value = true;  };

template< class T >
struct is_rvalue_reference< const boost::rv<T>& >
{  static const bool value = true;  };



namespace detail_add_rvalue_reference
{
   template< class T
            , bool emulation = has_move_emulation_enabled_impl<T>::value
            , bool rv        = is_rv_impl<T>::value  >
   struct add_rvalue_reference_impl { typedef T type; };

   template< class T, bool emulation>
   struct add_rvalue_reference_impl< T, emulation, true > { typedef T & type; };

   template< class T, bool rv >
   struct add_rvalue_reference_impl< T, true, rv > { typedef ::boost::rv<T>& type; };
} 

template< class T >
struct add_rvalue_reference
   : detail_add_rvalue_reference::add_rvalue_reference_impl<T>
{ };

template< class T >
struct add_rvalue_reference<T &>
{  typedef T & type; };


template< class T > struct remove_rvalue_reference { typedef T type; };

   template< class T > struct remove_rvalue_reference< rv<T> >                { typedef T type; };
   template< class T > struct remove_rvalue_reference< const rv<T> >          { typedef T type; };
   template< class T > struct remove_rvalue_reference< volatile rv<T> >       { typedef T type; };
   template< class T > struct remove_rvalue_reference< const volatile rv<T> > { typedef T type; };
   template< class T > struct remove_rvalue_reference< rv<T>& >               { typedef T type; };
   template< class T > struct remove_rvalue_reference< const rv<T>& >         { typedef T type; };
   template< class T > struct remove_rvalue_reference< volatile rv<T>& >      { typedef T type; };
   template< class T > struct remove_rvalue_reference< const volatile rv<T>& >{ typedef T type; };













}  
}  












   

   namespace boost {

   
   
   
   
   
   template <class T>
   class rv
      : public ::boost::move_detail::if_c
         < ::boost::move_detail::is_class_or_union<T>::value
         , T
         , ::boost::move_detail::nat
         >::type
   {
      rv();
      ~rv() throw();
      rv(rv const&);
      void operator=(rv const&);
   } __attribute__((__may_alias__));


   
   
   
   
   

   namespace move_detail {

   template <class T>
   struct is_rv
        
        
      : integral_constant<bool, ::boost::move_detail::is_rv_impl<T>::value >
   {};

   }  

   
   
   
   
   
   template<class T>
   struct has_move_emulation_enabled
      : ::boost::move_detail::has_move_emulation_enabled_impl<T>
   {};

   }  













   namespace boost {
   namespace move_detail {

   template <class Ret, class T>
   inline typename ::boost::move_detail::enable_if_c
      <  ::boost::move_detail::is_lvalue_reference<Ret>::value ||
        !::boost::has_move_emulation_enabled<T>::value
      , T&>::type
         move_return(T& x) 
   {
      return x;
   }

   template <class Ret, class T>
   inline typename ::boost::move_detail::enable_if_c
      < !::boost::move_detail::is_lvalue_reference<Ret>::value &&
         ::boost::has_move_emulation_enabled<T>::value
      , ::boost::rv<T>&>::type
         move_return(T& x) 
   {
      return *static_cast< ::boost::rv<T>* >(::boost::move_detail::addressof(x));
   }

   template <class Ret, class T>
   inline typename ::boost::move_detail::enable_if_c
      < !::boost::move_detail::is_lvalue_reference<Ret>::value &&
         ::boost::has_move_emulation_enabled<T>::value
      , ::boost::rv<T>&>::type
         move_return(::boost::rv<T>& x) 
   {
      return x;
   }

   }  
   }  

   

   
   
   
   
   

   
   
   
   
   



   namespace boost{
   namespace move_detail{

   template< class T>
   struct forward_type
   { typedef const T &type; };

   template< class T>
   struct forward_type< boost::rv<T> >
   { typedef T type; };

   }}























 



















































































namespace boost{


template <bool x> struct STATIC_ASSERTION_FAILURE;

template <> struct STATIC_ASSERTION_FAILURE<true> { enum { value = 1 }; };


template<int x> struct static_assert_test{};

}































   namespace boost {

   template<class T>
   struct enable_move_utility_emulation
   {
      static const bool value = true;
   };
    
   
   
   
   
   

   template <class T>
   inline typename ::boost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && !has_move_emulation_enabled<T>::value, T&>::type
         move(T& x) 
   {
      return x;
   }

   template <class T>
   inline typename ::boost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && has_move_emulation_enabled<T>::value, rv<T>&>::type
         move(T& x) 
   {
      return *static_cast<rv<T>* >(::boost::move_detail::addressof(x));
   }

   template <class T>
   inline typename ::boost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && has_move_emulation_enabled<T>::value, rv<T>&>::type
         move(rv<T>& x) 
   {
      return x;
   }

   
   
   
   
   

   template <class T>
   inline typename ::boost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && ::boost::move_detail::is_rv<T>::value, T &>::type
         forward(const typename ::boost::move_detail::identity<T>::type &x) 
   {
      return const_cast<T&>(x);
   }

   template <class T>
   inline typename ::boost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value && !::boost::move_detail::is_rv<T>::value, const T &>::type
      forward(const typename ::boost::move_detail::identity<T>::type &x) 
   {
      return x;
   }

   
   
   
   
   

   template <class T>
   inline typename ::boost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value &&
        ::boost::move_detail::is_rv<T>::value
      , T &>::type
         move_if_not_lvalue_reference(const typename ::boost::move_detail::identity<T>::type &x) 
   {
      return const_cast<T&>(x);
   }

   template <class T>
   inline typename ::boost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value &&
        !::boost::move_detail::is_rv<T>::value  &&
        (::boost::move_detail::is_lvalue_reference<T>::value ||
         !has_move_emulation_enabled<T>::value)
      , typename ::boost::move_detail::add_lvalue_reference<T>::type
      >::type
         move_if_not_lvalue_reference(typename ::boost::move_detail::remove_reference<T>::type &x) 
   {
      return x;
   }

   template <class T>
   inline typename ::boost::move_detail::enable_if_c
      < enable_move_utility_emulation<T>::value &&
        !::boost::move_detail::is_rv<T>::value  &&
        (!::boost::move_detail::is_lvalue_reference<T>::value &&
         has_move_emulation_enabled<T>::value)
      , rv<T>&
      >::type
         move_if_not_lvalue_reference(typename ::boost::move_detail::remove_reference<T>::type &x) 
   {
      return move(x);
   }

   }  



namespace boost{
namespace move_detail{

template<class T>
void swap(T &a, T &b)
{
   T c((::boost::move(a)));
   a = ::boost::move(b);
   b = ::boost::move(c);
}

template <typename T>
typename boost::move_detail::add_rvalue_reference<T>::type declval();

}  
}  
















































































 



























 



 



namespace boost {

namespace movelib {

template <class T>
struct default_delete;

}  


template <class T> class rv;

namespace move_upmu {




struct nat{};




template <class T> struct natify{};




template<bool C, typename T1, typename T2>
struct if_c
{
   typedef T1 type;
};

template<typename T1, typename T2>
struct if_c<false,T1,T2>
{
   typedef T2 type;
};




template<typename T1, typename T2, typename T3>
struct if_ : if_c<0 != T1::value, T2, T3>
{};


template <bool B, class T = nat>
struct enable_if_c
{
   typedef T type;
};




template <class T>
struct enable_if_c<false, T> {};




template <class Cond, class T = nat>
struct enable_if : public enable_if_c<Cond::value, T> {};




template<class T>
struct remove_reference
{
   typedef T type;
};

template<class T>
struct remove_reference<T&>
{
   typedef T type;
};


template<class T>
struct remove_reference< rv<T> >
{
   typedef T type;
};

template<class T>
struct remove_reference< rv<T> &>
{
   typedef T type;
};

template<class T>
struct remove_reference< const rv<T> &>
{
   typedef T type;
};






template< class T >
struct remove_const
{
   typedef T type;
};

template< class T >
struct remove_const<const T>
{
   typedef T type;
};




template< class T >
struct remove_volatile
{
   typedef T type;
};

template< class T >
struct remove_volatile<volatile T>
{
   typedef T type;
};




template< class T >
struct remove_cv
{
    typedef typename remove_volatile
      <typename remove_const<T>::type>::type type;
};




template<class T>
struct remove_extent
{
   typedef T type;
};
 
template<class T>
struct remove_extent<T[]>
{
   typedef T type;
};
 
template<class T, std::size_t N>
struct remove_extent<T[N]>
{
   typedef T type;
};





template<class T, unsigned N = 0>
struct extent
{
   static const std::size_t value = 0;
};
 
template<class T>
struct extent<T[], 0> 
{
   static const std::size_t value = 0;
};

template<class T, unsigned N>
struct extent<T[], N>
{
   static const std::size_t value = extent<T, N-1>::value;
};

template<class T, std::size_t N>
struct extent<T[N], 0> 
{
   static const std::size_t value = N;
};
 
template<class T, std::size_t I, unsigned N>
struct extent<T[I], N>
{
   static const std::size_t value = extent<T, N-1>::value;
};




template<class T>
struct add_lvalue_reference
{
   typedef T& type;
};

template<class T>
struct add_lvalue_reference<T&>
{
   typedef T& type;
};

template<>
struct add_lvalue_reference<void>
{
   typedef void type;
};

template<>
struct add_lvalue_reference<const void>
{
   typedef const void type;
};

template<>
struct add_lvalue_reference<volatile void>
{
   typedef volatile void type;
};

template<>
struct add_lvalue_reference<const volatile void>
{
   typedef const volatile void type;
};

template<class T>
struct add_const_lvalue_reference
{
   typedef typename remove_reference<T>::type   t_unreferenced;
   typedef const t_unreferenced                 t_unreferenced_const;
   typedef typename add_lvalue_reference
      <t_unreferenced_const>::type              type;
};




template<class T, class U>
struct is_same
{
   static const bool value = false;
};
 
template<class T>
struct is_same<T, T>
{
   static const bool value = true;
};




template< class T >
struct is_pointer
{
    static const bool value = false;
};

template< class T >
struct is_pointer<T*>
{
    static const bool value = true;
};




template< class T >
struct is_reference
{
    static const bool value = false;
};

template< class T >
struct is_reference<T&>
{
    static const bool value = true;
};





template<class T>
struct is_lvalue_reference
{
    static const bool value = false;
};

template<class T>
struct is_lvalue_reference<T&>
{
    static const bool value = true;
};




template<class T>
struct is_array
{
   static const bool value = false;
};
 
template<class T>
struct is_array<T[]>
{
   static const bool value = true;
};
 
template<class T, std::size_t N>
struct is_array<T[N]>
{
   static const bool value = true;
};




template <class T>
struct has_pointer_type
{
   struct two { char c[2]; };
   template <class U> static two test(...);
   template <class U> static char test(typename U::pointer* = 0);
   static const bool value = sizeof(test<T>(0)) == 1;
};




template <class T, class D, bool = has_pointer_type<D>::value>
struct pointer_type_imp
{
    typedef typename D::pointer type;
};

template <class T, class D>
struct pointer_type_imp<T, D, false>
{
    typedef typename remove_extent<T>::type* type;
};

template <class T, class D>
struct pointer_type
{
    typedef typename pointer_type_imp
      <typename remove_extent<T>::type, typename remove_reference<D>::type>::type type;
};





template <class T, class U>
class is_convertible
{
   typedef typename add_lvalue_reference<T>::type t_reference;
   typedef char true_t;
   class false_t { char dummy[2]; };
   static false_t dispatch(...);
   static true_t  dispatch(U);
   static t_reference       trigger();
   public:
   static const bool value = sizeof(dispatch(trigger())) == sizeof(true_t);
};







template <typename T>
struct is_unary_function_impl
{  static const bool value = false; };



template <typename R>
struct is_unary_function_impl<R (*)()>
{  static const bool value = true;  };

template <typename R>
struct is_unary_function_impl<R (*)(...)>
{  static const bool value = true;  };




template <typename R, class T0>
struct is_unary_function_impl<R (*)(T0)>
{  static const bool value = true;  };

template <typename R, class T0>
struct is_unary_function_impl<R (*)(T0...)>
{  static const bool value = true;  };


template <typename T>
struct is_unary_function_impl<T&>
{  static const bool value = false; };

template<typename T>
struct is_unary_function
{  static const bool value = is_unary_function_impl<T>::value;   };





   template<class T>
   struct has_virtual_destructor{   static const bool value = __has_virtual_destructor(T);  };





template< class T, class U
        , bool enable =  is_convertible< U*, T*>::value &&
                        !is_array<T>::value &&
                        !is_same<typename remove_cv<T>::type, void>::value &&
                        !is_same<typename remove_cv<U>::type, typename remove_cv<T>::type>::value
        >
struct missing_virtual_destructor_default_delete
{  static const bool value = !has_virtual_destructor<T>::value;  };

template<class T, class U>
struct missing_virtual_destructor_default_delete<T, U, false>
{  static const bool value = false;  };

template<class Deleter, class U>
struct missing_virtual_destructor
{  static const bool value = false;  };

template<class T, class U>
struct missing_virtual_destructor< ::boost::movelib::default_delete<T>, U >
   : missing_virtual_destructor_default_delete<T, U>
{};

}  
}  























































 



























 



 




namespace boost{
namespace move_upd {

namespace bmupmu = ::boost::move_upmu;








template<class U, class T>
struct def_del_compatible_cond
   : bmupmu::is_convertible<U*, T*>
{};

template<class U, class T, std::size_t N>
struct def_del_compatible_cond<U[N], T[]>
   : def_del_compatible_cond<U[], T[]>
{};

template<class U, class T, class Type = bmupmu::nat>
struct enable_def_del
   : bmupmu::enable_if_c<def_del_compatible_cond<U, T>::value, Type>
{};









template<class U, class T, class Type = bmupmu::nat>
struct enable_defdel_call
   : public enable_def_del<U, T, Type>
{};

template<class U, class T, class Type>
struct enable_defdel_call<U, T[], Type>
   : public enable_def_del<U[], T[], Type>
{};

template<class U, class T, class Type, std::size_t N>
struct enable_defdel_call<U, T[N], Type>
   : public enable_def_del<U[N], T[N], Type>
{};





struct bool_conversion {int for_bool; int for_arg(); };
typedef int bool_conversion::* explicit_bool_arg;

   typedef int (bool_conversion::*nullptr_type)();

}  

namespace movelib {

namespace bmupd = boost::move_upd;
namespace bmupmu = ::boost::move_upmu;





template <class T>
struct default_delete
{
   
   
    default_delete()
   
   {};

   typedef typename bmupmu::remove_extent<T>::type element_type;

   
   
   
   
   
   template <class U>
   default_delete(const default_delete<U>&
      , typename bmupd::enable_def_del<U , T> ::type* =0
      ) 
   {
      
      
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((( ! ::boost::move_upmu::missing_virtual_destructor<default_delete, U> ::value )) == 0 ? false : true) >)> boost_static_assert_typedef_136 __attribute__((unused));
   }

   
   
   
   
   
   template <class U>
   typename bmupd::enable_def_del<U , T , default_delete & > ::type
      operator=(const default_delete<U>&) 
   {
      
      
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((( ! ::boost::move_upmu::missing_virtual_destructor<default_delete, U> ::value )) == 0 ? false : true) >)> boost_static_assert_typedef_151 __attribute__((unused));
      return *this;
   }

   
   
   
   
   
   
   
   
   template <class U>
   typename bmupd::enable_defdel_call<U , T , void> ::type
      operator()(U* ptr) const 
   {
      
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((sizeof(U) > 0) == 0 ? false : true) >)> boost_static_assert_typedef_168 __attribute__((unused));
      
      
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((( ! ::boost::move_upmu::missing_virtual_destructor<default_delete, U> ::value )) == 0 ? false : true) >)> boost_static_assert_typedef_171 __attribute__((unused));
      element_type * const p = static_cast<element_type*>(ptr);
      bmupmu::is_array<T>::value ? delete [] p : delete p;
   }

   
   
   void operator()(bmupd::nullptr_type) const 
   {  typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((sizeof(element_type) > 0) == 0 ? false : true) >)> boost_static_assert_typedef_179 __attribute__((unused));  }
};

}  
}  


























































 



 







 


extern "C" {

 
extern void __assert_fail (const char *__assertion, const char *__file,
			   unsigned int __line, const char *__function)
     throw () __attribute__ ((__noreturn__));

 
extern void __assert_perror_fail (int __errnum, const char *__file,
				  unsigned int __line, const char *__function)
     throw () __attribute__ ((__noreturn__));



 
extern void __assert (const char *__assertion, const char *__file, int __line)
     throw () __attribute__ ((__noreturn__));


}







 














































 



























 



 
















namespace boost{
namespace move_upd {




template <class T>
class is_noncopyable
{
   typedef char true_t;
   class false_t { char dummy[2]; };
   template<class U> static false_t dispatch(...);
   template<class U> static true_t  dispatch(typename U::boost_move_no_copy_constructor_or_assign*);
   public:
   static const bool value = sizeof(dispatch<T>(0)) == sizeof(true_t);
};

template <class D>
struct deleter_types
{
   typedef typename bmupmu::add_lvalue_reference<D>::type            del_ref;
   typedef typename bmupmu::add_const_lvalue_reference<D>::type      del_cref;
   typedef typename bmupmu::if_c
      < is_noncopyable<D>::value, bmupmu::nat, del_cref>::type       non_ref_deleter_arg1;
   typedef typename bmupmu::if_c< bmupmu::is_lvalue_reference<D>::value
                       , D, non_ref_deleter_arg1 >::type          deleter_arg_type1;
   typedef ::boost::rv<D> &                                       deleter_arg_type2;
};




template <class P, class D, bool = bmupmu::is_unary_function<D>::value || bmupmu::is_reference<D>::value >
struct unique_ptr_data
{
   typedef typename deleter_types<D>::deleter_arg_type1  deleter_arg_type1;
   typedef typename deleter_types<D>::del_ref            del_ref;
   typedef typename deleter_types<D>::del_cref           del_cref;

   unique_ptr_data() 
      : m_p(), d()
   {}

   explicit unique_ptr_data(P p) 
      : m_p(p), d()
   {}

   unique_ptr_data(P p, deleter_arg_type1 d1) 
      : m_p(p), d(d1)
   {}

   template <class U>
   unique_ptr_data(P p, const U & d) 
      : m_p(p), d(::boost::forward<U>(d))
   {}

   del_ref deleter()       { return d; }
   del_cref deleter() const{ return d; }

   P m_p;
   D d;

   private:
   unique_ptr_data& operator=(const unique_ptr_data&);
   unique_ptr_data(const unique_ptr_data&);
};

template <class P, class D>
struct unique_ptr_data<P, D, false>
   : private D
{
   typedef typename deleter_types<D>::deleter_arg_type1  deleter_arg_type1;
   typedef typename deleter_types<D>::del_ref            del_ref;
   typedef typename deleter_types<D>::del_cref           del_cref;

   unique_ptr_data() 
      : D(), m_p()
   {}

   explicit unique_ptr_data(P p) 
      : D(), m_p(p)
   {}

   unique_ptr_data(P p, deleter_arg_type1 d1) 
      : D(d1), m_p(p)
   {}

   template <class U>
   unique_ptr_data(P p, const U & d) 
      : D(::boost::forward<U>(d)), m_p(p)
   {}

   del_ref deleter()           {  return static_cast<del_ref>(*this);   }
   del_cref deleter() const    {  return static_cast<del_cref>(*this);  }

   P m_p;

   private:
   unique_ptr_data& operator=(const unique_ptr_data&);
   unique_ptr_data(const unique_ptr_data&);
};







template <typename T>
struct get_element_type
{
   struct DefaultWrap { typedef bmupmu::natify<T> element_type; };
   template <typename X>   static char test(int, typename X::element_type*);
   template <typename X>   static int test(...);
   static const bool value = (1 == sizeof(test<T>(0, 0)));
   typedef typename bmupmu::if_c<value, T, DefaultWrap>::type::element_type type;
};

template<class T>
struct get_element_type<T*>
{
   typedef T type;
};

template<class T>
struct get_cvelement
   : bmupmu::remove_cv<typename get_element_type<T>::type>
{};

template <class P1, class P2>
struct is_same_cvelement_and_convertible
{
   typedef typename bmupmu::remove_reference<P1>::type arg1;
   typedef typename bmupmu::remove_reference<P2>::type arg2;
   static const bool same_cvless =
      bmupmu::is_same<typename get_cvelement<arg1>::type,typename get_cvelement<arg2>::type>::value;
   static const bool value = same_cvless && bmupmu::is_convertible<arg1, arg2>::value;
};

template<bool IsArray, class FromPointer, class ThisPointer>
struct is_unique_ptr_convertible
   : is_same_cvelement_and_convertible<FromPointer, ThisPointer>
{};

template<class FromPointer, class ThisPointer>
struct is_unique_ptr_convertible<false, FromPointer, ThisPointer>
   : bmupmu::is_convertible<FromPointer, ThisPointer>
{};





template<class T, class FromPointer, class ThisPointer, class Type = bmupmu::nat>
struct enable_up_ptr
   : bmupmu::enable_if_c< is_unique_ptr_convertible
      < bmupmu::is_array<T>::value, FromPointer, ThisPointer>::value, Type>
{};





template<class T, class D, class U, class E>
struct unique_moveconvert_assignable
{
   static const bool value = (bmupmu::extent<T>::value == bmupmu::extent<U>::value) && is_unique_ptr_convertible
      < bmupmu::is_array<T>::value
      , typename bmupmu::pointer_type<U, E>::type, typename bmupmu::pointer_type<T, D>::type>::value;
};

template<class T, class D, class U, class E, std::size_t N>
struct unique_moveconvert_assignable<T[], D, U[N], E>
   : unique_moveconvert_assignable<T[], D, U[], E>
{};

template<class T, class D, class U, class E, class Type = bmupmu::nat>
struct enable_up_moveconv_assign
   : bmupmu::enable_if_c<unique_moveconvert_assignable<T, D, U, E>::value, Type>
{};





template<class D, class E, bool IsReference = bmupmu::is_reference<D>::value>
struct unique_deleter_is_initializable
   : bmupmu::is_same<D, E>
{};

template <class T, class U>
class is_rvalue_convertible
{
   typedef typename bmupmu::if_c
      < ::boost::has_move_emulation_enabled<T>::value && !bmupmu::is_reference<T>::value
      , ::boost::rv<T>&
      , typename bmupmu::add_lvalue_reference<T>::type
      >::type t_from;

   typedef char true_t;
   class false_t { char dummy[2]; };
   static false_t dispatch(...);
   static true_t  dispatch(U);
   static t_from trigger();
   public:
   static const bool value = sizeof(dispatch(trigger())) == sizeof(true_t);
};

template<class D, class E>
struct unique_deleter_is_initializable<D, E, false>
{
   
   
   static const bool value = true;   
};

template<class T, class D, class U, class E, class Type = bmupmu::nat>
struct enable_up_moveconv_constr
   : bmupmu::enable_if_c<unique_moveconvert_assignable<T, D, U, E>::value &&
                      unique_deleter_is_initializable<D, E>::value, Type>
{};

}  

namespace movelib {













































template <class T, class D = default_delete<T> >
class unique_ptr
{
   private: unique_ptr(unique_ptr &); unique_ptr& operator=(unique_ptr &); public: typedef int boost_move_no_copy_constructor_or_assign; private: public: operator ::boost::rv<unique_ptr> &() { return *static_cast< ::boost::rv<unique_ptr> * >(this); } operator const ::boost::rv<unique_ptr> &() const { return *static_cast<const ::boost::rv<unique_ptr> * >(this); } private:

   typedef bmupmu::pointer_type<T, D >                            pointer_type_obtainer;
   typedef bmupd::unique_ptr_data
      <typename pointer_type_obtainer::type, D>                data_type;
   typedef typename bmupd::deleter_types<D>::deleter_arg_type1 deleter_arg_type1;
   typedef typename bmupd::deleter_types<D>::deleter_arg_type2 deleter_arg_type2;
   data_type m_data;

   public:
   
   
   
   typedef typename pointer_type_obtainer::type pointer;
   
   
   typedef typename bmupmu::remove_extent<T> ::type element_type;
   typedef D deleter_type;

   
   
   
   
   
   
   
   
   
   
    unique_ptr() 
      : m_data()
   {
      
      
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((!bmupmu::is_pointer<D> ::value) == 0 ? false : true) >)> boost_static_assert_typedef_380 __attribute__((unused));
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((!bmupmu::is_reference<D> ::value) == 0 ? false : true) >)> boost_static_assert_typedef_381 __attribute__((unused));
   }

   
   
    unique_ptr(bmupd::nullptr_type) 
      : m_data()
   {
      
      
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((!bmupmu::is_pointer<D> ::value) == 0 ? false : true) >)> boost_static_assert_typedef_391 __attribute__((unused));
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((!bmupmu::is_reference<D> ::value) == 0 ? false : true) >)> boost_static_assert_typedef_392 __attribute__((unused));
   }

   
   
   
   
   
   
   
   
   
   
   
   
   
   template<class Pointer>
   explicit unique_ptr(Pointer p
      , typename bmupd::enable_up_ptr<T , Pointer , pointer> ::type* =0
                 ) 
      : m_data(p)
   {
      
      
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((( ! ::boost::move_upmu::missing_virtual_destructor <D, typename bmupd::get_element_type<Pointer> ::type> ::value )) == 0 ? false : true) >)> boost_static_assert_typedef_417 __attribute__((unused));
      
      
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((!bmupmu::is_pointer<D> ::value) == 0 ? false : true) >)> boost_static_assert_typedef_420 __attribute__((unused));
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((!bmupmu::is_reference<D> ::value) == 0 ? false : true) >)> boost_static_assert_typedef_421 __attribute__((unused));
   }

   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   template<class Pointer>
   unique_ptr(Pointer p, deleter_arg_type1 d1
      , typename bmupd::enable_up_ptr<T , Pointer , pointer> ::type* =0
              ) 
      : m_data(p, d1)
   {
      
      
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((( ! ::boost::move_upmu::missing_virtual_destructor <D, typename bmupd::get_element_type<Pointer> ::type> ::value )) == 0 ? false : true) >)> boost_static_assert_typedef_455 __attribute__((unused));
   }

   
   
   unique_ptr(bmupd::nullptr_type, deleter_arg_type1 d1) 
      : m_data(pointer(), d1)
   {}

   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   template<class Pointer>
   unique_ptr(Pointer p, deleter_arg_type2 d2
      , typename bmupd::enable_up_ptr<T , Pointer , pointer> ::type* =0
             ) 
      : m_data(p, ::boost::move(d2))
   {
      
      
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((( ! ::boost::move_upmu::missing_virtual_destructor <D, typename bmupd::get_element_type<Pointer> ::type> ::value )) == 0 ? false : true) >)> boost_static_assert_typedef_493 __attribute__((unused));
   }

   
   
   unique_ptr(bmupd::nullptr_type, deleter_arg_type2 d2) 
      : m_data(pointer(), ::boost::move(d2))
   {}

   
   
   
   
   
   
   
   
   
   
   unique_ptr(::boost::rv< unique_ptr > & u) 
      : m_data(u.release(), ::boost::move_if_not_lvalue_reference<D>(u.get_deleter()))
   {}

   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   template <class U, class E>
   unique_ptr( ::boost::rv< unique_ptr<U, E> > & u
      , typename bmupd::enable_up_moveconv_constr<T , D , U , E> ::type* =0
      ) 
      : m_data(u.release(), ::boost::move_if_not_lvalue_reference<E>(u.get_deleter()))
   {
      
      
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((( ! ::boost::move_upmu::missing_virtual_destructor <D, typename unique_ptr<U, E> ::pointer> ::value )) == 0 ? false : true) >)> boost_static_assert_typedef_540 __attribute__((unused));
   }

   
   
   
   
   
   
   ~unique_ptr()
   {  if(m_data.m_p) m_data.deleter()(m_data.m_p);   }

   
   
   
   
   
   
   
   
   
   unique_ptr& operator=(::boost::rv< unique_ptr > & u) 
   {
      this->reset(u.release());
      m_data.deleter() = ::boost::move_if_not_lvalue_reference<D>(u.get_deleter());
      return *this;
   }

   
   
   
   
   
   
   
   
   
   
   
   
   template <class U, class E>
   typename bmupd::enable_up_moveconv_assign <T , D , U , E , unique_ptr & > ::type
      operator=(::boost::rv< unique_ptr<U, E> > & u) 
   {
      this->reset(u.release());
      m_data.deleter() = ::boost::move_if_not_lvalue_reference<E>(u.get_deleter());
      return *this;
   }

   
   
   
   
   
   unique_ptr& operator=(bmupd::nullptr_type) 
   {  this->reset(); return *this;  }

   
   
   
   
   
   typename bmupmu::add_lvalue_reference<element_type> ::type
      operator*() const 
   {
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (((!bmupmu::is_array<T> ::value)) == 0 ? false : true) >)> boost_static_assert_typedef_606 __attribute__((unused));
      return *m_data.m_p;
   }

   
   
   
   
   
   typename bmupmu::add_lvalue_reference<element_type> ::type
      operator[](std::size_t i) const 
   {
      ((bmupmu::extent<T> ::value == 0 || i < bmupmu::extent<T> ::value) ? static_cast<void> (0) : __assert_fail ("bmupmu::extent<T>::value == 0 || i < bmupmu::extent<T>::value", "unique_ptr.hpp", 618, __PRETTY_FUNCTION__));
      ((m_data . m_p) ? static_cast<void> (0) : __assert_fail ("m_data.m_p", "unique_ptr.hpp", 619, __PRETTY_FUNCTION__));
      return m_data.m_p[i];
   }

   
   
   
   
   
   
   
   pointer operator->() const 
   {
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (((!bmupmu::is_array<T> ::value)) == 0 ? false : true) >)> boost_static_assert_typedef_632 __attribute__((unused));
      ((m_data . m_p) ? static_cast<void> (0) : __assert_fail ("m_data.m_p", "unique_ptr.hpp", 633, __PRETTY_FUNCTION__));
      return m_data.m_p;
   }

   
   
   pointer get() const 
   {  return m_data.m_p;  }

   
   
   typename bmupmu::add_lvalue_reference<D> ::type
      get_deleter() 
   {  return m_data.deleter();  }   

   
   
   typename bmupmu::add_const_lvalue_reference<D> ::type
      get_deleter() const 
   {  return m_data.deleter();  }

   operator bmupd::explicit_bool_arg
      ()const 
   {
      return m_data.m_p
         ? &bmupd::bool_conversion::for_bool
         : bmupd::explicit_bool_arg(0);
   }

   
   
   
   pointer release() 
   {
      const pointer tmp = m_data.m_p;
      m_data.m_p = pointer();
      return tmp;
   }

   
   
   
   
   
   
   
   
   
   
   
   
   
   template<class Pointer>
   typename bmupd::enable_up_ptr<T , Pointer , pointer , void> ::type
      reset(Pointer p) 
   {
      
      
      typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< ((( ! ::boost::move_upmu::missing_virtual_destructor <D, typename bmupd::get_element_type<Pointer> ::type> ::value )) == 0 ? false : true) >)> boost_static_assert_typedef_698 __attribute__((unused));
      pointer tmp = m_data.m_p;
      m_data.m_p = p;
      if(tmp) m_data.deleter()(tmp);
   }

   
   
   
   
   
   
   
   
   
   void reset() 
   {  this->reset(pointer());  }

   
   
   void reset(bmupd::nullptr_type) 
   {  this->reset(); }

   
   
   
   void swap(unique_ptr& u) 
   {
      using ::boost::move_detail::swap;
      swap(m_data.m_p, u.m_data.m_p);
      swap(m_data.deleter(), u.m_data.deleter());
   }
};



template <class T, class D>
inline void swap(unique_ptr<T, D> &x, unique_ptr<T, D> &y) 
{  x.swap(y); }



template <class T1, class D1, class T2, class D2>
inline bool operator==(const unique_ptr<T1, D1> &x, const unique_ptr<T2, D2> &y)
{  return x.get() == y.get(); }



template <class T1, class D1, class T2, class D2>
inline bool operator!=(const unique_ptr<T1, D1> &x, const unique_ptr<T2, D2> &y)
{  return x.get() != y.get(); }





template <class T1, class D1, class T2, class D2>
inline bool operator<(const unique_ptr<T1, D1> &x, const unique_ptr<T2, D2> &y)
{  return x.get() < y.get();  }



template <class T1, class D1, class T2, class D2>
inline bool operator<=(const unique_ptr<T1, D1> &x, const unique_ptr<T2, D2> &y)
{  return !(y < x);  }



template <class T1, class D1, class T2, class D2>
inline bool operator>(const unique_ptr<T1, D1> &x, const unique_ptr<T2, D2> &y)
{  return y < x;  }



template <class T1, class D1, class T2, class D2>
inline bool operator>=(const unique_ptr<T1, D1> &x, const unique_ptr<T2, D2> &y)
{  return !(x < y);  }



template <class T, class D>
inline bool operator==(const unique_ptr<T, D> &x, bmupd::nullptr_type) 
{  return !x;  }



template <class T, class D>
inline bool operator==(bmupd::nullptr_type, const unique_ptr<T, D> &x) 
{  return !x;  }



template <class T, class D>
inline bool operator!=(const unique_ptr<T, D> &x, bmupd::nullptr_type) 
{  return !!x;  }



template <class T, class D>
inline bool operator!=(bmupd::nullptr_type, const unique_ptr<T, D> &x) 
{  return !!x;  }




template <class T, class D>
inline bool operator<(const unique_ptr<T, D> &x, bmupd::nullptr_type)
{  return x.get() < typename unique_ptr<T, D>::pointer();  }




template <class T, class D>
inline bool operator<(bmupd::nullptr_type, const unique_ptr<T, D> &x)
{  return typename unique_ptr<T, D>::pointer() < x.get();  }



template <class T, class D>
inline bool operator>(const unique_ptr<T, D> &x, bmupd::nullptr_type)
{  return x.get() > typename unique_ptr<T, D>::pointer();  }



template <class T, class D>
inline bool operator>(bmupd::nullptr_type, const unique_ptr<T, D> &x)
{  return typename unique_ptr<T, D>::pointer() > x.get();  }



template <class T, class D>
inline bool operator<=(const unique_ptr<T, D> &x, bmupd::nullptr_type)
{  return !(bmupd::nullptr_type() < x);  }



template <class T, class D>
inline bool operator<=(bmupd::nullptr_type, const unique_ptr<T, D> &x)
{  return !(x < bmupd::nullptr_type());  }



template <class T, class D>
inline bool operator>=(const unique_ptr<T, D> &x, bmupd::nullptr_type)
{  return !(x < bmupd::nullptr_type());  }



template <class T, class D>
inline bool operator>=(bmupd::nullptr_type, const unique_ptr<T, D> &x)
{  return !(bmupd::nullptr_type() < x);  }

}  
}  











































 



























 



 







 

namespace std {   

struct nothrow_t;

}  

namespace boost{
namespace move_upmu {




template<class T>
struct unique_ptr_if
{
   typedef ::boost::movelib::unique_ptr<T> t_is_not_array;
};

template<class T>
struct unique_ptr_if<T[]>
{
   typedef ::boost::movelib::unique_ptr<T[]> t_is_array_of_unknown_bound;
};

template<class T, std::size_t N>
struct unique_ptr_if<T[N]>
{
   typedef void t_is_array_of_known_bound;
};

static std::nothrow_t *pnothrow;

}  
}  


namespace boost{
namespace movelib {


   
   template<class T>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique()
   {  return unique_ptr<T>(new T());  }

   template<class T>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique_nothrow()
   {  return unique_ptr<T>(new (*boost::move_upmu::pnothrow)T());  }

   
   template<class T, class P0>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique( const P0 & p0
                 )
   {
      return unique_ptr<T>
         (  new T( ::boost::forward<P0>(p0)
                 )
         );
   }

   template<class T, class P0>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique_nothrow( const P0 & p0
                 )
   {
      return unique_ptr<T>
         (  new (*boost::move_upmu::pnothrow)T ( ::boost::forward<P0>(p0)
                              )
         );
   }
   
   template<class T, class P0, class P1>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique( const P0 & p0
                 , const P1 & p1
                 )
   {
      return unique_ptr<T>
         (  new T( ::boost::forward<P0>(p0)
                 , ::boost::forward<P1>(p1)
                 )
         );
   }

   template<class T, class P0, class P1>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique_nothrow( const P0 & p0
                         , const P1 & p1
                         )
   {
      return unique_ptr<T>
         (  new (*boost::move_upmu::pnothrow)T ( ::boost::forward<P0>(p0)
                              , ::boost::forward<P1>(p1)
                              )
         );
   }
   
   template<class T, class P0, class P1, class P2>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique( const P0 & p0
                 , const P1 & p1
                 , const P2 & p2
                 )
   {
      return unique_ptr<T>
         (  new T( ::boost::forward<P0>(p0)
                 , ::boost::forward<P1>(p1)
                 , ::boost::forward<P2>(p2)
                 )
         );
   }

   template<class T, class P0, class P1, class P2>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique_nothrow  ( const P0 & p0
                           , const P1 & p1
                           , const P2 & p2
                           )
   {
      return unique_ptr<T>
         (  new (*boost::move_upmu::pnothrow)T ( ::boost::forward<P0>(p0)
                              , ::boost::forward<P1>(p1)
                              , ::boost::forward<P2>(p2)
                              )
         );
   }
   
   template<class T, class P0, class P1, class P2, class P3>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique( const P0 & p0
                 , const P1 & p1
                 , const P2 & p2
                 , const P3 & p3
                 )
   {
      return unique_ptr<T>
         (  new T( ::boost::forward<P0>(p0)
                 , ::boost::forward<P1>(p1)
                 , ::boost::forward<P2>(p2)
                 , ::boost::forward<P3>(p3)
                 )
         );
   }

   template<class T, class P0, class P1, class P2, class P3>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique_nothrow  ( const P0 & p0
                           , const P1 & p1
                           , const P2 & p2
                           , const P3 & p3
                           )
   {
      return unique_ptr<T>
         (  new (*boost::move_upmu::pnothrow)T ( ::boost::forward<P0>(p0)
                              , ::boost::forward<P1>(p1)
                              , ::boost::forward<P2>(p2)
                              , ::boost::forward<P3>(p3)
                              )
         );
   }
   
   template<class T, class P0, class P1, class P2, class P3, class P4>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique( const P0 & p0
                 , const P1 & p1
                 , const P2 & p2
                 , const P3 & p3
                 , const P4 & p4
                 )
   {
      return unique_ptr<T>
         (  new T( ::boost::forward<P0>(p0)
                 , ::boost::forward<P1>(p1)
                 , ::boost::forward<P2>(p2)
                 , ::boost::forward<P3>(p3)
                 , ::boost::forward<P4>(p4)
                 )
         );
   }

   template<class T, class P0, class P1, class P2, class P3, class P4>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique_nothrow  ( const P0 & p0
                           , const P1 & p1
                           , const P2 & p2
                           , const P3 & p3
                           , const P4 & p4
                           )
   {
      return unique_ptr<T>
         (  new (*boost::move_upmu::pnothrow)T ( ::boost::forward<P0>(p0)
                              , ::boost::forward<P1>(p1)
                              , ::boost::forward<P2>(p2)
                              , ::boost::forward<P3>(p3)
                              , ::boost::forward<P4>(p4)
                              )
         );
   }
   
   template<class T, class P0, class P1, class P2, class P3, class P4, class P5>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique( const P0 & p0
                 , const P1 & p1
                 , const P2 & p2
                 , const P3 & p3
                 , const P4 & p4
                 , const P5 & p5
                 )
   {
      return unique_ptr<T>
         (  new T( ::boost::forward<P0>(p0)
                 , ::boost::forward<P1>(p1)
                 , ::boost::forward<P2>(p2)
                 , ::boost::forward<P3>(p3)
                 , ::boost::forward<P4>(p4)
                 , ::boost::forward<P5>(p5)
                 )
         );
   }

   template<class T, class P0, class P1, class P2, class P3, class P4, class P5>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique_nothrow  ( const P0 & p0
                           , const P1 & p1
                           , const P2 & p2
                           , const P3 & p3
                           , const P4 & p4
                           , const P5 & p5
                           )
   {
      return unique_ptr<T>
         (  new (*boost::move_upmu::pnothrow)T ( ::boost::forward<P0>(p0)
                              , ::boost::forward<P1>(p1)
                              , ::boost::forward<P2>(p2)
                              , ::boost::forward<P3>(p3)
                              , ::boost::forward<P4>(p4)
                              , ::boost::forward<P5>(p5)
                              )
         );
   }
   
   template<class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique( const P0 & p0
                 , const P1 & p1
                 , const P2 & p2
                 , const P3 & p3
                 , const P4 & p4
                 , const P5 & p5
                 , const P6 & p6
                 )
   {
      return unique_ptr<T>
         (  new T( ::boost::forward<P0>(p0)
                 , ::boost::forward<P1>(p1)
                 , ::boost::forward<P2>(p2)
                 , ::boost::forward<P3>(p3)
                 , ::boost::forward<P4>(p4)
                 , ::boost::forward<P5>(p5)
                 , ::boost::forward<P6>(p6)
                 )
         );
   }

   template<class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique_nothrow  ( const P0 & p0
                           , const P1 & p1
                           , const P2 & p2
                           , const P3 & p3
                           , const P4 & p4
                           , const P5 & p5
                           , const P6 & p6
                           )
   {
      return unique_ptr<T>
         (  new (*boost::move_upmu::pnothrow)T ( ::boost::forward<P0>(p0)
                              , ::boost::forward<P1>(p1)
                              , ::boost::forward<P2>(p2)
                              , ::boost::forward<P3>(p3)
                              , ::boost::forward<P4>(p4)
                              , ::boost::forward<P5>(p5)
                              , ::boost::forward<P6>(p6)
                              )
         );
   }

   
   template<class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique( const P0 & p0
                 , const P1 & p1
                 , const P2 & p2
                 , const P3 & p3
                 , const P4 & p4
                 , const P5 & p5
                 , const P6 & p6
                 , const P7 & p7
                 )
   {
      return unique_ptr<T>
         (  new T( ::boost::forward<P0>(p0)
                 , ::boost::forward<P1>(p1)
                 , ::boost::forward<P2>(p2)
                 , ::boost::forward<P3>(p3)
                 , ::boost::forward<P4>(p4)
                 , ::boost::forward<P5>(p5)
                 , ::boost::forward<P6>(p6)
                 , ::boost::forward<P7>(p7)
                 )
         );
   }

   template<class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique_nothrow  ( const P0 & p0
                           , const P1 & p1
                           , const P2 & p2
                           , const P3 & p3
                           , const P4 & p4
                           , const P5 & p5
                           , const P6 & p6
                           , const P7 & p7
                           )
   {
      return unique_ptr<T>
         (  new (*boost::move_upmu::pnothrow)T ( ::boost::forward<P0>(p0)
                              , ::boost::forward<P1>(p1)
                              , ::boost::forward<P2>(p2)
                              , ::boost::forward<P3>(p3)
                              , ::boost::forward<P4>(p4)
                              , ::boost::forward<P5>(p5)
                              , ::boost::forward<P6>(p6)
                              , ::boost::forward<P7>(p7)
                              )
         );
   }
   
   template<class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique( const P0 & p0
                 , const P1 & p1
                 , const P2 & p2
                 , const P3 & p3
                 , const P4 & p4
                 , const P5 & p5
                 , const P6 & p6
                 , const P7 & p7
                 , const P8 & p8
                 )
   {
      return unique_ptr<T>
         (  new T( ::boost::forward<P0>(p0)
                 , ::boost::forward<P1>(p1)
                 , ::boost::forward<P2>(p2)
                 , ::boost::forward<P3>(p3)
                 , ::boost::forward<P4>(p4)
                 , ::boost::forward<P5>(p5)
                 , ::boost::forward<P6>(p6)
                 , ::boost::forward<P7>(p7)
                 , ::boost::forward<P8>(p8)
                 )
         );
   }

   template<class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique_nothrow  ( const P0 & p0
                           , const P1 & p1
                           , const P2 & p2
                           , const P3 & p3
                           , const P4 & p4
                           , const P5 & p5
                           , const P6 & p6
                           , const P7 & p7
                           , const P8 & p8
                           )
   {
      return unique_ptr<T>
         (  new (*boost::move_upmu::pnothrow)T ( ::boost::forward<P0>(p0)
                              , ::boost::forward<P1>(p1)
                              , ::boost::forward<P2>(p2)
                              , ::boost::forward<P3>(p3)
                              , ::boost::forward<P4>(p4)
                              , ::boost::forward<P5>(p5)
                              , ::boost::forward<P6>(p6)
                              , ::boost::forward<P7>(p7)
                              , ::boost::forward<P8>(p8)
                              )
         );
   }
   
   template<class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique( const P0 & p0
                 , const P1 & p1
                 , const P2 & p2
                 , const P3 & p3
                 , const P4 & p4
                 , const P5 & p5
                 , const P6 & p6
                 , const P7 & p7
                 , const P8 & p8
                 , const P9 & p9
                 )
   {
      return unique_ptr<T>
         (  new T( ::boost::forward<P0>(p0)
                 , ::boost::forward<P1>(p1)
                 , ::boost::forward<P2>(p2)
                 , ::boost::forward<P3>(p3)
                 , ::boost::forward<P4>(p4)
                 , ::boost::forward<P5>(p5)
                 , ::boost::forward<P6>(p6)
                 , ::boost::forward<P7>(p7)
                 , ::boost::forward<P8>(p8)
                 , ::boost::forward<P9>(p9)
                 )
         );
   }

   template<class T, class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7, class P8, class P9>
   typename ::boost::move_upmu::unique_ptr_if<T>::t_is_not_array
      make_unique_nothrow  ( const P0 & p0
                           , const P1 & p1
                           , const P2 & p2
                           , const P3 & p3
                           , const P4 & p4
                           , const P5 & p5
                           , const P6 & p6
                           , const P7 & p7
                           , const P8 & p8
                           , const P9 & p9
                           )
   {
      return unique_ptr<T>
         (  new (*boost::move_upmu::pnothrow)T ( ::boost::forward<P0>(p0)
                              , ::boost::forward<P1>(p1)
                              , ::boost::forward<P2>(p2)
                              , ::boost::forward<P3>(p3)
                              , ::boost::forward<P4>(p4)
                              , ::boost::forward<P5>(p5)
                              , ::boost::forward<P6>(p6)
                              , ::boost::forward<P7>(p7)
                              , ::boost::forward<P8>(p8)
                              , ::boost::forward<P9>(p9)
                              )
         );
   }





template<class T>
inline typename ::boost::move_upmu::unique_ptr_if<T> ::t_is_not_array
      make_unique_definit()
{
    return unique_ptr<T>(new T);
}




template<class T>
inline typename ::boost::move_upmu::unique_ptr_if<T> ::t_is_not_array
      make_unique_nothrow_definit()
{
    return unique_ptr<T>(new (*boost::move_upmu::pnothrow)T);
}





template<class T>
inline typename ::boost::move_upmu::unique_ptr_if<T> ::t_is_array_of_unknown_bound
      make_unique(std::size_t n)
{
    typedef typename ::boost::move_upmu::remove_extent<T>::type U;
    return unique_ptr<T>(new U[n]());
}





template<class T>
inline typename ::boost::move_upmu::unique_ptr_if<T> ::t_is_array_of_unknown_bound
      make_unique_nothrow(std::size_t n)
{
    typedef typename ::boost::move_upmu::remove_extent<T>::type U;
    return unique_ptr<T>(new (*boost::move_upmu::pnothrow)U[n]());
}





template<class T>
inline typename ::boost::move_upmu::unique_ptr_if<T> ::t_is_array_of_unknown_bound
      make_unique_definit(std::size_t n)
{
    typedef typename ::boost::move_upmu::remove_extent<T>::type U;
    return unique_ptr<T>(new U[n]);
}





template<class T>
inline typename ::boost::move_upmu::unique_ptr_if<T> ::t_is_array_of_unknown_bound
      make_unique_nothrow_definit(std::size_t n)
{
    typedef typename ::boost::move_upmu::remove_extent<T>::type U;
    return unique_ptr<T>(new (*boost::move_upmu::pnothrow) U[n]);
}


}  

}  











