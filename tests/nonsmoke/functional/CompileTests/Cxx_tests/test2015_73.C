 
 
 
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

}


