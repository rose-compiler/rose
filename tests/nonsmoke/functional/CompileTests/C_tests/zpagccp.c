

typedef int sigset_t;
typedef unsigned int    speed_t;         
typedef unsigned long   tcflag_t;        
typedef unsigned char   cc_t;            
typedef int             pid_t;           
typedef unsigned short  mode_t;          
typedef short           nlink_t;         
typedef long            clock_t;         
typedef long            time_t;          
typedef long unsigned int size_t;                
typedef int ptrdiff_t;   
typedef short unsigned int wchar_t;      
extern unsigned int _mb_cur_max;
extern void     abort( );
extern int      abs  (int)  ;
extern double   atof  (const char *)  ;
extern int      atoi  (const char *)  ;
extern long int atol  (const char *)  ;
extern char *   bsearch  (const void *, const void *, long unsigned int , long unsigned int , int (*) (const void *, const void *))  ;
extern void *   calloc  (long unsigned int , long unsigned int )  ;
extern void     exit  (int)  ;
extern void     free  (void *)  ;
extern char *   getenv  (const char *)  ;
extern char *   malloc  (unsigned int )  ;
extern int      qsort  (void *, long unsigned int , long unsigned int , int (*) (const void *, const void *))  ;
extern int      rand( );
extern void *   realloc  (void *, long unsigned int )  ;
extern int      srand  (unsigned int)  ;
extern int    mbtowc  (short unsigned int *, const char *, long unsigned int )  ;
extern int    wctomb  (char *, short unsigned int )  ;
extern size_t mbstowcs  (short unsigned int *, const char *, long unsigned int )  ;
extern size_t wcstombs  (char *, const short unsigned int *, long unsigned int )  ;
extern char *   strcat  (char *, const char *)  ;
extern char *   strchr  (const char *, int)  ;
extern int      strcmp  (const char *, const char *)  ;
extern char *   strcpy  (char *, const char *)  ;
extern size_t   strcspn  (const char *, const char *)  ;
extern char *   strdup  (const char *)  ;
extern size_t   strlen  (const char *)  ;
extern char *   strncat  (char *, const char *, long unsigned int )  ;
extern int      strncmp  (const char *, const char *, long unsigned int )  ;
extern char *   strncpy  (char *, const char *, long unsigned int )  ;
extern char *   strpbrk  (const char *, const char *)  ;
extern char *   strrchr  (const char *, int)  ;
extern size_t   strspn  (const char *, const char *)  ;
extern char *   strstr  (const char *, const char *)  ;
extern char *   strtok  (char *, const char *)  ;
typedef char * __gnuc_va_list;
extern  struct  _iobuf {
        int     _cnt;
        unsigned char *_ptr;
        unsigned char *_base;
        int     _bufsiz;
        short   _flag;
        char    _file;           
} _iob[];
extern int      fprintf  (struct _iobuf  *, char *, ...)  ;
extern int fclose(struct _iobuf  *);
extern int fflush(struct _iobuf  *);
extern int system(const char*);
/*extern int mkdir(const* char, int);*/
extern int _flsbuf(unsigned char, struct _iobuf  *);
extern int      printf  (char *, ...)  ;
extern struct _iobuf    *fopen  (const char *, const char *)  ;
extern struct _iobuf    *fdopen  (int, const char *)  ;
extern struct _iobuf    *freopen  (const char *, const char *, struct _iobuf  *)  ;

extern struct _iobuf    *popen  (const char *, const char *)  ;
extern struct _iobuf    *tmpfile();
extern long     ftell  (struct _iobuf  *)  ;
extern char     *fgets  (char *, int, struct _iobuf  *)  ;
extern char     *gets  (char *)  ;
extern char     *sprintf  (char *, const char *, ...)  ;
extern char     *ctermid  (char *)  ;
extern char     *cuserid  (char *)  ;
extern char     *tempnam  (const char *, const char *)  ;
extern char     *tmpnam  (char *)  ;
extern void __eprintf (const char *, const char *, unsigned, const char *);
typedef unsigned char   u_char;
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;
typedef unsigned short  ushort;          
typedef unsigned int    uint;            
typedef struct  _physadr_t { int r[1]; } *physadr_t;
typedef struct label_t {
        int     val[2];
} label_t;
typedef struct  _quad_t { long val[2]; } quad_t;
typedef long    daddr_t;
typedef char *  caddr_t;
typedef unsigned long   ino_t;
typedef short   dev_t;
typedef long    off_t;
typedef unsigned short  uid_t;
typedef unsigned short  gid_t;
typedef long    key_t;
typedef char *  addr_t;
typedef long    fd_mask;
typedef struct fd_set {
        fd_mask fds_bits[((( 256  )+((  (sizeof (fd_mask) * 8 )  )-1))/(  (sizeof (fd_mask) * 8 )  )) ];
} fd_set;
typedef long NUMBER;
typedef unsigned long ulong;
extern int ein_neur_name;
int fatal_error__(char *,int);
extern int fatal_error_(char *, ...);
char *strip_extension ( char * );
char *extension ( char* );
char *basename ( char * );
char *pathname ( char * );
void memmove ( void *, void *, int );
double get_time ( void );
int get_size ( void );
typedef long unsigned int  MYINT;
void MYINT_init ( void );
char *MYINT_print ( MYINT );
void MYINT_print_stdout ( MYINT );
void MYINT_destroy_whole ( MYINT );
MYINT MYINT_dublicate_whole ( MYINT );
int  MYINT_eq ( MYINT, MYINT );
void MYINT_init(void)
{
   static int  flag = 0 ;
   flag = 1 ;
}
char *MYINT_print(MYINT elem)
{
        char *s;
        s  = (char *)malloc(sizeof(char)*20);
        if (s == 0 )
                ein_neur_name= fatal_error__("1.c",54)+fatal_error_ ("No memory");
                        sprintf(s,"%d",elem);
        return(s);
}
void MYINT_print_stdout(MYINT elem)
{
        fprintf((&_iob[1]) ,"%s",MYINT_print(elem));
}
void MYINT_destroy_whole(MYINT elem)
{
}
MYINT MYINT_dublicate_whole(MYINT elem)
{
        return(elem);
}
int  MYINT_eq(MYINT e1,MYINT  e2)
{
   return(e1==e2);
}
typedef struct MYINT_list_base_ 
{
        MYINT head;
        struct MYINT_list_base_ *tail;
} *MYINT_list,_MYINT_list;
extern long MYINT_list_power;
void MYINT_list_init(void);
void MYINT_list_destroy_whole(MYINT_list);
void MYINT_list_destroy(MYINT_list);
MYINT_list MYINT_list_new();
MYINT_list MYINT_list_extend(MYINT head, MYINT_list tail);
MYINT_list MYINT_list_conc ( MYINT_list, MYINT_list );
int MYINT_list_length ( MYINT_list );
MYINT MYINT_list_head ( MYINT_list );
MYINT_list MYINT_list_tail ( );
MYINT MYINT_list_get_elem ( MYINT_list, int );
MYINT_list MYINT_list_set_elem ( MYINT_list, int, MYINT );
MYINT_list MYINT_list_insert_bevor ( );
int  MYINT_list_is_empty ( );
void MYINT_list_print_stdout ( MYINT_list );
MYINT_list MYINT_list_append ( MYINT_list, MYINT );
MYINT_list MYINT_list_dublicate_whole ( MYINT_list );
MYINT_list MYINT_list_dublicate ( MYINT_list );
int  MYINT_list_is_elem ( MYINT_list, MYINT );
MYINT_list MYINT_list_remove ( MYINT_list, MYINT );
char *MYINT_list_print ( MYINT_list );
char *MYINT_list_vcgprint ( MYINT_list );
int  MYINT_list_eq ( MYINT_list, MYINT_list );
MYINT_list MYINT_list_rest ( MYINT_list, MYINT_list );
MYINT_list MYINT_list_vereinigung ( MYINT_list, MYINT_list );
MYINT_list MYINT_list_schnitt ( MYINT_list, MYINT_list );
static MYINT_list _MYINT_list_dublicate ( MYINT_list, int  );
long MYINT_list_power;
void MYINT_list_init(void)
{
   static int  flag = 0 ;
        if (!flag) {
        flag = 1 ;
        MYINT_list_power = -1 ;
                MYINT_init();
        }
}
MYINT_list MYINT_list_new()
{
        return(0 );
}
MYINT_list MYINT_list_extend(MYINT head, MYINT_list tail)
{
        MYINT_list help;
        help = (MYINT_list) malloc( sizeof(*help) ) ;
        if (help == 0 )
                ein_neur_name= fatal_error__("1.c",181)+fatal_error_ ("No Memory in  function MYINT_list_extend\n");    
        help->head = head;
        help->tail = tail;
        return(help);
}
void MYINT_list_destroy(list)
MYINT_list list;
{
        MYINT_list help;
        while(list!= 0 )
        {
                help = list;
                list = list->tail;
                free((void *)help);
        }
}
void MYINT_list_destroy_whole(list)
MYINT_list list;
{
        MYINT_list help;
        while(list!= 0 )
        {
                help = list;
                MYINT_destroy_whole(help->head);
                list = list->tail;
      free((void *)help);
        }
}
MYINT_list MYINT_list_conc(MYINT_list list1, MYINT_list list2)
{
        MYINT_list help,merk;
        if (list1== 0 )
                return(list2);
        if (list2 == 0 )
                return(list1);
        merk = help = MYINT_list_dublicate(list1);
        while(help->tail!= 0 )
                help=help->tail;
        help->tail=list2;
        return(merk);
}
        
int MYINT_list_length(MYINT_list list)
{
        if (list == 0 )
                return(0);
        return(MYINT_list_length(list->tail)+1);
}
MYINT MYINT_list_head(MYINT_list list)
{
        ((void) (( list!= 0  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   249 ,  "list!= NULL" ), 0) )) ;
        return(list->head);
}
MYINT_list MYINT_list_tail(list)
MYINT_list list;
{
        ((void) (( list!= 0  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   259 ,  "list!= NULL" ), 0) )) ;
        return(list->tail);
}
MYINT MYINT_list_get_elem(MYINT_list list, int nr)
{
        ((void) (( nr>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   269 ,  "nr>0" ), 0) )) ;
        ((void) (( list!= 0  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   270 ,  "list!=NULL" ), 0) )) ;
        if (nr == 1)
                return(list->head);
        return(MYINT_list_get_elem(list->tail,nr-1));
}
MYINT_list MYINT_list_set_elem(MYINT_list list,int nr,MYINT elem)
{
        int i;
        MYINT_list help;
        ((void) (( nr>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   286 ,  "nr>0" ), 0) )) ;
        help = list;
        for(i=1;i<nr;i++)
        {
                ((void) (( help!= 0  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   290 ,  "help!=NULL" ), 0) )) ;
                help = help->tail;
        }
        ((void) (( help!= 0  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   293 ,  "help!=NULL" ), 0) )) ;
        help->head = elem;
        return(list);
}
MYINT_list MYINT_list_insert_bevor(list1,nr,list2)
MYINT_list list1,list2;
int nr;
{
        int i;
        MYINT_list help,help2;
        ((void) (( nr>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   311 ,  "nr>0" ), 0) )) ;
        help = list1;
        if (nr == 1) 
        {
                return(MYINT_list_conc(list2,list1));
        }
        for(i=2;i<nr;i++)
        {
                ((void) (( help != 0  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",       "1.c" ,   319 ,  "help != NULL" ), 0) )) ;
                help = help ->tail;
        }
        ((void) (( help= 0  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   322 ,  "help=NULL" ), 0) )) ;
        help2 = help->tail;
        help->tail = list2;
        (void) MYINT_list_conc(help,help2);
        return(list1);
}
int  MYINT_list_is_empty(list)
MYINT_list list;
{
        return(list == 0 );
}
void MYINT_list_print_stdout(MYINT_list list)
{
        char *s;
        s = MYINT_list_print(list);
        fprintf((&_iob[1]) ,"%s",s);
}
MYINT_list MYINT_list_append(MYINT_list list, MYINT elem)
{
        MYINT_list help1,help2;
        help1 = help2 = list;
        while (help2!= 0 ) {
                help1 = help2;
                help2 = help2->tail;
        }
        help2 = MYINT_list_extend(elem,0 );
        if (help1!= 0 ) {
                help1->tail = help2;
                return(list);
        }
        return(help2);
}
MYINT_list MYINT_list_dublicate_whole(MYINT_list list)
{
        return(_MYINT_list_dublicate(list,1 ));
}
MYINT_list MYINT_list_dublicate(MYINT_list list)
{
        return(_MYINT_list_dublicate(list,0 ));
}
static MYINT_list _MYINT_list_dublicate(MYINT_list list, int  whole_mode)
{
        MYINT_list help1,head,help;
        head = 0 ;
        help1 = 0 ;
        if (list== 0 ) return(0 );
        while(list != 0 ) {
   help = (MYINT_list) malloc(sizeof(*help));
                if (help == 0 ) {
                        ein_neur_name= fatal_error__("1.c",406)+fatal_error_ ("No Memory in  function MYINT_list_extend\n");    
                }
                help->head = whole_mode?MYINT_dublicate_whole(list->head):list->head;
                if (help1!= 0 )
                        help1->tail = help;
                else
                        head = help;
                help1 = help;
                list = list->tail;
        }
        help->tail= 0 ;
        return(head);
}
                
                
int  MYINT_list_is_elem(MYINT_list list,MYINT elem)
{
        while(list!= 0 ) {
                if (MYINT_eq(list->head,elem))
                        return(1 );
                list = list->tail;
        }
        return(0 );
}
MYINT_list MYINT_list_remove(MYINT_list list, MYINT elem)
{
        MYINT_list help1,help2;
        help1 =help2 = list;
        while(help2!= 0 ) {
                if (MYINT_eq(help2->head,elem))
                        break;
                help1 = help2;
                help2 = help2->tail;
        }
        if (help2== 0 )
                return(list);
        if (help2==list)
                return(help2->tail);
        help1->tail = help2->tail;
        return(list);
}
char *MYINT_list_print(MYINT_list list)
{
        int x,len = 1024;
        char *s,*e,*w;
        e = (char*)  malloc(sizeof(char)*len);
        if (e== 0 )
                ein_neur_name= fatal_error__("1.c",468)+fatal_error_ ("No memeory");
        e[0]='\0';
        strcpy(e,"[");
        x=strlen(e);
        while (!MYINT_list_is_empty(list)) {
                s = MYINT_print(MYINT_list_head(list));
                x += strlen(s);
                if (x+5 >= len) {
                        len *= 2;
                        w = (char*)malloc(sizeof(char)*len);
                        if (w== 0 )
                                ein_neur_name= fatal_error__("1.c",479)+fatal_error_ ("No memeory");
                        w[0] = '\0';
                        strcpy(w,e);
                        e = w;
                }
                strcat(e,s);
                strcat(e,",");
                x = strlen(e);
                list = MYINT_list_tail(list);
        }
        strcat(e,"]");
        return(e);
}
int  MYINT_list_eq(MYINT_list l1, MYINT_list l2)
{
        while(!MYINT_list_is_empty(l1)) {
                if  (MYINT_list_is_empty(l2)) 
                        return(0 );
                if (!MYINT_eq(MYINT_list_head(l1),MYINT_list_head(l2)))
                        return(0 );
                l1 = MYINT_list_tail(l1);
                l2 = MYINT_list_tail(l2);
        }
        if  (!MYINT_list_is_empty(l2)) 
                return(0 );
        return(1 );
}
                
MYINT_list MYINT_list_rest(MYINT_list l1, MYINT_list l2)
{
   MYINT_list l3;
   MYINT elem;
   l3 = MYINT_list_new();
   while(!MYINT_list_is_empty(l1)) {
      elem = MYINT_list_head(l1);
      if (!MYINT_list_is_elem(l2,elem))
         l3 = MYINT_list_extend(elem,l3);
      l1 = MYINT_list_tail(l1);
   }
   return(l3);
}
MYINT_list MYINT_list_vereinigung(MYINT_list l1, MYINT_list l2)
{
   MYINT_list l3;
   MYINT elem;
   l3 = MYINT_list_dublicate(l2);
   while(!MYINT_list_is_empty(l1)) {
      elem = MYINT_list_head(l1);
      if (!MYINT_list_is_elem(l2,elem))
         l3 = MYINT_list_extend(elem,l3);
      l1 = MYINT_list_tail(l1);
   }
   return(l3);
}
MYINT_list MYINT_list_schnitt(MYINT_list l1, MYINT_list l2)
{
   MYINT_list l3;
   MYINT elem;
   l3 = MYINT_list_new();
   while(!MYINT_list_is_empty(l1)) {
      elem = MYINT_list_head(l1);
      if (MYINT_list_is_elem(l2,elem))
         l3 = MYINT_list_extend(elem,l3);
      l1 = MYINT_list_tail(l1);
   }
   return(l3);
}
extern double   __infinity();
extern double   acos  (double)  ;
extern double   asin  (double)  ;
extern double   atan  (double)  ;
extern double   atan2  (double, double)  ;
extern double   ceil  (double)  ;
extern double   cos  (double)  ;
extern double   cosh  (double)  ;
extern double   exp  (double)  ;
extern double   fabs  (double)  ;
extern double   floor  (double)  ;
extern double   fmod  (double, double)  ;
extern double   frexp  (double, int *)  ;
extern double   ldexp  (double, int)  ;
extern double   log  (double)  ;
extern double   log10  (double)  ;
extern double   modf  (double, double *)  ;
extern double   pow  (double, double)  ;
extern double   sin  (double)  ;
extern double   sinh  (double)  ;
extern double   sqrt  (double)  ;
extern double   tan  (double)  ;
extern double   tanh  (double)  ;
enum fp_direction_type           
        {
        fp_nearest      = 0,
        fp_tozero       = 1,
        fp_positive     = 2,
        fp_negative     = 3
        } ;
enum fp_precision_type           
        {
        fp_extended     = 0,
        fp_single       = 1,
        fp_double       = 2,
        fp_precision_3  = 3
        } ;
enum fp_exception_type           
        {
        fp_inexact      = 0,
        fp_division     = 1,
        fp_underflow    = 2,
        fp_overflow     = 3,
        fp_invalid      = 4
        } ;
enum fp_class_type               
        {
        fp_zero         = 0,
        fp_subnormal    = 1,
        fp_normal       = 2,
        fp_infinity     = 3,
        fp_quiet        = 4,
        fp_signaling    = 5
        } ;
typedef float   single;
typedef unsigned long extended[3];       
typedef long double quadruple;   
typedef unsigned fp_exception_field_type;
typedef int     sigfpe_code_type;        
typedef void    (*sigfpe_handler_type) ();
extern enum fp_direction_type fp_direction;
extern enum fp_precision_type fp_precision;
extern fp_exception_field_type fp_accrued_exceptions;
typedef char    decimal_string[512 ];
typedef struct {
        enum fp_class_type fpclass;
        int             sign;
        int             exponent;
        decimal_string  ds;      
        int             more;    
        int             ndigits; 
}
                decimal_record;
enum decimal_form {
        fixed_form,              
        floating_form            
};
typedef struct {
        enum fp_direction_type rd;
         
        enum decimal_form df;    
        int             ndigits; 
}
                decimal_mode;
enum decimal_string_form {       
        invalid_form,            
        whitespace_form,         
        fixed_int_form,          
        fixed_intdot_form,       
        fixed_dotfrac_form,      
        fixed_intdotfrac_form,   
        floating_int_form,       
        floating_intdot_form,    
        floating_dotfrac_form,   
        floating_intdotfrac_form,        
        inf_form,                
        infinity_form,           
        nan_form,                
        nanstring_form           
};
extern void     double_to_decimal();
extern void     quadruple_to_decimal();
extern char    *econvert();
extern char    *fconvert();
extern char    *gconvert();
extern char    *qeconvert();
extern char    *qfconvert();
extern char    *qgconvert();
extern sigfpe_handler_type ieee_handlers[5 ];
extern sigfpe_handler_type sigfpe();
extern void     single_to_decimal();
extern void     extended_to_decimal();
extern void     decimal_to_single();
extern void     decimal_to_double();
extern void     decimal_to_extended();
extern void     decimal_to_quadruple();
extern char    *seconvert();
extern char    *sfconvert();
extern char    *sgconvert();
extern void     string_to_decimal();
extern void     file_to_decimal();
extern void     func_to_decimal();
extern double   atof  (const char *)  ;
extern int      errno;
extern double   strtod  (const char *, char **)  ;
extern double   acosh  (double)  ;
extern double   asinh  (double)  ;
extern double   atanh  (double)  ;
extern double   cbrt  (double)  ;
extern double   copysign  (double, double)  ;
extern double   erf  (double)  ;
extern double   erfc  (double)  ;
extern double   expm1();
extern int      finite  (double)  ;
extern double   hypot  (double, double)  ;
extern double   j0  (double)  ;
extern double   j1  (double)  ;
extern double   jn  (int, double)  ;
extern double   lgamma  (double)  ;
extern double   log1p();
extern double   rint  (double)  ;
extern double   y0  (double)  ;
extern double   y1  (double)  ;
extern double   yn  (int, double)  ;
enum fp_pi_type {
        fp_pi_infinite  = 0,     
        fp_pi_66        = 1,     
        fp_pi_53        = 2      
};
extern enum     fp_pi_type fp_pi;
extern enum     fp_class_type fp_class();
extern int      ieee_flags();
extern int      ieee_handler();
extern void     ieee_retrospective();
extern int      ilogb();
extern double   infinity();
extern int      irint();
extern int      isinf  (double)  ;
extern int      isnan  (double)  ;
extern int      isnormal();
extern int      issubnormal();
extern int      iszero();
extern double   logb  (double)  ;
extern double   max_normal();
extern double   max_subnormal();
extern double   min_normal();
extern double   min_subnormal();
extern double   nextafter  (double, double)  ;
extern void     nonstandard_arithmetic();
extern double   quiet_nan();
extern double   remainder  (double, double)  ;
extern double   scalb  (double, double)  ;
extern double   scalbn();
extern double   signaling_nan();
extern int      signbit();
extern double   significand();
extern void     standard_arithmetic();
extern double   acospi();
extern double   aint();
extern double   anint();
extern double   annuity();
extern double   asinpi();
extern double   atan2pi();
extern double   atanpi();
extern double   compound();
extern double   cospi();
extern double   exp10();
extern double   exp2();
extern double   log2();
extern int      nint();
extern void     sincos();
extern void     sincospi();
extern double   sinpi();
extern double   tanpi();
struct exception;
extern int      matherr  (struct exception *)  ;
extern int               ir_finite_();
extern enum fp_class_type ir_fp_class_();
extern int               ir_ilogb_();
extern int               ir_irint_();
extern int               ir_isinf_();
extern int               ir_isnan_();
extern int               ir_isnormal_();
extern int               ir_issubnormal_();
extern int               ir_iszero_();
extern int               ir_nint_();
extern int               ir_signbit_();
extern void              r_sincos_();
extern void              r_sincospi_();
extern double  r_acos_();
extern double  r_acosh_();
extern double  r_acospi_();
extern double  r_aint_();
extern double  r_anint_();
extern double  r_annuity_();
extern double  r_asin_();
extern double  r_asinh_();
extern double  r_asinpi_();
extern double  r_atan2_();
extern double  r_atan2pi_();
extern double  r_atan_();
extern double  r_atanh_();
extern double  r_atanpi_();
extern double  r_cbrt_();
extern double  r_ceil_();
extern double  r_compound_();
extern double  r_copysign_();
extern double  r_cos_();
extern double  r_cosh_();
extern double  r_cospi_();
extern double  r_erf_();
extern double  r_erfc_();
extern double  r_exp10_();
extern double  r_exp2_();
extern double  r_exp_();
extern double  r_expm1_();
extern double  r_fabs_();
extern double  r_floor_();
extern double  r_fmod_();
extern double  r_hypot_();
extern double  r_infinity_();
extern double  r_j0_();
extern double  r_j1_();
extern double  r_jn_();
extern double  r_lgamma_();
extern double  r_log10_();
extern double  r_log1p_();
extern double  r_log2_();
extern double  r_log_();
extern double  r_logb_();
extern double  r_max_normal_();
extern double  r_max_subnormal_();
extern double  r_min_normal_();
extern double  r_min_subnormal_();
extern double  r_nextafter_();
extern double  r_pow_();
extern double  r_quiet_nan_();
extern double  r_remainder_();
extern double  r_rint_();
extern double  r_scalb_();
extern double  r_scalbn_();
extern double  r_signaling_nan_();
extern double  r_significand_();
extern double  r_sin_();
extern double  r_sinh_();
extern double  r_sinpi_();
extern double  r_sqrt_();
extern double  r_tan_();
extern double  r_tanh_();
extern double  r_tanpi_();
extern double  r_y0_();
extern double  r_y1_();
extern double  r_yn_();
struct exception {
        int type;
        char *name;
        double arg1;
        double arg2;
        double retval;
};
extern int      signgam;
extern double cabs();    
extern double drem  (double, double)  ;  
extern double gamma  (double)  ;         
struct  tm {
        int     tm_sec;
        int     tm_min;
        int     tm_hour;
        int     tm_mday;
        int     tm_mon;
        int     tm_year;
        int     tm_wday;
        int     tm_yday;
        int     tm_isdst;
        char    *tm_zone;
        long    tm_gmtoff;
};
extern  struct tm *gmtime  (const long *)  , *localtime  (const long *)  ;
extern  char *asctime  (const struct tm *)  , *ctime  (const long *)  ;
extern  void tzset(), tzsetwall();
extern  int dysize  (int)  ;
extern  time_t timelocal(), timegm();
typedef struct __dirdesc {
        int     dd_fd;           
        long    dd_loc;          
        long    dd_size;         
        long    dd_bsize;        
        long    dd_off;          
        char    *dd_buf;         
} DIR;
extern  DIR *opendir  (const char *)  ;
extern  struct dirent *readdir  (DIR *)  ;
extern  int closedir  (DIR *)  ;
extern  void seekdir  (DIR *, long int)  ;
extern  long telldir  (DIR *)  ;
extern  void rewinddir( );
struct  dirent {
        off_t           d_off;           
        unsigned long   d_fileno;        
        unsigned short  d_reclen;        
        unsigned short  d_namlen;        
        char            d_name[255+1];   
};
typedef short int BYTE;
typedef int bool; 
extern long bool_power ;
char *bool_len ( void );
int bool_to_c ( bool );
bool bool_from_c ( int );
void bool_init ( void );
bool bool_and ( bool, bool );
bool bool_dublicate ( bool );
bool bool_dublicate_whole ( bool );
int  bool_eq ( bool, bool );
unsigned int bool_hash ( bool );
int  bool_neq ( bool, bool );
bool bool_create ( int  );
void bool_free ( bool );
void bool_destroy_whole ( bool );
char *bool_print ( bool );
char *bool_vcgprint ( bool );
void bool_print_stdout ( bool );
bool bool_or ( bool, bool );
bool bool_not ( bool );
long bool_power = 2;
char *bool_len(void)
{
        ein_neur_name= fatal_error__("1.c",635)+fatal_error_ ("len in bool");
        exit(1);
}
int bool_to_c(bool b)
{
        return(b);
}
bool bool_from_c(int i)
{
        return(i);
}
void bool_init(void)
{
        static int  flag = 0 ;
        flag = 1 ;
}
bool bool_and(bool n1, bool n2)
{
        return(n1 && n2);
}
bool bool_dublicate(bool inh)
{
                return(inh);
}
bool bool_dublicate_whole(bool inh)
{
                return(inh);
}
int  bool_eq(bool e1, bool e2)
{
        return(e1?e2:!e2);
}
unsigned int bool_hash(bool e) {
        return(e?0U:1U);
}
int  bool_neq(bool e1, bool e2)
{
        return(!bool_eq(e1,e2));
}
bool bool_create(int  i)
{
        return(i);
}
void bool_free(bool h)
{
}
void bool_destroy_whole(bool h)
{
}
char *bool_print(bool l)
{
        char *s;
        s = (char*)malloc( sizeof(char)*7 ) ;
        sprintf(s,"%s",l?"TRUE":"FALSE");
        return(s);
}
char *bool_vcgprint(bool l)
{
        char *s;
        s = (char*)malloc( sizeof(char)*7 ) ;
        sprintf(s,"%s",l?"\\mbox{\\sf T}":"\\mbox{\\sf F}");
        return(s);
}
void bool_print_stdout(bool l)
{
char *erg = bool_print(l);
        fprintf((&_iob[1]) ,"%s",erg);
free((void *) erg ) ;
}
bool bool_or(bool a,bool b)
{
        return(a||b);
}
bool bool_not(bool a)
{
        return(!a);
}
typedef char chr; 
extern long chr_power; 
unsigned int chr_hash ( chr );
char *chr_len ( void );
void chr_init ( void );
chr chr_dublicate ( chr );
chr chr_dublicate_whole ( chr );
int chr_eq ( chr, chr );
int chr_neq ( chr, chr );
chr chr_create ( char );
void chr_free ( chr );
void chr_destroy_whole ( chr );
char *chr_print ( chr );
char *chr_vcgprint ( chr );
void chr_print_stdout ( chr );
int  chr_le ( chr, chr );
int  chr_leq ( chr, chr );
int  chr_ge ( chr, chr );
int  chr_geq ( chr, chr );
int  chr_is_top ( chr );
int  chr_is_bottom ( chr );
chr chr_top ( void );
chr chr_bottom ( void );
chr chr_lub ( chr, chr );
chr chr_glb ( chr, chr );
long chr_power = 127 - (-128) +1;
unsigned int chr_hash(chr x) {
        return((unsigned int)x);
}
char *chr_len(void)
{
   ein_neur_name= fatal_error__("1.c",836)+fatal_error_ ("len in chr");
   exit(1);
        return("0");
}
void chr_init(void)
{
        static int  flag = 0 ;
        flag = 1 ;
}
chr chr_dublicate(chr inh)
{
                return(inh);
}
chr chr_dublicate_whole(chr inh)
{
                return(inh);
}
int chr_eq(chr e1, chr e2)
{
        return(e1==e2);
}
int chr_neq(chr e1, chr e2)
{
        return(e1!=e2);
}
chr chr_create(char i)
{
        return(i);
}
void chr_free(chr h)
{
}
void chr_destroy_whole(chr h)
{
}
char *chr_print(chr l)
{
        char *s;
        s = (char*)malloc( sizeof(char)*4 ) ;
        sprintf(s,"%c ",l);
        return(s);
}
char *chr_vcgprint(chr l)
{
        char *s;
        s = (char*)malloc( sizeof(char)*4 ) ;
        sprintf(s,"\\mbox{\tt %c} ",l);
        return(s);
}
void chr_print_stdout(chr l)
{
        char *s;
        s = chr_print(l);
        fprintf((&_iob[1]) ,"%s",s);
        free((void *) s ) ;
}
int  chr_le(chr a,chr b)
{
        return(a<b);
}
int  chr_leq(chr a,chr b)
{
        return(a<=b);
}
int  chr_ge(chr a,chr b)
{
        return(a>b);
}
int  chr_geq(chr a,chr b)
{
        return(a>=b);
}
int  chr_is_top(chr a)
{
        return(a== 127 );
}
int  chr_is_bottom(chr a)
{
        return(a== (-128) );
}
chr chr_top(void)
{
        return(127 );
}
chr chr_bottom(void)
{
        return((-128) );
}
chr chr_lub(chr a,chr b)
{
        return(a<b?b:a);
}
chr chr_glb(chr a,chr b)
{
        return(a<b?a:b);
}
extern int x64_control;          
typedef
struct {
        unsigned short  a[(64 / (8 *sizeof(short)) ) ];
} x64_t;
extern char * atox64(x64_t *, char *);
extern int    ctox64(x64_t *, char);
extern int    itox64(x64_t *, int);
extern int    ltox64(x64_t *, long);
extern int    stox64(x64_t *, short);
extern int    uctox64(x64_t *, unsigned char);
extern int    uitox64(x64_t *, unsigned int);
extern int    ultox64(x64_t *, unsigned long);
extern int    ustox64(x64_t *, unsigned short);
extern int    x64abs(x64_t *);
extern int    x64adc(x64_t *, int);
extern int    x64add(x64_t *, x64_t *);
extern int    x64and(x64_t *, x64_t *);
extern int    x64asl(x64_t *);
extern int    x64asr(x64_t *);
extern int    x64bic(x64_t *, int);
extern int    x64bin(x64_t *, int);
extern int    x64bis(x64_t *, int);
extern int    x64bit(x64_t *, int);
extern int    x64cmp(x64_t *, x64_t *);
extern int    x64dec(x64_t *);
extern int    x64div(x64_t *, x64_t *);
extern int    x64divrem(x64_t *, x64_t *);
extern int    x64eor(x64_t *, x64_t *);
extern int    x64inc(x64_t *);
extern void   x64left(x64_t *, int);
extern void   x64lin(x64_t *, long);
extern long   x64lout(x64_t *);
extern int    x64lsl(x64_t *);
extern int    x64lsr(x64_t *);
extern int    x64lxl(x64_t *, int);
extern int    x64lxr(x64_t *, int);
extern int    x64mod(x64_t *, x64_t *);
extern int    x64mul(x64_t *, x64_t *);
extern int    x64muldiv(x64_t *, x64_t *, x64_t *);
extern int    x64muldivrem(x64_t *, x64_t *, x64_t *);
extern int    x64mulmod(x64_t *, x64_t *, x64_t *);
extern int    x64mulquo(x64_t *, x64_t *, x64_t *);
extern int    x64mulquomod(x64_t *, x64_t *, x64_t *);
extern int    x64mulrem(x64_t *, x64_t *, x64_t *);
extern int    x64neg(x64_t *);
extern int    x64not(x64_t *);
extern int    x64or(x64_t *, x64_t *);
extern int    x64quo(x64_t *, x64_t *);
extern int    x64quomod(x64_t *, x64_t *);
extern int    x64rem(x64_t *, x64_t *);
extern void   x64right(x64_t *, int);
extern int    x64ext(x64_t *, int);
extern int    x64rol(x64_t *);
extern int    x64ror(x64_t *);
extern int    x64sbc(x64_t *, int);
extern int    x64sub(x64_t *, x64_t *);
extern char * x64toa(x64_t *, char *);
extern int    x64toc(x64_t *, char *);
extern int    x64toi(x64_t *, int *);
extern int    x64tol(x64_t *, long *);
extern int    x64tos(x64_t *, short *);
extern int    x64touc(x64_t *, unsigned char *);
extern int    x64toui(x64_t *, unsigned int *);
extern int    x64toul(x64_t *, unsigned long *);
extern int    x64tous(x64_t *, unsigned short *);
extern int    x64tst(x64_t *);
extern int    x64ucmp(x64_t *, x64_t *);
extern int    x64udiv(x64_t *, x64_t *);
extern int    x64udivrem(x64_t *, x64_t *);
extern int    x64umul(x64_t *, x64_t *);
extern int    x64umuldiv(x64_t *, x64_t *, x64_t *);
extern int    x64umuldivrem(x64_t *, x64_t *, x64_t *);
extern int    x64umulrem(x64_t *, x64_t *, x64_t *);
extern int    x64urem(x64_t *, x64_t *);
extern int    x64val(x64_t *, int);
extern  char    f80_infinity[];          
extern  char    f80_nonumber[];          
typedef short           cond_t;          
typedef short           bit_t;
typedef short           bool_t;
typedef short           status_t;
typedef short           control_t;
typedef short           sign_t;          
typedef unsigned short  mp_t;
typedef long            exp_t;
typedef unsigned long   convert_t;       
typedef
struct {
        convert_t       sval[((1  + 8  + (23 + 1 ) - 1 ) /(8 *sizeof(convert_t))) ];
} single_t;
typedef
struct {
        convert_t       dval[((1  + 11  + (52 + 1 ) - 1 ) /(8 *sizeof(convert_t))) ];
} double_t;
typedef
struct {
        convert_t       eval[((1  + (15 +16) + (64 + 0 ) - 0 ) /(8 *sizeof(convert_t))) ];
} extend_t;
typedef
struct {
        convert_t       pval[((1  + (15 +16) + (64 + 0 ) - 0 ) /(8 *sizeof(convert_t))) ];               
} packed_t;
typedef
struct {
        int     d_mbits;         
        int     d_rmbits;        
        int     d_hidden;        
        int     d_ebits;         
        int     d_rebits;        
        int     d_size;          
        exp_t   d_emask;         
        exp_t   d_emin;          
        exp_t   d_emax;
        exp_t   d_remin;
        exp_t   d_remax;
        exp_t   d_ebias;
} descr_t;
extern char *   dtoa(char *,...);
extern char *   etoa(char *,...);
extern char *   ftoa(char *,...);
extern char     dtoc(double_t);
extern char     etoc(extend_t);
extern char     ftoc(single_t);
extern double_t atod(char *);
extern double_t ctod(char);
extern double_t etod(extend_t);
extern double_t ftod(single_t);
extern double_t itod(int);
extern double_t ltod(long);
extern double_t satod(char *, char **);
extern double_t stod(short);
extern extend_t atoe(char *);
extern extend_t ctoe(char);
extern extend_t dtoe(double_t);
extern extend_t ftoe(single_t);
extern extend_t itoe(int);
extern extend_t ltoe(long);
extern extend_t satoe(char *, char **);
extern extend_t stoe(short);
extern int      dtoi(double_t);
extern int      etoi(extend_t);
extern int      ftoi(single_t);
extern long     dtol(double_t);
extern long     etol(extend_t);
extern long     ftol(single_t);
extern short    dtos(double_t);
extern short    etos(extend_t);
extern short    ftos(single_t);
extern single_t f80_atof(char *);
extern single_t ctof(char);
extern single_t dtof(double_t);
extern single_t etof(extend_t);
extern single_t itof(int);
extern single_t ltof(long);
extern single_t satof(char *, char **);
extern single_t stof(short);
extern bool_t   fcmpe(extend_t, extend_t, int);
extern bool_t   fcmp(double_t, double_t, int);
extern bool_t   fcmpf(single_t, single_t, int);
extern void     ftste(extend_t);
extern void     ftst(double_t);
extern void     ftstf(single_t);
extern extend_t fabse(extend_t);
extern extend_t facose(extend_t);
extern extend_t fadde(extend_t, extend_t);
extern extend_t fasine(extend_t);
extern extend_t fatn2e(extend_t, extend_t);
extern extend_t fatne(extend_t);
extern extend_t fcose(extend_t);
extern extend_t fcoshe(extend_t);
extern extend_t fdive(extend_t, extend_t);
extern extend_t fexpe(extend_t);
extern extend_t finte(extend_t);
extern extend_t flne(extend_t);
extern extend_t floge(extend_t);
extern extend_t fmule(extend_t, extend_t);
extern extend_t fnege(extend_t);
extern extend_t fpowe(extend_t, extend_t);
extern extend_t freme(extend_t, extend_t);
extern extend_t fsine(extend_t);
extern extend_t fsinhe(extend_t);
extern extend_t fsqrte(extend_t);
extern extend_t fsube(extend_t, extend_t);
extern extend_t ftne(extend_t);
extern extend_t ftnhe(extend_t);
extern extend_t fvale(int);
extern extend_t fatnhe(extend_t);
extern extend_t fmaxvale(sign_t);
extern extend_t fminvale(sign_t);
extern double_t f80_fabs(double_t);
extern double_t facos(double_t);
extern double_t fadd(double_t, double_t);
extern double_t fasin(double_t);
extern double_t fatn2(double_t, double_t);
extern double_t fatn(double_t);
extern double_t fcos(double_t);
extern double_t fcosh(double_t);
extern double_t fdiv(double_t, double_t);
extern double_t fexp(double_t);
extern double_t fint(double_t);
extern double_t fln(double_t);
extern double_t flog(double_t);
extern double_t fmul(double_t, double_t);
extern double_t fneg(double_t);
extern double_t fpow(double_t, double_t);
extern double_t frem(double_t, double_t);
extern double_t fsin(double_t);
extern double_t fsinh(double_t);
extern double_t fsqrt(double_t);
extern double_t fsub(double_t, double_t);
extern double_t ftn(double_t);
extern double_t ftnh(double_t);
extern double_t fval(int);
extern double_t fatnh(double_t);
extern double_t fmaxval(sign_t);
extern double_t fminval(sign_t);
extern single_t f80_fabsf(single_t);
extern single_t facosf(single_t);
extern single_t faddf(single_t, single_t);
extern single_t fasinf(single_t);
extern single_t fatn2f(single_t, single_t);
extern single_t fatnf(single_t);
extern single_t fcosf(single_t);
extern single_t fcoshf(single_t);
extern single_t fdivf(single_t, single_t);
extern single_t fexpf(single_t);
extern single_t fintf(single_t);
extern single_t flnf(single_t);
extern single_t flogf(single_t);
extern single_t fmulf(single_t, single_t);
extern single_t fnegf(single_t);
extern single_t fpowf(single_t, single_t);
extern single_t fremf(single_t, single_t);
extern single_t fsinf(single_t);
extern single_t fsinhf(single_t);
extern single_t fsqrtf(single_t);
extern single_t fsubf(single_t, single_t);
extern single_t ftnf(single_t);
extern single_t ftnhf(single_t);
extern single_t fvalf(int);
extern single_t fatnhf(single_t);
extern single_t fmaxvalf(sign_t);
extern single_t fminvalf(sign_t);
typedef struct mant_s   mant_t;
typedef mant_t*         mant_p;
typedef struct f80_s    f80_t;
typedef f80_t*          f80_p;
struct mant_s {                                  
        mp_t    p[((64  + (sizeof(mp_t)* 8 ) )/ (sizeof(mp_t)* 8 ) ) ];                  
};
struct f80_s {                                   
        sign_t  s;                               
        exp_t   e;                               
        mant_t  m;                               
};
         
         
extern  status_t        f80_status;
extern  status_t        f80_exception;           
extern  status_t        f80_aexception;          
extern  control_t       f80_econtrol;            
extern  control_t       f80_control;
extern bool_t   fbranch80(int);
extern bool_t   fcmp80(f80_t *, f80_t *, int);
extern char     *f80toa(char *,...);
extern char     f80toc(f80_t);
extern double_t f80tod(f80_t);
extern extend_t f80toe(f80_t);
extern f80_t    htof80(char *);
extern f80_t    shtof80(char *, char **);
extern f80_t    atof80(char *);
extern f80_t    satof80(char *, char **);
extern f80_t    dtof80(double_t);
extern f80_t    etof80(extend_t);
extern f80_t    ftof80(single_t);
extern f80_t    ctof80(char);
extern f80_t    itof80(int);
extern f80_t    ltof80(long);
extern f80_t    stof80(short);
extern f80_t    fabs80(f80_t);
extern f80_t    fadd80(f80_t, f80_t);
extern f80_t    facos80(f80_t);
extern f80_t    fasin80(f80_t);
extern f80_t    fatn280(f80_t, f80_t);
extern f80_t    fatn80(f80_t);
extern f80_t    fatnh80(f80_t);
extern f80_t    fcos80(f80_t);
extern f80_t    fcosh80(f80_t);
extern f80_t    fdenorm80(f80_t, int);
extern f80_t    fdiv80(f80_t, f80_t);
extern f80_t    fexp80(f80_t);
extern f80_t    fint80(f80_t);
extern f80_t    fln80(f80_t);
extern f80_t    flog80(f80_t);
extern f80_t    fmaxval80(descr_t *, sign_t);
extern f80_t    fmul80(f80_t, f80_t);
extern f80_t    fneg80(f80_t);
extern f80_t    fnorm80(f80_t);
extern f80_t    fpow80(f80_t, f80_t);
extern f80_t    *fpval80(int);
extern f80_t    frem80(f80_t, f80_t);
extern f80_t    frexp80(f80_t, int *);
extern f80_t    fminval80(descr_t *, sign_t);
extern f80_t    fsin80(f80_t);
extern f80_t    fsinh80(f80_t);
extern f80_t    fsqrt80(f80_t);
extern f80_t    fsub80(f80_t, f80_t);
extern f80_t    ftn80(f80_t);
extern f80_t    ftnh80(f80_t);
extern f80_t    fval80(int);
extern f80_t    ldexp80(f80_t, int);
extern f80_t    cftof80(descr_t *, convert_t *);
extern int      fset80(status_t);
extern int      f80toi(f80_t);
extern long     cvtf80(char *, f80_t);
extern long     f80tol(f80_t);
extern short    f80tos(f80_t);
extern single_t f80tof(f80_t);
extern void     ferror80(char *, int, f80_t *);
extern void     finit80(int);
extern void     ftst80(f80_t *);
extern void     cf80tof(char *, descr_t *, convert_t *, f80_t *);
extern packed_t f80top(f80_t);
extern void __eprintf (const char *, const char *, unsigned, const char *);
extern void poolsInit(struct _iobuf  *log);
extern void poolsFinish(void);
extern   void *cdpAlloc(size_t size);
extern void cdpFree(  void *old);
extern void   *cdpRealloc(  void *old, size_t size);
extern   void *ccpAlloc(size_t size);
extern void ccpFree(  void *old);
extern   void *ccpRealloc(  void *old, size_t size);
extern   char *ccpString(const char *string);
extern void poolsBacktrace(struct _iobuf  *stream);
void optionInit(struct _iobuf  *log);
void optionFinish(void);
typedef struct optionStruct *optionType;
extern optionType optionDefine(optionType options,  
                               const char *path,
                               const char *name,
                               const char *value);
extern optionType optionRefine(optionType options,
                               const char *name, const char *class_name);
extern size_t optionGet(optionType options, const char *name,
                        const char ***table);
extern size_t optionIsSet(optionType options, const char *name);
extern void optionBacktrace(struct _iobuf  *stream);
typedef struct engineStruct *engineType;
typedef const struct engineClassStruct *engineClassType;
typedef unsigned short  ShadowContext;
typedef unsigned short  ShadowTag;
typedef int (*engineStart)(  void *param, int position,
                           optionType options);
typedef engineType (* engineCallback)(void);
extern void engineInit(struct _iobuf  *log);
extern void engineFinish(void);
extern engineType engineSelf(void);
extern void *engineLocalSet(void *local);
extern void *engineLocalGet(void);
extern void *engineParam(engineType engine);
extern int enginePosition(engineType engine);
extern engineClassType engineClass(engineType engine);
extern   const char *engineName(engineType engine);
extern ShadowContext engineGetShadowContext(void);
extern void          engineSetShadowContext(ShadowContext);
extern   engineType engineCreate(  const char *name,
                                        const char *class_name,
                                      engineClassType class,
                                        void *param, int position);
extern   engineType engineCreateTop(  const char *name,
                                           const char *class_name,
                                         const engineClassType class,
                                           void *param, int position);
extern int engineFork(  engineType engine,
                      engineStart start,
                      optionType options);
extern engineCallback engineSetCallback(engineCallback work);
extern int engineExecInit(  engineType engine,
                          engineStart start,
                          optionType options);
extern engineType engineExec(engineType engine);
extern void engineWait(size_t nr_engines, engineType engines[]);
extern void (engineDestroy)(engineType engine);
extern void msgSend(engineType to,   void *msg);
extern   void *msgReceive(engineType *fromReturn);
extern void engineCancel(engineType engine);
extern void engineCancelHandler(void (*handler)(engineType by));
extern void engineKill(engineType engine);
extern void engineBacktrace(struct _iobuf  *stream);
typedef struct x * DMCP_xnode;
typedef void * DMCP_node;
extern void __eprintf (const char *, const char *, unsigned, const char *);
typedef enum domcode
{
        dom_none,
        xdom_mirOrigin,
        xdom_LABEL,
        xdom_OPRUN,
        xdom_mirVARIANCE,
        xdom_mirLINKAGE,
        xdom_mirCKF,
        xdom_UNIV_ADDRESS,
        xdom_NAME,
        xdom_REAL,
        xdom_BOOL,
        xdom_UNIV_REAL,
        xdom_UNIV_INT,
        xdom_INT,
        xdom_mirParamKIND,
        xdom_mirROUNDING,
        xdom_mirRELATION,
        MAX_OPAQUE,
        xdom_LIST_mirIntraCfg_FLIST,
        xdom_LIST_mirCFG_FLIST,
        xdom_LIST_Symbols_FLIST,
        xdom_LIST_mirTYPE_FLIST,
        xdom_LIST_mirSection_FLIST,
        xdom_LIST_mirDataGlobal_FLIST,
        xdom_LIST_mirProcGlobal_FLIST,
        xdom_LIST_mirPlaced_FLIST,
        xdom_LIST_mirRegister_FLIST,
        xdom_LIST_pagPair_FLIST,
        xdom_LIST_mirEDGE_FLIST,
        xdom_LIST_Const_FLIST,
        xdom_LIST_Const_d_FLIST,
        xdom_LIST_mirObject_FLIST,
        xdom_LIST_mirEXPR_FLIST,
        xdom_LIST_mirIterMask_FLIST,
        xdom_LIST_mirCase_FLIST,
        xdom_LIST_mirPROCESS_FLIST,
        xdom_LIST_mirALTERNATIVE_FLIST,
        xdom_LIST_mirField_FLIST,
        xdom_LIST_mirParameter_FLIST,
        xdom_LIST_mirSTMT_FLIST,
        xdom_LIST_mirBasicBlock_FLIST,
        xdom_LIST_mirLocal_FLIST,
        xdom_LIST_mirIntraCfg,
        xdom_LIST_mirCFG,
        xdom_LIST_Symbols,
        xdom_LIST_mirTYPE,
        xdom_LIST_mirSection,
        xdom_LIST_mirDataGlobal,
        xdom_LIST_mirProcGlobal,
        xdom_LIST_mirPlaced,
        xdom_LIST_mirRegister,
        xdom_LIST_pagPair,
        xdom_LIST_mirEDGE,
        xdom_LIST_Const,
        xdom_LIST_Const_d,
        xdom_LIST_mirObject,
        xdom_LIST_mirEXPR,
        xdom_LIST_mirIterMask,
        xdom_LIST_mirCase,
        xdom_LIST_mirPROCESS,
        xdom_LIST_mirALTERNATIVE,
        xdom_LIST_mirField,
        xdom_LIST_mirParameter,
        xdom_LIST_mirSTMT,
        xdom_LIST_mirBasicBlock,
        xdom_LIST_mirLocal,
        xdom_F_1,
        xdom_F_1_FLIST,
        MAX_FUNCTOR,
        xdom_allocsOP2Flds,
        xdom_allocsEXPRFlds,
        xdom_anc0ExtraFlds,
        xdom_anc0UnitFlds,
        xdom_anc0TYPEFlds,
        xdom_mirStandard,
        xdom_anc0SectionFlds,
        xdom_anc0STMTFlds,
        xdom_anc0GLOBALFlds,
        xdom_anc0ObjectFlds,
        xdom_anc0OP2Flds,
        xdom_anc0EXPRFlds,
        xdom_anc0CaseFlds,
        xdom_anc0BodyFlds,
        xdom_intervallOP2Flds,
        xdom_intervallEXPRFlds,
        xdom_intervallGLOBALFlds,
        xdom_intervallObjectFlds,
        xdom_intervallVector,
        xdom_gc_ipcp2OP2Flds,
        xdom_gc_ipcp2EXPRFlds,
        xdom_gc_ipcp2GLOBALFlds,
        xdom_gc_ipcp2ObjectFlds,
        xdom_gc_ipcp2Vector,
        xdom_cfOP2Flds,
        xdom_cfEXPRFlds,
        xdom_cfGLOBALFlds,
        xdom_cfObjectFlds,
        xdom_cfVector,
        xdom_conOP2Flds,
        xdom_conEXPRFlds,
        xdom_conGLOBALFlds,
        xdom_conObjectFlds,
        xdom_conVector,
        xdom_interOP2Flds,
        xdom_interEXPRFlds,
        xdom_interGLOBALFlds,
        xdom_interObjectFlds,
        xdom_interVector,
        xdom_Print,
        xdom_TarDes,
        xdom_Labelx,
        xdom_IR,
        xdom_mirRegularSection,
        xdom_mirBody,
        xdom_mirSTMTFlds,
        xdom_mirOP2Flds,
        xdom_mirEXPRFlds,
        xdom_mirGLOBALFlds,
        xdom_mirObjectFlds,
        xdom_mirFlds,
        xdom_Lift_Int,
        xdom_Triple,
        xdom_VALUE,
        xdom_mirTYPE,
        xdom_mirSection,
        xdom_mirEXPR,
        xdom_mirObject,
        xdom_mirProcGlobal,
        xdom_mirParameter,
        xdom_mirLocal,
        xdom_mirField,
        xdom_mirOP2,
        xdom_mirOP1,
        xdom_mirSimpleExpr,
        xdom_mirSTMT,
        xdom_mirSimpleSTMT,
        xdom_mirControlSTMT,
        xdom_mirGLOBAL,
        xdom_mirBasicBlock,
        xdom_mirDataGlobal,
        xdom_mirPlaced,
        xdom_mirRegister,
        xdom_mirOP2NoDiv,
        xdom_mirDIVISION,
        xdom_mirCase,
        xdom_mirALTERNATIVE,
        xdom_Symbols,
        xdom_mirPROCESS,
        xdom_mirIterMask,
        xdom_mirProc,
        xdom_mirIntraCfg,
        xdom_Const,
        xdom_mirCFG,
        xdom_mirEDGE,
        xdom_mySTMT,
        xdom_pagPair,
        xdom_Const_d,
        xdom_mirUnit,
        xdom_mirUNIT,
        xdom_D_11,
        MAX_DOMAIN
} domcode_t;
typedef enum opcode
{
        op_none,
        xop_Sort1_mirUnit = 1,
        xop_Sort10_Const = 2,
        xop_Sort10_Const_d = 3,
        xop_Sort10_ExternProc = 4,
        xop_Sort10_Sym = 5,
        xop_Sort10_mirAbs = 6,
        xop_Sort10_mirAcquire = 7,
        xop_Sort10_mirAddrConst = 8,
        xop_Sort10_mirAddrDiff = 9,
        xop_Sort10_mirAddrPlus = 10,
        xop_Sort10_mirAltAlways = 11,
        xop_Sort10_mirAltChannel = 12,
        xop_Sort10_mirAltGate = 13,
        xop_Sort10_mirAltTime = 14,
        xop_Sort10_mirAnd = 15,
        xop_Sort10_mirAnyType = 16,
        xop_Sort10_mirArray = 17,
        xop_Sort10_mirArrayAssign = 18,
        xop_Sort10_mirArrayCom = 19,
        xop_Sort10_mirAssign = 20,
        xop_Sort10_mirBasicBlock = 21,
        xop_Sort10_mirBasicBlockType = 22,
        xop_Sort10_mirBeginCritical = 23,
        xop_Sort10_mirBeginProcedure = 24,
        xop_Sort10_mirBeginProcess = 25,
        xop_Sort10_mirBeginRemote = 26,
        xop_Sort10_mirBitExtract = 27,
        xop_Sort10_mirBitInsert = 28,
        xop_Sort10_mirBoolConst = 29,
        xop_Sort10_mirBoolVal = 30,
        xop_Sort10_mirBoolean = 31,
        xop_Sort10_mirCFG = 32,
        xop_Sort10_mirCall = 33,
        xop_Sort10_mirCallRet = 34,
        xop_Sort10_mirCase = 35,
        xop_Sort10_mirCast = 36,
        xop_Sort10_mirCheckRange = 37,
        xop_Sort10_mirCheckReal = 38,
        xop_Sort10_mirComRepeat = 39,
        xop_Sort10_mirCompare = 40,
        xop_Sort10_mirContent = 41,
        xop_Sort10_mirConvert = 42,
        xop_Sort10_mirDataGlobal = 43,
        xop_Sort10_mirDiff = 44,
        xop_Sort10_mirDiv = 45,
        xop_Sort10_mirEDGE = 46,
        xop_Sort10_mirEndCall = 47,
        xop_Sort10_mirEndCritical = 48,
        xop_Sort10_mirEndFuncCall = 49,
        xop_Sort10_mirEndProcedure = 50,
        xop_Sort10_mirEndProcess = 51,
        xop_Sort10_mirEndRemote = 52,
        xop_Sort10_mirEndTailCall = 53,
        xop_Sort10_mirEvaluate = 54,
        xop_Sort10_mirExternProc = 55,
        xop_Sort10_mirField = 56,
        xop_Sort10_mirFuncCall = 57,
        xop_Sort10_mirGoto = 58,
        xop_Sort10_mirHandler = 59,
        xop_Sort10_mirHeapAllocate = 60,
        xop_Sort10_mirIf = 61,
        xop_Sort10_mirIntConst = 62,
        xop_Sort10_mirInteger = 63,
        xop_Sort10_mirIntraCfg = 64,
        xop_Sort10_mirIterator = 65,
        xop_Sort10_mirLocal = 66,
        xop_Sort10_mirMask = 67,
        xop_Sort10_mirMember = 68,
        xop_Sort10_mirMod = 69,
        xop_Sort10_mirMult = 70,
        xop_Sort10_mirNeg = 71,
        xop_Sort10_mirNoExpr = 72,
        xop_Sort10_mirNoType = 73,
        xop_Sort10_mirNot = 74,
        xop_Sort10_mirObjectAddr = 75,
        xop_Sort10_mirOr = 76,
        xop_Sort10_mirParallel = 77,
        xop_Sort10_mirParameter = 78,
        xop_Sort10_mirPlaced = 79,
        xop_Sort10_mirPlus = 80,
        xop_Sort10_mirPocf = 81,
        xop_Sort10_mirPointer = 82,
        xop_Sort10_mirPopHandler = 83,
        xop_Sort10_mirProcGlobal = 84,
        xop_Sort10_mirProcType = 85,
        xop_Sort10_mirProcess = 86,
        xop_Sort10_mirPushHandler = 87,
        xop_Sort10_mirQuo = 88,
        xop_Sort10_mirRaise = 89,
        xop_Sort10_mirReal = 90,
        xop_Sort10_mirRealConst = 91,
        xop_Sort10_mirReceive = 92,
        xop_Sort10_mirReceiveMem = 93,
        xop_Sort10_mirRecord = 94,
        xop_Sort10_mirRecordCom = 95,
        xop_Sort10_mirRegister = 96,
        xop_Sort10_mirRelease = 97,
        xop_Sort10_mirRem = 98,
        xop_Sort10_mirRemote = 99,
        xop_Sort10_mirReplicatedAlt = 100,
        xop_Sort10_mirReplicatedProcess = 101,
        xop_Sort10_mirReturn = 102,
        xop_Sort10_mirRight = 103,
        xop_Sort10_mirSection = 104,
        xop_Sort10_mirSend = 105,
        xop_Sort10_mirSendMem = 106,
        xop_Sort10_mirShiftLeft = 107,
        xop_Sort10_mirShiftRight = 108,
        xop_Sort10_mirShiftRightSign = 109,
        xop_Sort10_mirStackAllocate = 110,
        xop_Sort10_mirSubscript = 111,
        xop_Sort10_mirSwitch = 112,
        xop_Sort10_mirTailCall = 113,
        xop_Sort10_mirTryAcquire = 114,
        xop_Sort10_mirTypeSize = 115,
        xop_Sort10_mirUnion = 116,
        xop_Sort10_mirUnionCom = 117,
        xop_Sort10_mirWaitAndBranch = 118,
        xop_Sort10_mirXor = 119,
        xop_Sort10_pagPair = 120,
        xop_Sort2_IntConst = 121,
        xop_Sort2_RealConst = 122,
        xop_Sort3_Triple = 123,
        xop_Sort4_Bot = 124,
        xop_Sort4_Norm = 125,
        xop_Sort4_Top = 126,
        xop_Sort12_mirNoBody = 127,
        xop_Sort12_mirProcBody = 128,
        xop_Sort13_mirRegularSection = 129,
        xop_Sort14_IR = 130,
        xop_Sort16_TarDes = 131,
        xop_Sort56_mirStandard = 132,
        xop_Sort58_LIST_Const_List = 133,
        xop_Sort57_LIST_Const_FLIST_Flist = 134,
        MAX_OPER
} opcode_t;
extern char * domain_name (domcode_t);
extern char * opcode_name (opcode_t);
extern int      DMCP_is_opaque (domcode_t);
extern int      DMCP_is_functor (domcode_t);
extern int      DMCP_is_domain (domcode_t);
extern int      domcheck (DMCP_node, domcode_t);
extern int      isdom (DMCP_node, domcode_t);
typedef union x_Sort57 * LIST_mirIntraCfg_FLIST;
typedef union x_Sort57 * LIST_mirCFG_FLIST;
typedef union x_Sort57 * LIST_Symbols_FLIST;
typedef union x_Sort57 * LIST_mirTYPE_FLIST;
typedef union x_Sort57 * LIST_mirSection_FLIST;
typedef union x_Sort57 * LIST_mirDataGlobal_FLIST;
typedef union x_Sort57 * LIST_mirProcGlobal_FLIST;
typedef union x_Sort57 * LIST_mirPlaced_FLIST;
typedef union x_Sort57 * LIST_mirRegister_FLIST;
typedef union x_Sort57 * LIST_pagPair_FLIST;
typedef union x_Sort57 * LIST_mirEDGE_FLIST;
typedef union x_Sort57 * LIST_Const_FLIST;
typedef union x_Sort57 * LIST_Const_d_FLIST;
typedef union x_Sort57 * LIST_mirObject_FLIST;
typedef union x_Sort57 * LIST_mirEXPR_FLIST;
typedef union x_Sort57 * LIST_mirIterMask_FLIST;
typedef union x_Sort57 * LIST_mirCase_FLIST;
typedef union x_Sort57 * LIST_mirPROCESS_FLIST;
typedef union x_Sort57 * LIST_mirALTERNATIVE_FLIST;
typedef union x_Sort57 * LIST_mirField_FLIST;
typedef union x_Sort57 * LIST_mirParameter_FLIST;
typedef union x_Sort57 * LIST_mirSTMT_FLIST;
typedef union x_Sort57 * LIST_mirBasicBlock_FLIST;
typedef union x_Sort57 * LIST_mirLocal_FLIST;
typedef union x_Sort58 * LIST_mirIntraCfg;
typedef union x_Sort58 * LIST_mirCFG;
typedef union x_Sort58 * LIST_Symbols;
typedef union x_Sort58 * LIST_mirTYPE;
typedef union x_Sort58 * LIST_mirSection;
typedef union x_Sort58 * LIST_mirDataGlobal;
typedef union x_Sort58 * LIST_mirProcGlobal;
typedef union x_Sort58 * LIST_mirPlaced;
typedef union x_Sort58 * LIST_mirRegister;
typedef union x_Sort58 * LIST_pagPair;
typedef union x_Sort58 * LIST_mirEDGE;
typedef union x_Sort58 * LIST_Const;
typedef union x_Sort58 * LIST_Const_d;
typedef union x_Sort58 * LIST_mirObject;
typedef union x_Sort58 * LIST_mirEXPR;
typedef union x_Sort58 * LIST_mirIterMask;
typedef union x_Sort58 * LIST_mirCase;
typedef union x_Sort58 * LIST_mirPROCESS;
typedef union x_Sort58 * LIST_mirALTERNATIVE;
typedef union x_Sort58 * LIST_mirField;
typedef union x_Sort58 * LIST_mirParameter;
typedef union x_Sort58 * LIST_mirSTMT;
typedef union x_Sort58 * LIST_mirBasicBlock;
typedef union x_Sort58 * LIST_mirLocal;
typedef union x_Sort56 * mirStandard;
typedef union x_Sort16 * TarDes;
typedef union x_Sort14 * IR;
typedef union x_Sort13 * mirRegularSection;
typedef union x_Sort12 * mirBody;
typedef union x_Sort4 * Lift_Int;
typedef union x_Sort3 * Triple;
typedef union x_Sort2 * VALUE;
typedef union x_Sort10 * mirTYPE;
typedef union x_Sort10 * mirSection;
typedef union x_Sort10 * mirEXPR;
typedef union x_Sort10 * mirObject;
typedef union x_Sort10 * mirProcGlobal;
typedef union x_Sort10 * mirParameter;
typedef union x_Sort10 * mirLocal;
typedef union x_Sort10 * mirField;
typedef union x_Sort10 * mirOP2;
typedef union x_Sort10 * mirOP1;
typedef union x_Sort10 * mirSimpleExpr;
typedef union x_Sort10 * mirSTMT;
typedef union x_Sort10 * mirSimpleSTMT;
typedef union x_Sort10 * mirControlSTMT;
typedef union x_Sort10 * mirGLOBAL;
typedef union x_Sort10 * mirBasicBlock;
typedef union x_Sort10 * mirDataGlobal;
typedef union x_Sort10 * mirPlaced;
typedef union x_Sort10 * mirRegister;
typedef union x_Sort10 * mirOP2NoDiv;
typedef union x_Sort10 * mirDIVISION;
typedef union x_Sort10 * mirCase;
typedef union x_Sort10 * mirALTERNATIVE;
typedef union x_Sort10 * Symbols;
typedef union x_Sort10 * mirPROCESS;
typedef union x_Sort10 * mirIterMask;
typedef union x_Sort10 * mirProc;
typedef union x_Sort10 * mirIntraCfg;
typedef union x_Sort10 * Const;
typedef union x_Sort10 * mirCFG;
typedef union x_Sort10 * mirEDGE;
typedef union x_Sort10 * mySTMT;
typedef union x_Sort10 * pagPair;
typedef union x_Sort10 * Const_d;
typedef union x_Sort1 * mirUnit;
typedef union x_Sort1 * mirUNIT;
typedef union x_Sort10 * D_11;
typedef union x_Sort58 * F_1;
typedef union x_Sort57 * F_1_FLIST;
typedef DMCP_node allocsOP2Flds;
typedef DMCP_node allocsEXPRFlds;
typedef DMCP_node anc0ExtraFlds;
typedef DMCP_node anc0UnitFlds;
typedef DMCP_node anc0TYPEFlds;
typedef DMCP_node anc0SectionFlds;
typedef DMCP_node anc0STMTFlds;
typedef DMCP_node anc0GLOBALFlds;
typedef DMCP_node anc0ObjectFlds;
typedef DMCP_node anc0OP2Flds;
typedef DMCP_node anc0EXPRFlds;
typedef DMCP_node anc0CaseFlds;
typedef DMCP_node anc0BodyFlds;
typedef DMCP_node intervallOP2Flds;
typedef DMCP_node intervallEXPRFlds;
typedef DMCP_node intervallGLOBALFlds;
typedef DMCP_node intervallObjectFlds;
typedef DMCP_node intervallVector;
typedef DMCP_node gc_ipcp2OP2Flds;
typedef DMCP_node gc_ipcp2EXPRFlds;
typedef DMCP_node gc_ipcp2GLOBALFlds;
typedef DMCP_node gc_ipcp2ObjectFlds;
typedef DMCP_node gc_ipcp2Vector;
typedef DMCP_node cfOP2Flds;
typedef DMCP_node cfEXPRFlds;
typedef DMCP_node cfGLOBALFlds;
typedef DMCP_node cfObjectFlds;
typedef DMCP_node cfVector;
typedef DMCP_node conOP2Flds;
typedef DMCP_node conEXPRFlds;
typedef DMCP_node conGLOBALFlds;
typedef DMCP_node conObjectFlds;
typedef DMCP_node conVector;
typedef DMCP_node interOP2Flds;
typedef DMCP_node interEXPRFlds;
typedef DMCP_node interGLOBALFlds;
typedef DMCP_node interObjectFlds;
typedef DMCP_node interVector;
typedef DMCP_node Print;
typedef DMCP_node Labelx;
typedef DMCP_node mirSTMTFlds;
typedef DMCP_node mirOP2Flds;
typedef DMCP_node mirEXPRFlds;
typedef DMCP_node mirGLOBALFlds;
typedef DMCP_node mirObjectFlds;
typedef DMCP_node mirFlds;
struct x { opcode_t op; };
extern DMCP_node x_create(opcode_t);
extern DMCP_node x_copy(DMCP_node);
        
        
typedef struct
{
        const char      * File;
        int             Line;
}
        orig_t; 
        typedef orig_t  mirOrigin;
        
        extern mirOrigin mirOrigin_dummy (void);
        extern void out_mirOrigin (struct _iobuf  *, mirOrigin);
        
        
typedef char    * rom_t;
typedef struct str      str_t, * str_p;
struct str
{
        str_p   less, more;
        char    s[1];    
};
extern rom_t    rom (char *);
extern void     prrom (struct _iobuf  *);
        typedef rom_t LABEL;
        
        LABEL LABEL_dummy(void);
        extern void out_LABEL(struct _iobuf  *, LABEL);
         
        
        typedef
            enum {
                Constant,
                Invariant,
                Variant
            } mirVARIANCE;
        mirVARIANCE mirVARIANCE_dummy(void);
        void    out_mirVARIANCE (struct _iobuf  * f, mirVARIANCE p);
        typedef
            enum {
                StaticLinkage,
                ExportLinkage,
                ImportLinkage,
                CommonLinkage
            } mirLINKAGE;
        mirLINKAGE mirLINKAGE_dummy(void);
        void    out_mirLINKAGE (struct _iobuf  * f, mirLINKAGE p);
        
        typedef int mirCKF; 
        
        mirCKF mirCKF_dummy(void);
        extern void out_mirCKF(struct _iobuf  *, mirCKF);
        
        typedef x64_t UNIV_ADDRESS; 
        typedef x64_t UnivAddr; 
        
        
        UNIV_ADDRESS UNIV_ADDRESS_dummy(void);
        UnivAddr UnivAddr_dummy(void);
        extern void out_UNIV_ADDRESS(struct _iobuf  *, UNIV_ADDRESS);
        extern void out_UnivAddr(struct _iobuf  *, UnivAddr);
        typedef char* NAME; 
        
        NAME NAME_dummy(void);
        extern void out_NAME(struct _iobuf  *, NAME);
        typedef float REAL; 
        
        REAL REAL_dummy(void);
        extern void out_REAL(struct _iobuf  *, REAL);
        
        typedef bool_t BOOL; 
        
        BOOL BOOL_dummy(void);
        extern void out_BOOL(struct _iobuf  *, BOOL);
        
        typedef f80_t UNIV_REAL; 
        typedef f80_t UnivReal; 
        
        
        UNIV_REAL UNIV_REAL_dummy(void); 
        UnivReal UnivReal_dummy(void); 
        extern void out_UNIV_REAL(struct _iobuf  *, UNIV_REAL);
        extern void out_UnivReal(struct _iobuf  *, UnivReal);
        
        typedef x64_t UNIV_INT; 
        typedef x64_t UnivInt; 
        
        
        UNIV_INT UNIV_INT_dummy(void);
        UnivInt UnivInt_dummy(void);
        extern void out_UNIV_INT(struct _iobuf  *, UNIV_INT);
        extern void out_UnivInt(struct _iobuf  *, UnivInt);
        
        typedef int INT; 
        
        INT INT_dummy(void);
        extern void out_INT(struct _iobuf  *, INT);
        typedef
            enum {
                ByValue,
                ByReference,
                ByCopyInOut
            } mirParamKIND;
        mirParamKIND mirParamKIND_dummy(void);
        void    out_mirParamKIND (struct _iobuf  * f, mirParamKIND p);
        typedef
            enum {
                Truncation,
                Nearest,
                Floor,
                Ceiling
            } mirROUNDING ;
        mirROUNDING mirROUNDING_dummy(void);
        void    out_mirROUNDING (struct _iobuf  * f, mirROUNDING p);
        typedef
            enum {
                Less,
                LessEqual,
                Greater,
                GreaterEqual,
                Equal,
                UnEqual
            } mirRELATION;
        mirRELATION mirRELATION_dummy(void);
        void    out_mirRELATION (struct _iobuf  * f, mirRELATION p);
typedef struct oprun    oprun_t, * oprun_p, * OPRUN;
struct oprun
{
        short   optims;          
        short   iterno;          
        char    ** cause;        
        char    * state;         
        char    * name[1];       
};
extern int      get_optno (oprun_p, char *);
extern BOOL     should_run (oprun_p, char *);
extern int      has_run (oprun_p, char *, BOOL);
extern void     has_skipped (oprun_p, char *);
extern void     out_OPRUN (struct _iobuf  *, OPRUN);
typedef const size_t * DMCP_fields_t;
struct DMCP_fields
{
        size_t id;
        const char *     name;
        domcode_t        type;
        size_t   offset;
        size_t   shadowsize;
        size_t   shadowtag;
};
struct DMCP_opers
{
        const char *     name;
        struct DMCP_doms *       domains;
        DMCP_fields_t    fields;
        size_t size;
};
struct DMCP_doms
{
        domcode_t        domain;
        DMCP_fields_t    readable;
        DMCP_fields_t    writable;
};
extern const char *      DMCP_domains_table[MAX_DOMAIN];
extern const struct DMCP_opers   DMCP_opers_table[MAX_OPER];
extern const struct DMCP_fields  DMCP_fields_table[249 ];
extern DMCP_fields_t DMCP_get_fields (domcode_t, DMCP_node);
extern DMCP_fields_t DMCP_set_fields (domcode_t, DMCP_node);
extern void DMCP_set (DMCP_fields_t, DMCP_node, DMCP_node);
void DMCP_set_mirOrigin(DMCP_fields_t, DMCP_node, mirOrigin);
void DMCP_set_LABEL(DMCP_fields_t, DMCP_node, LABEL);
void DMCP_set_OPRUN(DMCP_fields_t, DMCP_node, OPRUN);
void DMCP_set_mirVARIANCE(DMCP_fields_t, DMCP_node, mirVARIANCE);
void DMCP_set_mirLINKAGE(DMCP_fields_t, DMCP_node, mirLINKAGE);
void DMCP_set_mirCKF(DMCP_fields_t, DMCP_node, mirCKF);
void DMCP_set_UNIV_ADDRESS(DMCP_fields_t, DMCP_node, UNIV_ADDRESS);
void DMCP_set_NAME(DMCP_fields_t, DMCP_node, NAME);
void DMCP_set_REAL(DMCP_fields_t, DMCP_node, REAL);
void DMCP_set_BOOL(DMCP_fields_t, DMCP_node, BOOL);
void DMCP_set_UNIV_REAL(DMCP_fields_t, DMCP_node, UNIV_REAL);
void DMCP_set_UNIV_INT(DMCP_fields_t, DMCP_node, UNIV_INT);
void DMCP_set_INT(DMCP_fields_t, DMCP_node, INT);
void DMCP_set_mirParamKIND(DMCP_fields_t, DMCP_node, mirParamKIND);
void DMCP_set_mirROUNDING(DMCP_fields_t, DMCP_node, mirROUNDING);
void DMCP_set_mirRELATION(DMCP_fields_t, DMCP_node, mirRELATION);
union x_Sort1
{
         
        struct x_Sort1_mirUnit
        {
        opcode_t        op;
        LIST_mirTYPE    f_Types;
        mirStandard     f_Standard;
        LIST_mirSection f_Sections;
        LIST_mirRegister        f_Registers;
        LIST_mirProcGlobal      f_Procedures;
        LIST_mirPlaced  f_Placed;
        LIST_mirDataGlobal      f_Globals;
        OPRUN   f_OpRun;
        mirOrigin       f_Origin;
        INT     f_Printed;
        LABEL   f_Label;
        LIST_Symbols    f_symbols;
        INT     f_cp_number;
        mirIntraCfg     f_cp_main;
        LIST_mirCFG     f_cp_all;
        LIST_mirIntraCfg        f_CFGS;
        BOOL    f_Init;
        mirCFG  f_Node;
        INT     f_Visualized;
        INT     f_Number;
        } o_mirUnit;
};
union x_Sort10
{
         
        struct x_Sort10_Const
        {
        opcode_t        op;
        mirObject       f_obj;
        INT     f_num;
        VALUE   f_val;
        } o_Const;
        struct x_Sort10_Const_d
        {
        opcode_t        op;
        mirObject       f_obj;
        INT     f_num;
        Triple  f_val;
        } o_Const_d;
        struct x_Sort10_ExternProc
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_0;  
        INT     coco_1;  
        mirEXPR coco_2;  
        BOOL    coco_3;  
        BOOL    coco_4;  
        INT     coco_5;  
        UNIV_INT        coco_6;  
        BOOL    coco_7;  
        BOOL    coco_8;  
        REAL    f_UseEstimate;
        BOOL    f_AddressKnown;
        BOOL    f_IsTempo;
        BOOL    f_IsWriteOnce;
        INT     f_cpid;
        BOOL    f_IsVolatile;
        INT     f_RegisterCandidacy;
        NAME    f_Name;
        mirLINKAGE      f_Linkage;
        mirSection      f_Section;
        LIST_mirParameter       f_Params;
        mirBody f_Body;
        mirCKF  f_Predef;
        } o_ExternProc;
        struct x_Sort10_Sym
        {
        opcode_t        op;
        mirObject       f_obj;
        INT     f_num;
        } o_Sym;
        struct x_Sort10_mirAbs
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_9;  
        INT     coco_10;         
        mirCFG  coco_11;         
        INT     coco_12;         
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        INT     coco_13;         
        UNIV_INT        coco_14;         
        BOOL    coco_15;         
        BOOL    coco_16;         
        mirEXPR f_Value;
        } o_mirAbs;
        struct x_Sort10_mirAcquire
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_17;         
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_18;         
        INT     coco_19;         
        mirEXPR coco_20;         
        BOOL    coco_21;         
        BOOL    coco_22;         
        INT     coco_23;         
        UNIV_INT        coco_24;         
        BOOL    coco_25;         
        BOOL    coco_26;         
        mirEXPR coco_27;         
        mirEXPR coco_28;         
        mirEXPR coco_29;         
        mirEXPR f_Gate;
        mirEXPR coco_30;         
        mirEXPR f_Count;
        } o_mirAcquire;
        struct x_Sort10_mirAddrConst
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_31;         
        INT     coco_32;         
        mirCFG  coco_33;         
        INT     coco_34;         
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        UNIV_ADDRESS    f_Value;
        } o_mirAddrConst;
        struct x_Sort10_mirAddrDiff
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_35;         
        INT     coco_36;         
        mirCFG  coco_37;         
        INT     coco_38;         
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        } o_mirAddrDiff;
        struct x_Sort10_mirAddrPlus
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_39;         
        INT     coco_40;         
        mirCFG  coco_41;         
        INT     coco_42;         
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        } o_mirAddrPlus;
        struct x_Sort10_mirAltAlways
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_43;         
        BOOL    coco_44;         
        INT     coco_45;         
        mirCFG  coco_46;         
        INT     coco_47;         
        mirVARIANCE     coco_48;         
        INT     coco_49;         
        mirEXPR coco_50;         
        BOOL    coco_51;         
        BOOL    coco_52;         
        INT     coco_53;         
        UNIV_INT        coco_54;         
        BOOL    coco_55;         
        BOOL    coco_56;         
        mirEXPR coco_57;         
        mirEXPR f_Target;
        mirEXPR coco_58;         
        mirEXPR coco_59;         
        mirEXPR f_Mask;
        } o_mirAltAlways;
        struct x_Sort10_mirAltChannel
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_60;         
        BOOL    coco_61;         
        INT     coco_62;         
        mirCFG  coco_63;         
        INT     coco_64;         
        mirVARIANCE     coco_65;         
        INT     coco_66;         
        mirEXPR coco_67;         
        BOOL    coco_68;         
        BOOL    coco_69;         
        INT     coco_70;         
        UNIV_INT        coco_71;         
        BOOL    coco_72;         
        BOOL    coco_73;         
        mirEXPR coco_74;         
        mirEXPR f_Target;
        mirEXPR f_Channel;
        mirEXPR coco_75;         
        mirEXPR f_Mask;
        } o_mirAltChannel;
        struct x_Sort10_mirAltGate
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_76;         
        BOOL    coco_77;         
        INT     coco_78;         
        mirCFG  coco_79;         
        INT     coco_80;         
        mirVARIANCE     coco_81;         
        INT     coco_82;         
        mirEXPR coco_83;         
        BOOL    coco_84;         
        BOOL    coco_85;         
        INT     coco_86;         
        UNIV_INT        coco_87;         
        BOOL    coco_88;         
        BOOL    coco_89;         
        mirEXPR coco_90;         
        mirEXPR f_Target;
        mirEXPR coco_91;         
        mirEXPR f_Gate;
        mirEXPR f_Mask;
        } o_mirAltGate;
        struct x_Sort10_mirAltTime
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_92;         
        BOOL    coco_93;         
        INT     coco_94;         
        mirCFG  coco_95;         
        INT     coco_96;         
        mirVARIANCE     coco_97;         
        INT     coco_98;         
        mirEXPR coco_99;         
        BOOL    coco_100;        
        BOOL    coco_101;        
        INT     coco_102;        
        UNIV_INT        coco_103;        
        BOOL    coco_104;        
        BOOL    coco_105;        
        mirEXPR coco_106;        
        mirEXPR f_Target;
        mirEXPR coco_107;        
        mirEXPR coco_108;        
        mirEXPR f_Mask;
        mirEXPR f_Time;
        } o_mirAltTime;
        struct x_Sort10_mirAnd
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_109;        
        INT     coco_110;        
        mirCFG  coco_111;        
        INT     coco_112;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        } o_mirAnd;
        struct x_Sort10_mirAnyType
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_113;        
        BOOL    coco_114;        
        INT     coco_115;        
        mirCFG  coco_116;        
        INT     coco_117;        
        mirVARIANCE     coco_118;        
        INT     coco_119;        
        mirEXPR coco_120;        
        BOOL    coco_121;        
        BOOL    coco_122;        
        INT     f_Alignment;
        UNIV_INT        f_Size;
        BOOL    f_AlignmentKnown;
        BOOL    f_SizeKnown;
        } o_mirAnyType;
        struct x_Sort10_mirArray
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_123;        
        BOOL    coco_124;        
        INT     coco_125;        
        mirCFG  coco_126;        
        INT     coco_127;        
        mirVARIANCE     coco_128;        
        INT     coco_129;        
        mirEXPR coco_130;        
        BOOL    coco_131;        
        BOOL    coco_132;        
        INT     f_Alignment;
        UNIV_INT        f_Size;
        BOOL    f_AlignmentKnown;
        BOOL    f_SizeKnown;
        mirEXPR coco_133;        
        mirEXPR coco_134;        
        mirEXPR f_Upb;
        mirEXPR f_Lwb;
        mirEXPR f_ElemSize;
        mirTYPE f_ElemType;
        } o_mirArray;
        struct x_Sort10_mirArrayAssign
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_135;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirEXPR f_Lhs;
        mirEXPR f_Rhs;
        LIST_mirIterMask        f_Ispace;
        } o_mirArrayAssign;
        struct x_Sort10_mirArrayCom
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_136;        
        INT     coco_137;        
        mirCFG  coco_138;        
        INT     coco_139;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        LIST_mirEXPR    f_Value;
        } o_mirArrayCom;
        struct x_Sort10_mirAssign
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_140;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirEXPR f_Lhs;
        mirEXPR f_Rhs;
        } o_mirAssign;
        struct x_Sort10_mirBasicBlock
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_141;        
        INT     coco_142;        
        mirEXPR coco_143;        
        BOOL    coco_144;        
        BOOL    coco_145;        
        INT     coco_146;        
        UNIV_INT        coco_147;        
        BOOL    coco_148;        
        BOOL    coco_149;        
        REAL    f_UseEstimate;
        BOOL    f_AddressKnown;
        BOOL    f_IsTempo;
        BOOL    f_IsWriteOnce;
        INT     f_cpid;
        BOOL    f_IsVolatile;
        INT     f_RegisterCandidacy;
        NAME    f_Name;
        mirLINKAGE      f_Linkage;
        mirSection      f_Section;
        INT     f_Iref;
        INT     f_Dref;
        BOOL    f_Jump;
        BOOL    f_Empty;
        LIST_mirBasicBlock      f_Preds;
        LIST_mirBasicBlock      f_Succs;
        BOOL    f_Exit;
        BOOL    f_Entry;
        BOOL    f_Reachable;
        LIST_mirSTMT    f_Stmts;
        } o_mirBasicBlock;
        struct x_Sort10_mirBasicBlockType
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_150;        
        BOOL    coco_151;        
        INT     coco_152;        
        mirCFG  coco_153;        
        INT     coco_154;        
        mirVARIANCE     coco_155;        
        INT     coco_156;        
        mirEXPR coco_157;        
        BOOL    coco_158;        
        BOOL    coco_159;        
        INT     f_Alignment;
        UNIV_INT        f_Size;
        BOOL    f_AlignmentKnown;
        BOOL    f_SizeKnown;
        } o_mirBasicBlockType;
        struct x_Sort10_mirBeginCritical
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_160;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        } o_mirBeginCritical;
        struct x_Sort10_mirBeginProcedure
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_161;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_162;        
        INT     coco_163;        
        mirEXPR coco_164;        
        BOOL    coco_165;        
        BOOL    coco_166;        
        INT     coco_167;        
        UNIV_INT        coco_168;        
        BOOL    coco_169;        
        BOOL    coco_170;        
        mirEXPR coco_171;        
        mirEXPR coco_172;        
        mirEXPR coco_173;        
        mirEXPR coco_174;        
        INT     coco_175;        
        LIST_mirObject  f_Params;
        LIST_mirObject  f_Locals;
        } o_mirBeginProcedure;
        struct x_Sort10_mirBeginProcess
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_176;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        } o_mirBeginProcess;
        struct x_Sort10_mirBeginRemote
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_177;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        } o_mirBeginRemote;
        struct x_Sort10_mirBitExtract
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_178;        
        INT     coco_179;        
        mirCFG  coco_180;        
        INT     coco_181;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Src;
        mirEXPR f_Width;
        mirEXPR f_Start;
        } o_mirBitExtract;
        struct x_Sort10_mirBitInsert
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_182;        
        INT     coco_183;        
        mirCFG  coco_184;        
        INT     coco_185;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Src;
        mirEXPR f_Width;
        mirEXPR f_Start;
        mirEXPR f_Dest;
        } o_mirBitInsert;
        struct x_Sort10_mirBoolConst
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_186;        
        INT     coco_187;        
        mirCFG  coco_188;        
        INT     coco_189;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        BOOL    f_Value;
        } o_mirBoolConst;
        struct x_Sort10_mirBoolVal
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_190;        
        INT     coco_191;        
        mirCFG  coco_192;        
        INT     coco_193;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        INT     coco_194;        
        UNIV_INT        coco_195;        
        BOOL    coco_196;        
        BOOL    coco_197;        
        mirEXPR f_Value;
        UNIV_INT        f_False;
        UNIV_INT        f_True;
        } o_mirBoolVal;
        struct x_Sort10_mirBoolean
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_198;        
        BOOL    coco_199;        
        INT     coco_200;        
        mirCFG  coco_201;        
        INT     coco_202;        
        mirVARIANCE     coco_203;        
        INT     coco_204;        
        mirEXPR coco_205;        
        BOOL    coco_206;        
        BOOL    coco_207;        
        INT     f_Alignment;
        UNIV_INT        f_Size;
        BOOL    f_AlignmentKnown;
        BOOL    f_SizeKnown;
        } o_mirBoolean;
        struct x_Sort10_mirCFG
        {
        opcode_t        op;
        INT     f_Type;
        INT     f_cycle;
        LIST_mirCFG     f_DirSuccs;
        LIST_mirCFG     f_DirPreds;
        INT     f_Visited;
        INT     f_dfsnum;
        mirSTMT f_Stat;
        INT     f_Unique;
        LIST_Const_d    f_consttab_d;
        LIST_Const      f_consttab2;
        LIST_Const      f_consttab;
        INT     f_CallNum;
        INT     f_Arrity;
        INT     f_there;
        LIST_mirEDGE    f_Succs;
        LIST_mirEDGE    f_Preds;
        } o_mirCFG;
        struct x_Sort10_mirCall
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_208;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_209;        
        INT     coco_210;        
        mirEXPR coco_211;        
        BOOL    coco_212;        
        BOOL    coco_213;        
        INT     coco_214;        
        UNIV_INT        coco_215;        
        BOOL    coco_216;        
        BOOL    coco_217;        
        mirEXPR coco_218;        
        mirEXPR coco_219;        
        mirEXPR coco_220;        
        mirEXPR coco_221;        
        mirEXPR f_Proc;
        LIST_mirEXPR    f_Params;
        } o_mirCall;
        struct x_Sort10_mirCallRet
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_222;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_223;        
        INT     coco_224;        
        mirEXPR coco_225;        
        BOOL    coco_226;        
        BOOL    coco_227;        
        INT     coco_228;        
        UNIV_INT        coco_229;        
        BOOL    coco_230;        
        BOOL    coco_231;        
        mirEXPR coco_232;        
        mirEXPR coco_233;        
        mirEXPR coco_234;        
        mirEXPR coco_235;        
        INT     f_caller;
        LIST_mirObject  f_Params;
        LIST_mirObject  f_Locals;
        } o_mirCallRet;
        struct x_Sort10_mirCase
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_236;        
        BOOL    coco_237;        
        INT     coco_238;        
        mirCFG  coco_239;        
        INT     coco_240;        
        mirVARIANCE     coco_241;        
        INT     coco_242;        
        mirEXPR coco_243;        
        BOOL    coco_244;        
        BOOL    coco_245;        
        INT     coco_246;        
        UNIV_INT        coco_247;        
        BOOL    coco_248;        
        BOOL    coco_249;        
        mirEXPR coco_250;        
        mirEXPR f_Target;
        mirEXPR f_Upb;
        mirEXPR f_Lwb;
        } o_mirCase;
        struct x_Sort10_mirCast
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_251;        
        INT     coco_252;        
        mirCFG  coco_253;        
        INT     coco_254;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        INT     coco_255;        
        UNIV_INT        coco_256;        
        BOOL    coco_257;        
        BOOL    coco_258;        
        mirEXPR f_Value;
        } o_mirCast;
        struct x_Sort10_mirCheckRange
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_259;        
        INT     coco_260;        
        mirCFG  coco_261;        
        INT     coco_262;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        INT     coco_263;        
        UNIV_INT        coco_264;        
        BOOL    coco_265;        
        BOOL    coco_266;        
        mirEXPR f_Value;
        } o_mirCheckRange;
        struct x_Sort10_mirCheckReal
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_267;        
        INT     coco_268;        
        mirCFG  coco_269;        
        INT     coco_270;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        INT     coco_271;        
        UNIV_INT        coco_272;        
        BOOL    coco_273;        
        BOOL    coco_274;        
        mirEXPR f_Value;
        } o_mirCheckReal;
        struct x_Sort10_mirComRepeat
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_275;        
        INT     coco_276;        
        mirCFG  coco_277;        
        INT     coco_278;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        INT     coco_279;        
        UNIV_INT        coco_280;        
        BOOL    coco_281;        
        BOOL    coco_282;        
        mirEXPR f_Value;
        INT     f_Times;
        } o_mirComRepeat;
        struct x_Sort10_mirCompare
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_283;        
        INT     coco_284;        
        mirCFG  coco_285;        
        INT     coco_286;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        mirRELATION     f_Relation;
        } o_mirCompare;
        struct x_Sort10_mirContent
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_287;        
        INT     coco_288;        
        mirCFG  coco_289;        
        INT     coco_290;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        INT     coco_291;        
        UNIV_INT        coco_292;        
        BOOL    coco_293;        
        BOOL    coco_294;        
        mirEXPR coco_295;        
        mirEXPR coco_296;        
        mirEXPR coco_297;        
        mirEXPR coco_298;        
        mirEXPR coco_299;        
        mirEXPR f_Addr;
        } o_mirContent;
        struct x_Sort10_mirConvert
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_300;        
        INT     coco_301;        
        mirCFG  coco_302;        
        INT     coco_303;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        INT     coco_304;        
        UNIV_INT        coco_305;        
        BOOL    coco_306;        
        BOOL    coco_307;        
        mirEXPR f_Value;
        mirROUNDING     f_Rounding;
        } o_mirConvert;
        struct x_Sort10_mirDataGlobal
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_308;        
        INT     coco_309;        
        mirEXPR coco_310;        
        BOOL    coco_311;        
        BOOL    coco_312;        
        INT     coco_313;        
        UNIV_INT        coco_314;        
        BOOL    coco_315;        
        BOOL    coco_316;        
        REAL    f_UseEstimate;
        BOOL    f_AddressKnown;
        BOOL    f_IsTempo;
        BOOL    f_IsWriteOnce;
        INT     f_cpid;
        BOOL    f_IsVolatile;
        INT     f_RegisterCandidacy;
        NAME    f_Name;
        mirLINKAGE      f_Linkage;
        mirSection      f_Section;
        mirEXPR f_InitialValue;
        } o_mirDataGlobal;
        struct x_Sort10_mirDiff
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_317;        
        INT     coco_318;        
        mirCFG  coco_319;        
        INT     coco_320;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        } o_mirDiff;
        struct x_Sort10_mirDiv
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_321;        
        INT     coco_322;        
        mirCFG  coco_323;        
        INT     coco_324;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        mirEXPR f_OnZero;
        } o_mirDiv;
        struct x_Sort10_mirEDGE
        {
        opcode_t        op;
        INT     f_Type;
        mirCFG  f_Target;
        mirCFG  f_Source;
        } o_mirEDGE;
        struct x_Sort10_mirEndCall
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_325;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_326;        
        INT     coco_327;        
        mirEXPR coco_328;        
        BOOL    coco_329;        
        BOOL    coco_330;        
        INT     coco_331;        
        UNIV_INT        coco_332;        
        BOOL    coco_333;        
        BOOL    coco_334;        
        mirEXPR coco_335;        
        mirEXPR coco_336;        
        mirEXPR coco_337;        
        mirEXPR coco_338;        
        INT     f_caller;
        } o_mirEndCall;
        struct x_Sort10_mirEndCritical
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_339;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_340;        
        INT     coco_341;        
        mirEXPR coco_342;        
        BOOL    coco_343;        
        BOOL    coco_344;        
        INT     coco_345;        
        UNIV_INT        coco_346;        
        BOOL    coco_347;        
        BOOL    coco_348;        
        mirEXPR coco_349;        
        mirEXPR coco_350;        
        mirEXPR coco_351;        
        mirEXPR f_Gate;
        mirEXPR f_Next;
        } o_mirEndCritical;
        struct x_Sort10_mirEndFuncCall
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_352;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_353;        
        INT     coco_354;        
        mirEXPR coco_355;        
        BOOL    coco_356;        
        BOOL    coco_357;        
        INT     coco_358;        
        UNIV_INT        coco_359;        
        BOOL    coco_360;        
        BOOL    coco_361;        
        mirEXPR coco_362;        
        mirEXPR coco_363;        
        mirEXPR coco_364;        
        mirEXPR f_Res;
        INT     f_caller;
        } o_mirEndFuncCall;
        struct x_Sort10_mirEndProcedure
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_365;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_366;        
        INT     coco_367;        
        mirEXPR coco_368;        
        BOOL    coco_369;        
        BOOL    coco_370;        
        INT     coco_371;        
        UNIV_INT        coco_372;        
        BOOL    coco_373;        
        BOOL    coco_374;        
        mirEXPR coco_375;        
        mirEXPR coco_376;        
        mirEXPR coco_377;        
        mirEXPR coco_378;        
        INT     coco_379;        
        LIST_mirObject  f_Params;
        LIST_mirObject  f_Locals;
        } o_mirEndProcedure;
        struct x_Sort10_mirEndProcess
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_380;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        } o_mirEndProcess;
        struct x_Sort10_mirEndRemote
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_381;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_382;        
        INT     coco_383;        
        mirEXPR coco_384;        
        BOOL    coco_385;        
        BOOL    coco_386;        
        INT     coco_387;        
        UNIV_INT        coco_388;        
        BOOL    coco_389;        
        BOOL    coco_390;        
        mirEXPR coco_391;        
        mirEXPR f_Target;
        } o_mirEndRemote;
        struct x_Sort10_mirEndTailCall
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_392;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_393;        
        INT     coco_394;        
        mirEXPR coco_395;        
        BOOL    coco_396;        
        BOOL    coco_397;        
        INT     coco_398;        
        UNIV_INT        coco_399;        
        BOOL    coco_400;        
        BOOL    coco_401;        
        mirEXPR coco_402;        
        mirEXPR coco_403;        
        mirEXPR coco_404;        
        mirEXPR coco_405;        
        INT     f_caller;
        } o_mirEndTailCall;
        struct x_Sort10_mirEvaluate
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_406;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirEXPR f_Expr;
        } o_mirEvaluate;
        struct x_Sort10_mirExternProc
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_407;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        } o_mirExternProc;
        struct x_Sort10_mirField
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_408;        
        INT     coco_409;        
        mirEXPR coco_410;        
        BOOL    coco_411;        
        BOOL    coco_412;        
        INT     coco_413;        
        UNIV_INT        coco_414;        
        BOOL    coco_415;        
        BOOL    coco_416;        
        REAL    f_UseEstimate;
        BOOL    f_AddressKnown;
        BOOL    f_IsTempo;
        BOOL    f_IsWriteOnce;
        INT     f_cpid;
        BOOL    f_IsVolatile;
        INT     f_RegisterCandidacy;
        UNIV_INT        f_Offset;
        mirTYPE f_Composite;
        } o_mirField;
        struct x_Sort10_mirFuncCall
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_417;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_418;        
        INT     coco_419;        
        mirEXPR coco_420;        
        BOOL    coco_421;        
        BOOL    coco_422;        
        INT     coco_423;        
        UNIV_INT        coco_424;        
        BOOL    coco_425;        
        BOOL    coco_426;        
        mirEXPR coco_427;        
        mirEXPR coco_428;        
        mirEXPR coco_429;        
        mirEXPR f_Res;
        mirEXPR f_Proc;
        LIST_mirEXPR    f_Params;
        } o_mirFuncCall;
        struct x_Sort10_mirGoto
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_430;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_431;        
        INT     coco_432;        
        mirEXPR coco_433;        
        BOOL    coco_434;        
        BOOL    coco_435;        
        INT     coco_436;        
        UNIV_INT        coco_437;        
        BOOL    coco_438;        
        BOOL    coco_439;        
        mirEXPR coco_440;        
        mirEXPR f_Target;
        } o_mirGoto;
        struct x_Sort10_mirHandler
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_441;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirEXPR f_ParamObj;
        mirEXPR f_IdObj;
        } o_mirHandler;
        struct x_Sort10_mirHeapAllocate
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_442;        
        INT     coco_443;        
        mirEXPR coco_444;        
        BOOL    coco_445;        
        BOOL    coco_446;        
        INT     coco_447;        
        UNIV_INT        coco_448;        
        BOOL    coco_449;        
        BOOL    coco_450;        
        mirEXPR coco_451;        
        mirEXPR coco_452;        
        mirEXPR coco_453;        
        mirEXPR f_Res;
        mirEXPR f_Size;
        } o_mirHeapAllocate;
        struct x_Sort10_mirIf
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_454;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_455;        
        INT     coco_456;        
        mirEXPR coco_457;        
        BOOL    coco_458;        
        BOOL    coco_459;        
        INT     coco_460;        
        UNIV_INT        coco_461;        
        BOOL    coco_462;        
        BOOL    coco_463;        
        mirEXPR coco_464;        
        mirEXPR coco_465;        
        mirEXPR coco_466;        
        mirEXPR coco_467;        
        mirEXPR f_Else;
        mirEXPR coco_468;        
        mirEXPR f_Cond;
        mirEXPR f_Then;
        } o_mirIf;
        struct x_Sort10_mirIntConst
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_469;        
        INT     coco_470;        
        mirCFG  coco_471;        
        INT     coco_472;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        UNIV_INT        f_Value;
        } o_mirIntConst;
        struct x_Sort10_mirInteger
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_473;        
        BOOL    coco_474;        
        INT     coco_475;        
        mirCFG  coco_476;        
        INT     coco_477;        
        mirVARIANCE     coco_478;        
        INT     coco_479;        
        mirEXPR coco_480;        
        BOOL    coco_481;        
        BOOL    coco_482;        
        INT     f_Alignment;
        UNIV_INT        f_Size;
        BOOL    f_AlignmentKnown;
        BOOL    f_SizeKnown;
        INT     f_TagSize;
        INT     f_TagValue;
        UNIV_INT        f_Upb;
        BOOL    f_Signed;
        UNIV_INT        f_Lwb;
        } o_mirInteger;
        struct x_Sort10_mirIntraCfg
        {
        opcode_t        op;
        mirCFG  f_CfgExit;
        mirProc f_Proc;
        LIST_mirCFG     f_NodeList;
        INT     f_NumOfNodes;
        mirCFG  f_CfgEntry;
        LIST_pagPair    f_replace;
        } o_mirIntraCfg;
        struct x_Sort10_mirIterator
        {
        opcode_t        op;
        mirRegularSection       f_Triplet;
        mirObject       f_Index;
        } o_mirIterator;
        struct x_Sort10_mirLocal
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_483;        
        INT     coco_484;        
        mirEXPR coco_485;        
        BOOL    coco_486;        
        BOOL    coco_487;        
        INT     coco_488;        
        UNIV_INT        coco_489;        
        BOOL    coco_490;        
        BOOL    coco_491;        
        REAL    f_UseEstimate;
        BOOL    f_AddressKnown;
        BOOL    f_IsTempo;
        BOOL    f_IsWriteOnce;
        INT     f_cpid;
        BOOL    f_IsVolatile;
        INT     f_RegisterCandidacy;
        UNIV_INT        f_Offset;
        mirProcGlobal   f_Procedure;
        } o_mirLocal;
        struct x_Sort10_mirMask
        {
        opcode_t        op;
        mirOrigin       coco_492;        
        LABEL   coco_493;        
        INT     coco_494;        
        mirTYPE coco_495;        
        BOOL    coco_496;        
        INT     coco_497;        
        mirCFG  coco_498;        
        INT     coco_499;        
        mirVARIANCE     coco_500;        
        INT     coco_501;        
        mirEXPR coco_502;        
        BOOL    coco_503;        
        BOOL    coco_504;        
        INT     coco_505;        
        UNIV_INT        coco_506;        
        BOOL    coco_507;        
        BOOL    coco_508;        
        mirEXPR coco_509;        
        mirEXPR coco_510;        
        mirEXPR coco_511;        
        mirEXPR coco_512;        
        mirEXPR coco_513;        
        mirEXPR coco_514;        
        mirEXPR f_Cond;
        } o_mirMask;
        struct x_Sort10_mirMember
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_515;        
        INT     coco_516;        
        mirCFG  coco_517;        
        INT     coco_518;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Base;
        mirField        f_Field;
        } o_mirMember;
        struct x_Sort10_mirMod
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_519;        
        INT     coco_520;        
        mirCFG  coco_521;        
        INT     coco_522;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        mirEXPR f_OnZero;
        } o_mirMod;
        struct x_Sort10_mirMult
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_523;        
        INT     coco_524;        
        mirCFG  coco_525;        
        INT     coco_526;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        } o_mirMult;
        struct x_Sort10_mirNeg
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_527;        
        INT     coco_528;        
        mirCFG  coco_529;        
        INT     coco_530;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        INT     coco_531;        
        UNIV_INT        coco_532;        
        BOOL    coco_533;        
        BOOL    coco_534;        
        mirEXPR f_Value;
        } o_mirNeg;
        struct x_Sort10_mirNoExpr
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_535;        
        INT     coco_536;        
        mirCFG  coco_537;        
        INT     coco_538;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        } o_mirNoExpr;
        struct x_Sort10_mirNoType
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_539;        
        BOOL    coco_540;        
        INT     coco_541;        
        mirCFG  coco_542;        
        INT     coco_543;        
        mirVARIANCE     coco_544;        
        INT     coco_545;        
        mirEXPR coco_546;        
        BOOL    coco_547;        
        BOOL    coco_548;        
        INT     f_Alignment;
        UNIV_INT        f_Size;
        BOOL    f_AlignmentKnown;
        BOOL    f_SizeKnown;
        } o_mirNoType;
        struct x_Sort10_mirNot
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_549;        
        INT     coco_550;        
        mirCFG  coco_551;        
        INT     coco_552;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        INT     coco_553;        
        UNIV_INT        coco_554;        
        BOOL    coco_555;        
        BOOL    coco_556;        
        mirEXPR f_Value;
        } o_mirNot;
        struct x_Sort10_mirObjectAddr
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_557;        
        INT     coco_558;        
        mirCFG  coco_559;        
        INT     coco_560;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirObject       f_Obj;
        } o_mirObjectAddr;
        struct x_Sort10_mirOr
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_561;        
        INT     coco_562;        
        mirCFG  coco_563;        
        INT     coco_564;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        } o_mirOr;
        struct x_Sort10_mirParallel
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_565;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_566;        
        INT     coco_567;        
        mirEXPR coco_568;        
        BOOL    coco_569;        
        BOOL    coco_570;        
        INT     coco_571;        
        UNIV_INT        coco_572;        
        BOOL    coco_573;        
        BOOL    coco_574;        
        mirEXPR coco_575;        
        mirEXPR coco_576;        
        mirEXPR coco_577;        
        mirEXPR coco_578;        
        mirEXPR f_Next;
        LIST_mirPROCESS f_Processes;
        } o_mirParallel;
        struct x_Sort10_mirParameter
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_579;        
        INT     coco_580;        
        mirEXPR coco_581;        
        BOOL    coco_582;        
        BOOL    coco_583;        
        INT     coco_584;        
        UNIV_INT        coco_585;        
        BOOL    coco_586;        
        BOOL    coco_587;        
        REAL    f_UseEstimate;
        BOOL    f_AddressKnown;
        BOOL    f_IsTempo;
        BOOL    f_IsWriteOnce;
        INT     f_cpid;
        BOOL    f_IsVolatile;
        INT     f_RegisterCandidacy;
        UNIV_INT        f_Offset;
        mirParamKIND    f_ParamKind;
        mirTYPE f_Procedure;
        } o_mirParameter;
        struct x_Sort10_mirPlaced
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_588;        
        INT     coco_589;        
        mirEXPR coco_590;        
        BOOL    coco_591;        
        BOOL    coco_592;        
        INT     coco_593;        
        UNIV_INT        coco_594;        
        BOOL    coco_595;        
        BOOL    coco_596;        
        REAL    f_UseEstimate;
        BOOL    f_AddressKnown;
        BOOL    f_IsTempo;
        BOOL    f_IsWriteOnce;
        INT     f_cpid;
        BOOL    f_IsVolatile;
        INT     f_RegisterCandidacy;
        UNIV_ADDRESS    f_Address;
        } o_mirPlaced;
        struct x_Sort10_mirPlus
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_597;        
        INT     coco_598;        
        mirCFG  coco_599;        
        INT     coco_600;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        } o_mirPlus;
        struct x_Sort10_mirPocf
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_601;        
        BOOL    coco_602;        
        INT     coco_603;        
        mirCFG  coco_604;        
        INT     coco_605;        
        mirVARIANCE     coco_606;        
        INT     coco_607;        
        mirEXPR coco_608;        
        BOOL    coco_609;        
        BOOL    coco_610;        
        INT     f_Alignment;
        UNIV_INT        f_Size;
        BOOL    f_AlignmentKnown;
        BOOL    f_SizeKnown;
        INT     f_TagSize;
        INT     f_TagValue;
        mirTYPE f_RefType;
        BOOL    f_Volatile;
        BOOL    f_Storable;
        } o_mirPocf;
        struct x_Sort10_mirPointer
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_611;        
        BOOL    coco_612;        
        INT     coco_613;        
        mirCFG  coco_614;        
        INT     coco_615;        
        mirVARIANCE     coco_616;        
        INT     coco_617;        
        mirEXPR coco_618;        
        BOOL    coco_619;        
        BOOL    coco_620;        
        INT     f_Alignment;
        UNIV_INT        f_Size;
        BOOL    f_AlignmentKnown;
        BOOL    f_SizeKnown;
        INT     f_TagSize;
        INT     f_TagValue;
        mirTYPE f_RefType;
        BOOL    f_Volatile;
        BOOL    f_Storable;
        } o_mirPointer;
        struct x_Sort10_mirPopHandler
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_621;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        } o_mirPopHandler;
        struct x_Sort10_mirProcGlobal
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_622;        
        INT     coco_623;        
        mirEXPR coco_624;        
        BOOL    coco_625;        
        BOOL    coco_626;        
        INT     coco_627;        
        UNIV_INT        coco_628;        
        BOOL    coco_629;        
        BOOL    coco_630;        
        REAL    f_UseEstimate;
        BOOL    f_AddressKnown;
        BOOL    f_IsTempo;
        BOOL    f_IsWriteOnce;
        INT     f_cpid;
        BOOL    f_IsVolatile;
        INT     f_RegisterCandidacy;
        NAME    f_Name;
        mirLINKAGE      f_Linkage;
        mirSection      f_Section;
        LIST_mirParameter       f_Params;
        mirBody f_Body;
        mirCKF  f_Predef;
        } o_mirProcGlobal;
        struct x_Sort10_mirProcType
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_631;        
        BOOL    coco_632;        
        INT     coco_633;        
        mirCFG  coco_634;        
        INT     coco_635;        
        mirVARIANCE     coco_636;        
        INT     coco_637;        
        mirEXPR coco_638;        
        BOOL    coco_639;        
        BOOL    coco_640;        
        INT     f_Alignment;
        UNIV_INT        f_Size;
        BOOL    f_AlignmentKnown;
        BOOL    f_SizeKnown;
        REAL    coco_641;        
        BOOL    coco_642;        
        BOOL    coco_643;        
        BOOL    coco_644;        
        INT     coco_645;        
        BOOL    coco_646;        
        INT     coco_647;        
        NAME    coco_648;        
        mirLINKAGE      coco_649;        
        mirSection      coco_650;        
        LIST_mirParameter       f_Params;
        BOOL    f_MoreArgs;
        mirTYPE f_ResultType;
        } o_mirProcType;
        struct x_Sort10_mirProcess
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_651;        
        BOOL    coco_652;        
        INT     coco_653;        
        mirCFG  coco_654;        
        INT     coco_655;        
        mirVARIANCE     coco_656;        
        INT     coco_657;        
        mirEXPR coco_658;        
        BOOL    coco_659;        
        BOOL    coco_660;        
        INT     coco_661;        
        UNIV_INT        coco_662;        
        BOOL    coco_663;        
        BOOL    coco_664;        
        mirEXPR coco_665;        
        mirEXPR f_Target;
        } o_mirProcess;
        struct x_Sort10_mirPushHandler
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_666;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirEXPR f_Handler;
        } o_mirPushHandler;
        struct x_Sort10_mirQuo
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_667;        
        INT     coco_668;        
        mirCFG  coco_669;        
        INT     coco_670;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        mirEXPR f_OnZero;
        } o_mirQuo;
        struct x_Sort10_mirRaise
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_671;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirEXPR f_Param;
        mirEXPR f_Id;
        } o_mirRaise;
        struct x_Sort10_mirReal
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_672;        
        BOOL    coco_673;        
        INT     coco_674;        
        mirCFG  coco_675;        
        INT     coco_676;        
        mirVARIANCE     coco_677;        
        INT     coco_678;        
        mirEXPR coco_679;        
        BOOL    coco_680;        
        BOOL    coco_681;        
        INT     f_Alignment;
        UNIV_INT        f_Size;
        BOOL    f_AlignmentKnown;
        BOOL    f_SizeKnown;
        } o_mirReal;
        struct x_Sort10_mirRealConst
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_682;        
        INT     coco_683;        
        mirCFG  coco_684;        
        INT     coco_685;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        UNIV_REAL       f_Value;
        } o_mirRealConst;
        struct x_Sort10_mirReceive
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_686;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_687;        
        INT     coco_688;        
        mirEXPR coco_689;        
        BOOL    coco_690;        
        BOOL    coco_691;        
        INT     coco_692;        
        UNIV_INT        coco_693;        
        BOOL    coco_694;        
        BOOL    coco_695;        
        mirEXPR coco_696;        
        mirEXPR coco_697;        
        mirEXPR f_Channel;
        mirEXPR coco_698;        
        mirEXPR coco_699;        
        mirEXPR f_Variable;
        } o_mirReceive;
        struct x_Sort10_mirReceiveMem
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_700;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_701;        
        INT     coco_702;        
        mirEXPR coco_703;        
        BOOL    coco_704;        
        BOOL    coco_705;        
        INT     coco_706;        
        UNIV_INT        coco_707;        
        BOOL    coco_708;        
        BOOL    coco_709;        
        mirEXPR coco_710;        
        mirEXPR coco_711;        
        mirEXPR f_Channel;
        mirEXPR coco_712;        
        mirEXPR f_Size;
        mirEXPR f_Variable;
        } o_mirReceiveMem;
        struct x_Sort10_mirRecord
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_713;        
        BOOL    coco_714;        
        INT     coco_715;        
        mirCFG  coco_716;        
        INT     coco_717;        
        mirVARIANCE     coco_718;        
        INT     coco_719;        
        mirEXPR coco_720;        
        BOOL    coco_721;        
        BOOL    coco_722;        
        INT     f_Alignment;
        UNIV_INT        f_Size;
        BOOL    f_AlignmentKnown;
        BOOL    f_SizeKnown;
        BOOL    f_Ordered;
        LIST_mirField   f_Fields;
        } o_mirRecord;
        struct x_Sort10_mirRecordCom
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_723;        
        INT     coco_724;        
        mirCFG  coco_725;        
        INT     coco_726;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        LIST_mirEXPR    f_Value;
        } o_mirRecordCom;
        struct x_Sort10_mirRegister
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_727;        
        INT     coco_728;        
        mirEXPR coco_729;        
        BOOL    coco_730;        
        BOOL    coco_731;        
        INT     coco_732;        
        UNIV_INT        coco_733;        
        BOOL    coco_734;        
        BOOL    coco_735;        
        REAL    f_UseEstimate;
        BOOL    f_AddressKnown;
        BOOL    f_IsTempo;
        BOOL    f_IsWriteOnce;
        INT     f_cpid;
        BOOL    f_IsVolatile;
        INT     f_RegisterCandidacy;
        INT     f_RegisterId;
        } o_mirRegister;
        struct x_Sort10_mirRelease
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_736;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_737;        
        INT     coco_738;        
        mirEXPR coco_739;        
        BOOL    coco_740;        
        BOOL    coco_741;        
        INT     coco_742;        
        UNIV_INT        coco_743;        
        BOOL    coco_744;        
        BOOL    coco_745;        
        mirEXPR coco_746;        
        mirEXPR coco_747;        
        mirEXPR coco_748;        
        mirEXPR f_Gate;
        } o_mirRelease;
        struct x_Sort10_mirRem
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_749;        
        INT     coco_750;        
        mirCFG  coco_751;        
        INT     coco_752;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        mirEXPR f_OnZero;
        } o_mirRem;
        struct x_Sort10_mirRemote
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_753;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_754;        
        INT     coco_755;        
        mirEXPR coco_756;        
        BOOL    coco_757;        
        BOOL    coco_758;        
        INT     coco_759;        
        UNIV_INT        coco_760;        
        BOOL    coco_761;        
        BOOL    coco_762;        
        mirEXPR coco_763;        
        mirEXPR f_Target;
        mirEXPR f_ProcNr;
        } o_mirRemote;
        struct x_Sort10_mirReplicatedAlt
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_764;        
        BOOL    coco_765;        
        INT     coco_766;        
        mirCFG  coco_767;        
        INT     coco_768;        
        mirVARIANCE     coco_769;        
        INT     coco_770;        
        mirEXPR coco_771;        
        BOOL    coco_772;        
        BOOL    coco_773;        
        INT     coco_774;        
        UNIV_INT        coco_775;        
        BOOL    coco_776;        
        BOOL    coco_777;        
        mirEXPR coco_778;        
        mirEXPR f_Target;
        mirEXPR f_Upb;
        mirEXPR f_Lwb;
        mirEXPR f_Var;
        mirALTERNATIVE  f_Alt;
        } o_mirReplicatedAlt;
        struct x_Sort10_mirReplicatedProcess
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_779;        
        BOOL    coco_780;        
        INT     coco_781;        
        mirCFG  coco_782;        
        INT     coco_783;        
        mirVARIANCE     coco_784;        
        INT     coco_785;        
        mirEXPR coco_786;        
        BOOL    coco_787;        
        BOOL    coco_788;        
        INT     coco_789;        
        UNIV_INT        coco_790;        
        BOOL    coco_791;        
        BOOL    coco_792;        
        mirEXPR coco_793;        
        mirEXPR f_Target;
        mirEXPR f_Upb;
        mirEXPR f_Lwb;
        mirEXPR f_Var;
        } o_mirReplicatedProcess;
        struct x_Sort10_mirReturn
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_794;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_795;        
        INT     coco_796;        
        mirEXPR coco_797;        
        BOOL    coco_798;        
        BOOL    coco_799;        
        INT     coco_800;        
        UNIV_INT        coco_801;        
        BOOL    coco_802;        
        BOOL    coco_803;        
        mirEXPR f_Value;
        mirEXPR coco_804;        
        mirEXPR coco_805;        
        mirEXPR coco_806;        
        mirEXPR f_Next;
        } o_mirReturn;
        struct x_Sort10_mirRight
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_807;        
        INT     coco_808;        
        mirCFG  coco_809;        
        INT     coco_810;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        } o_mirRight;
        struct x_Sort10_mirSection
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        NAME    f_Sname;
        } o_mirSection;
        struct x_Sort10_mirSend
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_811;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_812;        
        INT     coco_813;        
        mirEXPR coco_814;        
        BOOL    coco_815;        
        BOOL    coco_816;        
        INT     coco_817;        
        UNIV_INT        coco_818;        
        BOOL    coco_819;        
        BOOL    coco_820;        
        mirEXPR coco_821;        
        mirEXPR coco_822;        
        mirEXPR f_Channel;
        mirEXPR f_Message;
        } o_mirSend;
        struct x_Sort10_mirSendMem
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_823;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_824;        
        INT     coco_825;        
        mirEXPR coco_826;        
        BOOL    coco_827;        
        BOOL    coco_828;        
        INT     coco_829;        
        UNIV_INT        coco_830;        
        BOOL    coco_831;        
        BOOL    coco_832;        
        mirEXPR coco_833;        
        mirEXPR coco_834;        
        mirEXPR f_Channel;
        mirEXPR coco_835;        
        mirEXPR f_Size;
        mirEXPR f_Addr;
        } o_mirSendMem;
        struct x_Sort10_mirShiftLeft
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_836;        
        INT     coco_837;        
        mirCFG  coco_838;        
        INT     coco_839;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        } o_mirShiftLeft;
        struct x_Sort10_mirShiftRight
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_840;        
        INT     coco_841;        
        mirCFG  coco_842;        
        INT     coco_843;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        } o_mirShiftRight;
        struct x_Sort10_mirShiftRightSign
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_844;        
        INT     coco_845;        
        mirCFG  coco_846;        
        INT     coco_847;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        } o_mirShiftRightSign;
        struct x_Sort10_mirStackAllocate
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_848;        
        INT     coco_849;        
        mirEXPR coco_850;        
        BOOL    coco_851;        
        BOOL    coco_852;        
        INT     coco_853;        
        UNIV_INT        coco_854;        
        BOOL    coco_855;        
        BOOL    coco_856;        
        mirEXPR coco_857;        
        mirEXPR coco_858;        
        mirEXPR coco_859;        
        mirEXPR f_Res;
        mirEXPR f_Size;
        } o_mirStackAllocate;
        struct x_Sort10_mirSubscript
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_860;        
        INT     coco_861;        
        mirCFG  coco_862;        
        INT     coco_863;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Base;
        mirEXPR f_Index;
        } o_mirSubscript;
        struct x_Sort10_mirSwitch
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_864;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_865;        
        INT     coco_866;        
        mirEXPR coco_867;        
        BOOL    coco_868;        
        BOOL    coco_869;        
        INT     coco_870;        
        UNIV_INT        coco_871;        
        BOOL    coco_872;        
        BOOL    coco_873;        
        mirEXPR f_Value;
        mirEXPR f_Default;
        LIST_mirCase    f_Cases;
        } o_mirSwitch;
        struct x_Sort10_mirTailCall
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_874;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_875;        
        INT     coco_876;        
        mirEXPR coco_877;        
        BOOL    coco_878;        
        BOOL    coco_879;        
        INT     coco_880;        
        UNIV_INT        coco_881;        
        BOOL    coco_882;        
        BOOL    coco_883;        
        mirEXPR coco_884;        
        mirEXPR coco_885;        
        mirEXPR coco_886;        
        mirEXPR coco_887;        
        mirEXPR f_Proc;
        LIST_mirEXPR    f_Params;
        } o_mirTailCall;
        struct x_Sort10_mirTryAcquire
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_888;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_889;        
        INT     coco_890;        
        mirEXPR coco_891;        
        BOOL    coco_892;        
        BOOL    coco_893;        
        INT     coco_894;        
        UNIV_INT        coco_895;        
        BOOL    coco_896;        
        BOOL    coco_897;        
        mirEXPR coco_898;        
        mirEXPR coco_899;        
        mirEXPR coco_900;        
        mirEXPR f_Gate;
        mirEXPR f_Else;
        mirEXPR f_Count;
        mirEXPR coco_901;        
        mirEXPR f_Then;
        } o_mirTryAcquire;
        struct x_Sort10_mirTypeSize
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_902;        
        INT     coco_903;        
        mirCFG  coco_904;        
        INT     coco_905;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirTYPE f_OfType;
        } o_mirTypeSize;
        struct x_Sort10_mirUnion
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_906;        
        BOOL    coco_907;        
        INT     coco_908;        
        mirCFG  coco_909;        
        INT     coco_910;        
        mirVARIANCE     coco_911;        
        INT     coco_912;        
        mirEXPR coco_913;        
        BOOL    coco_914;        
        BOOL    coco_915;        
        INT     f_Alignment;
        UNIV_INT        f_Size;
        BOOL    f_AlignmentKnown;
        BOOL    f_SizeKnown;
        LIST_mirField   f_Members;
        } o_mirUnion;
        struct x_Sort10_mirUnionCom
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_916;        
        INT     coco_917;        
        mirCFG  coco_918;        
        INT     coco_919;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        INT     coco_920;        
        UNIV_INT        coco_921;        
        BOOL    coco_922;        
        BOOL    coco_923;        
        mirEXPR f_Value;
        mirField        f_Part;
        } o_mirUnionCom;
        struct x_Sort10_mirWaitAndBranch
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE coco_924;        
        BOOL    f_Init;
        INT     f_Number;
        mirCFG  f_Node;
        INT     f_Visualized;
        mirVARIANCE     coco_925;        
        INT     coco_926;        
        mirEXPR coco_927;        
        BOOL    coco_928;        
        BOOL    coco_929;        
        INT     coco_930;        
        UNIV_INT        coco_931;        
        BOOL    coco_932;        
        BOOL    coco_933;        
        mirEXPR coco_934;        
        mirEXPR coco_935;        
        mirEXPR coco_936;        
        mirEXPR coco_937;        
        mirEXPR f_Else;
        LIST_mirALTERNATIVE     f_Alternatives;
        } o_mirWaitAndBranch;
        struct x_Sort10_mirXor
        {
        opcode_t        op;
        mirOrigin       f_Origin;
        LABEL   f_Label;
        INT     f_Printed;
        mirTYPE f_Type;
        BOOL    coco_938;        
        INT     coco_939;        
        mirCFG  coco_940;        
        INT     coco_941;        
        mirVARIANCE     f_Variance;
        INT     f_UseCount;
        mirEXPR f_OnError;
        BOOL    f_Checked;
        BOOL    f_Group;
        mirEXPR f_Left;
        mirEXPR f_Right;
        BOOL    f_Strict;
        } o_mirXor;
        struct x_Sort10_pagPair
        {
        opcode_t        op;
        mirParameter    f_new;
        mirParameter    f_old;
        } o_pagPair;
};
union x_Sort2
{
         
        struct x_Sort2_IntConst
        {
        opcode_t        op;
        UNIV_INT        f_i;
        } o_IntConst;
        struct x_Sort2_RealConst
        {
        opcode_t        op;
        UNIV_REAL       f_i;
        } o_RealConst;
};
union x_Sort3
{
         
        struct x_Sort3_Triple
        {
        opcode_t        op;
        Lift_Int        f_z3;
        UNIV_INT        f_z2;
        UNIV_INT        f_z1;
        } o_Triple;
};
union x_Sort4
{
         
        struct x_Sort4_Bot
        {
        opcode_t        op;
        } o_Bot;
        struct x_Sort4_Norm
        {
        opcode_t        op;
        UNIV_INT        f_i;
        } o_Norm;
        struct x_Sort4_Top
        {
        opcode_t        op;
        } o_Top;
};
union x_Sort12
{
         
        struct x_Sort12_mirNoBody
        {
        opcode_t        op;
        LABEL   f_Label;
        INT     f_Printed;
        mirOrigin       f_Origin;
        } o_mirNoBody;
        struct x_Sort12_mirProcBody
        {
        opcode_t        op;
        LABEL   f_Label;
        INT     f_Printed;
        mirOrigin       f_Origin;
        LIST_mirLocal   f_Locals;
        LIST_mirBasicBlock      f_LinearBlocks;
        INT     f_Level;
        mirProcGlobal   f_Parent;
        } o_mirProcBody;
};
union x_Sort13
{
         
        struct x_Sort13_mirRegularSection
        {
        opcode_t        op;
        mirEXPR f_Stride;
        mirEXPR f_Upb;
        mirEXPR f_Lwb;
        } o_mirRegularSection;
};
union x_Sort14
{
         
        struct x_Sort14_IR
        {
        opcode_t        op;
        INT     f_Printed;
        LABEL   f_Label;
        } o_IR;
};
union x_Sort16
{
         
        struct x_Sort16_TarDes
        {
        opcode_t        op;
        INT     f_SZPOINT;
        INT     f_SZINT;
        INT     f_ALPOINT;
        INT     f_ALINT;
        INT     f_EXEMAGIC;
        INT     f_OBJMAGIC;
        NAME    f_LABFMT;
        NAME    f_FCONFMT;
        NAME    f_CONFMT;
        BOOL    f_STARGREF;
        BOOL    f_ARGCOPYIN;
        BOOL    f_RTOLBYTES;
        BOOL    f_BACKPARAM;
        BOOL    f_BACKTEMP;
        BOOL    f_BACKAUTO;
        INT     f_SEGNAME;
        INT     f_AUTOINIT;
        INT     f_ARGINIT;
        INT     f_SZWORD;
        INT     f_SZDOUBLE;
        INT     f_SZFLOAT;
        INT     f_SZLONG;
        INT     f_SZSHORT;
        INT     f_SZCHAR;
        INT     f_ALDATA;
        INT     f_ALSTACK;
        INT     f_ALSTRUCT;
        INT     f_ALDOUBLE;
        INT     f_ALFLOAT;
        INT     f_ALLONG;
        INT     f_ALSHORT;
        INT     f_ALCHAR;
        INT     f_Printed;
        LABEL   f_Label;
        } o_TarDes;
};
union x_Sort56
{
         
        struct x_Sort56_mirStandard
        {
        opcode_t        op;
        mirTYPE f_Void;
        mirTYPE f_Offset;
        mirEXPR f_NoError;
        mirTYPE f_Index;
        mirSection      f_Data;
        mirSection      f_Code;
        mirTYPE f_Boolean;
        mirTYPE f_BlockPtr;
        mirSection      f_Blank;
        mirOrigin       f_Origin;
        INT     f_Printed;
        LABEL   f_Label;
        } o_mirStandard;
};
union x_Sort58
{
         
        struct x_Sort58_LIST_Const_List
        {
        opcode_t        op;
        LIST_mirIntraCfg_FLIST  f_Elems;
        LIST_mirIntraCfg_FLIST  f_Tail;
        } o_LIST_Const_List;
};
union x_Sort57
{
         
        struct x_Sort57_LIST_Const_FLIST_Flist
        {
        opcode_t        op;
        LIST_mirIntraCfg_FLIST  f_Next;
        mirIntraCfg     f_Value;
        LIST_mirBasicBlock      f_TailPtr;
        } o_LIST_Const_FLIST_Flist;
};
mirUnit mirUnit_copy (mirUnit);
mirUnit mirUnit_copy (mirUnit);
mirIntraCfg     mirIntraCfg_copy (mirIntraCfg);
mirProcGlobal   mirProcGlobal_copy (mirProcGlobal);
mirProc mirProc_copy (mirProc);
mirEDGE mirEDGE_copy (mirEDGE);
mirCFG  mirCFG_copy (mirCFG);
mirField        mirField_copy (mirField);
mirBasicBlock   mirBasicBlock_copy (mirBasicBlock);
mirDataGlobal   mirDataGlobal_copy (mirDataGlobal);
mirGLOBAL       mirGLOBAL_copy (mirGLOBAL);
mirLocal        mirLocal_copy (mirLocal);
mirParameter    mirParameter_copy (mirParameter);
mirPlaced       mirPlaced_copy (mirPlaced);
mirRegister     mirRegister_copy (mirRegister);
mirObject       mirObject_copy (mirObject);
Const   Const_copy (Const);
Symbols Symbols_copy (Symbols);
mirTYPE mirTYPE_copy (mirTYPE);
mirOP1  mirOP1_copy (mirOP1);
mirDIVISION     mirDIVISION_copy (mirDIVISION);
mirOP2NoDiv     mirOP2NoDiv_copy (mirOP2NoDiv);
mirOP2  mirOP2_copy (mirOP2);
mirSimpleExpr   mirSimpleExpr_copy (mirSimpleExpr);
mirEXPR mirEXPR_copy (mirEXPR);
mirControlSTMT  mirControlSTMT_copy (mirControlSTMT);
mirSimpleSTMT   mirSimpleSTMT_copy (mirSimpleSTMT);
mirSTMT mirSTMT_copy (mirSTMT);
mirCase mirCase_copy (mirCase);
mirSection      mirSection_copy (mirSection);
mirPROCESS      mirPROCESS_copy (mirPROCESS);
mirALTERNATIVE  mirALTERNATIVE_copy (mirALTERNATIVE);
mirIterMask     mirIterMask_copy (mirIterMask);
mirEDGE mirEDGE_create (opcode_t);
mirEDGE mirEDGE_copy (mirEDGE);
mirCFG  mirCFG_create (opcode_t);
mirCFG  mirCFG_copy (mirCFG);
mirTYPE mirTYPE_copy (mirTYPE);
mirProc mirProc_create (opcode_t);
mirProc mirProc_copy (mirProc);
mirProcGlobal   mirProcGlobal_create (opcode_t);
mirProcGlobal   mirProcGlobal_copy (mirProcGlobal);
mirBasicBlock   mirBasicBlock_copy (mirBasicBlock);
mirLocal        mirLocal_copy (mirLocal);
mirParameter    mirParameter_create (opcode_t);
mirParameter    mirParameter_copy (mirParameter);
mirField        mirField_copy (mirField);
mirDataGlobal   mirDataGlobal_copy (mirDataGlobal);
mirGLOBAL       mirGLOBAL_create (opcode_t);
mirGLOBAL       mirGLOBAL_copy (mirGLOBAL);
mirPlaced       mirPlaced_copy (mirPlaced);
mirRegister     mirRegister_copy (mirRegister);
mirObject       mirObject_create (opcode_t);
mirObject       mirObject_copy (mirObject);
mirOP1  mirOP1_copy (mirOP1);
mirDIVISION     mirDIVISION_copy (mirDIVISION);
mirOP2NoDiv     mirOP2NoDiv_copy (mirOP2NoDiv);
mirOP2  mirOP2_copy (mirOP2);
mirSimpleExpr   mirSimpleExpr_copy (mirSimpleExpr);
mirEXPR mirEXPR_copy (mirEXPR);
mirIterMask     mirIterMask_copy (mirIterMask);
mirPROCESS      mirPROCESS_copy (mirPROCESS);
mirALTERNATIVE  mirALTERNATIVE_copy (mirALTERNATIVE);
mirControlSTMT  mirControlSTMT_copy (mirControlSTMT);
mirSimpleSTMT   mirSimpleSTMT_create (opcode_t);
mirSimpleSTMT   mirSimpleSTMT_copy (mirSimpleSTMT);
mirSTMT mirSTMT_create (opcode_t);
mirSTMT mirSTMT_copy (mirSTMT);
mirCase mirCase_copy (mirCase);
mirSection      mirSection_copy (mirSection);
mirCFG  mirCFG_create (opcode_t);
mirCFG  mirCFG_copy (mirCFG);
mirEDGE mirEDGE_copy (mirEDGE);
Const   Const_create (opcode_t);
Const   Const_copy (Const);
mirIntraCfg     mirIntraCfg_copy (mirIntraCfg);
mirProc mirProc_copy (mirProc);
mirTYPE mirTYPE_copy (mirTYPE);
mirDataGlobal   mirDataGlobal_copy (mirDataGlobal);
mirProcGlobal   mirProcGlobal_copy (mirProcGlobal);
mirBasicBlock   mirBasicBlock_copy (mirBasicBlock);
mirLocal        mirLocal_copy (mirLocal);
mirParameter    mirParameter_copy (mirParameter);
mirField        mirField_copy (mirField);
mirPlaced       mirPlaced_copy (mirPlaced);
mirRegister     mirRegister_copy (mirRegister);
mirGLOBAL       mirGLOBAL_copy (mirGLOBAL);
mirObject       mirObject_copy (mirObject);
mirOP1  mirOP1_copy (mirOP1);
mirDIVISION     mirDIVISION_copy (mirDIVISION);
mirOP2NoDiv     mirOP2NoDiv_copy (mirOP2NoDiv);
mirOP2  mirOP2_copy (mirOP2);
mirSimpleExpr   mirSimpleExpr_copy (mirSimpleExpr);
mirEXPR mirEXPR_copy (mirEXPR);
mirIterMask     mirIterMask_copy (mirIterMask);
mirPROCESS      mirPROCESS_copy (mirPROCESS);
mirALTERNATIVE  mirALTERNATIVE_copy (mirALTERNATIVE);
mirControlSTMT  mirControlSTMT_copy (mirControlSTMT);
mirSimpleSTMT   mirSimpleSTMT_copy (mirSimpleSTMT);
mySTMT  mySTMT_copy (mySTMT);
mirSTMT mirSTMT_copy (mirSTMT);
mirCase mirCase_copy (mirCase);
mirSection      mirSection_copy (mirSection);
Symbols Symbols_create (opcode_t);
Symbols Symbols_copy (Symbols);
VALUE   VALUE_copy (VALUE);
VALUE   VALUE_create (opcode_t);
VALUE   VALUE_copy (VALUE);
mirBody mirBody_copy (mirBody);
mirBody mirBody_copy (mirBody);
mirBody mirBody_copy (mirBody);
mirRegularSection       mirRegularSection_copy (mirRegularSection);
mirRegularSection       mirRegularSection_copy (mirRegularSection);
mirRegularSection       mirRegularSection_copy (mirRegularSection);
LIST_mirIntraCfg        d_LIST_mirIntraCfg_create (opcode_t);
void    d_LIST_mirIntraCfg_delete (LIST_mirIntraCfg);
LIST_mirIntraCfg  LIST_mirIntraCfg_tl(LIST_mirIntraCfg);
LIST_mirIntraCfg        LIST_mirIntraCfg_create(void);
LIST_mirIntraCfg        LIST_mirIntraCfg_listoneelt (mirIntraCfg);
LIST_mirIntraCfg        LIST_mirIntraCfg_copy (LIST_mirIntraCfg);
LIST_mirIntraCfg_FLIST  LIST_mirIntraCfg_FLIST_deepcopy (LIST_mirIntraCfg_FLIST p);
LIST_mirIntraCfg        LIST_mirIntraCfg_merge (LIST_mirIntraCfg, LIST_mirIntraCfg);
void    LIST_mirIntraCfg_delete(LIST_mirIntraCfg This);
int     LIST_mirIntraCfg_FLIST_pos (LIST_mirIntraCfg_FLIST p, mirIntraCfg elt);
int     LIST_mirIntraCfg_FLIST_length (LIST_mirIntraCfg_FLIST p);
mirIntraCfg     LIST_mirIntraCfg_last (LIST_mirIntraCfg);
mirIntraCfg     LIST_mirIntraCfg_nth (LIST_mirIntraCfg, int);
mirIntraCfg     LIST_mirIntraCfg_FLIST_nth (LIST_mirIntraCfg_FLIST p, int i);
LIST_mirIntraCfg_FLIST  LIST_mirIntraCfg_FLIST_index (LIST_mirIntraCfg_FLIST p, int i);
mirIntraCfg     LIST_mirIntraCfg_hd (LIST_mirIntraCfg);
LIST_mirIntraCfg_FLIST  LIST_mirIntraCfg_FLIST_before (LIST_mirIntraCfg_FLIST p, LIST_mirIntraCfg_FLIST q);
LIST_mirIntraCfg_FLIST  LIST_mirIntraCfg_FLIST_last (LIST_mirIntraCfg_FLIST p);
void    LIST_mirIntraCfg_printids(LIST_mirIntraCfg This);
void    LIST_mirIntraCfg_FLIST_printids (LIST_mirIntraCfg_FLIST p);
mirIntraCfg     LIST_mirIntraCfg_rightbrother(LIST_mirIntraCfg, mirIntraCfg);
BOOL    LIST_mirIntraCfg_isequal (LIST_mirIntraCfg, LIST_mirIntraCfg);
BOOL    LIST_mirIntraCfg_FLIST_isequal (LIST_mirIntraCfg_FLIST p, LIST_mirIntraCfg_FLIST q);
BOOL    LIST_mirIntraCfg_isfirst (LIST_mirIntraCfg, mirIntraCfg);
BOOL    LIST_mirIntraCfg_islast (LIST_mirIntraCfg, mirIntraCfg);
mirIntraCfg     LIST_mirIntraCfg_leftbrother(LIST_mirIntraCfg, mirIntraCfg);
void    LIST_mirIntraCfg_append (LIST_mirIntraCfg, LIST_mirIntraCfg);
void    LIST_mirIntraCfg_appendelt (LIST_mirIntraCfg, mirIntraCfg);
LIST_mirIntraCfg_FLIST  LIST_mirIntraCfg_FLIST_make (mirIntraCfg value, LIST_mirIntraCfg_FLIST next);
void    LIST_mirIntraCfg_assign (LIST_mirIntraCfg, mirIntraCfg, int);
void    LIST_mirIntraCfg_cons (LIST_mirIntraCfg, mirIntraCfg);
void    LIST_mirIntraCfg_reverse (LIST_mirIntraCfg);
void    LIST_mirIntraCfg_dreverse (LIST_mirIntraCfg);
LIST_mirIntraCfg_FLIST  LIST_mirIntraCfg_FLIST_reverse (LIST_mirIntraCfg_FLIST p);
void    LIST_mirIntraCfg_del (LIST_mirIntraCfg, mirIntraCfg);
LIST_mirIntraCfg_FLIST  LIST_mirIntraCfg_FLIST_delelt (LIST_mirIntraCfg_FLIST p, mirIntraCfg value);
void    LIST_mirIntraCfg_insert(LIST_mirIntraCfg, int i, mirIntraCfg E);
LIST_mirIntraCfg_FLIST  LIST_mirIntraCfg_FLIST_insert (LIST_mirIntraCfg_FLIST p, int i, mirIntraCfg elt);
void    LIST_mirIntraCfg_delpos (LIST_mirIntraCfg, int);
LIST_mirIntraCfg_FLIST  LIST_mirIntraCfg_FLIST_delpos (LIST_mirIntraCfg_FLIST p, int pos);
LIST_mirIntraCfg  LIST_mirIntraCfg_fappend (LIST_mirIntraCfg, LIST_mirIntraCfg);
LIST_mirIntraCfg  LIST_mirIntraCfg_fappend1 (LIST_mirIntraCfg, mirIntraCfg);
LIST_mirIntraCfg        LIST_mirIntraCfg_fcons(LIST_mirIntraCfg, mirIntraCfg);
LIST_mirIntraCfg        LIST_mirIntraCfg_freverse (LIST_mirIntraCfg);
struct ITERLIST_mirIntraCfg {
        LIST_mirIntraCfg        thelist;
        LIST_mirIntraCfg_FLIST  cursor;
};
typedef struct ITERLIST_mirIntraCfg * ITERLIST_mirIntraCfg;
ITERLIST_mirIntraCfg ITERLIST_mirIntraCfg_create(LIST_mirIntraCfg);
void    ITERLIST_mirIntraCfg_delete (ITERLIST_mirIntraCfg This);
mirIntraCfg     ITERLIST_mirIntraCfg_getcurrentelt(ITERLIST_mirIntraCfg);
LIST_mirIntraCfg_FLIST  ITERLIST_mirIntraCfg_getcurrentsublist (ITERLIST_mirIntraCfg);
mirIntraCfg     ITERLIST_mirIntraCfg_iter (ITERLIST_mirIntraCfg);
mirIntraCfg     ITERLIST_mirIntraCfg_nth (ITERLIST_mirIntraCfg, int);
mirIntraCfg     ITERLIST_mirIntraCfg_reviter (ITERLIST_mirIntraCfg);
void    ITERLIST_mirIntraCfg_setcursor(ITERLIST_mirIntraCfg This, int pos);
void    ITERLIST_mirIntraCfg_stepcursor (ITERLIST_mirIntraCfg);
void    ITERLIST_mirIntraCfg_tolast (ITERLIST_mirIntraCfg);
void    ITERLIST_mirIntraCfg_assign (ITERLIST_mirIntraCfg, mirIntraCfg elt);
void    ITERLIST_mirIntraCfg_delpos(ITERLIST_mirIntraCfg);
void    ITERLIST_mirIntraCfg_insertelt(ITERLIST_mirIntraCfg cur, mirIntraCfg newelt, BOOL before);
void    ITERLIST_mirIntraCfg_insertlist(ITERLIST_mirIntraCfg cursor, LIST_mirIntraCfg inserted, BOOL before);
LIST_mirIntraCfg ITERLIST_mirIntraCfg_split(ITERLIST_mirIntraCfg);
void    ITERLIST_mirIntraCfg_insertaux(ITERLIST_mirIntraCfg cursor, LIST_mirIntraCfg_FLIST extra, BOOL before);
void    LIST_mirIntraCfg_FLIST_insertlist(LIST_mirIntraCfg_FLIST*, LIST_mirIntraCfg_FLIST*, LIST_mirIntraCfg_FLIST*, LIST_mirIntraCfg_FLIST, BOOL before);
LIST_Symbols    d_LIST_Symbols_create (opcode_t);
void    d_LIST_Symbols_delete (LIST_Symbols);
LIST_Symbols  LIST_Symbols_tl(LIST_Symbols);
LIST_Symbols    LIST_Symbols_create(void);
LIST_Symbols    LIST_Symbols_listoneelt (Symbols);
LIST_Symbols    LIST_Symbols_copy (LIST_Symbols);
LIST_Symbols_FLIST      LIST_Symbols_FLIST_deepcopy (LIST_Symbols_FLIST p);
LIST_Symbols    LIST_Symbols_merge (LIST_Symbols, LIST_Symbols);
void    LIST_Symbols_delete(LIST_Symbols This);
int     LIST_Symbols_FLIST_pos (LIST_Symbols_FLIST p, Symbols elt);
int     LIST_Symbols_FLIST_length (LIST_Symbols_FLIST p);
Symbols LIST_Symbols_last (LIST_Symbols);
Symbols LIST_Symbols_nth (LIST_Symbols, int);
Symbols LIST_Symbols_FLIST_nth (LIST_Symbols_FLIST p, int i);
LIST_Symbols_FLIST      LIST_Symbols_FLIST_index (LIST_Symbols_FLIST p, int i);
Symbols LIST_Symbols_hd (LIST_Symbols);
LIST_Symbols_FLIST      LIST_Symbols_FLIST_before (LIST_Symbols_FLIST p, LIST_Symbols_FLIST q);
LIST_Symbols_FLIST      LIST_Symbols_FLIST_last (LIST_Symbols_FLIST p);
void    LIST_Symbols_printids(LIST_Symbols This);
void    LIST_Symbols_FLIST_printids (LIST_Symbols_FLIST p);
Symbols LIST_Symbols_rightbrother(LIST_Symbols, Symbols);
BOOL    LIST_Symbols_isequal (LIST_Symbols, LIST_Symbols);
BOOL    LIST_Symbols_FLIST_isequal (LIST_Symbols_FLIST p, LIST_Symbols_FLIST q);
BOOL    LIST_Symbols_isfirst (LIST_Symbols, Symbols);
BOOL    LIST_Symbols_islast (LIST_Symbols, Symbols);
Symbols LIST_Symbols_leftbrother(LIST_Symbols, Symbols);
void    LIST_Symbols_append (LIST_Symbols, LIST_Symbols);
void    LIST_Symbols_appendelt (LIST_Symbols, Symbols);
LIST_Symbols_FLIST      LIST_Symbols_FLIST_make (Symbols value, LIST_Symbols_FLIST next);
void    LIST_Symbols_assign (LIST_Symbols, Symbols, int);
void    LIST_Symbols_cons (LIST_Symbols, Symbols);
void    LIST_Symbols_reverse (LIST_Symbols);
void    LIST_Symbols_dreverse (LIST_Symbols);
LIST_Symbols_FLIST      LIST_Symbols_FLIST_reverse (LIST_Symbols_FLIST p);
void    LIST_Symbols_del (LIST_Symbols, Symbols);
LIST_Symbols_FLIST      LIST_Symbols_FLIST_delelt (LIST_Symbols_FLIST p, Symbols value);
void    LIST_Symbols_insert(LIST_Symbols, int i, Symbols E);
LIST_Symbols_FLIST      LIST_Symbols_FLIST_insert (LIST_Symbols_FLIST p, int i, Symbols elt);
void    LIST_Symbols_delpos (LIST_Symbols, int);
LIST_Symbols_FLIST      LIST_Symbols_FLIST_delpos (LIST_Symbols_FLIST p, int pos);
LIST_Symbols  LIST_Symbols_fappend (LIST_Symbols, LIST_Symbols);
LIST_Symbols  LIST_Symbols_fappend1 (LIST_Symbols, Symbols);
LIST_Symbols    LIST_Symbols_fcons(LIST_Symbols, Symbols);
LIST_Symbols    LIST_Symbols_freverse (LIST_Symbols);
struct ITERLIST_Symbols {
        LIST_Symbols    thelist;
        LIST_Symbols_FLIST      cursor;
};
typedef struct ITERLIST_Symbols * ITERLIST_Symbols;
ITERLIST_Symbols ITERLIST_Symbols_create(LIST_Symbols);
void    ITERLIST_Symbols_delete (ITERLIST_Symbols This);
Symbols ITERLIST_Symbols_getcurrentelt(ITERLIST_Symbols);
LIST_Symbols_FLIST      ITERLIST_Symbols_getcurrentsublist (ITERLIST_Symbols);
Symbols ITERLIST_Symbols_iter (ITERLIST_Symbols);
Symbols ITERLIST_Symbols_nth (ITERLIST_Symbols, int);
Symbols ITERLIST_Symbols_reviter (ITERLIST_Symbols);
void    ITERLIST_Symbols_setcursor(ITERLIST_Symbols This, int pos);
void    ITERLIST_Symbols_stepcursor (ITERLIST_Symbols);
void    ITERLIST_Symbols_tolast (ITERLIST_Symbols);
void    ITERLIST_Symbols_assign (ITERLIST_Symbols, Symbols elt);
void    ITERLIST_Symbols_delpos(ITERLIST_Symbols);
void    ITERLIST_Symbols_insertelt(ITERLIST_Symbols cur, Symbols newelt, BOOL before);
void    ITERLIST_Symbols_insertlist(ITERLIST_Symbols cursor, LIST_Symbols inserted, BOOL before);
LIST_Symbols ITERLIST_Symbols_split(ITERLIST_Symbols);
void    ITERLIST_Symbols_insertaux(ITERLIST_Symbols cursor, LIST_Symbols_FLIST extra, BOOL before);
void    LIST_Symbols_FLIST_insertlist(LIST_Symbols_FLIST*, LIST_Symbols_FLIST*, LIST_Symbols_FLIST*, LIST_Symbols_FLIST, BOOL before);
LIST_mirCFG     d_LIST_mirCFG_create (opcode_t);
void    d_LIST_mirCFG_delete (LIST_mirCFG);
LIST_mirCFG  LIST_mirCFG_tl(LIST_mirCFG);
LIST_mirCFG     LIST_mirCFG_create(void);
LIST_mirCFG     LIST_mirCFG_listoneelt (mirCFG);
LIST_mirCFG     LIST_mirCFG_copy (LIST_mirCFG);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_deepcopy (LIST_mirCFG_FLIST p);
LIST_mirCFG     LIST_mirCFG_merge (LIST_mirCFG, LIST_mirCFG);
void    LIST_mirCFG_delete(LIST_mirCFG This);
int     LIST_mirCFG_FLIST_pos (LIST_mirCFG_FLIST p, mirCFG elt);
int     LIST_mirCFG_FLIST_length (LIST_mirCFG_FLIST p);
mirCFG  LIST_mirCFG_last (LIST_mirCFG);
mirCFG  LIST_mirCFG_nth (LIST_mirCFG, int);
mirCFG  LIST_mirCFG_FLIST_nth (LIST_mirCFG_FLIST p, int i);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_index (LIST_mirCFG_FLIST p, int i);
mirCFG  LIST_mirCFG_hd (LIST_mirCFG);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_before (LIST_mirCFG_FLIST p, LIST_mirCFG_FLIST q);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_last (LIST_mirCFG_FLIST p);
void    LIST_mirCFG_printids(LIST_mirCFG This);
void    LIST_mirCFG_FLIST_printids (LIST_mirCFG_FLIST p);
mirCFG  LIST_mirCFG_rightbrother(LIST_mirCFG, mirCFG);
BOOL    LIST_mirCFG_isequal (LIST_mirCFG, LIST_mirCFG);
BOOL    LIST_mirCFG_FLIST_isequal (LIST_mirCFG_FLIST p, LIST_mirCFG_FLIST q);
BOOL    LIST_mirCFG_isfirst (LIST_mirCFG, mirCFG);
BOOL    LIST_mirCFG_islast (LIST_mirCFG, mirCFG);
mirCFG  LIST_mirCFG_leftbrother(LIST_mirCFG, mirCFG);
void    LIST_mirCFG_append (LIST_mirCFG, LIST_mirCFG);
void    LIST_mirCFG_appendelt (LIST_mirCFG, mirCFG);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_make (mirCFG value, LIST_mirCFG_FLIST next);
void    LIST_mirCFG_assign (LIST_mirCFG, mirCFG, int);
void    LIST_mirCFG_cons (LIST_mirCFG, mirCFG);
void    LIST_mirCFG_reverse (LIST_mirCFG);
void    LIST_mirCFG_dreverse (LIST_mirCFG);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_reverse (LIST_mirCFG_FLIST p);
void    LIST_mirCFG_del (LIST_mirCFG, mirCFG);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_delelt (LIST_mirCFG_FLIST p, mirCFG value);
void    LIST_mirCFG_insert(LIST_mirCFG, int i, mirCFG E);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_insert (LIST_mirCFG_FLIST p, int i, mirCFG elt);
void    LIST_mirCFG_delpos (LIST_mirCFG, int);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_delpos (LIST_mirCFG_FLIST p, int pos);
LIST_mirCFG  LIST_mirCFG_fappend (LIST_mirCFG, LIST_mirCFG);
LIST_mirCFG  LIST_mirCFG_fappend1 (LIST_mirCFG, mirCFG);
LIST_mirCFG     LIST_mirCFG_fcons(LIST_mirCFG, mirCFG);
LIST_mirCFG     LIST_mirCFG_freverse (LIST_mirCFG);
struct ITERLIST_mirCFG {
        LIST_mirCFG     thelist;
        LIST_mirCFG_FLIST       cursor;
};
typedef struct ITERLIST_mirCFG * ITERLIST_mirCFG;
ITERLIST_mirCFG ITERLIST_mirCFG_create(LIST_mirCFG);
void    ITERLIST_mirCFG_delete (ITERLIST_mirCFG This);
mirCFG  ITERLIST_mirCFG_getcurrentelt(ITERLIST_mirCFG);
LIST_mirCFG_FLIST       ITERLIST_mirCFG_getcurrentsublist (ITERLIST_mirCFG);
mirCFG  ITERLIST_mirCFG_iter (ITERLIST_mirCFG);
mirCFG  ITERLIST_mirCFG_nth (ITERLIST_mirCFG, int);
mirCFG  ITERLIST_mirCFG_reviter (ITERLIST_mirCFG);
void    ITERLIST_mirCFG_setcursor(ITERLIST_mirCFG This, int pos);
void    ITERLIST_mirCFG_stepcursor (ITERLIST_mirCFG);
void    ITERLIST_mirCFG_tolast (ITERLIST_mirCFG);
void    ITERLIST_mirCFG_assign (ITERLIST_mirCFG, mirCFG elt);
void    ITERLIST_mirCFG_delpos(ITERLIST_mirCFG);
void    ITERLIST_mirCFG_insertelt(ITERLIST_mirCFG cur, mirCFG newelt, BOOL before);
void    ITERLIST_mirCFG_insertlist(ITERLIST_mirCFG cursor, LIST_mirCFG inserted, BOOL before);
LIST_mirCFG ITERLIST_mirCFG_split(ITERLIST_mirCFG);
void    ITERLIST_mirCFG_insertaux(ITERLIST_mirCFG cursor, LIST_mirCFG_FLIST extra, BOOL before);
void    LIST_mirCFG_FLIST_insertlist(LIST_mirCFG_FLIST*, LIST_mirCFG_FLIST*, LIST_mirCFG_FLIST*, LIST_mirCFG_FLIST, BOOL before);
LIST_mirTYPE    LIST_mirTYPE_create(void);
int     LIST_mirTYPE_FLIST_pos (LIST_mirTYPE_FLIST p, mirTYPE elt);
int     LIST_mirTYPE_FLIST_length (LIST_mirTYPE_FLIST p);
mirTYPE LIST_mirTYPE_last (LIST_mirTYPE);
mirTYPE LIST_mirTYPE_nth (LIST_mirTYPE, int);
mirTYPE LIST_mirTYPE_FLIST_nth (LIST_mirTYPE_FLIST p, int i);
LIST_mirTYPE_FLIST      LIST_mirTYPE_FLIST_index (LIST_mirTYPE_FLIST p, int i);
mirTYPE LIST_mirTYPE_hd (LIST_mirTYPE);
LIST_mirTYPE_FLIST      LIST_mirTYPE_FLIST_before (LIST_mirTYPE_FLIST p, LIST_mirTYPE_FLIST q);
LIST_mirTYPE_FLIST      LIST_mirTYPE_FLIST_last (LIST_mirTYPE_FLIST p);
void    LIST_mirTYPE_printids(LIST_mirTYPE This);
void    LIST_mirTYPE_FLIST_printids (LIST_mirTYPE_FLIST p);
mirTYPE LIST_mirTYPE_rightbrother(LIST_mirTYPE, mirTYPE);
void    LIST_mirTYPE_append (LIST_mirTYPE, LIST_mirTYPE);
void    LIST_mirTYPE_appendelt (LIST_mirTYPE, mirTYPE);
LIST_mirTYPE_FLIST      LIST_mirTYPE_FLIST_make (mirTYPE value, LIST_mirTYPE_FLIST next);
struct ITERLIST_mirTYPE {
        LIST_mirTYPE    thelist;
        LIST_mirTYPE_FLIST      cursor;
};
typedef struct ITERLIST_mirTYPE * ITERLIST_mirTYPE;
ITERLIST_mirTYPE ITERLIST_mirTYPE_create(LIST_mirTYPE);
void    ITERLIST_mirTYPE_delete (ITERLIST_mirTYPE This);
mirTYPE ITERLIST_mirTYPE_getcurrentelt(ITERLIST_mirTYPE);
LIST_mirTYPE_FLIST      ITERLIST_mirTYPE_getcurrentsublist (ITERLIST_mirTYPE);
mirTYPE ITERLIST_mirTYPE_iter (ITERLIST_mirTYPE);
mirTYPE ITERLIST_mirTYPE_nth (ITERLIST_mirTYPE, int);
mirTYPE ITERLIST_mirTYPE_reviter (ITERLIST_mirTYPE);
void    ITERLIST_mirTYPE_setcursor(ITERLIST_mirTYPE This, int pos);
void    ITERLIST_mirTYPE_stepcursor (ITERLIST_mirTYPE);
void    ITERLIST_mirTYPE_tolast (ITERLIST_mirTYPE);
LIST_mirSection LIST_mirSection_create(void);
int     LIST_mirSection_FLIST_pos (LIST_mirSection_FLIST p, mirSection elt);
int     LIST_mirSection_FLIST_length (LIST_mirSection_FLIST p);
mirSection      LIST_mirSection_last (LIST_mirSection);
mirSection      LIST_mirSection_nth (LIST_mirSection, int);
mirSection      LIST_mirSection_FLIST_nth (LIST_mirSection_FLIST p, int i);
LIST_mirSection_FLIST   LIST_mirSection_FLIST_index (LIST_mirSection_FLIST p, int i);
mirSection      LIST_mirSection_hd (LIST_mirSection);
LIST_mirSection_FLIST   LIST_mirSection_FLIST_before (LIST_mirSection_FLIST p, LIST_mirSection_FLIST q);
LIST_mirSection_FLIST   LIST_mirSection_FLIST_last (LIST_mirSection_FLIST p);
void    LIST_mirSection_printids(LIST_mirSection This);
void    LIST_mirSection_FLIST_printids (LIST_mirSection_FLIST p);
mirSection      LIST_mirSection_rightbrother(LIST_mirSection, mirSection);
void    LIST_mirSection_append (LIST_mirSection, LIST_mirSection);
void    LIST_mirSection_appendelt (LIST_mirSection, mirSection);
LIST_mirSection_FLIST   LIST_mirSection_FLIST_make (mirSection value, LIST_mirSection_FLIST next);
struct ITERLIST_mirSection {
        LIST_mirSection thelist;
        LIST_mirSection_FLIST   cursor;
};
typedef struct ITERLIST_mirSection * ITERLIST_mirSection;
ITERLIST_mirSection ITERLIST_mirSection_create(LIST_mirSection);
void    ITERLIST_mirSection_delete (ITERLIST_mirSection This);
mirSection      ITERLIST_mirSection_getcurrentelt(ITERLIST_mirSection);
LIST_mirSection_FLIST   ITERLIST_mirSection_getcurrentsublist (ITERLIST_mirSection);
mirSection      ITERLIST_mirSection_iter (ITERLIST_mirSection);
mirSection      ITERLIST_mirSection_nth (ITERLIST_mirSection, int);
mirSection      ITERLIST_mirSection_reviter (ITERLIST_mirSection);
void    ITERLIST_mirSection_setcursor(ITERLIST_mirSection This, int pos);
void    ITERLIST_mirSection_stepcursor (ITERLIST_mirSection);
void    ITERLIST_mirSection_tolast (ITERLIST_mirSection);
LIST_mirRegister        LIST_mirRegister_create(void);
int     LIST_mirRegister_FLIST_pos (LIST_mirRegister_FLIST p, mirRegister elt);
int     LIST_mirRegister_FLIST_length (LIST_mirRegister_FLIST p);
mirRegister     LIST_mirRegister_last (LIST_mirRegister);
mirRegister     LIST_mirRegister_nth (LIST_mirRegister, int);
mirRegister     LIST_mirRegister_FLIST_nth (LIST_mirRegister_FLIST p, int i);
LIST_mirRegister_FLIST  LIST_mirRegister_FLIST_index (LIST_mirRegister_FLIST p, int i);
mirRegister     LIST_mirRegister_hd (LIST_mirRegister);
LIST_mirRegister_FLIST  LIST_mirRegister_FLIST_before (LIST_mirRegister_FLIST p, LIST_mirRegister_FLIST q);
LIST_mirRegister_FLIST  LIST_mirRegister_FLIST_last (LIST_mirRegister_FLIST p);
void    LIST_mirRegister_printids(LIST_mirRegister This);
void    LIST_mirRegister_FLIST_printids (LIST_mirRegister_FLIST p);
mirRegister     LIST_mirRegister_rightbrother(LIST_mirRegister, mirRegister);
struct ITERLIST_mirRegister {
        LIST_mirRegister        thelist;
        LIST_mirRegister_FLIST  cursor;
};
typedef struct ITERLIST_mirRegister * ITERLIST_mirRegister;
ITERLIST_mirRegister ITERLIST_mirRegister_create(LIST_mirRegister);
void    ITERLIST_mirRegister_delete (ITERLIST_mirRegister This);
mirRegister     ITERLIST_mirRegister_getcurrentelt(ITERLIST_mirRegister);
LIST_mirRegister_FLIST  ITERLIST_mirRegister_getcurrentsublist (ITERLIST_mirRegister);
mirRegister     ITERLIST_mirRegister_iter (ITERLIST_mirRegister);
mirRegister     ITERLIST_mirRegister_nth (ITERLIST_mirRegister, int);
mirRegister     ITERLIST_mirRegister_reviter (ITERLIST_mirRegister);
void    ITERLIST_mirRegister_setcursor(ITERLIST_mirRegister This, int pos);
void    ITERLIST_mirRegister_stepcursor (ITERLIST_mirRegister);
void    ITERLIST_mirRegister_tolast (ITERLIST_mirRegister);
LIST_mirProcGlobal  LIST_mirProcGlobal_tl(LIST_mirProcGlobal);
LIST_mirProcGlobal      LIST_mirProcGlobal_create(void);
LIST_mirProcGlobal      LIST_mirProcGlobal_listoneelt (mirProcGlobal);
LIST_mirProcGlobal      LIST_mirProcGlobal_copy (LIST_mirProcGlobal);
LIST_mirProcGlobal_FLIST        LIST_mirProcGlobal_FLIST_deepcopy (LIST_mirProcGlobal_FLIST p);
LIST_mirProcGlobal      LIST_mirProcGlobal_merge (LIST_mirProcGlobal, LIST_mirProcGlobal);
void    LIST_mirProcGlobal_delete(LIST_mirProcGlobal This);
int     LIST_mirProcGlobal_FLIST_pos (LIST_mirProcGlobal_FLIST p, mirProcGlobal elt);
int     LIST_mirProcGlobal_FLIST_length (LIST_mirProcGlobal_FLIST p);
mirProcGlobal   LIST_mirProcGlobal_last (LIST_mirProcGlobal);
mirProcGlobal   LIST_mirProcGlobal_nth (LIST_mirProcGlobal, int);
mirProcGlobal   LIST_mirProcGlobal_FLIST_nth (LIST_mirProcGlobal_FLIST p, int i);
LIST_mirProcGlobal_FLIST        LIST_mirProcGlobal_FLIST_index (LIST_mirProcGlobal_FLIST p, int i);
mirProcGlobal   LIST_mirProcGlobal_hd (LIST_mirProcGlobal);
LIST_mirProcGlobal_FLIST        LIST_mirProcGlobal_FLIST_before (LIST_mirProcGlobal_FLIST p, LIST_mirProcGlobal_FLIST q);
LIST_mirProcGlobal_FLIST        LIST_mirProcGlobal_FLIST_last (LIST_mirProcGlobal_FLIST p);
void    LIST_mirProcGlobal_printids(LIST_mirProcGlobal This);
void    LIST_mirProcGlobal_FLIST_printids (LIST_mirProcGlobal_FLIST p);
mirProcGlobal   LIST_mirProcGlobal_rightbrother(LIST_mirProcGlobal, mirProcGlobal);
BOOL    LIST_mirProcGlobal_isequal (LIST_mirProcGlobal, LIST_mirProcGlobal);
BOOL    LIST_mirProcGlobal_FLIST_isequal (LIST_mirProcGlobal_FLIST p, LIST_mirProcGlobal_FLIST q);
BOOL    LIST_mirProcGlobal_isfirst (LIST_mirProcGlobal, mirProcGlobal);
BOOL    LIST_mirProcGlobal_islast (LIST_mirProcGlobal, mirProcGlobal);
mirProcGlobal   LIST_mirProcGlobal_leftbrother(LIST_mirProcGlobal, mirProcGlobal);
void    LIST_mirProcGlobal_append (LIST_mirProcGlobal, LIST_mirProcGlobal);
void    LIST_mirProcGlobal_appendelt (LIST_mirProcGlobal, mirProcGlobal);
LIST_mirProcGlobal_FLIST        LIST_mirProcGlobal_FLIST_make (mirProcGlobal value, LIST_mirProcGlobal_FLIST next);
void    LIST_mirProcGlobal_assign (LIST_mirProcGlobal, mirProcGlobal, int);
void    LIST_mirProcGlobal_cons (LIST_mirProcGlobal, mirProcGlobal);
void    LIST_mirProcGlobal_reverse (LIST_mirProcGlobal);
void    LIST_mirProcGlobal_dreverse (LIST_mirProcGlobal);
LIST_mirProcGlobal_FLIST        LIST_mirProcGlobal_FLIST_reverse (LIST_mirProcGlobal_FLIST p);
void    LIST_mirProcGlobal_del (LIST_mirProcGlobal, mirProcGlobal);
LIST_mirProcGlobal_FLIST        LIST_mirProcGlobal_FLIST_delelt (LIST_mirProcGlobal_FLIST p, mirProcGlobal value);
void    LIST_mirProcGlobal_insert(LIST_mirProcGlobal, int i, mirProcGlobal E);
LIST_mirProcGlobal_FLIST        LIST_mirProcGlobal_FLIST_insert (LIST_mirProcGlobal_FLIST p, int i, mirProcGlobal elt);
void    LIST_mirProcGlobal_delpos (LIST_mirProcGlobal, int);
LIST_mirProcGlobal_FLIST        LIST_mirProcGlobal_FLIST_delpos (LIST_mirProcGlobal_FLIST p, int pos);
LIST_mirProcGlobal  LIST_mirProcGlobal_fappend (LIST_mirProcGlobal, LIST_mirProcGlobal);
LIST_mirProcGlobal  LIST_mirProcGlobal_fappend1 (LIST_mirProcGlobal, mirProcGlobal);
LIST_mirProcGlobal      LIST_mirProcGlobal_fcons(LIST_mirProcGlobal, mirProcGlobal);
LIST_mirProcGlobal      LIST_mirProcGlobal_freverse (LIST_mirProcGlobal);
struct ITERLIST_mirProcGlobal {
        LIST_mirProcGlobal      thelist;
        LIST_mirProcGlobal_FLIST        cursor;
};
typedef struct ITERLIST_mirProcGlobal * ITERLIST_mirProcGlobal;
ITERLIST_mirProcGlobal ITERLIST_mirProcGlobal_create(LIST_mirProcGlobal);
void    ITERLIST_mirProcGlobal_delete (ITERLIST_mirProcGlobal This);
mirProcGlobal   ITERLIST_mirProcGlobal_getcurrentelt(ITERLIST_mirProcGlobal);
LIST_mirProcGlobal_FLIST        ITERLIST_mirProcGlobal_getcurrentsublist (ITERLIST_mirProcGlobal);
mirProcGlobal   ITERLIST_mirProcGlobal_iter (ITERLIST_mirProcGlobal);
mirProcGlobal   ITERLIST_mirProcGlobal_nth (ITERLIST_mirProcGlobal, int);
mirProcGlobal   ITERLIST_mirProcGlobal_reviter (ITERLIST_mirProcGlobal);
void    ITERLIST_mirProcGlobal_setcursor(ITERLIST_mirProcGlobal This, int pos);
void    ITERLIST_mirProcGlobal_stepcursor (ITERLIST_mirProcGlobal);
void    ITERLIST_mirProcGlobal_tolast (ITERLIST_mirProcGlobal);
void    ITERLIST_mirProcGlobal_assign (ITERLIST_mirProcGlobal, mirProcGlobal elt);
void    ITERLIST_mirProcGlobal_delpos(ITERLIST_mirProcGlobal);
void    ITERLIST_mirProcGlobal_insertelt(ITERLIST_mirProcGlobal cur, mirProcGlobal newelt, BOOL before);
void    ITERLIST_mirProcGlobal_insertlist(ITERLIST_mirProcGlobal cursor, LIST_mirProcGlobal inserted, BOOL before);
LIST_mirProcGlobal ITERLIST_mirProcGlobal_split(ITERLIST_mirProcGlobal);
void    ITERLIST_mirProcGlobal_insertaux(ITERLIST_mirProcGlobal cursor, LIST_mirProcGlobal_FLIST extra, BOOL before);
void    LIST_mirProcGlobal_FLIST_insertlist(LIST_mirProcGlobal_FLIST*, LIST_mirProcGlobal_FLIST*, LIST_mirProcGlobal_FLIST*, LIST_mirProcGlobal_FLIST, BOOL before);
LIST_mirPlaced  LIST_mirPlaced_create(void);
int     LIST_mirPlaced_FLIST_pos (LIST_mirPlaced_FLIST p, mirPlaced elt);
int     LIST_mirPlaced_FLIST_length (LIST_mirPlaced_FLIST p);
mirPlaced       LIST_mirPlaced_last (LIST_mirPlaced);
mirPlaced       LIST_mirPlaced_nth (LIST_mirPlaced, int);
mirPlaced       LIST_mirPlaced_FLIST_nth (LIST_mirPlaced_FLIST p, int i);
LIST_mirPlaced_FLIST    LIST_mirPlaced_FLIST_index (LIST_mirPlaced_FLIST p, int i);
mirPlaced       LIST_mirPlaced_hd (LIST_mirPlaced);
LIST_mirPlaced_FLIST    LIST_mirPlaced_FLIST_before (LIST_mirPlaced_FLIST p, LIST_mirPlaced_FLIST q);
LIST_mirPlaced_FLIST    LIST_mirPlaced_FLIST_last (LIST_mirPlaced_FLIST p);
void    LIST_mirPlaced_printids(LIST_mirPlaced This);
void    LIST_mirPlaced_FLIST_printids (LIST_mirPlaced_FLIST p);
mirPlaced       LIST_mirPlaced_rightbrother(LIST_mirPlaced, mirPlaced);
struct ITERLIST_mirPlaced {
        LIST_mirPlaced  thelist;
        LIST_mirPlaced_FLIST    cursor;
};
typedef struct ITERLIST_mirPlaced * ITERLIST_mirPlaced;
ITERLIST_mirPlaced ITERLIST_mirPlaced_create(LIST_mirPlaced);
void    ITERLIST_mirPlaced_delete (ITERLIST_mirPlaced This);
mirPlaced       ITERLIST_mirPlaced_getcurrentelt(ITERLIST_mirPlaced);
LIST_mirPlaced_FLIST    ITERLIST_mirPlaced_getcurrentsublist (ITERLIST_mirPlaced);
mirPlaced       ITERLIST_mirPlaced_iter (ITERLIST_mirPlaced);
mirPlaced       ITERLIST_mirPlaced_nth (ITERLIST_mirPlaced, int);
mirPlaced       ITERLIST_mirPlaced_reviter (ITERLIST_mirPlaced);
void    ITERLIST_mirPlaced_setcursor(ITERLIST_mirPlaced This, int pos);
void    ITERLIST_mirPlaced_stepcursor (ITERLIST_mirPlaced);
void    ITERLIST_mirPlaced_tolast (ITERLIST_mirPlaced);
LIST_mirDataGlobal      LIST_mirDataGlobal_create(void);
int     LIST_mirDataGlobal_FLIST_pos (LIST_mirDataGlobal_FLIST p, mirDataGlobal elt);
int     LIST_mirDataGlobal_FLIST_length (LIST_mirDataGlobal_FLIST p);
mirDataGlobal   LIST_mirDataGlobal_last (LIST_mirDataGlobal);
mirDataGlobal   LIST_mirDataGlobal_nth (LIST_mirDataGlobal, int);
mirDataGlobal   LIST_mirDataGlobal_FLIST_nth (LIST_mirDataGlobal_FLIST p, int i);
LIST_mirDataGlobal_FLIST        LIST_mirDataGlobal_FLIST_index (LIST_mirDataGlobal_FLIST p, int i);
mirDataGlobal   LIST_mirDataGlobal_hd (LIST_mirDataGlobal);
LIST_mirDataGlobal_FLIST        LIST_mirDataGlobal_FLIST_before (LIST_mirDataGlobal_FLIST p, LIST_mirDataGlobal_FLIST q);
LIST_mirDataGlobal_FLIST        LIST_mirDataGlobal_FLIST_last (LIST_mirDataGlobal_FLIST p);
void    LIST_mirDataGlobal_printids(LIST_mirDataGlobal This);
void    LIST_mirDataGlobal_FLIST_printids (LIST_mirDataGlobal_FLIST p);
mirDataGlobal   LIST_mirDataGlobal_rightbrother(LIST_mirDataGlobal, mirDataGlobal);
void    LIST_mirDataGlobal_append (LIST_mirDataGlobal, LIST_mirDataGlobal);
void    LIST_mirDataGlobal_appendelt (LIST_mirDataGlobal, mirDataGlobal);
LIST_mirDataGlobal_FLIST        LIST_mirDataGlobal_FLIST_make (mirDataGlobal value, LIST_mirDataGlobal_FLIST next);
struct ITERLIST_mirDataGlobal {
        LIST_mirDataGlobal      thelist;
        LIST_mirDataGlobal_FLIST        cursor;
};
typedef struct ITERLIST_mirDataGlobal * ITERLIST_mirDataGlobal;
ITERLIST_mirDataGlobal ITERLIST_mirDataGlobal_create(LIST_mirDataGlobal);
void    ITERLIST_mirDataGlobal_delete (ITERLIST_mirDataGlobal This);
mirDataGlobal   ITERLIST_mirDataGlobal_getcurrentelt(ITERLIST_mirDataGlobal);
LIST_mirDataGlobal_FLIST        ITERLIST_mirDataGlobal_getcurrentsublist (ITERLIST_mirDataGlobal);
mirDataGlobal   ITERLIST_mirDataGlobal_iter (ITERLIST_mirDataGlobal);
mirDataGlobal   ITERLIST_mirDataGlobal_nth (ITERLIST_mirDataGlobal, int);
mirDataGlobal   ITERLIST_mirDataGlobal_reviter (ITERLIST_mirDataGlobal);
void    ITERLIST_mirDataGlobal_setcursor(ITERLIST_mirDataGlobal This, int pos);
void    ITERLIST_mirDataGlobal_stepcursor (ITERLIST_mirDataGlobal);
void    ITERLIST_mirDataGlobal_tolast (ITERLIST_mirDataGlobal);
LIST_mirBasicBlock  LIST_mirBasicBlock_tl(LIST_mirBasicBlock);
LIST_mirBasicBlock      LIST_mirBasicBlock_create(void);
LIST_mirBasicBlock      LIST_mirBasicBlock_listoneelt (mirBasicBlock);
LIST_mirBasicBlock      LIST_mirBasicBlock_copy (LIST_mirBasicBlock);
LIST_mirBasicBlock_FLIST        LIST_mirBasicBlock_FLIST_deepcopy (LIST_mirBasicBlock_FLIST p);
LIST_mirBasicBlock      LIST_mirBasicBlock_merge (LIST_mirBasicBlock, LIST_mirBasicBlock);
void    LIST_mirBasicBlock_delete(LIST_mirBasicBlock This);
int     LIST_mirBasicBlock_FLIST_pos (LIST_mirBasicBlock_FLIST p, mirBasicBlock elt);
int     LIST_mirBasicBlock_FLIST_length (LIST_mirBasicBlock_FLIST p);
mirBasicBlock   LIST_mirBasicBlock_last (LIST_mirBasicBlock);
mirBasicBlock   LIST_mirBasicBlock_nth (LIST_mirBasicBlock, int);
mirBasicBlock   LIST_mirBasicBlock_FLIST_nth (LIST_mirBasicBlock_FLIST p, int i);
LIST_mirBasicBlock_FLIST        LIST_mirBasicBlock_FLIST_index (LIST_mirBasicBlock_FLIST p, int i);
mirBasicBlock   LIST_mirBasicBlock_hd (LIST_mirBasicBlock);
LIST_mirBasicBlock_FLIST        LIST_mirBasicBlock_FLIST_before (LIST_mirBasicBlock_FLIST p, LIST_mirBasicBlock_FLIST q);
LIST_mirBasicBlock_FLIST        LIST_mirBasicBlock_FLIST_last (LIST_mirBasicBlock_FLIST p);
void    LIST_mirBasicBlock_printids(LIST_mirBasicBlock This);
void    LIST_mirBasicBlock_FLIST_printids (LIST_mirBasicBlock_FLIST p);
mirBasicBlock   LIST_mirBasicBlock_rightbrother(LIST_mirBasicBlock, mirBasicBlock);
BOOL    LIST_mirBasicBlock_isequal (LIST_mirBasicBlock, LIST_mirBasicBlock);
BOOL    LIST_mirBasicBlock_FLIST_isequal (LIST_mirBasicBlock_FLIST p, LIST_mirBasicBlock_FLIST q);
BOOL    LIST_mirBasicBlock_isfirst (LIST_mirBasicBlock, mirBasicBlock);
BOOL    LIST_mirBasicBlock_islast (LIST_mirBasicBlock, mirBasicBlock);
mirBasicBlock   LIST_mirBasicBlock_leftbrother(LIST_mirBasicBlock, mirBasicBlock);
void    LIST_mirBasicBlock_append (LIST_mirBasicBlock, LIST_mirBasicBlock);
void    LIST_mirBasicBlock_appendelt (LIST_mirBasicBlock, mirBasicBlock);
LIST_mirBasicBlock_FLIST        LIST_mirBasicBlock_FLIST_make (mirBasicBlock value, LIST_mirBasicBlock_FLIST next);
void    LIST_mirBasicBlock_assign (LIST_mirBasicBlock, mirBasicBlock, int);
void    LIST_mirBasicBlock_cons (LIST_mirBasicBlock, mirBasicBlock);
void    LIST_mirBasicBlock_reverse (LIST_mirBasicBlock);
void    LIST_mirBasicBlock_dreverse (LIST_mirBasicBlock);
LIST_mirBasicBlock_FLIST        LIST_mirBasicBlock_FLIST_reverse (LIST_mirBasicBlock_FLIST p);
void    LIST_mirBasicBlock_del (LIST_mirBasicBlock, mirBasicBlock);
LIST_mirBasicBlock_FLIST        LIST_mirBasicBlock_FLIST_delelt (LIST_mirBasicBlock_FLIST p, mirBasicBlock value);
void    LIST_mirBasicBlock_insert(LIST_mirBasicBlock, int i, mirBasicBlock E);
LIST_mirBasicBlock_FLIST        LIST_mirBasicBlock_FLIST_insert (LIST_mirBasicBlock_FLIST p, int i, mirBasicBlock elt);
void    LIST_mirBasicBlock_delpos (LIST_mirBasicBlock, int);
LIST_mirBasicBlock_FLIST        LIST_mirBasicBlock_FLIST_delpos (LIST_mirBasicBlock_FLIST p, int pos);
LIST_mirBasicBlock  LIST_mirBasicBlock_fappend (LIST_mirBasicBlock, LIST_mirBasicBlock);
LIST_mirBasicBlock  LIST_mirBasicBlock_fappend1 (LIST_mirBasicBlock, mirBasicBlock);
LIST_mirBasicBlock      LIST_mirBasicBlock_fcons(LIST_mirBasicBlock, mirBasicBlock);
LIST_mirBasicBlock      LIST_mirBasicBlock_freverse (LIST_mirBasicBlock);
struct ITERLIST_mirBasicBlock {
        LIST_mirBasicBlock      thelist;
        LIST_mirBasicBlock_FLIST        cursor;
};
typedef struct ITERLIST_mirBasicBlock * ITERLIST_mirBasicBlock;
ITERLIST_mirBasicBlock ITERLIST_mirBasicBlock_create(LIST_mirBasicBlock);
void    ITERLIST_mirBasicBlock_delete (ITERLIST_mirBasicBlock This);
mirBasicBlock   ITERLIST_mirBasicBlock_getcurrentelt(ITERLIST_mirBasicBlock);
LIST_mirBasicBlock_FLIST        ITERLIST_mirBasicBlock_getcurrentsublist (ITERLIST_mirBasicBlock);
mirBasicBlock   ITERLIST_mirBasicBlock_iter (ITERLIST_mirBasicBlock);
mirBasicBlock   ITERLIST_mirBasicBlock_nth (ITERLIST_mirBasicBlock, int);
mirBasicBlock   ITERLIST_mirBasicBlock_reviter (ITERLIST_mirBasicBlock);
void    ITERLIST_mirBasicBlock_setcursor(ITERLIST_mirBasicBlock This, int pos);
void    ITERLIST_mirBasicBlock_stepcursor (ITERLIST_mirBasicBlock);
void    ITERLIST_mirBasicBlock_tolast (ITERLIST_mirBasicBlock);
void    ITERLIST_mirBasicBlock_assign (ITERLIST_mirBasicBlock, mirBasicBlock elt);
void    ITERLIST_mirBasicBlock_delpos(ITERLIST_mirBasicBlock);
void    ITERLIST_mirBasicBlock_insertelt(ITERLIST_mirBasicBlock cur, mirBasicBlock newelt, BOOL before);
void    ITERLIST_mirBasicBlock_insertlist(ITERLIST_mirBasicBlock cursor, LIST_mirBasicBlock inserted, BOOL before);
LIST_mirBasicBlock ITERLIST_mirBasicBlock_split(ITERLIST_mirBasicBlock);
void    ITERLIST_mirBasicBlock_insertaux(ITERLIST_mirBasicBlock cursor, LIST_mirBasicBlock_FLIST extra, BOOL before);
void    LIST_mirBasicBlock_FLIST_insertlist(LIST_mirBasicBlock_FLIST*, LIST_mirBasicBlock_FLIST*, LIST_mirBasicBlock_FLIST*, LIST_mirBasicBlock_FLIST, BOOL before);
LIST_mirLocal  LIST_mirLocal_tl(LIST_mirLocal);
LIST_mirLocal   LIST_mirLocal_create(void);
LIST_mirLocal   LIST_mirLocal_listoneelt (mirLocal);
LIST_mirLocal   LIST_mirLocal_copy (LIST_mirLocal);
LIST_mirLocal_FLIST     LIST_mirLocal_FLIST_deepcopy (LIST_mirLocal_FLIST p);
LIST_mirLocal   LIST_mirLocal_merge (LIST_mirLocal, LIST_mirLocal);
void    LIST_mirLocal_delete(LIST_mirLocal This);
int     LIST_mirLocal_FLIST_pos (LIST_mirLocal_FLIST p, mirLocal elt);
int     LIST_mirLocal_FLIST_length (LIST_mirLocal_FLIST p);
mirLocal        LIST_mirLocal_last (LIST_mirLocal);
mirLocal        LIST_mirLocal_nth (LIST_mirLocal, int);
mirLocal        LIST_mirLocal_FLIST_nth (LIST_mirLocal_FLIST p, int i);
LIST_mirLocal_FLIST     LIST_mirLocal_FLIST_index (LIST_mirLocal_FLIST p, int i);
mirLocal        LIST_mirLocal_hd (LIST_mirLocal);
LIST_mirLocal_FLIST     LIST_mirLocal_FLIST_before (LIST_mirLocal_FLIST p, LIST_mirLocal_FLIST q);
LIST_mirLocal_FLIST     LIST_mirLocal_FLIST_last (LIST_mirLocal_FLIST p);
void    LIST_mirLocal_printids(LIST_mirLocal This);
void    LIST_mirLocal_FLIST_printids (LIST_mirLocal_FLIST p);
mirLocal        LIST_mirLocal_rightbrother(LIST_mirLocal, mirLocal);
BOOL    LIST_mirLocal_isequal (LIST_mirLocal, LIST_mirLocal);
BOOL    LIST_mirLocal_FLIST_isequal (LIST_mirLocal_FLIST p, LIST_mirLocal_FLIST q);
BOOL    LIST_mirLocal_isfirst (LIST_mirLocal, mirLocal);
BOOL    LIST_mirLocal_islast (LIST_mirLocal, mirLocal);
mirLocal        LIST_mirLocal_leftbrother(LIST_mirLocal, mirLocal);
void    LIST_mirLocal_append (LIST_mirLocal, LIST_mirLocal);
void    LIST_mirLocal_appendelt (LIST_mirLocal, mirLocal);
LIST_mirLocal_FLIST     LIST_mirLocal_FLIST_make (mirLocal value, LIST_mirLocal_FLIST next);
void    LIST_mirLocal_assign (LIST_mirLocal, mirLocal, int);
void    LIST_mirLocal_cons (LIST_mirLocal, mirLocal);
void    LIST_mirLocal_reverse (LIST_mirLocal);
void    LIST_mirLocal_dreverse (LIST_mirLocal);
LIST_mirLocal_FLIST     LIST_mirLocal_FLIST_reverse (LIST_mirLocal_FLIST p);
void    LIST_mirLocal_del (LIST_mirLocal, mirLocal);
LIST_mirLocal_FLIST     LIST_mirLocal_FLIST_delelt (LIST_mirLocal_FLIST p, mirLocal value);
void    LIST_mirLocal_insert(LIST_mirLocal, int i, mirLocal E);
LIST_mirLocal_FLIST     LIST_mirLocal_FLIST_insert (LIST_mirLocal_FLIST p, int i, mirLocal elt);
void    LIST_mirLocal_delpos (LIST_mirLocal, int);
LIST_mirLocal_FLIST     LIST_mirLocal_FLIST_delpos (LIST_mirLocal_FLIST p, int pos);
LIST_mirLocal  LIST_mirLocal_fappend (LIST_mirLocal, LIST_mirLocal);
LIST_mirLocal  LIST_mirLocal_fappend1 (LIST_mirLocal, mirLocal);
LIST_mirLocal   LIST_mirLocal_fcons(LIST_mirLocal, mirLocal);
LIST_mirLocal   LIST_mirLocal_freverse (LIST_mirLocal);
struct ITERLIST_mirLocal {
        LIST_mirLocal   thelist;
        LIST_mirLocal_FLIST     cursor;
};
typedef struct ITERLIST_mirLocal * ITERLIST_mirLocal;
ITERLIST_mirLocal ITERLIST_mirLocal_create(LIST_mirLocal);
void    ITERLIST_mirLocal_delete (ITERLIST_mirLocal This);
mirLocal        ITERLIST_mirLocal_getcurrentelt(ITERLIST_mirLocal);
LIST_mirLocal_FLIST     ITERLIST_mirLocal_getcurrentsublist (ITERLIST_mirLocal);
mirLocal        ITERLIST_mirLocal_iter (ITERLIST_mirLocal);
mirLocal        ITERLIST_mirLocal_nth (ITERLIST_mirLocal, int);
mirLocal        ITERLIST_mirLocal_reviter (ITERLIST_mirLocal);
void    ITERLIST_mirLocal_setcursor(ITERLIST_mirLocal This, int pos);
void    ITERLIST_mirLocal_stepcursor (ITERLIST_mirLocal);
void    ITERLIST_mirLocal_tolast (ITERLIST_mirLocal);
void    ITERLIST_mirLocal_assign (ITERLIST_mirLocal, mirLocal elt);
void    ITERLIST_mirLocal_delpos(ITERLIST_mirLocal);
void    ITERLIST_mirLocal_insertelt(ITERLIST_mirLocal cur, mirLocal newelt, BOOL before);
void    ITERLIST_mirLocal_insertlist(ITERLIST_mirLocal cursor, LIST_mirLocal inserted, BOOL before);
LIST_mirLocal ITERLIST_mirLocal_split(ITERLIST_mirLocal);
void    ITERLIST_mirLocal_insertaux(ITERLIST_mirLocal cursor, LIST_mirLocal_FLIST extra, BOOL before);
void    LIST_mirLocal_FLIST_insertlist(LIST_mirLocal_FLIST*, LIST_mirLocal_FLIST*, LIST_mirLocal_FLIST*, LIST_mirLocal_FLIST, BOOL before);
LIST_mirCase  LIST_mirCase_tl(LIST_mirCase);
LIST_mirCase    LIST_mirCase_create(void);
LIST_mirCase    LIST_mirCase_listoneelt (mirCase);
LIST_mirCase    LIST_mirCase_copy (LIST_mirCase);
LIST_mirCase_FLIST      LIST_mirCase_FLIST_deepcopy (LIST_mirCase_FLIST p);
LIST_mirCase    LIST_mirCase_merge (LIST_mirCase, LIST_mirCase);
void    LIST_mirCase_delete(LIST_mirCase This);
int     LIST_mirCase_FLIST_pos (LIST_mirCase_FLIST p, mirCase elt);
int     LIST_mirCase_FLIST_length (LIST_mirCase_FLIST p);
mirCase LIST_mirCase_last (LIST_mirCase);
mirCase LIST_mirCase_nth (LIST_mirCase, int);
mirCase LIST_mirCase_FLIST_nth (LIST_mirCase_FLIST p, int i);
LIST_mirCase_FLIST      LIST_mirCase_FLIST_index (LIST_mirCase_FLIST p, int i);
mirCase LIST_mirCase_hd (LIST_mirCase);
LIST_mirCase_FLIST      LIST_mirCase_FLIST_before (LIST_mirCase_FLIST p, LIST_mirCase_FLIST q);
LIST_mirCase_FLIST      LIST_mirCase_FLIST_last (LIST_mirCase_FLIST p);
void    LIST_mirCase_printids(LIST_mirCase This);
void    LIST_mirCase_FLIST_printids (LIST_mirCase_FLIST p);
mirCase LIST_mirCase_rightbrother(LIST_mirCase, mirCase);
BOOL    LIST_mirCase_isequal (LIST_mirCase, LIST_mirCase);
BOOL    LIST_mirCase_FLIST_isequal (LIST_mirCase_FLIST p, LIST_mirCase_FLIST q);
BOOL    LIST_mirCase_isfirst (LIST_mirCase, mirCase);
BOOL    LIST_mirCase_islast (LIST_mirCase, mirCase);
mirCase LIST_mirCase_leftbrother(LIST_mirCase, mirCase);
void    LIST_mirCase_append (LIST_mirCase, LIST_mirCase);
void    LIST_mirCase_appendelt (LIST_mirCase, mirCase);
LIST_mirCase_FLIST      LIST_mirCase_FLIST_make (mirCase value, LIST_mirCase_FLIST next);
void    LIST_mirCase_assign (LIST_mirCase, mirCase, int);
void    LIST_mirCase_cons (LIST_mirCase, mirCase);
void    LIST_mirCase_reverse (LIST_mirCase);
void    LIST_mirCase_dreverse (LIST_mirCase);
LIST_mirCase_FLIST      LIST_mirCase_FLIST_reverse (LIST_mirCase_FLIST p);
void    LIST_mirCase_del (LIST_mirCase, mirCase);
LIST_mirCase_FLIST      LIST_mirCase_FLIST_delelt (LIST_mirCase_FLIST p, mirCase value);
void    LIST_mirCase_insert(LIST_mirCase, int i, mirCase E);
LIST_mirCase_FLIST      LIST_mirCase_FLIST_insert (LIST_mirCase_FLIST p, int i, mirCase elt);
void    LIST_mirCase_delpos (LIST_mirCase, int);
LIST_mirCase_FLIST      LIST_mirCase_FLIST_delpos (LIST_mirCase_FLIST p, int pos);
LIST_mirCase  LIST_mirCase_fappend (LIST_mirCase, LIST_mirCase);
LIST_mirCase  LIST_mirCase_fappend1 (LIST_mirCase, mirCase);
LIST_mirCase    LIST_mirCase_fcons(LIST_mirCase, mirCase);
LIST_mirCase    LIST_mirCase_freverse (LIST_mirCase);
struct ITERLIST_mirCase {
        LIST_mirCase    thelist;
        LIST_mirCase_FLIST      cursor;
};
typedef struct ITERLIST_mirCase * ITERLIST_mirCase;
ITERLIST_mirCase ITERLIST_mirCase_create(LIST_mirCase);
void    ITERLIST_mirCase_delete (ITERLIST_mirCase This);
mirCase ITERLIST_mirCase_getcurrentelt(ITERLIST_mirCase);
LIST_mirCase_FLIST      ITERLIST_mirCase_getcurrentsublist (ITERLIST_mirCase);
mirCase ITERLIST_mirCase_iter (ITERLIST_mirCase);
mirCase ITERLIST_mirCase_nth (ITERLIST_mirCase, int);
mirCase ITERLIST_mirCase_reviter (ITERLIST_mirCase);
void    ITERLIST_mirCase_setcursor(ITERLIST_mirCase This, int pos);
void    ITERLIST_mirCase_stepcursor (ITERLIST_mirCase);
void    ITERLIST_mirCase_tolast (ITERLIST_mirCase);
void    ITERLIST_mirCase_assign (ITERLIST_mirCase, mirCase elt);
void    ITERLIST_mirCase_delpos(ITERLIST_mirCase);
void    ITERLIST_mirCase_insertelt(ITERLIST_mirCase cur, mirCase newelt, BOOL before);
void    ITERLIST_mirCase_insertlist(ITERLIST_mirCase cursor, LIST_mirCase inserted, BOOL before);
LIST_mirCase ITERLIST_mirCase_split(ITERLIST_mirCase);
void    ITERLIST_mirCase_insertaux(ITERLIST_mirCase cursor, LIST_mirCase_FLIST extra, BOOL before);
void    LIST_mirCase_FLIST_insertlist(LIST_mirCase_FLIST*, LIST_mirCase_FLIST*, LIST_mirCase_FLIST*, LIST_mirCase_FLIST, BOOL before);
LIST_mirEXPR  LIST_mirEXPR_tl(LIST_mirEXPR);
LIST_mirEXPR    LIST_mirEXPR_create(void);
LIST_mirEXPR    LIST_mirEXPR_listoneelt (mirEXPR);
LIST_mirEXPR    LIST_mirEXPR_copy (LIST_mirEXPR);
LIST_mirEXPR_FLIST      LIST_mirEXPR_FLIST_deepcopy (LIST_mirEXPR_FLIST p);
LIST_mirEXPR    LIST_mirEXPR_merge (LIST_mirEXPR, LIST_mirEXPR);
void    LIST_mirEXPR_delete(LIST_mirEXPR This);
int     LIST_mirEXPR_FLIST_pos (LIST_mirEXPR_FLIST p, mirEXPR elt);
int     LIST_mirEXPR_FLIST_length (LIST_mirEXPR_FLIST p);
mirEXPR LIST_mirEXPR_last (LIST_mirEXPR);
mirEXPR LIST_mirEXPR_nth (LIST_mirEXPR, int);
mirEXPR LIST_mirEXPR_FLIST_nth (LIST_mirEXPR_FLIST p, int i);
LIST_mirEXPR_FLIST      LIST_mirEXPR_FLIST_index (LIST_mirEXPR_FLIST p, int i);
mirEXPR LIST_mirEXPR_hd (LIST_mirEXPR);
LIST_mirEXPR_FLIST      LIST_mirEXPR_FLIST_before (LIST_mirEXPR_FLIST p, LIST_mirEXPR_FLIST q);
LIST_mirEXPR_FLIST      LIST_mirEXPR_FLIST_last (LIST_mirEXPR_FLIST p);
void    LIST_mirEXPR_printids(LIST_mirEXPR This);
void    LIST_mirEXPR_FLIST_printids (LIST_mirEXPR_FLIST p);
mirEXPR LIST_mirEXPR_rightbrother(LIST_mirEXPR, mirEXPR);
BOOL    LIST_mirEXPR_isequal (LIST_mirEXPR, LIST_mirEXPR);
BOOL    LIST_mirEXPR_FLIST_isequal (LIST_mirEXPR_FLIST p, LIST_mirEXPR_FLIST q);
BOOL    LIST_mirEXPR_isfirst (LIST_mirEXPR, mirEXPR);
BOOL    LIST_mirEXPR_islast (LIST_mirEXPR, mirEXPR);
mirEXPR LIST_mirEXPR_leftbrother(LIST_mirEXPR, mirEXPR);
void    LIST_mirEXPR_append (LIST_mirEXPR, LIST_mirEXPR);
void    LIST_mirEXPR_appendelt (LIST_mirEXPR, mirEXPR);
LIST_mirEXPR_FLIST      LIST_mirEXPR_FLIST_make (mirEXPR value, LIST_mirEXPR_FLIST next);
void    LIST_mirEXPR_assign (LIST_mirEXPR, mirEXPR, int);
void    LIST_mirEXPR_cons (LIST_mirEXPR, mirEXPR);
void    LIST_mirEXPR_reverse (LIST_mirEXPR);
void    LIST_mirEXPR_dreverse (LIST_mirEXPR);
LIST_mirEXPR_FLIST      LIST_mirEXPR_FLIST_reverse (LIST_mirEXPR_FLIST p);
void    LIST_mirEXPR_del (LIST_mirEXPR, mirEXPR);
LIST_mirEXPR_FLIST      LIST_mirEXPR_FLIST_delelt (LIST_mirEXPR_FLIST p, mirEXPR value);
void    LIST_mirEXPR_insert(LIST_mirEXPR, int i, mirEXPR E);
LIST_mirEXPR_FLIST      LIST_mirEXPR_FLIST_insert (LIST_mirEXPR_FLIST p, int i, mirEXPR elt);
void    LIST_mirEXPR_delpos (LIST_mirEXPR, int);
LIST_mirEXPR_FLIST      LIST_mirEXPR_FLIST_delpos (LIST_mirEXPR_FLIST p, int pos);
LIST_mirEXPR  LIST_mirEXPR_fappend (LIST_mirEXPR, LIST_mirEXPR);
LIST_mirEXPR  LIST_mirEXPR_fappend1 (LIST_mirEXPR, mirEXPR);
LIST_mirEXPR    LIST_mirEXPR_fcons(LIST_mirEXPR, mirEXPR);
LIST_mirEXPR    LIST_mirEXPR_freverse (LIST_mirEXPR);
struct ITERLIST_mirEXPR {
        LIST_mirEXPR    thelist;
        LIST_mirEXPR_FLIST      cursor;
};
typedef struct ITERLIST_mirEXPR * ITERLIST_mirEXPR;
ITERLIST_mirEXPR ITERLIST_mirEXPR_create(LIST_mirEXPR);
void    ITERLIST_mirEXPR_delete (ITERLIST_mirEXPR This);
mirEXPR ITERLIST_mirEXPR_getcurrentelt(ITERLIST_mirEXPR);
LIST_mirEXPR_FLIST      ITERLIST_mirEXPR_getcurrentsublist (ITERLIST_mirEXPR);
mirEXPR ITERLIST_mirEXPR_iter (ITERLIST_mirEXPR);
mirEXPR ITERLIST_mirEXPR_nth (ITERLIST_mirEXPR, int);
mirEXPR ITERLIST_mirEXPR_reviter (ITERLIST_mirEXPR);
void    ITERLIST_mirEXPR_setcursor(ITERLIST_mirEXPR This, int pos);
void    ITERLIST_mirEXPR_stepcursor (ITERLIST_mirEXPR);
void    ITERLIST_mirEXPR_tolast (ITERLIST_mirEXPR);
void    ITERLIST_mirEXPR_assign (ITERLIST_mirEXPR, mirEXPR elt);
void    ITERLIST_mirEXPR_delpos(ITERLIST_mirEXPR);
void    ITERLIST_mirEXPR_insertelt(ITERLIST_mirEXPR cur, mirEXPR newelt, BOOL before);
void    ITERLIST_mirEXPR_insertlist(ITERLIST_mirEXPR cursor, LIST_mirEXPR inserted, BOOL before);
LIST_mirEXPR ITERLIST_mirEXPR_split(ITERLIST_mirEXPR);
void    ITERLIST_mirEXPR_insertaux(ITERLIST_mirEXPR cursor, LIST_mirEXPR_FLIST extra, BOOL before);
void    LIST_mirEXPR_FLIST_insertlist(LIST_mirEXPR_FLIST*, LIST_mirEXPR_FLIST*, LIST_mirEXPR_FLIST*, LIST_mirEXPR_FLIST, BOOL before);
LIST_mirALTERNATIVE  LIST_mirALTERNATIVE_tl(LIST_mirALTERNATIVE);
LIST_mirALTERNATIVE     LIST_mirALTERNATIVE_create(void);
LIST_mirALTERNATIVE     LIST_mirALTERNATIVE_listoneelt (mirALTERNATIVE);
LIST_mirALTERNATIVE     LIST_mirALTERNATIVE_copy (LIST_mirALTERNATIVE);
LIST_mirALTERNATIVE_FLIST       LIST_mirALTERNATIVE_FLIST_deepcopy (LIST_mirALTERNATIVE_FLIST p);
LIST_mirALTERNATIVE     LIST_mirALTERNATIVE_merge (LIST_mirALTERNATIVE, LIST_mirALTERNATIVE);
void    LIST_mirALTERNATIVE_delete(LIST_mirALTERNATIVE This);
int     LIST_mirALTERNATIVE_FLIST_pos (LIST_mirALTERNATIVE_FLIST p, mirALTERNATIVE elt);
int     LIST_mirALTERNATIVE_FLIST_length (LIST_mirALTERNATIVE_FLIST p);
mirALTERNATIVE  LIST_mirALTERNATIVE_last (LIST_mirALTERNATIVE);
mirALTERNATIVE  LIST_mirALTERNATIVE_nth (LIST_mirALTERNATIVE, int);
mirALTERNATIVE  LIST_mirALTERNATIVE_FLIST_nth (LIST_mirALTERNATIVE_FLIST p, int i);
LIST_mirALTERNATIVE_FLIST       LIST_mirALTERNATIVE_FLIST_index (LIST_mirALTERNATIVE_FLIST p, int i);
mirALTERNATIVE  LIST_mirALTERNATIVE_hd (LIST_mirALTERNATIVE);
LIST_mirALTERNATIVE_FLIST       LIST_mirALTERNATIVE_FLIST_before (LIST_mirALTERNATIVE_FLIST p, LIST_mirALTERNATIVE_FLIST q);
LIST_mirALTERNATIVE_FLIST       LIST_mirALTERNATIVE_FLIST_last (LIST_mirALTERNATIVE_FLIST p);
void    LIST_mirALTERNATIVE_printids(LIST_mirALTERNATIVE This);
void    LIST_mirALTERNATIVE_FLIST_printids (LIST_mirALTERNATIVE_FLIST p);
mirALTERNATIVE  LIST_mirALTERNATIVE_rightbrother(LIST_mirALTERNATIVE, mirALTERNATIVE);
BOOL    LIST_mirALTERNATIVE_isequal (LIST_mirALTERNATIVE, LIST_mirALTERNATIVE);
BOOL    LIST_mirALTERNATIVE_FLIST_isequal (LIST_mirALTERNATIVE_FLIST p, LIST_mirALTERNATIVE_FLIST q);
BOOL    LIST_mirALTERNATIVE_isfirst (LIST_mirALTERNATIVE, mirALTERNATIVE);
BOOL    LIST_mirALTERNATIVE_islast (LIST_mirALTERNATIVE, mirALTERNATIVE);
mirALTERNATIVE  LIST_mirALTERNATIVE_leftbrother(LIST_mirALTERNATIVE, mirALTERNATIVE);
void    LIST_mirALTERNATIVE_append (LIST_mirALTERNATIVE, LIST_mirALTERNATIVE);
void    LIST_mirALTERNATIVE_appendelt (LIST_mirALTERNATIVE, mirALTERNATIVE);
LIST_mirALTERNATIVE_FLIST       LIST_mirALTERNATIVE_FLIST_make (mirALTERNATIVE value, LIST_mirALTERNATIVE_FLIST next);
void    LIST_mirALTERNATIVE_assign (LIST_mirALTERNATIVE, mirALTERNATIVE, int);
void    LIST_mirALTERNATIVE_cons (LIST_mirALTERNATIVE, mirALTERNATIVE);
void    LIST_mirALTERNATIVE_reverse (LIST_mirALTERNATIVE);
void    LIST_mirALTERNATIVE_dreverse (LIST_mirALTERNATIVE);
LIST_mirALTERNATIVE_FLIST       LIST_mirALTERNATIVE_FLIST_reverse (LIST_mirALTERNATIVE_FLIST p);
void    LIST_mirALTERNATIVE_del (LIST_mirALTERNATIVE, mirALTERNATIVE);
LIST_mirALTERNATIVE_FLIST       LIST_mirALTERNATIVE_FLIST_delelt (LIST_mirALTERNATIVE_FLIST p, mirALTERNATIVE value);
void    LIST_mirALTERNATIVE_insert(LIST_mirALTERNATIVE, int i, mirALTERNATIVE E);
LIST_mirALTERNATIVE_FLIST       LIST_mirALTERNATIVE_FLIST_insert (LIST_mirALTERNATIVE_FLIST p, int i, mirALTERNATIVE elt);
void    LIST_mirALTERNATIVE_delpos (LIST_mirALTERNATIVE, int);
LIST_mirALTERNATIVE_FLIST       LIST_mirALTERNATIVE_FLIST_delpos (LIST_mirALTERNATIVE_FLIST p, int pos);
LIST_mirALTERNATIVE  LIST_mirALTERNATIVE_fappend (LIST_mirALTERNATIVE, LIST_mirALTERNATIVE);
LIST_mirALTERNATIVE  LIST_mirALTERNATIVE_fappend1 (LIST_mirALTERNATIVE, mirALTERNATIVE);
LIST_mirALTERNATIVE     LIST_mirALTERNATIVE_fcons(LIST_mirALTERNATIVE, mirALTERNATIVE);
LIST_mirALTERNATIVE     LIST_mirALTERNATIVE_freverse (LIST_mirALTERNATIVE);
struct ITERLIST_mirALTERNATIVE {
        LIST_mirALTERNATIVE     thelist;
        LIST_mirALTERNATIVE_FLIST       cursor;
};
typedef struct ITERLIST_mirALTERNATIVE * ITERLIST_mirALTERNATIVE;
ITERLIST_mirALTERNATIVE ITERLIST_mirALTERNATIVE_create(LIST_mirALTERNATIVE);
void    ITERLIST_mirALTERNATIVE_delete (ITERLIST_mirALTERNATIVE This);
mirALTERNATIVE  ITERLIST_mirALTERNATIVE_getcurrentelt(ITERLIST_mirALTERNATIVE);
LIST_mirALTERNATIVE_FLIST       ITERLIST_mirALTERNATIVE_getcurrentsublist (ITERLIST_mirALTERNATIVE);
mirALTERNATIVE  ITERLIST_mirALTERNATIVE_iter (ITERLIST_mirALTERNATIVE);
mirALTERNATIVE  ITERLIST_mirALTERNATIVE_nth (ITERLIST_mirALTERNATIVE, int);
mirALTERNATIVE  ITERLIST_mirALTERNATIVE_reviter (ITERLIST_mirALTERNATIVE);
void    ITERLIST_mirALTERNATIVE_setcursor(ITERLIST_mirALTERNATIVE This, int pos);
void    ITERLIST_mirALTERNATIVE_stepcursor (ITERLIST_mirALTERNATIVE);
void    ITERLIST_mirALTERNATIVE_tolast (ITERLIST_mirALTERNATIVE);
void    ITERLIST_mirALTERNATIVE_assign (ITERLIST_mirALTERNATIVE, mirALTERNATIVE elt);
void    ITERLIST_mirALTERNATIVE_delpos(ITERLIST_mirALTERNATIVE);
void    ITERLIST_mirALTERNATIVE_insertelt(ITERLIST_mirALTERNATIVE cur, mirALTERNATIVE newelt, BOOL before);
void    ITERLIST_mirALTERNATIVE_insertlist(ITERLIST_mirALTERNATIVE cursor, LIST_mirALTERNATIVE inserted, BOOL before);
LIST_mirALTERNATIVE ITERLIST_mirALTERNATIVE_split(ITERLIST_mirALTERNATIVE);
void    ITERLIST_mirALTERNATIVE_insertaux(ITERLIST_mirALTERNATIVE cursor, LIST_mirALTERNATIVE_FLIST extra, BOOL before);
void    LIST_mirALTERNATIVE_FLIST_insertlist(LIST_mirALTERNATIVE_FLIST*, LIST_mirALTERNATIVE_FLIST*, LIST_mirALTERNATIVE_FLIST*, LIST_mirALTERNATIVE_FLIST, BOOL before);
LIST_mirPROCESS  LIST_mirPROCESS_tl(LIST_mirPROCESS);
LIST_mirPROCESS LIST_mirPROCESS_create(void);
LIST_mirPROCESS LIST_mirPROCESS_listoneelt (mirPROCESS);
LIST_mirPROCESS LIST_mirPROCESS_copy (LIST_mirPROCESS);
LIST_mirPROCESS_FLIST   LIST_mirPROCESS_FLIST_deepcopy (LIST_mirPROCESS_FLIST p);
LIST_mirPROCESS LIST_mirPROCESS_merge (LIST_mirPROCESS, LIST_mirPROCESS);
void    LIST_mirPROCESS_delete(LIST_mirPROCESS This);
int     LIST_mirPROCESS_FLIST_pos (LIST_mirPROCESS_FLIST p, mirPROCESS elt);
int     LIST_mirPROCESS_FLIST_length (LIST_mirPROCESS_FLIST p);
mirPROCESS      LIST_mirPROCESS_last (LIST_mirPROCESS);
mirPROCESS      LIST_mirPROCESS_nth (LIST_mirPROCESS, int);
mirPROCESS      LIST_mirPROCESS_FLIST_nth (LIST_mirPROCESS_FLIST p, int i);
LIST_mirPROCESS_FLIST   LIST_mirPROCESS_FLIST_index (LIST_mirPROCESS_FLIST p, int i);
mirPROCESS      LIST_mirPROCESS_hd (LIST_mirPROCESS);
LIST_mirPROCESS_FLIST   LIST_mirPROCESS_FLIST_before (LIST_mirPROCESS_FLIST p, LIST_mirPROCESS_FLIST q);
LIST_mirPROCESS_FLIST   LIST_mirPROCESS_FLIST_last (LIST_mirPROCESS_FLIST p);
void    LIST_mirPROCESS_printids(LIST_mirPROCESS This);
void    LIST_mirPROCESS_FLIST_printids (LIST_mirPROCESS_FLIST p);
mirPROCESS      LIST_mirPROCESS_rightbrother(LIST_mirPROCESS, mirPROCESS);
BOOL    LIST_mirPROCESS_isequal (LIST_mirPROCESS, LIST_mirPROCESS);
BOOL    LIST_mirPROCESS_FLIST_isequal (LIST_mirPROCESS_FLIST p, LIST_mirPROCESS_FLIST q);
BOOL    LIST_mirPROCESS_isfirst (LIST_mirPROCESS, mirPROCESS);
BOOL    LIST_mirPROCESS_islast (LIST_mirPROCESS, mirPROCESS);
mirPROCESS      LIST_mirPROCESS_leftbrother(LIST_mirPROCESS, mirPROCESS);
void    LIST_mirPROCESS_append (LIST_mirPROCESS, LIST_mirPROCESS);
void    LIST_mirPROCESS_appendelt (LIST_mirPROCESS, mirPROCESS);
LIST_mirPROCESS_FLIST   LIST_mirPROCESS_FLIST_make (mirPROCESS value, LIST_mirPROCESS_FLIST next);
void    LIST_mirPROCESS_assign (LIST_mirPROCESS, mirPROCESS, int);
void    LIST_mirPROCESS_cons (LIST_mirPROCESS, mirPROCESS);
void    LIST_mirPROCESS_reverse (LIST_mirPROCESS);
void    LIST_mirPROCESS_dreverse (LIST_mirPROCESS);
LIST_mirPROCESS_FLIST   LIST_mirPROCESS_FLIST_reverse (LIST_mirPROCESS_FLIST p);
void    LIST_mirPROCESS_del (LIST_mirPROCESS, mirPROCESS);
LIST_mirPROCESS_FLIST   LIST_mirPROCESS_FLIST_delelt (LIST_mirPROCESS_FLIST p, mirPROCESS value);
void    LIST_mirPROCESS_insert(LIST_mirPROCESS, int i, mirPROCESS E);
LIST_mirPROCESS_FLIST   LIST_mirPROCESS_FLIST_insert (LIST_mirPROCESS_FLIST p, int i, mirPROCESS elt);
void    LIST_mirPROCESS_delpos (LIST_mirPROCESS, int);
LIST_mirPROCESS_FLIST   LIST_mirPROCESS_FLIST_delpos (LIST_mirPROCESS_FLIST p, int pos);
LIST_mirPROCESS  LIST_mirPROCESS_fappend (LIST_mirPROCESS, LIST_mirPROCESS);
LIST_mirPROCESS  LIST_mirPROCESS_fappend1 (LIST_mirPROCESS, mirPROCESS);
LIST_mirPROCESS LIST_mirPROCESS_fcons(LIST_mirPROCESS, mirPROCESS);
LIST_mirPROCESS LIST_mirPROCESS_freverse (LIST_mirPROCESS);
struct ITERLIST_mirPROCESS {
        LIST_mirPROCESS thelist;
        LIST_mirPROCESS_FLIST   cursor;
};
typedef struct ITERLIST_mirPROCESS * ITERLIST_mirPROCESS;
ITERLIST_mirPROCESS ITERLIST_mirPROCESS_create(LIST_mirPROCESS);
void    ITERLIST_mirPROCESS_delete (ITERLIST_mirPROCESS This);
mirPROCESS      ITERLIST_mirPROCESS_getcurrentelt(ITERLIST_mirPROCESS);
LIST_mirPROCESS_FLIST   ITERLIST_mirPROCESS_getcurrentsublist (ITERLIST_mirPROCESS);
mirPROCESS      ITERLIST_mirPROCESS_iter (ITERLIST_mirPROCESS);
mirPROCESS      ITERLIST_mirPROCESS_nth (ITERLIST_mirPROCESS, int);
mirPROCESS      ITERLIST_mirPROCESS_reviter (ITERLIST_mirPROCESS);
void    ITERLIST_mirPROCESS_setcursor(ITERLIST_mirPROCESS This, int pos);
void    ITERLIST_mirPROCESS_stepcursor (ITERLIST_mirPROCESS);
void    ITERLIST_mirPROCESS_tolast (ITERLIST_mirPROCESS);
void    ITERLIST_mirPROCESS_assign (ITERLIST_mirPROCESS, mirPROCESS elt);
void    ITERLIST_mirPROCESS_delpos(ITERLIST_mirPROCESS);
void    ITERLIST_mirPROCESS_insertelt(ITERLIST_mirPROCESS cur, mirPROCESS newelt, BOOL before);
void    ITERLIST_mirPROCESS_insertlist(ITERLIST_mirPROCESS cursor, LIST_mirPROCESS inserted, BOOL before);
LIST_mirPROCESS ITERLIST_mirPROCESS_split(ITERLIST_mirPROCESS);
void    ITERLIST_mirPROCESS_insertaux(ITERLIST_mirPROCESS cursor, LIST_mirPROCESS_FLIST extra, BOOL before);
void    LIST_mirPROCESS_FLIST_insertlist(LIST_mirPROCESS_FLIST*, LIST_mirPROCESS_FLIST*, LIST_mirPROCESS_FLIST*, LIST_mirPROCESS_FLIST, BOOL before);
LIST_mirObject  d_LIST_mirObject_create (opcode_t);
void    d_LIST_mirObject_delete (LIST_mirObject);
LIST_mirObject  LIST_mirObject_tl(LIST_mirObject);
LIST_mirObject  LIST_mirObject_create(void);
LIST_mirObject  LIST_mirObject_listoneelt (mirObject);
LIST_mirObject  LIST_mirObject_copy (LIST_mirObject);
LIST_mirObject_FLIST    LIST_mirObject_FLIST_deepcopy (LIST_mirObject_FLIST p);
LIST_mirObject  LIST_mirObject_merge (LIST_mirObject, LIST_mirObject);
void    LIST_mirObject_delete(LIST_mirObject This);
int     LIST_mirObject_FLIST_pos (LIST_mirObject_FLIST p, mirObject elt);
int     LIST_mirObject_FLIST_length (LIST_mirObject_FLIST p);
mirObject       LIST_mirObject_last (LIST_mirObject);
mirObject       LIST_mirObject_nth (LIST_mirObject, int);
mirObject       LIST_mirObject_FLIST_nth (LIST_mirObject_FLIST p, int i);
LIST_mirObject_FLIST    LIST_mirObject_FLIST_index (LIST_mirObject_FLIST p, int i);
mirObject       LIST_mirObject_hd (LIST_mirObject);
LIST_mirObject_FLIST    LIST_mirObject_FLIST_before (LIST_mirObject_FLIST p, LIST_mirObject_FLIST q);
LIST_mirObject_FLIST    LIST_mirObject_FLIST_last (LIST_mirObject_FLIST p);
void    LIST_mirObject_printids(LIST_mirObject This);
void    LIST_mirObject_FLIST_printids (LIST_mirObject_FLIST p);
mirObject       LIST_mirObject_rightbrother(LIST_mirObject, mirObject);
BOOL    LIST_mirObject_isequal (LIST_mirObject, LIST_mirObject);
BOOL    LIST_mirObject_FLIST_isequal (LIST_mirObject_FLIST p, LIST_mirObject_FLIST q);
BOOL    LIST_mirObject_isfirst (LIST_mirObject, mirObject);
BOOL    LIST_mirObject_islast (LIST_mirObject, mirObject);
mirObject       LIST_mirObject_leftbrother(LIST_mirObject, mirObject);
void    LIST_mirObject_append (LIST_mirObject, LIST_mirObject);
void    LIST_mirObject_appendelt (LIST_mirObject, mirObject);
LIST_mirObject_FLIST    LIST_mirObject_FLIST_make (mirObject value, LIST_mirObject_FLIST next);
void    LIST_mirObject_assign (LIST_mirObject, mirObject, int);
void    LIST_mirObject_cons (LIST_mirObject, mirObject);
void    LIST_mirObject_reverse (LIST_mirObject);
void    LIST_mirObject_dreverse (LIST_mirObject);
LIST_mirObject_FLIST    LIST_mirObject_FLIST_reverse (LIST_mirObject_FLIST p);
void    LIST_mirObject_del (LIST_mirObject, mirObject);
LIST_mirObject_FLIST    LIST_mirObject_FLIST_delelt (LIST_mirObject_FLIST p, mirObject value);
void    LIST_mirObject_insert(LIST_mirObject, int i, mirObject E);
LIST_mirObject_FLIST    LIST_mirObject_FLIST_insert (LIST_mirObject_FLIST p, int i, mirObject elt);
void    LIST_mirObject_delpos (LIST_mirObject, int);
LIST_mirObject_FLIST    LIST_mirObject_FLIST_delpos (LIST_mirObject_FLIST p, int pos);
LIST_mirObject  LIST_mirObject_fappend (LIST_mirObject, LIST_mirObject);
LIST_mirObject  LIST_mirObject_fappend1 (LIST_mirObject, mirObject);
LIST_mirObject  LIST_mirObject_fcons(LIST_mirObject, mirObject);
LIST_mirObject  LIST_mirObject_freverse (LIST_mirObject);
struct ITERLIST_mirObject {
        LIST_mirObject  thelist;
        LIST_mirObject_FLIST    cursor;
};
typedef struct ITERLIST_mirObject * ITERLIST_mirObject;
ITERLIST_mirObject ITERLIST_mirObject_create(LIST_mirObject);
void    ITERLIST_mirObject_delete (ITERLIST_mirObject This);
mirObject       ITERLIST_mirObject_getcurrentelt(ITERLIST_mirObject);
LIST_mirObject_FLIST    ITERLIST_mirObject_getcurrentsublist (ITERLIST_mirObject);
mirObject       ITERLIST_mirObject_iter (ITERLIST_mirObject);
mirObject       ITERLIST_mirObject_nth (ITERLIST_mirObject, int);
mirObject       ITERLIST_mirObject_reviter (ITERLIST_mirObject);
void    ITERLIST_mirObject_setcursor(ITERLIST_mirObject This, int pos);
void    ITERLIST_mirObject_stepcursor (ITERLIST_mirObject);
void    ITERLIST_mirObject_tolast (ITERLIST_mirObject);
void    ITERLIST_mirObject_assign (ITERLIST_mirObject, mirObject elt);
void    ITERLIST_mirObject_delpos(ITERLIST_mirObject);
void    ITERLIST_mirObject_insertelt(ITERLIST_mirObject cur, mirObject newelt, BOOL before);
void    ITERLIST_mirObject_insertlist(ITERLIST_mirObject cursor, LIST_mirObject inserted, BOOL before);
LIST_mirObject ITERLIST_mirObject_split(ITERLIST_mirObject);
void    ITERLIST_mirObject_insertaux(ITERLIST_mirObject cursor, LIST_mirObject_FLIST extra, BOOL before);
void    LIST_mirObject_FLIST_insertlist(LIST_mirObject_FLIST*, LIST_mirObject_FLIST*, LIST_mirObject_FLIST*, LIST_mirObject_FLIST, BOOL before);
LIST_mirIterMask  LIST_mirIterMask_tl(LIST_mirIterMask);
LIST_mirIterMask        LIST_mirIterMask_create(void);
LIST_mirIterMask        LIST_mirIterMask_listoneelt (mirIterMask);
LIST_mirIterMask        LIST_mirIterMask_copy (LIST_mirIterMask);
LIST_mirIterMask_FLIST  LIST_mirIterMask_FLIST_deepcopy (LIST_mirIterMask_FLIST p);
LIST_mirIterMask        LIST_mirIterMask_merge (LIST_mirIterMask, LIST_mirIterMask);
void    LIST_mirIterMask_delete(LIST_mirIterMask This);
int     LIST_mirIterMask_FLIST_pos (LIST_mirIterMask_FLIST p, mirIterMask elt);
int     LIST_mirIterMask_FLIST_length (LIST_mirIterMask_FLIST p);
mirIterMask     LIST_mirIterMask_last (LIST_mirIterMask);
mirIterMask     LIST_mirIterMask_nth (LIST_mirIterMask, int);
mirIterMask     LIST_mirIterMask_FLIST_nth (LIST_mirIterMask_FLIST p, int i);
LIST_mirIterMask_FLIST  LIST_mirIterMask_FLIST_index (LIST_mirIterMask_FLIST p, int i);
mirIterMask     LIST_mirIterMask_hd (LIST_mirIterMask);
LIST_mirIterMask_FLIST  LIST_mirIterMask_FLIST_before (LIST_mirIterMask_FLIST p, LIST_mirIterMask_FLIST q);
LIST_mirIterMask_FLIST  LIST_mirIterMask_FLIST_last (LIST_mirIterMask_FLIST p);
void    LIST_mirIterMask_printids(LIST_mirIterMask This);
void    LIST_mirIterMask_FLIST_printids (LIST_mirIterMask_FLIST p);
mirIterMask     LIST_mirIterMask_rightbrother(LIST_mirIterMask, mirIterMask);
BOOL    LIST_mirIterMask_isequal (LIST_mirIterMask, LIST_mirIterMask);
BOOL    LIST_mirIterMask_FLIST_isequal (LIST_mirIterMask_FLIST p, LIST_mirIterMask_FLIST q);
BOOL    LIST_mirIterMask_isfirst (LIST_mirIterMask, mirIterMask);
BOOL    LIST_mirIterMask_islast (LIST_mirIterMask, mirIterMask);
mirIterMask     LIST_mirIterMask_leftbrother(LIST_mirIterMask, mirIterMask);
void    LIST_mirIterMask_append (LIST_mirIterMask, LIST_mirIterMask);
void    LIST_mirIterMask_appendelt (LIST_mirIterMask, mirIterMask);
LIST_mirIterMask_FLIST  LIST_mirIterMask_FLIST_make (mirIterMask value, LIST_mirIterMask_FLIST next);
void    LIST_mirIterMask_assign (LIST_mirIterMask, mirIterMask, int);
void    LIST_mirIterMask_cons (LIST_mirIterMask, mirIterMask);
void    LIST_mirIterMask_reverse (LIST_mirIterMask);
void    LIST_mirIterMask_dreverse (LIST_mirIterMask);
LIST_mirIterMask_FLIST  LIST_mirIterMask_FLIST_reverse (LIST_mirIterMask_FLIST p);
void    LIST_mirIterMask_del (LIST_mirIterMask, mirIterMask);
LIST_mirIterMask_FLIST  LIST_mirIterMask_FLIST_delelt (LIST_mirIterMask_FLIST p, mirIterMask value);
void    LIST_mirIterMask_insert(LIST_mirIterMask, int i, mirIterMask E);
LIST_mirIterMask_FLIST  LIST_mirIterMask_FLIST_insert (LIST_mirIterMask_FLIST p, int i, mirIterMask elt);
void    LIST_mirIterMask_delpos (LIST_mirIterMask, int);
LIST_mirIterMask_FLIST  LIST_mirIterMask_FLIST_delpos (LIST_mirIterMask_FLIST p, int pos);
LIST_mirIterMask  LIST_mirIterMask_fappend (LIST_mirIterMask, LIST_mirIterMask);
LIST_mirIterMask  LIST_mirIterMask_fappend1 (LIST_mirIterMask, mirIterMask);
LIST_mirIterMask        LIST_mirIterMask_fcons(LIST_mirIterMask, mirIterMask);
LIST_mirIterMask        LIST_mirIterMask_freverse (LIST_mirIterMask);
struct ITERLIST_mirIterMask {
        LIST_mirIterMask        thelist;
        LIST_mirIterMask_FLIST  cursor;
};
typedef struct ITERLIST_mirIterMask * ITERLIST_mirIterMask;
ITERLIST_mirIterMask ITERLIST_mirIterMask_create(LIST_mirIterMask);
void    ITERLIST_mirIterMask_delete (ITERLIST_mirIterMask This);
mirIterMask     ITERLIST_mirIterMask_getcurrentelt(ITERLIST_mirIterMask);
LIST_mirIterMask_FLIST  ITERLIST_mirIterMask_getcurrentsublist (ITERLIST_mirIterMask);
mirIterMask     ITERLIST_mirIterMask_iter (ITERLIST_mirIterMask);
mirIterMask     ITERLIST_mirIterMask_nth (ITERLIST_mirIterMask, int);
mirIterMask     ITERLIST_mirIterMask_reviter (ITERLIST_mirIterMask);
void    ITERLIST_mirIterMask_setcursor(ITERLIST_mirIterMask This, int pos);
void    ITERLIST_mirIterMask_stepcursor (ITERLIST_mirIterMask);
void    ITERLIST_mirIterMask_tolast (ITERLIST_mirIterMask);
void    ITERLIST_mirIterMask_assign (ITERLIST_mirIterMask, mirIterMask elt);
void    ITERLIST_mirIterMask_delpos(ITERLIST_mirIterMask);
void    ITERLIST_mirIterMask_insertelt(ITERLIST_mirIterMask cur, mirIterMask newelt, BOOL before);
void    ITERLIST_mirIterMask_insertlist(ITERLIST_mirIterMask cursor, LIST_mirIterMask inserted, BOOL before);
LIST_mirIterMask ITERLIST_mirIterMask_split(ITERLIST_mirIterMask);
void    ITERLIST_mirIterMask_insertaux(ITERLIST_mirIterMask cursor, LIST_mirIterMask_FLIST extra, BOOL before);
void    LIST_mirIterMask_FLIST_insertlist(LIST_mirIterMask_FLIST*, LIST_mirIterMask_FLIST*, LIST_mirIterMask_FLIST*, LIST_mirIterMask_FLIST, BOOL before);
LIST_mirSTMT  LIST_mirSTMT_tl(LIST_mirSTMT);
LIST_mirSTMT    LIST_mirSTMT_create(void);
LIST_mirSTMT    LIST_mirSTMT_listoneelt (mirSTMT);
LIST_mirSTMT    LIST_mirSTMT_copy (LIST_mirSTMT);
LIST_mirSTMT_FLIST      LIST_mirSTMT_FLIST_deepcopy (LIST_mirSTMT_FLIST p);
LIST_mirSTMT    LIST_mirSTMT_merge (LIST_mirSTMT, LIST_mirSTMT);
void    LIST_mirSTMT_delete(LIST_mirSTMT This);
int     LIST_mirSTMT_FLIST_pos (LIST_mirSTMT_FLIST p, mirSTMT elt);
int     LIST_mirSTMT_FLIST_length (LIST_mirSTMT_FLIST p);
mirSTMT LIST_mirSTMT_last (LIST_mirSTMT);
mirSTMT LIST_mirSTMT_nth (LIST_mirSTMT, int);
mirSTMT LIST_mirSTMT_FLIST_nth (LIST_mirSTMT_FLIST p, int i);
LIST_mirSTMT_FLIST      LIST_mirSTMT_FLIST_index (LIST_mirSTMT_FLIST p, int i);
mirSTMT LIST_mirSTMT_hd (LIST_mirSTMT);
LIST_mirSTMT_FLIST      LIST_mirSTMT_FLIST_before (LIST_mirSTMT_FLIST p, LIST_mirSTMT_FLIST q);
LIST_mirSTMT_FLIST      LIST_mirSTMT_FLIST_last (LIST_mirSTMT_FLIST p);
void    LIST_mirSTMT_printids(LIST_mirSTMT This);
void    LIST_mirSTMT_FLIST_printids (LIST_mirSTMT_FLIST p);
mirSTMT LIST_mirSTMT_rightbrother(LIST_mirSTMT, mirSTMT);
BOOL    LIST_mirSTMT_isequal (LIST_mirSTMT, LIST_mirSTMT);
BOOL    LIST_mirSTMT_FLIST_isequal (LIST_mirSTMT_FLIST p, LIST_mirSTMT_FLIST q);
BOOL    LIST_mirSTMT_isfirst (LIST_mirSTMT, mirSTMT);
BOOL    LIST_mirSTMT_islast (LIST_mirSTMT, mirSTMT);
mirSTMT LIST_mirSTMT_leftbrother(LIST_mirSTMT, mirSTMT);
void    LIST_mirSTMT_append (LIST_mirSTMT, LIST_mirSTMT);
void    LIST_mirSTMT_appendelt (LIST_mirSTMT, mirSTMT);
LIST_mirSTMT_FLIST      LIST_mirSTMT_FLIST_make (mirSTMT value, LIST_mirSTMT_FLIST next);
void    LIST_mirSTMT_assign (LIST_mirSTMT, mirSTMT, int);
void    LIST_mirSTMT_cons (LIST_mirSTMT, mirSTMT);
void    LIST_mirSTMT_reverse (LIST_mirSTMT);
void    LIST_mirSTMT_dreverse (LIST_mirSTMT);
LIST_mirSTMT_FLIST      LIST_mirSTMT_FLIST_reverse (LIST_mirSTMT_FLIST p);
void    LIST_mirSTMT_del (LIST_mirSTMT, mirSTMT);
LIST_mirSTMT_FLIST      LIST_mirSTMT_FLIST_delelt (LIST_mirSTMT_FLIST p, mirSTMT value);
void    LIST_mirSTMT_insert(LIST_mirSTMT, int i, mirSTMT E);
LIST_mirSTMT_FLIST      LIST_mirSTMT_FLIST_insert (LIST_mirSTMT_FLIST p, int i, mirSTMT elt);
void    LIST_mirSTMT_delpos (LIST_mirSTMT, int);
LIST_mirSTMT_FLIST      LIST_mirSTMT_FLIST_delpos (LIST_mirSTMT_FLIST p, int pos);
LIST_mirSTMT  LIST_mirSTMT_fappend (LIST_mirSTMT, LIST_mirSTMT);
LIST_mirSTMT  LIST_mirSTMT_fappend1 (LIST_mirSTMT, mirSTMT);
LIST_mirSTMT    LIST_mirSTMT_fcons(LIST_mirSTMT, mirSTMT);
LIST_mirSTMT    LIST_mirSTMT_freverse (LIST_mirSTMT);
struct ITERLIST_mirSTMT {
        LIST_mirSTMT    thelist;
        LIST_mirSTMT_FLIST      cursor;
};
typedef struct ITERLIST_mirSTMT * ITERLIST_mirSTMT;
ITERLIST_mirSTMT ITERLIST_mirSTMT_create(LIST_mirSTMT);
void    ITERLIST_mirSTMT_delete (ITERLIST_mirSTMT This);
mirSTMT ITERLIST_mirSTMT_getcurrentelt(ITERLIST_mirSTMT);
LIST_mirSTMT_FLIST      ITERLIST_mirSTMT_getcurrentsublist (ITERLIST_mirSTMT);
mirSTMT ITERLIST_mirSTMT_iter (ITERLIST_mirSTMT);
mirSTMT ITERLIST_mirSTMT_nth (ITERLIST_mirSTMT, int);
mirSTMT ITERLIST_mirSTMT_reviter (ITERLIST_mirSTMT);
void    ITERLIST_mirSTMT_setcursor(ITERLIST_mirSTMT This, int pos);
void    ITERLIST_mirSTMT_stepcursor (ITERLIST_mirSTMT);
void    ITERLIST_mirSTMT_tolast (ITERLIST_mirSTMT);
void    ITERLIST_mirSTMT_assign (ITERLIST_mirSTMT, mirSTMT elt);
void    ITERLIST_mirSTMT_delpos(ITERLIST_mirSTMT);
void    ITERLIST_mirSTMT_insertelt(ITERLIST_mirSTMT cur, mirSTMT newelt, BOOL before);
void    ITERLIST_mirSTMT_insertlist(ITERLIST_mirSTMT cursor, LIST_mirSTMT inserted, BOOL before);
LIST_mirSTMT ITERLIST_mirSTMT_split(ITERLIST_mirSTMT);
void    ITERLIST_mirSTMT_insertaux(ITERLIST_mirSTMT cursor, LIST_mirSTMT_FLIST extra, BOOL before);
void    LIST_mirSTMT_FLIST_insertlist(LIST_mirSTMT_FLIST*, LIST_mirSTMT_FLIST*, LIST_mirSTMT_FLIST*, LIST_mirSTMT_FLIST, BOOL before);
LIST_mirParameter       d_LIST_mirParameter_create (opcode_t);
void    d_LIST_mirParameter_delete (LIST_mirParameter);
LIST_mirParameter  LIST_mirParameter_tl(LIST_mirParameter);
LIST_mirParameter       LIST_mirParameter_create(void);
LIST_mirParameter       LIST_mirParameter_listoneelt (mirParameter);
LIST_mirParameter       LIST_mirParameter_copy (LIST_mirParameter);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_deepcopy (LIST_mirParameter_FLIST p);
LIST_mirParameter       LIST_mirParameter_merge (LIST_mirParameter, LIST_mirParameter);
void    LIST_mirParameter_delete(LIST_mirParameter This);
int     LIST_mirParameter_FLIST_pos (LIST_mirParameter_FLIST p, mirParameter elt);
int     LIST_mirParameter_FLIST_length (LIST_mirParameter_FLIST p);
mirParameter    LIST_mirParameter_last (LIST_mirParameter);
mirParameter    LIST_mirParameter_nth (LIST_mirParameter, int);
mirParameter    LIST_mirParameter_FLIST_nth (LIST_mirParameter_FLIST p, int i);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_index (LIST_mirParameter_FLIST p, int i);
mirParameter    LIST_mirParameter_hd (LIST_mirParameter);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_before (LIST_mirParameter_FLIST p, LIST_mirParameter_FLIST q);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_last (LIST_mirParameter_FLIST p);
void    LIST_mirParameter_printids(LIST_mirParameter This);
void    LIST_mirParameter_FLIST_printids (LIST_mirParameter_FLIST p);
mirParameter    LIST_mirParameter_rightbrother(LIST_mirParameter, mirParameter);
BOOL    LIST_mirParameter_isequal (LIST_mirParameter, LIST_mirParameter);
BOOL    LIST_mirParameter_FLIST_isequal (LIST_mirParameter_FLIST p, LIST_mirParameter_FLIST q);
BOOL    LIST_mirParameter_isfirst (LIST_mirParameter, mirParameter);
BOOL    LIST_mirParameter_islast (LIST_mirParameter, mirParameter);
mirParameter    LIST_mirParameter_leftbrother(LIST_mirParameter, mirParameter);
void    LIST_mirParameter_append (LIST_mirParameter, LIST_mirParameter);
void    LIST_mirParameter_appendelt (LIST_mirParameter, mirParameter);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_make (mirParameter value, LIST_mirParameter_FLIST next);
void    LIST_mirParameter_assign (LIST_mirParameter, mirParameter, int);
void    LIST_mirParameter_cons (LIST_mirParameter, mirParameter);
void    LIST_mirParameter_reverse (LIST_mirParameter);
void    LIST_mirParameter_dreverse (LIST_mirParameter);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_reverse (LIST_mirParameter_FLIST p);
void    LIST_mirParameter_del (LIST_mirParameter, mirParameter);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_delelt (LIST_mirParameter_FLIST p, mirParameter value);
void    LIST_mirParameter_insert(LIST_mirParameter, int i, mirParameter E);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_insert (LIST_mirParameter_FLIST p, int i, mirParameter elt);
void    LIST_mirParameter_delpos (LIST_mirParameter, int);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_delpos (LIST_mirParameter_FLIST p, int pos);
LIST_mirParameter  LIST_mirParameter_fappend (LIST_mirParameter, LIST_mirParameter);
LIST_mirParameter  LIST_mirParameter_fappend1 (LIST_mirParameter, mirParameter);
LIST_mirParameter       LIST_mirParameter_fcons(LIST_mirParameter, mirParameter);
LIST_mirParameter       LIST_mirParameter_freverse (LIST_mirParameter);
struct ITERLIST_mirParameter {
        LIST_mirParameter       thelist;
        LIST_mirParameter_FLIST cursor;
};
typedef struct ITERLIST_mirParameter * ITERLIST_mirParameter;
ITERLIST_mirParameter ITERLIST_mirParameter_create(LIST_mirParameter);
void    ITERLIST_mirParameter_delete (ITERLIST_mirParameter This);
mirParameter    ITERLIST_mirParameter_getcurrentelt(ITERLIST_mirParameter);
LIST_mirParameter_FLIST ITERLIST_mirParameter_getcurrentsublist (ITERLIST_mirParameter);
mirParameter    ITERLIST_mirParameter_iter (ITERLIST_mirParameter);
mirParameter    ITERLIST_mirParameter_nth (ITERLIST_mirParameter, int);
mirParameter    ITERLIST_mirParameter_reviter (ITERLIST_mirParameter);
void    ITERLIST_mirParameter_setcursor(ITERLIST_mirParameter This, int pos);
void    ITERLIST_mirParameter_stepcursor (ITERLIST_mirParameter);
void    ITERLIST_mirParameter_tolast (ITERLIST_mirParameter);
void    ITERLIST_mirParameter_assign (ITERLIST_mirParameter, mirParameter elt);
void    ITERLIST_mirParameter_delpos(ITERLIST_mirParameter);
void    ITERLIST_mirParameter_insertelt(ITERLIST_mirParameter cur, mirParameter newelt, BOOL before);
void    ITERLIST_mirParameter_insertlist(ITERLIST_mirParameter cursor, LIST_mirParameter inserted, BOOL before);
LIST_mirParameter ITERLIST_mirParameter_split(ITERLIST_mirParameter);
void    ITERLIST_mirParameter_insertaux(ITERLIST_mirParameter cursor, LIST_mirParameter_FLIST extra, BOOL before);
void    LIST_mirParameter_FLIST_insertlist(LIST_mirParameter_FLIST*, LIST_mirParameter_FLIST*, LIST_mirParameter_FLIST*, LIST_mirParameter_FLIST, BOOL before);
LIST_mirField   LIST_mirField_create(void);
int     LIST_mirField_FLIST_pos (LIST_mirField_FLIST p, mirField elt);
int     LIST_mirField_FLIST_length (LIST_mirField_FLIST p);
mirField        LIST_mirField_last (LIST_mirField);
mirField        LIST_mirField_nth (LIST_mirField, int);
mirField        LIST_mirField_FLIST_nth (LIST_mirField_FLIST p, int i);
LIST_mirField_FLIST     LIST_mirField_FLIST_index (LIST_mirField_FLIST p, int i);
mirField        LIST_mirField_hd (LIST_mirField);
LIST_mirField_FLIST     LIST_mirField_FLIST_before (LIST_mirField_FLIST p, LIST_mirField_FLIST q);
LIST_mirField_FLIST     LIST_mirField_FLIST_last (LIST_mirField_FLIST p);
void    LIST_mirField_printids(LIST_mirField This);
void    LIST_mirField_FLIST_printids (LIST_mirField_FLIST p);
mirField        LIST_mirField_rightbrother(LIST_mirField, mirField);
void    LIST_mirField_append (LIST_mirField, LIST_mirField);
void    LIST_mirField_appendelt (LIST_mirField, mirField);
LIST_mirField_FLIST     LIST_mirField_FLIST_make (mirField value, LIST_mirField_FLIST next);
struct ITERLIST_mirField {
        LIST_mirField   thelist;
        LIST_mirField_FLIST     cursor;
};
typedef struct ITERLIST_mirField * ITERLIST_mirField;
ITERLIST_mirField ITERLIST_mirField_create(LIST_mirField);
void    ITERLIST_mirField_delete (ITERLIST_mirField This);
mirField        ITERLIST_mirField_getcurrentelt(ITERLIST_mirField);
LIST_mirField_FLIST     ITERLIST_mirField_getcurrentsublist (ITERLIST_mirField);
mirField        ITERLIST_mirField_iter (ITERLIST_mirField);
mirField        ITERLIST_mirField_nth (ITERLIST_mirField, int);
mirField        ITERLIST_mirField_reviter (ITERLIST_mirField);
void    ITERLIST_mirField_setcursor(ITERLIST_mirField This, int pos);
void    ITERLIST_mirField_stepcursor (ITERLIST_mirField);
void    ITERLIST_mirField_tolast (ITERLIST_mirField);
LIST_mirEDGE  LIST_mirEDGE_tl(LIST_mirEDGE);
LIST_mirEDGE    LIST_mirEDGE_create(void);
LIST_mirEDGE    LIST_mirEDGE_listoneelt (mirEDGE);
LIST_mirEDGE    LIST_mirEDGE_copy (LIST_mirEDGE);
LIST_mirEDGE_FLIST      LIST_mirEDGE_FLIST_deepcopy (LIST_mirEDGE_FLIST p);
LIST_mirEDGE    LIST_mirEDGE_merge (LIST_mirEDGE, LIST_mirEDGE);
void    LIST_mirEDGE_delete(LIST_mirEDGE This);
int     LIST_mirEDGE_FLIST_pos (LIST_mirEDGE_FLIST p, mirEDGE elt);
int     LIST_mirEDGE_FLIST_length (LIST_mirEDGE_FLIST p);
mirEDGE LIST_mirEDGE_last (LIST_mirEDGE);
mirEDGE LIST_mirEDGE_nth (LIST_mirEDGE, int);
mirEDGE LIST_mirEDGE_FLIST_nth (LIST_mirEDGE_FLIST p, int i);
LIST_mirEDGE_FLIST      LIST_mirEDGE_FLIST_index (LIST_mirEDGE_FLIST p, int i);
mirEDGE LIST_mirEDGE_hd (LIST_mirEDGE);
LIST_mirEDGE_FLIST      LIST_mirEDGE_FLIST_before (LIST_mirEDGE_FLIST p, LIST_mirEDGE_FLIST q);
LIST_mirEDGE_FLIST      LIST_mirEDGE_FLIST_last (LIST_mirEDGE_FLIST p);
void    LIST_mirEDGE_printids(LIST_mirEDGE This);
void    LIST_mirEDGE_FLIST_printids (LIST_mirEDGE_FLIST p);
mirEDGE LIST_mirEDGE_rightbrother(LIST_mirEDGE, mirEDGE);
BOOL    LIST_mirEDGE_isequal (LIST_mirEDGE, LIST_mirEDGE);
BOOL    LIST_mirEDGE_FLIST_isequal (LIST_mirEDGE_FLIST p, LIST_mirEDGE_FLIST q);
BOOL    LIST_mirEDGE_isfirst (LIST_mirEDGE, mirEDGE);
BOOL    LIST_mirEDGE_islast (LIST_mirEDGE, mirEDGE);
mirEDGE LIST_mirEDGE_leftbrother(LIST_mirEDGE, mirEDGE);
void    LIST_mirEDGE_append (LIST_mirEDGE, LIST_mirEDGE);
void    LIST_mirEDGE_appendelt (LIST_mirEDGE, mirEDGE);
LIST_mirEDGE_FLIST      LIST_mirEDGE_FLIST_make (mirEDGE value, LIST_mirEDGE_FLIST next);
void    LIST_mirEDGE_assign (LIST_mirEDGE, mirEDGE, int);
void    LIST_mirEDGE_cons (LIST_mirEDGE, mirEDGE);
void    LIST_mirEDGE_reverse (LIST_mirEDGE);
void    LIST_mirEDGE_dreverse (LIST_mirEDGE);
LIST_mirEDGE_FLIST      LIST_mirEDGE_FLIST_reverse (LIST_mirEDGE_FLIST p);
void    LIST_mirEDGE_del (LIST_mirEDGE, mirEDGE);
LIST_mirEDGE_FLIST      LIST_mirEDGE_FLIST_delelt (LIST_mirEDGE_FLIST p, mirEDGE value);
void    LIST_mirEDGE_insert(LIST_mirEDGE, int i, mirEDGE E);
LIST_mirEDGE_FLIST      LIST_mirEDGE_FLIST_insert (LIST_mirEDGE_FLIST p, int i, mirEDGE elt);
void    LIST_mirEDGE_delpos (LIST_mirEDGE, int);
LIST_mirEDGE_FLIST      LIST_mirEDGE_FLIST_delpos (LIST_mirEDGE_FLIST p, int pos);
LIST_mirEDGE  LIST_mirEDGE_fappend (LIST_mirEDGE, LIST_mirEDGE);
LIST_mirEDGE  LIST_mirEDGE_fappend1 (LIST_mirEDGE, mirEDGE);
LIST_mirEDGE    LIST_mirEDGE_fcons(LIST_mirEDGE, mirEDGE);
LIST_mirEDGE    LIST_mirEDGE_freverse (LIST_mirEDGE);
struct ITERLIST_mirEDGE {
        LIST_mirEDGE    thelist;
        LIST_mirEDGE_FLIST      cursor;
};
typedef struct ITERLIST_mirEDGE * ITERLIST_mirEDGE;
ITERLIST_mirEDGE ITERLIST_mirEDGE_create(LIST_mirEDGE);
void    ITERLIST_mirEDGE_delete (ITERLIST_mirEDGE This);
mirEDGE ITERLIST_mirEDGE_getcurrentelt(ITERLIST_mirEDGE);
LIST_mirEDGE_FLIST      ITERLIST_mirEDGE_getcurrentsublist (ITERLIST_mirEDGE);
mirEDGE ITERLIST_mirEDGE_iter (ITERLIST_mirEDGE);
mirEDGE ITERLIST_mirEDGE_nth (ITERLIST_mirEDGE, int);
mirEDGE ITERLIST_mirEDGE_reviter (ITERLIST_mirEDGE);
void    ITERLIST_mirEDGE_setcursor(ITERLIST_mirEDGE This, int pos);
void    ITERLIST_mirEDGE_stepcursor (ITERLIST_mirEDGE);
void    ITERLIST_mirEDGE_tolast (ITERLIST_mirEDGE);
void    ITERLIST_mirEDGE_assign (ITERLIST_mirEDGE, mirEDGE elt);
void    ITERLIST_mirEDGE_delpos(ITERLIST_mirEDGE);
void    ITERLIST_mirEDGE_insertelt(ITERLIST_mirEDGE cur, mirEDGE newelt, BOOL before);
void    ITERLIST_mirEDGE_insertlist(ITERLIST_mirEDGE cursor, LIST_mirEDGE inserted, BOOL before);
LIST_mirEDGE ITERLIST_mirEDGE_split(ITERLIST_mirEDGE);
void    ITERLIST_mirEDGE_insertaux(ITERLIST_mirEDGE cursor, LIST_mirEDGE_FLIST extra, BOOL before);
void    LIST_mirEDGE_FLIST_insertlist(LIST_mirEDGE_FLIST*, LIST_mirEDGE_FLIST*, LIST_mirEDGE_FLIST*, LIST_mirEDGE_FLIST, BOOL before);
LIST_Const      d_LIST_Const_create (opcode_t);
void    d_LIST_Const_delete (LIST_Const);
LIST_Const  LIST_Const_tl(LIST_Const);
LIST_Const      LIST_Const_create(void);
LIST_Const      LIST_Const_listoneelt (Const);
LIST_Const      LIST_Const_copy (LIST_Const);
LIST_Const_FLIST        LIST_Const_FLIST_deepcopy (LIST_Const_FLIST p);
LIST_Const      LIST_Const_merge (LIST_Const, LIST_Const);
void    LIST_Const_delete(LIST_Const This);
int     LIST_Const_FLIST_pos (LIST_Const_FLIST p, Const elt);
int     LIST_Const_FLIST_length (LIST_Const_FLIST p);
Const   LIST_Const_last (LIST_Const);
Const   LIST_Const_nth (LIST_Const, int);
Const   LIST_Const_FLIST_nth (LIST_Const_FLIST p, int i);
LIST_Const_FLIST        LIST_Const_FLIST_index (LIST_Const_FLIST p, int i);
Const   LIST_Const_hd (LIST_Const);
LIST_Const_FLIST        LIST_Const_FLIST_before (LIST_Const_FLIST p, LIST_Const_FLIST q);
LIST_Const_FLIST        LIST_Const_FLIST_last (LIST_Const_FLIST p);
void    LIST_Const_printids(LIST_Const This);
void    LIST_Const_FLIST_printids (LIST_Const_FLIST p);
Const   LIST_Const_rightbrother(LIST_Const, Const);
BOOL    LIST_Const_isequal (LIST_Const, LIST_Const);
BOOL    LIST_Const_FLIST_isequal (LIST_Const_FLIST p, LIST_Const_FLIST q);
BOOL    LIST_Const_isfirst (LIST_Const, Const);
BOOL    LIST_Const_islast (LIST_Const, Const);
Const   LIST_Const_leftbrother(LIST_Const, Const);
void    LIST_Const_append (LIST_Const, LIST_Const);
void    LIST_Const_appendelt (LIST_Const, Const);
LIST_Const_FLIST        LIST_Const_FLIST_make (Const value, LIST_Const_FLIST next);
void    LIST_Const_assign (LIST_Const, Const, int);
void    LIST_Const_cons (LIST_Const, Const);
void    LIST_Const_reverse (LIST_Const);
void    LIST_Const_dreverse (LIST_Const);
LIST_Const_FLIST        LIST_Const_FLIST_reverse (LIST_Const_FLIST p);
void    LIST_Const_del (LIST_Const, Const);
LIST_Const_FLIST        LIST_Const_FLIST_delelt (LIST_Const_FLIST p, Const value);
void    LIST_Const_insert(LIST_Const, int i, Const E);
LIST_Const_FLIST        LIST_Const_FLIST_insert (LIST_Const_FLIST p, int i, Const elt);
void    LIST_Const_delpos (LIST_Const, int);
LIST_Const_FLIST        LIST_Const_FLIST_delpos (LIST_Const_FLIST p, int pos);
LIST_Const  LIST_Const_fappend (LIST_Const, LIST_Const);
LIST_Const  LIST_Const_fappend1 (LIST_Const, Const);
LIST_Const      LIST_Const_fcons(LIST_Const, Const);
LIST_Const      LIST_Const_freverse (LIST_Const);
struct ITERLIST_Const {
        LIST_Const      thelist;
        LIST_Const_FLIST        cursor;
};
typedef struct ITERLIST_Const * ITERLIST_Const;
ITERLIST_Const ITERLIST_Const_create(LIST_Const);
void    ITERLIST_Const_delete (ITERLIST_Const This);
Const   ITERLIST_Const_getcurrentelt(ITERLIST_Const);
LIST_Const_FLIST        ITERLIST_Const_getcurrentsublist (ITERLIST_Const);
Const   ITERLIST_Const_iter (ITERLIST_Const);
Const   ITERLIST_Const_nth (ITERLIST_Const, int);
Const   ITERLIST_Const_reviter (ITERLIST_Const);
void    ITERLIST_Const_setcursor(ITERLIST_Const This, int pos);
void    ITERLIST_Const_stepcursor (ITERLIST_Const);
void    ITERLIST_Const_tolast (ITERLIST_Const);
void    ITERLIST_Const_assign (ITERLIST_Const, Const elt);
void    ITERLIST_Const_delpos(ITERLIST_Const);
void    ITERLIST_Const_insertelt(ITERLIST_Const cur, Const newelt, BOOL before);
void    ITERLIST_Const_insertlist(ITERLIST_Const cursor, LIST_Const inserted, BOOL before);
LIST_Const ITERLIST_Const_split(ITERLIST_Const);
void    ITERLIST_Const_insertaux(ITERLIST_Const cursor, LIST_Const_FLIST extra, BOOL before);
void    LIST_Const_FLIST_insertlist(LIST_Const_FLIST*, LIST_Const_FLIST*, LIST_Const_FLIST*, LIST_Const_FLIST, BOOL before);
LIST_mirObject  d_LIST_mirObject_create (opcode_t);
void    d_LIST_mirObject_delete (LIST_mirObject);
LIST_mirParameter       d_LIST_mirParameter_create (opcode_t);
void    d_LIST_mirParameter_delete (LIST_mirParameter);
LIST_mirEDGE    d_LIST_mirEDGE_create (opcode_t);
void    d_LIST_mirEDGE_delete (LIST_mirEDGE);
LIST_mirCFG     d_LIST_mirCFG_create (opcode_t);
void    d_LIST_mirCFG_delete (LIST_mirCFG);
LIST_mirIntraCfg_FLIST  LIST_mirIntraCfg_FLIST_create (opcode_t);
void    LIST_mirIntraCfg_FLIST_delete (LIST_mirIntraCfg_FLIST);
LIST_mirIntraCfg_FLIST  LIST_mirIntraCfg_FLIST_copy (LIST_mirIntraCfg_FLIST);
LIST_Symbols_FLIST      LIST_Symbols_FLIST_create (opcode_t);
void    LIST_Symbols_FLIST_delete (LIST_Symbols_FLIST);
LIST_Symbols_FLIST      LIST_Symbols_FLIST_copy (LIST_Symbols_FLIST);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_create (opcode_t);
void    LIST_mirCFG_FLIST_delete (LIST_mirCFG_FLIST);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_copy (LIST_mirCFG_FLIST);
LIST_mirTYPE_FLIST      LIST_mirTYPE_FLIST_copy (LIST_mirTYPE_FLIST);
LIST_mirSection_FLIST   LIST_mirSection_FLIST_copy (LIST_mirSection_FLIST);
LIST_mirRegister_FLIST  LIST_mirRegister_FLIST_copy (LIST_mirRegister_FLIST);
LIST_mirProcGlobal_FLIST        LIST_mirProcGlobal_FLIST_copy (LIST_mirProcGlobal_FLIST);
LIST_mirPlaced_FLIST    LIST_mirPlaced_FLIST_copy (LIST_mirPlaced_FLIST);
LIST_mirDataGlobal_FLIST        LIST_mirDataGlobal_FLIST_copy (LIST_mirDataGlobal_FLIST);
LIST_mirBasicBlock_FLIST        LIST_mirBasicBlock_FLIST_copy (LIST_mirBasicBlock_FLIST);
LIST_mirLocal_FLIST     LIST_mirLocal_FLIST_copy (LIST_mirLocal_FLIST);
LIST_mirCase_FLIST      LIST_mirCase_FLIST_copy (LIST_mirCase_FLIST);
LIST_mirEXPR_FLIST      LIST_mirEXPR_FLIST_copy (LIST_mirEXPR_FLIST);
LIST_mirALTERNATIVE_FLIST       LIST_mirALTERNATIVE_FLIST_copy (LIST_mirALTERNATIVE_FLIST);
LIST_mirPROCESS_FLIST   LIST_mirPROCESS_FLIST_copy (LIST_mirPROCESS_FLIST);
LIST_mirObject_FLIST    LIST_mirObject_FLIST_create (opcode_t);
void    LIST_mirObject_FLIST_delete (LIST_mirObject_FLIST);
LIST_mirObject_FLIST    LIST_mirObject_FLIST_copy (LIST_mirObject_FLIST);
LIST_mirIterMask_FLIST  LIST_mirIterMask_FLIST_copy (LIST_mirIterMask_FLIST);
LIST_mirEXPR_FLIST      LIST_mirEXPR_FLIST_copy (LIST_mirEXPR_FLIST);
LIST_mirSTMT_FLIST      LIST_mirSTMT_FLIST_copy (LIST_mirSTMT_FLIST);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_create (opcode_t);
void    LIST_mirParameter_FLIST_delete (LIST_mirParameter_FLIST);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_copy (LIST_mirParameter_FLIST);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_copy (LIST_mirParameter_FLIST);
LIST_mirField_FLIST     LIST_mirField_FLIST_copy (LIST_mirField_FLIST);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_copy (LIST_mirCFG_FLIST);
LIST_mirEDGE_FLIST      LIST_mirEDGE_FLIST_copy (LIST_mirEDGE_FLIST);
LIST_Const_FLIST        LIST_Const_FLIST_create (opcode_t);
void    LIST_Const_FLIST_delete (LIST_Const_FLIST);
LIST_Const_FLIST        LIST_Const_FLIST_copy (LIST_Const_FLIST);
LIST_mirBasicBlock_FLIST        LIST_mirBasicBlock_FLIST_copy (LIST_mirBasicBlock_FLIST);
LIST_mirLocal_FLIST     LIST_mirLocal_FLIST_copy (LIST_mirLocal_FLIST);
LIST_mirCase_FLIST      LIST_mirCase_FLIST_copy (LIST_mirCase_FLIST);
LIST_mirEXPR_FLIST      LIST_mirEXPR_FLIST_copy (LIST_mirEXPR_FLIST);
LIST_mirALTERNATIVE_FLIST       LIST_mirALTERNATIVE_FLIST_copy (LIST_mirALTERNATIVE_FLIST);
LIST_mirPROCESS_FLIST   LIST_mirPROCESS_FLIST_copy (LIST_mirPROCESS_FLIST);
LIST_mirIterMask_FLIST  LIST_mirIterMask_FLIST_copy (LIST_mirIterMask_FLIST);
LIST_mirObject_FLIST    LIST_mirObject_FLIST_create (opcode_t);
void    LIST_mirObject_FLIST_delete (LIST_mirObject_FLIST);
LIST_mirObject_FLIST    LIST_mirObject_FLIST_copy (LIST_mirObject_FLIST);
LIST_mirEXPR_FLIST      LIST_mirEXPR_FLIST_copy (LIST_mirEXPR_FLIST);
LIST_mirSTMT_FLIST      LIST_mirSTMT_FLIST_copy (LIST_mirSTMT_FLIST);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_create (opcode_t);
void    LIST_mirParameter_FLIST_delete (LIST_mirParameter_FLIST);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_copy (LIST_mirParameter_FLIST);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_copy (LIST_mirParameter_FLIST);
LIST_mirField_FLIST     LIST_mirField_FLIST_copy (LIST_mirField_FLIST);
LIST_mirEDGE_FLIST      LIST_mirEDGE_FLIST_create (opcode_t);
void    LIST_mirEDGE_FLIST_delete (LIST_mirEDGE_FLIST);
LIST_mirEDGE_FLIST      LIST_mirEDGE_FLIST_copy (LIST_mirEDGE_FLIST);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_create (opcode_t);
void    LIST_mirCFG_FLIST_delete (LIST_mirCFG_FLIST);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_copy (LIST_mirCFG_FLIST);
LIST_mirBasicBlock_FLIST        LIST_mirBasicBlock_FLIST_copy (LIST_mirBasicBlock_FLIST);
LIST_mirLocal_FLIST     LIST_mirLocal_FLIST_copy (LIST_mirLocal_FLIST);
LIST_mirSTMT_FLIST      LIST_mirSTMT_FLIST_copy (LIST_mirSTMT_FLIST);
LIST_mirParameter_FLIST LIST_mirParameter_FLIST_copy (LIST_mirParameter_FLIST);
LIST_mirField_FLIST     LIST_mirField_FLIST_copy (LIST_mirField_FLIST);
LIST_mirALTERNATIVE_FLIST       LIST_mirALTERNATIVE_FLIST_copy (LIST_mirALTERNATIVE_FLIST);
LIST_mirPROCESS_FLIST   LIST_mirPROCESS_FLIST_copy (LIST_mirPROCESS_FLIST);
LIST_mirCase_FLIST      LIST_mirCase_FLIST_copy (LIST_mirCase_FLIST);
LIST_mirIterMask_FLIST  LIST_mirIterMask_FLIST_copy (LIST_mirIterMask_FLIST);
LIST_mirEXPR_FLIST      LIST_mirEXPR_FLIST_copy (LIST_mirEXPR_FLIST);
LIST_mirObject_FLIST    LIST_mirObject_FLIST_copy (LIST_mirObject_FLIST);
LIST_Const_FLIST        LIST_Const_FLIST_copy (LIST_Const_FLIST);
LIST_mirEDGE_FLIST      LIST_mirEDGE_FLIST_copy (LIST_mirEDGE_FLIST);
LIST_mirRegister_FLIST  LIST_mirRegister_FLIST_copy (LIST_mirRegister_FLIST);
LIST_mirPlaced_FLIST    LIST_mirPlaced_FLIST_copy (LIST_mirPlaced_FLIST);
LIST_mirProcGlobal_FLIST        LIST_mirProcGlobal_FLIST_copy (LIST_mirProcGlobal_FLIST);
LIST_mirDataGlobal_FLIST        LIST_mirDataGlobal_FLIST_copy (LIST_mirDataGlobal_FLIST);
LIST_mirSection_FLIST   LIST_mirSection_FLIST_copy (LIST_mirSection_FLIST);
LIST_mirTYPE_FLIST      LIST_mirTYPE_FLIST_copy (LIST_mirTYPE_FLIST);
LIST_Symbols_FLIST      LIST_Symbols_FLIST_copy (LIST_Symbols_FLIST);
LIST_mirCFG_FLIST       LIST_mirCFG_FLIST_copy (LIST_mirCFG_FLIST);
LIST_mirIntraCfg_FLIST  LIST_mirIntraCfg_FLIST_copy (LIST_mirIntraCfg_FLIST);
char * INT_get_value ( INT );
char * UNIV_REAL_get_value ( f80_t );
char * UNIV_INT_get_value ( x64_t );
char * UNIV_ADDRESS_get_value ( x64_t );
void mirObject_print_stdout ( mirObject );
typedef enum {
        o_tt = 0,
        o_ff = 1,
        o_norm = 2,
o_edges_dummy = 4
} o_edges;
typedef struct {
         o_edges von;
         o_edges nach;
} o_edges_kante;
typedef enum { o_edges_forward,o_edges_backward } o_edges_direction;
extern o_edges o_edges_top;
extern o_edges o_edges_bottom;
extern long o_edges_power;
typedef struct o_edges_list_base_ 
{
        o_edges head;
        struct o_edges_list_base_ *tail;
} *o_edges_list,_o_edges_list;
extern long o_edges_list_power;
void o_edges_list_init(void);
void o_edges_list_destroy_whole(o_edges_list);
void o_edges_list_destroy(o_edges_list);
o_edges_list o_edges_list_new();
o_edges_list o_edges_list_extend(o_edges head, o_edges_list tail);
o_edges_list o_edges_list_conc ( o_edges_list, o_edges_list );
int o_edges_list_length ( o_edges_list );
o_edges o_edges_list_head ( o_edges_list );
o_edges_list o_edges_list_tail ( );
o_edges o_edges_list_get_elem ( o_edges_list, int );
o_edges_list o_edges_list_set_elem ( o_edges_list, int, o_edges );
o_edges_list o_edges_list_insert_bevor ( );
int  o_edges_list_is_empty ( );
void o_edges_list_print_stdout ( o_edges_list );
o_edges_list o_edges_list_append ( o_edges_list, o_edges );
o_edges_list o_edges_list_dublicate_whole ( o_edges_list );
o_edges_list o_edges_list_dublicate ( o_edges_list );
int  o_edges_list_is_elem ( o_edges_list, o_edges );
o_edges_list o_edges_list_remove ( o_edges_list, o_edges );
char *o_edges_list_print ( o_edges_list );
unsigned int o_edges_list_hash(o_edges_list);
char *o_edges_list_vcgprint ( o_edges_list );
int  o_edges_list_eq ( o_edges_list, o_edges_list );
o_edges_list o_edges_list_rest ( o_edges_list, o_edges_list );
o_edges_list o_edges_list_vereinigung ( o_edges_list, o_edges_list );
o_edges_list o_edges_list_schnitt ( o_edges_list, o_edges_list );
void o_edges_test(o_edges x);
void o_edges_init(void);
unsigned int o_edges_hash(o_edges x)
;
char *o_edges_len(void);
int  o_edges_istop(o_edges x);
int  o_edges_isbottom(o_edges x);
static o_edges_list o_edges_in_1_step(o_edges x,o_edges_direction d);
static o_edges_list o_edges_in_n_step(o_edges x,int n,o_edges_direction d);
static o_edges o_edges_lgub(o_edges e1, o_edges e2, o_edges_direction d);
o_edges o_edges_glb(o_edges e1, o_edges e2);
o_edges o_edges_lub(o_edges e1, o_edges e2);
int  o_edges_le(o_edges e1, o_edges e2);
int  o_edges_ge(o_edges e1, o_edges e2);
int  o_edges_eq(o_edges e1, o_edges e2);
int  o_edges_neq(o_edges e1, o_edges e2);
o_edges o_edges_dublicate_whole(o_edges e);
o_edges o_edges_dublicate(o_edges e);
void o_edges_print_stdout(o_edges e);
char *o_edges_print(o_edges e);
char *o_edges_vcgprint(o_edges e);
void o_edges_destroy(o_edges e);
void o_edges_destroy_whole(o_edges e);
typedef enum {
        o_nix = 0,
o_noth_dom_dummy = 2
} o_noth_dom;
typedef struct {
         o_noth_dom von;
         o_noth_dom nach;
} o_noth_dom_kante;
typedef enum { o_noth_dom_forward,o_noth_dom_backward } o_noth_dom_direction;
extern o_noth_dom o_noth_dom_top;
extern o_noth_dom o_noth_dom_bottom;
extern long o_noth_dom_power;
typedef struct o_noth_dom_list_base_ 
{
        o_noth_dom head;
        struct o_noth_dom_list_base_ *tail;
} *o_noth_dom_list,_o_noth_dom_list;
extern long o_noth_dom_list_power;
void o_noth_dom_list_init(void);
void o_noth_dom_list_destroy_whole(o_noth_dom_list);
void o_noth_dom_list_destroy(o_noth_dom_list);
o_noth_dom_list o_noth_dom_list_new();
o_noth_dom_list o_noth_dom_list_extend(o_noth_dom head, o_noth_dom_list tail);
o_noth_dom_list o_noth_dom_list_conc ( o_noth_dom_list, o_noth_dom_list );
int o_noth_dom_list_length ( o_noth_dom_list );
o_noth_dom o_noth_dom_list_head ( o_noth_dom_list );
o_noth_dom_list o_noth_dom_list_tail ( );
o_noth_dom o_noth_dom_list_get_elem ( o_noth_dom_list, int );
o_noth_dom_list o_noth_dom_list_set_elem ( o_noth_dom_list, int, o_noth_dom );
o_noth_dom_list o_noth_dom_list_insert_bevor ( );
int  o_noth_dom_list_is_empty ( );
void o_noth_dom_list_print_stdout ( o_noth_dom_list );
o_noth_dom_list o_noth_dom_list_append ( o_noth_dom_list, o_noth_dom );
o_noth_dom_list o_noth_dom_list_dublicate_whole ( o_noth_dom_list );
o_noth_dom_list o_noth_dom_list_dublicate ( o_noth_dom_list );
int  o_noth_dom_list_is_elem ( o_noth_dom_list, o_noth_dom );
o_noth_dom_list o_noth_dom_list_remove ( o_noth_dom_list, o_noth_dom );
char *o_noth_dom_list_print ( o_noth_dom_list );
unsigned int o_noth_dom_list_hash(o_noth_dom_list);
char *o_noth_dom_list_vcgprint ( o_noth_dom_list );
int  o_noth_dom_list_eq ( o_noth_dom_list, o_noth_dom_list );
o_noth_dom_list o_noth_dom_list_rest ( o_noth_dom_list, o_noth_dom_list );
o_noth_dom_list o_noth_dom_list_vereinigung ( o_noth_dom_list, o_noth_dom_list );
o_noth_dom_list o_noth_dom_list_schnitt ( o_noth_dom_list, o_noth_dom_list );
void o_noth_dom_test(o_noth_dom x);
void o_noth_dom_init(void);
unsigned int o_noth_dom_hash(o_noth_dom x)
;
char *o_noth_dom_len(void);
int  o_noth_dom_istop(o_noth_dom x);
int  o_noth_dom_isbottom(o_noth_dom x);
static o_noth_dom_list o_noth_dom_in_1_step(o_noth_dom x,o_noth_dom_direction d);
static o_noth_dom_list o_noth_dom_in_n_step(o_noth_dom x,int n,o_noth_dom_direction d);
static o_noth_dom o_noth_dom_lgub(o_noth_dom e1, o_noth_dom e2, o_noth_dom_direction d);
o_noth_dom o_noth_dom_glb(o_noth_dom e1, o_noth_dom e2);
o_noth_dom o_noth_dom_lub(o_noth_dom e1, o_noth_dom e2);
int  o_noth_dom_le(o_noth_dom e1, o_noth_dom e2);
int  o_noth_dom_ge(o_noth_dom e1, o_noth_dom e2);
int  o_noth_dom_eq(o_noth_dom e1, o_noth_dom e2);
int  o_noth_dom_neq(o_noth_dom e1, o_noth_dom e2);
o_noth_dom o_noth_dom_dublicate_whole(o_noth_dom e);
o_noth_dom o_noth_dom_dublicate(o_noth_dom e);
void o_noth_dom_print_stdout(o_noth_dom e);
char *o_noth_dom_print(o_noth_dom e);
char *o_noth_dom_vcgprint(o_noth_dom e);
void o_noth_dom_destroy(o_noth_dom e);
void o_noth_dom_destroy_whole(o_noth_dom e);
typedef x64_t *snum; 
extern long snum_power;
typedef f80_t real; 
extern long real_power;
typedef x64_t *unum; 
extern long unum_power;
char *unum_len ( void );
unsigned int unum_hash ( unum );
void unum_init ( void );
x64_t unum_to_x64 ( unum );
unum unum_to_unum ( unum );
real unum_to_real ( unum );
snum unum_to_snum ( unum );
unum unum_add ( unum, unum );
unum unum_dublicate ( unum );
unum unum_dublicate_whole ( unum );
unum unum_abs ( unum );
int unum_eq ( unum, unum );
int unum_neq ( unum, unum );
unum unum_create ( char * );
void unum_free ( unum );
void unum_destroy_whole ( unum );
char *unum_print ( unum );
char *unum_vcgprint ( unum );
void unum_print_stdout ( unum );
int  unum_le ( unum, unum );
int  unum_leq ( unum, unum );
int  unum_ge ( unum, unum );
int  unum_geq ( unum, unum );
unum unum_modulo ( unum, unum );
unum unum_mal ( unum, unum );
unum unum_min ( unum, unum );
unum unum_hoch ( unum, unum );
unum unum_geteilt ( unum, unum );
unum unum_unplus ( unum );
unum unum_unmin ( unum );
unum unum_rnd ( unum );
unum unum_bor ( unum, unum );
unum unum_band ( unum, unum );
snum unum_sgn ( unum );
unsigned int unum_to_unsigned ( unum );
unum int_to_unum ( int );
unsigned int real_hash ( real );
char *real_len ( void );
void real_init ( void );
real real_add ( real, real );
real real_dublicate ( real );
real real_dublicate_whole ( real );
int  real_eq ( real, real );
int  real_neq ( real, real );
snum real_to_snum ( real );
unum real_to_unum ( real );
snum real_trunc ( real );
real real_create ( char * );
void real_free ( real );
void real_destroy_whole ( real );
char *real_print ( real );
char *real_vcgprint ( real );
void real_print_stdout ( real );
int  real_le ( real, real );
int  real_leq ( real, real );
int  real_ge ( real, real );
int  real_geq ( real, real );
real real_mal ( real, real );
real real_min ( real, real );
real real_hoch ( real, real );
real real_geteilt ( real, real );
real real_unplus ( real );
real real_unmin ( real );
real real_abs ( real );
real real_modulo ( real, real );
real real_to_real ( real );
snum real_sgn ( real );
f80_t real_to_f80 ( real );
char *snum_len ( void );
unsigned int snum_hash ( snum );
void snum_init ( void );
x64_t snum_to_x64 ( unum );
unum snum_to_unum ( snum );
real snum_to_real ( snum );
snum snum_add ( snum, snum );
snum snum_dublicate ( snum );
snum snum_dublicate_whole ( snum );
snum snum_abs ( snum );
int snum_eq ( snum, snum );
int snum_neq ( snum, snum );
snum snum_create ( char * );
void snum_free ( snum );
void snum_destroy_whole ( snum );
char *snum_print ( snum );
char *snum_vcgprint ( snum );
void snum_print_stdout ( snum );
int  snum_le ( snum, snum );
int  snum_leq ( snum, snum );
int  snum_ge ( snum, snum );
int  snum_geq ( snum, snum );
snum snum_modulo ( snum, snum );
snum snum_mal ( snum, snum );
snum snum_min ( snum, snum );
snum snum_hoch ( snum, snum );
snum snum_geteilt ( snum, snum );
snum snum_unplus ( snum );
snum snum_unmin ( snum );
snum snum_rnd ( snum );
snum snum_sgn ( snum );
snum snum_bor ( snum, snum );
snum snum_band ( snum, snum );
snum snum_to_snum ( snum );
typedef enum {
        o_const_val_none_0_flag = 0
        ,o_const_val_bool_1_flag = 1
        ,o_const_val_snum_2_flag = 2
        ,o_const_val_real_3_flag = 3
        ,o_const_val_o_noth_dom_4_flag = 4
        ,o_const_val_unum_5_flag = 5
} o_const_val_flag_type;
typedef union {
        bool bool_1_sel;
        snum snum_2_sel;
        real real_3_sel;
        o_noth_dom o_noth_dom_4_sel;
        unum unum_5_sel;
} o_const_val_union_type;
typedef struct {
        o_const_val_flag_type flag;
        o_const_val_union_type elem;
} *o_const_val, _o_const_val;
extern o_const_val o_const_val_top;
extern o_const_val o_const_val_bottom;
extern long o_const_val_power;
char *o_const_val_len(void);
void o_const_val_init(void);
unsigned int o_const_val_hash(o_const_val x) ;
o_const_val o_const_val_create_from_1(bool elem);
o_const_val o_const_val_create_from_2(snum elem);
o_const_val o_const_val_create_from_3(real elem);
o_const_val o_const_val_create_from_4(o_noth_dom elem);
o_const_val o_const_val_create_from_5(unum elem);
static o_const_val o_const_val_new(void);
int o_const_val_selector(o_const_val x);
int  o_const_val_is_1(o_const_val x);
int  o_const_val_is_2(o_const_val x);
int  o_const_val_is_3(o_const_val x);
int  o_const_val_is_4(o_const_val x);
int  o_const_val_is_5(o_const_val x);
bool o_const_val_down_to_1(o_const_val x);
snum o_const_val_down_to_2(o_const_val x);
real o_const_val_down_to_3(o_const_val x);
o_noth_dom o_const_val_down_to_4(o_const_val x);
unum o_const_val_down_to_5(o_const_val x);
int  o_const_val_eq(o_const_val x1, o_const_val x2);
static o_const_val _o_const_val_dublicate(o_const_val x,int  whole_mode);
o_const_val o_const_val_dublicate_whole(o_const_val x);
o_const_val o_const_val_dublicate(o_const_val x);
void o_const_val_print_stdout(o_const_val x);
char *o_const_val_print(o_const_val x);
char *o_const_val_vcgprint(o_const_val x);
void o_const_val_destroy(o_const_val x);
void o_const_val_destroy_whole(o_const_val x);
int  o_const_val_le(o_const_val x1, o_const_val x2);
typedef struct {
        unum unum_1_comp;
        bool bool_2_comp;
} *o_numbool, _o_numbool;
extern long o_numbool_power;
void o_numbool_init(void);
unsigned int o_numbool_hash(o_numbool x) ;
o_numbool o_numbool_create(unum c1, bool c2  );
static o_numbool o_numbool_new(void);
o_numbool o_numbool_update_1(o_numbool tup, unum x);
o_numbool o_numbool_update_2(o_numbool tup, bool x);
unum o_numbool_select_1(o_numbool tup);
bool o_numbool_select_2(o_numbool tup);
int  o_numbool_eq(o_numbool x1, o_numbool x2);
o_numbool o_numbool_dublicate(o_numbool x);
o_numbool o_numbool_dublicate_whole(o_numbool x);
static o_numbool _o_numbool_dublicate(o_numbool x,int  whole_mode);
void o_numbool_print_stdout(o_numbool x);
char *o_numbool_print(o_numbool x);
char *o_numbool_vcgprint(o_numbool x);
void o_numbool_destroy(o_numbool x);
void o_numbool_destroy_whole(o_numbool x);
typedef enum { o_t1_top_class = 1 , o_t1_bottom_class = 2, o_t1_norm_class = 3} 
        o_t1_class;
typedef struct {
        unum inh;
   o_t1_class flag; 
} *o_t1,_o_t1;
extern o_t1 o_t1_top;
extern o_t1 o_t1_bottom;
extern long o_t1_power;
char *o_t1_len(void);
void o_t1_init ( void );
void o_t1_destroy ( o_t1 );
unsigned int o_t1_hash(o_t1);
void o_t1_destroy_whole ( o_t1 );
o_t1 o_t1_dublicate_whole ( o_t1 );
o_t1 o_t1_dublicate ( o_t1 );
char *o_t1_print ( o_t1 );
char *o_t1_vcgprint ( o_t1 );
void o_t1_print_stdout ( o_t1 );
int  o_t1_isbottom ( o_t1 );
int  o_t1_istop ( o_t1 );
int  o_t1_eq ( o_t1, o_t1 );
int  o_t1_neq ( o_t1, o_t1 );
o_t1 o_t1_lift ( unum );
o_t1 o_t1_glb ( o_t1, o_t1 );
o_t1 o_t1_lub ( o_t1, o_t1 );
int  o_t1_le ( o_t1, o_t1 );
unum o_t1_drop ( o_t1 );
typedef struct o_alias_list_base_ {
        int howmany;
        unum source[100 ];
        o_t1 target[100 ];
        struct o_alias_list_base_ *next;
        struct o_alias_list_base_ *prev;
} *o_alias_li,_o_alias_li;
typedef struct {
        o_t1 def;
        o_alias_li head;
} *o_alias,_o_alias;
        
extern o_alias o_alias_top;
extern o_alias o_alias_bottom;
extern long o_alias_power;
char *o_alias_len(void);
void o_alias_init ( void );
void o_alias_destroy ( o_alias );
unsigned int o_alias_hash(o_alias);
void o_alias_destroy_whole ( o_alias );
char * o_alias_print ( o_alias );
char * o_alias_vcgprint ( o_alias );
void o_alias_print_stdout ( o_alias );
o_alias o_alias_update ( o_alias, unum, o_t1 );
o_alias o_alias_dublicate ( o_alias );
o_alias o_alias_dublicate_whole ( o_alias );
int  o_alias_eq ( o_alias, o_alias );
o_alias o_alias_create ( o_t1 );
o_t1 o_alias_get ( o_alias, unum );
int  o_alias_isbottom ( o_alias );
int  o_alias_istop ( o_alias );
o_alias o_alias_lub ( o_alias, o_alias );
o_alias o_alias_glb ( o_alias, o_alias );
int  o_alias_le ( o_alias, o_alias );
typedef enum { o_va_top_class = 1 , o_va_bottom_class = 2, o_va_norm_class = 3} 
        o_va_class;
typedef struct {
        o_const_val inh;
   o_va_class flag; 
} *o_va,_o_va;
extern o_va o_va_top;
extern o_va o_va_bottom;
extern long o_va_power;
char *o_va_len(void);
void o_va_init ( void );
void o_va_destroy ( o_va );
unsigned int o_va_hash(o_va);
void o_va_destroy_whole ( o_va );
o_va o_va_dublicate_whole ( o_va );
o_va o_va_dublicate ( o_va );
char *o_va_print ( o_va );
char *o_va_vcgprint ( o_va );
void o_va_print_stdout ( o_va );
int  o_va_isbottom ( o_va );
int  o_va_istop ( o_va );
int  o_va_eq ( o_va, o_va );
int  o_va_neq ( o_va, o_va );
o_va o_va_lift ( o_const_val );
o_va o_va_glb ( o_va, o_va );
o_va o_va_lub ( o_va, o_va );
int  o_va_le ( o_va, o_va );
o_const_val o_va_drop ( o_va );
typedef struct o_const_fkt_list_base_ {
        int howmany;
        unum source[100 ];
        o_va target[100 ];
        struct o_const_fkt_list_base_ *next;
        struct o_const_fkt_list_base_ *prev;
} *o_const_fkt_li,_o_const_fkt_li;
typedef struct {
        o_va def;
        o_const_fkt_li head;
} *o_const_fkt,_o_const_fkt;
        
extern o_const_fkt o_const_fkt_top;
extern o_const_fkt o_const_fkt_bottom;
extern long o_const_fkt_power;
char *o_const_fkt_len(void);
void o_const_fkt_init ( void );
void o_const_fkt_destroy ( o_const_fkt );
unsigned int o_const_fkt_hash(o_const_fkt);
void o_const_fkt_destroy_whole ( o_const_fkt );
char * o_const_fkt_print ( o_const_fkt );
char * o_const_fkt_vcgprint ( o_const_fkt );
void o_const_fkt_print_stdout ( o_const_fkt );
o_const_fkt o_const_fkt_update ( o_const_fkt, unum, o_va );
o_const_fkt o_const_fkt_dublicate ( o_const_fkt );
o_const_fkt o_const_fkt_dublicate_whole ( o_const_fkt );
int  o_const_fkt_eq ( o_const_fkt, o_const_fkt );
o_const_fkt o_const_fkt_create ( o_va );
o_va o_const_fkt_get ( o_const_fkt, unum );
int  o_const_fkt_isbottom ( o_const_fkt );
int  o_const_fkt_istop ( o_const_fkt );
o_const_fkt o_const_fkt_lub ( o_const_fkt, o_const_fkt );
o_const_fkt o_const_fkt_glb ( o_const_fkt, o_const_fkt );
int  o_const_fkt_le ( o_const_fkt, o_const_fkt );
typedef struct {
        o_const_fkt o_const_fkt_1_comp;
        o_va o_va_2_comp;
        o_const_fkt o_const_fkt_3_comp;
        o_alias o_alias_4_comp;
        o_alias o_alias_5_comp;
} *o_dfi, _o_dfi;
extern o_dfi o_dfi_top;
extern o_dfi o_dfi_bottom;
extern long o_dfi_power;
void o_dfi_init(void);
unsigned int o_dfi_hash(o_dfi x) ;
char *o_dfi_len(void);
o_dfi o_dfi_create(o_const_fkt c1, o_va c2, o_const_fkt c3, o_alias c4, o_alias c5  );
static o_dfi o_dfi_new(void);
o_dfi o_dfi_update_1(o_dfi tup, o_const_fkt x);
o_dfi o_dfi_update_2(o_dfi tup, o_va x);
o_dfi o_dfi_update_3(o_dfi tup, o_const_fkt x);
o_dfi o_dfi_update_4(o_dfi tup, o_alias x);
o_dfi o_dfi_update_5(o_dfi tup, o_alias x);
o_const_fkt o_dfi_select_1(o_dfi tup);
o_va o_dfi_select_2(o_dfi tup);
o_const_fkt o_dfi_select_3(o_dfi tup);
o_alias o_dfi_select_4(o_dfi tup);
o_alias o_dfi_select_5(o_dfi tup);
int  o_dfi_eq(o_dfi x1, o_dfi x2);
o_dfi o_dfi_dublicate(o_dfi x);
o_dfi o_dfi_dublicate_whole(o_dfi x);
static o_dfi _o_dfi_dublicate(o_dfi x,int  whole_mode);
void o_dfi_print_stdout(o_dfi x);
char *o_dfi_print(o_dfi x);
char *o_dfi_vcgprint(o_dfi x);
void o_dfi_destroy(o_dfi x);
void o_dfi_destroy_whole(o_dfi x);
int  o_dfi_le(o_dfi x1, o_dfi x2);
int  o_dfi_leq(o_dfi x1, o_dfi x2);
o_dfi o_dfi_glb(o_dfi x1, o_dfi x2);
o_dfi o_dfi_lub(o_dfi x1, o_dfi x2);
int  o_dfi_istop(o_dfi x1);
int  o_dfi_isbottom(o_dfi x1);
typedef enum { o_ddfi_top_class = 1 , o_ddfi_bottom_class = 2, o_ddfi_norm_class = 3} 
        o_ddfi_class;
typedef struct {
        o_dfi inh;
   o_ddfi_class flag; 
} *o_ddfi,_o_ddfi;
extern o_ddfi o_ddfi_top;
extern o_ddfi o_ddfi_bottom;
extern long o_ddfi_power;
unsigned int o_ddfi_hash(o_ddfi x) ;
char *o_ddfi_len(void);
void o_ddfi_init ( void );
void o_ddfi_destroy ( o_ddfi );
void o_ddfi_destroy_whole ( o_ddfi );
o_ddfi o_ddfi_dublicate_whole ( o_ddfi );
o_ddfi o_ddfi_dublicate ( o_ddfi );
char *o_ddfi_print ( o_ddfi );
char *o_ddfi_vcgprint ( o_ddfi );
void o_ddfi_print_stdout ( o_ddfi );
int  o_ddfi_isbottom ( o_ddfi );
int  o_ddfi_istop ( o_ddfi );
int  o_ddfi_eq ( o_ddfi, o_ddfi );
int  o_ddfi_neq ( o_ddfi, o_ddfi );
o_ddfi o_ddfi_lift ( o_dfi );
o_ddfi o_ddfi_lub ( o_ddfi, o_ddfi );
o_ddfi o_ddfi_glb ( o_ddfi, o_ddfi );
int  o_ddfi_le ( o_ddfi, o_ddfi );
o_dfi o_ddfi_drop ( o_ddfi );
typedef char* str; 
extern long str_power; 
char *str_len ( void );
unsigned int str_hash ( str );
void str_init ( void );
str str_conc ( str, str );
str str_dublicate ( str );
str str_dublicate_whole ( str );
int str_eq ( str, str );
int str_neq ( str, str );
str str_create ( char * );
void str_free ( str );
void str_destroy_whole ( str );
char *str_print ( str );
char *str_vcgprint ( str );
void str_print_stdout ( str );
int  str_le ( str, str );
int  str_leq ( str, str );
int  str_ge ( str, str );
int  str_geq ( str, str );
int  str_is_top ( str );
int  str_is_bottom ( str );
str str_top ( void );
str str_bottom ( void );
str str_lub ( str, str );
str str_glb ( str, str );
typedef enum {
        CALL, RETURN, START, END, INNER
}               KFG_NODE_TYPE;
typedef int     KFG_NODE_ID;     
typedef mirCFG  KFG_NODE;        
typedef mirEDGE KFG_EDGE;        
typedef mirIntraCfg KFG;
typedef LIST_mirCFG KFG_NODE_LIST;       
typedef LIST_mirEDGE KFG_EDGE_LIST;      
typedef enum {
        kfg_edge_true = 0,
        kfg_edge_false,
        kfg_edge_normal
}               KFG_EDGE_TYPE;
KFG_NODE_LIST   _kfg_cfgnodes;
static KFG K = 0;
extern int      _kfg_cfgnumnodes;
extern LIST_mirIntraCfg _kfg_cfgs;
extern LIST_mirCFG _kfg_starts, _kfg_ends, _kfg_calls, _kfg_returns;
extern KFG      _kfg_k;
extern KFG_NODE *_kfg_table;
 KFG kfg_get ( void );
KFG kfg_create ( mirUnit );
 KFG_NODE kfg_entry ( KFG );
 KFG_NODE_LIST kfg_entrys ( KFG );
 KFG_NODE_LIST kfg_calls ( KFG );
 KFG_NODE_LIST kfg_returns ( KFG );
 KFG_NODE_LIST kfg_exits ( KFG );
 KFG_NODE kfg_exit ( KFG );
 KFG_NODE_LIST kfg_predecessors ( KFG, KFG_NODE );
 KFG_NODE_LIST kfg_successors ( KFG, KFG_NODE );
 KFG_NODE node_list_head ( KFG_NODE_LIST );
 KFG_NODE_LIST node_list_tail ( KFG_NODE_LIST );
 int  node_list_is_empty ( KFG_NODE_LIST );
 int node_list_length ( KFG_NODE_LIST );
 KFG_NODE_LIST kfg_all_nodes ( KFG );
 KFG_NODE kfg_get_node ( KFG, KFG_NODE_ID );
 KFG_NODE_ID kfg_get_id ( KFG, KFG_NODE );
 int kfg_anz_node ( KFG );
 KFG_NODE_TYPE kfg_node_type ( KFG, KFG_NODE );
 int get_caller ( KFG_NODE );
 KFG_NODE kfg_get_call ( KFG, KFG_NODE );
 KFG_NODE kfg_get_return ( KFG, KFG_NODE );
 KFG_NODE kfg_get_startend ( KFG, KFG_NODE, int  );
 KFG_NODE kfg_get_start ( KFG, KFG_NODE );
 KFG_NODE kfg_get_end ( KFG, KFG_NODE );
 int kfg_proc_anz ( KFG );
 int kfg_edge_type_max ( void );
 int kfg_max_edge ( void );
 KFG_EDGE_TYPE kfg_edge_type ( KFG_NODE, KFG_NODE );
 int kfg_which_edges ( KFG_NODE );
 int kfg_bitpos ( int, KFG_EDGE_TYPE );
 int kfg_bit_anz ( KFG_NODE );
o_ddfi transfer( o_ddfi p_0, KFG_NODE p_2,o_edges p_1);
o_ddfi transfer_( o_ddfi p_3,mirSTMT p_4,o_edges p_5 );
void transfer__mark(void); 
void transfer__stat(void);
o_va o_mycast( o_va p_6,snum p_7 );
void o_mycast_mark(void); 
void o_mycast_stat(void);
str o_print_type( mirTYPE p_8 );
void o_print_type_mark(void); 
void o_print_type_stat(void);
snum o_which_type( mirTYPE p_9 );
void o_which_type_mark(void); 
void o_which_type_stat(void);
str o_print_const_val_type( o_const_val p_10 );
void o_print_const_val_type_mark(void); 
void o_print_const_val_type_stat(void);
snum o_const_val_type( o_const_val p_11 );
void o_const_val_type_mark(void); 
void o_const_val_type_stat(void);
typedef struct o_myid_struc {
        INT _1;
        unum res;
        struct o_myid_struc *next;
} o_myid_MEMO,*o_myid_MEMOP;
unum o_myid( INT p_12 );
void o_myid_mark(void); 
void o_myid_stat(void);
o_va o_value( mirEXPR p_13,o_const_fkt p_14,o_alias p_15 );
void o_value_mark(void); 
void o_value_stat(void);
typedef struct o_is_signed_struc {
        mirTYPE _1;
        bool res;
        struct o_is_signed_struc *next;
} o_is_signed_MEMO,*o_is_signed_MEMOP;
bool o_is_signed( mirTYPE p_16 );
void o_is_signed_mark(void); 
void o_is_signed_stat(void);
o_const_fkt o_uninitfunc( );
void o_uninitfunc_mark(void); 
void o_uninitfunc_stat(void);
o_va o_uninit( );
void o_uninit_mark(void); 
void o_uninit_stat(void);
typedef struct o_type_struc {
        mirEXPR _1;
        mirTYPE res;
        struct o_type_struc *next;
} o_type_MEMO,*o_type_MEMOP;
mirTYPE o_type( mirEXPR p_17 );
void o_type_mark(void); 
void o_type_stat(void);
typedef struct o_is_simple_type_struc {
        mirTYPE _1;
        bool res;
        struct o_is_simple_type_struc *next;
} o_is_simple_type_MEMO,*o_is_simple_type_MEMOP;
bool o_is_simple_type( mirTYPE p_18 );
void o_is_simple_type_mark(void); 
void o_is_simple_type_stat(void);
typedef struct o_putin_struc {
        LIST_mirObject _1;
        o_alias _2;
        o_alias _3;
        unum _4;
        o_alias res;
        struct o_putin_struc *next;
} o_putin_MEMO,*o_putin_MEMOP;
o_alias o_putin( LIST_mirObject p_19,o_alias p_20,o_alias p_21,unum p_22 );
void o_putin_mark(void); 
void o_putin_stat(void);
o_const_fkt o_fill_pars( o_const_fkt p_23,LIST_mirObject p_24,o_const_fkt p_25,unum p_26 );
void o_fill_pars_mark(void); 
void o_fill_pars_stat(void);
o_const_fkt o_init_locals( o_const_fkt p_27,LIST_mirObject p_28 );
void o_init_locals_mark(void); 
void o_init_locals_stat(void);
o_alias o_eval_objs( LIST_mirEXPR p_29,o_alias p_30,unum p_31 );
void o_eval_objs_mark(void); 
void o_eval_objs_stat(void);
o_const_fkt o_eval_params( LIST_mirEXPR p_32,o_const_fkt p_33,unum p_34 );
void o_eval_params_mark(void); 
void o_eval_params_stat(void);
typedef struct o_id_struc {
        mirObject _1;
        unum res;
        struct o_id_struc *next;
} o_id_MEMO,*o_id_MEMOP;
unum o_id( mirObject p_35 );
void o_id_mark(void); 
void o_id_stat(void);
typedef struct o_is_volatile_struc {
        mirObject _1;
        bool res;
        struct o_is_volatile_struc *next;
} o_is_volatile_MEMO,*o_is_volatile_MEMOP;
bool o_is_volatile( mirObject p_36 );
void o_is_volatile_mark(void); 
void o_is_volatile_stat(void);
o_numbool o_get_objinfo( mirObject p_37 );
void o_get_objinfo_mark(void); 
void o_get_objinfo_stat(void);
o_const_fkt o_erase_var( o_const_fkt p_38,LIST_mirObject p_39 );
void o_erase_var_mark(void); 
void o_erase_var_stat(void);
o_ddfi o_comb( o_ddfi p_40,o_ddfi p_41 );
void o_comb_mark(void); 
void o_comb_stat(void);
o_const_fkt o_built( o_const_fkt p_42,o_const_fkt p_43,LIST_mirObject p_44,LIST_mirObject p_45 );
void o_built_mark(void); 
void o_built_stat(void);
o_const_fkt o_upd( o_const_fkt p_46,o_const_fkt p_47,LIST_mirObject p_48 );
void o_upd_mark(void); 
void o_upd_stat(void);
o_const_fkt o_enrich( o_const_fkt p_49,LIST_mirEXPR p_50 );
void o_enrich_mark(void); 
void o_enrich_stat(void);
o_ddfi o_www( o_ddfi p_51,o_ddfi p_52 );
void o_www_mark(void); 
void o_www_stat(void);
void all_mark(void);
void all_stat(void);
extern KFG      g;
void 
all_init(void)
{
        o_ddfi_init();
        o_dfi_init();
        o_const_fkt_init();
        o_va_init();
        o_alias_init();
        o_t1_init();
        o_numbool_init();
        o_const_val_init();
        o_noth_dom_init();
        o_edges_init();
        snum_init();
        unum_init();
        real_init();
        bool_init();
        chr_init();
        str_init();
}
o_ddfi 
transfer(o_ddfi p_0, KFG_NODE p_2, o_edges p_1)
{
        return (transfer_(p_0, ((       0 , ((struct x_Sort10_mirCFG *)((   p_2   ) ))->f_Stat) ) , p_1));
}
o_ddfi 
transfer_(o_ddfi p_3, mirSTMT p_4, o_edges p_5)
{
        {
                o_ddfi          v_0;
                mirEXPR         v_1;
                mirEXPR         v_2;
                mirObject       v_3;
                mirTYPE         v_4;
                mirObject       v_5;
                mirTYPE         v_6;
                o_ddfi          v_7;
                o_dfi           v_8;
                o_dfi           v_9;
                o_alias         v_10;
                o_alias         v_11;
                BOOL            v_12;
                mirEXPR         v_13;
                mirSection      v_14;
                NAME            v_15;
                BOOL            v_16;
                mirTYPE         v_17;
                INT             v_18;
                int             v_19;
                o_const_fkt     v_20;
                o_va            v_21;
                o_const_fkt     v_22;
                int             v_23;
                o_ddfi          v_24;
                o_dfi           v_25;
                int             v_26;
                int             v_27;
                mirEXPR         v_28;
                mirTYPE         v_29;
                o_ddfi          v_30;
                o_dfi           v_31;
                int             v_32;
                mirEXPR         v_33;
                mirEXPR         v_34;
                mirEXPR         v_35;
                o_ddfi          v_36;
                o_dfi           v_37;
                o_va            v_38;
                int             v_39;
                o_ddfi          v_40;
                o_dfi           v_41;
                o_va            v_42;
                int             v_43;
                mirEXPR         v_44;
                LIST_mirEXPR    v_45;
                mirEXPR         v_46;
                o_ddfi          v_47;
                o_dfi           v_48;
                int             v_49;
                LIST_mirEXPR    v_50;
                mirEXPR         v_51;
                o_ddfi          v_52;
                o_dfi           v_53;
                int             v_54;
                LIST_mirEXPR    v_55;
                mirEXPR         v_56;
                o_ddfi          v_57;
                o_dfi           v_58;
                int             v_59;
                LIST_mirObject  v_60;
                LIST_mirObject  v_61;
                o_ddfi          v_62;
                o_dfi           v_63;
                int             v_64;
                mirEXPR         v_65;
                mirEXPR         v_66;
                o_ddfi          v_67;
                o_dfi           v_68;
                int             v_69;
                mirEXPR         v_70;
                mirObject       v_71;
                mirTYPE         v_72;
                o_ddfi          v_73;
                o_dfi           v_74;
                int             v_75;
                LIST_mirObject  v_76;
                LIST_mirObject  v_77;
                o_ddfi          v_78;
                o_dfi           v_79;
                int             v_80;
                LIST_mirObject  v_81;
                LIST_mirObject  v_82;
                o_ddfi          v_83;
                o_dfi           v_84;
                int             v_85;
                o_ddfi          v_86;
                o_dfi           v_87;
                int             v_88;
                 
                if (((((DMCP_xnode)(   p_4   ) )->op) )  == xop_Sort10_mirAssign ) {
                        v_1 = ( 0 , ((struct x_Sort10_mirAssign *)((  p_4  ) ))->f_Rhs) ;
                        v_2 = ( 0 , ((struct x_Sort10_mirAssign *)((  p_4  ) ))->f_Lhs) ;
                         
                        if (((((DMCP_xnode)(   v_2   ) )->op) )  == xop_Sort10_mirObjectAddr ) {
                                v_3 = ( 0 , ((struct x_Sort10_mirObjectAddr *)((  v_2  ) ))->f_Obj) ;
                                v_4 = ( 0 , ((struct x_Sort10_mirObjectAddr *)((  v_2  ) ))->f_Type) ;
                                 
                                 
                                if (((((DMCP_xnode)(   v_1   ) )->op) )  == xop_Sort10_mirObjectAddr ) {
                                        v_5 = ( 0 , ((struct x_Sort10_mirObjectAddr *)((  v_1  ) ))->f_Obj) ;
                                        v_6 = ( 0 , ((struct x_Sort10_mirObjectAddr *)((  v_1  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_7 = p_3;
                                                        {
                                                                 
                                                                 
                                                                if (o_ddfi_isbottom(v_7)) {
                                                                        if (1) {         
                                                                                v_0 = o_ddfi_bottom;
                                                                        } else {;        
                                                                                goto l_3;
                                                                        }        
                                                                } else
                                                                         
 if (o_ddfi_istop(v_7)) {
                                                                        if (1) {         
                                                                                v_0 = o_ddfi_top;
                                                                        } else {;        
                                                                                goto l_3;
                                                                        }        
                                                                } else   
                                                        l_3:
                                                                {        
                                                                        ;
                                                                        if (1) {         
                                                                                {
                                                                                        v_8 = o_ddfi_drop(v_7);
                                                                                        {{
                                                                                                        v_10 = o_alias_create(o_t1_top);
                                                                                                        {
                                                                                                                 
                                                                                                                 
                                                                                                                if (((((DMCP_xnode)(   v_5   ) )->op) )  == xop_Sort10_mirDataGlobal ) {
                                                                                                                        v_12 = (        0 , ((struct x_Sort10_mirDataGlobal *)((  v_5  ) ))->f_IsWriteOnce) ;
                                                                                                                        v_13 = (        0 , ((struct x_Sort10_mirDataGlobal *)((  v_5  ) ))->f_InitialValue) ;
                                                                                                                        v_14 = (        0 , ((struct x_Sort10_mirDataGlobal *)((  v_5  ) ))->f_Section) ;
                                                                                                                        v_15 = (        0 , ((struct x_Sort10_mirDataGlobal *)((  v_5  ) ))->f_Name) ;
                                                                                                                        v_16 = (        0 , ((struct x_Sort10_mirDataGlobal *)((  v_5  ) ))->f_IsVolatile) ;
                                                                                                                        v_17 = (        0 , ((struct x_Sort10_mirDataGlobal *)((  v_5  ) ))->f_Type) ;
                                                                                                                        v_18 = (        0 , ((struct x_Sort10_mirDataGlobal *)((  v_5  ) ))->f_cpid) ;
                                                                                                                        if (1) {         
                                                                                                                                v_11 = o_alias_update(o_dfi_select_4(v_8), o_id(v_3), o_t1_lift(o_id(v_5)));
                                                                                                                        } else {;        
                                                                                                                                goto l_4;
                                                                                                                        }        
                                                                                                                } else   
                                                                                                        l_4:
                                                                                                                {        
                                                                                                                        ;
                                                                                                                        if (1) {         
                                                                                                                                v_11 = o_dfi_select_4(v_8);
                                                                                                                        } else {;        
                                                                                                                                v_19 = 1;        
                                                                                                                        }
                                                                                                                }
                                                                                                                v_19 = 0;        
                                                                                                                if (v_19) {;     
                                                                                                                        ein_neur_name= fatal_error__("1.c",1343)+fatal_error_ ("No pattern matched in case Expression (12,23-15,33)");
                                                                                                                }
                                                                                                        }
                                                                                                        v_20 = o_const_fkt_create(o_va_top);
                                                                                                        v_21 = o_va_top;
                                                                                                        v_22 = o_dfi_select_1(v_8);
                                                                                                        v_9 = o_dfi_create(v_22, v_21, v_20, v_11, v_10);
                                                                                        }
                                                                                        v_0 = o_ddfi_lift(v_9);
                                                                                        }
                                                                                }
                                                                        } else {;        
                                                                                v_23 = 1;        
                                                                        }
                                                                }
                                                                v_23 = 0;        
                                                                if (v_23) {;     
                                                                        ein_neur_name= fatal_error__("1.c",1363)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_2;
                                        }        
                                } else   
                        l_2:
                                {        
                                        ;
                                        if (1) {         
                                                {
                                                        v_24 = p_3;
                                                        {
                                                                 
                                                                 
                                                                if (o_ddfi_isbottom(v_24)) {
                                                                        if (1) {         
                                                                                v_0 = o_ddfi_bottom;
                                                                        } else {;        
                                                                                goto l_5;
                                                                        }        
                                                                } else
                                                                         
 if (o_ddfi_istop(v_24)) {
                                                                        if (1) {         
                                                                                v_0 = o_ddfi_top;
                                                                        } else {;        
                                                                                goto l_5;
                                                                        }        
                                                                } else   
                                                        l_5:
                                                                {        
                                                                        ;
                                                                        if (1) {         
                                                                                {
                                                                                        v_25 = o_ddfi_drop(v_24);
                                                                                        v_0 = o_ddfi_lift(o_dfi_create((bool_to_c(o_is_volatile(v_3))) ? (o_dfi_select_1(v_25)) : (o_const_fkt_update(o_dfi_select_1(v_25), o_id(v_3), o_value(v_1, o_dfi_select_1(v_25), o_dfi_select_4(v_25))))
                                                                                                                       ,o_va_top, o_const_fkt_create(o_va_top), o_dfi_select_4(v_25), o_alias_create(o_t1_top)));
                                                                                }
                                                                        } else {;        
                                                                                v_26 = 1;        
                                                                        }
                                                                }
                                                                v_26 = 0;        
                                                                if (v_26) {;     
                                                                        ein_neur_name= fatal_error__("1.c",1433)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                v_27 = 1;        
                                        }
                                }
                                v_27 = 0;        
                                if (v_27) {;     
                                        goto l_1;
                                }        
                        } else
                           if (((((DMCP_xnode)(   v_2   ) )->op) )  == xop_Sort10_mirContent ) {
                                v_28 = (        0 , ((struct x_Sort10_mirContent *)((  v_2  ) ))->f_Addr) ;
                                v_29 = (        0 , ((struct x_Sort10_mirContent *)((  v_2  ) ))->f_Type) ;
                                if (1) {         
                                        {
                                                v_30 = p_3;
                                                {
                                                         
                                                         
                                                        if (o_ddfi_isbottom(v_30)) {
                                                                if (1) {         
                                                                        v_0 = o_ddfi_bottom;
                                                                } else {;        
                                                                        goto l_6;
                                                                }        
                                                        } else
                                                                 
 if (o_ddfi_istop(v_30)) {
                                                                if (1) {         
                                                                        v_0 = o_ddfi_top;
                                                                } else {;        
                                                                        goto l_6;
                                                                }        
                                                        } else   
                                                l_6:
                                                        {        
                                                                ;
                                                                if (1) {         
                                                                        {
                                                                                v_31 = o_ddfi_drop(v_30);
                                                                                v_0 = o_ddfi_lift(o_dfi_create(o_const_fkt_create(o_va_top), o_va_top, o_const_fkt_create(o_va_top), o_dfi_select_4(v_31), o_alias_create(o_t1_top)));
                                                                        }
                                                                } else {;        
                                                                        v_32 = 1;        
                                                                }
                                                        }
                                                        v_32 = 0;        
                                                        if (v_32) {;     
                                                                ein_neur_name= fatal_error__("1.c",1504)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                        }
                                                }
                                        }
                                } else {;        
                                        goto l_1;
                                }        
                        } else   
                l_1:
                        {        
                                ;
                                goto l_0;
                        }        
                } else
                   if (((((DMCP_xnode)(   p_4   ) )->op) )  == xop_Sort10_mirIf ) {
                        v_33 = (        0 , ((struct x_Sort10_mirIf *)((  p_4  ) ))->f_Else) ;
                        v_34 = (        0 , ((struct x_Sort10_mirIf *)((  p_4  ) ))->f_Then) ;
                        v_35 = (        0 , ((struct x_Sort10_mirIf *)((  p_4  ) ))->f_Cond) ;
                         
                        if (o_edges_eq(o_tt, p_5)) {
                                if (1) {         
                                        {
                                                v_36 = p_3;
                                                {
                                                         
                                                         
                                                        if (o_ddfi_isbottom(v_36)) {
                                                                if (1) {         
                                                                        v_0 = o_ddfi_bottom;
                                                                } else {;        
                                                                        goto l_8;
                                                                }        
                                                        } else
                                                                 
 if (o_ddfi_istop(v_36)) {
                                                                if (1) {         
                                                                        v_0 = o_ddfi_top;
                                                                } else {;        
                                                                        goto l_8;
                                                                }        
                                                        } else   
                                                l_8:
                                                        {        
                                                                ;
                                                                if (1) {         
                                                                        {
                                                                                v_37 = o_ddfi_drop(v_36);
                                                                                {
                                                                                        v_38 = o_value(v_35, o_dfi_select_1(v_37), o_dfi_select_4(v_37));
                                                                                        v_0 = (bool_to_c(bool_and(o_va_neq(v_38, o_va_top), o_va_neq(v_38, o_va_bottom)))) ? ((bool_to_c(bool_not(o_const_val_is_4(o_va_drop(v_38))))) ? ((bool_to_c(o_const_val_down_to_1(o_va_drop(v_38)))) ? (p_3) : (o_ddfi_bottom)
                                                                                                                                                                                                                                          ) : (p_3)
                                                                                                ) : (p_3);
                                                                                }
                                                                        }
                                                                } else {;        
                                                                        v_39 = 1;        
                                                                }
                                                        }
                                                        v_39 = 0;        
                                                        if (v_39) {;     
                                                                ein_neur_name= fatal_error__("1.c",1583)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                        }
                                                }
                                        }
                                } else {;        
                                        goto l_7;
                                }        
                        } else
                           if (o_edges_eq(o_ff, p_5)) {
                                if (1) {         
                                        {
                                                v_40 = p_3;
                                                {
                                                         
                                                         
                                                        if (o_ddfi_isbottom(v_40)) {
                                                                if (1) {         
                                                                        v_0 = o_ddfi_bottom;
                                                                } else {;        
                                                                        goto l_9;
                                                                }        
                                                        } else
                                                                 
 if (o_ddfi_istop(v_40)) {
                                                                if (1) {         
                                                                        v_0 = o_ddfi_top;
                                                                } else {;        
                                                                        goto l_9;
                                                                }        
                                                        } else   
                                                l_9:
                                                        {        
                                                                ;
                                                                if (1) {         
                                                                        {
                                                                                v_41 = o_ddfi_drop(v_40);
                                                                                {
                                                                                        v_42 = o_value(v_35, o_dfi_select_1(v_41), o_dfi_select_4(v_41));
                                                                                        v_0 = (bool_to_c(bool_and(o_va_neq(v_42, o_va_top), o_va_neq(v_42, o_va_bottom)))) ? ((bool_to_c(bool_not(o_const_val_is_4(o_va_drop(v_42))))) ? ((bool_to_c(o_const_val_down_to_1(o_va_drop(v_42)))) ? (o_ddfi_bottom) : (p_3)
                                                                                                                                                                                                                                          ) : (p_3)
                                                                                                ) : (p_3);
                                                                                }
                                                                        }
                                                                } else {;        
                                                                        v_43 = 1;        
                                                                }
                                                        }
                                                        v_43 = 0;        
                                                        if (v_43) {;     
                                                                ein_neur_name= fatal_error__("1.c",1650)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                        }
                                                }
                                        }
                                } else {;        
                                        goto l_7;
                                }        
                        } else   
                l_7:
                        {        
                                ;
                                goto l_0;
                        }        
                } else
                   if (((((DMCP_xnode)(   p_4   ) )->op) )  == xop_Sort10_mirFuncCall ) {
                        v_44 = (        0 , ((struct x_Sort10_mirFuncCall *)((  p_4  ) ))->f_Res) ;
                        v_45 = (        0 , ((struct x_Sort10_mirFuncCall *)((  p_4  ) ))->f_Params) ;
                        v_46 = (        0 , ((struct x_Sort10_mirFuncCall *)((  p_4  ) ))->f_Proc) ;
                        if (1) { 
                                {
                                        v_47 = p_3;
                                        {
                                                 
                                                 
                                                if (o_ddfi_isbottom(v_47)) {
                                                        if (1) {         
                                                                v_0 = o_ddfi_bottom;
                                                        } else {;        
                                                                goto l_10;
                                                        }        
                                                } else
                                                   if (o_ddfi_istop(v_47)) {
                                                        if (1) {         
                                                                v_0 = o_ddfi_top;
                                                        } else {;        
                                                                goto l_10;
                                                        }        
                                                } else   
                                        l_10:
                                                {        
                                                        ;
                                                        if (1) {         
                                                                {
                                                                        v_48 = o_ddfi_drop(v_47);
                                                                        v_0 = o_ddfi_lift(o_dfi_create(o_enrich(o_dfi_select_1(v_48), v_45), o_va_top, o_eval_params(v_45, o_dfi_select_1(v_48), unum_create("1")), o_dfi_select_4(v_48), o_eval_objs(v_45, o_dfi_select_4(v_48), unum_create("1"))));
                                                                }
                                                        } else {;        
                                                                v_49 = 1;        
                                                        }
                                                }
                                                v_49 = 0;        
                                                if (v_49) {;     
                                                        ein_neur_name= fatal_error__("1.c",1711)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                }
                                        }
                                }
                        } else {;        
                                goto l_0;
                        }        
                } else
                   if (((((DMCP_xnode)(   p_4   ) )->op) )  == xop_Sort10_mirCall ) {
                        v_50 = (        0 , ((struct x_Sort10_mirCall *)((  p_4  ) ))->f_Params) ;
                        v_51 = (        0 , ((struct x_Sort10_mirCall *)((  p_4  ) ))->f_Proc) ;
                        if (1) { 
                                {
                                        v_52 = p_3;
                                        {
                                                 
                                                 
                                                if (o_ddfi_isbottom(v_52)) {
                                                        if (1) {         
                                                                v_0 = o_ddfi_bottom;
                                                        } else {;        
                                                                goto l_11;
                                                        }        
                                                } else
                                                   if (o_ddfi_istop(v_52)) {
                                                        if (1) {         
                                                                v_0 = o_ddfi_top;
                                                        } else {;        
                                                                goto l_11;
                                                        }        
                                                } else   
                                        l_11:
                                                {        
                                                        ;
                                                        if (1) {         
                                                                {
                                                                        v_53 = o_ddfi_drop(v_52);
                                                                        v_0 = o_ddfi_lift(o_dfi_create(o_enrich(o_dfi_select_1(v_53), v_50), o_va_top, o_eval_params(v_50, o_dfi_select_1(v_53), unum_create("1")), o_dfi_select_4(v_53), o_eval_objs(v_50, o_dfi_select_4(v_53), unum_create("1"))));
                                                                }
                                                        } else {;        
                                                                v_54 = 1;        
                                                        }
                                                }
                                                v_54 = 0;        
                                                if (v_54) {;     
                                                        ein_neur_name= fatal_error__("1.c",1765)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                }
                                        }
                                }
                        } else {;        
                                goto l_0;
                        }        
                } else
                   if (((((DMCP_xnode)(   p_4   ) )->op) )  == xop_Sort10_mirTailCall ) {
                        v_55 = (        0 , ((struct x_Sort10_mirTailCall *)((  p_4  ) ))->f_Params) ;
                        v_56 = (        0 , ((struct x_Sort10_mirTailCall *)((  p_4  ) ))->f_Proc) ;
                        if (1) { 
                                {
                                        v_57 = p_3;
                                        {
                                                 
                                                 
                                                if (o_ddfi_isbottom(v_57)) {
                                                        if (1) {         
                                                                v_0 = o_ddfi_bottom;
                                                        } else {;        
                                                                goto l_12;
                                                        }        
                                                } else
                                                   if (o_ddfi_istop(v_57)) {
                                                        if (1) {         
                                                                v_0 = o_ddfi_top;
                                                        } else {;        
                                                                goto l_12;
                                                        }        
                                                } else   
                                        l_12:
                                                {        
                                                        ;
                                                        if (1) {         
                                                                {
                                                                        v_58 = o_ddfi_drop(v_57);
                                                                        v_0 = o_ddfi_lift(o_dfi_create(o_const_fkt_create(o_va_top), o_va_top, o_const_fkt_create(o_va_top), o_dfi_select_4(v_58), o_alias_create(o_t1_top)));
                                                                }
                                                        } else {;        
                                                                v_59 = 1;        
                                                        }
                                                }
                                                v_59 = 0;        
                                                if (v_59) {;     
                                                        ein_neur_name= fatal_error__("1.c",1819)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                }
                                        }
                                }
                        } else {;        
                                goto l_0;
                        }        
                } else
                   if (((((DMCP_xnode)(   p_4   ) )->op) )  == xop_Sort10_mirBeginProcedure ) {
                        v_60 = (        0 , ((struct x_Sort10_mirBeginProcedure *)((  p_4  ) ))->f_Locals) ;
                        v_61 = (        0 , ((struct x_Sort10_mirBeginProcedure *)((  p_4  ) ))->f_Params) ;
                        if (1) { 
                                {
                                        v_62 = p_3;
                                        {
                                                 
                                                 
                                                if (o_ddfi_isbottom(v_62)) {
                                                        if (1) {         
                                                                v_0 = o_ddfi_bottom;
                                                        } else {;        
                                                                goto l_13;
                                                        }        
                                                } else
                                                   if (o_ddfi_istop(v_62)) {
                                                        if (1) {         
                                                                v_0 = o_ddfi_top;
                                                        } else {;        
                                                                goto l_13;
                                                        }        
                                                } else   
                                        l_13:
                                                {        
                                                        ;
                                                        if (1) {         
                                                                {
                                                                        v_63 = o_ddfi_drop(v_62);
                                                                        v_0 = o_ddfi_lift(o_dfi_create(o_init_locals(o_fill_pars(o_dfi_select_1(v_63), v_61, o_dfi_select_3(v_63), unum_create("1")), v_60), o_va_top, o_const_fkt_create(o_va_top), o_putin(v_61, o_dfi_select_4(v_63), o_dfi_select_5(v_63), unum_create("1")), o_alias_top));
                                                                }
                                                        } else {;        
                                                                v_64 = 1;        
                                                        }
                                                }
                                                v_64 = 0;        
                                                if (v_64) {;     
                                                        ein_neur_name= fatal_error__("1.c",1873)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                }
                                        }
                                }
                        } else {;        
                                goto l_0;
                        }        
                } else
                   if (((((DMCP_xnode)(   p_4   ) )->op) )  == xop_Sort10_mirReturn ) {
                        v_65 = (        0 , ((struct x_Sort10_mirReturn *)((  p_4  ) ))->f_Next) ;
                        v_66 = (        0 , ((struct x_Sort10_mirReturn *)((  p_4  ) ))->f_Value) ;
                        if (1) { 
                                {
                                        v_67 = p_3;
                                        {
                                                 
                                                 
                                                if (o_ddfi_isbottom(v_67)) {
                                                        if (1) {         
                                                                v_0 = o_ddfi_bottom;
                                                        } else {;        
                                                                goto l_14;
                                                        }        
                                                } else
                                                   if (o_ddfi_istop(v_67)) {
                                                        if (1) {         
                                                                v_0 = o_ddfi_top;
                                                        } else {;        
                                                                goto l_14;
                                                        }        
                                                } else   
                                        l_14:
                                                {        
                                                        ;
                                                        if (1) {         
                                                                {
                                                                        v_68 = o_ddfi_drop(v_67);
                                                                        v_0 = o_ddfi_lift(o_dfi_create(o_dfi_select_1(v_68), o_value(v_66, o_dfi_select_1(v_68), o_dfi_select_4(v_68)), o_const_fkt_create(o_va_top), o_dfi_select_4(v_68), o_alias_create(o_t1_top)));
                                                                }
                                                        } else {;        
                                                                v_69 = 1;        
                                                        }
                                                }
                                                v_69 = 0;        
                                                if (v_69) {;     
                                                        ein_neur_name= fatal_error__("1.c",1927)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                }
                                        }
                                }
                        } else {;        
                                goto l_0;
                        }        
                } else
                   if (((((DMCP_xnode)(   p_4   ) )->op) )  == xop_Sort10_mirEndFuncCall ) {
                        v_70 = (        0 , ((struct x_Sort10_mirEndFuncCall *)((  p_4  ) ))->f_Res) ;
                         
                        if (((((DMCP_xnode)(   v_70   ) )->op) )  == xop_Sort10_mirObjectAddr ) {
                                v_71 = (        0 , ((struct x_Sort10_mirObjectAddr *)((  v_70  ) ))->f_Obj) ;
                                v_72 = (        0 , ((struct x_Sort10_mirObjectAddr *)((  v_70  ) ))->f_Type) ;
                                if (1) {         
                                        {
                                                v_73 = p_3;
                                                {
                                                         
                                                         
                                                        if (o_ddfi_isbottom(v_73)) {
                                                                if (1) {         
                                                                        v_0 = o_ddfi_bottom;
                                                                } else {;        
                                                                        goto l_16;
                                                                }        
                                                        } else
                                                                 
 if (o_ddfi_istop(v_73)) {
                                                                if (1) {         
                                                                        v_0 = o_ddfi_top;
                                                                } else {;        
                                                                        goto l_16;
                                                                }        
                                                        } else   
                                                l_16:
                                                        {        
                                                                ;
                                                                if (1) {         
                                                                        {
                                                                                v_74 = o_ddfi_drop(v_73);
                                                                                v_0 = o_ddfi_lift(o_dfi_create((bool_to_c(o_is_volatile(v_71))) ? (o_dfi_select_1(v_74)) : (o_const_fkt_update(o_dfi_select_1(v_74), o_id(v_71), o_dfi_select_2(v_74)))
                                                                                                               ,o_dfi_select_2(v_74), o_const_fkt_create(o_va_top), o_dfi_select_4(v_74), o_alias_create(o_t1_top)));
                                                                        }
                                                                } else {;        
                                                                        v_75 = 1;        
                                                                }
                                                        }
                                                        v_75 = 0;        
                                                        if (v_75) {;     
                                                                ein_neur_name= fatal_error__("1.c",1995)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                        }
                                                }
                                        }
                                } else {;        
                                        goto l_15;
                                }        
                        } else   
                l_15:
                        {        
                                ;
                                goto l_0;
                        }        
                } else
                   if (((((DMCP_xnode)(   p_4   ) )->op) )  == xop_Sort10_mirCallRet ) {
                        v_76 = (        0 , ((struct x_Sort10_mirCallRet *)((  p_4  ) ))->f_Locals) ;
                        v_77 = (        0 , ((struct x_Sort10_mirCallRet *)((  p_4  ) ))->f_Params) ;
                        if (1) { 
                                {
                                        v_78 = p_3;
                                        {
                                                 
                                                 
                                                if (o_ddfi_isbottom(v_78)) {
                                                        if (1) {         
                                                                v_0 = o_ddfi_bottom;
                                                        } else {;        
                                                                goto l_17;
                                                        }        
                                                } else
                                                   if (o_ddfi_istop(v_78)) {
                                                        if (1) {         
                                                                v_0 = o_ddfi_top;
                                                        } else {;        
                                                                goto l_17;
                                                        }        
                                                } else   
                                        l_17:
                                                {        
                                                        ;
                                                        if (1) {         
                                                                {
                                                                        v_79 = o_ddfi_drop(v_78);
                                                                        v_0 = o_ddfi_lift(o_dfi_create(o_built(o_const_fkt_create(o_va_bottom), o_dfi_select_1(v_79), v_77, v_76), o_dfi_select_2(v_79), o_const_fkt_create(o_va_top), o_dfi_select_4(v_79), o_alias_create(o_t1_top)));
                                                                }
                                                        } else {;        
                                                                v_80 = 1;        
                                                        }
                                                }
                                                v_80 = 0;        
                                                if (v_80) {;     
                                                        ein_neur_name= fatal_error__("1.c",2055)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                }
                                        }
                                }
                        } else {;        
                                goto l_0;
                        }        
                } else
                   if (((((DMCP_xnode)(   p_4   ) )->op) )  == xop_Sort10_mirEndProcedure ) {
                        v_81 = (        0 , ((struct x_Sort10_mirEndProcedure *)((  p_4  ) ))->f_Locals) ;
                        v_82 = (        0 , ((struct x_Sort10_mirEndProcedure *)((  p_4  ) ))->f_Params) ;
                        if (1) { 
                                {
                                        v_83 = p_3;
                                        {
                                                 
                                                 
                                                if (o_ddfi_isbottom(v_83)) {
                                                        if (1) {         
                                                                v_0 = o_ddfi_bottom;
                                                        } else {;        
                                                                goto l_18;
                                                        }        
                                                } else
                                                   if (o_ddfi_istop(v_83)) {
                                                        if (1) {         
                                                                v_0 = o_ddfi_top;
                                                        } else {;        
                                                                goto l_18;
                                                        }        
                                                } else   
                                        l_18:
                                                {        
                                                        ;
                                                        if (1) {         
                                                                {
                                                                        v_84 = o_ddfi_drop(v_83);
                                                                        v_0 = o_ddfi_lift(o_dfi_create(o_erase_var(o_erase_var(o_dfi_select_1(v_84), v_82), v_81), o_dfi_select_2(v_84), o_const_fkt_create(o_va_top), o_dfi_select_4(v_84), o_alias_create(o_t1_top)));
                                                                }
                                                        } else {;        
                                                                v_85 = 1;        
                                                        }
                                                }
                                                v_85 = 0;        
                                                if (v_85) {;     
                                                        ein_neur_name= fatal_error__("1.c",2109)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                }
                                        }
                                }
                        } else {;        
                                goto l_0;
                        }        
                } else           
        l_0:
                {                
                        ; {
                                v_86 = p_3;
                                {
                                         
                                         
                                        if (o_ddfi_isbottom(v_86)) {
                                                if (1) {         
                                                        v_0 = o_ddfi_bottom;
                                                } else {;        
                                                        goto l_19;
                                                }        
                                        } else
                                           if (o_ddfi_istop(v_86)) {
                                                if (1) {         
                                                        v_0 = o_ddfi_top;
                                                } else {;        
                                                        goto l_19;
                                                }        
                                        } else   
                                l_19:
                                        {        
                                                ;
                                                if (1) {         
                                                        {
                                                                v_87 = o_ddfi_drop(v_86);
                                                                v_0 = o_ddfi_lift(o_dfi_create(o_dfi_select_1(v_87), o_va_top, o_const_fkt_create(o_va_top), o_dfi_select_4(v_87), o_alias_create(o_t1_top)));
                                                        }
                                                } else {;        
                                                        v_88 = 1;        
                                                }
                                        }
                                        v_88 = 0;        
                                        if (v_88) {;     
                                                ein_neur_name= fatal_error__("1.c",2160)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                        }
                                }
                        }
                }
                return (v_0);
        }
}
void 
transfer__stat(void)
{
}
o_va 
o_mycast(o_va p_6, snum p_7)
{
        {
                o_va            v_89;
                o_va            v_90;
                o_const_val     v_91;
                snum            v_92;
                o_const_val     v_93;
                int             v_94;
                int             v_95;
                if (1) {         
                        {
                                v_90 = p_6;
                                {
                                         
                                         
                                        if (o_va_isbottom(v_90)) {
                                                if (1) {         
                                                        v_89 = o_va_bottom;
                                                } else {;        
                                                        goto l_20;
                                                }        
                                        } else
                                           if (o_va_istop(v_90)) {
                                                if (1) {         
                                                        v_89 = o_va_top;
                                                } else {;        
                                                        goto l_20;
                                                }        
                                        } else   
                                l_20:
                                        {        
                                                ;
                                                if (1) {         
                                                        {
                                                                v_91 = o_va_drop(v_90);
                                                                {
                                                                        v_92 = o_const_val_type(v_91);
                                                                        {{
                                                                                         
                                                                                         
                                                                                        if (snum_eq(v_92, snum_create("4"))) {
                                                                                                if (1) {         
                                                                                                        v_93 = v_91;
                                                                                                } else {;        
                                                                                                        goto l_21;
                                                                                                }        
                                                                                        } else   
                                                                                l_21:
                                                                                        {        
                                                                                                ;
                                                                                                 
                                                                                                if (snum_eq(p_7, snum_create("0"))) {
                                                                                                        if (1) {         
                                                                                                                v_93 = (o_const_val) fatal_error_ (str_create("Oops (1)"));
                                                                                                        } else {;        
                                                                                                                goto l_22;
                                                                                                        }        
                                                                                                } else   
                                                                                        l_22:
                                                                                                {        
                                                                                                        ;
                                                                                                         
                                                                                                        if (snum_eq(v_92, snum_create("1"))) {
                                                                                                                 
                                                                                                                if (snum_eq(p_7, snum_create("1"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = v_91;
                                                                                                                        } else {;        
                                                                                                                                goto l_24;
                                                                                                                        }        
                                                                                                                } else
                                                                                                                         
 if (snum_eq(p_7, snum_create("2"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = (bool_to_c(o_const_val_down_to_1(v_91))) ? (o_const_val_create_from_2(snum_create("1"))) : (o_const_val_create_from_2(snum_create("0")));
                                                                                                                        } else {;        
                                                                                                                                goto l_24;
                                                                                                                        }        
                                                                                                                } else
                                                                                                                         
 if (snum_eq(p_7, snum_create("3"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = (bool_to_c(o_const_val_down_to_1(v_91))) ? (o_const_val_create_from_3(real_create("1.0"))) : (o_const_val_create_from_3(real_create("0.0")));
                                                                                                                        } else {;        
                                                                                                                                goto l_24;
                                                                                                                        }        
                                                                                                                } else
                                                                                                                         
 if (snum_eq(p_7, snum_create("5"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = (bool_to_c(o_const_val_down_to_1(v_91))) ? (o_const_val_create_from_5(unum_create("1"))) : (o_const_val_create_from_5(unum_create("0")));
                                                                                                                        } else {;        
                                                                                                                                goto l_24;
                                                                                                                        }        
                                                                                                                } else   
                                                                                                        l_24:
                                                                                                                {        
                                                                                                                        ;
                                                                                                                        goto l_23;
                                                                                                                }        
                                                                                                        } else
                                                                                                                 
 if (snum_eq(v_92, snum_create("2"))) {
                                                                                                                 
                                                                                                                if (snum_eq(p_7, snum_create("1"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = o_const_val_create_from_1(snum_eq(o_const_val_down_to_2(v_91), snum_create("0")));
                                                                                                                        } else {;        
                                                                                                                                goto l_25;
                                                                                                                        }        
                                                                                                                } else
                                                                                                                         
 if (snum_eq(p_7, snum_create("2"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = v_91;
                                                                                                                        } else {;        
                                                                                                                                goto l_25;
                                                                                                                        }        
                                                                                                                } else
                                                                                                                         
 if (snum_eq(p_7, snum_create("3"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = o_const_val_create_from_3(snum_to_real(o_const_val_down_to_2(v_91)));
                                                                                                                        } else {;        
                                                                                                                                goto l_25;
                                                                                                                        }        
                                                                                                                } else
                                                                                                                         
 if (snum_eq(p_7, snum_create("5"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = o_const_val_create_from_5(snum_to_unum(o_const_val_down_to_2(v_91)));
                                                                                                                        } else {;        
                                                                                                                                goto l_25;
                                                                                                                        }        
                                                                                                                } else   
                                                                                                        l_25:
                                                                                                                {        
                                                                                                                        ;
                                                                                                                        goto l_23;
                                                                                                                }        
                                                                                                        } else
                                                                                                                 
 if (snum_eq(v_92, snum_create("3"))) {
                                                                                                                 
                                                                                                                if (snum_eq(p_7, snum_create("1"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = o_const_val_create_from_1(real_eq(o_const_val_down_to_3(v_91), real_create("0.0")));
                                                                                                                        } else {;        
                                                                                                                                goto l_26;
                                                                                                                        }        
                                                                                                                } else
                                                                                                                         
 if (snum_eq(p_7, snum_create("2"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = o_const_val_create_from_2(real_to_snum(o_const_val_down_to_3(v_91)));
                                                                                                                        } else {;        
                                                                                                                                goto l_26;
                                                                                                                        }        
                                                                                                                } else
                                                                                                                         
 if (snum_eq(p_7, snum_create("3"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = v_91;
                                                                                                                        } else {;        
                                                                                                                                goto l_26;
                                                                                                                        }        
                                                                                                                } else
                                                                                                                         
 if (snum_eq(p_7, snum_create("5"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = o_const_val_create_from_5(real_to_unum(o_const_val_down_to_3(v_91)));
                                                                                                                        } else {;        
                                                                                                                                goto l_26;
                                                                                                                        }        
                                                                                                                } else   
                                                                                                        l_26:
                                                                                                                {        
                                                                                                                        ;
                                                                                                                        goto l_23;
                                                                                                                }        
                                                                                                        } else
                                                                                                                 
 if (snum_eq(v_92, snum_create("5"))) {
                                                                                                                 
                                                                                                                if (snum_eq(p_7, snum_create("1"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = o_const_val_create_from_1(unum_eq(o_const_val_down_to_5(v_91), unum_create("0")));
                                                                                                                        } else {;        
                                                                                                                                goto l_27;
                                                                                                                        }        
                                                                                                                } else
                                                                                                                         
 if (snum_eq(p_7, snum_create("2"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = o_const_val_create_from_2(unum_to_snum(o_const_val_down_to_5(v_91)));
                                                                                                                        } else {;        
                                                                                                                                goto l_27;
                                                                                                                        }        
                                                                                                                } else
                                                                                                                         
 if (snum_eq(p_7, snum_create("3"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = o_const_val_create_from_3(unum_to_real(o_const_val_down_to_5(v_91)));
                                                                                                                        } else {;        
                                                                                                                                goto l_27;
                                                                                                                        }        
                                                                                                                } else
                                                                                                                         
 if (snum_eq(p_7, snum_create("5"))) {
                                                                                                                        if (1) {         
                                                                                                                                v_93 = v_91;
                                                                                                                        } else {;        
                                                                                                                                goto l_27;
                                                                                                                        }        
                                                                                                                } else   
                                                                                                        l_27:
                                                                                                                {        
                                                                                                                        ;
                                                                                                                        goto l_23;
                                                                                                                }        
                                                                                                        } else   
                                                                                                l_23:
                                                                                                        {        
                                                                                                                ;
                                                                                                                v_94 = 1;        
                                                                                                        }
                                                                                                }
                                                                                        }
                                                                                        v_94 = 0;        
                                                                                        if (v_94) {;     
                                                                                                ein_neur_name= fatal_error__("1.c",2907)+fatal_error_ ("No pattern matched in case Expression (759,2-778,9)");
                                                                                        }
                                                                        }
                                                                        v_89 = o_va_lift(v_93);
                                                                        }
                                                                }
                                                        }
                                                } else {;        
                                                        v_95 = 1;        
                                                }
                                        }
                                        v_95 = 0;        
                                        if (v_95) {;     
                                                ein_neur_name= fatal_error__("1.c",2925)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                        }
                                }
                        }
                } else {;        
                        ein_neur_name= fatal_error__("1.c",2931)+fatal_error_ ("No pattern matched in function o_mycast");
                }
                return (v_89);
        }
}
void 
o_mycast_stat(void)
{
}
str 
o_print_type(mirTYPE p_8)
{
        {
                str             v_96;
                BOOL            v_97;
                mirTYPE         v_98;
                mirEXPR         v_99;
                mirEXPR         v_100;
                mirTYPE         v_101;
                LIST_mirObject  v_102;
                LIST_mirObject  v_103;
                mirTYPE         v_104;
                int             v_105;
                if (1) {         
                        {
                                 
                                 
                                if (((((DMCP_xnode)(   p_8   ) )->op) )  == xop_Sort10_mirNoType ) {
                                        if (1) {         
                                                v_96 = str_create("no type");
                                        } else {;        
                                                goto l_28;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_8   ) )->op) )  == xop_Sort10_mirAnyType ) {
                                        if (1) {         
                                                v_96 = str_create("Any type");
                                        } else {;        
                                                goto l_28;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_8   ) )->op) )  == xop_Sort10_mirBoolean ) {
                                        if (1) {         
                                                v_96 = str_create("Bool type");
                                        } else {;        
                                                goto l_28;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_8   ) )->op) )  == xop_Sort10_mirBasicBlockType ) {
                                        if (1) {         
                                                v_96 = str_create("Basic Block type");
                                        } else {;        
                                                goto l_28;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_8   ) )->op) )  == xop_Sort10_mirInteger ) {
                                        v_97 = (        0 , ((struct x_Sort10_mirInteger *)((  p_8  ) ))->f_Signed) ;
                                        if (1) {         
                                                v_96 = str_conc((bool_to_c(bool_create(( v_97 ) )
                                                                           )) ? (str_create("signed")) : (str_create("unsigned"))
                                                                ,str_create(" integer type"));
                                        } else {;        
                                                goto l_28;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_8   ) )->op) )  == xop_Sort10_mirReal ) {
                                        if (1) {         
                                                v_96 = str_create("Real type");
                                        } else {;        
                                                goto l_28;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_8   ) )->op) )  == xop_Sort10_mirPointer ) {
                                        v_98 = (        0 , ((struct x_Sort10_mirPointer *)((  p_8  ) ))->f_RefType) ;
                                        if (1) {         
                                                v_96 = str_create("Pointer type");
                                        } else {;        
                                                goto l_28;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_8   ) )->op) )  == xop_Sort10_mirArray ) {
                                        v_99 = (        0 , ((struct x_Sort10_mirArray *)((  p_8  ) ))->f_Upb) ;
                                        v_100 = (       0 , ((struct x_Sort10_mirArray *)((  p_8  ) ))->f_Lwb) ;
                                        v_101 = (       0 , ((struct x_Sort10_mirArray *)((  p_8  ) ))->f_ElemType) ;
                                        if (1) {         
                                                v_96 = str_create("Array type");
                                        } else {;        
                                                goto l_28;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_8   ) )->op) )  == xop_Sort10_mirRecord ) {
                                        v_102 = (       0 , ((struct x_Sort10_mirRecord *)((  p_8  ) ))->f_Fields) ;
                                        if (1) {         
                                                v_96 = str_create("Record type");
                                        } else {;        
                                                goto l_28;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_8   ) )->op) )  == xop_Sort10_mirUnion ) {
                                        v_103 = (       0 , ((struct x_Sort10_mirUnion *)((  p_8  ) ))->f_Members) ;
                                        if (1) {         
                                                v_96 = str_create("Union type");
                                        } else {;        
                                                goto l_28;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_8   ) )->op) )  == xop_Sort10_mirProcType ) {
                                        v_104 = (       0 , ((struct x_Sort10_mirProcType *)((  p_8  ) ))->f_ResultType) ;
                                        if (1) {         
                                                v_96 = str_create("Proc type");
                                        } else {;        
                                                goto l_28;
                                        }        
                                } else   
                        l_28:
                                {        
                                        ;
                                        if (1) {         
                                                v_96 = str_create("Unkown type");
                                        } else {;        
                                                v_105 = 1;       
                                        }
                                }
                                v_105 = 0;       
                                if (v_105) {;    
                                        ein_neur_name= fatal_error__("1.c",3087)+fatal_error_ ("No pattern matched in case Expression (737,2-751,11)");
                                }
                        }
                } else {;        
                        ein_neur_name= fatal_error__("1.c",3091)+fatal_error_ ("No pattern matched in function o_print_type");
                }
                return (v_96);
        }
}
void 
o_print_type_stat(void)
{
}
snum 
o_which_type(mirTYPE p_9)
{
        {
                snum            v_106;
                BOOL            v_107;
                mirTYPE         v_108;
                mirEXPR         v_109;
                mirEXPR         v_110;
                mirTYPE         v_111;
                LIST_mirObject  v_112;
                LIST_mirObject  v_113;
                mirTYPE         v_114;
                int             v_115;
                if (1) {         
                        {
                                 
                                 
                                if (((((DMCP_xnode)(   p_9   ) )->op) )  == xop_Sort10_mirNoType ) {
                                        if (1) {         
                                                v_106 = snum_create("0");
                                        } else {;        
                                                goto l_29;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_9   ) )->op) )  == xop_Sort10_mirAnyType ) {
                                        if (1) {         
                                                v_106 = snum_create("0");
                                        } else {;        
                                                goto l_29;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_9   ) )->op) )  == xop_Sort10_mirBoolean ) {
                                        if (1) {         
                                                v_106 = snum_create("1");
                                        } else {;        
                                                goto l_29;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_9   ) )->op) )  == xop_Sort10_mirBasicBlockType ) {
                                        if (1) {         
                                                v_106 = snum_create("0");
                                        } else {;        
                                                goto l_29;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_9   ) )->op) )  == xop_Sort10_mirInteger ) {
                                        v_107 = (       0 , ((struct x_Sort10_mirInteger *)((  p_9  ) ))->f_Signed) ;
                                        if (1) {         
                                                v_106 = (bool_to_c(bool_create(( v_107 ) )
                                                                   )) ? (snum_create("2")) : (snum_create("5"));
                                        } else {;        
                                                goto l_29;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_9   ) )->op) )  == xop_Sort10_mirReal ) {
                                        if (1) {         
                                                v_106 = snum_create("3");
                                        } else {;        
                                                goto l_29;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_9   ) )->op) )  == xop_Sort10_mirPointer ) {
                                        v_108 = (       0 , ((struct x_Sort10_mirPointer *)((  p_9  ) ))->f_RefType) ;
                                        if (1) {         
                                                v_106 = snum_create("0");
                                        } else {;        
                                                goto l_29;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_9   ) )->op) )  == xop_Sort10_mirArray ) {
                                        v_109 = (       0 , ((struct x_Sort10_mirArray *)((  p_9  ) ))->f_Upb) ;
                                        v_110 = (       0 , ((struct x_Sort10_mirArray *)((  p_9  ) ))->f_Lwb) ;
                                        v_111 = (       0 , ((struct x_Sort10_mirArray *)((  p_9  ) ))->f_ElemType) ;
                                        if (1) {         
                                                v_106 = snum_create("0");
                                        } else {;        
                                                goto l_29;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_9   ) )->op) )  == xop_Sort10_mirRecord ) {
                                        v_112 = (       0 , ((struct x_Sort10_mirRecord *)((  p_9  ) ))->f_Fields) ;
                                        if (1) {         
                                                v_106 = snum_create("0");
                                        } else {;        
                                                goto l_29;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_9   ) )->op) )  == xop_Sort10_mirUnion ) {
                                        v_113 = (       0 , ((struct x_Sort10_mirUnion *)((  p_9  ) ))->f_Members) ;
                                        if (1) {         
                                                v_106 = snum_create("0");
                                        } else {;        
                                                goto l_29;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_9   ) )->op) )  == xop_Sort10_mirProcType ) {
                                        v_114 = (       0 , ((struct x_Sort10_mirProcType *)((  p_9  ) ))->f_ResultType) ;
                                        if (1) {         
                                                v_106 = snum_create("0");
                                        } else {;        
                                                goto l_29;
                                        }        
                                } else   
                        l_29:
                                {        
                                        ;
                                        if (1) {         
                                                v_106 = (snum) fatal_error_ (str_create("Unkown type"));
                                        } else {;        
                                                v_115 = 1;       
                                        }
                                }
                                v_115 = 0;       
                                if (v_115) {;    
                                        ein_neur_name= fatal_error__("1.c",3246)+fatal_error_ ("No pattern matched in case Expression (721,2-734,11)");
                                }
                        }
                } else {;        
                        ein_neur_name= fatal_error__("1.c",3250)+fatal_error_ ("No pattern matched in function o_which_type");
                }
                return (v_106);
        }
}
void 
o_which_type_stat(void)
{
}
str 
o_print_const_val_type(o_const_val p_10)
{
        {
                str             v_116;
                snum            v_117;
                int             v_118;
                if (1) {         
                        {
                                v_117 = o_const_val_type(p_10);
                                {
                                         
                                         
                                        if (snum_eq(v_117, snum_create("1"))) {
                                                if (1) {         
                                                        v_116 = str_create("Type bool");
                                                } else {;        
                                                        goto l_30;
                                                }        
                                        } else
                                           if (snum_eq(v_117, snum_create("2"))) {
                                                if (1) {         
                                                        v_116 = str_create("Type snum");
                                                } else {;        
                                                        goto l_30;
                                                }        
                                        } else
                                           if (snum_eq(v_117, snum_create("3"))) {
                                                if (1) {         
                                                        v_116 = str_create("Type real");
                                                } else {;        
                                                        goto l_30;
                                                }        
                                        } else
                                           if (snum_eq(v_117, snum_create("4"))) {
                                                if (1) {         
                                                        v_116 = str_create("Type nothing");
                                                } else {;        
                                                        goto l_30;
                                                }        
                                        } else
                                           if (snum_eq(v_117, snum_create("5"))) {
                                                if (1) {         
                                                        v_116 = str_create("Type unum");
                                                } else {;        
                                                        goto l_30;
                                                }        
                                        } else   
                                l_30:
                                        {        
                                                ;
                                                if (1) {         
                                                        v_116 = (str) fatal_error_ (str_create("Type unknown "));
                                                } else {;        
                                                        v_118 = 1;       
                                                }
                                        }
                                        v_118 = 0;       
                                        if (v_118) {;    
                                                ein_neur_name= fatal_error__("1.c",3343)+fatal_error_ ("No pattern matched in case Expression (710,2-717,9)");
                                        }
                                }
                        }
                } else {;        
                        ein_neur_name= fatal_error__("1.c",3349)+fatal_error_ ("No pattern matched in function o_print_const_val_type");
                }
                return (v_116);
        }
}
void 
o_print_const_val_type_stat(void)
{
}
snum 
o_const_val_type(o_const_val p_11)
{
        {
                snum            v_119;
                if (1) {         
                        v_119 = (bool_to_c(o_const_val_is_1(p_11))) ? (snum_create("1")) : ((bool_to_c(o_const_val_is_2(p_11))) ? (snum_create("2")) : ((bool_to_c(o_const_val_is_3(p_11))) ? (snum_create("3")) : ((bool_to_c(o_const_val_is_4(p_11))) ? (snum_create("4")) : ((bool_to_c(o_const_val_is_5(p_11))) ? (snum_create("5")) : ((snum) fatal_error_ (str_create("Type unknown (const_val_type)")))
                                                                           )
                                                                           )
                                                                           )
                                );
                } else {;        
                        ein_neur_name= fatal_error__("1.c",3386)+fatal_error_ ("No pattern matched in function o_const_val_type");
                }
                return (v_119);
        }
}
void 
o_const_val_type_stat(void)
{
}
static          o_myid_chit = 0;
static          o_myid_hit = 0;
static int      o_myid_initialized = 0, o_myid_active = 1, o_myid_size = 3989;
static o_myid_MEMOP *o_myid_memotable;
unum 
o_myid(INT p_12)
{
        unsigned int    h;
        o_myid_MEMOP    z;
        int             w;
        if (o_myid_active) {
                if (!o_myid_initialized) {
                        o_myid_memotable = (o_myid_MEMOP *) calloc(o_myid_size, sizeof(o_myid_MEMOP));
                        if (o_myid_memotable != ((void *)0) )
                                o_myid_initialized = 1;
                        else {
                                o_myid_active = 0;
                                goto o_myid_main;
                        }
                }
                h = 0;
                h += 2 * ((unsigned int)( p_12 )) ;
                h %= o_myid_size;
                for (w = 0, z = o_myid_memotable[h]; z != ((void *)0) ; w++, z = z->next) {
                        if (
                            p_12 == z->_1) {
                                if (w == 0)
                                        o_myid_chit += 1;
                                o_myid_hit += 1;
                                return (z->res);
                        }
                }
        } {
                unum            v_120;
                snum            v_121;
o_myid_main:
                if (1) {         
                        {
                                v_121 = snum_create(INT_get_value(p_12));
                                v_120 = (bool_to_c(snum_le(v_121, snum_create("0")))) ? ((unum) fatal_error_ (str_create("Negativ id found!"))) : (snum_to_unum(v_121));
                        }
                } else {;        
                        ein_neur_name= fatal_error__("1.c",3456)+fatal_error_ ("No pattern matched in function o_myid");
                }
                if (o_myid_active) {
                        z = (o_myid_MEMOP) malloc(sizeof(o_myid_MEMO));
                        if (z == ((void *)0) ) {
                                ein_neur_name= fatal_error__("1.c",3461)+fatal_error_ ("No mem");
                        }
                        z->_1 = p_12;
                        z->res = v_120;
                        z->next = o_myid_memotable[h];
                        o_myid_memotable[h] = z;
                }
                return (v_120);
        }
}
void 
o_myid_stat(void)
{
        int             o_myid_anz = 0, o_myid_max = 0;
        double          o_myid_durch = 0;
        int             s, leer = 0;
        int             j, i;
        o_myid_MEMOP    z;
        if (o_myid_active && o_myid_initialized) {
                for (i = 0; i < o_myid_size; i++) {
                        for (j = 0, z = o_myid_memotable[i]; z != ((void *)0) ; j++, z = z->next) {
                                o_myid_anz += 1;
                        }
                        leer += j == 0 ? 1 : 0;
                        o_myid_max = o_myid_max < j ? j : o_myid_max;
                }
                o_myid_durch = (double) o_myid_anz / (double) (o_myid_size - leer);
        }
        if (o_myid_active) {
                s = o_myid_size;
                fprintf((&_iob[2]) , "o_myid:\n");
                fprintf((&_iob[2]) , " size=%d\n", s);
                fprintf((&_iob[2]) , " anz=%d\n", o_myid_anz);
                fprintf((&_iob[2]) , " max=%d\n", o_myid_max);
                fprintf((&_iob[2]) , " durchschnitt=%f\n", o_myid_durch);
                fprintf((&_iob[2]) , " cache hit=%d\n", o_myid_chit);
                fprintf((&_iob[2]) , " leere Eintraege=%d\n", leer);
                fprintf((&_iob[2]) , " hit=%d\n", o_myid_hit);
                fprintf((&_iob[2]) , " Guete=%f\n", (double) o_myid_hit / (double) o_myid_anz);
                fprintf((&_iob[2]) , " cache Guete=%f\n", (double) o_myid_chit / (double) o_myid_anz);
                fprintf((&_iob[2]) , "\n");
        }
}
o_va 
o_value(mirEXPR p_13, o_const_fkt p_14, o_alias p_15)
{
        {
                o_va            v_122;
                mirTYPE         v_123;
                UNIV_INT        v_124;
                mirTYPE         v_125;
                UNIV_REAL       v_126;
                mirTYPE         v_127;
                BOOL            v_128;
                mirTYPE         v_129;
                UNIV_ADDRESS    v_130;
                mirTYPE         v_131;
                LIST_mirEXPR    v_132;
                mirTYPE         v_133;
                LIST_mirObject  v_134;
                mirEXPR         v_135;
                mirTYPE         v_136;
                mirTYPE         v_137;
                mirTYPE         v_138;
                mirObject       v_139;
                mirTYPE         v_140;
                mirEXPR         v_141;
                mirTYPE         v_142;
                mirEXPR         v_143;
                mirTYPE         v_144;
                mirObject       v_145;
                mirTYPE         v_146;
                o_t1            v_147;
                unum            v_148;
                mirObject       v_149;
                mirTYPE         v_150;
                BOOL            v_151;
                mirEXPR         v_152;
                mirSection      v_153;
                NAME            v_154;
                BOOL            v_155;
                mirTYPE         v_156;
                INT             v_157;
                int             v_158;
                int             v_159;
                mirEXPR         v_160;
                mirEXPR         v_161;
                mirTYPE         v_162;
                LIST_mirObject  v_163;
                mirEXPR         v_164;
                mirTYPE         v_165;
                mirEXPR         v_166;
                mirEXPR         v_167;
                mirEXPR         v_168;
                mirTYPE         v_169;
                mirEXPR         v_170;
                mirEXPR         v_171;
                mirEXPR         v_172;
                mirEXPR         v_173;
                mirTYPE         v_174;
                UNIV_INT        v_175;
                UNIV_INT        v_176;
                mirEXPR         v_177;
                mirTYPE         v_178;
                o_va            v_179;
                o_const_val     v_180;
                int             v_181;
                mirROUNDING     v_182;
                mirEXPR         v_183;
                mirTYPE         v_184;
                o_va            v_185;
                o_const_val     v_186;
                BOOL            v_187;
                int             v_188;
                BOOL            v_189;
                int             v_190;
                real            v_191;
                snum            v_192;
                bool            v_193;
                real            v_194;
                unum            v_195;
                unum            v_196;
                unum            v_197;
                unum            v_198;
                BOOL            v_199;
                int             v_200;
                mirEXPR         v_201;
                mirTYPE         v_202;
                mirEXPR         v_203;
                mirTYPE         v_204;
                o_va            v_205;
                o_const_val     v_206;
                int             v_207;
                mirEXPR         v_208;
                mirTYPE         v_209;
                o_va            v_210;
                o_const_val     v_211;
                int             v_212;
                mirEXPR         v_213;
                mirTYPE         v_214;
                o_va            v_215;
                o_const_val     v_216;
                int             v_217;
                mirRELATION     v_218;
                BOOL            v_219;
                mirEXPR         v_220;
                mirEXPR         v_221;
                mirTYPE         v_222;
                o_va            v_223;
                o_va            v_224;
                o_const_val     v_225;
                o_const_val     v_226;
                o_const_val     v_227;
                snum            v_228;
                snum            v_229;
                bool            v_230;
                real            v_231;
                real            v_232;
                bool            v_233;
                unum            v_234;
                unum            v_235;
                bool            v_236;
                int             v_237;
                int             v_238;
                BOOL            v_239;
                mirEXPR         v_240;
                mirEXPR         v_241;
                mirTYPE         v_242;
                snum            v_243;
                o_va            v_244;
                o_va            v_245;
                o_const_val     v_246;
                o_const_val     v_247;
                o_const_val     v_248;
                snum            v_249;
                snum            v_250;
                int             v_251;
                int             v_252;
                int             v_253;
                BOOL            v_254;
                mirEXPR         v_255;
                mirEXPR         v_256;
                mirTYPE         v_257;
                o_va            v_258;
                o_va            v_259;
                o_const_val     v_260;
                o_const_val     v_261;
                int             v_262;
                int             v_263;
                BOOL            v_264;
                mirEXPR         v_265;
                mirEXPR         v_266;
                mirTYPE         v_267;
                o_va            v_268;
                o_va            v_269;
                o_const_val     v_270;
                o_const_val     v_271;
                int             v_272;
                int             v_273;
                BOOL            v_274;
                mirEXPR         v_275;
                mirEXPR         v_276;
                mirTYPE         v_277;
                o_va            v_278;
                o_va            v_279;
                o_const_val     v_280;
                o_const_val     v_281;
                int             v_282;
                int             v_283;
                BOOL            v_284;
                mirEXPR         v_285;
                mirEXPR         v_286;
                mirTYPE         v_287;
                o_va            v_288;
                o_va            v_289;
                o_const_val     v_290;
                o_const_val     v_291;
                int             v_292;
                int             v_293;
                BOOL            v_294;
                mirEXPR         v_295;
                mirEXPR         v_296;
                mirTYPE         v_297;
                o_va            v_298;
                o_va            v_299;
                o_const_val     v_300;
                o_const_val     v_301;
                o_const_val     v_302;
                bool            v_303;
                bool            v_304;
                bool            v_305;
                int             v_306;
                int             v_307;
                BOOL            v_308;
                mirEXPR         v_309;
                mirEXPR         v_310;
                mirEXPR         v_311;
                mirTYPE         v_312;
                o_va            v_313;
                o_va            v_314;
                o_const_val     v_315;
                o_const_val     v_316;
                o_const_val     v_317;
                snum            v_318;
                real            v_319;
                unum            v_320;
                real            v_321;
                int             v_322;
                int             v_323;
                BOOL            v_324;
                mirEXPR         v_325;
                mirEXPR         v_326;
                mirEXPR         v_327;
                mirTYPE         v_328;
                o_va            v_329;
                o_va            v_330;
                o_const_val     v_331;
                o_const_val     v_332;
                int             v_333;
                int             v_334;
                BOOL            v_335;
                mirEXPR         v_336;
                mirEXPR         v_337;
                mirEXPR         v_338;
                mirTYPE         v_339;
                o_va            v_340;
                o_va            v_341;
                o_const_val     v_342;
                o_const_val     v_343;
                int             v_344;
                int             v_345;
                BOOL            v_346;
                mirEXPR         v_347;
                mirEXPR         v_348;
                mirTYPE         v_349;
                BOOL            v_350;
                mirEXPR         v_351;
                mirEXPR         v_352;
                mirTYPE         v_353;
                BOOL            v_354;
                mirEXPR         v_355;
                mirEXPR         v_356;
                mirTYPE         v_357;
                BOOL            v_358;
                mirEXPR         v_359;
                mirEXPR         v_360;
                mirEXPR         v_361;
                mirTYPE         v_362;
                o_va            v_363;
                o_va            v_364;
                o_const_val     v_365;
                o_const_val     v_366;
                int             v_367;
                int             v_368;
                int             v_369;
                if (1) {         
                        if (bool_to_c(bool_not(o_is_simple_type(o_type(p_13)))))
                                v_122 = o_va_top;
                        else {
                                 
                                 
                                if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirNoExpr ) {
                                        v_123 = (       0 , ((struct x_Sort10_mirNoExpr *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_va_top;
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirIntConst ) {
                                        v_124 = (       0 , ((struct x_Sort10_mirIntConst *)((  p_13  ) ))->f_Value) ;
                                        v_125 = (       0 , ((struct x_Sort10_mirIntConst *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_va_lift((bool_to_c(o_is_signed(v_125))) ? (o_const_val_create_from_2(snum_create(UNIV_INT_get_value(v_124))
                                                                                                                               )) : (o_const_val_create_from_5(snum_to_unum(snum_create(UNIV_INT_get_value(v_124))
                                                                         )))
                                                        );
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirRealConst ) {
                                        v_126 = (       0 , ((struct x_Sort10_mirRealConst *)((  p_13  ) ))->f_Value) ;
                                        v_127 = (       0 , ((struct x_Sort10_mirRealConst *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_va_lift(o_const_val_create_from_3(real_create(UNIV_REAL_get_value(v_126))
                                                                         ));
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirBoolConst ) {
                                        v_128 = (       0 , ((struct x_Sort10_mirBoolConst *)((  p_13  ) ))->f_Value) ;
                                        v_129 = (       0 , ((struct x_Sort10_mirBoolConst *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_va_lift(o_const_val_create_from_1(bool_create(( v_128 ) )
                                                                         ));
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirAddrConst ) {
                                        v_130 = (       0 , ((struct x_Sort10_mirAddrConst *)((  p_13  ) ))->f_Value) ;
                                        v_131 = (       0 , ((struct x_Sort10_mirAddrConst *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_uninit();
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirArrayCom ) {
                                        v_132 = (       0 , ((struct x_Sort10_mirArrayCom *)((  p_13  ) ))->f_Value) ;
                                        v_133 = (       0 , ((struct x_Sort10_mirArrayCom *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_uninit();
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirUnionCom ) {
                                        v_134 = (LIST_mirObject) (      0 , ((struct x_Sort10_mirUnionCom *)((  p_13  ) ))->f_Part) ;
                                        v_135 = (       0 , ((struct x_Sort10_mirUnionCom *)((  p_13  ) ))->f_Value) ;
                                        v_136 = (       0 , ((struct x_Sort10_mirUnionCom *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_uninit();
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirTypeSize ) {
                                        v_137 = (       0 , ((struct x_Sort10_mirTypeSize *)((  p_13  ) ))->f_OfType) ;
                                        v_138 = (       0 , ((struct x_Sort10_mirTypeSize *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_va_top;
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirObjectAddr ) {
                                        v_139 = (       0 , ((struct x_Sort10_mirObjectAddr *)((  p_13  ) ))->f_Obj) ;
                                        v_140 = (       0 , ((struct x_Sort10_mirObjectAddr *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_uninit();
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirContent ) {
                                        v_141 = (       0 , ((struct x_Sort10_mirContent *)((  p_13  ) ))->f_Addr) ;
                                        v_142 = (       0 , ((struct x_Sort10_mirContent *)((  p_13  ) ))->f_Type) ;
                                         
                                         
                                        if (((((DMCP_xnode)(   v_141   ) )->op) )  == xop_Sort10_mirContent ) {
                                                v_143 = (       0 , ((struct x_Sort10_mirContent *)((  v_141  ) ))->f_Addr) ;
                                                v_144 = (       0 , ((struct x_Sort10_mirContent *)((  v_141  ) ))->f_Type) ;
                                                 
                                                if (((((DMCP_xnode)(   v_143   ) )->op) )  == xop_Sort10_mirObjectAddr ) {
                                                        v_145 = (       0 , ((struct x_Sort10_mirObjectAddr *)((  v_143  ) ))->f_Obj) ;
                                                        v_146 = (       0 , ((struct x_Sort10_mirObjectAddr *)((  v_143  ) ))->f_Type) ;
                                                        if (1) {         
                                                                {
                                                                        v_147 = o_alias_get(p_15, o_id(v_145));
                                                                        if (bool_to_c(bool_or(o_t1_eq(v_147, o_t1_top), o_t1_eq(v_147, o_t1_bottom))))
                                                                                v_122 = o_va_top;
                                                                        else {
                                                                                v_148 = o_t1_drop(v_147);
                                                                                v_122 = o_const_fkt_get(p_14, v_148);
                                                                        }
                                                                }
                                                        } else {;        
                                                                goto l_33;
                                                        }        
                                                } else   
                                        l_33:
                                                {        
                                                        ;
                                                        goto l_32;
                                                }        
                                        } else
                                           if (((((DMCP_xnode)(   v_141   ) )->op) )  == xop_Sort10_mirObjectAddr ) {
                                                v_149 = (       0 , ((struct x_Sort10_mirObjectAddr *)((  v_141  ) ))->f_Obj) ;
                                                v_150 = (       0 , ((struct x_Sort10_mirObjectAddr *)((  v_141  ) ))->f_Type) ;
                                                 
                                                 
                                                if (((((DMCP_xnode)(   v_149   ) )->op) )  == xop_Sort10_mirDataGlobal ) {
                                                        v_151 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_149  ) ))->f_IsWriteOnce) ;
                                                        v_152 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_149  ) ))->f_InitialValue) ;
                                                        v_153 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_149  ) ))->f_Section) ;
                                                        v_154 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_149  ) ))->f_Name) ;
                                                        v_155 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_149  ) ))->f_IsVolatile) ;
                                                        v_156 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_149  ) ))->f_Type) ;
                                                        v_157 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_149  ) ))->f_cpid) ;
                                                        if (1) {         
                                                                v_122 = (bool_to_c(bool_create(( v_151 ) )
                                                                                   )) ? (o_value(v_152, p_14, p_15)) : (o_const_fkt_get(p_14, o_myid(v_157)));
                                                        } else {;        
                                                                goto l_34;
                                                        }        
                                                } else   
                                        l_34:
                                                {        
                                                        ;
                                                        if (1) {         
                                                                v_122 = o_const_fkt_get(p_14, o_id(v_149));
                                                        } else {;        
                                                                v_158 = 1;       
                                                        }
                                                }
                                                v_158 = 0;       
                                                if (v_158) {;    
                                                        goto l_32;
                                                }        
                                        } else   
                                l_32:
                                        {        
                                                ;
                                                if (1) {         
                                                        v_122 = o_va_top;
                                                } else {;        
                                                        v_159 = 1;       
                                                }
                                        }
                                        v_159 = 0;       
                                        if (v_159) {;    
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirSubscript ) {
                                        v_160 = (       0 , ((struct x_Sort10_mirSubscript *)((  p_13  ) ))->f_Index) ;
                                        v_161 = (       0 , ((struct x_Sort10_mirSubscript *)((  p_13  ) ))->f_Base) ;
                                        v_162 = (       0 , ((struct x_Sort10_mirSubscript *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_va_top;
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirMember ) {
                                        v_163 = (LIST_mirObject) (      0 , ((struct x_Sort10_mirMember *)((  p_13  ) ))->f_Field) ;
                                        v_164 = (       0 , ((struct x_Sort10_mirMember *)((  p_13  ) ))->f_Base) ;
                                        v_165 = (       0 , ((struct x_Sort10_mirMember *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_uninit();
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirBitExtract ) {
                                        v_166 = (       0 , ((struct x_Sort10_mirBitExtract *)((  p_13  ) ))->f_Width) ;
                                        v_167 = (       0 , ((struct x_Sort10_mirBitExtract *)((  p_13  ) ))->f_Start) ;
                                        v_168 = (       0 , ((struct x_Sort10_mirBitExtract *)((  p_13  ) ))->f_Src) ;
                                        v_169 = (       0 , ((struct x_Sort10_mirBitExtract *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_va_top;
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirBitInsert ) {
                                        v_170 = (       0 , ((struct x_Sort10_mirBitInsert *)((  p_13  ) ))->f_Src) ;
                                        v_171 = (       0 , ((struct x_Sort10_mirBitInsert *)((  p_13  ) ))->f_Width) ;
                                        v_172 = (       0 , ((struct x_Sort10_mirBitInsert *)((  p_13  ) ))->f_Start) ;
                                        v_173 = (       0 , ((struct x_Sort10_mirBitInsert *)((  p_13  ) ))->f_Dest) ;
                                        v_174 = (       0 , ((struct x_Sort10_mirBitInsert *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_va_top;
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirBoolVal ) {
                                        v_175 = (       0 , ((struct x_Sort10_mirBoolVal *)((  p_13  ) ))->f_False) ;
                                        v_176 = (       0 , ((struct x_Sort10_mirBoolVal *)((  p_13  ) ))->f_True) ;
                                        v_177 = (       0 , ((struct x_Sort10_mirBoolVal *)((  p_13  ) ))->f_Value) ;
                                        v_178 = (       0 , ((struct x_Sort10_mirBoolVal *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_179 = o_mycast(o_value(v_177, p_14, p_15), snum_create("1"));
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_179)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_35;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_179)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_35;
                                                                        }        
                                                                } else   
                                                        l_35:
                                                                {        
                                                                        ;
                                                                        if (1) {         
                                                                                {
                                                                                        v_180 = o_va_drop(v_179);
                                                                                        v_122 = (bool_to_c(o_const_val_is_1(v_180))) ? (o_va_lift(o_const_val_create_from_2(snum_create(UNIV_INT_get_value((bool_to_c(o_const_val_down_to_1(v_180))) ? (v_176) : (v_175)
                                                                                                                                                                                                           ))
                                                                                                                                                                            ))) : ((bool_to_c(o_const_val_is_4(v_180))) ? (o_va_lift(v_180)) : ((o_va) fatal_error_ (str_create("Unkwon type in mirBoolVal")))
                                                                                                );
                                                                                }
                                                                        } else {;        
                                                                                v_181 = 1;       
                                                                        }
                                                                }
                                                                v_181 = 0;       
                                                                if (v_181) {;    
                                                                        ein_neur_name= fatal_error__("1.c",4095)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirConvert ) {
                                        v_182 = (       0 , ((struct x_Sort10_mirConvert *)((  p_13  ) ))->f_Rounding) ;
                                        v_183 = (       0 , ((struct x_Sort10_mirConvert *)((  p_13  ) ))->f_Value) ;
                                        v_184 = (       0 , ((struct x_Sort10_mirConvert *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                if (bool_to_c(bool_not(o_is_simple_type(o_type(v_183)))))
                                                        v_122 = o_va_top;
                                                else {
                                                        v_185 = o_value(v_183, p_14, p_15);
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_185)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_36;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_185)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_36;
                                                                        }        
                                                                } else   
                                                        l_36:
                                                                {        
                                                                        ;
                                                                        if (1) {         
                                                                                {
                                                                                        v_186 = o_va_drop(v_185);
                                                                                        if (bool_to_c(o_const_val_is_2(v_186))) {
                                                                                                 
                                                                                                 
                                                                                                if (((((DMCP_xnode)(   v_184   ) )->op) )  == xop_Sort10_mirReal ) {
                                                                                                        if (1) {         
                                                                                                                v_122 = o_va_lift(o_const_val_create_from_3(snum_to_real(o_const_val_down_to_2(v_186))));
                                                                                                        } else {;        
                                                                                                                goto l_37;
                                                                                                        }        
                                                                                                } else
                                                                                                         
 if (((((DMCP_xnode)(   v_184   ) )->op) )  == xop_Sort10_mirInteger ) {
                                                                                                        v_187 = (       0 , ((struct x_Sort10_mirInteger *)((  v_184  ) ))->f_Signed) ;
                                                                                                        if (1) {         
                                                                                                                v_122 = (bool_to_c(bool_create(( v_187 ) )
                                                                                                                                   )) ? (o_va_lift(v_186)) : (o_va_lift(o_const_val_create_from_5(snum_to_unum(snum_abs(o_const_val_down_to_2(v_186))))));
                                                                                                        } else {;        
                                                                                                                goto l_37;
                                                                                                        }        
                                                                                                } else   
                                                                                        l_37:
                                                                                                {        
                                                                                                        ;
                                                                                                        if (1) {         
                                                                                                                v_122 = (o_va) fatal_error_ (str_create("Unknown Type convert"));
                                                                                                        } else {;        
                                                                                                                v_188 = 1;       
                                                                                                        }
                                                                                                }
                                                                                                v_188 = 0;       
                                                                                                if (v_188) {;    
                                                                                                        ein_neur_name= fatal_error__("1.c",4258)+fatal_error_ ("No pattern matched in case Expression (305,7-315,14)");
                                                                                                }
                                                                                        } else if (bool_to_c(o_const_val_is_5(v_186))) {
                                                                                                 
                                                                                                 
                                                                                                if (((((DMCP_xnode)(   v_184   ) )->op) )  == xop_Sort10_mirReal ) {
                                                                                                        if (1) {         
                                                                                                                v_122 = o_va_lift(o_const_val_create_from_3(unum_to_real(o_const_val_down_to_5(v_186))));
                                                                                                        } else {;        
                                                                                                                goto l_38;
                                                                                                        }        
                                                                                                } else
                                                                                                         
 if (((((DMCP_xnode)(   v_184   ) )->op) )  == xop_Sort10_mirInteger ) {
                                                                                                        v_189 = (       0 , ((struct x_Sort10_mirInteger *)((  v_184  ) ))->f_Signed) ;
                                                                                                        if (1) {         
                                                                                                                v_122 = (bool_to_c(bool_create(( v_189 ) )
                                                                                                                                   )) ? (o_va_lift(o_const_val_create_from_2(unum_to_snum(o_const_val_down_to_5(v_186))))) : (o_va_lift(v_186));
                                                                                                        } else {;        
                                                                                                                goto l_38;
                                                                                                        }        
                                                                                                } else   
                                                                                        l_38:
                                                                                                {        
                                                                                                        ;
                                                                                                        if (1) {         
                                                                                                                v_122 = (o_va) fatal_error_ (str_create("Unknown Type convert"));
                                                                                                        } else {;        
                                                                                                                v_190 = 1;       
                                                                                                        }
                                                                                                }
                                                                                                v_190 = 0;       
                                                                                                if (v_190) {;    
                                                                                                        ein_neur_name= fatal_error__("1.c",4361)+fatal_error_ ("No pattern matched in case Expression (317,7-327,14)");
                                                                                                }
                                                                                        } else if (bool_to_c(o_const_val_is_4(v_186)))
                                                                                                v_122 = o_va_lift(v_186);
                                                                                        else if (bool_to_c(o_const_val_is_3(v_186))) {
                                                                                                v_191 = o_const_val_down_to_3(v_186);
                                                                                                {
                                                                                                        v_192 = real_sgn(v_191);
                                                                                                        v_193 = snum_geq(real_sgn(v_191), snum_create("0"));
                                                                                                        v_194 = real_abs(v_191);
                                                                                                        {
                                                                                                                v_195 = real_to_unum(real_add(v_194, real_create(".5")));
                                                                                                                v_196 = real_to_unum(v_194);
                                                                                                                {
                                                                                                                        v_197 = (bool_to_c(real_eq(unum_to_real(v_196), v_194))) ? (v_196) : (unum_add(v_196, unum_create("1")));
                                                                                                                        {
                                                                                                                                {
                                                                                                                                         
                                                                                                                                        if (( v_182 )  == Truncation ) {
                                                                                                                                                if (1) {         
                                                                                                                                                        v_198 = v_196;
                                                                                                                                                } else {;        
                                                                                                                                                        goto l_39;
                                                                                                                                                }        
                                                                                                                                        } else
                                                                                                                                                 
 if (( v_182 )  == Nearest ) {
                                                                                                                                                if (1) {         
                                                                                                                                                        v_198 = v_195;
                                                                                                                                                } else {;        
                                                                                                                                                        goto l_39;
                                                                                                                                                }        
                                                                                                                                        } else
                                                                                                                                                 
 if (( v_182 )  == Floor ) {
                                                                                                                                                if (1) {         
                                                                                                                                                        v_198 = (bool_to_c(v_193)) ? (v_196) : (v_197);
                                                                                                                                                } else {;        
                                                                                                                                                        goto l_39;
                                                                                                                                                }        
                                                                                                                                        } else
                                                                                                                                                 
 if (( v_182 )  == Ceiling ) {
                                                                                                                                                if (1) {         
                                                                                                                                                        v_198 = (bool_to_c(v_193)) ? (v_197) : (v_196);
                                                                                                                                                } else {;        
                                                                                                                                                        goto l_39;
                                                                                                                                                }        
                                                                                                                                        } else   
                                                                                                                                l_39:
                                                                                                                                        {        
                                                                                                                                                ;
                                                                                                                                                ein_neur_name= fatal_error__("1.c",4496)+fatal_error_ ("No pattern matched in case Expression (339,7-344,14)");
                                                                                                                                        }
                                                                                                                                }
                                                                                                                                {
                                                                                                                                         
                                                                                                                                        if (((((DMCP_xnode)(   v_184   ) )->op) )  == xop_Sort10_mirReal ) {
                                                                                                                                                if (1) {         
                                                                                                                                                        v_122 = o_va_lift(v_186);
                                                                                                                                                } else {;        
                                                                                                                                                        goto l_40;
                                                                                                                                                }        
                                                                                                                                        } else
                                                                                                                                                 
 if (((((DMCP_xnode)(   v_184   ) )->op) )  == xop_Sort10_mirInteger ) {
                                                                                                                                                v_199 = (       0 , ((struct x_Sort10_mirInteger *)((  v_184  ) ))->f_Signed) ;
                                                                                                                                                if (1) {         
                                                                                                                                                        v_122 = (bool_to_c(bool_create(( v_199 ) )
                                                                                                                                                                           )) ? (o_va_lift(o_const_val_create_from_2(snum_mal(v_192, unum_to_snum(v_198))))) : (o_va_lift(o_const_val_create_from_5(v_198)));
                                                                                                                                                } else {;        
                                                                                                                                                        goto l_40;
                                                                                                                                                }        
                                                                                                                                        } else   
                                                                                                                                l_40:
                                                                                                                                        {        
                                                                                                                                                ;
                                                                                                                                                ein_neur_name= fatal_error__("1.c",4567)+fatal_error_ ("No pattern matched in case Expression (345,8-352,15)");
                                                                                                                                        }
                                                                                                                                }
                                                                                                                        }
                                                                                                                }
                                                                                                        }
                                                                                                }
                                                                                        } else
                                                                                                v_122 = (o_va) fatal_error_ (str_create("Unknown type-convert"));
                                                                                }
                                                                        } else {;        
                                                                                v_200 = 1;       
                                                                        }
                                                                }
                                                                v_200 = 0;       
                                                                if (v_200) {;    
                                                                        ein_neur_name= fatal_error__("1.c",4596)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirCast ) {
                                        v_201 = (       0 , ((struct x_Sort10_mirCast *)((  p_13  ) ))->f_Value) ;
                                        v_202 = (       0 , ((struct x_Sort10_mirCast *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_va_top;
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirAbs ) {
                                        v_203 = (       0 , ((struct x_Sort10_mirAbs *)((  p_13  ) ))->f_Value) ;
                                        v_204 = (       0 , ((struct x_Sort10_mirAbs *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_205 = o_value(v_203, p_14, p_15);
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_205)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_41;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_205)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_41;
                                                                        }        
                                                                } else   
                                                        l_41:
                                                                {        
                                                                        ;
                                                                        if (1) {         
                                                                                {
                                                                                        v_206 = o_va_drop(v_205);
                                                                                        v_122 = (bool_to_c(o_const_val_is_2(v_206))) ? (o_va_lift(o_const_val_create_from_2(snum_abs(o_const_val_down_to_2(v_206))))) : ((bool_to_c(o_const_val_is_3(v_206))) ? (o_va_lift(o_const_val_create_from_3(real_abs(o_const_val_down_to_3(v_206))))) : ((bool_to_c(o_const_val_is_5(v_206))) ? (o_va_lift(o_const_val_create_from_5(unum_abs(o_const_val_down_to_5(v_206))))) : ((bool_to_c(o_const_val_is_4(v_206))) ? (o_va_lift(v_206)) : ((o_va) fatal_error_ (str_create("Wrong type")))
                                                                                                                                                                                                                                                                                                                                                                                                                                                                           )
                                                                                                                                                                                                                                                                                                                                                  )
                                                                                                );
                                                                                }
                                                                        } else {;        
                                                                                v_207 = 1;       
                                                                        }
                                                                }
                                                                v_207 = 0;       
                                                                if (v_207) {;    
                                                                        ein_neur_name= fatal_error__("1.c",4680)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirNeg ) {
                                        v_208 = (       0 , ((struct x_Sort10_mirNeg *)((  p_13  ) ))->f_Value) ;
                                        v_209 = (       0 , ((struct x_Sort10_mirNeg *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_210 = o_value(v_208, p_14, p_15);
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_210)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_42;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_210)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_42;
                                                                        }        
                                                                } else   
                                                        l_42:
                                                                {        
                                                                        ;
                                                                        if (1) {         
                                                                                {
                                                                                        v_211 = o_va_drop(v_210);
                                                                                        v_122 = (bool_to_c(o_const_val_is_2(v_211))) ? (o_va_lift(o_const_val_create_from_2(snum_unmin(o_const_val_down_to_2(v_211))))) : ((bool_to_c(o_const_val_is_3(v_211))) ? (o_va_lift(o_const_val_create_from_3(real_unmin(o_const_val_down_to_3(v_211))))) : ((bool_to_c(o_const_val_is_5(v_211))) ? (o_va_top) : ((bool_to_c(o_const_val_is_4(v_211))) ? (o_va_lift(v_211)) : ((o_va) fatal_error_ (str_create("Wrong type in neg")))
                                                                                                                                                                                                                                                                                                                                                                                                           )
                                                                                                                                                                                                                                                                                                                                                      )
                                                                                                );
                                                                                }
                                                                        } else {;        
                                                                                v_212 = 1;       
                                                                        }
                                                                }
                                                                v_212 = 0;       
                                                                if (v_212) {;    
                                                                        ein_neur_name= fatal_error__("1.c",4753)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirNot ) {
                                        v_213 = (       0 , ((struct x_Sort10_mirNot *)((  p_13  ) ))->f_Value) ;
                                        v_214 = (       0 , ((struct x_Sort10_mirNot *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_215 = o_value(v_213, p_14, p_15);
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_215)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_43;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_215)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_43;
                                                                        }        
                                                                } else   
                                                        l_43:
                                                                {        
                                                                        ;
                                                                        if (1) {         
                                                                                {
                                                                                        v_216 = o_va_drop(v_215);
                                                                                        v_122 = (bool_to_c(o_const_val_is_1(v_216))) ? (o_va_lift(o_const_val_create_from_1(bool_not(o_const_val_down_to_1(v_216))))) : ((o_va) fatal_error_ (str_create("Wrong type in not")));
                                                                                }
                                                                        } else {;        
                                                                                v_217 = 1;       
                                                                        }
                                                                }
                                                                v_217 = 0;       
                                                                if (v_217) {;    
                                                                        ein_neur_name= fatal_error__("1.c",4823)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirCompare ) {
                                        v_218 = (       0 , ((struct x_Sort10_mirCompare *)((  p_13  ) ))->f_Relation) ;
                                        v_219 = (       0 , ((struct x_Sort10_mirCompare *)((  p_13  ) ))->f_Strict) ;
                                        v_220 = (       0 , ((struct x_Sort10_mirCompare *)((  p_13  ) ))->f_Right) ;
                                        v_221 = (       0 , ((struct x_Sort10_mirCompare *)((  p_13  ) ))->f_Left) ;
                                        v_222 = (       0 , ((struct x_Sort10_mirCompare *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_223 = o_value(v_220, p_14, p_15);
                                                        v_224 = o_value(v_221, p_14, p_15);
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_223)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_44;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_223)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_44;
                                                                        }        
                                                                } else   
                                                        l_44:
                                                                {        
                                                                        ;        
                                                                         
                                                                        if (o_va_isbottom(v_224)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_bottom;
                                                                                } else {;        
                                                                                        goto l_45;
                                                                                }        
                                                                        } else
                                                                                 
 if (o_va_istop(v_224)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_top;
                                                                                } else {;        
                                                                                        goto l_45;
                                                                                }        
                                                                        } else   
                                                                l_45:
                                                                        {        
                                                                                ;
                                                                                if (1) {         
                                                                                        {
                                                                                                v_225 = o_va_drop(v_223);
                                                                                                v_226 = o_va_drop(v_224);
                                                                                                {
                                                                                                        if (bool_to_c(bool_and(o_const_val_is_2(v_226), o_const_val_is_2(v_225)))) {
                                                                                                                v_228 = o_const_val_down_to_2(v_226);
                                                                                                                v_229 = o_const_val_down_to_2(v_225);
                                                                                                                {{
                                                                                                                                 
                                                                                                                                if (( v_218 )  == Less ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_230 = snum_le(v_228, v_229);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_46;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == LessEqual ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_230 = snum_leq(v_228, v_229);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_46;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == Greater ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_230 = snum_ge(v_228, v_229);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_46;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == GreaterEqual ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_230 = snum_geq(v_228, v_229);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_46;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == Equal ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_230 = snum_eq(v_228, v_229);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_46;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == UnEqual ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_230 = snum_neq(v_228, v_229);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_46;
                                                                                                                                        }        
                                                                                                                                } else   
                                                                                                                        l_46:
                                                                                                                                {        
                                                                                                                                        ;
                                                                                                                                        ein_neur_name= fatal_error__("1.c",5111)+fatal_error_ ("No pattern matched in case Expression (389,7-396,14)");
                                                                                                                                }
                                                                                                                }
                                                                                                                v_227 = o_const_val_create_from_1(v_230);
                                                                                                                }
                                                                                                        } else if (bool_to_c(bool_and(o_const_val_is_3(v_226), o_const_val_is_3(v_225)))) {
                                                                                                                v_231 = o_const_val_down_to_3(v_226);
                                                                                                                v_232 = o_const_val_down_to_3(v_225);
                                                                                                                {{
                                                                                                                                 
                                                                                                                                if (( v_218 )  == Less ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_233 = real_le(v_231, v_232);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_47;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == LessEqual ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_233 = real_leq(v_231, v_232);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_47;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == Greater ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_233 = real_ge(v_231, v_232);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_47;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == GreaterEqual ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_233 = real_geq(v_231, v_232);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_47;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == Equal ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_233 = real_eq(v_231, v_232);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_47;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == UnEqual ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_233 = real_neq(v_231, v_232);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_47;
                                                                                                                                        }        
                                                                                                                                } else   
                                                                                                                        l_47:
                                                                                                                                {        
                                                                                                                                        ;
                                                                                                                                        ein_neur_name= fatal_error__("1.c",5290)+fatal_error_ ("No pattern matched in case Expression (405,8-412,15)");
                                                                                                                                }
                                                                                                                }
                                                                                                                v_227 = o_const_val_create_from_1(v_233);
                                                                                                                }
                                                                                                        } else if (bool_to_c(bool_and(o_const_val_is_5(v_226), o_const_val_is_5(v_225)))) {
                                                                                                                v_234 = o_const_val_down_to_5(v_226);
                                                                                                                v_235 = o_const_val_down_to_5(v_225);
                                                                                                                {{
                                                                                                                                 
                                                                                                                                if (( v_218 )  == Less ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_236 = unum_le(v_234, v_235);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_48;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == LessEqual ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_236 = unum_leq(v_234, v_235);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_48;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == Greater ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_236 = unum_ge(v_234, v_235);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_48;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == GreaterEqual ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_236 = unum_geq(v_234, v_235);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_48;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == Equal ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_236 = unum_eq(v_234, v_235);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_48;
                                                                                                                                        }        
                                                                                                                                } else
                                                                                                                                         
 if (( v_218 )  == UnEqual ) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_236 = unum_neq(v_234, v_235);
                                                                                                                                        } else {;        
                                                                                                                                                goto l_48;
                                                                                                                                        }        
                                                                                                                                } else   
                                                                                                                        l_48:
                                                                                                                                {        
                                                                                                                                        ;
                                                                                                                                        ein_neur_name= fatal_error__("1.c",5469)+fatal_error_ ("No pattern matched in case Expression (421,9-428,16)");
                                                                                                                                }
                                                                                                                }
                                                                                                                v_227 = o_const_val_create_from_1(v_236);
                                                                                                                }
                                                                                                        } else
                                                                                                                v_227 = (bool_to_c(bool_or(o_const_val_is_4(v_226), o_const_val_is_4(v_225)))) ? (o_const_val_create_from_4(o_nix)) : ((o_const_val) fatal_error_ (str_create("worong type in Compare")));
                                                                                                        v_122 = o_va_lift(v_227);
                                                                                                }
                                                                                        }
                                                                                } else {;        
                                                                                        v_237 = 1;       
                                                                                }
                                                                        }
                                                                        v_237 = 0;       
                                                                        if (v_237) {;    
                                                                                v_238 = 1;       
                                                                        }
                                                                }
                                                                v_238 = 0;       
                                                                if (v_238) {;    
                                                                        ein_neur_name= fatal_error__("1.c",5500)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirPlus ) {
                                        v_239 = (       0 , ((struct x_Sort10_mirPlus *)((  p_13  ) ))->f_Strict) ;
                                        v_240 = (       0 , ((struct x_Sort10_mirPlus *)((  p_13  ) ))->f_Right) ;
                                        v_241 = (       0 , ((struct x_Sort10_mirPlus *)((  p_13  ) ))->f_Left) ;
                                        v_242 = (       0 , ((struct x_Sort10_mirPlus *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_243 = o_which_type(v_242);
                                                        {
                                                                v_244 = o_mycast(o_value(v_240, p_14, p_15), v_243);
                                                                v_245 = o_mycast(o_value(v_241, p_14, p_15), v_243);
                                                                {
                                                                         
                                                                         
                                                                        if (o_va_isbottom(v_244)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_bottom;
                                                                                } else {;        
                                                                                        goto l_49;
                                                                                }        
                                                                        } else
                                                                                 
 if (o_va_istop(v_244)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_top;
                                                                                } else {;        
                                                                                        goto l_49;
                                                                                }        
                                                                        } else   
                                                                l_49:
                                                                        {        
                                                                                ;        
                                                                                 
                                                                                if (o_va_isbottom(v_245)) {
                                                                                        if (1) {         
                                                                                                v_122 = o_va_bottom;
                                                                                        } else {;        
                                                                                                goto l_50;
                                                                                        }        
                                                                                } else
                                                                                         
 if (o_va_istop(v_245)) {
                                                                                        if (1) {         
                                                                                                v_122 = o_va_top;
                                                                                        } else {;        
                                                                                                goto l_50;
                                                                                        }        
                                                                                } else   
                                                                        l_50:
                                                                                {        
                                                                                        ;
                                                                                        if (1) {         
                                                                                                {
                                                                                                        v_246 = o_va_drop(v_244);
                                                                                                        v_247 = o_va_drop(v_245);
                                                                                                        {{
                                                                                                                        v_249 = o_const_val_type(v_247);
                                                                                                                        v_250 = o_const_val_type(v_246);
                                                                                                                        {
                                                                                                                                 
                                                                                                                                 
                                                                                                                                if (snum_eq(v_249, snum_create("4"))) {
                                                                                                                                        if (1) {         
                                                                                                                                                v_248 = v_247;
                                                                                                                                        } else {;        
                                                                                                                                                goto l_51;
                                                                                                                                        }        
                                                                                                                                } else   
                                                                                                                        l_51:
                                                                                                                                {        
                                                                                                                                        ;
                                                                                                                                         
                                                                                                                                        if (snum_eq(v_250, snum_create("4"))) {
                                                                                                                                                if (1) {         
                                                                                                                                                        v_248 = v_246;
                                                                                                                                                } else {;        
                                                                                                                                                        goto l_52;
                                                                                                                                                }        
                                                                                                                                        } else   
                                                                                                                                l_52:
                                                                                                                                        {        
                                                                                                                                                ;
                                                                                                                                                 
                                                                                                                                                if (snum_eq(v_249, snum_create("2"))) {
                                                                                                                                                         
                                                                                                                                                        if (snum_eq(v_250, snum_create("2"))) {
                                                                                                                                                                if (1) {         
                                                                                                                                                                        v_248 = o_const_val_create_from_2(snum_add(o_const_val_down_to_2(v_247), o_const_val_down_to_2(v_246)));
                                                                                                                                                                } else {;        
                                                                                                                                                                        goto l_54;
                                                                                                                                                                }        
                                                                                                                                                        } else   
                                                                                                                                                l_54:
                                                                                                                                                        {        
                                                                                                                                                                ;
                                                                                                                                                                goto l_53;
                                                                                                                                                        }        
                                                                                                                                                } else
                                                                                                                                                         
 if (snum_eq(v_249, snum_create("3"))) {
                                                                                                                                                         
                                                                                                                                                        if (snum_eq(v_250, snum_create("3"))) {
                                                                                                                                                                if (1) {         
                                                                                                                                                                        v_248 = o_const_val_create_from_3(real_add(o_const_val_down_to_3(v_247), o_const_val_down_to_3(v_246)));
                                                                                                                                                                } else {;        
                                                                                                                                                                        goto l_55;
                                                                                                                                                                }        
                                                                                                                                                        } else   
                                                                                                                                                l_55:
                                                                                                                                                        {        
                                                                                                                                                                ;
                                                                                                                                                                goto l_53;
                                                                                                                                                        }        
                                                                                                                                                } else
                                                                                                                                                         
 if (snum_eq(v_249, snum_create("5"))) {
                                                                                                                                                         
                                                                                                                                                        if (snum_eq(v_250, snum_create("5"))) {
                                                                                                                                                                if (1) {         
                                                                                                                                                                        v_248 = o_const_val_create_from_5(unum_add(o_const_val_down_to_5(v_247), o_const_val_down_to_5(v_246)));
                                                                                                                                                                } else {;        
                                                                                                                                                                        goto l_56;
                                                                                                                                                                }        
                                                                                                                                                        } else   
                                                                                                                                                l_56:
                                                                                                                                                        {        
                                                                                                                                                                ;
                                                                                                                                                                goto l_53;
                                                                                                                                                        }        
                                                                                                                                                } else   
                                                                                                                                        l_53:
                                                                                                                                                {        
                                                                                                                                                        ;
                                                                                                                                                        if (1) {         
                                                                                                                                                                v_248 = (o_const_val) fatal_error_ (str_create("This should not happen"));
                                                                                                                                                        } else {;        
                                                                                                                                                                v_251 = 1;       
                                                                                                                                                        }
                                                                                                                                                }
                                                                                                                                        }
                                                                                                                                }
                                                                                                                                v_251 = 0;       
                                                                                                                                if (v_251) {;    
                                                                                                                                        ein_neur_name= fatal_error__("1.c",5964)+fatal_error_ ("No pattern matched in case Expression (448,8-455,12)");
                                                                                                                                }
                                                                                                                        }
                                                                                                        }
                                                                                                        v_122 = o_va_lift(v_248);
                                                                                                        }
                                                                                                }
                                                                                        } else {;        
                                                                                                v_252 = 1;       
                                                                                        }
                                                                                }
                                                                                v_252 = 0;       
                                                                                if (v_252) {;    
                                                                                        v_253 = 1;       
                                                                                }
                                                                        }
                                                                        v_253 = 0;       
                                                                        if (v_253) {;    
                                                                                ein_neur_name= fatal_error__("1.c",5989)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                        }
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirDiff ) {
                                        v_254 = (       0 , ((struct x_Sort10_mirDiff *)((  p_13  ) ))->f_Strict) ;
                                        v_255 = (       0 , ((struct x_Sort10_mirDiff *)((  p_13  ) ))->f_Right) ;
                                        v_256 = (       0 , ((struct x_Sort10_mirDiff *)((  p_13  ) ))->f_Left) ;
                                        v_257 = (       0 , ((struct x_Sort10_mirDiff *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_258 = o_value(v_255, p_14, p_15);
                                                        v_259 = o_value(v_256, p_14, p_15);
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_258)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_57;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_258)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_57;
                                                                        }        
                                                                } else   
                                                        l_57:
                                                                {        
                                                                        ;        
                                                                         
                                                                        if (o_va_isbottom(v_259)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_bottom;
                                                                                } else {;        
                                                                                        goto l_58;
                                                                                }        
                                                                        } else
                                                                                 
 if (o_va_istop(v_259)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_top;
                                                                                } else {;        
                                                                                        goto l_58;
                                                                                }        
                                                                        } else   
                                                                l_58:
                                                                        {        
                                                                                ;
                                                                                if (1) {         
                                                                                        {
                                                                                                v_260 = o_va_drop(v_258);
                                                                                                v_261 = o_va_drop(v_259);
                                                                                                v_122 = o_va_lift((bool_to_c(bool_and(o_const_val_is_2(v_261), o_const_val_is_2(v_260)))) ? (o_const_val_create_from_2(snum_min(o_const_val_down_to_2(v_261), o_const_val_down_to_2(v_260)))) : ((bool_to_c(bool_and(o_const_val_is_3(v_261), o_const_val_is_3(v_260)))) ? (o_const_val_create_from_3(real_min(o_const_val_down_to_3(v_261), o_const_val_down_to_3(v_260)))) : ((bool_to_c(bool_and(o_const_val_is_5(v_261), o_const_val_is_5(v_260)))) ? (o_const_val_create_from_5(unum_min(o_const_val_down_to_5(v_261), o_const_val_down_to_5(v_260)))) : ((bool_to_c(bool_or(o_const_val_is_4(v_261), o_const_val_is_4(v_260)))) ? (o_const_val_create_from_4(o_nix)) : ((o_const_val) fatal_error_ (str_create("wrong types (-)")))
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               )
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                )
                                                                                                                                                                                                                                                                                                 )
                                                                                                        );
                                                                                        }
                                                                                } else {;        
                                                                                        v_262 = 1;       
                                                                                }
                                                                        }
                                                                        v_262 = 0;       
                                                                        if (v_262) {;    
                                                                                v_263 = 1;       
                                                                        }
                                                                }
                                                                v_263 = 0;       
                                                                if (v_263) {;    
                                                                        ein_neur_name= fatal_error__("1.c",6127)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirMult ) {
                                        v_264 = (       0 , ((struct x_Sort10_mirMult *)((  p_13  ) ))->f_Strict) ;
                                        v_265 = (       0 , ((struct x_Sort10_mirMult *)((  p_13  ) ))->f_Right) ;
                                        v_266 = (       0 , ((struct x_Sort10_mirMult *)((  p_13  ) ))->f_Left) ;
                                        v_267 = (       0 , ((struct x_Sort10_mirMult *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_268 = o_value(v_265, p_14, p_15);
                                                        v_269 = o_value(v_266, p_14, p_15);
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_268)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_59;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_268)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_59;
                                                                        }        
                                                                } else   
                                                        l_59:
                                                                {        
                                                                        ;        
                                                                         
                                                                        if (o_va_isbottom(v_269)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_bottom;
                                                                                } else {;        
                                                                                        goto l_60;
                                                                                }        
                                                                        } else
                                                                                 
 if (o_va_istop(v_269)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_top;
                                                                                } else {;        
                                                                                        goto l_60;
                                                                                }        
                                                                        } else   
                                                                l_60:
                                                                        {        
                                                                                ;
                                                                                if (1) {         
                                                                                        {
                                                                                                v_270 = o_va_drop(v_268);
                                                                                                v_271 = o_va_drop(v_269);
                                                                                                v_122 = o_va_lift((bool_to_c(bool_and(o_const_val_is_2(v_271), o_const_val_is_2(v_270)))) ? (o_const_val_create_from_2(snum_mal(o_const_val_down_to_2(v_271), o_const_val_down_to_2(v_270)))) : ((bool_to_c(bool_and(o_const_val_is_3(v_271), o_const_val_is_3(v_270)))) ? (o_const_val_create_from_3(real_mal(o_const_val_down_to_3(v_271), o_const_val_down_to_3(v_270)))) : ((bool_to_c(bool_and(o_const_val_is_5(v_271), o_const_val_is_5(v_270)))) ? (o_const_val_create_from_5(unum_mal(o_const_val_down_to_5(v_271), o_const_val_down_to_5(v_270)))) : ((bool_to_c(bool_or(o_const_val_is_4(v_271), o_const_val_is_4(v_270)))) ? (o_const_val_create_from_4(o_nix)) : ((o_const_val) fatal_error_ (str_create("wrong types (*)")))
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               )
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                )
                                                                                                                                                                                                                                                                                                 )
                                                                                                        );
                                                                                        }
                                                                                } else {;        
                                                                                        v_272 = 1;       
                                                                                }
                                                                        }
                                                                        v_272 = 0;       
                                                                        if (v_272) {;    
                                                                                v_273 = 1;       
                                                                        }
                                                                }
                                                                v_273 = 0;       
                                                                if (v_273) {;    
                                                                        ein_neur_name= fatal_error__("1.c",6263)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirAnd ) {
                                        v_274 = (       0 , ((struct x_Sort10_mirAnd *)((  p_13  ) ))->f_Strict) ;
                                        v_275 = (       0 , ((struct x_Sort10_mirAnd *)((  p_13  ) ))->f_Right) ;
                                        v_276 = (       0 , ((struct x_Sort10_mirAnd *)((  p_13  ) ))->f_Left) ;
                                        v_277 = (       0 , ((struct x_Sort10_mirAnd *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_278 = o_value(v_275, p_14, p_15);
                                                        v_279 = o_value(v_276, p_14, p_15);
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_278)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_61;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_278)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_61;
                                                                        }        
                                                                } else   
                                                        l_61:
                                                                {        
                                                                        ;        
                                                                         
                                                                        if (o_va_isbottom(v_279)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_bottom;
                                                                                } else {;        
                                                                                        goto l_62;
                                                                                }        
                                                                        } else
                                                                                 
 if (o_va_istop(v_279)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_top;
                                                                                } else {;        
                                                                                        goto l_62;
                                                                                }        
                                                                        } else   
                                                                l_62:
                                                                        {        
                                                                                ;
                                                                                if (1) {         
                                                                                        {
                                                                                                v_280 = o_va_drop(v_278);
                                                                                                v_281 = o_va_drop(v_279);
                                                                                                v_122 = (bool_to_c(bool_and(o_const_val_is_1(v_281), o_const_val_is_1(v_280)))) ? (o_va_lift(o_const_val_create_from_1(bool_and(o_const_val_down_to_1(v_281), o_const_val_down_to_1(v_280))))) : ((bool_to_c(bool_and(o_const_val_is_2(v_281), o_const_val_is_2(v_280)))) ? (o_va_top) : ((bool_to_c(bool_and(o_const_val_is_5(v_281), o_const_val_is_5(v_280)))) ? (o_va_top) : ((bool_to_c(bool_or(o_const_val_is_4(v_281), o_const_val_is_4(v_280)))) ? (o_va_lift(o_const_val_create_from_4(o_nix))) : ((o_va) fatal_error_ (str_create("wrong types (&&) ")))
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  )
                                                                                                                                                                                                                                                                                                                                                                                          )
                                                                                                        );
                                                                                        }
                                                                                } else {;        
                                                                                        v_282 = 1;       
                                                                                }
                                                                        }
                                                                        v_282 = 0;       
                                                                        if (v_282) {;    
                                                                                v_283 = 1;       
                                                                        }
                                                                }
                                                                v_283 = 0;       
                                                                if (v_283) {;    
                                                                        ein_neur_name= fatal_error__("1.c",6398)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirOr ) {
                                        v_284 = (       0 , ((struct x_Sort10_mirOr *)((  p_13  ) ))->f_Strict) ;
                                        v_285 = (       0 , ((struct x_Sort10_mirOr *)((  p_13  ) ))->f_Right) ;
                                        v_286 = (       0 , ((struct x_Sort10_mirOr *)((  p_13  ) ))->f_Left) ;
                                        v_287 = (       0 , ((struct x_Sort10_mirOr *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_288 = o_value(v_285, p_14, p_15);
                                                        v_289 = o_value(v_286, p_14, p_15);
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_288)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_63;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_288)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_63;
                                                                        }        
                                                                } else   
                                                        l_63:
                                                                {        
                                                                        ;        
                                                                         
                                                                        if (o_va_isbottom(v_289)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_bottom;
                                                                                } else {;        
                                                                                        goto l_64;
                                                                                }        
                                                                        } else
                                                                                 
 if (o_va_istop(v_289)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_top;
                                                                                } else {;        
                                                                                        goto l_64;
                                                                                }        
                                                                        } else   
                                                                l_64:
                                                                        {        
                                                                                ;
                                                                                if (1) {         
                                                                                        {
                                                                                                v_290 = o_va_drop(v_288);
                                                                                                v_291 = o_va_drop(v_289);
                                                                                                v_122 = (bool_to_c(bool_and(o_const_val_is_1(v_291), o_const_val_is_1(v_290)))) ? (o_va_lift(o_const_val_create_from_1(bool_or(o_const_val_down_to_1(v_291), o_const_val_down_to_1(v_290))))) : ((bool_to_c(bool_and(o_const_val_is_2(v_291), o_const_val_is_2(v_290)))) ? (o_va_top) : ((bool_to_c(bool_and(o_const_val_is_5(v_291), o_const_val_is_5(v_290)))) ? (o_va_top) : ((bool_to_c(bool_or(o_const_val_is_4(v_291), o_const_val_is_4(v_290)))) ? (o_va_lift(o_const_val_create_from_4(o_nix))) : ((o_va) fatal_error_ (str_create("wrong types (|)")))
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 )
                                                                                                                                                                                                                                                                                                                                                                                         )
                                                                                                        );
                                                                                        }
                                                                                } else {;        
                                                                                        v_292 = 1;       
                                                                                }
                                                                        }
                                                                        v_292 = 0;       
                                                                        if (v_292) {;    
                                                                                v_293 = 1;       
                                                                        }
                                                                }
                                                                v_293 = 0;       
                                                                if (v_293) {;    
                                                                        ein_neur_name= fatal_error__("1.c",6533)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirXor ) {
                                        v_294 = (       0 , ((struct x_Sort10_mirXor *)((  p_13  ) ))->f_Strict) ;
                                        v_295 = (       0 , ((struct x_Sort10_mirXor *)((  p_13  ) ))->f_Right) ;
                                        v_296 = (       0 , ((struct x_Sort10_mirXor *)((  p_13  ) ))->f_Left) ;
                                        v_297 = (       0 , ((struct x_Sort10_mirXor *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_298 = o_value(v_295, p_14, p_15);
                                                        v_299 = o_value(v_296, p_14, p_15);
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_298)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_65;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_298)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_65;
                                                                        }        
                                                                } else   
                                                        l_65:
                                                                {        
                                                                        ;        
                                                                         
                                                                        if (o_va_isbottom(v_299)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_bottom;
                                                                                } else {;        
                                                                                        goto l_66;
                                                                                }        
                                                                        } else
                                                                                 
 if (o_va_istop(v_299)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_top;
                                                                                } else {;        
                                                                                        goto l_66;
                                                                                }        
                                                                        } else   
                                                                l_66:
                                                                        {        
                                                                                ;
                                                                                if (1) {         
                                                                                        {
                                                                                                v_300 = o_va_drop(v_298);
                                                                                                v_301 = o_va_drop(v_299);
                                                                                                if (bool_to_c(bool_and(o_const_val_is_1(v_301), o_const_val_is_1(v_300)))) {{{
                                                                                                                        v_304 = o_const_val_down_to_1(v_301);
                                                                                                                        v_305 = o_const_val_down_to_1(v_300);
                                                                                                                        v_303 = bool_and(bool_or(v_304, v_305), bool_not(bool_and(v_304, v_305)));
                                                                                                }
                                                                                                v_302 = o_const_val_create_from_1(v_303);
                                                                                                }
                                                                                                v_122 = o_va_lift(v_302);
                                                                                                } else
                                                                                                        v_122 = (bool_to_c(bool_and(o_const_val_is_2(v_301), o_const_val_is_2(v_300)))) ? (o_va_top) : ((bool_to_c(bool_and(o_const_val_is_5(v_301), o_const_val_is_5(v_300)))) ? (o_va_top) : ((bool_to_c(bool_or(o_const_val_is_4(v_301), o_const_val_is_4(v_300)))) ? (o_va_lift(o_const_val_create_from_4(o_nix))) : ((o_va) fatal_error_ (str_create("wrong types(^)")))
                                                                                                                                                                                                                                                                                                )
                                                                                                                );
                                                                                        }
                                                                                } else {;        
                                                                                        v_306 = 1;       
                                                                                }
                                                                        }
                                                                        v_306 = 0;       
                                                                        if (v_306) {;    
                                                                                v_307 = 1;       
                                                                        }
                                                                }
                                                                v_307 = 0;       
                                                                if (v_307) {;    
                                                                        ein_neur_name= fatal_error__("1.c",6678)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirDiv ) {
                                        v_308 = (       0 , ((struct x_Sort10_mirDiv *)((  p_13  ) ))->f_Strict) ;
                                        v_309 = (       0 , ((struct x_Sort10_mirDiv *)((  p_13  ) ))->f_Right) ;
                                        v_310 = (       0 , ((struct x_Sort10_mirDiv *)((  p_13  ) ))->f_Left) ;
                                        v_311 = (       0 , ((struct x_Sort10_mirDiv *)((  p_13  ) ))->f_OnZero) ;
                                        v_312 = (       0 , ((struct x_Sort10_mirDiv *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_313 = o_value(v_309, p_14, p_15);
                                                        v_314 = o_value(v_310, p_14, p_15);
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_313)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_67;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_313)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_67;
                                                                        }        
                                                                } else   
                                                        l_67:
                                                                {        
                                                                        ;        
                                                                         
                                                                        if (o_va_isbottom(v_314)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_bottom;
                                                                                } else {;        
                                                                                        goto l_68;
                                                                                }        
                                                                        } else
                                                                                 
 if (o_va_istop(v_314)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_top;
                                                                                } else {;        
                                                                                        goto l_68;
                                                                                }        
                                                                        } else   
                                                                l_68:
                                                                        {        
                                                                                ;
                                                                                if (1) {         
                                                                                        {
                                                                                                v_315 = o_va_drop(v_313);
                                                                                                v_316 = o_va_drop(v_314);
                                                                                                {
                                                                                                        if (bool_to_c(bool_and(o_const_val_is_2(v_316), o_const_val_is_2(v_315)))) {{
                                                                                                                        v_319 = real_geteilt(snum_to_real(o_const_val_down_to_2(v_316)), snum_to_real(o_const_val_down_to_2(v_315)));
                                                                                                                        v_318 = real_trunc((bool_to_c(real_le(v_319, real_create("0.0")))) ? (real_min(v_319, real_create("1.0"))) : (v_319)
                                                                                                                                );
                                                                                                        }
                                                                                                        v_317 = o_const_val_create_from_2(v_318);
                                                                                                        } else if (bool_to_c(bool_and(o_const_val_is_5(v_316), o_const_val_is_5(v_315)))) {{
                                                                                                                        v_321 = real_geteilt(unum_to_real(o_const_val_down_to_5(v_316)), unum_to_real(o_const_val_down_to_5(v_315)));
                                                                                                                        v_320 = snum_to_unum(real_trunc((bool_to_c(real_le(v_321, real_create("0.0")))) ? (real_min(v_321, real_create("1.0"))) : (v_321)
                                                                                                                                                        ));
                                                                                                        }
                                                                                                        v_317 = o_const_val_create_from_5(v_320);
                                                                                                        } else
                                                                                                                v_317 = (bool_to_c(bool_and(o_const_val_is_3(v_316), o_const_val_is_3(v_315)))) ? (o_const_val_create_from_3(real_geteilt(o_const_val_down_to_3(v_316), o_const_val_down_to_3(v_315)))) : ((bool_to_c(bool_or(o_const_val_is_4(v_316), o_const_val_is_4(v_315)))) ? (o_const_val_create_from_4(o_nix)) : ((o_const_val) fatal_error_ (str_create("wrong types (/)")))
                                                                                                                        );
                                                                                                        v_122 = o_va_lift(v_317);
                                                                                                }
                                                                                        }
                                                                                } else {;        
                                                                                        v_322 = 1;       
                                                                                }
                                                                        }
                                                                        v_322 = 0;       
                                                                        if (v_322) {;    
                                                                                v_323 = 1;       
                                                                        }
                                                                }
                                                                v_323 = 0;       
                                                                if (v_323) {;    
                                                                        ein_neur_name= fatal_error__("1.c",6832)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirQuo ) {
                                        v_324 = (       0 , ((struct x_Sort10_mirQuo *)((  p_13  ) ))->f_Strict) ;
                                        v_325 = (       0 , ((struct x_Sort10_mirQuo *)((  p_13  ) ))->f_Right) ;
                                        v_326 = (       0 , ((struct x_Sort10_mirQuo *)((  p_13  ) ))->f_Left) ;
                                        v_327 = (       0 , ((struct x_Sort10_mirQuo *)((  p_13  ) ))->f_OnZero) ;
                                        v_328 = (       0 , ((struct x_Sort10_mirQuo *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_329 = o_value(v_325, p_14, p_15);
                                                        v_330 = o_value(v_326, p_14, p_15);
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_329)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_69;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_329)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_69;
                                                                        }        
                                                                } else   
                                                        l_69:
                                                                {        
                                                                        ;        
                                                                         
                                                                        if (o_va_isbottom(v_330)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_bottom;
                                                                                } else {;        
                                                                                        goto l_70;
                                                                                }        
                                                                        } else
                                                                                 
 if (o_va_istop(v_330)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_top;
                                                                                } else {;        
                                                                                        goto l_70;
                                                                                }        
                                                                        } else   
                                                                l_70:
                                                                        {        
                                                                                ;
                                                                                if (1) {         
                                                                                        {
                                                                                                v_331 = o_va_drop(v_329);
                                                                                                v_332 = o_va_drop(v_330);
                                                                                                v_122 = o_va_lift((bool_to_c(bool_and(o_const_val_is_2(v_332), o_const_val_is_2(v_331)))) ? (o_const_val_create_from_2(snum_geteilt(o_const_val_down_to_2(v_332), o_const_val_down_to_2(v_331)))) : ((bool_to_c(bool_and(o_const_val_is_5(v_332), o_const_val_is_5(v_331)))) ? (o_const_val_create_from_5(unum_geteilt(o_const_val_down_to_5(v_332), o_const_val_down_to_5(v_331)))) : ((bool_to_c(bool_and(o_const_val_is_3(v_332), o_const_val_is_3(v_331)))) ? (o_const_val_create_from_3(real_geteilt(o_const_val_down_to_3(v_332), o_const_val_down_to_3(v_331)))) : ((bool_to_c(bool_or(o_const_val_is_4(v_332), o_const_val_is_4(v_331)))) ? (o_const_val_create_from_4(o_nix)) : ((o_const_val) fatal_error_ (str_create("wrong types (%%)")))
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           )
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        )
                                                                                                                                                                                                                                                                                                     )
                                                                                                        );
                                                                                        }
                                                                                } else {;        
                                                                                        v_333 = 1;       
                                                                                }
                                                                        }
                                                                        v_333 = 0;       
                                                                        if (v_333) {;    
                                                                                v_334 = 1;       
                                                                        }
                                                                }
                                                                v_334 = 0;       
                                                                if (v_334) {;    
                                                                        ein_neur_name= fatal_error__("1.c",6969)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirMod ) {
                                        v_335 = (       0 , ((struct x_Sort10_mirMod *)((  p_13  ) ))->f_Strict) ;
                                        v_336 = (       0 , ((struct x_Sort10_mirMod *)((  p_13  ) ))->f_Right) ;
                                        v_337 = (       0 , ((struct x_Sort10_mirMod *)((  p_13  ) ))->f_Left) ;
                                        v_338 = (       0 , ((struct x_Sort10_mirMod *)((  p_13  ) ))->f_OnZero) ;
                                        v_339 = (       0 , ((struct x_Sort10_mirMod *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_340 = o_value(v_336, p_14, p_15);
                                                        v_341 = o_value(v_337, p_14, p_15);
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_340)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_71;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_340)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_71;
                                                                        }        
                                                                } else   
                                                        l_71:
                                                                {        
                                                                        ;        
                                                                         
                                                                        if (o_va_isbottom(v_341)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_bottom;
                                                                                } else {;        
                                                                                        goto l_72;
                                                                                }        
                                                                        } else
                                                                                 
 if (o_va_istop(v_341)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_top;
                                                                                } else {;        
                                                                                        goto l_72;
                                                                                }        
                                                                        } else   
                                                                l_72:
                                                                        {        
                                                                                ;
                                                                                if (1) {         
                                                                                        {
                                                                                                v_342 = o_va_drop(v_340);
                                                                                                v_343 = o_va_drop(v_341);
                                                                                                v_122 = o_va_lift((bool_to_c(bool_and(o_const_val_is_2(v_343), o_const_val_is_2(v_342)))) ? (o_const_val_create_from_2(snum_mal(snum_sgn(o_const_val_down_to_2(v_343)), snum_modulo(snum_abs(o_const_val_down_to_2(v_343)), snum_abs(o_const_val_down_to_2(v_342)))))) : ((bool_to_c(bool_and(o_const_val_is_5(v_343), o_const_val_is_5(v_342)))) ? (o_const_val_create_from_5(unum_modulo(o_const_val_down_to_5(v_343), o_const_val_down_to_5(v_342)))) : ((bool_to_c(bool_and(o_const_val_is_3(v_343), o_const_val_is_3(v_342)))) ? (o_const_val_create_from_3(real_modulo(o_const_val_down_to_3(v_343), o_const_val_down_to_3(v_342)))) : ((bool_to_c(bool_or(o_const_val_is_4(v_343), o_const_val_is_4(v_342)))) ? (o_const_val_create_from_4(o_nix)) : ((o_const_val) fatal_error_ (str_create("wrong types (mod)")))
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              )
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            )
                                                                                                                                                                                                                                                                                                                                                                          )
                                                                                                        );
                                                                                        }
                                                                                } else {;        
                                                                                        v_344 = 1;       
                                                                                }
                                                                        }
                                                                        v_344 = 0;       
                                                                        if (v_344) {;    
                                                                                v_345 = 1;       
                                                                        }
                                                                }
                                                                v_345 = 0;       
                                                                if (v_345) {;    
                                                                        ein_neur_name= fatal_error__("1.c",7106)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirShiftLeft ) {
                                        v_346 = (       0 , ((struct x_Sort10_mirShiftLeft *)((  p_13  ) ))->f_Strict) ;
                                        v_347 = (       0 , ((struct x_Sort10_mirShiftLeft *)((  p_13  ) ))->f_Right) ;
                                        v_348 = (       0 , ((struct x_Sort10_mirShiftLeft *)((  p_13  ) ))->f_Left) ;
                                        v_349 = (       0 , ((struct x_Sort10_mirShiftLeft *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_va_top;
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirShiftRight ) {
                                        v_350 = (       0 , ((struct x_Sort10_mirShiftRight *)((  p_13  ) ))->f_Strict) ;
                                        v_351 = (       0 , ((struct x_Sort10_mirShiftRight *)((  p_13  ) ))->f_Right) ;
                                        v_352 = (       0 , ((struct x_Sort10_mirShiftRight *)((  p_13  ) ))->f_Left) ;
                                        v_353 = (       0 , ((struct x_Sort10_mirShiftRight *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_va_top;
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirShiftRightSign ) {
                                        v_354 = (       0 , ((struct x_Sort10_mirShiftRightSign *)((  p_13  ) ))->f_Strict) ;
                                        v_355 = (       0 , ((struct x_Sort10_mirShiftRightSign *)((  p_13  ) ))->f_Right) ;
                                        v_356 = (       0 , ((struct x_Sort10_mirShiftRightSign *)((  p_13  ) ))->f_Left) ;
                                        v_357 = (       0 , ((struct x_Sort10_mirShiftRightSign *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                v_122 = o_va_top;
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_13   ) )->op) )  == xop_Sort10_mirRem ) {
                                        v_358 = (       0 , ((struct x_Sort10_mirRem *)((  p_13  ) ))->f_Strict) ;
                                        v_359 = (       0 , ((struct x_Sort10_mirRem *)((  p_13  ) ))->f_Right) ;
                                        v_360 = (       0 , ((struct x_Sort10_mirRem *)((  p_13  ) ))->f_Left) ;
                                        v_361 = (       0 , ((struct x_Sort10_mirRem *)((  p_13  ) ))->f_OnZero) ;
                                        v_362 = (       0 , ((struct x_Sort10_mirRem *)((  p_13  ) ))->f_Type) ;
                                        if (1) {         
                                                {
                                                        v_363 = o_value(v_359, p_14, p_15);
                                                        v_364 = o_value(v_360, p_14, p_15);
                                                        {
                                                                 
                                                                 
                                                                if (o_va_isbottom(v_363)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_bottom;
                                                                        } else {;        
                                                                                goto l_73;
                                                                        }        
                                                                } else
                                                                         
 if (o_va_istop(v_363)) {
                                                                        if (1) {         
                                                                                v_122 = o_va_top;
                                                                        } else {;        
                                                                                goto l_73;
                                                                        }        
                                                                } else   
                                                        l_73:
                                                                {        
                                                                        ;        
                                                                         
                                                                        if (o_va_isbottom(v_364)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_bottom;
                                                                                } else {;        
                                                                                        goto l_74;
                                                                                }        
                                                                        } else
                                                                                 
 if (o_va_istop(v_364)) {
                                                                                if (1) {         
                                                                                        v_122 = o_va_top;
                                                                                } else {;        
                                                                                        goto l_74;
                                                                                }        
                                                                        } else   
                                                                l_74:
                                                                        {        
                                                                                ;
                                                                                if (1) {         
                                                                                        {
                                                                                                v_365 = o_va_drop(v_363);
                                                                                                v_366 = o_va_drop(v_364);
                                                                                                v_122 = o_va_lift((bool_to_c(bool_and(o_const_val_is_2(v_366), o_const_val_is_2(v_365)))) ? (o_const_val_create_from_2(snum_mal(snum_sgn(o_const_val_down_to_2(v_365)), snum_modulo(snum_abs(o_const_val_down_to_2(v_366)), snum_abs(o_const_val_down_to_2(v_365)))))) : ((bool_to_c(bool_and(o_const_val_is_5(v_366), o_const_val_is_5(v_365)))) ? (o_const_val_create_from_5(unum_modulo(o_const_val_down_to_5(v_366), o_const_val_down_to_5(v_365)))) : ((bool_to_c(bool_and(o_const_val_is_3(v_366), o_const_val_is_3(v_365)))) ? (o_const_val_create_from_3(real_modulo(o_const_val_down_to_3(v_366), o_const_val_down_to_3(v_365)))) : ((bool_to_c(bool_or(o_const_val_is_4(v_366), o_const_val_is_4(v_365)))) ? (o_const_val_create_from_4(o_nix)) : ((o_const_val) fatal_error_ (str_create("wrong types")))
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              )
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            )
                                                                                                                                                                                                                                                                                                                                                                          )
                                                                                                        );
                                                                                        }
                                                                                } else {;        
                                                                                        v_367 = 1;       
                                                                                }
                                                                        }
                                                                        v_367 = 0;       
                                                                        if (v_367) {;    
                                                                                v_368 = 1;       
                                                                        }
                                                                }
                                                                v_368 = 0;       
                                                                if (v_368) {;    
                                                                        ein_neur_name= fatal_error__("1.c",7279)+fatal_error_ ("No pattern matched in case Expression (2,1-778,11)");
                                                                }
                                                        }
                                                }
                                        } else {;        
                                                goto l_31;
                                        }        
                                } else   
                        l_31:
                                {        
                                        ;
                                        if (1) {         
                                                v_122 = o_va_top;
                                        } else {;        
                                                v_369 = 1;       
                                        }
                                }
                                v_369 = 0;       
                                if (v_369) {;    
                                        ein_neur_name= fatal_error__("1.c",7303)+fatal_error_ ("No pattern matched in case Expression (260,2-672,8)");
                                }
                        }
                } else {;        
                        ein_neur_name= fatal_error__("1.c",7308)+fatal_error_ ("No pattern matched in function o_value");
                }
                return (v_122);
        }
}
void 
o_value_stat(void)
{
}
static          o_is_signed_chit = 0;
static          o_is_signed_hit = 0;
static int      o_is_signed_initialized = 0, o_is_signed_active = 1, o_is_signed_size = 7;
static o_is_signed_MEMOP *o_is_signed_memotable;
bool 
o_is_signed(mirTYPE p_16)
{
        unsigned int    h;
        o_is_signed_MEMOP z;
        int             w;
        if (o_is_signed_active) {
                if (!o_is_signed_initialized) {
                        o_is_signed_memotable = (o_is_signed_MEMOP *) calloc(o_is_signed_size, sizeof(o_is_signed_MEMOP));
                        if (o_is_signed_memotable != ((void *)0) )
                                o_is_signed_initialized = 1;
                        else {
                                o_is_signed_active = 0;
                                goto o_is_signed_main;
                        }
                }
                h = 0;
                h += 2 * ((unsigned int)( p_16 )) ;
                h %= o_is_signed_size;
                for (w = 0, z = o_is_signed_memotable[h]; z != ((void *)0) ; w++, z = z->next) {
                        if (
                            p_16 == z->_1) {
                                if (w == 0)
                                        o_is_signed_chit += 1;
                                o_is_signed_hit += 1;
                                return (z->res);
                        }
                }
        } {
                bool            v_370;
                BOOL            v_371;
                int             v_372;
o_is_signed_main:
                 
                 
                if (((((DMCP_xnode)(   p_16   ) )->op) )  == xop_Sort10_mirInteger ) {
                        v_371 = (       0 , ((struct x_Sort10_mirInteger *)((  p_16  ) ))->f_Signed) ;
                        if (1) { 
                                v_370 = bool_create(( v_371 ) );
                        } else {;        
                                goto l_75;
                        }        
                } else           
        l_75:
                {                
                        ;
                        if (1) { 
                                v_370 = (bool) fatal_error_ (str_create("is_signed"));
                        } else {;        
                                v_372 = 1;       
                        }
                }
                v_372 = 0;       
                if (v_372) {;    
                        ein_neur_name= fatal_error__("1.c",7395)+fatal_error_ ("No pattern matched in function o_is_signed");
                }
                if (o_is_signed_active) {
                        z = (o_is_signed_MEMOP) malloc(sizeof(o_is_signed_MEMO));
                        if (z == ((void *)0) ) {
                                ein_neur_name= fatal_error__("1.c",7400)+fatal_error_ ("No mem");
                        }
                        z->_1 = p_16;
                        z->res = v_370;
                        z->next = o_is_signed_memotable[h];
                        o_is_signed_memotable[h] = z;
                }
                return (v_370);
        }
}
void 
o_is_signed_stat(void)
{
        int             o_is_signed_anz = 0, o_is_signed_max = 0;
        double          o_is_signed_durch = 0;
        int             s, leer = 0;
        int             j, i;
        o_is_signed_MEMOP z;
        if (o_is_signed_active && o_is_signed_initialized) {
                for (i = 0; i < o_is_signed_size; i++) {
                        for (j = 0, z = o_is_signed_memotable[i]; z != ((void *)0) ; j++, z = z->next) {
                                o_is_signed_anz += 1;
                        }
                        leer += j == 0 ? 1 : 0;
                        o_is_signed_max = o_is_signed_max < j ? j : o_is_signed_max;
                }
                o_is_signed_durch = (double) o_is_signed_anz / (double) (o_is_signed_size - leer);
        }
        if (o_is_signed_active) {
                s = o_is_signed_size;
                fprintf((&_iob[2]) , "o_is_signed:\n");
                fprintf((&_iob[2]) , " size=%d\n", s);
                fprintf((&_iob[2]) , " anz=%d\n", o_is_signed_anz);
                fprintf((&_iob[2]) , " max=%d\n", o_is_signed_max);
                fprintf((&_iob[2]) , " durchschnitt=%f\n", o_is_signed_durch);
                fprintf((&_iob[2]) , " cache hit=%d\n", o_is_signed_chit);
                fprintf((&_iob[2]) , " leere Eintraege=%d\n", leer);
                fprintf((&_iob[2]) , " hit=%d\n", o_is_signed_hit);
                fprintf((&_iob[2]) , " Guete=%f\n", (double) o_is_signed_hit / (double) o_is_signed_anz);
                fprintf((&_iob[2]) , " cache Guete=%f\n", (double) o_is_signed_chit / (double) o_is_signed_anz);
                fprintf((&_iob[2]) , "\n");
        }
}
o_const_fkt 
o_uninitfunc()
{
        {
                o_const_fkt     v_373;
                if (1) {         
                        v_373 = o_const_fkt_create(o_uninit());
                } else {;        
                        ein_neur_name= fatal_error__("1.c",7478)+fatal_error_ ("No pattern matched in function o_uninitfunc");
                }
                return (v_373);
        }
}
void 
o_uninitfunc_stat(void)
{
}
static          o_uninit_chit = 0;
static          o_uninit_hit = 0;
static o_va     o_uninit_result = ((void *)0) ;
o_va 
o_uninit()
{
        if (o_uninit_result != ((void *)0) ) {
                o_uninit_chit += 1;
                o_uninit_hit += 1;
                return (o_uninit_result);
        } {
                o_va            v_374;
                if (1) {         
                        v_374 = o_va_lift(o_const_val_create_from_4(o_nix));
                } else {;        
                        ein_neur_name= fatal_error__("1.c",7518)+fatal_error_ ("No pattern matched in function o_uninit");
                }
                o_uninit_result = v_374;
                return (v_374);
        }
}
void 
o_uninit_stat(void)
{
        int             o_uninit_anz = 0, o_uninit_max = 0;
        double          o_uninit_durch = 0;
        int             s, leer = 0;
        if (o_uninit_result != ((void *)0) ) {
                o_uninit_anz = 1;
                o_uninit_max = 1;
                o_uninit_durch = 1;
        }
        s = 1;
        fprintf((&_iob[2]) , "o_uninit:\n");
        fprintf((&_iob[2]) , " size=%d\n", s);
        fprintf((&_iob[2]) , " anz=%d\n", o_uninit_anz);
        fprintf((&_iob[2]) , " max=%d\n", o_uninit_max);
        fprintf((&_iob[2]) , " durchschnitt=%f\n", o_uninit_durch);
        fprintf((&_iob[2]) , " cache hit=%d\n", o_uninit_chit);
        fprintf((&_iob[2]) , " leere Eintraege=%d\n", leer);
        fprintf((&_iob[2]) , " hit=%d\n", o_uninit_hit);
        fprintf((&_iob[2]) , " Guete=%f\n", (double) o_uninit_hit / (double) o_uninit_anz);
        fprintf((&_iob[2]) , " cache Guete=%f\n", (double) o_uninit_chit / (double) o_uninit_anz);
        fprintf((&_iob[2]) , "\n");
}
static          o_type_chit = 0;
static          o_type_hit = 0;
static int      o_type_initialized = 0, o_type_active = 1, o_type_size = 7993;
static o_type_MEMOP *o_type_memotable;
mirTYPE 
o_type(mirEXPR p_17)
{
        unsigned int    h;
        o_type_MEMOP    z;
        int             w;
        if (o_type_active) {
                if (!o_type_initialized) {
                        o_type_memotable = (o_type_MEMOP *) calloc(o_type_size, sizeof(o_type_MEMOP));
                        if (o_type_memotable != ((void *)0) )
                                o_type_initialized = 1;
                        else {
                                o_type_active = 0;
                                goto o_type_main;
                        }
                }
                h = 0;
                h += 2 * ((unsigned int)( p_17 )) ;
                h %= o_type_size;
                for (w = 0, z = o_type_memotable[h]; z != ((void *)0) ; w++, z = z->next) {
                        if (
                            p_17 == z->_1) {
                                if (w == 0)
                                        o_type_chit += 1;
                                o_type_hit += 1;
                                return (z->res);
                        }
                }
        } {
                mirTYPE         v_375;
                mirTYPE         v_376;
                UNIV_INT        v_377;
                mirTYPE         v_378;
                UNIV_REAL       v_379;
                mirTYPE         v_380;
                BOOL            v_381;
                mirTYPE         v_382;
                UNIV_ADDRESS    v_383;
                mirTYPE         v_384;
                LIST_mirEXPR    v_385;
                mirTYPE         v_386;
                LIST_mirEXPR    v_387;
                mirTYPE         v_388;
                LIST_mirObject  v_389;
                mirEXPR         v_390;
                mirTYPE         v_391;
                mirTYPE         v_392;
                mirTYPE         v_393;
                mirObject       v_394;
                mirTYPE         v_395;
                mirEXPR         v_396;
                mirTYPE         v_397;
                mirEXPR         v_398;
                mirEXPR         v_399;
                mirTYPE         v_400;
                LIST_mirObject  v_401;
                mirEXPR         v_402;
                mirTYPE         v_403;
                mirEXPR         v_404;
                mirEXPR         v_405;
                mirEXPR         v_406;
                mirTYPE         v_407;
                mirEXPR         v_408;
                mirEXPR         v_409;
                mirEXPR         v_410;
                mirEXPR         v_411;
                mirTYPE         v_412;
                UNIV_INT        v_413;
                UNIV_INT        v_414;
                mirEXPR         v_415;
                mirTYPE         v_416;
                mirROUNDING     v_417;
                mirEXPR         v_418;
                mirTYPE         v_419;
                mirEXPR         v_420;
                mirTYPE         v_421;
                mirEXPR         v_422;
                mirTYPE         v_423;
                mirEXPR         v_424;
                mirTYPE         v_425;
                mirEXPR         v_426;
                mirTYPE         v_427;
                mirEXPR         v_428;
                mirTYPE         v_429;
                mirEXPR         v_430;
                mirTYPE         v_431;
                BOOL            v_432;
                mirEXPR         v_433;
                mirEXPR         v_434;
                mirTYPE         v_435;
                BOOL            v_436;
                mirEXPR         v_437;
                mirEXPR         v_438;
                mirTYPE         v_439;
                mirRELATION     v_440;
                BOOL            v_441;
                mirEXPR         v_442;
                mirEXPR         v_443;
                mirTYPE         v_444;
                BOOL            v_445;
                mirEXPR         v_446;
                mirEXPR         v_447;
                mirTYPE         v_448;
                BOOL            v_449;
                mirEXPR         v_450;
                mirEXPR         v_451;
                mirTYPE         v_452;
                BOOL            v_453;
                mirEXPR         v_454;
                mirEXPR         v_455;
                mirTYPE         v_456;
                BOOL            v_457;
                mirEXPR         v_458;
                mirEXPR         v_459;
                mirTYPE         v_460;
                BOOL            v_461;
                mirEXPR         v_462;
                mirEXPR         v_463;
                mirTYPE         v_464;
                BOOL            v_465;
                mirEXPR         v_466;
                mirEXPR         v_467;
                mirTYPE         v_468;
                BOOL            v_469;
                mirEXPR         v_470;
                mirEXPR         v_471;
                mirTYPE         v_472;
                BOOL            v_473;
                mirEXPR         v_474;
                mirEXPR         v_475;
                mirTYPE         v_476;
                BOOL            v_477;
                mirEXPR         v_478;
                mirEXPR         v_479;
                mirTYPE         v_480;
                BOOL            v_481;
                mirEXPR         v_482;
                mirEXPR         v_483;
                mirEXPR         v_484;
                mirTYPE         v_485;
                BOOL            v_486;
                mirEXPR         v_487;
                mirEXPR         v_488;
                mirEXPR         v_489;
                mirTYPE         v_490;
                BOOL            v_491;
                mirEXPR         v_492;
                mirEXPR         v_493;
                mirEXPR         v_494;
                mirTYPE         v_495;
                BOOL            v_496;
                mirEXPR         v_497;
                mirEXPR         v_498;
                mirEXPR         v_499;
                mirTYPE         v_500;
o_type_main:
                if (1) {         
                        {
                                 
                                if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirNoExpr ) {
                                        v_376 = (       0 , ((struct x_Sort10_mirNoExpr *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_376;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirIntConst ) {
                                        v_377 = (       0 , ((struct x_Sort10_mirIntConst *)((  p_17  ) ))->f_Value) ;
                                        v_378 = (       0 , ((struct x_Sort10_mirIntConst *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_378;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirRealConst ) {
                                        v_379 = (       0 , ((struct x_Sort10_mirRealConst *)((  p_17  ) ))->f_Value) ;
                                        v_380 = (       0 , ((struct x_Sort10_mirRealConst *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_380;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirBoolConst ) {
                                        v_381 = (       0 , ((struct x_Sort10_mirBoolConst *)((  p_17  ) ))->f_Value) ;
                                        v_382 = (       0 , ((struct x_Sort10_mirBoolConst *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_382;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirAddrConst ) {
                                        v_383 = (       0 , ((struct x_Sort10_mirAddrConst *)((  p_17  ) ))->f_Value) ;
                                        v_384 = (       0 , ((struct x_Sort10_mirAddrConst *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_384;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirArrayCom ) {
                                        v_385 = (       0 , ((struct x_Sort10_mirArrayCom *)((  p_17  ) ))->f_Value) ;
                                        v_386 = (       0 , ((struct x_Sort10_mirArrayCom *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_386;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirRecordCom ) {
                                        v_387 = (       0 , ((struct x_Sort10_mirRecordCom *)((  p_17  ) ))->f_Value) ;
                                        v_388 = (       0 , ((struct x_Sort10_mirRecordCom *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_388;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirUnionCom ) {
                                        v_389 = (LIST_mirObject)  (     0 , ((struct x_Sort10_mirUnionCom *)((  p_17  ) ))->f_Part) ;
                                        v_390 = (       0 , ((struct x_Sort10_mirUnionCom *)((  p_17  ) ))->f_Value) ;
                                        v_391 = (       0 , ((struct x_Sort10_mirUnionCom *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_391;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirTypeSize ) {
                                        v_392 = (       0 , ((struct x_Sort10_mirTypeSize *)((  p_17  ) ))->f_OfType) ;
                                        v_393 = (       0 , ((struct x_Sort10_mirTypeSize *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_393;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirObjectAddr ) {
                                        v_394 = (       0 , ((struct x_Sort10_mirObjectAddr *)((  p_17  ) ))->f_Obj) ;
                                        v_395 = (       0 , ((struct x_Sort10_mirObjectAddr *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_395;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirContent ) {
                                        v_396 = (       0 , ((struct x_Sort10_mirContent *)((  p_17  ) ))->f_Addr) ;
                                        v_397 = (       0 , ((struct x_Sort10_mirContent *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_397;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirSubscript ) {
                                        v_398 = (       0 , ((struct x_Sort10_mirSubscript *)((  p_17  ) ))->f_Index) ;
                                        v_399 = (       0 , ((struct x_Sort10_mirSubscript *)((  p_17  ) ))->f_Base) ;
                                        v_400 = (       0 , ((struct x_Sort10_mirSubscript *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_400;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirMember ) {
                                        v_401 =  (LIST_mirObject)(      0 , ((struct x_Sort10_mirMember *)((  p_17  ) ))->f_Field) ;
                                        v_402 = (       0 , ((struct x_Sort10_mirMember *)((  p_17  ) ))->f_Base) ;
                                        v_403 = (       0 , ((struct x_Sort10_mirMember *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_403;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirBitExtract ) {
                                        v_404 = (       0 , ((struct x_Sort10_mirBitExtract *)((  p_17  ) ))->f_Width) ;
                                        v_405 = (       0 , ((struct x_Sort10_mirBitExtract *)((  p_17  ) ))->f_Start) ;
                                        v_406 = (       0 , ((struct x_Sort10_mirBitExtract *)((  p_17  ) ))->f_Src) ;
                                        v_407 = (       0 , ((struct x_Sort10_mirBitExtract *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_407;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirBitInsert ) {
                                        v_408 = (       0 , ((struct x_Sort10_mirBitInsert *)((  p_17  ) ))->f_Src) ;
                                        v_409 = (       0 , ((struct x_Sort10_mirBitInsert *)((  p_17  ) ))->f_Width) ;
                                        v_410 = (       0 , ((struct x_Sort10_mirBitInsert *)((  p_17  ) ))->f_Start) ;
                                        v_411 = (       0 , ((struct x_Sort10_mirBitInsert *)((  p_17  ) ))->f_Dest) ;
                                        v_412 = (       0 , ((struct x_Sort10_mirBitInsert *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_412;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirBoolVal ) {
                                        v_413 = (       0 , ((struct x_Sort10_mirBoolVal *)((  p_17  ) ))->f_False) ;
                                        v_414 = (       0 , ((struct x_Sort10_mirBoolVal *)((  p_17  ) ))->f_True) ;
                                        v_415 = (       0 , ((struct x_Sort10_mirBoolVal *)((  p_17  ) ))->f_Value) ;
                                        v_416 = (       0 , ((struct x_Sort10_mirBoolVal *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_416;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirConvert ) {
                                        v_417 = (       0 , ((struct x_Sort10_mirConvert *)((  p_17  ) ))->f_Rounding) ;
                                        v_418 = (       0 , ((struct x_Sort10_mirConvert *)((  p_17  ) ))->f_Value) ;
                                        v_419 = (       0 , ((struct x_Sort10_mirConvert *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_419;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirCast ) {
                                        v_420 = (       0 , ((struct x_Sort10_mirCast *)((  p_17  ) ))->f_Value) ;
                                        v_421 = (       0 , ((struct x_Sort10_mirCast *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_421;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirCheckReal ) {
                                        v_422 = (       0 , ((struct x_Sort10_mirCheckReal *)((  p_17  ) ))->f_Value) ;
                                        v_423 = (       0 , ((struct x_Sort10_mirCheckReal *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_423;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirCheckRange ) {
                                        v_424 = (       0 , ((struct x_Sort10_mirCheckRange *)((  p_17  ) ))->f_Value) ;
                                        v_425 = (       0 , ((struct x_Sort10_mirCheckRange *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_425;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirAbs ) {
                                        v_426 = (       0 , ((struct x_Sort10_mirAbs *)((  p_17  ) ))->f_Value) ;
                                        v_427 = (       0 , ((struct x_Sort10_mirAbs *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_427;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirNeg ) {
                                        v_428 = (       0 , ((struct x_Sort10_mirNeg *)((  p_17  ) ))->f_Value) ;
                                        v_429 = (       0 , ((struct x_Sort10_mirNeg *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_429;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirNot ) {
                                        v_430 = (       0 , ((struct x_Sort10_mirNot *)((  p_17  ) ))->f_Value) ;
                                        v_431 = (       0 , ((struct x_Sort10_mirNot *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_431;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirAddrPlus ) {
                                        v_432 = (       0 , ((struct x_Sort10_mirAddrPlus *)((  p_17  ) ))->f_Strict) ;
                                        v_433 = (       0 , ((struct x_Sort10_mirAddrPlus *)((  p_17  ) ))->f_Right) ;
                                        v_434 = (       0 , ((struct x_Sort10_mirAddrPlus *)((  p_17  ) ))->f_Left) ;
                                        v_435 = (       0 , ((struct x_Sort10_mirAddrPlus *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_435;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirAddrDiff ) {
                                        v_436 = (       0 , ((struct x_Sort10_mirAddrDiff *)((  p_17  ) ))->f_Strict) ;
                                        v_437 = (       0 , ((struct x_Sort10_mirAddrDiff *)((  p_17  ) ))->f_Right) ;
                                        v_438 = (       0 , ((struct x_Sort10_mirAddrDiff *)((  p_17  ) ))->f_Left) ;
                                        v_439 = (       0 , ((struct x_Sort10_mirAddrDiff *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_439;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirCompare ) {
                                        v_440 = (       0 , ((struct x_Sort10_mirCompare *)((  p_17  ) ))->f_Relation) ;
                                        v_441 = (       0 , ((struct x_Sort10_mirCompare *)((  p_17  ) ))->f_Strict) ;
                                        v_442 = (       0 , ((struct x_Sort10_mirCompare *)((  p_17  ) ))->f_Right) ;
                                        v_443 = (       0 , ((struct x_Sort10_mirCompare *)((  p_17  ) ))->f_Left) ;
                                        v_444 = (       0 , ((struct x_Sort10_mirCompare *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_444;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirPlus ) {
                                        v_445 = (       0 , ((struct x_Sort10_mirPlus *)((  p_17  ) ))->f_Strict) ;
                                        v_446 = (       0 , ((struct x_Sort10_mirPlus *)((  p_17  ) ))->f_Right) ;
                                        v_447 = (       0 , ((struct x_Sort10_mirPlus *)((  p_17  ) ))->f_Left) ;
                                        v_448 = (       0 , ((struct x_Sort10_mirPlus *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_448;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirDiff ) {
                                        v_449 = (       0 , ((struct x_Sort10_mirDiff *)((  p_17  ) ))->f_Strict) ;
                                        v_450 = (       0 , ((struct x_Sort10_mirDiff *)((  p_17  ) ))->f_Right) ;
                                        v_451 = (       0 , ((struct x_Sort10_mirDiff *)((  p_17  ) ))->f_Left) ;
                                        v_452 = (       0 , ((struct x_Sort10_mirDiff *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_452;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirMult ) {
                                        v_453 = (       0 , ((struct x_Sort10_mirMult *)((  p_17  ) ))->f_Strict) ;
                                        v_454 = (       0 , ((struct x_Sort10_mirMult *)((  p_17  ) ))->f_Right) ;
                                        v_455 = (       0 , ((struct x_Sort10_mirMult *)((  p_17  ) ))->f_Left) ;
                                        v_456 = (       0 , ((struct x_Sort10_mirMult *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_456;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirShiftLeft ) {
                                        v_457 = (       0 , ((struct x_Sort10_mirShiftLeft *)((  p_17  ) ))->f_Strict) ;
                                        v_458 = (       0 , ((struct x_Sort10_mirShiftLeft *)((  p_17  ) ))->f_Right) ;
                                        v_459 = (       0 , ((struct x_Sort10_mirShiftLeft *)((  p_17  ) ))->f_Left) ;
                                        v_460 = (       0 , ((struct x_Sort10_mirShiftLeft *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_460;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirShiftRight ) {
                                        v_461 = (       0 , ((struct x_Sort10_mirShiftRight *)((  p_17  ) ))->f_Strict) ;
                                        v_462 = (       0 , ((struct x_Sort10_mirShiftRight *)((  p_17  ) ))->f_Right) ;
                                        v_463 = (       0 , ((struct x_Sort10_mirShiftRight *)((  p_17  ) ))->f_Left) ;
                                        v_464 = (       0 , ((struct x_Sort10_mirShiftRight *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_464;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirShiftRightSign ) {
                                        v_465 = (       0 , ((struct x_Sort10_mirShiftRightSign *)((  p_17  ) ))->f_Strict) ;
                                        v_466 = (       0 , ((struct x_Sort10_mirShiftRightSign *)((  p_17  ) ))->f_Right) ;
                                        v_467 = (       0 , ((struct x_Sort10_mirShiftRightSign *)((  p_17  ) ))->f_Left) ;
                                        v_468 = (       0 , ((struct x_Sort10_mirShiftRightSign *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_468;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirAnd ) {
                                        v_469 = (       0 , ((struct x_Sort10_mirAnd *)((  p_17  ) ))->f_Strict) ;
                                        v_470 = (       0 , ((struct x_Sort10_mirAnd *)((  p_17  ) ))->f_Right) ;
                                        v_471 = (       0 , ((struct x_Sort10_mirAnd *)((  p_17  ) ))->f_Left) ;
                                        v_472 = (       0 , ((struct x_Sort10_mirAnd *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_472;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirOr ) {
                                        v_473 = (       0 , ((struct x_Sort10_mirOr *)((  p_17  ) ))->f_Strict) ;
                                        v_474 = (       0 , ((struct x_Sort10_mirOr *)((  p_17  ) ))->f_Right) ;
                                        v_475 = (       0 , ((struct x_Sort10_mirOr *)((  p_17  ) ))->f_Left) ;
                                        v_476 = (       0 , ((struct x_Sort10_mirOr *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_476;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirXor ) {
                                        v_477 = (       0 , ((struct x_Sort10_mirXor *)((  p_17  ) ))->f_Strict) ;
                                        v_478 = (       0 , ((struct x_Sort10_mirXor *)((  p_17  ) ))->f_Right) ;
                                        v_479 = (       0 , ((struct x_Sort10_mirXor *)((  p_17  ) ))->f_Left) ;
                                        v_480 = (       0 , ((struct x_Sort10_mirXor *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_480;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirDiv ) {
                                        v_481 = (       0 , ((struct x_Sort10_mirDiv *)((  p_17  ) ))->f_Strict) ;
                                        v_482 = (       0 , ((struct x_Sort10_mirDiv *)((  p_17  ) ))->f_Right) ;
                                        v_483 = (       0 , ((struct x_Sort10_mirDiv *)((  p_17  ) ))->f_Left) ;
                                        v_484 = (       0 , ((struct x_Sort10_mirDiv *)((  p_17  ) ))->f_OnZero) ;
                                        v_485 = (       0 , ((struct x_Sort10_mirDiv *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_485;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirQuo ) {
                                        v_486 = (       0 , ((struct x_Sort10_mirQuo *)((  p_17  ) ))->f_Strict) ;
                                        v_487 = (       0 , ((struct x_Sort10_mirQuo *)((  p_17  ) ))->f_Right) ;
                                        v_488 = (       0 , ((struct x_Sort10_mirQuo *)((  p_17  ) ))->f_Left) ;
                                        v_489 = (       0 , ((struct x_Sort10_mirQuo *)((  p_17  ) ))->f_OnZero) ;
                                        v_490 = (       0 , ((struct x_Sort10_mirQuo *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_490;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirMod ) {
                                        v_491 = (       0 , ((struct x_Sort10_mirMod *)((  p_17  ) ))->f_Strict) ;
                                        v_492 = (       0 , ((struct x_Sort10_mirMod *)((  p_17  ) ))->f_Right) ;
                                        v_493 = (       0 , ((struct x_Sort10_mirMod *)((  p_17  ) ))->f_Left) ;
                                        v_494 = (       0 , ((struct x_Sort10_mirMod *)((  p_17  ) ))->f_OnZero) ;
                                        v_495 = (       0 , ((struct x_Sort10_mirMod *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_495;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_17   ) )->op) )  == xop_Sort10_mirRem ) {
                                        v_496 = (       0 , ((struct x_Sort10_mirRem *)((  p_17  ) ))->f_Strict) ;
                                        v_497 = (       0 , ((struct x_Sort10_mirRem *)((  p_17  ) ))->f_Right) ;
                                        v_498 = (       0 , ((struct x_Sort10_mirRem *)((  p_17  ) ))->f_Left) ;
                                        v_499 = (       0 , ((struct x_Sort10_mirRem *)((  p_17  ) ))->f_OnZero) ;
                                        v_500 = (       0 , ((struct x_Sort10_mirRem *)((  p_17  ) ))->f_Type) ;
                                        if (1) {         
                                                v_375 = v_500;
                                        } else {;        
                                                goto l_76;
                                        }        
                                } else   
                        l_76:
                                {        
                                        ;
                                        ein_neur_name= fatal_error__("1.c",8172)+fatal_error_ ("No pattern matched in case Expression (195,2-235,9)");
                                }
                        }
                } else {;        
                        ein_neur_name= fatal_error__("1.c",8176)+fatal_error_ ("No pattern matched in function o_type");
                }
                if (o_type_active) {
                        z = (o_type_MEMOP) malloc(sizeof(o_type_MEMO));
                        if (z == ((void *)0) ) {
                                ein_neur_name= fatal_error__("1.c",8181)+fatal_error_ ("No mem");
                        }
                        z->_1 = p_17;
                        z->res = v_375;
                        z->next = o_type_memotable[h];
                        o_type_memotable[h] = z;
                }
                return (v_375);
        }
}
void 
o_type_stat(void)
{
        int             o_type_anz = 0, o_type_max = 0;
        double          o_type_durch = 0;
        int             s, leer = 0;
        int             j, i;
        o_type_MEMOP    z;
        if (o_type_active && o_type_initialized) {
                for (i = 0; i < o_type_size; i++) {
                        for (j = 0, z = o_type_memotable[i]; z != ((void *)0) ; j++, z = z->next) {
                                o_type_anz += 1;
                        }
                        leer += j == 0 ? 1 : 0;
                        o_type_max = o_type_max < j ? j : o_type_max;
                }
                o_type_durch = (double) o_type_anz / (double) (o_type_size - leer);
        }
        if (o_type_active) {
                s = o_type_size;
                fprintf((&_iob[2]) , "o_type:\n");
                fprintf((&_iob[2]) , " size=%d\n", s);
                fprintf((&_iob[2]) , " anz=%d\n", o_type_anz);
                fprintf((&_iob[2]) , " max=%d\n", o_type_max);
                fprintf((&_iob[2]) , " durchschnitt=%f\n", o_type_durch);
                fprintf((&_iob[2]) , " cache hit=%d\n", o_type_chit);
                fprintf((&_iob[2]) , " leere Eintraege=%d\n", leer);
                fprintf((&_iob[2]) , " hit=%d\n", o_type_hit);
                fprintf((&_iob[2]) , " Guete=%f\n", (double) o_type_hit / (double) o_type_anz);
                fprintf((&_iob[2]) , " cache Guete=%f\n", (double) o_type_chit / (double) o_type_anz);
                fprintf((&_iob[2]) , "\n");
        }
}
static          o_is_simple_type_chit = 0;
static          o_is_simple_type_hit = 0;
static int      o_is_simple_type_initialized = 0, o_is_simple_type_active = 1,
                o_is_simple_type_size = 97;
static o_is_simple_type_MEMOP *o_is_simple_type_memotable;
bool 
o_is_simple_type(mirTYPE p_18)
{
        unsigned int    h;
        o_is_simple_type_MEMOP z;
        int             w;
        if (o_is_simple_type_active) {
                if (!o_is_simple_type_initialized) {
                        o_is_simple_type_memotable = (o_is_simple_type_MEMOP *) calloc(o_is_simple_type_size, sizeof(o_is_simple_type_MEMOP));
                        if (o_is_simple_type_memotable != ((void *)0) )
                                o_is_simple_type_initialized = 1;
                        else {
                                o_is_simple_type_active = 0;
                                goto o_is_simple_type_main;
                        }
                }
                h = 0;
                h += 2 * ((unsigned int)( p_18 )) ;
                h %= o_is_simple_type_size;
                for (w = 0, z = o_is_simple_type_memotable[h]; z != ((void *)0) ; w++, z = z->next) {
                        if (
                            p_18 == z->_1) {
                                if (w == 0)
                                        o_is_simple_type_chit += 1;
                                o_is_simple_type_hit += 1;
                                return (z->res);
                        }
                }
        } {
                bool            v_501;
                BOOL            v_502;
                int             v_503;
o_is_simple_type_main:
                if (1) {         
                        {
                                 
                                 
                                if (((((DMCP_xnode)(   p_18   ) )->op) )  == xop_Sort10_mirBoolean ) {
                                        if (1) {         
                                                v_501 = 1;
                                        } else {;        
                                                goto l_77;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_18   ) )->op) )  == xop_Sort10_mirInteger ) {
                                        v_502 = (       0 , ((struct x_Sort10_mirInteger *)((  p_18  ) ))->f_Signed) ;
                                        if (1) {         
                                                v_501 = 1;
                                        } else {;        
                                                goto l_77;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_18   ) )->op) )  == xop_Sort10_mirReal ) {
                                        if (1) {         
                                                v_501 = 1;
                                        } else {;        
                                                goto l_77;
                                        }        
                                } else   
                        l_77:
                                {        
                                        ;
                                        if (1) {         
                                                v_501 = 0;
                                        } else {;        
                                                v_503 = 1;       
                                        }
                                }
                                v_503 = 0;       
                                if (v_503) {;    
                                        ein_neur_name= fatal_error__("1.c",8334)+fatal_error_ ("No pattern matched in case Expression (185,2-190,9)");
                                }
                        }
                } else {;        
                        ein_neur_name= fatal_error__("1.c",8338)+fatal_error_ ("No pattern matched in function o_is_simple_type");
                }
                if (o_is_simple_type_active) {
                        z = (o_is_simple_type_MEMOP) malloc(sizeof(o_is_simple_type_MEMO));
                        if (z == ((void *)0) ) {
                                ein_neur_name= fatal_error__("1.c",8343)+fatal_error_ ("No mem");
                        }
                        z->_1 = p_18;
                        z->res = v_501;
                        z->next = o_is_simple_type_memotable[h];
                        o_is_simple_type_memotable[h] = z;
                }
                return (v_501);
        }
}
void 
o_is_simple_type_stat(void)
{
        int             o_is_simple_type_anz = 0, o_is_simple_type_max = 0;
        double          o_is_simple_type_durch = 0;
        int             s, leer = 0;
        int             j, i;
        o_is_simple_type_MEMOP z;
        if (o_is_simple_type_active && o_is_simple_type_initialized) {
                for (i = 0; i < o_is_simple_type_size; i++) {
                        for (j = 0, z = o_is_simple_type_memotable[i]; z != ((void *)0) ; j++, z = z->next) {
                                o_is_simple_type_anz += 1;
                        }
                        leer += j == 0 ? 1 : 0;
                        o_is_simple_type_max = o_is_simple_type_max < j ? j : o_is_simple_type_max;
                }
                o_is_simple_type_durch = (double) o_is_simple_type_anz / (double) (o_is_simple_type_size - leer);
        }
        if (o_is_simple_type_active) {
                s = o_is_simple_type_size;
                fprintf((&_iob[2]) , "o_is_simple_type:\n");
                fprintf((&_iob[2]) , " size=%d\n", s);
                fprintf((&_iob[2]) , " anz=%d\n", o_is_simple_type_anz);
                fprintf((&_iob[2]) , " max=%d\n", o_is_simple_type_max);
                fprintf((&_iob[2]) , " durchschnitt=%f\n", o_is_simple_type_durch);
                fprintf((&_iob[2]) , " cache hit=%d\n", o_is_simple_type_chit);
                fprintf((&_iob[2]) , " leere Eintraege=%d\n", leer);
                fprintf((&_iob[2]) , " hit=%d\n", o_is_simple_type_hit);
                fprintf((&_iob[2]) , " Guete=%f\n", (double) o_is_simple_type_hit / (double) o_is_simple_type_anz);
                fprintf((&_iob[2]) , " cache Guete=%f\n", (double) o_is_simple_type_chit / (double) o_is_simple_type_anz);
                fprintf((&_iob[2]) , "\n");
        }
}
static          o_putin_chit = 0;
static          o_putin_hit = 0;
static int      o_putin_initialized = 0, o_putin_active = 1, o_putin_size = 499;
static o_putin_MEMOP *o_putin_memotable;
o_alias 
o_putin(LIST_mirObject p_19, o_alias p_20, o_alias p_21, unum p_22)
{
        unsigned int    h;
        o_putin_MEMOP   z;
        int             w;
        if (o_putin_active) {
                if (!o_putin_initialized) {
                        o_putin_memotable = (o_putin_MEMOP *) calloc(o_putin_size, sizeof(o_putin_MEMOP));
                        if (o_putin_memotable != ((void *)0) )
                                o_putin_initialized = 1;
                        else {
                                o_putin_active = 0;
                                goto o_putin_main;
                        }
                }
                h = 0;
                h += 2 * ((unsigned int)( p_19 )) ;
                h += 3 * o_alias_hash(p_20);
                h += 5 * o_alias_hash(p_21);
                h += 7 * unum_hash(p_22);
                h %= o_putin_size;
                for (w = 0, z = o_putin_memotable[h]; z != ((void *)0) ; w++, z = z->next) {
                        if (
                            p_19 == z->_1 &&
                            o_alias_eq(p_20, z->_2) &&
                            o_alias_eq(p_21, z->_3) &&
                            unum_eq(p_22, z->_4)) {
                                if (w == 0)
                                        o_putin_chit += 1;
                                o_putin_hit += 1;
                                return (z->res);
                        }
                }
        } {
                o_alias         v_504;
                LIST_mirObject  v_505;
                mirObject       v_506;
                o_alias         v_507;
                mirParamKIND    v_508;
                mirTYPE         v_509;
                BOOL            v_510;
                mirTYPE         v_511;
                INT             v_512;
                int             v_513;
o_putin_main:
                 
                if (((BOOL) ((   (      0 , ((struct x_Sort58_LIST_Const_List *)((   p_19   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        if (1) { 
                                v_504 = p_20;
                        } else {;        
                                goto l_78;
                        }        
                } else
                   if (! ((BOOL) ((   ( 0 , ((struct x_Sort58_LIST_Const_List *)((   p_19   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        v_505 = LIST_mirObject_tl(p_19);
                        v_506 = LIST_mirObject_hd(p_19);
                        if (1) { 
                                {
                                        v_507 = o_putin(v_505, p_20, p_21, unum_add(p_22, unum_create("1")));
                                        {
                                                 
                                                 
                                                if (((((DMCP_xnode)(   v_506   ) )->op) )  == xop_Sort10_mirParameter ) {
                                                        v_508 = (       0 , ((struct x_Sort10_mirParameter *)((  v_506  ) ))->f_ParamKind) ;
                                                        v_509 = (       0 , ((struct x_Sort10_mirParameter *)((  v_506  ) ))->f_Procedure) ;
                                                        v_510 = (       0 , ((struct x_Sort10_mirParameter *)((  v_506  ) ))->f_IsVolatile) ;
                                                        v_511 = (       0 , ((struct x_Sort10_mirParameter *)((  v_506  ) ))->f_Type) ;
                                                        v_512 = (       0 , ((struct x_Sort10_mirParameter *)((  v_506  ) ))->f_cpid) ;
                                                        if (1) {         
                                                                v_504 = o_alias_update(v_507, o_myid(v_512), o_alias_get(p_21, p_22));
                                                        } else {;        
                                                                goto l_79;
                                                        }        
                                                } else   
                                        l_79:
                                                {        
                                                        ;
                                                        if (1) {         
                                                                v_504 = (o_alias) fatal_error_ (str_create("unexpected Parameter Type"));
                                                        } else {;        
                                                                v_513 = 1;       
                                                        }
                                                }
                                                v_513 = 0;       
                                                if (v_513) {;    
                                                        ein_neur_name= fatal_error__("1.c",8512)+fatal_error_ ("No pattern matched in case Expression (177,9-180,16)");
                                                }
                                        }
                                }
                        } else {;        
                                goto l_78;
                        }        
                } else           
        l_78:
                {                
                        ;
                        ein_neur_name= fatal_error__("1.c",8524)+fatal_error_ ("No pattern matched in function o_putin");
                }
                if (o_putin_active) {
                        z = (o_putin_MEMOP) malloc(sizeof(o_putin_MEMO));
                        if (z == ((void *)0) ) {
                                ein_neur_name= fatal_error__("1.c",8529)+fatal_error_ ("No mem");
                        }
                        z->_1 = p_19;
                        z->_2 = p_20;
                        z->_3 = p_21;
                        z->_4 = p_22;
                        z->res = v_504;
                        z->next = o_putin_memotable[h];
                        o_putin_memotable[h] = z;
                }
                return (v_504);
        }
}
void 
o_putin_stat(void)
{
        int             o_putin_anz = 0, o_putin_max = 0;
        double          o_putin_durch = 0;
        int             s, leer = 0;
        int             j, i;
        o_putin_MEMOP   z;
        if (o_putin_active && o_putin_initialized) {
                for (i = 0; i < o_putin_size; i++) {
                        for (j = 0, z = o_putin_memotable[i]; z != ((void *)0) ; j++, z = z->next) {
                                o_putin_anz += 1;
                        }
                        leer += j == 0 ? 1 : 0;
                        o_putin_max = o_putin_max < j ? j : o_putin_max;
                }
                o_putin_durch = (double) o_putin_anz / (double) (o_putin_size - leer);
        }
        if (o_putin_active) {
                s = o_putin_size;
                fprintf((&_iob[2]) , "o_putin:\n");
                fprintf((&_iob[2]) , " size=%d\n", s);
                fprintf((&_iob[2]) , " anz=%d\n", o_putin_anz);
                fprintf((&_iob[2]) , " max=%d\n", o_putin_max);
                fprintf((&_iob[2]) , " durchschnitt=%f\n", o_putin_durch);
                fprintf((&_iob[2]) , " cache hit=%d\n", o_putin_chit);
                fprintf((&_iob[2]) , " leere Eintraege=%d\n", leer);
                fprintf((&_iob[2]) , " hit=%d\n", o_putin_hit);
                fprintf((&_iob[2]) , " Guete=%f\n", (double) o_putin_hit / (double) o_putin_anz);
                fprintf((&_iob[2]) , " cache Guete=%f\n", (double) o_putin_chit / (double) o_putin_anz);
                fprintf((&_iob[2]) , "\n");
        }
}
o_const_fkt 
o_fill_pars(o_const_fkt p_23, LIST_mirObject p_24, o_const_fkt p_25, unum p_26)
{
        {
                o_const_fkt     v_514;
                LIST_mirObject  v_515;
                mirObject       v_516;
                o_const_fkt     v_517;
                mirParamKIND    v_518;
                mirTYPE         v_519;
                BOOL            v_520;
                mirTYPE         v_521;
                INT             v_522;
                int             v_523;
                 
                if (((BOOL) ((   (      0 , ((struct x_Sort58_LIST_Const_List *)((   p_24   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        if (1) { 
                                v_514 = p_23;
                        } else {;        
                                goto l_80;
                        }        
                } else
                   if (! ((BOOL) ((   ( 0 , ((struct x_Sort58_LIST_Const_List *)((   p_24   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        v_515 = LIST_mirObject_tl(p_24);
                        v_516 = LIST_mirObject_hd(p_24);
                        if (1) { 
                                {
                                        v_517 = o_fill_pars(p_23, v_515, p_25, unum_add(p_26, unum_create("1")));
                                        {
                                                 
                                                 
                                                if (((((DMCP_xnode)(   v_516   ) )->op) )  == xop_Sort10_mirParameter ) {
                                                        v_518 = (       0 , ((struct x_Sort10_mirParameter *)((  v_516  ) ))->f_ParamKind) ;
                                                        v_519 = (       0 , ((struct x_Sort10_mirParameter *)((  v_516  ) ))->f_Procedure) ;
                                                        v_520 = (       0 , ((struct x_Sort10_mirParameter *)((  v_516  ) ))->f_IsVolatile) ;
                                                        v_521 = (       0 , ((struct x_Sort10_mirParameter *)((  v_516  ) ))->f_Type) ;
                                                        v_522 = (       0 , ((struct x_Sort10_mirParameter *)((  v_516  ) ))->f_cpid) ;
                                                        if (1) {         
                                                                if (bool_to_c(bool_create(( v_520 ) )
                                                                          ))
                                                                        v_514 = (o_const_fkt) fatal_error_ (str_create("error in ccmir: Volatile Parameter!\n"));
                                                                else {
                                                                        if (1) {         
                                                                                v_514 = (bool_to_c(o_is_simple_type(v_521))) ? (o_const_fkt_update(v_517, o_myid(v_522), o_const_fkt_get(p_25, p_26))) : (v_517);
                                                                        } else {;        
                                                                                ein_neur_name= fatal_error__("1.c",8654)+fatal_error_ ("No pattern matched in case Expression (164,4-168,11)");
                                                                        }
                                                                }
                                                        } else {;        
                                                                goto l_81;
                                                        }        
                                                } else   
                                        l_81:
                                                {        
                                                        ;
                                                        if (1) {         
                                                                v_514 = (o_const_fkt) fatal_error_ (str_create("unexpected Parameter Type"));
                                                        } else {;        
                                                                v_523 = 1;       
                                                        }
                                                }
                                                v_523 = 0;       
                                                if (v_523) {;    
                                                        ein_neur_name= fatal_error__("1.c",8678)+fatal_error_ ("No pattern matched in case Expression (160,2-171,9)");
                                                }
                                        }
                                }
                        } else {;        
                                goto l_80;
                        }        
                } else           
        l_80:
                {                
                        ;
                        ein_neur_name= fatal_error__("1.c",8690)+fatal_error_ ("No pattern matched in function o_fill_pars");
                }
                return (v_514);
        }
}
update(int cand);
void 
o_fill_pars_stat(void)
{
}
o_const_fkt 
o_init_locals(o_const_fkt p_27, LIST_mirObject p_28)
{
        {
                o_const_fkt     v_524;
                LIST_mirObject  v_525;
                mirObject       v_526;
                o_const_fkt     v_527;
                mirObject       v_528;
                BOOL            v_529;
                mirTYPE         v_530;
                INT             v_531;
                int             v_532;
                 
                if (((BOOL) ((   (      0 , ((struct x_Sort58_LIST_Const_List *)((   p_28   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        if (1) { 
                                v_524 = p_27;
                        } else {;        
                                goto l_82;
                        }        
                } else
                   if (! ((BOOL) ((   ( 0 , ((struct x_Sort58_LIST_Const_List *)((   p_28   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        v_525 = LIST_mirObject_tl(p_28);
                        v_526 = LIST_mirObject_hd(p_28);
                        if (1) { 
                                {
                                        v_527 = o_init_locals(p_27, v_525);
                                        {
                                                 
                                                 
                                                if (((((DMCP_xnode)(   v_526   ) )->op) )  == xop_Sort10_mirLocal ) {
                                                        v_528 = (       0 , ((struct x_Sort10_mirLocal *)((  v_526  ) ))->f_Procedure) ;
                                                        v_529 = (       0 , ((struct x_Sort10_mirLocal *)((  v_526  ) ))->f_IsVolatile) ;
                                                        v_530 = (       0 , ((struct x_Sort10_mirLocal *)((  v_526  ) ))->f_Type) ;
                                                        v_531 = (       0 , ((struct x_Sort10_mirLocal *)((  v_526  ) ))->f_cpid) ;
                                                        if (1) {         
                                                                v_524 = (bool_to_c(bool_create(( v_529 ) )
                                                                                   )) ? ((o_const_fkt) fatal_error_ (str_create("error in ccmir: volatile local found!\n"))) : ((bool_to_c(o_is_simple_type(v_530))) ? (o_const_fkt_update(v_527, o_myid(v_531), o_uninit())) : (v_527)
                                                                        );
                                                        } else {;        
                                                                goto l_83;
                                                        }        
                                                } else   
                                        l_83:
                                                {        
                                                        ;
                                                        if (1) {         
                                                                v_524 = (o_const_fkt) fatal_error_ (str_create("error in ccmir: Local Variable not a mirLocal!\n"));
                                                        } else {;        
                                                                v_532 = 1;       
                                                        }
                                                }
                                                v_532 = 0;       
                                                if (v_532) {;    
                                                        ein_neur_name= fatal_error__("1.c",8776)+fatal_error_ ("No pattern matched in case Expression (146,2-153,9)");
                                                }
                                        }
                                }
                        } else {;        
                                goto l_82;
                        }        
                } else           
        l_82:
                {                
                        ;
                        ein_neur_name= fatal_error__("1.c",8788)+fatal_error_ ("No pattern matched in function o_init_locals");
                }
                return (v_524);
        }
}
void 
o_init_locals_stat(void)
{
}
o_alias 
o_eval_objs(LIST_mirEXPR p_29, o_alias p_30, unum p_31)
{
        {
                o_alias         v_533;
                LIST_mirEXPR    v_534;
                mirEXPR         v_535;
                o_alias         v_536;
                o_alias         v_537;
                unum            v_538;
                o_t1            v_539;
                mirObject       v_540;
                mirTYPE         v_541;
                BOOL            v_542;
                mirEXPR         v_543;
                mirSection      v_544;
                NAME            v_545;
                BOOL            v_546;
                mirTYPE         v_547;
                INT             v_548;
                int             v_549;
                 
                if (((BOOL) ((   (      0 , ((struct x_Sort58_LIST_Const_List *)((   p_29   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        if (1) { 
                                v_533 = o_alias_create(o_t1_top);
                        } else {;        
                                goto l_84;
                        }        
                } else
                   if (! ((BOOL) ((   ( 0 , ((struct x_Sort58_LIST_Const_List *)((   p_29   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        v_534 = LIST_mirEXPR_tl(p_29);
                        v_535 = LIST_mirEXPR_hd(p_29);
                        if (1) { 
                                {
                                        v_536 = o_eval_objs(v_534, p_30, unum_add(p_31, unum_create("1")));
                                        {
                                                v_537 = v_536;
                                                v_538 = p_31;
                                                {
                                                         
                                                         
                                                        if (((((DMCP_xnode)(   v_535   ) )->op) )  == xop_Sort10_mirObjectAddr ) {
                                                                v_540 = (       0 , ((struct x_Sort10_mirObjectAddr *)((  v_535  ) ))->f_Obj) ;
                                                                v_541 = (       0 , ((struct x_Sort10_mirObjectAddr *)((  v_535  ) ))->f_Type) ;
                                                                 
                                                                if (((((DMCP_xnode)(   v_540   ) )->op) )  == xop_Sort10_mirDataGlobal ) {
                                                                        v_542 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_540  ) ))->f_IsWriteOnce) ;
                                                                        v_543 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_540  ) ))->f_InitialValue) ;
                                                                        v_544 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_540  ) ))->f_Section) ;
                                                                        v_545 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_540  ) ))->f_Name) ;
                                                                        v_546 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_540  ) ))->f_IsVolatile) ;
                                                                        v_547 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_540  ) ))->f_Type) ;
                                                                        v_548 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_540  ) ))->f_cpid) ;
                                                                        if (1) {         
                                                                                v_539 = o_t1_lift(o_myid(v_548));
                                                                        } else {;        
                                                                                goto l_86;
                                                                        }        
                                                                } else   
                                                        l_86:
                                                                {        
                                                                        ;
                                                                        goto l_85;
                                                                }        
                                                        } else   
                                                l_85:
                                                        {        
                                                                ;
                                                                if (1) {         
                                                                        v_539 = o_t1_top;
                                                                } else {;        
                                                                        v_549 = 1;       
                                                                }
                                                        }
                                                        v_549 = 0;       
                                                        if (v_549) {;    
                                                                ein_neur_name= fatal_error__("1.c",8908)+fatal_error_ ("No pattern matched in case Expression (137,30-140,37)");
                                                        }
                                                }
                                                v_533 = o_alias_update(v_537, v_538, v_539);
                                        }
                                }
                        } else {;        
                                goto l_84;
                        }        
                } else           
        l_84:
                {                
                        ;
                        ein_neur_name= fatal_error__("1.c",8922)+fatal_error_ ("No pattern matched in function o_eval_objs");
                }
                return (v_533);
        }
}
void 
o_eval_objs_stat(void)
{
}
o_const_fkt 
o_eval_params(LIST_mirEXPR p_32, o_const_fkt p_33, unum p_34)
{
        {
                o_const_fkt     v_550;
                LIST_mirEXPR    v_551;
                mirEXPR         v_552;
                 
                if (((BOOL) ((   (      0 , ((struct x_Sort58_LIST_Const_List *)((   p_32   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        if (1) { 
                                v_550 = o_const_fkt_create(o_va_top);
                        } else {;        
                                goto l_87;
                        }        
                } else
                   if (! ((BOOL) ((   ( 0 , ((struct x_Sort58_LIST_Const_List *)((   p_32   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        v_551 = LIST_mirEXPR_tl(p_32);
                        v_552 = LIST_mirEXPR_hd(p_32);
                        if (1) { 
                                v_550 = o_const_fkt_update(o_eval_params(v_551, p_33, unum_add(p_34, unum_create("1"))), p_34, o_value(v_552, p_33, o_alias_top));
                        } else {;        
                                goto l_87;
                        }        
                } else           
        l_87:
                {                
                        ;
                        ein_neur_name= fatal_error__("1.c",8975)+fatal_error_ ("No pattern matched in function o_eval_params");
                }
                return (v_550);
        }
}
void 
o_eval_params_stat(void)
{
}
static          o_id_chit = 0;
static          o_id_hit = 0;
static int      o_id_initialized = 0, o_id_active = 1, o_id_size = 3989;
static o_id_MEMOP *o_id_memotable;
unum 
o_id(mirObject p_35)
{
        unsigned int    h;
        o_id_MEMOP      z;
        int             w;
        if (o_id_active) {
                if (!o_id_initialized) {
                        o_id_memotable = (o_id_MEMOP *) calloc(o_id_size, sizeof(o_id_MEMOP));
                        if (o_id_memotable != ((void *)0) )
                                o_id_initialized = 1;
                        else {
                                o_id_active = 0;
                                goto o_id_main;
                        }
                }
                h = 0;
                h += 2 * ((unsigned int)( p_35 )) ;
                h %= o_id_size;
                for (w = 0, z = o_id_memotable[h]; z != ((void *)0) ; w++, z = z->next) {
                        if (
                            p_35 == z->_1) {
                                if (w == 0)
                                        o_id_chit += 1;
                                o_id_hit += 1;
                                return (z->res);
                        }
                }
        } {
                unum            v_553;
o_id_main:
                if (1) {         
                        v_553 = o_numbool_select_1(o_get_objinfo(p_35));
                } else {;        
                        ein_neur_name= fatal_error__("1.c",9040)+fatal_error_ ("No pattern matched in function o_id");
                }
                if (o_id_active) {
                        z = (o_id_MEMOP) malloc(sizeof(o_id_MEMO));
                        if (z == ((void *)0) ) {
                                ein_neur_name= fatal_error__("1.c",9045)+fatal_error_ ("No mem");
                        }
                        z->_1 = p_35;
                        z->res = v_553;
                        z->next = o_id_memotable[h];
                        o_id_memotable[h] = z;
                }
                return (v_553);
        }
}
void 
o_id_stat(void)
{
        int             o_id_anz = 0, o_id_max = 0;
        double          o_id_durch = 0;
        int             s, leer = 0;
        int             j, i;
        o_id_MEMOP      z;
        if (o_id_active && o_id_initialized) {
                for (i = 0; i < o_id_size; i++) {
                        for (j = 0, z = o_id_memotable[i]; z != ((void *)0) ; j++, z = z->next) {
                                o_id_anz += 1;
                        }
                        leer += j == 0 ? 1 : 0;
                        o_id_max = o_id_max < j ? j : o_id_max;
                }
                o_id_durch = (double) o_id_anz / (double) (o_id_size - leer);
        }
        if (o_id_active) {
                s = o_id_size;
                fprintf((&_iob[2]) , "o_id:\n");
                fprintf((&_iob[2]) , " size=%d\n", s);
                fprintf((&_iob[2]) , " anz=%d\n", o_id_anz);
                fprintf((&_iob[2]) , " max=%d\n", o_id_max);
                fprintf((&_iob[2]) , " durchschnitt=%f\n", o_id_durch);
                fprintf((&_iob[2]) , " cache hit=%d\n", o_id_chit);
                fprintf((&_iob[2]) , " leere Eintraege=%d\n", leer);
                fprintf((&_iob[2]) , " hit=%d\n", o_id_hit);
                fprintf((&_iob[2]) , " Guete=%f\n", (double) o_id_hit / (double) o_id_anz);
                fprintf((&_iob[2]) , " cache Guete=%f\n", (double) o_id_chit / (double) o_id_anz);
                fprintf((&_iob[2]) , "\n");
        }
}
static          o_is_volatile_chit = 0;
static          o_is_volatile_hit = 0;
static int      o_is_volatile_initialized = 0, o_is_volatile_active = 1,
                o_is_volatile_size = 3989;
static o_is_volatile_MEMOP *o_is_volatile_memotable;
bool 
o_is_volatile(mirObject p_36)
{
        unsigned int    h;
        o_is_volatile_MEMOP z;
        int             w;
        if (o_is_volatile_active) {
                if (!o_is_volatile_initialized) {
                        o_is_volatile_memotable = (o_is_volatile_MEMOP *) calloc(o_is_volatile_size, sizeof(o_is_volatile_MEMOP));
                        if (o_is_volatile_memotable != ((void *)0) )
                                o_is_volatile_initialized = 1;
                        else {
                                o_is_volatile_active = 0;
                                goto o_is_volatile_main;
                        }
                }
                h = 0;
                h += 2 * ((unsigned int)( p_36 )) ;
                h %= o_is_volatile_size;
                for (w = 0, z = o_is_volatile_memotable[h]; z != ((void *)0) ; w++, z = z->next) {
                        if (
                            p_36 == z->_1) {
                                if (w == 0)
                                        o_is_volatile_chit += 1;
                                o_is_volatile_hit += 1;
                                return (z->res);
                        }
                }
        } {
                bool            v_554;
o_is_volatile_main:
                if (1) {         
                        v_554 = o_numbool_select_2(o_get_objinfo(p_36));
                } else {;        
                        ein_neur_name= fatal_error__("1.c",9156)+fatal_error_ ("No pattern matched in function o_is_volatile");
                }
                if (o_is_volatile_active) {
                        z = (o_is_volatile_MEMOP) malloc(sizeof(o_is_volatile_MEMO));
                        if (z == ((void *)0) ) {
                                ein_neur_name= fatal_error__("1.c",9161)+fatal_error_ ("No mem");
                        }
                        z->_1 = p_36;
                        z->res = v_554;
                        z->next = o_is_volatile_memotable[h];
                        o_is_volatile_memotable[h] = z;
                }
                return (v_554);
        }
}
void 
o_is_volatile_stat(void)
{
        int             o_is_volatile_anz = 0, o_is_volatile_max = 0;
        double          o_is_volatile_durch = 0;
        int             s, leer = 0;
        int             j, i;
        o_is_volatile_MEMOP z;
        if (o_is_volatile_active && o_is_volatile_initialized) {
                for (i = 0; i < o_is_volatile_size; i++) {
                        for (j = 0, z = o_is_volatile_memotable[i]; z != ((void *)0) ; j++, z = z->next) {
                                o_is_volatile_anz += 1;
                        }
                        leer += j == 0 ? 1 : 0;
                        o_is_volatile_max = o_is_volatile_max < j ? j : o_is_volatile_max;
                }
                o_is_volatile_durch = (double) o_is_volatile_anz / (double) (o_is_volatile_size - leer);
        }
        if (o_is_volatile_active) {
                s = o_is_volatile_size;
                fprintf((&_iob[2]) , "o_is_volatile:\n");
                fprintf((&_iob[2]) , " size=%d\n", s);
                fprintf((&_iob[2]) , " anz=%d\n", o_is_volatile_anz);
                fprintf((&_iob[2]) , " max=%d\n", o_is_volatile_max);
                fprintf((&_iob[2]) , " durchschnitt=%f\n", o_is_volatile_durch);
                fprintf((&_iob[2]) , " cache hit=%d\n", o_is_volatile_chit);
                fprintf((&_iob[2]) , " leere Eintraege=%d\n", leer);
                fprintf((&_iob[2]) , " hit=%d\n", o_is_volatile_hit);
                fprintf((&_iob[2]) , " Guete=%f\n", (double) o_is_volatile_hit / (double) o_is_volatile_anz);
                fprintf((&_iob[2]) , " cache Guete=%f\n", (double) o_is_volatile_chit / (double) o_is_volatile_anz);
                fprintf((&_iob[2]) , "\n");
        }
}
o_numbool 
o_get_objinfo(mirObject p_37)
{
        {
                o_numbool       v_555;
                BOOL            v_556;
                mirEXPR         v_557;
                mirSection      v_558;
                NAME            v_559;
                BOOL            v_560;
                mirTYPE         v_561;
                INT             v_562;
                mirObject       v_563;
                BOOL            v_564;
                mirTYPE         v_565;
                INT             v_566;
                mirParamKIND    v_567;
                mirTYPE         v_568;
                BOOL            v_569;
                mirTYPE         v_570;
                INT             v_571;
                INT             v_572;
                BOOL            v_573;
                mirTYPE         v_574;
                INT             v_575;
                int             v_576;
                if (1) {         
                        {
                                 
                                 
                                if (((((DMCP_xnode)(   p_37   ) )->op) )  == xop_Sort10_mirDataGlobal ) {
                                        v_556 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  p_37  ) ))->f_IsWriteOnce) ;
                                        v_557 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  p_37  ) ))->f_InitialValue) ;
                                        v_558 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  p_37  ) ))->f_Section) ;
                                        v_559 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  p_37  ) ))->f_Name) ;
                                        v_560 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  p_37  ) ))->f_IsVolatile) ;
                                        v_561 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  p_37  ) ))->f_Type) ;
                                        v_562 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  p_37  ) ))->f_cpid) ;
                                        if (1) {         
                                                v_555 = o_numbool_create(o_myid(v_562), bool_create(( v_560 ) )
                                                        );
                                        } else {;        
                                                goto l_88;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_37   ) )->op) )  == xop_Sort10_mirLocal ) {
                                        v_563 = (       0 , ((struct x_Sort10_mirLocal *)((  p_37  ) ))->f_Procedure) ;
                                        v_564 = (       0 , ((struct x_Sort10_mirLocal *)((  p_37  ) ))->f_IsVolatile) ;
                                        v_565 = (       0 , ((struct x_Sort10_mirLocal *)((  p_37  ) ))->f_Type) ;
                                        v_566 = (       0 , ((struct x_Sort10_mirLocal *)((  p_37  ) ))->f_cpid) ;
                                        if (1) {         
                                                v_555 = o_numbool_create(o_myid(v_566), bool_create(( v_564 ) )
                                                        );
                                        } else {;        
                                                goto l_88;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_37   ) )->op) )  == xop_Sort10_mirParameter ) {
                                        v_567 = (       0 , ((struct x_Sort10_mirParameter *)((  p_37  ) ))->f_ParamKind) ;
                                        v_568 = (       0 , ((struct x_Sort10_mirParameter *)((  p_37  ) ))->f_Procedure) ;
                                        v_569 = (       0 , ((struct x_Sort10_mirParameter *)((  p_37  ) ))->f_IsVolatile) ;
                                        v_570 = (       0 , ((struct x_Sort10_mirParameter *)((  p_37  ) ))->f_Type) ;
                                        v_571 = (       0 , ((struct x_Sort10_mirParameter *)((  p_37  ) ))->f_cpid) ;
                                        if (1) {         
                                                v_555 = o_numbool_create(o_myid(v_571), bool_create(( v_569 ) )
                                                        );
                                        } else {;        
                                                goto l_88;
                                        }        
                                } else
                                   if (((((DMCP_xnode)(   p_37   ) )->op) )  == xop_Sort10_mirRegister ) {
                                        v_572 = (       0 , ((struct x_Sort10_mirRegister *)((  p_37  ) ))->f_RegisterId) ;
                                        v_573 = (       0 , ((struct x_Sort10_mirRegister *)((  p_37  ) ))->f_IsVolatile) ;
                                        v_574 = (       0 , ((struct x_Sort10_mirRegister *)((  p_37  ) ))->f_Type) ;
                                        v_575 = (       0 , ((struct x_Sort10_mirRegister *)((  p_37  ) ))->f_cpid) ;
                                        if (1) {         
                                                v_555 = o_numbool_create(o_myid(v_575), bool_create(( v_573 ) )
                                                        );
                                        } else {;        
                                                goto l_88;
                                        }        
                                } else   
                        l_88:
                                {        
                                        ;
                                        if (1) {         
                                                v_555 = o_numbool_create(unum_create("0"), 1);
                                        } else {;        
                                                v_576 = 1;       
                                        }
                                }
                                v_576 = 0;       
                                if (v_576) {;    
                                        ein_neur_name= fatal_error__("1.c",9332)+fatal_error_ ("No pattern matched in case Expression (114,2-121,9)");
                                }
                        }
                } else {;        
                        ein_neur_name= fatal_error__("1.c",9336)+fatal_error_ ("No pattern matched in function o_get_objinfo");
                }
                return (v_555);
        }
}
void 
o_get_objinfo_stat(void)
{
}
o_const_fkt 
o_erase_var(o_const_fkt p_38, LIST_mirObject p_39)
{
        {
                o_const_fkt     v_577;
                LIST_mirObject  v_578;
                mirObject       v_579;
                o_const_fkt     v_580;
                mirObject       v_581;
                BOOL            v_582;
                mirTYPE         v_583;
                INT             v_584;
                mirParamKIND    v_585;
                mirTYPE         v_586;
                BOOL            v_587;
                mirTYPE         v_588;
                INT             v_589;
                int             v_590;
                 
                if (((BOOL) ((   (      0 , ((struct x_Sort58_LIST_Const_List *)((   p_39   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        if (1) { 
                                v_577 = p_38;
                        } else {;        
                                goto l_89;
                        }        
                } else
                   if (! ((BOOL) ((   ( 0 , ((struct x_Sort58_LIST_Const_List *)((   p_39   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        v_578 = LIST_mirObject_tl(p_39);
                        v_579 = LIST_mirObject_hd(p_39);
                        if (1) { 
                                {
                                        v_580 = o_erase_var(p_38, v_578);
                                        {
                                                 
                                                 
                                                if (((((DMCP_xnode)(   v_579   ) )->op) )  == xop_Sort10_mirLocal ) {
                                                        v_581 = (       0 , ((struct x_Sort10_mirLocal *)((  v_579  ) ))->f_Procedure) ;
                                                        v_582 = (       0 , ((struct x_Sort10_mirLocal *)((  v_579  ) ))->f_IsVolatile) ;
                                                        v_583 = (       0 , ((struct x_Sort10_mirLocal *)((  v_579  ) ))->f_Type) ;
                                                        v_584 = (       0 , ((struct x_Sort10_mirLocal *)((  v_579  ) ))->f_cpid) ;
                                                        if (1) {         
                                                                v_577 = o_const_fkt_update(v_580, o_myid(v_584), o_va_bottom);
                                                        } else {;        
                                                                goto l_90;
                                                        }        
                                                } else
                                                   if (((((DMCP_xnode)(   v_579   ) )->op) )  == xop_Sort10_mirParameter ) {
                                                        v_585 = (       0 , ((struct x_Sort10_mirParameter *)((  v_579  ) ))->f_ParamKind) ;
                                                        v_586 = (       0 , ((struct x_Sort10_mirParameter *)((  v_579  ) ))->f_Procedure) ;
                                                        v_587 = (       0 , ((struct x_Sort10_mirParameter *)((  v_579  ) ))->f_IsVolatile) ;
                                                        v_588 = (       0 , ((struct x_Sort10_mirParameter *)((  v_579  ) ))->f_Type) ;
                                                        v_589 = (       0 , ((struct x_Sort10_mirParameter *)((  v_579  ) ))->f_cpid) ;
                                                        if (1) {         
                                                                v_577 = o_const_fkt_update(v_580, o_myid(v_589), o_va_bottom);
                                                        } else {;        
                                                                goto l_90;
                                                        }        
                                                } else   
                                        l_90:
                                                {        
                                                        ;
                                                        if (1) {         
                                                                v_577 = v_580;
                                                        } else {;        
                                                                v_590 = 1;       
                                                        }
                                                }
                                                v_590 = 0;       
                                                if (v_590) {;    
                                                        ein_neur_name= fatal_error__("1.c",9438)+fatal_error_ ("No pattern matched in case Expression (105,2-109,9)");
                                                }
                                        }
                                }
                        } else {;        
                                goto l_89;
                        }        
                } else           
        l_89:
                {                
                        ;
                        ein_neur_name= fatal_error__("1.c",9450)+fatal_error_ ("No pattern matched in function o_erase_var");
                }
                return (v_577);
        }
}
void 
o_erase_var_stat(void)
{
}
o_ddfi 
o_comb(o_ddfi p_40, o_ddfi p_41)
{
        {
                o_ddfi          v_591;
                if (1) {         
                        v_591 = o_ddfi_lub(p_40, p_41);
                } else {;        
                        ein_neur_name= fatal_error__("1.c",9483)+fatal_error_ ("No pattern matched in function o_comb");
                }
                return (v_591);
        }
}
void 
o_comb_stat(void)
{
}
o_const_fkt 
o_built(o_const_fkt p_42, o_const_fkt p_43, LIST_mirObject p_44, LIST_mirObject p_45)
{
        {
                o_const_fkt     v_592;
                o_const_fkt     v_593;
                if (1) {         
                        {
                                v_593 = o_upd(p_42, p_43, p_44);
                                v_592 = o_upd(v_593, p_43, p_45);
                        }
                } else {;        
                        ein_neur_name= fatal_error__("1.c",9521)+fatal_error_ ("No pattern matched in function o_built");
                }
                return (v_592);
        }
}
void 
o_built_stat(void)
{
}
o_const_fkt 
o_upd(o_const_fkt p_46, o_const_fkt p_47, LIST_mirObject p_48)
{
        {
                o_const_fkt     v_594;
                LIST_mirObject  v_595;
                mirObject       v_596;
                o_const_fkt     v_597;
                mirObject       v_598;
                BOOL            v_599;
                mirTYPE         v_600;
                INT             v_601;
                mirParamKIND    v_602;
                mirTYPE         v_603;
                BOOL            v_604;
                mirTYPE         v_605;
                INT             v_606;
                int             v_607;
                 
                if (((BOOL) ((   (      0 , ((struct x_Sort58_LIST_Const_List *)((   p_48   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        if (1) { 
                                v_594 = p_46;
                        } else {;        
                                goto l_91;
                        }        
                } else
                   if (! ((BOOL) ((   ( 0 , ((struct x_Sort58_LIST_Const_List *)((   p_48   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        v_595 = LIST_mirObject_tl(p_48);
                        v_596 = LIST_mirObject_hd(p_48);
                        if (1) { 
                                {
                                        v_597 = o_upd(p_46, p_47, v_595);
                                        {
                                                 
                                                 
                                                if (((((DMCP_xnode)(   v_596   ) )->op) )  == xop_Sort10_mirLocal ) {
                                                        v_598 = (       0 , ((struct x_Sort10_mirLocal *)((  v_596  ) ))->f_Procedure) ;
                                                        v_599 = (       0 , ((struct x_Sort10_mirLocal *)((  v_596  ) ))->f_IsVolatile) ;
                                                        v_600 = (       0 , ((struct x_Sort10_mirLocal *)((  v_596  ) ))->f_Type) ;
                                                        v_601 = (       0 , ((struct x_Sort10_mirLocal *)((  v_596  ) ))->f_cpid) ;
                                                        if (1) {         
                                                                v_594 = o_const_fkt_update(v_597, o_myid(v_601), o_const_fkt_get(p_47, o_myid(v_601)));
                                                        } else {;        
                                                                goto l_92;
                                                        }        
                                                } else
                                                   if (((((DMCP_xnode)(   v_596   ) )->op) )  == xop_Sort10_mirParameter ) {
                                                        v_602 = (       0 , ((struct x_Sort10_mirParameter *)((  v_596  ) ))->f_ParamKind) ;
                                                        v_603 = (       0 , ((struct x_Sort10_mirParameter *)((  v_596  ) ))->f_Procedure) ;
                                                        v_604 = (       0 , ((struct x_Sort10_mirParameter *)((  v_596  ) ))->f_IsVolatile) ;
                                                        v_605 = (       0 , ((struct x_Sort10_mirParameter *)((  v_596  ) ))->f_Type) ;
                                                        v_606 = (       0 , ((struct x_Sort10_mirParameter *)((  v_596  ) ))->f_cpid) ;
                                                        if (1) {         
                                                                v_594 = o_const_fkt_update(v_597, o_myid(v_606), o_const_fkt_get(p_47, o_myid(v_606)));
                                                        } else {;        
                                                                goto l_92;
                                                        }        
                                                } else   
                                        l_92:
                                                {        
                                                        ;
                                                        if (1) {         
                                                                v_594 = v_597;
                                                        } else {;        
                                                                v_607 = 1;       
                                                        }
                                                }
                                                v_607 = 0;       
                                                if (v_607) {;    
                                                        ein_neur_name= fatal_error__("1.c",9623)+fatal_error_ ("No pattern matched in case Expression (90,9-94,16)");
                                                }
                                        }
                                }
                        } else {;        
                                goto l_91;
                        }        
                } else           
        l_91:
                {                
                        ;
                        ein_neur_name= fatal_error__("1.c",9635)+fatal_error_ ("No pattern matched in function o_upd");
                }
                return (v_594);
        }
}
void 
o_upd_stat(void)
{
}
o_const_fkt 
o_enrich(o_const_fkt p_49, LIST_mirEXPR p_50)
{
        {
                o_const_fkt     v_608;
                LIST_mirEXPR    v_609;
                mirEXPR         v_610;
                o_const_fkt     v_611;
                mirObject       v_612;
                mirTYPE         v_613;
                BOOL            v_614;
                mirEXPR         v_615;
                mirSection      v_616;
                NAME            v_617;
                BOOL            v_618;
                mirTYPE         v_619;
                INT             v_620;
                int             v_621;
                 
                if (((BOOL) ((   (      0 , ((struct x_Sort58_LIST_Const_List *)((   p_50   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        if (1) { 
                                v_608 = p_49;
                        } else {;        
                                goto l_93;
                        }        
                } else
                   if (! ((BOOL) ((   ( 0 , ((struct x_Sort58_LIST_Const_List *)((   p_50   ) ))->f_Elems)    )  == ( 0  ) )  ) ) {
                        v_609 = LIST_mirEXPR_tl(p_50);
                        v_610 = LIST_mirEXPR_hd(p_50);
                        if (1) { 
                                {
                                        v_611 = o_enrich(p_49, v_609);
                                        {
                                                 
                                                 
                                                if (((((DMCP_xnode)(   v_610   ) )->op) )  == xop_Sort10_mirObjectAddr ) {
                                                        v_612 = (       0 , ((struct x_Sort10_mirObjectAddr *)((  v_610  ) ))->f_Obj) ;
                                                        v_613 = (       0 , ((struct x_Sort10_mirObjectAddr *)((  v_610  ) ))->f_Type) ;
                                                         
                                                        if (((((DMCP_xnode)(   v_612   ) )->op) )  == xop_Sort10_mirDataGlobal ) {
                                                                v_614 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_612  ) ))->f_IsWriteOnce) ;
                                                                v_615 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_612  ) ))->f_InitialValue) ;
                                                                v_616 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_612  ) ))->f_Section) ;
                                                                v_617 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_612  ) ))->f_Name) ;
                                                                v_618 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_612  ) ))->f_IsVolatile) ;
                                                                v_619 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_612  ) ))->f_Type) ;
                                                                v_620 = (       0 , ((struct x_Sort10_mirDataGlobal *)((  v_612  ) ))->f_cpid) ;
                                                                if (1) {         
                                                                        v_608 = (bool_to_c(bool_create(( v_614 ) )
                                                                                           )) ? (o_const_fkt_update(v_611, o_myid(v_620), o_value(v_615, p_49, o_alias_top))) : (v_611);
                                                                } else {;        
                                                                        goto l_95;
                                                                }        
                                                        } else   
                                                l_95:
                                                        {        
                                                                ;
                                                                goto l_94;
                                                        }        
                                                } else   
                                        l_94:
                                                {        
                                                        ;
                                                        if (1) {         
                                                                v_608 = v_611;
                                                        } else {;        
                                                                v_621 = 1;       
                                                        }
                                                }
                                                v_621 = 0;       
                                                if (v_621) {;    
                                                        ein_neur_name= fatal_error__("1.c",9743)+fatal_error_ ("No pattern matched in case Expression (79,22-85,26)");
                                                }
                                        }
                                }
                        } else {;        
                                goto l_93;
                        }        
                } else           
        l_93:
                {                
                        ;
                        ein_neur_name= fatal_error__("1.c",9755)+fatal_error_ ("No pattern matched in function o_enrich");
                }
                return (v_608);
        }
}
void 
o_enrich_stat(void)
{
}
o_ddfi 
o_www(o_ddfi p_51, o_ddfi p_52)
{
        {
                o_ddfi          v_622;
                if (1) {         
                        v_622 = p_52;
                } else {;        
                        ein_neur_name= fatal_error__("1.c",9786)+fatal_error_ ("No pattern matched in function o_www");
                }
                return (v_622);
        }
}
void 
o_www_stat(void)
{
}
void 
all_stat(void)
{
        o_mycast_stat();
        o_print_type_stat();
        o_which_type_stat();
        o_print_const_val_type_stat();
        o_const_val_type_stat();
        o_myid_stat();
        o_value_stat();
        o_is_signed_stat();
        o_uninitfunc_stat();
        o_uninit_stat();
        o_type_stat();
        o_is_simple_type_stat();
        o_putin_stat();
        o_fill_pars_stat();
        o_init_locals_stat();
        o_eval_objs_stat();
        o_eval_params_stat();
        o_id_stat();
        o_is_volatile_stat();
        o_get_objinfo_stat();
        o_erase_var_stat();
        o_comb_stat();
        o_built_stat();
        o_upd_stat();
        o_enrich_stat();
        o_www_stat();
        transfer__stat();
}
int GC_init ( void );
int GC_newbank ( int );
int GC__registertype ( int, void (*f)(void *), char *, int );
void GC_mark ( void ** );
void *GC_alloc ( int );
void * GC_allocate ( int );
void GC_collect ( void );
void GC_print ( void );
double GC_avail ( void );
void GC_free ( void * );
void *gc_tmp ( unsigned int );
void *gc_perm(unsigned int i) ;
extern void __eprintf (const char *, const char *, unsigned, const char *);
struct  mallinfo  {
        int arena;       
        int ordblks;     
        int smblks;      
        int hblks;       
        int hblkhd;      
        int usmblks;     
        int fsmblks;     
        int uordblks;    
        int fordblks;    
        int keepcost;    
        int mxfast;      
        int nlblks;      
        int grain;       
        int uordbytes;   
        int allocated;   
        int treeoverhead;        
};
typedef void *  malloc_t;
extern  malloc_t        realloc  (void *, long unsigned int )  ;
extern  int             mallopt  (int, int)  ;
extern  struct mallinfo mallinfo();
char    *strcat  (char *, const char *)  ;
char    *strncat  (char *, const char *, long unsigned int )  ;
int     strcmp  (const char *, const char *)  ;
int     strncmp  (const char *, const char *, long unsigned int )  ;
int     strcasecmp();
char    *strcpy  (char *, const char *)  ;
char    *strncpy  (char *, const char *, long unsigned int )  ;
long unsigned int strlen  (const char *)  ;
char    *index  (const char *, int)  ;
char    *rindex  (const char *, int)  ;
typedef struct _types {
        int             typenumber;
        int             size;
        void            (*function) (void *);
        char           *file;
        int             line;
        int             calls;
}               types;
static types    typetable[5000 ];
static int      typepointer = 0;
static struct _iobuf     *file = ((void *)0) ;
typedef struct _memory {
        struct _memory *mem_next;
        int            *mem_start;
        int            *mem_end;
        int            *mem_middle;
        int             bank;
}               memory_struct, *pmemory_struct;
static int 
align(int x)
{
        int             eg = x;
        if (x % 8 )
                eg = x + (8  - (x % 8 ));
        return eg;
}
static pmemory_struct MEMORY = ((void *)0) ;
static pmemory_struct ROOT = ((void *)0) ;
static int      banks = 0;
int 
GC_init(void)
{
        ((void) (( MEMORY == ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   10058 ,  "MEMORY == NULL" ), 0) )) ;
        ((void) (( !(4000000  % 4) ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",          "1.c" ,   10059 ,  "!(GC_MEMSIZE % 4)" ), 0) )) ;
        ((void) (( !(4000000  % 8 ) ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   10060 ,  "!(GC_MEMSIZE % ALIGN)" ), 0) )) ;
        ((void) (( !(8  % 4) ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   10061 ,  "!(ALIGN % 4)" ), 0) )) ;
        MEMORY = (pmemory_struct) malloc( sizeof(memory_struct) ) ;
        if (MEMORY == ((void *)0) ) {
                fprintf((&_iob[2]) , "memory exhausted\n");
                exit(1);
        }
        (( MEMORY )->mem_next)  = ((void *)0) ;
        (( MEMORY )->mem_start)  = (int *) malloc( 4000000  ) ;
        if ((( MEMORY )->mem_start)  == ((void *)0) ) {
                fprintf((&_iob[2]) , "memory exhausted\n");
                exit(1);
        }
        (( MEMORY )->mem_start)  += 1;
        (( MEMORY )->mem_middle)  = (( MEMORY )->mem_start) ;
        (( MEMORY )->mem_end)  = (( MEMORY )->mem_start)  + (4000000  / 4 - 1);
        (( MEMORY )->mem_next)  = ((void *)0) ;
        (( MEMORY )->bank)  = 1;
        ROOT = MEMORY;
        typepointer = 0;
        banks = 1;
        return 1;
}
GC_finish(void) {
pmemory_struct help,help2;
help = ROOT;
while (help != ((void *)0) ) {
  help2 = (( help )->mem_next) ;
  free((void *) (char *) ((( help )->mem_start) -1) ) ;
  free((void *) (char *) help ) ;
  help = help2;
}
fprintf((&_iob[2]) ," done\n"); 
}
int 
GC_newbank(int x)
{
        pmemory_struct  help;
        while ((( MEMORY )->mem_next)  != ((void *)0) ) {
                MEMORY = (( MEMORY )->mem_next) ;
        }
        help = (pmemory_struct) malloc( sizeof(memory_struct) ) ;
        if ((help == ((void *)0) ) || (x > 4000000  / 2)) {
                fprintf((&_iob[2]) , "memory exhausted\n");
                exit(1);
        }
        (( MEMORY )->mem_next)  = help;
        MEMORY = help;
        (( MEMORY )->mem_next)  = ((void *)0) ;
        (( MEMORY )->bank)  = ++banks;
        (( MEMORY )->mem_start)  = (int *) malloc( 4000000  ) ;
        if ((( MEMORY )->mem_start)  == ((void *)0) ) {
                fprintf((&_iob[2]) , "memory exhausted\n");
                return 0;
        }
        (( MEMORY )->mem_start)  += 1;
        (( MEMORY )->mem_middle)  = (( MEMORY )->mem_start) ;
        (( MEMORY )->mem_end)  = (( MEMORY )->mem_start)  + (4000000  / 4 - 1);
        (( MEMORY )->mem_next)  = ((void *)0) ;
        return 1;
}
int             GC__registertype(int len, void (*f) (void *), char *my_str, int l) {
        int             erg = typepointer;
        typetable[((( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000 ) : (   typepointer   ) )  ) & 0x10000000 )  ? (( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) 
       ^ 0x40000000 ) : (   typepointer   ) )  ) ^ 0x10000000 ) : ( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000 ) : (   typepointer   ) )  ) )  ].typenumber  = typepointer;

        typetable[((( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000 ) : (   typepointer   ) )  ) & 0x10000000 )  ? (( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000
       ) : (   typepointer   ) )  ) ^ 0x10000000 ) : ( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000 ) : (   typepointer   ) )  ) )  ].size  = ((len + 4) % 8 ) ? len + 4 : (len + 8);

        typetable[((( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000 ) : (   typepointer   ) )  ) & 0x10000000 )  ? (( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000
       ) : (   typepointer   ) )  ) ^ 0x10000000 ) : ( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000 ) : (   typepointer   ) )  ) )  ].function  = f;

        typetable[((( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000 ) : (   typepointer   ) )  ) & 0x10000000 )  ? (( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000 )
     : (   typepointer   ) )  ) ^ 0x10000000 ) : ( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000 ) : (   typepointer   ) )  ) )  ].calls  = 0;

        typetable[((( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000 ) : (   typepointer   ) )  ) & 0x10000000 )  ? (( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000
 ) : (   typepointer   ) )  ) ^ 0x10000000 ) : ( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000 ) : (   typepointer   ) )  ) )  ].file  = (char *) strdup(my_str);

        typetable[((( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000 ) : (   typepointer   ) )  ) & 0x10000000 )  ? (( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000 ) : (   typepointer   ) )  ) ^ 0x10000000 ) : ( (((   typepointer   ) & 0x40000000 ) ? ((   typepointer   ) ^ 0x40000000 ) : (   typepointer   ) )  ) )  ].line  = l;

        typepointer++;
        if (typepointer == 5000 ) {
                fprintf((&_iob[2]) , "type register table exhausted\n");
                exit(0);
        }
        return erg;
}
void 
GC_mark(void **rr)
{
        int            *p, t;
        void            (*f) (void *);
        int             objtype;
        int           **r;
        r = (int **) rr;
        p = *r;
        if (((long)( p ) > 0L)  && ! ( p [-1L] & 0x10000000 ) ) {
                ((void) (( p[-1] & 0x40000000  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   10216 ,  "p[-1] & MARK2BIT" ), 0) )) ;
                objtype = p[-1] ^ 0x40000000 ;
                f = typetable[((( (((   objtype   ) & 0x40000000 ) ? ((   objtype   ) ^ 0x40000000 ) : (   objtype   ) )  ) & 0x10000000 )  ? (( (((   objtype   ) & 0x40000000 ) ? ((   objtype   ) ^ 0x40000000 ) : (   objtype   ) )  ) ^ 0x10000000 ) : ( (((   objtype   ) & 0x40000000 ) ? ((   objtype   ) ^ 0x40000000 ) : (   objtype   ) )  ) )  ].function ;
                ( p [-1L] = ( p [-1L] | 0x10000000 )) ;
                (*f) (p);
        } else {
        }
        ((void) (( p[-1] & 0x10000000  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   10229 ,  "p[-1] & MARKBIT" ), 0) )) ;
        t = p[-1] | 0x10000000 ;
        p[-1] = (int) ((int) r | 0x10000000 );
        *r = (int *) t;
}
static void 
Fixup(void)
{
        int            *p, *q, *Mid, t, tt;
        int             len = 0L;
        pmemory_struct  help,help2;
        help = ROOT;
help2 = ROOT;
q = (( help2 )->mem_start)  + 1;
        while (help != ((void *)0) ) {
                for (p = (( help )->mem_start)  + 1; p < (( help )->mem_middle) ;) {
                if ((t = p[-1L]) & 0x10000000 ) {
           while (!(t & 0x40000000 )) {
             tt = *(int   *)(t & ~0x10000000 );
             t = tt;
           }
         len = typetable[((( (((   ((( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) & 0x10000000 )  ? (( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) ^ 0x10000000 ) : 
            ( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) )     ) & 0x40000000 ) ? ((   ((( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) & 0x10000000 )  ? (( (((  t  ) &
        0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) ^ 0x10000000 ) : ( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) )     ) ^ 0x40000000 ) : (   ((( (((  t  ) & 0x40000000 ) ? 
          ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) & 0x10000000 )  ? (( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) ^ 0x10000000 ) : ( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  
            t  ) )  ) )     ) )  ) & 0x10000000 )  ? (( (((   ((( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) & 0x10000000 )  ? (( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  )
         )  ) ^ 0x10000000 ) : ( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) )     ) & 0x40000000 ) ? ((   ((( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) & 0x10000000
        )  ? (( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) ^ 0x10000000 ) : ( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) )     ) ^ 0x40000000 ) : (   ((( (((  t  ) &
      0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) & 0x10000000 )  ? (( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) ^ 0x10000000 ) : ( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000
     ) : (  t  ) )  ) )     ) )  ) ^ 0x10000000 ) : ( (((   ((( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) & 0x10000000 )  ? (( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) :
      (  t  ) )  ) ^ 0x10000000 ) : ( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) )     ) & 0x40000000 ) ? ((   ((( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) &
     0x10000000 )  ? (( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) ^ 0x10000000 ) : ( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) )     ) ^ 0x40000000 ) : (  
      ((( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) & 0x10000000 )  ? (( (((  t  ) & 0x40000000 ) ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) ^ 0x10000000 ) : ( (((  t  ) & 0x40000000 )
      ? ((  t  ) ^ 0x40000000 ) : (  t  ) )  ) )     ) )  ) )  ].size  / 4 + 1;
         if ((unsigned int) (q + len) >= (unsigned int) (( help2 )->mem_end) ) {
         ((void) (( (( help2 )->mem_next)  != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",          "1.c" ,   10281 ,  "MN(help2) != NULL" ), 0) )) ;
         help2 = (( help2 )->mem_next) ;
         q = (( help2 )->mem_start)  + 1;
         }
        t = p[-1L];
                                while (!(t & 0x40000000 )) {
                                        tt = *(int *) (t & ~0x10000000 );
                                        *(int **) (t & ~0x10000000 ) = q;        
                                        t = tt;
                                }
                                 
                                p[-1L] = t | (0x10000000  | 0x40000000 );
                        }
                        ((void) (( !(typetable[((( (((   p[-1]   ) & 0x40000000 ) ? ((   p[-1]   ) ^ 0x40000000 ) : (   p[-1]   ) )  ) & 0x10000000 )  ? (( (((   p[-1]   ) & 0x40000000 ) ? ((   p[-1]   ) ^ 0x40000000 ) : (   p[-1]   ) )  ) ^ 0x10000000 ) : ( (((   p[-1]   ) & 0x40000000 ) ? ((   p[-1]   ) ^ 0x40000000 ) : (   p[-1]   ) )  ) )  ].size  % 4) ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   10306 ,  "!(TS(p[-1]) % 4)" ), 0) )) ;
                        len = typetable[((( (((   p[-1]   ) & 0x40000000 ) ? ((   p[-1]   ) ^ 0x40000000 ) : (   p[-1]   ) )  ) & 0x10000000 )  ? (( (((   p[-1]   ) & 0x40000000 ) ? ((   p[-1]   ) ^ 0x40000000 ) : (   p[-1]   ) )  ) ^ 0x10000000 ) : ( (((   p[-1]   ) & 0x40000000 ) ? ((   p[-1]   ) ^ 0x40000000 ) : (   p[-1]   ) )  ) )  ].size  / 4 + 1;
                        if (( p [-1L] & 0x10000000 ) )
                                q += len;
                        p += len;
                }
                help = (( help )->mem_next) ;
        }
}
static void 
Move(void)
{
        int            *p, *q,*zs;
        int             len = 0L, i = 0L,fi;
        pmemory_struct  help,help2,help3;
        double          frac, z;
  help2 = ROOT;
  q = (( help2 )->mem_start)  + 1;
  help3 = ((void *)0) ;
        help = ROOT;
        while (help != ((void *)0) ) {
                for (p = (( help )->mem_start)  + 1; p < (( help )->mem_middle) ;) {
                        ((void) (( !(typetable[((( (((   p[-1]   ) & 0x40000000 ) ? ((   p[-1]   ) ^ 0x40000000 ) : (   p[-1]   ) )  ) & 0x10000000 )  ? (( (((   p[-1]   ) & 0x40000000 ) ? ((   p[-1]   ) ^ 0x40000000 ) : (   p[-1]   ) )  ) ^ 0x10000000 ) : ( (((   p[-1]   ) & 0x40000000 ) ? ((   p[-1]   ) ^ 0x40000000 ) : (   p[-1]   ) )  ) )  ].size  % 4) ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   10353 ,  "!(TS(p[-1]) % 4)" ), 0) )) ;
                        len = typetable[((( (((   p[-1]   ) & 0x40000000 ) ? ((   p[-1]   ) ^ 0x40000000 ) : (   p[-1]   ) )  ) & 0x10000000 )  ? (( (((   p[-1]   ) & 0x40000000 ) ? ((   p[-1]   ) ^ 0x40000000 ) : (   p[-1]   ) )  ) ^ 0x10000000 ) : ( (((   p[-1]   ) & 0x40000000 ) ? ((   p[-1]   ) ^ 0x40000000 ) : (   p[-1]   ) )  ) )  ].size  / 4 + 1;
                        if (( p [-1L] & 0x10000000 ) ) {
     if ((unsigned int) (q + len) >= (unsigned int) (( help2 )->mem_end) ) {
         help3 = help2;
         zs = q-1;
         help2 = (( help2 )->mem_next) ;
         q = (( help2 )->mem_start)  + 1;
     }
                                q[-1L] = p[-1L] ^ 0x10000000 ;
                                for (i = 1; i < len; i++)
                                        *q++ = *p++;
                                q += 1;
                                p += 1;
                                continue;
                        } else {
                        }
                        p += len;
                }
                if (help3 != ((void *)0) )   {
                     (( help3 )->mem_middle)  = zs;
                     help3 = ((void *)0) ;
                }
                frac = 100.0 * ((double) ((double) ( ((unsigned int) q - 1 ) - (unsigned int) (( 0 ==0) ? (( help2 )->mem_start)  : (( help )->mem_start) )) / (double) 4000000 ) ) ;
                help = (( help )->mem_next) ;
        }
if ((( help2 )->mem_middle)  != zs)
    (( help2 )->mem_middle)  = q-1;
help2 = (( help2 )->mem_next) ;
fi = 0;
while (help2 != ((void *)0) ) { fi++;
(( help2 )->mem_middle)  = (( help2 )->mem_start) ;
help2 = (( help2 )->mem_next) ;
}
}
void           *
GC_alloc(int type)
{
        int             size;
        int            *help;
        char           *x;
        int             hh;
        if (banks == 0) {
                fprintf(file, ":: garbage collector not initialized \n");
                fprintf((&_iob[2]) , ":: garbage collector not initialized \n");
                exit(1);
        }
        if ((type >= typepointer) || (type < 0)) {
                fprintf(file, ":: unknown type (not registered)  %d in GC_alloc \n", type);
                fprintf((&_iob[2]) , ":: unknown type (not registered)  %d in GC_alloc \n", type);
                exit(1);
        }
        size = typetable[((( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) & 0x10000000 )  ? (( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) ^ 0x10000000 ) : ( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) )  ].size ;
        typetable[((( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) & 0x10000000 )  ? (( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) ^ 0x10000000 ) : ( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) )  ].calls ++;
begin:
        if (((unsigned int) (( MEMORY )->mem_middle)  + typetable[((( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) & 0x10000000 )  ? (( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) ^ 0x10000000 ) : ( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) )  ].size  + 100) > (unsigned int) (( MEMORY )->mem_end)  - 2) {
                if ((( MEMORY )->mem_next)  != ((void *)0) ) {
                        if ((( MEMORY )->mem_next)  != ((void *)0) ) {
                                MEMORY = (( MEMORY )->mem_next) ;
                                goto begin;
                        } else {
                                hh = GC_newbank(typetable[((( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) & 0x10000000 )  ? (( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) ^ 0x10000000 ) : ( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) )  ].size );
                                if (!hh)
                                        return (void *) ((void *)0) ;
                        }
                } else {
                        hh = GC_newbank(typetable[((( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) & 0x10000000 )  ? (( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) ^ 0x10000000 ) : ( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) )  ].size );
                        if (!hh)
                                return (void *) ((void *)0) ;
                }
        }
        *((( MEMORY )->mem_middle) ) = type | 0x40000000 ;
        help = (( MEMORY )->mem_middle)  + 1;
        x = (char *) help;
        x += typetable[((( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) & 0x10000000 )  ? (( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) ^ 0x10000000 ) : ( (((   type   ) & 0x40000000 ) ? ((   type   ) ^ 0x40000000 ) : (   type   ) )  ) )  ].size ;
        (( MEMORY )->mem_middle)  = (int *) x;
        return (void *) help;
}
void           *
GC_allocate(int type)
{
        void           *erg = GC_alloc(type);
        return erg;
}
void 
GC_collect(void)
{
        Fixup();
        Move();
        MEMORY = ROOT;
        GC_print();
}
void 
GC_print(void)
{
        int            *p;
        pmemory_struct  help;
        help = ROOT;
}
double 
GC_avail(void)
{
        double          erg, erg2 = 0.0;
        pmemory_struct  help,help2;
        erg = 0.0;
        help2 = help = MEMORY;
        while (help != ((void *)0) ) {
                erg2 = (double) 100.0 - (double) 100.0 *((double) ((double) ( ((unsigned int) (( help )->mem_middle)  ) - (unsigned int) (( 1 ==0) ? (( help2 )->mem_start)  : (( help )->mem_start) )) / (double) 4000000 ) ) ;
                erg = (( erg2  <  erg ) ?  erg  :  erg2 ) ;
                help = (( help )->mem_next) ;
        }
        return erg;
}
void 
GC_free(void *x)
{
}

static void 
mark(void *x)
{
}
void           *
gc_tmp(unsigned int wie)
{
        int             tmp;
        void           *help;
        int             i;
        for (i = 0; i < typepointer; i++) {
                if ((typetable[((( (((   i   ) & 0x40000000 ) ? ((   i   ) ^ 0x40000000 ) : (   i   ) )  ) & 0x10000000 )  ? (( (((   i   ) & 0x40000000 ) ? ((   i   ) ^ 0x40000000 ) : (   i   ) )  ) ^ 0x10000000 ) : ( (((   i   ) & 0x40000000 ) ? ((   i   ) ^ 0x40000000 ) : (   i   ) )  ) )  ].size  > wie) && (typetable[((( (((   i   ) & 0x40000000 ) ? ((   i   ) ^ 0x40000000 ) : (   i   ) )  ) & 0x10000000 )  ? (( (((   i   ) & 0x40000000 ) ? ((   i   ) ^ 0x40000000 ) : (   i   ) )  ) ^ 0x10000000 ) : ( (((   i   ) & 0x40000000 ) ? ((   i   ) ^ 0x40000000 ) : (   i   ) )  ) )  ].size  < (wie + 1000))) {
                        help = GC_alloc(i);
                        return help;
                }
        }
        wie += 1000 - (wie % 1000);
        tmp = GC__registertype(wie, mark, "1.c", 10727);
        help = GC_alloc(tmp);
        return help;
}
static pmemory_struct permanent = ((void *)0) ;
static int perm_init = 0;
void *gc_perm(unsigned int i) {
void *help;
pmemory_struct list2, list = permanent;
i = align(i);
if (i+4 > 4000000 ) {
    fprintf((&_iob[2]) ,"no such large block allowed (%d, maximum %d\n",i,4000000 );
    exit(1);
}
if (!perm_init) {
 permanent = (pmemory_struct) malloc( sizeof(memory_struct) ) ;
 (( permanent )->mem_start)  = (( permanent )->mem_middle)  = (int *) malloc( 4000000  ) ;
 (( permanent )->mem_end)  = (int *)( (int) (( permanent )->mem_start)  + 4000000  - 1);
 (( permanent )->mem_next)  = ((void *)0) ;
 perm_init = 1;
 list = permanent;
}
while (  ( ((int) (( list )->mem_middle)  + i+4) > (int) (( list )->mem_end) ) && ((( list )->mem_next)  != ((void *)0) ) )
    list = (( list )->mem_next) ;
if ((( list )->mem_next)  == ((void *)0) ) {
  list2 = (pmemory_struct) malloc( sizeof(memory_struct) ) ;
  (( list2 )->mem_start)  = (( list2 )->mem_middle)  = (int *) malloc( 4000000  ) ;
  (( list2 )->mem_end)  = (int *) ((int) (( list2 )->mem_start)  + 4000000  - 1);
  (( list2 )->mem_next)  = (pmemory_struct) ((void *)0) ;
  (( list )->mem_next)   = list2;
  list = list2;
  }
help = (void *) (( list )->mem_middle) ;
(( list )->mem_middle)  = (int *) ((int) (( list )->mem_middle)  + i);
return help;
}
void GC_usage(void) {
int i;
printf("Usage statistics|n");
for(i=0;i<typepointer;i++) {
printf("type %d had %d calls\n",i,typetable[((( (((   i   ) & 0x40000000 ) ? ((   i   ) ^ 0x40000000 ) : (   i   ) )  ) & 0x10000000 )  ? (( (((   i   ) & 0x40000000 ) ? ((   i   ) ^ 0x40000000 ) : (   i   ) )  ) ^ 0x10000000 ) : ( (((   i   ) & 0x40000000 ) ? ((   i   ) ^ 0x40000000 ) : (   i   ) )  ) )  ].calls );
}
fflush((&_iob[1]) );
}
void va_end (__gnuc_va_list);            
enum __va_type_classes {
  __no_type_class = -1,
  __void_type_class,
  __integer_type_class,
  __char_type_class,
  __enumeral_type_class,
  __boolean_type_class,
  __pointer_type_class,
  __reference_type_class,
  __offset_type_class,
  __real_type_class,
  __complex_type_class,
  __function_type_class,
  __method_type_class,
  __record_type_class,
  __union_type_class,
  __array_type_class,
  __string_type_class,
  __set_type_class,
  __file_type_class,
  __lang_type_class
};
typedef __gnuc_va_list va_list;
struct timeval {
        long    tv_sec;          
        long    tv_usec;         
};
struct timezone {
        int     tz_minuteswest;  
        int     tz_dsttime;      
};
struct  itimerval {
        struct  timeval it_interval;     
        struct  timeval it_value;        
};
struct  rusage {
        struct timeval ru_utime;         
        struct timeval ru_stime;         
        long    ru_maxrss;
        long    ru_ixrss;                
        long    ru_idrss;                
        long    ru_isrss;                
        long    ru_minflt;               
        long    ru_majflt;               
        long    ru_nswap;                
        long    ru_inblock;              
        long    ru_oublock;              
        long    ru_msgsnd;               
        long    ru_msgrcv;               
        long    ru_nsignals;             
        long    ru_nvcsw;                
        long    ru_nivcsw;               
};
struct rlimit {
        int     rlim_cur;                
        int     rlim_max;                
};
static char file_[150];
static int line_;
int ein_neur_name;
int fatal_error__(file,line)
int line;
char * file;
{
        strcpy(file_,file);
        line_=line;
        return(1);
}
char *strip_extension(mstr)
char *mstr;
{
        char *x1,*x2,*x3;
        x2= x1= x3 = basename(mstr);
        while(x1 != ((void *)0) )
        {
                x2 = x1;
                x1 = strchr(x1+1,'.');
        }
        if (x2 != x3) *x2='\0';
        return(mstr);
}
char *extension(char* mstr)
{
        static char *empty="";
        char *x1,*x2,*x3;
        x2=x1=x3 = basename(mstr);
        while(x1!= ((void *)0) ) {
                x2 = x1;
                x1 = strchr(x1+1,'.');
        }
        if (x2 == x3) return(empty);
        return(x2+1);
}
char *basename(char *mstr)
{
        char *x1,*x2;
        x2=x1 = mstr;
        while(x1!= ((void *)0) ) {
                x2 = x1;
                x1 = strchr(x1+1,'/');
        }
        if (x2==mstr) return(mstr);
        return(x2+1);
}
char *pathname(char *mstr)
{
        char *x1,*x2;
        if (strlen(mstr)==0) return(mstr);
        x2=x1 = mstr;
        while(x1!= ((void *)0) ) {
                x2 = x1;
                x1 = strchr(x1+1,'/');
        }
        if (x2!=mstr) 
                x2 = x2+1;
         
        *x2 = '\0';
        return(mstr);
}
        
void memmove(void *p1,void *p2,int anz)
{
        char *q1,*q2;
        int i;
        ((void) (( anz>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   11026 ,  "anz>0" ), 0) )) ;
        q1 = (char*)p1;
        q2 = (char*)p2;
        for(i=0;i<anz;i++) {
                q1[i] = q2[i];
        }
}
void getrusage(int x, void* p)
{
}

int getpagesize(void)
{
return 1;
}

double get_time(void)
{
struct rusage r;
double x = 0.0;
u_long sec;
long int usec;
getrusage(0 ,&r);
sec = r.ru_utime.tv_sec;
usec= r.ru_utime.tv_usec;
x = ( (double) sec) + 0.000001 * (double) usec;
return x;
}
int get_size(void)
{
        struct rusage r;
        int x;
        
        getrusage(0 ,&r);
        x = r.ru_maxrss*getpagesize();
        x = x/1024;
        return(x);
}
typedef struct _w_listelem {
        int  node;
        int  pos;
        struct _w_listelem *next;
} W_LISTELEM,*WORKLIST;
WORKLIST worklist_create ( void );
WORKLIST worklist_insert ( WORKLIST, int, int );
int worklist_in ( WORKLIST, int );
int worklist_ine ( WORKLIST, int, int );
WORKLIST worklist_insert_if_not_there ( WORKLIST, int, int );
WORKLIST worklist_pop ( WORKLIST, int *, int * );
int  worklist_is_empty ( WORKLIST );
void worklist_print_stdout ( WORKLIST );
typedef struct { 
   int length;         
   KFG_NODE_ID *node;  
   int *pos;           
} EPREDS,*EPREDTAB;
        
typedef int DFI_ID;  
typedef struct {
        int length;
        o_ddfi  *array;
} *DFI_TUPLE,DFI_TUPLE_S;
typedef DFI_TUPLE *DFI_TAB;
typedef DFI_TAB *DFI_STORE;
typedef o_ddfi (*C_2_FUNK)(o_ddfi, o_ddfi);
typedef o_ddfi (*C_1_FUNK)(o_ddfi);
DFI_STORE gc_ipcp2_doit ( KFG );
int dfi_tuple_length ( DFI_TUPLE );
o_ddfi dfi_tuple_get ( DFI_TUPLE, int );
int dfi_size ( DFI_STORE );
DFI_TUPLE dfi_get ( DFI_STORE, DFI_ID ,KFG_EDGE_TYPE);
o_ddfi dfi_get_pos ( DFI_STORE, DFI_ID, int ,KFG_EDGE_TYPE);
void dfi_print_stdout ( DFI_STORE );
void dfi_print ( struct _iobuf  *, DFI_STORE );
o_ddfi dfi_get_info ( DFI_STORE, DFI_ID );
o_ddfi dfi_get_pre_info ( DFI_STORE, DFI_ID );
void mygc(int,int) ;
typedef unsigned long ull;
static ull mK;
extern int     *_mapping_arrity;
extern int     *_mapping_proc;
extern EPREDTAB *_mapping_extpred;
extern KFG      _mapping_g;
  int kfg_arrity ( KFG, KFG_NODE );
  int kfg_procnum ( KFG, KFG_NODE_ID );
KFG_NODE kfg_numproc ( KFG, int );
  EPREDS * ext_succs ( KFG_NODE, int );
void mapping_init ( KFG );
int get_arrity3 ( KFG_NODE );
void calc_mapping ( KFG );
int get_arrity ( KFG_NODE );
int get_mapping ( KFG_NODE, KFG_NODE, int );
char * ulltos ( ull );
static WORKLIST insert_succs ( KFG_NODE, int, WORKLIST );
static DFI_TUPLE calc_input_tup ( KFG_NODE );
static o_ddfi calc_input ( KFG_NODE, int );
static DFI_TUPLE dfi_tuple_create ( int );
static DFI_TUPLE dfi_tuple_set ( DFI_TUPLE, int, o_ddfi );
static DFI_TUPLE dfi_tuple_set_all ( DFI_TUPLE, o_ddfi );
static o_ddfi dfi_tuple_foldl ( DFI_TUPLE, C_2_FUNK, o_ddfi );
static DFI_STORE dfi_create (int);
static DFI_STORE dfi_set ( DFI_STORE, DFI_ID,KFG_EDGE_TYPE, DFI_TUPLE );
static DFI_STORE dfi_set_pos ( DFI_STORE, DFI_ID, int,KFG_EDGE_TYPE, o_ddfi );
static int anzahl=0;
static DFI_STORE d; 
KFG g;
static int anznod;
int debug_stat = 0;
int low_perc = 5;
int high_perc = 30;
int max_edge=32;
static DFI_STORE ddd;
static WORKLIST w= ((void *)0) ; 
extern long w_length;
extern double w_dlength;
DFI_STORE gc_ipcp2_doit(KFG xx)
{
        double start1,start2;
        int a,node,pos; 
        KFG_NODE_LIST l; 
        o_ddfi out,in;
        KFG_NODE n; 
        DFI_TUPLE tup;
        int ww,bit,i,j,b;
        int step=0;
        KFG_EDGE_TYPE ty;
        int  change;
         
        start1 = get_time();
        g = xx;
        mapping_init(g);
        all_init();
        w = worklist_create();
        anznod=a = kfg_anz_node(g); 
        ddd=d = dfi_create(a); 
        l = kfg_entrys (g); 
        while( !node_list_is_empty(l) ) { 
                w = worklist_insert(w, ((       0 , ((struct x_Sort10_mirCFG *)((   node_list_head(l)   ) ))->f_Unique) ) ,0); 
                l = node_list_tail(l); 
        } 
        max_edge=kfg_max_edge();
        for( i=0; i<a; i++) {    
                KFG_NODE n=kfg_get_node(g,i);
                int b,bit=kfg_which_edges(n);
                d[i] = (DFI_TAB) malloc( kfg_bit_anz(n)*sizeof(DFI_TUPLE) ) ;
                for(j=0,b=1;j<max_edge;b=b<<1,j++) {
                        if (bit&b) {
                                tup = dfi_tuple_create(kfg_arrity(g,n));
                                tup = dfi_tuple_set_all(tup,o_ddfi_bottom );
                                d = dfi_set(d,i,j,tup);   
                        }
                }
        } 
        start2 = get_time();
        while( !worklist_is_empty(w)) { 
                w_dlength += (double) w_length;
                step+=1;
                w = worklist_pop(w,&node,&pos); 
                n = kfg_get_node(g,node); 
                in = calc_input(n,pos);  
                bit = kfg_which_edges(n);
                change= 0 ;
                for(b=1,ww=0;ww<max_edge;b=b<<1,ww++) {
                        if (bit&b) {
                                out = o_www(in,transfer(in,n,(o_edges)ww)); 
                                if (!o_ddfi_eq(out,dfi_get_pos(d,node,pos,ww))) {
                                        change= 1 ;
                                        d = dfi_set_pos(d, node,pos,ww,out); 
                                }
                        }
                }
                if (change) {
                        w = insert_succs(n,pos,w);
                } 
        }
        if (debug_stat) {
                start1 = start2 - start1;
                start2 = get_time() - start2;
                fprintf((&_iob[2]) ,"\ngc_ipcp2 analyzer done in %g sec\n",start1+start2);
                fprintf((&_iob[2]) ,"%gsec initalizing %gsec iterating\n",start1,start2);
                fprintf((&_iob[2]) ,"%d steps, %d nodes, %f average worklistlength \n",step,a,w_dlength / (double) step);
                fprintf((&_iob[2]) ,"=> %g sec per step\n",start2/step);
        
                fprintf((&_iob[2]) ,"%dk Memory used total\n",get_size());
                {
                        char *s;
        
                        s = o_ddfi_len();
                        fprintf((&_iob[2]) ,"Longest Chain of o_ddfi: %s\n",s);
                         
                        free((void *) s ) ;
                }
                        all_stat();
        }
        return(d);
} 
static WORKLIST insert_succs(KFG_NODE node,int pos,WORKLIST w)
{
        KFG_NODE_LIST l;
        EPREDS *ll;
        int i;
        switch(kfg_node_type(g,node)) {
                case CALL:
                case END:
                        ll = ext_succs(node,pos);
                        for (i=0;i<ll->length;i++) {
                                w = worklist_insert_if_not_there(w,ll->node[i],ll->pos[i]);
                        }
                        break;
                default:
                        l = kfg_successors (g, node); 
                        while(!node_list_is_empty(l)) { 
                                w = worklist_insert_if_not_there(w,((   0 , ((struct x_Sort10_mirCFG *)((   node_list_head(l)   ) ))->f_Unique) ) 
                                        ,pos);
                                l = node_list_tail(l); 
                        }
        }
        return(w);
}
static DFI_TUPLE calc_input_tup(KFG_NODE n)
{
        DFI_TUPLE t;
        int a,i;
        a = kfg_arrity(g,n);
        t = dfi_tuple_create(a);
        for (i=0;i<a;i++) {
                t=dfi_tuple_set(t,i,calc_input(n,i));
        }
        return(t);
}
static o_ddfi calc_input(KFG_NODE n,int pos) 
{ 
        o_ddfi ret,in; 
        EPREDS *l; 
        KFG_NODE_LIST ll; 
        int i;
        ((void) (( pos>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11448 ,  "pos>=0" ), 0) )) ;
        ((void) (( pos<kfg_arrity(g,n) ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   11449 ,  "pos<kfg_arrity(g,n)" ), 0) )) ;
        switch(kfg_node_type(g,n)) { 
                case START: 
                case RETURN: 
                        l   = ext_succs(n,pos);
                        if (l->length==0) {
                                ret = o_ddfi_lift( o_dfi_create( o_const_fkt_create(o_va_top),o_va_top, o_const_fkt_create(o_va_top), o_alias_create(o_t1_top), o_alias_create(o_t1_top) ) ) ; 
                        } else {
                                ret = dfi_get_pos(d,l->node[0],l->pos[0],kfg_edge_type( kfg_get_node(g,l->node[0]) , n ) );
                                for(i=1;i<l->length;i++) {
                                        ret = o_comb(ret,dfi_get_pos(d,l->node[i],l->pos[i]
                                                ,kfg_edge_type( kfg_get_node(g,l->node[i]) , n ) )); 
                                }
                        }
                        break; 
                default: 
                        ll   = kfg_predecessors (g, n); 
                        if (node_list_is_empty(ll)){
                                ret = o_ddfi_bottom ;
                        } else {
                                KFG_NODE nn;
                                ret = dfi_get_pos(d,((  0 , ((struct x_Sort10_mirCFG *)((   nn=node_list_head(ll)   ) ))->f_Unique) ) ,pos
                                        ,kfg_edge_type( nn , n ) );
                                ll = node_list_tail(ll);
                                while( !node_list_is_empty(ll)) { 
                                        in = dfi_get_pos(d,((   0 , ((struct x_Sort10_mirCFG *)((   nn=node_list_head(ll)   ) ))->f_Unique) ) ,pos
                                        ,kfg_edge_type( nn , n ) );
                                        ret = o_comb(ret,in); 
                                        ll = node_list_tail(ll);
                                } 
                        }
        } 
        return(ret); 
} 
static DFI_TUPLE dfi_tuple_create(int anz)
{
        DFI_TUPLE help;
        int old;
        ((void) (( anz>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   11503 ,  "anz>0" ), 0) )) ;
        { ( help ) = ( DFI_TUPLE )malloc( sizeof(*( help )) ) ; if (( help )== ((void *)0) ) ein_neur_name= fatal_error__("1.c",11537)+fatal_error_ ("Out of memory (malloc)"); }; ;
        (( help )->length)  = anz;
        (( help )->array)  = (o_ddfi *) malloc( sizeof(o_ddfi) * anz ) ;
        if ((( help )->array)  == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",11543)+fatal_error_ ("No space in create_tuple");
        return (help);
}
int dfi_tuple_length(DFI_TUPLE t)
{
        return ((( t )->length) );
}
static DFI_TUPLE dfi_tuple_set(DFI_TUPLE t, int pos, o_ddfi elem)
{
        ((void) (( pos>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11563 ,  "pos>=0" ), 0) )) ;
        ((void) (( pos< (( t )->length)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   11564 ,  "pos<dfi_len(t)" ), 0) )) ;
        ((( t )->array) )[pos] = elem;
        return (t);
}
o_ddfi dfi_tuple_get(DFI_TUPLE t, int pos)
{
        ((void) (( pos>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11577 ,  "pos>=0" ), 0) )) ;
        ((void) (( pos< (( t )->length)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   11578 ,  "pos<dfi_len(t)" ), 0) )) ;
        return (((( t )->array) )[pos]);
}
static DFI_TUPLE dfi_tuple_set_all(DFI_TUPLE t , o_ddfi elem)
{
        int i;
        o_ddfi *z;
        for (i = 0, z = (( t )->array) ; i < (( t )->length) ; i++, z++) {
                *z = elem;
        }
        return (t);
}
static o_ddfi dfi_tuple_foldl(DFI_TUPLE t, C_2_FUNK funk, o_ddfi neutral)
{
        o_ddfi *z, erg;
        int i;
        erg = neutral;
        for (i = 0, z = (( t )->array) ; i < (( t )->length) ; i++, z++) {
                erg = (*funk) (erg, *z);
        }
        return (erg);
}
static DFI_STORE dfi_create(int anz)
{
        DFI_STORE help;
        ((void) (( anz>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   11674 ,  "anz>0" ), 0) )) ;
        if (anzahl!=0)
                ein_neur_name= fatal_error__("1.c",11676)+fatal_error_ ("wiederholter Aufruf von dfi_create");
        anzahl=anz;
        help = (DFI_STORE) malloc( sizeof(DFI_TUPLE) * anz ) ;
        if (help == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",11680)+fatal_error_ ("Kein Platz in create_dfi");
        return (help);
}
int dfi_size(DFI_STORE d)
{
        ((void) (( d==ddd ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11687 ,  "d==ddd" ), 0) )) ;
        return(anzahl);
}
static DFI_STORE dfi_set(DFI_STORE s, DFI_ID id,KFG_EDGE_TYPE ty,DFI_TUPLE t)
{
        ((void) (( s==ddd ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11697 ,  "s==ddd" ), 0) )) ;
        ((void) (( id>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   11698 ,  "id>=0" ), 0) )) ;
        ((void) (( id<anzahl ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   11699 ,  "id<anzahl" ), 0) )) ;
        s[id][kfg_bitpos(id,ty)] = t;
        return (s);
}
static DFI_STORE dfi_set_pos(DFI_STORE s, DFI_ID id,int pos,KFG_EDGE_TYPE ty, o_ddfi t)
{
        ((void) (( s==ddd ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11712 ,  "s==ddd" ), 0) )) ;
        ((void) (( id>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   11713 ,  "id>=0" ), 0) )) ;
        ((void) (( id<anzahl ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   11714 ,  "id<anzahl" ), 0) )) ;
        ((void) (( pos>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11715 ,  "pos>=0" ), 0) )) ;
         
        ((( s[id][kfg_bitpos(id,ty)] )->array) )[pos] = t;
         
        return (s);
}
DFI_TUPLE dfi_get(DFI_STORE s, DFI_ID id,KFG_EDGE_TYPE ty)
{
        ((void) (( s==ddd ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11728 ,  "s==ddd" ), 0) )) ;
        ((void) (( id>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   11729 ,  "id>=0" ), 0) )) ;
        ((void) (( id<anzahl ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   11730 ,  "id<anzahl" ), 0) )) ;
        return (s[id][kfg_bitpos(id,ty)]);
}
o_ddfi dfi_get_pos(DFI_STORE s, DFI_ID id,int pos,KFG_EDGE_TYPE ty)
{
        ((void) (( s==ddd ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11742 ,  "s==ddd" ), 0) )) ;
        ((void) (( id>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   11743 ,  "id>=0" ), 0) )) ;
        ((void) (( id<anzahl ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   11744 ,  "id<anzahl" ), 0) )) ;
        ((void) (( pos>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11745 ,  "pos>=0" ), 0) )) ;
        return (((( s[id][kfg_bitpos(id,ty)] )->array) )[pos]);
}
void dfi_print_stdout(DFI_STORE d)
{
        ((void) (( d==ddd ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11755 ,  "d==ddd" ), 0) )) ;
        dfi_print((&_iob[1]) ,d);
}
void dfi_print(struct _iobuf  *f, DFI_STORE d)
{
        int i,j,l,k,b,bit;
        DFI_TUPLE t;
        char *s;
        ((void) (( d==ddd ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11766 ,  "d==ddd" ), 0) )) ;
        for(i=0;i<anzahl;i++) {
                bit = kfg_which_edges(kfg_get_node(g,i));
                for(b=1,k=0;k<max_edge;b=b<<1,k++) {
                        if (bit&b) {
                                t = dfi_get(d,i,k);
                                l = dfi_tuple_length(t);
                                for(j=0;j<l;j++) {
                                        fprintf(f,"\n-- Knoten %d Tupelposition %d Kantentyp %d\n",i,j,k);
                                        s = o_ddfi_print(dfi_tuple_get(t,j));
                                        fprintf(f,"%s",s);
                                        free((void *) s ) ;
                                }
                        }
                }
        }
}
static char layout[1024];
char *dfi_printout(DFI_STORE d,int id, int pos,KFG_NODE_TYPE ty)
{
        DFI_TUPLE t;
        int l,j;
        char *s;
        ((void) (( d==ddd ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11793 ,  "d==ddd" ), 0) )) ;
        t = dfi_get(d,id,ty);
        l = dfi_tuple_length(t);
        ((void) (( l >= pos ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   11796 ,  "l >= pos" ), 0) )) ;
        j = pos;
        s = o_ddfi_print(dfi_tuple_get(t,j));
        sprintf(layout,"%.1024s",s);
        return(layout);
}
static char texlayout[10240];
char *dfi_vcgprintout(DFI_STORE d,int id, int pos,KFG_EDGE_TYPE ty,char *dir,char *file)
{
        DFI_TUPLE t;
        int l,j;
        char *s;
        struct _iobuf  *fp;
        char sd[2048];
        static char sd2[2048];
        ((void) (( d==ddd ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11813 ,  "d==ddd" ), 0) )) ;
        sprintf(sd,"%s/%s_%d_%d.tex",dir,file,id,pos);
        sprintf(sd2,"%s/%s_%d_%d.ppm",dir,file,id,pos);
        fp = fopen(sd,"w"); 
        if (fp == ((void *)0) ) {
            fprintf((&_iob[2]) ,"cannot open `%s' \n",sd);
            exit(1);
        }
        fprintf(fp,"\\documentstyle[12pt]{article}\n");
        fprintf(fp,"\\catcode`@=11 \n");
        fprintf(fp,"\\def\\verb{\\begingroup \\catcode``=13 \\@noligs \n");
        fprintf(fp,"\\tt\\LARGE \\let\\do\\@makeother \\dospecials \n");
        fprintf(fp,"\\@ifstar{\\@sverb}{\\@verb}} \n");
        fprintf(fp,"\\catcode`@=12 \n");
        fprintf(fp,"\\begin{document}\n");
        fprintf(fp,"\\LARGE\n");
        fprintf(fp,"\\pagestyle{empty}\n");
        fprintf(fp,"\\[\n");
        t = dfi_get(d,id,ty);
        l = dfi_tuple_length(t);
        ((void) (( l >= pos ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   11838 ,  "l >= pos" ), 0) )) ;
        j = pos;
        s = o_ddfi_vcgprint(dfi_tuple_get(t,j)); 
        fprintf(fp,"%s",s);
        fprintf(fp,"\\]\n");
        fprintf(fp,"\\end{document}\n");
        fclose(fp);
        return(sd2);
}
o_ddfi dfi_get_pre_info(DFI_STORE d, DFI_ID id)
{
        DFI_TUPLE tup;
        o_ddfi elem;
        ((void) (( d==ddd ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   11874 ,  "d==ddd" ), 0) )) ;
        tup = calc_input_tup(kfg_get_node(g,id));
        elem = dfi_tuple_foldl(tup,o_comb,o_ddfi_bottom );
        return(elem);
}
static void kfg_init ( KFG );
static int  has_kfg = 0 ;
KFG_NODE_LIST   _kfg_cfgnodes;
int             _kfg_cfgnumnodes;
LIST_mirIntraCfg _kfg_cfgs;
LIST_mirCFG     _kfg_starts, _kfg_ends, _kfg_calls, _kfg_returns;
KFG             _kfg_k;
KFG_NODE       *_kfg_table;
  KFG 
kfg_get(void)
{
        return (_kfg_k);
}
KFG 
kfg_create(mirUnit x)
{
        if (has_kfg)
                ein_neur_name= fatal_error__("1.c",11996)+fatal_error_ ("Nur ein KFG zur gleichen Zeit!");
        has_kfg = 1 ;
        _kfg_cfgnodes = (       0 , ((struct x_Sort1_mirUnit *)((  x  ) ))->f_cp_all) ;
        _kfg_cfgnumnodes = (    0 , ((struct x_Sort1_mirUnit *)((  x  ) ))->f_cp_number) ;
         
        ((void) (( _kfg_cfgnumnodes > 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   12004 ,  "_kfg_cfgnumnodes > 0" ), 0) )) ;
        _kfg_table = (KFG_NODE *) calloc( _kfg_cfgnumnodes + 5 ,  sizeof(KFG_NODE) ) ;
        _kfg_cfgs = (   0 , ((struct x_Sort1_mirUnit *)((  x  ) ))->f_CFGS) ;
        _kfg_k = (      0 , ((struct x_Sort1_mirUnit *)((  x  ) ))->f_cp_main) ;
        kfg_init(_kfg_k);
         
        return (_kfg_k);
}
  KFG_NODE 
kfg_entry(KFG g)
{
        return ((       0 , ((struct x_Sort10_mirIntraCfg *)((  g  ) ))->f_CfgEntry) );
}
  KFG_NODE_LIST 
kfg_entrys(KFG g)
{
        return (_kfg_starts);
}
  KFG_NODE_LIST 
kfg_calls(KFG g)
{
        return (_kfg_calls);
}
  KFG_NODE_LIST 
kfg_returns(KFG g)
{
        return (_kfg_returns);
}
  KFG_NODE_LIST 
kfg_exits(KFG g)
{
        return (_kfg_ends);
}
  KFG_NODE 
kfg_exit(KFG g)
{
        return ((       0 , ((struct x_Sort10_mirIntraCfg *)((  g  ) ))->f_CfgExit) );
}
  KFG_NODE_LIST 
kfg_predecessors(KFG k, KFG_NODE n)
{
        return ((       0 , ((struct x_Sort10_mirCFG *)((  n  ) ))->f_DirPreds) );
}
  KFG_NODE_LIST 
kfg_successors(KFG k, KFG_NODE n)
{
        return ((       0 , ((struct x_Sort10_mirCFG *)((  n  ) ))->f_DirSuccs) );
}
  KFG_NODE 
node_list_head(KFG_NODE_LIST l)
{
        return (LIST_mirCFG_hd(l));
}
  KFG_NODE_LIST 
node_list_tail(KFG_NODE_LIST l)
{
        return (LIST_mirCFG_tl(l));
}
  int  
node_list_is_empty(KFG_NODE_LIST l)
{
        return (((BOOL) ((   (  0 , ((struct x_Sort58_LIST_Const_List *)((   l   ) ))->f_Elems)    )  == ( 0  ) )  ) );
}
  int 
node_list_length(KFG_NODE_LIST l)
{
        return ((LIST_mirCFG_FLIST_length((     0 , ((struct x_Sort58_LIST_Const_List *)((   l   ) ))->f_Elems) )) );
}
  KFG_NODE_LIST 
kfg_all_nodes(KFG kfg)
{
        return (_kfg_cfgnodes);
}
           KFG_NODE
kfg_get_node(KFG g, KFG_NODE_ID id)
{
        ((void) (( _kfg_table[id] != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   12142 ,  "_kfg_table[id] != NULL" ), 0) )) ;
        return (_kfg_table[id]);
}
           KFG_NODE_ID
kfg_get_id(KFG g, KFG_NODE n)
{
        return ((       0 , ((struct x_Sort10_mirCFG *)((  n  ) ))->f_Unique) );
}
  int 
kfg_anz_node(KFG g)
{
        ((void) (( _kfg_cfgnumnodes > 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   12162 ,  "_kfg_cfgnumnodes > 0" ), 0) )) ;
        return (_kfg_cfgnumnodes);
}
           KFG_NODE_TYPE
kfg_node_type(KFG k, KFG_NODE n)
{
        ITERLIST_mirIntraCfg node_cur;
        mirIntraCfg     cfgnode;
        KFG_NODE_ID     nn;
        switch ((((DMCP_xnode)(   (     0 , ((struct x_Sort10_mirCFG *)((  n  ) ))->f_Stat)    ) )->op) )  {
                break; case xop_Sort10_mirEndCall  : 
                        return (RETURN);
                break; case xop_Sort10_mirEndTailCall  : 
                        return (RETURN);
                break; case xop_Sort10_mirEndFuncCall  : 
                        return (RETURN);
                break; case xop_Sort10_mirCall  : 
                        return (CALL);
                break; case xop_Sort10_mirTailCall  : 
                        return (CALL);
                break; case xop_Sort10_mirFuncCall  : 
                        return (CALL);
                break; default:; 
                        goto gut;
        }  ;
        ((void) (( 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   12196 ,  "0" ), 0) )) ;
gut:
        nn = kfg_get_id(k, n);
          node_cur  = ITERLIST_mirIntraCfg_create( _kfg_cfgs );   cfgnode =ITERLIST_mirIntraCfg_getcurrentelt(  node_cur );     for(;!(((     cfgnode    )  == ( 0  ) )  );     (ITERLIST_mirIntraCfg_stepcursor(  node_cur ),    cfgnode =ITERLIST_mirIntraCfg_getcurrentelt(  node_cur )))  {
                if (kfg_get_id(k, (     0 , ((struct x_Sort10_mirIntraCfg *)((  cfgnode  ) ))->f_CfgEntry) ) == nn) {
                        return (START);
                }
                if (kfg_get_id(k, (     0 , ((struct x_Sort10_mirIntraCfg *)((  cfgnode  ) ))->f_CfgExit) ) == nn) {
                        return (END);
                }
        }
        return (INNER);
}
static void 
kfg_init(KFG k)
{
        ITERLIST_mirCFG node_cur;
        mirCFG          nod;
        _kfg_starts = LIST_mirCFG_create();
        _kfg_ends = LIST_mirCFG_create();
        _kfg_calls = LIST_mirCFG_create();
        _kfg_returns = LIST_mirCFG_create();
          node_cur  = ITERLIST_mirCFG_create( _kfg_cfgnodes );    nod =ITERLIST_mirCFG_getcurrentelt(  node_cur );      for(;!(((     nod    )  == ( 0  ) )  ); (ITERLIST_mirCFG_stepcursor(  node_cur ),         nod =ITERLIST_mirCFG_getcurrentelt(  node_cur )))  {
                _kfg_table[(    0 , ((struct x_Sort10_mirCFG *)((  nod  ) ))->f_Unique) ] = nod;
                switch (kfg_node_type(k, nod)) {
                case CALL:
                        LIST_mirCFG_cons(_kfg_calls, nod);
                        break;
                case RETURN:
                        LIST_mirCFG_cons(_kfg_returns, nod);
                        break;
                case START:
                        LIST_mirCFG_cons(_kfg_starts, nod);
                        break;
                case END:
                        LIST_mirCFG_cons(_kfg_ends, nod);
                        break;
                }
        }
}
  int 
get_caller(KFG_NODE nod)
{
        mirSTMT         stat;
        int             caller;
        ((void) (( kfg_node_type(_kfg_k, nod) == RETURN ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   12250 ,  "kfg_node_type(_kfg_k, nod) == RETURN" ), 0) )) ;
        stat = (        0 , ((struct x_Sort10_mirCFG *)((  nod  ) ))->f_Stat) ;
        switch ((((DMCP_xnode)(   stat   ) )->op) )  {
                break; case xop_Sort10_mirEndCall  : 
                        caller = (      0 , ((struct x_Sort10_mirEndCall *)((  stat  ) ))->f_caller) ;
                break ;
                break; case xop_Sort10_mirEndTailCall  : 
                        caller = (      0 , ((struct x_Sort10_mirEndTailCall *)((  stat  ) ))->f_caller) ;
                break ;
                break; case xop_Sort10_mirEndFuncCall  : 
                        caller = (      0 , ((struct x_Sort10_mirEndFuncCall *)((  stat  ) ))->f_caller) ;
                break ;
        }  ;
        return (caller);
}
  KFG_NODE 
kfg_get_call(KFG k, KFG_NODE n)
{
        ((void) (( kfg_node_type(k, n) == RETURN ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   12270 ,  "kfg_node_type(k, n) == RETURN" ), 0) )) ;
        return (_kfg_table[get_caller(n)]);
}
  KFG_NODE 
kfg_get_return(KFG k, KFG_NODE n)
{
        ITERLIST_mirCFG node_cur;
        mirCFG          nod;
        KFG_NODE_ID     id;
        ((void) (( kfg_node_type(k, n) == CALL ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   12281 ,  "kfg_node_type(k, n) == CALL" ), 0) )) ;
        id = kfg_get_id(k, n);
          node_cur  = ITERLIST_mirCFG_create( _kfg_returns );     nod =ITERLIST_mirCFG_getcurrentelt(  node_cur );      for(;!(((     nod    )  == ( 0  ) )  ); (ITERLIST_mirCFG_stepcursor(  node_cur ),         nod =ITERLIST_mirCFG_getcurrentelt(  node_cur )))  {
                if (get_caller(nod) == id)
                        return (nod);
        }
        ein_neur_name= fatal_error__("1.c",12287)+fatal_error_ ("No return");
}
  KFG_NODE 
kfg_get_startend(KFG k, KFG_NODE n, int  sta)
{
        ITERLIST_mirIntraCfg cfg_cur;
        mirIntraCfg     cfg;
        KFG_NODE_ID     nn;
        ((void) (( kfg_node_type(k, n) == sta ? END : START ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   12299 ,  "kfg_node_type(k, n) == sta ? END : START" ), 0) )) ;
        nn = kfg_get_id(k, n);
          cfg_cur  = ITERLIST_mirIntraCfg_create( _kfg_cfgs );    cfg =ITERLIST_mirIntraCfg_getcurrentelt(  cfg_cur );  for(;!(((     cfg    )  == ( 0  ) )  ); (ITERLIST_mirIntraCfg_stepcursor(  cfg_cur ),     cfg =ITERLIST_mirIntraCfg_getcurrentelt(  cfg_cur )))  {
                if (kfg_get_id(k, sta ? (       0 , ((struct x_Sort10_mirIntraCfg *)((  cfg  ) ))->f_CfgExit) 
                       : (      0 , ((struct x_Sort10_mirIntraCfg *)((  cfg  ) ))->f_CfgEntry) ) == nn) {
                        return (sta ? ( 0 , ((struct x_Sort10_mirIntraCfg *)((  cfg  ) ))->f_CfgEntry)  :
                                (       0 , ((struct x_Sort10_mirIntraCfg *)((  cfg  ) ))->f_CfgExit) );
                }
        }
        ein_neur_name= fatal_error__("1.c",12308)+fatal_error_ ("No found!");
}
  KFG_NODE 
kfg_get_start(KFG k, KFG_NODE n)
{
        ((void) (( kfg_node_type(k, n) == END ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",       "1.c" ,   12315 ,  "kfg_node_type(k, n) == END" ), 0) )) ;
        return (kfg_get_startend(k, n, 1 ));
}
  KFG_NODE 
kfg_get_end(KFG k, KFG_NODE n)
{
        ((void) (( kfg_node_type(k, n) == START ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   12323 ,  "kfg_node_type(k, n) == START" ), 0) )) ;
        return (kfg_get_startend(k, n, 0 ));
}
  int 
kfg_proc_anz(KFG g)
{
        return (node_list_length(_kfg_starts));
}
  int 
kfg_edge_type_max(void)
{
        return 3;
}
  int 
kfg_max_edge(void)
{
        return kfg_edge_type_max();
}
  KFG_EDGE_TYPE 
kfg_edge_type(KFG_NODE source, KFG_NODE target)
{
        KFG_EDGE_LIST   help;
        KFG_EDGE        he;
        ITERLIST_mirEDGE hecur;
        int             erg = -1;
        help = (        0 , ((struct x_Sort10_mirCFG *)((  source  ) ))->f_Succs) ;
          hecur  = ITERLIST_mirEDGE_create( help );       he =ITERLIST_mirEDGE_getcurrentelt(  hecur ); for(;!(((     he    )  == ( 0  ) )  );  (ITERLIST_mirEDGE_stepcursor(  hecur ),   he =ITERLIST_mirEDGE_getcurrentelt(  hecur )))  {
                if ((   0 , ((struct x_Sort10_mirEDGE *)((  he  ) ))->f_Target)  == target)
                        switch ((       0 , ((struct x_Sort10_mirEDGE *)((  he  ) ))->f_Type) ) {
                        case 1:
                                erg = kfg_edge_true;
                                break;
                        case 2:
                                erg = kfg_edge_false;
                                break;
                        default:
                                erg = kfg_edge_normal;
                        }
        }
        if (erg != -1) {
                return (erg);
        }
        exit(1);
}
  int 
kfg_which_edges(KFG_NODE source)
{
        KFG_EDGE_LIST   help;
        KFG_EDGE        he;
        ITERLIST_mirEDGE hecur;
        int             ty, result = 0;
        help = (        0 , ((struct x_Sort10_mirCFG *)((  source  ) ))->f_Succs) ;
          hecur  = ITERLIST_mirEDGE_create( help );       he =ITERLIST_mirEDGE_getcurrentelt(  hecur ); for(;!(((     he    )  == ( 0  ) )  );  (ITERLIST_mirEDGE_stepcursor(  hecur ),   he =ITERLIST_mirEDGE_getcurrentelt(  hecur )))  {
                ty = (  0 , ((struct x_Sort10_mirEDGE *)((  he  ) ))->f_Type) ;
                switch (ty) {
                case 1:
                        result |= 1;
                        break;
                case 2:
                        result |= 2;
                        break;
                default:
                        result |= 4;
                }
        }
        if (result == 0)
                result = 4;
        return result;
}
  int 
kfg_bitpos(int nodeid, KFG_EDGE_TYPE edge)
{
        KFG_NODE        no = kfg_get_node(_kfg_k, nodeid);
        int             ed = kfg_which_edges(no);
        int             i = 0, j = -1;
        j = -1;
        ((void) (( edge < 32 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   12432 ,  "edge < 32" ), 0) )) ;
        ((void) (( edge >= 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   12433 ,  "edge >= 0" ), 0) )) ;
        while (i <= edge) {
                if ((1 << i) & ed)
                        j+=1;
                i+=1;
        }
        ((void) (( (1 << edge) & ed ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   12444 ,  "(1 << edge) & ed" ), 0) )) ;
        ((void) (( j >= 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   12445 ,  "j >= 0" ), 0) )) ;
        return (j);
}
  int 
kfg_bit_anz(KFG_NODE node)
{
        int             i, result;
        int             ed = kfg_which_edges(node);
        result = 0;
        for (i = 0; i < 31; i++) {
                if ((1 << i) & ed)
                        result+=1;
        }
        return result;
}
static void set_procnum ( KFG, KFG_NODE, int , int );
static   void calc_mapping1 ( KFG );
static   int get_arrity1 ( KFG_NODE );
static   int get_mapping1 ( KFG_NODE, KFG_NODE, int );
static   void calc_mapping2 ( KFG );
static   int get_arrity2 ( KFG_NODE );
static   int get_mapping2 ( KFG_NODE, KFG_NODE, int );
static   int get_anz_pos ( int );
static   int get_real_pos_ ( int, ull );
static   int get_real_pos ( int, ull );
static   int  is_in_virpos ( int, ull );
static   ull get_vir_pos ( int, int );
static   void virpos_insert ( int, ull );
static   int callnum ( KFG_NODE );
static   int get_mapping3 ( KFG_NODE, KFG_NODE, int );
static   void calc_mapping3 ( KFG );
int     *_mapping_arrity;
int     *_mapping_proc;
EPREDTAB *_mapping_extpred;
KFG      _mapping_g;
int sel_mapping=1;
static int     *visited;
static int      vis_val = 1;
static int      anz = 0;
static int prnum;
  int 
kfg_arrity(KFG k, KFG_NODE n)
{
        ((void) (( _mapping_arrity != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",          "1.c" ,   12503 ,  "_mapping_arrity != NULL" ), 0) )) ;
        return (_mapping_arrity[kfg_get_id(k, n)]);
}
  int kfg_procnum(KFG g, KFG_NODE_ID n) {
        return(_mapping_proc[n]);
}
KFG_NODE kfg_numproc(KFG g,int i) {
        KFG_NODE_LIST l;
        l = kfg_entrys(g);
        while(!node_list_is_empty(l)) {
                if((kfg_procnum( g ,kfg_get_id( g , node_list_head(l) ))) ==i) {
                        return(node_list_head(l));
                }
                l = node_list_tail(l);
        }
        ein_neur_name= fatal_error__("1.c",12524)+fatal_error_ ("in numproc (i=%d anz=%d)",i,node_list_length(kfg_entrys(g)));
        return(((void *)0) );
}
  EPREDS         *
ext_succs(KFG_NODE n, int pos)
{
        ((void) (( pos >= 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   12533 ,  "pos >= 0" ), 0) )) ;
        ((void) (( pos < _mapping_arrity[kfg_get_id(_mapping_g, n)] ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   12534 ,  "pos < _mapping_arrity[kfg_get_id(_mapping_g, n)]" ), 0) )) ;
        ((void) (( _mapping_extpred[kfg_get_id(_mapping_g, n)] != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   12535 ,  "_mapping_extpred[kfg_get_id(_mapping_g, n)] != NULL" ), 0) )) ;
        return (&(_mapping_extpred[kfg_get_id(_mapping_g, n)][pos]));
}
void 
mapping_init(KFG k)
{
        KFG_NODE_LIST   l, ll;
        KFG_NODE        n, nn;
        EPREDTAB        tab,tab1,tab2;
        int             faktor,mysum,pp,arr, i,  j, len;
        KFG_NODE_ID     id4,id3, id, id2;
        int             *x;
        fprintf((&_iob[2]) ,"Mapping %d\n",sel_mapping);
        ((void) (( sizeof(KFG_NODE_ID) == sizeof(int) ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",       "1.c" ,   12552 ,  "sizeof(KFG_NODE_ID) == sizeof(int)" ), 0) )) ;
        faktor = sizeof(EPREDS) / sizeof(int);
        ((void) (( sizeof(EPREDS) % sizeof(int)==0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",          "1.c" ,   12554 ,  "sizeof(EPREDS) % sizeof(int)==0" ), 0) )) ;
        _mapping_g = k;
        anz = kfg_anz_node(k);
         
        x = (int*)gc_perm(2* anz * sizeof(int) 
                + anz * ((( sizeof(EPREDTAB*) )<( sizeof(int) ))?( sizeof(int) ):( sizeof(EPREDTAB*) )) );
        if (x == ((void *)0) ) ein_neur_name= fatal_error__("1.c",12563)+fatal_error_ ("No mem");
        _mapping_proc =  x;
        x += anz;
        _mapping_arrity =  x;
        x += anz;
        visited = x;
        for(i=0;i<anz;i++) {
                visited[i] = 0;
        }
         
        prnum = 0;  
        l = kfg_entrys(k);
        while (!node_list_is_empty(l)) {
                n = node_list_head(l);
                ((void) (( kfg_node_type(k,n)==START ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   12579 ,  "kfg_node_type(k,n)==START" ), 0) )) ;
                l = node_list_tail(l);
                set_procnum(k, n, 1 ,prnum);
                vis_val++;
                set_procnum(k, kfg_get_end(k, n), 0 ,prnum);
                vis_val++;
                prnum+=1;
        }
        calc_mapping(_mapping_g);
        l = kfg_all_nodes(k);
        while (!node_list_is_empty(l)) {
                n = node_list_head(l);
                l = node_list_tail(l);
                arr = get_arrity(n);
                ((void) (( arr>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   12595 ,  "arr>0" ), 0) )) ;
                _mapping_arrity[kfg_get_id(k,n)] = arr;
        }
         
         
        fprintf((&_iob[2]) ,"Calculating Mappings\n");
        _mapping_extpred = (EPREDTAB *) x;   
         
        l = kfg_entrys(k);
        while (!node_list_is_empty(l)) {
                n = node_list_head(l);   
                l = node_list_tail(l);
                id = kfg_get_id(k, n);
                x = (int *) (gc_perm(2*_mapping_arrity[id]* sizeof(EPREDS)));
                if (x== ((void *)0) ) ein_neur_name= fatal_error__("1.c",12619)+fatal_error_ ("No mem");
                tab1 = (EPREDTAB) x;
                x += _mapping_arrity[id]*faktor;
                tab2 = (EPREDTAB) x;
                for (i=0;i<_mapping_arrity[id];i++) {
                        tab2[i].length = 0;
                        tab1[i].length = 0;
                        tab1[i].node = ((void *)0) ;
                }
                _mapping_extpred[id] = tab1;
                _mapping_extpred[kfg_get_id(k,kfg_get_end(k,n))] = tab2;
        }
        fprintf((&_iob[2]) ,"Calculating Call/Return-Mappings\n");
        mysum = 0;  
        l = kfg_calls(k);
        while (!node_list_is_empty(l)) {
                n = node_list_head(l);    
                l = node_list_tail(l);   
                id = kfg_get_id(k, n);
                id2 = kfg_get_id(k, kfg_get_return(k,n));
                ll = kfg_successors(k, n);
                len = node_list_length(ll);     
                x = (int *)gc_perm(2*_mapping_arrity[id]*sizeof(EPREDS)  
                        +2*_mapping_arrity[id]*len*(sizeof(KFG_NODE_ID)*sizeof(int)));
                if (x == ((void *)0) ) ein_neur_name= fatal_error__("1.c",12644)+fatal_error_ ("No mem");
                _mapping_extpred[id] = tab1 = (EPREDTAB) x;
                x += _mapping_arrity[id]*faktor;
                _mapping_extpred[id2] = tab2 = (EPREDTAB) x;
                x += _mapping_arrity[id]*faktor;
                if (len==0) {
                        for(i=0;i<_mapping_arrity[id];i++) {
                                tab2[i].length = tab1[i].length = 0;
                        }
                } else {
                        for (i = 0; i < _mapping_arrity[id]; i++) {
                                tab1[i].length = tab2[i].length = len;
                                tab1[i].pos =  x;
                                x += len;
                                tab2[i].pos =  x;
                                x += len;
                                tab1[i].node = (KFG_NODE_ID*) x;
                                x += len;
                                tab2[i].node = (KFG_NODE_ID*) x;
                                x += len;
                        }
                }
                j = 0;
                while (!node_list_is_empty(ll)) {
                        nn = node_list_head(ll);
                        id3 = kfg_get_id(k,nn);
                        for (i = 0; i < _mapping_arrity[id]; i++) {
                                if (kfg_node_type(k,nn) == START) {
                                        (tab1[i].node)[j] = id3;
                                        id4 = kfg_get_id(k,kfg_get_end(k,nn));
                                        (tab2[i].node)[j] = id4;
                                        pp = (tab2[i].pos)[j] 
                                                =(tab1[i].pos)[j] = get_mapping(n, nn, i);
                                         
                                        _mapping_extpred[id3][pp].length += 1;
                                        mysum +=1;
                                } else {  
                                        (tab2[i].node)[j] = (tab1[i].node)[j] = id3;
                                        (tab2[i].pos)[j] = (tab1[i].pos)[j] = i;
                                }
                        }
                        j +=1;
                        ll = node_list_tail(ll);
                }
        }
         
        fprintf((&_iob[2]) ,"Calculating Entry/Exit-Mappings\n");
        x = (int *) gc_perm(2*mysum *sizeof(int));
        if (x== ((void *)0) ) ein_neur_name= fatal_error__("1.c",12711)+fatal_error_ ("No mem");
        l = kfg_calls(k);
        while(!node_list_is_empty(l)) {
                n = node_list_head(l);
                l = node_list_tail(l);
                id = kfg_get_id(k, n);
                tab = _mapping_extpred[id];  
                for(i=0;i<_mapping_arrity[id];i++) {
                        for(j=0;j<tab[i].length;j++) {
                                nn = kfg_get_node(k,(tab[i].node)[j]);  
                                if (kfg_node_type(k,nn)==START) {  
                                         
                                        id2= kfg_get_id(k,nn);
                                        id3= kfg_get_id(k,kfg_get_end(k,nn));
                                        pp = (tab[i].pos)[j];
                                        if (_mapping_extpred[id2][pp].node == ((void *)0) ) { 
                                                 
                                                _mapping_extpred[id2][pp].node = x;   
                                                x += _mapping_extpred[id2][pp].length;
                                                _mapping_extpred[id2][pp].pos = x;
                                                x += _mapping_extpred[id2][pp].length;
                                                _mapping_extpred[id3][pp].node = x;   
                                                x += _mapping_extpred[id2][pp].length;
                                                _mapping_extpred[id3][pp].pos = x;
                                                x += _mapping_extpred[id2][pp].length;
                                                _mapping_extpred[id2][pp].length = 0;
                                        }
                                        (_mapping_extpred[id2][pp].node)[_mapping_extpred[id2][pp].length] = id;
                                        (_mapping_extpred[id3][pp].node)[_mapping_extpred[id2][pp].length] 
                                                = kfg_get_id(k,kfg_get_return(k,n));
                                        (_mapping_extpred[id2][pp].pos)[_mapping_extpred[id2][pp].length] 
                                        = (_mapping_extpred[id3][pp].pos)[_mapping_extpred[id2][pp].length] = i;
                                        _mapping_extpred[id3][pp].length = _mapping_extpred[id2][pp].length = _mapping_extpred[id2][pp].length +1;
                                }
                        }
                }
        }
        fprintf((&_iob[2]) ,"Calculating Mappings done!\n");
}
                
static void 
set_procnum(KFG k, KFG_NODE n, int  forw,int num)
{
        int             id;
        KFG_NODE_LIST   l;
        KFG_NODE        nod;
        id = kfg_get_id(k, n);
        if (visited[id] == vis_val)
                return;
        _mapping_proc[id] = num;
        visited[id] = vis_val;
        if (kfg_node_type(k, n) == (forw ? END : START))
                return;
        l = forw ? kfg_successors(k, n) : kfg_predecessors(k, n);
        while (!node_list_is_empty(l)) {
                nod = node_list_head(l);
                l = node_list_tail(l);
                if (kfg_node_type(k, nod) == (forw ? START : END)) {
                        nod = forw ? kfg_get_return(k, n) : kfg_get_call(k, n);
                }
                set_procnum(k, nod, forw,num);
        }
}
static   void calc_mapping1(KFG g) {
}
static   int 
get_arrity1(KFG_NODE node)
{
        return (1);
}
static   int 
get_mapping1(KFG_NODE call, KFG_NODE start, int pos)
{
        return (0);
}
static   void calc_mapping2(KFG g) {
}
static   int 
get_arrity2(KFG_NODE node)
{
        int             i;
        KFG_NODE start;
        start = kfg_numproc(_mapping_g,(kfg_procnum( _mapping_g ,kfg_get_id( _mapping_g , node ))) );
        i = node_list_length(kfg_predecessors(_mapping_g, start));
        if (i == 0)
                return (1);
        return (i);
}
static   int 
get_mapping2(KFG_NODE call, KFG_NODE start, int pos)
{
        KFG_NODE_LIST   l;
        int             i = 0;
        l = kfg_predecessors(_mapping_g, start);
        while (!node_list_is_empty(l)) {
                if (kfg_get_id(_mapping_g,node_list_head(l)) == kfg_get_id(_mapping_g,call))
                        return (i);
                i++;
                l = node_list_tail(l);
        }
        ein_neur_name= fatal_error__("1.c",12839)+fatal_error_ ("Call not found");
}
int J = 4;   
static int M;   
static int proc_anz;        
typedef struct {  int len;    
                  int max;    
                                                int oldoldlen;  
                                                int oldlen;  
                  ull *arr;    
               } virtyp;
static virtyp *virpos;  
static   int get_anz_pos(int proc) {
        ((void) (( proc>=0 && proc<proc_anz ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   12873 ,  "proc>=0 && proc<proc_anz" ), 0) )) ; 
        return(virpos[proc].len);
}
static   int get_real_pos_(int proc,ull pos) {
        virtyp *v = virpos+proc;
        ull *x;
        int i;
        ((void) (( proc>=0 && proc<proc_anz ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   12888 ,  "proc>=0 && proc<proc_anz" ), 0) )) ;
        for(i=0,x=v->arr;i<v->len; i++,x++)
                if (*x==pos) return(i); 
        return(-1);
}
static   int get_real_pos(int proc, ull pos) {
        int x = get_real_pos_(proc,pos);
        
        if (x<0) 
        ein_neur_name= fatal_error__("1.c",12899)+fatal_error_ ("real pos %s for proc %d not found in get_real_pos\n"
                ,ulltos(pos),proc);
        return(x);
}
static   int  is_in_virpos(int proc, ull pos) {
        int x=get_real_pos_(proc,pos);
        return(x==-1?0 : 1 );
}
static   ull get_vir_pos(int proc, int pos) {
        virtyp *v = virpos+proc;
        ((void) (( proc>=0 && proc<proc_anz ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   12922 ,  "proc>=0 && proc<proc_anz" ), 0) )) ;
        ((void) (( v->len>pos ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",       "1.c" ,   12923 ,  "v->len>pos" ), 0) )) ;
        ((void) (( pos>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   12924 ,  "pos>=0" ), 0) )) ;
        return(v->arr[pos]);
}
static   void virpos_insert(int proc, ull pos) {
        virtyp *v = virpos+proc;
        ull *help,*h1,*h2;
        int i;
        ((void) (( proc>=0 && proc<proc_anz ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   12935 ,  "proc>=0 && proc<proc_anz" ), 0) )) ;
        ((void) (( v->len <= v->max ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   12936 ,  "v->len <= v->max" ), 0) )) ;
        if (v->len == v->max) {
                if (v->max>= 2147483647  /2) ein_neur_name= fatal_error__("1.c",12944)+fatal_error_ ("Mapping too large");
                 
                help =(ull*) malloc( v->max*2*sizeof(ull) ) ;
                if (help== ((void *)0) ) ein_neur_name= fatal_error__("1.c",12947)+fatal_error_ ("No mem");
                v->max *= 2;
                for (i=0,h1=help,h2=v->arr;i<v->len;h1++,h2++,i++) 
                        *h1 = *h2;
                free((void *) (void*)v->arr ) ;
                v->arr =help;
        }
        v->arr[v->len++] = pos;
}
                        
static   int callnum(KFG_NODE call) {
        KFG_NODE_LIST l = kfg_calls(_mapping_g);
        int id = kfg_get_id(_mapping_g,call);
        int j=0;
        while(!node_list_is_empty(l)) {
                if (kfg_get_id(_mapping_g,node_list_head(l))==id)
                        return((j%(mK+1))+1);
                j +=1;
                l = node_list_tail(l);
        }
        ein_neur_name= fatal_error__("1.c",12970)+fatal_error_ ("in callnum");
        return(0);
}
static   int get_mapping3(KFG_NODE call, KFG_NODE start, int pos) {
        int i;
        i= (get_real_pos( (kfg_procnum( _mapping_g ,kfg_get_id( _mapping_g , start )))  , 
                ((get_vir_pos( (kfg_procnum( _mapping_g ,kfg_get_id( _mapping_g , call )))  , pos )) *(ull)M + (ull)(callnum( call )) ) % mK )) ;
        return(i);
}
int get_arrity3(KFG_NODE start) {
        
        return((get_anz_pos( (kfg_procnum( _mapping_g ,kfg_get_id( _mapping_g , start )))  )) );
}
static   void calc_mapping3(KFG g) {
        int count;
        ull j;
        int i,cnum;
        KFG_NODE_LIST l;
        KFG_NODE n;
        int  initial= (bool_t)1 ,change= 1 ;
        double d;
        virtyp   *v;
        ull *il;
        proc_anz = kfg_proc_anz(g);
        M = node_list_length(kfg_calls(g));
        d = pow((double)M,(double)J);
        if  (d> (0xFFFFFFFF) ) ein_neur_name= fatal_error__("1.c",13020)+fatal_error_ ("Arity too large (%e)",d);
        mK = (ull)d;
        if (mK<=proc_anz) {
                fprintf((&_iob[2]) ,"Warning: No unique mapping K=%s, proc_anz=%d\n",ulltos(mK),proc_anz);
        }
        
        virpos = (virtyp*) gc_perm(sizeof(virtyp)*proc_anz);
        if (virpos== ((void *)0) ) ein_neur_name= fatal_error__("1.c",13031)+fatal_error_ ("No mem");
        for (i=0;i<proc_anz;i++) {
                v=virpos+i;
                v->len = v->oldlen = v->oldoldlen = 0;
                v->max = (128) ;
                v->arr = (ull*) malloc( sizeof(ull)* (128)  ) ;
                if (v->arr== ((void *)0) ) ein_neur_name= fatal_error__("1.c",13037)+fatal_error_ ("No mem");
        }
        count=0;
        fprintf((&_iob[2]) ,"=> %5d\b\b\b\b\b\b\b\b",0);
        while(change) {
                if (++count% 1 ==0) fprintf((&_iob[2]) ,"=> %5d\b\b\b\b\b\b\b\b",count/ 1 );
                change = 0 ;
                for(i=0;i<proc_anz;i++) {
                        v = virpos +i;
                        v->oldoldlen=v->oldlen;
                        v->oldlen=v->len;
                }
                for (i=0;i<proc_anz;i++) {
                        fprintf((&_iob[2]) ,"=> %5d/%5d\b\b\b\b\b\b\b\b\b\b\b\b\b\b",count,i);
         l = kfg_predecessors(g,kfg_numproc(g,i));  
                        if (node_list_is_empty(l) && initial) {
                                if (!is_in_virpos(i,0UL)) {
                                        virpos_insert(i,0UL);
                                        change = 1 ;
                                }
                        } else {
                while(!node_list_is_empty(l)) {
                n = node_list_head(l);
                                        cnum = callnum(n);
                l = node_list_tail(l);
                                        v = virpos + (kfg_procnum( g ,kfg_get_id( g , n ))) ;
                                        il = v->arr+v->oldoldlen;
                                        while(il<v->arr+v->len) {
                                                j = (*il *(ull)M + (ull)cnum ) % mK;
                                                if (!is_in_virpos(i,j)) {
                                                        virpos_insert(i,j);
                                                        change = 1 ;
                                                }
                                                il +=1;
                                        }
                                }
                        }
                }
                initial= 0 ;
        }
        fprintf((&_iob[2]) ,"calc_mapping done!\n");
}
                        
                
void calc_mapping(KFG g)
{ 
        switch(sel_mapping) {
                case 1: (calc_mapping1(g));return;
                case 2: (calc_mapping2(g));return;
                case 3: (calc_mapping3(g));return;
        }
        ein_neur_name= fatal_error__("1.c",13269)+fatal_error_ ("Undef mapping");
}
int
get_arrity(KFG_NODE node)
{ 
        switch(sel_mapping) {
                case 1: return(get_arrity1(node));
                case 2: return(get_arrity2(node));
                case 3: return(get_arrity3(node));
        }
        ein_neur_name= fatal_error__("1.c",13280)+fatal_error_ ("Undef mapping");
}
int 
get_mapping(KFG_NODE call, KFG_NODE start, int pos)
{
        switch(sel_mapping) {
                case 1: return(get_mapping1(call,start,pos));
                case 2: return(get_mapping2(call,start,pos));
                case 3: return(get_mapping3(call,start,pos));
        }
        ein_neur_name= fatal_error__("1.c",13292)+fatal_error_ ("Undef mapping");
}
        
char * ulltos(ull x) {
        char *s;
        ull h1,h2;
        s = (char*)malloc(sizeof(char)*64);
        
   h1 = x & (0x00000000);
        h1 = h1 >> 32;
   h2 = x & (0xFFFFFFFF);
        sprintf(s,"(%lu,%lu)",(unsigned long )h1, (unsigned long)h2);
        return(s);
}
static void o_alias_remove_from_list ( o_alias, o_alias_li );
static void o_alias_remove_from_table ( o_alias_li, int );
static o_alias o_alias_update_bloody ( o_alias, unum, o_t1 );
static o_alias _o_alias_dublicate ( o_alias, int  );
static int  o_alias_get_diffs ( o_alias, unum, o_t1 * );
static int o_alias_size ( o_alias );
static o_alias o_alias_change_default ( o_alias, o_t1 );
o_alias o_alias_top;
o_alias o_alias_bottom;
long o_alias_power;
void o_alias_init(void) 
{
   static int  flag = 0 ;
   flag = 1 ;
        unum_init();
        o_t1_init();
        o_alias_top = o_alias_create(o_t1_top);
        o_alias_bottom = o_alias_create(o_t1_bottom);
   o_alias_power = unum_power==0 || o_t1_power==0?0
                :o_t1_power==1 ?1
                :unum_power== -1  || o_t1_power== -1  ? -1  
                :unum_power * o_t1_power;
}
char *o_alias_len(void)
{
        char *s1;
        char *s;
        int len;
        s1 = o_t1_len();
        len = strlen(s1);
        len += 200;
        s = (char *) malloc( len ) ;
        if (s== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",13514)+fatal_error_ ("memory exhausted in `o_alias_len'");
        if (unum_power != -1 ) 
                sprintf(s,"(%s)*%ld",s1,unum_power);
        else
                sprintf(s,"(%s)*unum",s1);
        free((void *) s1 ) ;
        return(s);
}
static void o_alias_remove_from_list(o_alias func, o_alias_li elem)
{
        ((void) (( elem!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   13532 ,  "elem!=NULL" ), 0) )) ;
        ((void) (( func!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   13533 ,  "func!=NULL" ), 0) )) ;
        ((void) (( func->head!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   13534 ,  "func->head!=NULL" ), 0) )) ;
        ((void) (( elem==func->head || (func->head->next != ((void *)0)  && elem->prev != ((void *)0) ) ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   13535 ,  "elem==func->head || (func->head->next != NULL && elem->prev != NULL)" ), 0) )) ;
        if (elem==func->head) {    
                if (elem->next != ((void *)0) )   
                        elem->next->prev = ((void *)0) ;
                func->head = elem->next;
        }
        else {
                elem->prev->next = elem->next;
                if (elem->next != ((void *)0) )
                        elem->next->prev = elem->prev;
        }
        free((void *) elem ) ;
}
void o_alias_destroy(o_alias e)
{
        o_alias_li help;
        o_alias_li old;
        
        help=e->head;
        free((void *) e ) ;
        while(e!= ((void *)0) ) {
                old = help;
                help = help->next;
                free((void *) old ) ;
        }
}
void o_alias_destroy_whole(o_alias e)
{
        o_alias_li help;
        o_alias_li old;
        int i;
        
        help=e->head;
        o_t1_destroy_whole(e->def);
        free((void *) e ) ;
        while(e!= ((void *)0) ) {
                old = help;
                for(i=0;i<help->howmany;i++) {
                        unum_destroy_whole(help->source[i]);
                        o_t1_destroy_whole(help->target[i]);
                }
                help = help->next;
      free((void *) old ) ;
        }
}
static int prime[] = { 2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997};
static int anz_prime=167;
static int *last_prime = &(prime[166]);
unsigned int o_alias_hash(o_alias e) {
        o_alias_li help = e->head;
        int i;
        unsigned int h;
        
        h=2*o_t1_hash(e->def);
   while(help!= ((void *)0) ) {
                        if (help->howmany >anz_prime)
                                ein_neur_name= fatal_error__("1.c",13617)+fatal_error_ ("Too long");
              for(i=0;i<help->howmany;i++) {
         h += prime[2*i+1] *unum_hash(help->source[i]) + prime[2*i+2]* o_t1_hash(help->target[i]);
      }
      help = help->next;
        }
        return(h);
}
static void o_alias_remove_from_table(o_alias_li help,int n)
{
        int anz;
        ((void) (( n>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   13632 ,  "n>=0" ), 0) )) ;
        ((void) (( help!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   13633 ,  "help!=NULL" ), 0) )) ;
        ((void) (( n<help->howmany ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",          "1.c" ,   13634 ,  "n<help->howmany" ), 0) )) ;
        help->howmany-=1;
        anz = help->howmany-n;
        ((void) (( anz>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   13637 ,  "anz>=0" ), 0) )) ;
        if (anz==0) 
                return;
        memmove((void*)((help->source)+n),(void*)((help->source)+n+1)
                ,sizeof(unum)*anz);
        memmove((void*)((help->target)+n),(void*)((help->target)+n+1)
                ,sizeof(o_t1)*anz);
}
        
char * o_alias_print(o_alias func)
{
        o_alias_li help ;
        int i;
        char *s,*q,*w,*s1,*zz;
        int a,len;
        
        len = 1024;
        a = 0;
        s = (char*) malloc( sizeof(char)*len ) ;
        if (s == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",13663)+fatal_error_ ("No space");
        sprintf(s, "[%s:",zz=o_t1_print(func->def));
        free((void *) zz ) ;
        a = strlen(s);
        help = func->head;
        while (help != ((void *)0) ) {
                for (i = 0; i < help->howmany; i++) {
                        q = unum_print(help->source[i]);
                        w = o_t1_print(help->target[i]);
                        a += strlen(q)+ strlen(w)+3;
                        if (a>len-10) {
                                len *= 2;
                                s1 = (char*)malloc( sizeof(char)*len ) ;
                                if (s1 == ((void *)0) )
                                        ein_neur_name= fatal_error__("1.c",13683)+fatal_error_ ("No space");
                                strcpy(s1,s);
                                free((void *) s ) ;
                                s = s1;
                        }
                        strcat(s,q);
                        strcat(s, "->");
                        strcat(s,w);
                        strcat(s,",");
                        free((void *) q ) ;
                        free((void *) w ) ;
                }
                help = help->next;
        }
        strcat(s, "]");
        return(s);
}
char * o_alias_vcgprint(o_alias func)
{
        o_alias_li help ;
        int i;
        char *s,*q,*w,*s1,*zz;
        int a,len;
        
        len = 1024;
        a = 0;
        s = (char*) malloc( sizeof(char)*len ) ;
        if (s == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",13721)+fatal_error_ ("No space");
        sprintf(s, "\\left\\{ \\begin{array}{lcl} \\Join  & \\leadsto & %s \\\\ ",zz=o_t1_vcgprint(func->def));
        free((void *) zz ) ;
        a = strlen(s);
        help = func->head;
        while (help != ((void *)0) ) {
                for (i = 0; i < help->howmany; i++) {
                        q = unum_vcgprint(help->source[i]);
                        w = o_t1_vcgprint(help->target[i]);
                        a += strlen(q)+ strlen(w)+100;
                        if (a>len-10) {
                                len *= 2;
                                s1 = (char*)malloc( sizeof(char)*len ) ;
                                if (s1 == ((void *)0) )
                                        ein_neur_name= fatal_error__("1.c",13741)+fatal_error_ ("No space in vcgprint a function");
                                strcpy(s1,s);
                                free((void *) s ) ;
                                s = s1;
                        }
                        strcat(s,q);
                        strcat(s, " & \\mapsto & ");
                        strcat(s,w);
                        strcat(s," \\\\");
                        free((void *) q ) ;
                        free((void *) w ) ;
                }
                help = help->next;
        }
        strcat(s, "\\end{array} \\right\\}");
        return(s);
}
void o_alias_print_stdout(o_alias func)
{
        char *s;
        
        s = o_alias_print(func);
        fprintf((&_iob[1]) ,"%s",s);
}
static o_alias o_alias_update_bloody(o_alias root, unum s, o_t1 t)
{
        int i;
        o_alias_li free,help;
        ((void) (( root!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   13788 ,  "root!=NULL" ), 0) )) ;
        free = ((void *)0) ;    
        help = root->head;
        while(help!= ((void *)0) ) {
                if (help->howmany!= 100 )
                        free = help;
                for (i=0; i<help->howmany;i++) {
                        if (unum_eq(help->source[i],s)) {
                                if (o_t1_eq(root->def,t)) {
                                        o_alias_remove_from_table(help,i);
                                        if (help->howmany==0)
                                                o_alias_remove_from_list(root,help);
                                        return(root);
                                }
                                help->target[i] = t;
                                return(root);
                        }
                }
                help = help->next;
        }
        if (o_t1_eq(t,root->def)) 
                return(root);
        if (free == ((void *)0) ) {
                { ( free ) = ( o_alias_li )malloc( sizeof(*( free )) ) ; if (( free )== ((void *)0) ) ein_neur_name= fatal_error__("1.c",13819)+fatal_error_ ("Out of memory (malloc)"); }; ;
                free->next = root->head;
                if (root->head != ((void *)0) )
                        root->head->prev = free;
                free->prev = ((void *)0) ;
                free->howmany = 0;
                root->head = free;
        }
        free->source[free->howmany] = s;
        free->target[free->howmany] = t;
        free->howmany += 1;
        return(root);
}
        
                                
o_alias o_alias_update(o_alias root, unum s, o_t1 t)
{
        return(o_alias_update_bloody(o_alias_dublicate(root),s,t));
}
static o_alias _o_alias_dublicate(o_alias root,int  wflag)
{
        o_alias help;
        o_alias_li s= ((void *)0) ,hilf,last;
        int i,x = 100 ;
        help = o_alias_create(root->def);
        hilf = root->head;
        while(hilf!= ((void *)0) ) {
                for (i = 0; i < hilf->howmany; i++) {
                        if ( x == 100 ) {
                                if (s!= ((void *)0) ) s->howmany = 100 ;
                                last = s;
                { ( s ) = ( o_alias_li )malloc( sizeof(*( s )) ) ; if (( s )== ((void *)0) ) ein_neur_name= fatal_error__("1.c",13871)+fatal_error_ ("Out of memory (malloc)"); }; ;
                                s->prev = last;
                                if (last!= ((void *)0) ) last->next = s;
                                else help->head = s;
                                s->next = ((void *)0) ;
                                x = 0;
                        }
                        s->source[x] = wflag ? unum_dublicate_whole(hilf->source[i])
                                :hilf->source[i];
                        s->target[x] = wflag ? o_t1_dublicate_whole(hilf->target[i])
                                :hilf->target[i];
                        x += 1;
                }
                hilf = hilf->next;
        }
        if (s!= ((void *)0) ) s->howmany = x;
        return(help);
}
o_alias o_alias_dublicate(o_alias root)
{
        return(_o_alias_dublicate(root,0 ));
}
                
o_alias o_alias_dublicate_whole(o_alias root)
{
        return(_o_alias_dublicate(root,1 ));
}
                
int  o_alias_eq(o_alias s, o_alias t)
{
        o_alias_li help;
        int num,i,sm,tm;
        o_t1 tar;
        if (unum_power == -1 ) {
                if (!o_t1_eq(s->def,t->def)) {
                        return(0 );
                }
                sm = o_alias_size(s);  
                tm = o_alias_size(t);  
        }
        else {
                sm = o_alias_size(s);  
                tm = o_alias_size(t);  
                ((void) (( unum_power>=sm ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   13934 ,  "unum_power>=sm" ), 0) )) ;
                if (unum_power==sm && unum_power !=tm) 
                        return(o_alias_eq(t,o_alias_change_default(s,t->def)));
                else 
                        if (unum_power==tm && unum_power !=sm) 
                                return(o_alias_eq(s,o_alias_change_default(t,s->def)));
                if (unum_power>sm) 
                        if (!o_t1_eq(s->def,t->def)) {
                                return(0 );
                        }
        }
        if (sm!=tm)  {
                return(0 );
        }
        help = s->head;
        num=0;
        while (help != ((void *)0) ) {
                for (i = 0; i < help->howmany; i++) {
                        if (!o_alias_get_diffs(t,help->source[i],&tar)) {
                                return(0 );
                        }
                        if (!o_t1_eq(tar, help->target[i])) {
                                return(0 );
                        }
                }
                help = help->next;
                num+=1;
        }
        return(1 );
}
static int  o_alias_get_diffs(o_alias f,unum elem,o_t1 *p)
{
        int i;
        o_alias_li help;
        help = f->head;
        while(help!= ((void *)0) ) {
                for(i=0;i<help->howmany;i++) {
                        if (unum_eq(elem,help->source[i])) {
                                *p = help->target[i];
                                return(1 );
                        }
                }
                help = help->next;
        }
        return(0 );
}
static int o_alias_size(o_alias f)
{
        int anz=0;
        o_alias_li help;
        help = f->head;
        while(help!= ((void *)0) ) {
                anz+=help->howmany;
                help=help->next;
        }
        return(anz);
}
                
        
static o_alias o_alias_change_default(o_alias f,o_t1 def)
{
        int i;
        o_alias_li help,p;
        ((void) (( unum_power==o_alias_size(f) ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   14027 ,  "unum_power==o_alias_size(f)" ), 0) )) ;
        f->def = def;
        help = f->head;
        while(help!= ((void *)0) ) {
                for(i=0;i<help->howmany;i++) {
                        if(o_t1_eq(def,help->target[i])) {
                                o_alias_remove_from_table(help,i);
                                i -= 1;
                        }
                }
                p = help;
                help = help->next;
                if (p->howmany==0)
                        o_alias_remove_from_list(f,p);
        }
        return(f);
}
o_alias o_alias_create(o_t1 def)
{
        o_alias help;
        { ( help ) = ( o_alias )malloc( sizeof(*( help )) ) ; if (( help )== ((void *)0) ) ein_neur_name= fatal_error__("1.c",14062)+fatal_error_ ("Out of memory (malloc)"); }; ;
        help->def = def;
        help->head= ((void *)0) ;
        return(help);
}
o_t1 o_alias_get(o_alias f, unum s)
{
        o_t1 help;
                
        if (o_alias_get_diffs(f,s,&help))
                return(help);
        return(f->def);
}
int  o_alias_isbottom(o_alias f)
{
        int i;
        i = o_alias_size(f);
        if (o_t1_isbottom(f->def))
                return(i==0);
        if (!(unum_power!= -1  && i == unum_power))
                return(0 );
        return(o_alias_size(o_alias_change_default(f,o_t1_bottom))==0);
}
int  o_alias_istop(o_alias f)
{
        int i;
        i = o_alias_size(f);
        if (o_t1_istop(f->def))
                return(i==0);
        if (!(unum_power!= -1  && i == unum_power))
                return(0 );
        return(o_alias_size(o_alias_change_default(f,o_t1_top))==0);
}
o_alias o_alias_lub(o_alias f1, o_alias f2)
{
        o_alias help;
        o_alias_li lauf;
        o_t1 ll;
        int i;
        ll = o_t1_lub(f1->def, f2->def);
        help = o_alias_create(ll);
        lauf = f1->head;
        while (lauf != ((void *)0) ) {
                for (i = 0; i < lauf->howmany; i++) {
                        ll = o_alias_get(f2, lauf->source[i]);
                        o_alias_update_bloody(help, lauf->source[i], o_t1_lub(lauf->target[i], ll));
                }
                lauf = lauf->next;
        }
        lauf = f2->head;
        while (lauf != ((void *)0) ) {
                for (i = 0; i < lauf->howmany; i++) {
                        if (!o_alias_get_diffs(f1, lauf->source[i],&ll))
                                o_alias_update_bloody(help, lauf->source[i], o_t1_lub(lauf->target[i],f1->def));
                }
                lauf = lauf->next;
        }
        return(help);
}
o_alias o_alias_glb(o_alias f1, o_alias f2)
{
        o_alias help;
        o_alias_li lauf;
        o_t1 ll;
        int i;
        ll = o_t1_glb(f1->def, f2->def);
        help = o_alias_create(ll);
        lauf = f1->head;
        while (lauf != ((void *)0) ) {
                for (i = 0; i < lauf->howmany; i++) {
                        ll = o_alias_get(f2, lauf->source[i]);
                        o_alias_update_bloody(help, lauf->source[i], o_t1_glb(lauf->target[i], ll));
                }
                lauf = lauf->next;
        }
        lauf = f2->head;
        while (lauf != ((void *)0) ) {
                for (i = 0; i < lauf->howmany; i++) {
                        if (!o_alias_get_diffs(f1, lauf->source[i],&ll))
                                o_alias_update_bloody(help, lauf->source[i], o_t1_glb(lauf->target[i],f1->def));
                }
                lauf = lauf->next;
        }
        return(help);
}
int  o_alias_le(o_alias f1, o_alias f2)
{
        o_alias_li lauf;
        o_t1 ll;
        int i;
        int  erg= 0 ;
        if (unum_power == -1  || unum_power == o_alias_size(f1)
                || unum_power == o_alias_size(f2)) {
                if (! (o_t1_eq( f1->def ,  f2->def ) || o_t1_le( f1->def ,  f2->def )) )
                        return(0 );
                if (o_t1_le(f1->def, f2->def))
                        erg = 1 ;
        }
        lauf = f1->head;
        while (lauf != ((void *)0) ) {
                for (i = 0; i < lauf->howmany; i++) {
                        ll = o_alias_get(f2, lauf->source[i]);
                        if (! (o_t1_eq( lauf->target[i] ,  ll ) || o_t1_le( lauf->target[i] ,  ll )) )
                                return(0 );
                        if (!erg)
                                if (o_t1_le(lauf->target[i], ll))
                                        erg = 1 ;
                }
                lauf = lauf->next;
        }
        lauf = f2->head;
        while (lauf != ((void *)0) ) {
                for (i = 0; i < lauf->howmany; i++) {
                        if (!o_alias_get_diffs(f2, lauf->source[i],&ll)) {
                                if (! (o_t1_eq( lauf->target[i] ,  f1->def ) || o_t1_le( lauf->target[i] ,  f1->def )) )
                                        return(0 );
                                if (!erg)
                                        if (o_t1_le(lauf->target[i], f1->def))
                                                erg = 1 ;
                        }
                }
                lauf = lauf->next;
        }
        return(erg);
}
static void o_const_fkt_remove_from_list ( o_const_fkt, o_const_fkt_li );
static void o_const_fkt_remove_from_table ( o_const_fkt_li, int );
static o_const_fkt o_const_fkt_update_bloody ( o_const_fkt, unum, o_va );
static o_const_fkt _o_const_fkt_dublicate ( o_const_fkt, int  );
static int  o_const_fkt_get_diffs ( o_const_fkt, unum, o_va * );
static int o_const_fkt_size ( o_const_fkt );
static o_const_fkt o_const_fkt_change_default ( o_const_fkt, o_va );
o_const_fkt o_const_fkt_top;
o_const_fkt o_const_fkt_bottom;
long o_const_fkt_power;
void o_const_fkt_init(void) 
{
   static int  flag = 0 ;
   flag = 1 ;
        unum_init();
        o_va_init();
        o_const_fkt_top = o_const_fkt_create(o_va_top);
        o_const_fkt_bottom = o_const_fkt_create(o_va_bottom);
   o_const_fkt_power = unum_power==0 || o_va_power==0?0
                :o_va_power==1 ?1
                :unum_power== -1  || o_va_power== -1  ? -1  
                :unum_power * o_va_power;
}
char *o_const_fkt_len(void)
{
        char *s1;
        char *s;
        int len;
        s1 = o_va_len();
        len = strlen(s1);
        len += 200;
        s = (char *) malloc( len ) ;
        if (s== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",14474)+fatal_error_ ("memory exhausted in `o_const_fkt_len'");
        if (unum_power != -1 ) 
                sprintf(s,"(%s)*%ld",s1,unum_power);
        else
                sprintf(s,"(%s)*unum",s1);
        free((void *) s1 ) ;
        return(s);
}
static void o_const_fkt_remove_from_list(o_const_fkt func, o_const_fkt_li elem)
{
        ((void) (( elem!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   14492 ,  "elem!=NULL" ), 0) )) ;
        ((void) (( func!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   14493 ,  "func!=NULL" ), 0) )) ;
        ((void) (( func->head!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   14494 ,  "func->head!=NULL" ), 0) )) ;
        ((void) (( elem==func->head || (func->head->next != ((void *)0)  && elem->prev != ((void *)0) ) ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   14495 ,  "elem==func->head || (func->head->next != NULL && elem->prev != NULL)" ), 0) )) ;
        if (elem==func->head) {    
                if (elem->next != ((void *)0) )   
                        elem->next->prev = ((void *)0) ;
                func->head = elem->next;
        }
        else {
                elem->prev->next = elem->next;
                if (elem->next != ((void *)0) )
                        elem->next->prev = elem->prev;
        }
        free((void *) elem ) ;
}
void o_const_fkt_destroy(o_const_fkt e)
{
        o_const_fkt_li help;
        o_const_fkt_li old;
        
        help=e->head;
        free((void *) e ) ;
        while(e!= ((void *)0) ) {
                old = help;
                help = help->next;
                free((void *) old ) ;
        }
}
void o_const_fkt_destroy_whole(o_const_fkt e)
{
        o_const_fkt_li help;
        o_const_fkt_li old;
        int i;
        
        help=e->head;
        o_va_destroy_whole(e->def);
        free((void *) e ) ;
        while(e!= ((void *)0) ) {
                old = help;
                for(i=0;i<help->howmany;i++) {
                        unum_destroy_whole(help->source[i]);
                        o_va_destroy_whole(help->target[i]);
                }
                help = help->next;
      free((void *) old ) ;
        }
}
unsigned int o_const_fkt_hash(o_const_fkt e) {
        o_const_fkt_li help = e->head;
        int i;
        unsigned int h;
        
        h=2*o_va_hash(e->def);
   while(help!= ((void *)0) ) {
                        if (help->howmany >anz_prime)
                                ein_neur_name= fatal_error__("1.c",14577)+fatal_error_ ("Too long");
              for(i=0;i<help->howmany;i++) {
         h += prime[2*i+1] *unum_hash(help->source[i]) + prime[2*i+2]* o_va_hash(help->target[i]);
      }
      help = help->next;
        }
        return(h);
}
static void o_const_fkt_remove_from_table(o_const_fkt_li help,int n)
{
        int anz;
        ((void) (( n>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   14592 ,  "n>=0" ), 0) )) ;
        ((void) (( help!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   14593 ,  "help!=NULL" ), 0) )) ;
        ((void) (( n<help->howmany ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",          "1.c" ,   14594 ,  "n<help->howmany" ), 0) )) ;
        help->howmany-=1;
        anz = help->howmany-n;
        ((void) (( anz>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   14597 ,  "anz>=0" ), 0) )) ;
        if (anz==0) 
                return;
        memmove((void*)((help->source)+n),(void*)((help->source)+n+1)
                ,sizeof(unum)*anz);
        memmove((void*)((help->target)+n),(void*)((help->target)+n+1)
                ,sizeof(o_va)*anz);
}
        
char * o_const_fkt_print(o_const_fkt func)
{
        o_const_fkt_li help ;
        int i;
        char *s,*q,*w,*s1,*zz;
        int a,len;
        
        len = 1024;
        a = 0;
        s = (char*) malloc( sizeof(char)*len ) ;
        if (s == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",14623)+fatal_error_ ("No space");
        sprintf(s, "[%s:",zz=o_va_print(func->def));
        free((void *) zz ) ;
        a = strlen(s);
        help = func->head;
        while (help != ((void *)0) ) {
                for (i = 0; i < help->howmany; i++) {
                        q = unum_print(help->source[i]);
                        w = o_va_print(help->target[i]);
                        a += strlen(q)+ strlen(w)+3;
                        if (a>len-10) {
                                len *= 2;
                                s1 = (char*)malloc( sizeof(char)*len ) ;
                                if (s1 == ((void *)0) )
                                        ein_neur_name= fatal_error__("1.c",14643)+fatal_error_ ("No space");
                                strcpy(s1,s);
                                free((void *) s ) ;
                                s = s1;
                        }
                        strcat(s,q);
                        strcat(s, "->");
                        strcat(s,w);
                        strcat(s,",");
                        free((void *) q ) ;
                        free((void *) w ) ;
                }
                help = help->next;
        }
        strcat(s, "]");
        return(s);
}
char * o_const_fkt_vcgprint(o_const_fkt func)
{
        o_const_fkt_li help ;
        int i;
        char *s,*q,*w,*s1,*zz;
        int a,len;
        
        len = 1024;
        a = 0;
        s = (char*) malloc( sizeof(char)*len ) ;
        if (s == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",14681)+fatal_error_ ("No space");
        sprintf(s, "\\left\\{ \\begin{array}{lcl} \\Join  & \\leadsto & %s \\\\ ",zz=o_va_vcgprint(func->def));
        free((void *) zz ) ;
        a = strlen(s);
        help = func->head;
        while (help != ((void *)0) ) {
                for (i = 0; i < help->howmany; i++) {
                        q = unum_vcgprint(help->source[i]);
                        w = o_va_vcgprint(help->target[i]);
                        a += strlen(q)+ strlen(w)+100;
                        if (a>len-10) {
                                len *= 2;
                                s1 = (char*)malloc( sizeof(char)*len ) ;
                                if (s1 == ((void *)0) )
                                        ein_neur_name= fatal_error__("1.c",14701)+fatal_error_ ("No space in vcgprint a function");
                                strcpy(s1,s);
                                free((void *) s ) ;
                                s = s1;
                        }
                        strcat(s,q);
                        strcat(s, " & \\mapsto & ");
                        strcat(s,w);
                        strcat(s," \\\\");
                        free((void *) q ) ;
                        free((void *) w ) ;
                }
                help = help->next;
        }
        strcat(s, "\\end{array} \\right\\}");
        return(s);
}
void o_const_fkt_print_stdout(o_const_fkt func)
{
        char *s;
        
        s = o_const_fkt_print(func);
        fprintf((&_iob[1]) ,"%s",s);
}
static o_const_fkt o_const_fkt_update_bloody(o_const_fkt root, unum s, o_va t)
{
        int i;
        o_const_fkt_li free,help;
        ((void) (( root!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   14748 ,  "root!=NULL" ), 0) )) ;
        free = ((void *)0) ;    
        help = root->head;
        while(help!= ((void *)0) ) {
                if (help->howmany!= 100 )
                        free = help;
                for (i=0; i<help->howmany;i++) {
                        if (unum_eq(help->source[i],s)) {
                                if (o_va_eq(root->def,t)) {
                                        o_const_fkt_remove_from_table(help,i);
                                        if (help->howmany==0)
                                                o_const_fkt_remove_from_list(root,help);
                                        return(root);
                                }
                                help->target[i] = t;
                                return(root);
                        }
                }
                help = help->next;
        }
        if (o_va_eq(t,root->def)) 
                return(root);
        if (free == ((void *)0) ) {
                { ( free ) = ( o_const_fkt_li )malloc( sizeof(*( free )) ) ; if (( free )== ((void *)0) ) ein_neur_name= fatal_error__("1.c",14779)+fatal_error_ ("Out of memory (malloc)"); }; ;
                free->next = root->head;
                if (root->head != ((void *)0) )
                        root->head->prev = free;
                free->prev = ((void *)0) ;
                free->howmany = 0;
                root->head = free;
        }
        free->source[free->howmany] = s;
        free->target[free->howmany] = t;
        free->howmany += 1;
        return(root);
}
        
                                
o_const_fkt o_const_fkt_update(o_const_fkt root, unum s, o_va t)
{
        return(o_const_fkt_update_bloody(o_const_fkt_dublicate(root),s,t));
}
static o_const_fkt _o_const_fkt_dublicate(o_const_fkt root,int  wflag)
{
        o_const_fkt help;
        o_const_fkt_li s= ((void *)0) ,hilf,last;
        int i,x = 100 ;
        help = o_const_fkt_create(root->def);
        hilf = root->head;
        while(hilf!= ((void *)0) ) {
                for (i = 0; i < hilf->howmany; i++) {
                        if ( x == 100 ) {
                                if (s!= ((void *)0) ) s->howmany = 100 ;
                                last = s;
                { ( s ) = ( o_const_fkt_li )malloc( sizeof(*( s )) ) ; if (( s )== ((void *)0) ) ein_neur_name= fatal_error__("1.c",14831)+fatal_error_ ("Out of memory (malloc)"); }; ;
                                s->prev = last;
                                if (last!= ((void *)0) ) last->next = s;
                                else help->head = s;
                                s->next = ((void *)0) ;
                                x = 0;
                        }
                        s->source[x] = wflag ? unum_dublicate_whole(hilf->source[i])
                                :hilf->source[i];
                        s->target[x] = wflag ? o_va_dublicate_whole(hilf->target[i])
                                :hilf->target[i];
                        x += 1;
                }
                hilf = hilf->next;
        }
        if (s!= ((void *)0) ) s->howmany = x;
        return(help);
}
o_const_fkt o_const_fkt_dublicate(o_const_fkt root)
{
        return(_o_const_fkt_dublicate(root,0 ));
}
                
o_const_fkt o_const_fkt_dublicate_whole(o_const_fkt root)
{
        return(_o_const_fkt_dublicate(root,1 ));
}
                
int  o_const_fkt_eq(o_const_fkt s, o_const_fkt t)
{
        o_const_fkt_li help;
        int num,i,sm,tm;
        o_va tar;
        if (unum_power == -1 ) {
                if (!o_va_eq(s->def,t->def)) {
                        return(0 );
                }
                sm = o_const_fkt_size(s);  
                tm = o_const_fkt_size(t);  
        }
        else {
                sm = o_const_fkt_size(s);  
                tm = o_const_fkt_size(t);  
                ((void) (( unum_power>=sm ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   14894 ,  "unum_power>=sm" ), 0) )) ;
                if (unum_power==sm && unum_power !=tm) 
                        return(o_const_fkt_eq(t,o_const_fkt_change_default(s,t->def)));
                else 
                        if (unum_power==tm && unum_power !=sm) 
                                return(o_const_fkt_eq(s,o_const_fkt_change_default(t,s->def)));
                if (unum_power>sm) 
                        if (!o_va_eq(s->def,t->def)) {
                                return(0 );
                        }
        }
        if (sm!=tm)  {
                return(0 );
        }
        help = s->head;
        num=0;
        while (help != ((void *)0) ) {
                for (i = 0; i < help->howmany; i++) {
                        if (!o_const_fkt_get_diffs(t,help->source[i],&tar)) {
                                return(0 );
                        }
                        if (!o_va_eq(tar, help->target[i])) {
                                return(0 );
                        }
                }
                help = help->next;
                num+=1;
        }
        return(1 );
}
static int  o_const_fkt_get_diffs(o_const_fkt f,unum elem,o_va *p)
{
        int i;
        o_const_fkt_li help;
        help = f->head;
        while(help!= ((void *)0) ) {
                for(i=0;i<help->howmany;i++) {
                        if (unum_eq(elem,help->source[i])) {
                                *p = help->target[i];
                                return(1 );
                        }
                }
                help = help->next;
        }
        return(0 );
}
static int o_const_fkt_size(o_const_fkt f)
{
        int anz=0;
        o_const_fkt_li help;
        help = f->head;
        while(help!= ((void *)0) ) {
                anz+=help->howmany;
                help=help->next;
        }
        return(anz);
}
                
        
static o_const_fkt o_const_fkt_change_default(o_const_fkt f,o_va def)
{
        int i;
        o_const_fkt_li help,p;
        ((void) (( unum_power==o_const_fkt_size(f) ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",          "1.c" ,   14987 ,  "unum_power==o_const_fkt_size(f)" ), 0) )) ;
        f->def = def;
        help = f->head;
        while(help!= ((void *)0) ) {
                for(i=0;i<help->howmany;i++) {
                        if(o_va_eq(def,help->target[i])) {
                                o_const_fkt_remove_from_table(help,i);
                                i -= 1;
                        }
                }
                p = help;
                help = help->next;
                if (p->howmany==0)
                        o_const_fkt_remove_from_list(f,p);
        }
        return(f);
}
o_const_fkt o_const_fkt_create(o_va def)
{
        o_const_fkt help;
        { ( help ) = ( o_const_fkt )malloc( sizeof(*( help )) ) ; if (( help )== ((void *)0) ) ein_neur_name= fatal_error__("1.c",15022)+fatal_error_ ("Out of memory (malloc)"); }; ;
        help->def = def;
        help->head= ((void *)0) ;
        return(help);
}
o_va o_const_fkt_get(o_const_fkt f, unum s)
{
        o_va help;
                
        if (o_const_fkt_get_diffs(f,s,&help))
                return(help);
        return(f->def);
}
int  o_const_fkt_isbottom(o_const_fkt f)
{
        int i;
        i = o_const_fkt_size(f);
        if (o_va_isbottom(f->def))
                return(i==0);
        if (!(unum_power!= -1  && i == unum_power))
                return(0 );
        return(o_const_fkt_size(o_const_fkt_change_default(f,o_va_bottom))==0);
}
int  o_const_fkt_istop(o_const_fkt f)
{
        int i;
        i = o_const_fkt_size(f);
        if (o_va_istop(f->def))
                return(i==0);
        if (!(unum_power!= -1  && i == unum_power))
                return(0 );
        return(o_const_fkt_size(o_const_fkt_change_default(f,o_va_top))==0);
}
o_const_fkt o_const_fkt_lub(o_const_fkt f1, o_const_fkt f2)
{
        o_const_fkt help;
        o_const_fkt_li lauf;
        o_va ll;
        int i;
        ll = o_va_lub(f1->def, f2->def);
        help = o_const_fkt_create(ll);
        lauf = f1->head;
        while (lauf != ((void *)0) ) {
                for (i = 0; i < lauf->howmany; i++) {
                        ll = o_const_fkt_get(f2, lauf->source[i]);
                        o_const_fkt_update_bloody(help, lauf->source[i], o_va_lub(lauf->target[i], ll));
                }
                lauf = lauf->next;
        }
        lauf = f2->head;
        while (lauf != ((void *)0) ) {
                for (i = 0; i < lauf->howmany; i++) {
                        if (!o_const_fkt_get_diffs(f1, lauf->source[i],&ll))
                                o_const_fkt_update_bloody(help, lauf->source[i], o_va_lub(lauf->target[i],f1->def));
                }
                lauf = lauf->next;
        }
        return(help);
}
o_const_fkt o_const_fkt_glb(o_const_fkt f1, o_const_fkt f2)
{
        o_const_fkt help;
        o_const_fkt_li lauf;
        o_va ll;
        int i;
        ll = o_va_glb(f1->def, f2->def);
        help = o_const_fkt_create(ll);
        lauf = f1->head;
        while (lauf != ((void *)0) ) {
                for (i = 0; i < lauf->howmany; i++) {
                        ll = o_const_fkt_get(f2, lauf->source[i]);
                        o_const_fkt_update_bloody(help, lauf->source[i], o_va_glb(lauf->target[i], ll));
                }
                lauf = lauf->next;
        }
        lauf = f2->head;
        while (lauf != ((void *)0) ) {
                for (i = 0; i < lauf->howmany; i++) {
                        if (!o_const_fkt_get_diffs(f1, lauf->source[i],&ll))
                                o_const_fkt_update_bloody(help, lauf->source[i], o_va_glb(lauf->target[i],f1->def));
                }
                lauf = lauf->next;
        }
        return(help);
}
int  o_const_fkt_le(o_const_fkt f1, o_const_fkt f2)
{
        o_const_fkt_li lauf;
        o_va ll;
        int i;
        int  erg= 0 ;
        if (unum_power == -1  || unum_power == o_const_fkt_size(f1)
                || unum_power == o_const_fkt_size(f2)) {
                if (! (o_va_eq( f1->def ,  f2->def ) || o_va_le( f1->def ,  f2->def )) )
                        return(0 );
                if (o_va_le(f1->def, f2->def))
                        erg = 1 ;
        }
        lauf = f1->head;
        while (lauf != ((void *)0) ) {
                for (i = 0; i < lauf->howmany; i++) {
                        ll = o_const_fkt_get(f2, lauf->source[i]);
                        if (! (o_va_eq( lauf->target[i] ,  ll ) || o_va_le( lauf->target[i] ,  ll )) )
                                return(0 );
                        if (!erg)
                                if (o_va_le(lauf->target[i], ll))
                                        erg = 1 ;
                }
                lauf = lauf->next;
        }
        lauf = f2->head;
        while (lauf != ((void *)0) ) {
                for (i = 0; i < lauf->howmany; i++) {
                        if (!o_const_fkt_get_diffs(f2, lauf->source[i],&ll)) {
                                if (! (o_va_eq( lauf->target[i] ,  f1->def ) || o_va_le( lauf->target[i] ,  f1->def )) )
                                        return(0 );
                                if (!erg)
                                        if (o_va_le(lauf->target[i], f1->def))
                                                erg = 1 ;
                        }
                }
                lauf = lauf->next;
        }
        return(erg);
}
long o_const_val_power;
char *o_const_val_len(void)
 {
        ein_neur_name= fatal_error__("1.c",15281)+fatal_error_ ("len in disjunktion");
}
void o_const_val_init(void)
 {
        static int  flag = 0 ;
        if(!flag) {
        flag = 1 ;
        bool_init();
        snum_init();
        real_init();
        o_noth_dom_init();
        unum_init();
        o_const_val_power = 
                bool_power== -1 ?-1 :
                snum_power== -1 ?-1 :
                real_power== -1 ?-1 :
                o_noth_dom_power== -1 ?-1 :
                unum_power== -1 ?-1 :
                bool_power+snum_power+real_power+o_noth_dom_power+unum_power ;
}
}
unsigned int o_const_val_hash(o_const_val x) {
        switch(x->flag) {
                case o_const_val_bool_1_flag: return(2*bool_hash((x->elem).bool_1_sel));
                case o_const_val_snum_2_flag: return(3*snum_hash((x->elem).snum_2_sel));
                case o_const_val_real_3_flag: return(5*real_hash((x->elem).real_3_sel));
                case o_const_val_o_noth_dom_4_flag: return(7*o_noth_dom_hash((x->elem).o_noth_dom_4_sel));
                case o_const_val_unum_5_flag: return(11*unum_hash((x->elem).unum_5_sel));
        }
}
o_const_val o_const_val_create_from_1(bool elem)
{
        o_const_val x;
        x = o_const_val_new();
        x->flag = o_const_val_bool_1_flag;
        (x->elem).bool_1_sel = elem;
        return(x);
}
o_const_val o_const_val_create_from_2(snum elem)
{
        o_const_val x;
        x = o_const_val_new();
        x->flag = o_const_val_snum_2_flag;
        (x->elem).snum_2_sel = elem;
        return(x);
}
o_const_val o_const_val_create_from_3(real elem)
{
        o_const_val x;
        x = o_const_val_new();
        x->flag = o_const_val_real_3_flag;
        (x->elem).real_3_sel = elem;
        return(x);
}
o_const_val o_const_val_create_from_4(o_noth_dom elem)
{
        o_const_val x;
        x = o_const_val_new();
        x->flag = o_const_val_o_noth_dom_4_flag;
        (x->elem).o_noth_dom_4_sel = elem;
        return(x);
}
o_const_val o_const_val_create_from_5(unum elem)
{
        o_const_val x;
        x = o_const_val_new();
        x->flag = o_const_val_unum_5_flag;
        (x->elem).unum_5_sel = elem;
        return(x);
}
static o_const_val o_const_val_new(void)
{
        o_const_val help;
        help = (o_const_val) calloc(sizeof(_o_const_val),1);
        if (help== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",15427)+fatal_error_ ("No memory");
        help->flag = o_const_val_none_0_flag;
        return(help);
}
int o_const_val_selector(o_const_val x)
{
        return(x->flag);
}
int  o_const_val_is_1(o_const_val x)
{
        return(x->flag==o_const_val_bool_1_flag);
}
int  o_const_val_is_2(o_const_val x)
{
        return(x->flag==o_const_val_snum_2_flag);
}
int  o_const_val_is_3(o_const_val x)
{
        return(x->flag==o_const_val_real_3_flag);
}
int  o_const_val_is_4(o_const_val x)
{
        return(x->flag==o_const_val_o_noth_dom_4_flag);
}
int  o_const_val_is_5(o_const_val x)
{
        return(x->flag==o_const_val_unum_5_flag);
}
bool o_const_val_down_to_1(o_const_val x)
{
        if (!o_const_val_is_1(x))
                ein_neur_name= fatal_error__("1.c",15493)+fatal_error_ ("o_const_val_down_to_1 applied to wrong argument");
        return((x->elem).bool_1_sel);
}
snum o_const_val_down_to_2(o_const_val x)
{
        if (!o_const_val_is_2(x))
                ein_neur_name= fatal_error__("1.c",15504)+fatal_error_ ("o_const_val_down_to_2 applied to wrong argument");
        return((x->elem).snum_2_sel);
}
real o_const_val_down_to_3(o_const_val x)
{
        if (!o_const_val_is_3(x))
                ein_neur_name= fatal_error__("1.c",15515)+fatal_error_ ("o_const_val_down_to_3 applied to wrong argument");
        return((x->elem).real_3_sel);
}
o_noth_dom o_const_val_down_to_4(o_const_val x)
{
        if (!o_const_val_is_4(x))
                ein_neur_name= fatal_error__("1.c",15526)+fatal_error_ ("o_const_val_down_to_4 applied to wrong argument");
        return((x->elem).o_noth_dom_4_sel);
}
unum o_const_val_down_to_5(o_const_val x)
{
        if (!o_const_val_is_5(x))
                ein_neur_name= fatal_error__("1.c",15537)+fatal_error_ ("o_const_val_down_to_5 applied to wrong argument");
        return((x->elem).unum_5_sel);
}
int  o_const_val_eq(o_const_val x1, o_const_val x2)
{
        if (x1->flag == o_const_val_none_0_flag || x2->flag == o_const_val_none_0_flag)
                ein_neur_name= fatal_error__("1.c",15581)+fatal_error_ ("o_const_val_eq auf undef wert");
        if (x1->flag!=x2->flag)
                return(0 );
        switch(x1->flag) {
                case o_const_val_bool_1_flag:
                        return(bool_eq((x1->elem).bool_1_sel,(x2->elem).bool_1_sel));
                case o_const_val_snum_2_flag:
                        return(snum_eq((x1->elem).snum_2_sel,(x2->elem).snum_2_sel));
                case o_const_val_real_3_flag:
                        return(real_eq((x1->elem).real_3_sel,(x2->elem).real_3_sel));
                case o_const_val_o_noth_dom_4_flag:
                        return(o_noth_dom_eq((x1->elem).o_noth_dom_4_sel,(x2->elem).o_noth_dom_4_sel));
                case o_const_val_unum_5_flag:
                        return(unum_eq((x1->elem).unum_5_sel,(x2->elem).unum_5_sel));
                default:
                        ein_neur_name= fatal_error__("1.c",15596)+fatal_error_ ("Unkown case");
                        return(0);
        }
}
static o_const_val _o_const_val_dublicate(o_const_val x,int  whole_mode)
{
        o_const_val erg;
        erg = o_const_val_new();
        erg->flag = x->flag;
        switch(x->flag) {
                case o_const_val_bool_1_flag:
                        (erg->elem).bool_1_sel = whole_mode?bool_dublicate_whole((x->elem).bool_1_sel):(x->elem).bool_1_sel;
                        break;
                case o_const_val_snum_2_flag:
                        (erg->elem).snum_2_sel = whole_mode?snum_dublicate_whole((x->elem).snum_2_sel):(x->elem).snum_2_sel;
                        break;
                case o_const_val_real_3_flag:
                        (erg->elem).real_3_sel = whole_mode?real_dublicate_whole((x->elem).real_3_sel):(x->elem).real_3_sel;
                        break;
                case o_const_val_o_noth_dom_4_flag:
                        (erg->elem).o_noth_dom_4_sel = whole_mode?o_noth_dom_dublicate_whole((x->elem).o_noth_dom_4_sel):(x->elem).o_noth_dom_4_sel;
                        break;
                case o_const_val_unum_5_flag:
                        (erg->elem).unum_5_sel = whole_mode?unum_dublicate_whole((x->elem).unum_5_sel):(x->elem).unum_5_sel;
                        break;
                default:
                        ein_neur_name= fatal_error__("1.c",15628)+fatal_error_ ("Unkown case");
        }
        return(erg);
}
o_const_val o_const_val_dublicate_whole(o_const_val x)
{
         return(_o_const_val_dublicate(x,1 ));
}
o_const_val o_const_val_dublicate(o_const_val x)
{
         return(_o_const_val_dublicate(x,0 ));
}
void o_const_val_print_stdout(o_const_val x)
{
        char *my_str;
        my_str = o_const_val_print(x);
        fprintf((&_iob[1]) ,"%s",my_str);
        free((void *) my_str ) ;
}
char *o_const_val_print(o_const_val x)
{
        switch(x->flag) {
                case o_const_val_bool_1_flag:
                        return(bool_print((x->elem).bool_1_sel));
                        break;
                case o_const_val_snum_2_flag:
                        return(snum_print((x->elem).snum_2_sel));
                        break;
                case o_const_val_real_3_flag:
                        return(real_print((x->elem).real_3_sel));
                        break;
                case o_const_val_o_noth_dom_4_flag:
                        return(o_noth_dom_print((x->elem).o_noth_dom_4_sel));
                        break;
                case o_const_val_unum_5_flag:
                        return(unum_print((x->elem).unum_5_sel));
                        break;
                default:
                        ein_neur_name= fatal_error__("1.c",15678)+fatal_error_ ("Unkown case");
                        return(0);
        }
}
char *o_const_val_vcgprint(o_const_val x)
{
char *help,*s;int len;
        switch(x->flag) {
                case o_const_val_bool_1_flag:
                        help=(bool_vcgprint((x->elem).bool_1_sel));
                        break;
                case o_const_val_snum_2_flag:
                        help=(snum_vcgprint((x->elem).snum_2_sel));
                        break;
                case o_const_val_real_3_flag:
                        help=(real_vcgprint((x->elem).real_3_sel));
                        break;
                case o_const_val_o_noth_dom_4_flag:
                        help=(o_noth_dom_vcgprint((x->elem).o_noth_dom_4_sel));
                        break;
                case o_const_val_unum_5_flag:
                        help=(unum_vcgprint((x->elem).unum_5_sel));
                        break;
                default:
                        ein_neur_name= fatal_error__("1.c",15705)+fatal_error_ ("Unkown case");
                        return(0);
        }
len = 40+strlen(help);
        s = (char*)malloc( len ) ;
sprintf(s,"\\left[ %s \\right]",help);
return(s);
}
void o_const_val_destroy(o_const_val x)
{
        free((void *) x ) ;
}
void o_const_val_destroy_whole(o_const_val x)
{
        switch(x->flag) {
                case o_const_val_bool_1_flag:
                        bool_destroy_whole((x->elem).bool_1_sel);
                        break;
                case o_const_val_snum_2_flag:
                        snum_destroy_whole((x->elem).snum_2_sel);
                        break;
                case o_const_val_real_3_flag:
                        real_destroy_whole((x->elem).real_3_sel);
                        break;
                case o_const_val_o_noth_dom_4_flag:
                        o_noth_dom_destroy_whole((x->elem).o_noth_dom_4_sel);
                        break;
                case o_const_val_unum_5_flag:
                        unum_destroy_whole((x->elem).unum_5_sel);
                        break;
                default:
                        ein_neur_name= fatal_error__("1.c",15750)+fatal_error_ ("Unkown case");
        }
        free((void *) x ) ;
}
int  o_const_val_le(o_const_val x1, o_const_val x2)
{
        if (x1->flag!=x2->flag)
                return(0 );
        switch(x1->flag) {
                case o_const_val_bool_1_flag:
                        return(((bool)ein_neur_name== fatal_error__("1.c",15771)+fatal_error_ ("No order on bool")) );
                case o_const_val_snum_2_flag:
                        return(snum_le((x1->elem).snum_2_sel,(x2->elem).snum_2_sel));
                case o_const_val_real_3_flag:
                        return(real_le((x1->elem).real_3_sel,(x2->elem).real_3_sel));
                case o_const_val_o_noth_dom_4_flag:
                        return(o_noth_dom_le((x1->elem).o_noth_dom_4_sel,(x2->elem).o_noth_dom_4_sel));
                case o_const_val_unum_5_flag:
                        return(unum_le((x1->elem).unum_5_sel,(x2->elem).unum_5_sel));
                default:
                        ein_neur_name= fatal_error__("1.c",15781)+fatal_error_ ("Unkown case");
                        return(0);
        }
}
static o_ddfi o_ddfi_new(void) ;
o_ddfi o_ddfi_top,o_ddfi_bottom;
long o_ddfi_power; 
unsigned int o_ddfi_hash(o_ddfi x) {
   switch(x->flag) {
      case o_ddfi_top_class : return(0);
      case o_ddfi_bottom_class: return(1);
      default: return(o_dfi_hash(x->inh)+2);
   }
}
void o_ddfi_init(void) 
{
        static int  flag = 0 ;
        if (!flag) {
                flag = 1 ;
                o_dfi_init();
                o_ddfi_power = o_dfi_power== -1 ? -1 :o_dfi_power+2;
                o_ddfi_top =  o_ddfi_new();
                o_ddfi_bottom = o_ddfi_new();
                o_ddfi_top->flag = o_ddfi_top_class;
                o_ddfi_bottom->flag = o_ddfi_bottom_class;
        }
}
static o_ddfi o_ddfi_new(void)
{
        o_ddfi help;
        help = (o_ddfi) malloc( sizeof(_o_ddfi) ) ;
        if (help == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",15908)+fatal_error_ ("No memeory!");
        return(help);
}
char *o_ddfi_len(void)
{
        char *s1,*s2;
        int len;
        s1 = o_dfi_len();
        len = strlen(s1);
        len += 5;
        s2 = (char*) malloc( len ) ;
        if (s2== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",15958)+fatal_error_ ("Out of memory");
        sprintf(s2,"(%s)+2",s1);
        free((void *) s1 ) ;
        return(s2);
}
        
void o_ddfi_destroy(o_ddfi help) 
{
        ((void) (( help!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   15988 ,  "help!=NULL" ), 0) )) ;
        free((void *) help ) ;
}
void o_ddfi_destroy_whole(o_ddfi help) 
{
        if (help->flag == o_ddfi_norm_class)
                o_dfi_destroy_whole(help->inh);
        free((void *) help ) ;
}
o_ddfi o_ddfi_dublicate_whole(o_ddfi help) 
{
        o_ddfi help1;
        help1 = o_ddfi_new();
        help1->flag = help->flag;
        if (help1->flag == o_ddfi_norm_class)
                help1->inh = o_dfi_dublicate_whole(help->inh);
        return(help1);
}
o_ddfi o_ddfi_dublicate(o_ddfi help) 
{
        o_ddfi help1;
        help1 = o_ddfi_new();
        help1->flag = help->flag;
        if (help1->flag == o_ddfi_norm_class)
                help1->inh = help->inh;
        return(help1);
}
char *o_ddfi_print(o_ddfi elem) 
{
        char *s;
        switch(elem->flag) { 
                case o_ddfi_top_class: 
                        s = (char*) malloc( sizeof(char)*5 ) ;
                        sprintf(s,"TOP");
                        break;
                case o_ddfi_bottom_class: 
                        s = (char*) malloc( sizeof(char)*8 ) ;
                        sprintf(s,"BOT");
                        break;
                case o_ddfi_norm_class:  
                        s= o_dfi_print(elem->inh); 
                        break;
                default: 
                        ein_neur_name= fatal_error__("1.c",16082)+fatal_error_ ("Unknown case in o_ddfi_print");
        }
        return(s);
}
char *o_ddfi_vcgprint(o_ddfi elem)
{
        char *s;
        switch(elem->flag) {
                case o_ddfi_top_class:
                        s = (char*) malloc( sizeof(char)*5 ) ;
                        sprintf(s,"\\top");
                        break;
                case o_ddfi_bottom_class:
                        s = (char*) malloc( sizeof(char)*8 ) ;
                        sprintf(s,"\\bot");
                        break;
                case o_ddfi_norm_class:
                        s= o_dfi_vcgprint(elem->inh);
                        break;
                default:
                        ein_neur_name= fatal_error__("1.c",16115)+fatal_error_ ("Unknown case in o_ddfi_vcgprint");
        }
        return(s);
}
void o_ddfi_print_stdout(o_ddfi elem)
{
        char *s;
        s = o_ddfi_print(elem);
        fprintf((&_iob[1]) ,"%s",s);
        free((void *) s ) ;
}
int  o_ddfi_isbottom(o_ddfi ob) 
{
        return(ob->flag == o_ddfi_bottom_class); 
}
int  o_ddfi_istop(o_ddfi ob) 
{
        return(ob->flag == o_ddfi_top_class);
}
int  o_ddfi_eq(o_ddfi s,o_ddfi t) 
{
        if (s->flag != t->flag)
                return(0 );
        if (s->flag == o_ddfi_norm_class)
                return(o_dfi_eq(s->inh,t->inh));
        return(1 );
}
int  o_ddfi_neq(o_ddfi e1, o_ddfi e2)
{
        return(!o_ddfi_eq(e1,e2));
}
o_ddfi o_ddfi_lift(o_dfi inh) 
{
        o_ddfi help;
        help = o_ddfi_new();
        help->inh = inh;
        help->flag = o_ddfi_norm_class;
        return(help);
}
o_ddfi o_ddfi_lub(o_ddfi e1, o_ddfi e2) 
{
        switch(e1->flag) {
                case o_ddfi_bottom_class:
                        return(e2);
                case o_ddfi_norm_class:
                        switch(e2->flag) {
                                case o_ddfi_bottom_class:
                                        return(e1);
                                case o_ddfi_norm_class:
                                        return(o_ddfi_lift(o_dfi_lub(e1->inh,e2->inh)));
                                case o_ddfi_top_class:
                                        return(e2);
                        }
                        ((void) (( 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   16224 ,  "0" ), 0) )) ;
                        return(((void *)0) );
                case o_ddfi_top_class:
                        return(e1);
        }
        ((void) (( 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   16229 ,  "0" ), 0) )) ;
        return(((void *)0) );
}
o_ddfi o_ddfi_glb(o_ddfi e1, o_ddfi e2) 
{
        switch(e1->flag) {
                case o_ddfi_bottom_class:
                        return(e1);
                case o_ddfi_norm_class:
                        switch(e2->flag) {
                                case o_ddfi_bottom_class:
                                        return(e2);
                                case o_ddfi_norm_class:
                                        return(o_ddfi_lift(o_dfi_glb(e1->inh,e2->inh)));
                                case o_ddfi_top_class:
                                        return(e1);
                        }
                        ((void) (( 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   16253 ,  "0" ), 0) )) ;
                        return(((void *)0) );
                case o_ddfi_top_class:
                        return(e2);
        }
        ((void) (( 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   16258 ,  "0" ), 0) )) ;
        return(((void *)0) );
}
int  o_ddfi_le(o_ddfi e1, o_ddfi e2) 
{
        switch(e1->flag) {
                case o_ddfi_bottom_class:
                        return(1 );
                case o_ddfi_norm_class:
                        switch(e2->flag) {
                                case o_ddfi_bottom_class:
                                        return(0 );
                                case o_ddfi_norm_class:
                                        return(o_dfi_le(e1->inh,e2->inh));
                                case o_ddfi_top_class:
                                        return(1 );
                        }
                        ((void) (( 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   16282 ,  "0" ), 0) )) ;
                        return(0 );
                case o_ddfi_top_class:
                        return(0 );
        }
        ((void) (( 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   16287 ,  "0" ), 0) )) ;
        return(0 );
}
o_dfi o_ddfi_drop(o_ddfi elem)
{
        switch(elem->flag) {
                case o_ddfi_top_class:
                case o_ddfi_bottom_class:
                        ein_neur_name= fatal_error__("1.c",16306)+fatal_error_ ("Drop auf top oder bottom angewandt (o_ddfi)");
                case o_ddfi_norm_class:
                        return(elem->inh);
                default:
                        ein_neur_name= fatal_error__("1.c",16310)+fatal_error_ ("Unknown case in o_ddfi_drop");
        }
        return(0);
}
o_dfi o_dfi_top;
o_dfi o_dfi_bottom;
long o_dfi_power;
void o_dfi_init(void)
{
        static int  flag = 0 ;
        if (!flag) {;
        flag = 1 ;
        o_const_fkt_init();
        o_va_init();
        o_const_fkt_init();
        o_alias_init();
        o_alias_init();
        o_dfi_top = o_dfi_create(o_const_fkt_top,o_va_top,o_const_fkt_top,o_alias_top,o_alias_top );
        o_dfi_bottom = o_dfi_create(o_const_fkt_bottom,o_va_bottom,o_const_fkt_bottom,o_alias_bottom,o_alias_bottom );
        o_dfi_power = 
                o_const_fkt_power== -1 ?-1 :
                o_va_power== -1 ?-1 :
                o_const_fkt_power== -1 ?-1 :
                o_alias_power== -1 ?-1 :
                o_alias_power== -1 ?-1 :
                o_const_fkt_power *o_va_power *o_const_fkt_power *o_alias_power *o_alias_power *1;
}
}
unsigned int o_dfi_hash(o_dfi x) {
        unsigned int h=0;
        h += o_const_fkt_hash(x->o_const_fkt_1_comp)*2;
        h += o_va_hash(x->o_va_2_comp)*3;
        h += o_const_fkt_hash(x->o_const_fkt_3_comp)*5;
        h += o_alias_hash(x->o_alias_4_comp)*7;
        h += o_alias_hash(x->o_alias_5_comp)*11;
        return(h);
}
char *o_dfi_len(void)
{
        int len=0;
        char *s1;
        char *s2;
        char *s3;
        char *s4;
        char *s5;
        char *s='\0';
        s1 = o_const_fkt_len();
        len += strlen(s1)+3;
        s2 = o_va_len();
        len += strlen(s2)+3;
        s3 = o_const_fkt_len();
        len += strlen(s3)+3;
        s4 = o_alias_len();
        len += strlen(s4)+3;
        s5 = o_alias_len();
        len += strlen(s5)+3;
        s = (char*)malloc( len ) ;
        if (s== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",16488)+fatal_error_ ("memory");
        sprintf(s,"%s (%s)",s,s1);
        free((void *) s1 ) ;
        sprintf(s,"%s*(%s)",s,s2);
        free((void *) s2 ) ;
        sprintf(s,"%s*(%s)",s,s3);
        free((void *) s3 ) ;
        sprintf(s,"%s*(%s)",s,s4);
        free((void *) s4 ) ;
        sprintf(s,"%s*(%s)",s,s5);
        free((void *) s5 ) ;
        return(s);
}
o_dfi o_dfi_create(o_const_fkt c1, o_va c2, o_const_fkt c3, o_alias c4, o_alias c5  )
{
        o_dfi x;
        x = o_dfi_new();
        x->o_const_fkt_1_comp = c1;
        x->o_va_2_comp = c2;
        x->o_const_fkt_3_comp = c3;
        x->o_alias_4_comp = c4;
        x->o_alias_5_comp = c5;
        return(x);
}
static o_dfi o_dfi_new(void)
{
        o_dfi help;
        help = (o_dfi) calloc(sizeof(_o_dfi),1);
        if (help== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",16539)+fatal_error_ ("No memory");
        return(help);
}
o_dfi o_dfi_update_1(o_dfi tup, o_const_fkt x)
{
        o_dfi help;
        help = o_dfi_dublicate(tup);
        help->o_const_fkt_1_comp = x;
        return(help);
}
o_dfi o_dfi_update_2(o_dfi tup, o_va x)
{
        o_dfi help;
        help = o_dfi_dublicate(tup);
        help->o_va_2_comp = x;
        return(help);
}
o_dfi o_dfi_update_3(o_dfi tup, o_const_fkt x)
{
        o_dfi help;
        help = o_dfi_dublicate(tup);
        help->o_const_fkt_3_comp = x;
        return(help);
}
o_dfi o_dfi_update_4(o_dfi tup, o_alias x)
{
        o_dfi help;
        help = o_dfi_dublicate(tup);
        help->o_alias_4_comp = x;
        return(help);
}
o_dfi o_dfi_update_5(o_dfi tup, o_alias x)
{
        o_dfi help;
        help = o_dfi_dublicate(tup);
        help->o_alias_5_comp = x;
        return(help);
}
o_const_fkt o_dfi_select_1(o_dfi tup)
{
        return(tup->o_const_fkt_1_comp);
}
o_va o_dfi_select_2(o_dfi tup)
{
        return(tup->o_va_2_comp);
}
o_const_fkt o_dfi_select_3(o_dfi tup)
{
        return(tup->o_const_fkt_3_comp);
}
o_alias o_dfi_select_4(o_dfi tup)
{
        return(tup->o_alias_4_comp);
}
o_alias o_dfi_select_5(o_dfi tup)
{
        return(tup->o_alias_5_comp);
}
int  o_dfi_eq(o_dfi x1, o_dfi x2)
{
        if ((!o_const_fkt_eq( x1->o_const_fkt_1_comp ,  x2->o_const_fkt_1_comp )) ) {
                        return(0 );
                }
        if (o_va_neq(x1->o_va_2_comp, x2->o_va_2_comp)) {
                        return(0 );
                }
        if ((!o_const_fkt_eq( x1->o_const_fkt_3_comp ,  x2->o_const_fkt_3_comp )) ) {
                        return(0 );
                }
        if ((!o_alias_eq( x1->o_alias_4_comp ,  x2->o_alias_4_comp )) ) {
                        return(0 );
                }
        if ((!o_alias_eq( x1->o_alias_5_comp ,  x2->o_alias_5_comp )) ) {
                        return(0 );
                }
        return(1 );
}
o_dfi o_dfi_dublicate(o_dfi x)
{
        return(_o_dfi_dublicate(x,0 ));
}
o_dfi o_dfi_dublicate_whole(o_dfi x)
{
        return(_o_dfi_dublicate(x,1 ));
}
static o_dfi _o_dfi_dublicate(o_dfi x,int  whole_mode)
{
        o_dfi erg;
        erg = o_dfi_new();
        erg->o_const_fkt_1_comp = whole_mode?o_const_fkt_dublicate_whole(x->o_const_fkt_1_comp):x->o_const_fkt_1_comp;
        erg->o_va_2_comp = whole_mode?o_va_dublicate_whole(x->o_va_2_comp):x->o_va_2_comp;
        erg->o_const_fkt_3_comp = whole_mode?o_const_fkt_dublicate_whole(x->o_const_fkt_3_comp):x->o_const_fkt_3_comp;
        erg->o_alias_4_comp = whole_mode?o_alias_dublicate_whole(x->o_alias_4_comp):x->o_alias_4_comp;
        erg->o_alias_5_comp = whole_mode?o_alias_dublicate_whole(x->o_alias_5_comp):x->o_alias_5_comp;
        return(erg);
}
void o_dfi_print_stdout(o_dfi x)
{
        char *my_str;
        my_str = o_dfi_print(x);
        fprintf((&_iob[1]) ,"%s",my_str);
        free((void *) my_str ) ;
}
char *o_dfi_print(o_dfi x)
{
        char *s1;
        char *s2;
        char *s3;
        char *s4;
        char *s5;
        char *s;
        int len =4;
        s1 = o_const_fkt_print(x->o_const_fkt_1_comp);
        len += strlen(s1)+2;
        s2 = o_va_print(x->o_va_2_comp);
        len += strlen(s2)+2;
        s3 = o_const_fkt_print(x->o_const_fkt_3_comp);
        len += strlen(s3)+2;
        s4 = o_alias_print(x->o_alias_4_comp);
        len += strlen(s4)+2;
        s5 = o_alias_print(x->o_alias_5_comp);
        len += strlen(s5)+2;
        s = malloc( sizeof(char)*len ) ;
        if (s == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",16762)+fatal_error_ ("No_memory");
        sprintf(s,"(");
        strcat(s,s1);
        free((void *) s1 ) ;
        strcat(s,",");
        strcat(s,s2);
        free((void *) s2 ) ;
        strcat(s,",");
        strcat(s,s3);
        free((void *) s3 ) ;
        strcat(s,",");
        strcat(s,s4);
        free((void *) s4 ) ;
        strcat(s,",");
        strcat(s,s5);
        free((void *) s5 ) ;
        strcat(s,")");
        return(s);
}
char *o_dfi_vcgprint(o_dfi x)
{
        char *s1;
        char *s2;
        char *s3;
        char *s4;
        char *s5;
        char *s;
        int len =60;
        s1 = o_const_fkt_vcgprint(x->o_const_fkt_1_comp);
        len += strlen(s1)+2;
        s2 = o_va_vcgprint(x->o_va_2_comp);
        len += strlen(s2)+2;
        s3 = o_const_fkt_vcgprint(x->o_const_fkt_3_comp);
        len += strlen(s3)+2;
        s4 = o_alias_vcgprint(x->o_alias_4_comp);
        len += strlen(s4)+2;
        s5 = o_alias_vcgprint(x->o_alias_5_comp);
        len += strlen(s5)+2;
        s = malloc( sizeof(char)*len ) ;
        if (s == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",16820)+fatal_error_ ("No_memory");
        sprintf(s,"\\left(");
        strcat(s,"\\begin{array}{|c|}\n");
        strcat(s,"\\hline");
        strcat(s,s1);
        free((void *) s1 ) ;
        strcat(s,"\\\\  \\hline ");
        strcat(s,s2);
        free((void *) s2 ) ;
        strcat(s,"\\\\  \\hline ");
        strcat(s,s3);
        free((void *) s3 ) ;
        strcat(s,"\\\\  \\hline ");
        strcat(s,s4);
        free((void *) s4 ) ;
        strcat(s,"\\\\  \\hline ");
        strcat(s,s5);
        free((void *) s5 ) ;
        strcat(s,"\\\\ \\hline");
        strcat(s,"\\end{array}");
        strcat(s,"\\right)");
        return(s);
}
void o_dfi_destroy(o_dfi x)
{
        free((void *) x ) ;
}
void o_dfi_destroy_whole(o_dfi x)
{
        o_const_fkt_destroy_whole(x->o_const_fkt_1_comp);
        o_va_destroy_whole(x->o_va_2_comp);
        o_const_fkt_destroy_whole(x->o_const_fkt_3_comp);
        o_alias_destroy_whole(x->o_alias_4_comp);
        o_alias_destroy_whole(x->o_alias_5_comp);
        free((void *) x ) ;
}
int  o_dfi_le(o_dfi x1, o_dfi x2)
{
        int  less_found = 0 ;
        if (less_found) {
                if (! (o_const_fkt_le( x1->o_const_fkt_1_comp ,  x2->o_const_fkt_1_comp ) || o_const_fkt_eq( x1->o_const_fkt_1_comp ,  x2->o_const_fkt_1_comp )) )
                        return(0 );
        } else {
                if (o_const_fkt_le(x1->o_const_fkt_1_comp, x2->o_const_fkt_1_comp)) {
                        less_found = 1 ;
                } else {
                        if ((!o_const_fkt_eq( x1->o_const_fkt_1_comp ,  x2->o_const_fkt_1_comp )) )
                                return(0 );
                }
        }
        if (less_found) {
                if (! (o_va_eq( x1->o_va_2_comp ,  x2->o_va_2_comp ) || o_va_le( x1->o_va_2_comp ,  x2->o_va_2_comp )) )
                        return(0 );
        } else {
                if (o_va_le(x1->o_va_2_comp, x2->o_va_2_comp)) {
                        less_found = 1 ;
                } else {
                        if (o_va_neq(x1->o_va_2_comp, x2->o_va_2_comp))
                                return(0 );
                }
        }
        if (less_found) {
                if (! (o_const_fkt_le( x1->o_const_fkt_3_comp ,  x2->o_const_fkt_3_comp ) || o_const_fkt_eq( x1->o_const_fkt_3_comp ,  x2->o_const_fkt_3_comp )) )
                        return(0 );
        } else {
                if (o_const_fkt_le(x1->o_const_fkt_3_comp, x2->o_const_fkt_3_comp)) {
                        less_found = 1 ;
                } else {
                        if ((!o_const_fkt_eq( x1->o_const_fkt_3_comp ,  x2->o_const_fkt_3_comp )) )
                                return(0 );
                }
        }
        if (less_found) {
                if (! (o_alias_le( x1->o_alias_4_comp ,  x2->o_alias_4_comp ) || o_alias_eq( x1->o_alias_4_comp ,  x2->o_alias_4_comp )) )
                        return(0 );
        } else {
                if (o_alias_le(x1->o_alias_4_comp, x2->o_alias_4_comp)) {
                        less_found = 1 ;
                } else {
                        if ((!o_alias_eq( x1->o_alias_4_comp ,  x2->o_alias_4_comp )) )
                                return(0 );
                }
        }
        if (less_found) {
                if (! (o_alias_le( x1->o_alias_5_comp ,  x2->o_alias_5_comp ) || o_alias_eq( x1->o_alias_5_comp ,  x2->o_alias_5_comp )) )
                        return(0 );
        } else {
                if (o_alias_le(x1->o_alias_5_comp, x2->o_alias_5_comp)) {
                        less_found = 1 ;
                } else {
                        if ((!o_alias_eq( x1->o_alias_5_comp ,  x2->o_alias_5_comp )) )
                                return(0 );
                }
        }
        return(less_found);
}
int  o_dfi_leq(o_dfi x1, o_dfi x2)
{
        if (! (o_const_fkt_le( x1->o_const_fkt_1_comp ,  x2->o_const_fkt_1_comp ) || o_const_fkt_eq( x1->o_const_fkt_1_comp ,  x2->o_const_fkt_1_comp )) )
                        return(0 );
        if (! (o_va_eq( x1->o_va_2_comp ,  x2->o_va_2_comp ) || o_va_le( x1->o_va_2_comp ,  x2->o_va_2_comp )) )
                        return(0 );
        if (! (o_const_fkt_le( x1->o_const_fkt_3_comp ,  x2->o_const_fkt_3_comp ) || o_const_fkt_eq( x1->o_const_fkt_3_comp ,  x2->o_const_fkt_3_comp )) )
                        return(0 );
        if (! (o_alias_le( x1->o_alias_4_comp ,  x2->o_alias_4_comp ) || o_alias_eq( x1->o_alias_4_comp ,  x2->o_alias_4_comp )) )
                        return(0 );
        if (! (o_alias_le( x1->o_alias_5_comp ,  x2->o_alias_5_comp ) || o_alias_eq( x1->o_alias_5_comp ,  x2->o_alias_5_comp )) )
                        return(0 );
        return(1 );
}
o_dfi o_dfi_glb(o_dfi x1, o_dfi x2)
{
        o_dfi erg;
        erg = o_dfi_new();
        erg->o_const_fkt_1_comp = o_const_fkt_glb(x1->o_const_fkt_1_comp, x2->o_const_fkt_1_comp);
        erg->o_va_2_comp = o_va_glb(x1->o_va_2_comp, x2->o_va_2_comp);
        erg->o_const_fkt_3_comp = o_const_fkt_glb(x1->o_const_fkt_3_comp, x2->o_const_fkt_3_comp);
        erg->o_alias_4_comp = o_alias_glb(x1->o_alias_4_comp, x2->o_alias_4_comp);
        erg->o_alias_5_comp = o_alias_glb(x1->o_alias_5_comp, x2->o_alias_5_comp);
        return(erg);
}
o_dfi o_dfi_lub(o_dfi x1, o_dfi x2)
{
        o_dfi erg;
        erg = o_dfi_new();
        erg->o_const_fkt_1_comp = o_const_fkt_lub(x1->o_const_fkt_1_comp, x2->o_const_fkt_1_comp);
        erg->o_va_2_comp = o_va_lub(x1->o_va_2_comp, x2->o_va_2_comp);
        erg->o_const_fkt_3_comp = o_const_fkt_lub(x1->o_const_fkt_3_comp, x2->o_const_fkt_3_comp);
        erg->o_alias_4_comp = o_alias_lub(x1->o_alias_4_comp, x2->o_alias_4_comp);
        erg->o_alias_5_comp = o_alias_lub(x1->o_alias_5_comp, x2->o_alias_5_comp);
        return(erg);
}
int  o_dfi_istop(o_dfi x1)
{
return(
        o_const_fkt_istop(x1->o_const_fkt_1_comp) &&
        o_va_istop(x1->o_va_2_comp) &&
        o_const_fkt_istop(x1->o_const_fkt_3_comp) &&
        o_alias_istop(x1->o_alias_4_comp) &&
        o_alias_istop(x1->o_alias_5_comp) 
        );
}
int  o_dfi_isbottom(o_dfi x1)
{
return(
        o_const_fkt_isbottom(x1->o_const_fkt_1_comp) &&
        o_va_isbottom(x1->o_va_2_comp) &&
        o_const_fkt_isbottom(x1->o_const_fkt_3_comp) &&
        o_alias_isbottom(x1->o_alias_4_comp) &&
        o_alias_isbottom(x1->o_alias_5_comp) 
        );
}
static o_edges_kante mg[] = {
{o_edges_dummy,o_edges_dummy}
};
o_edges o_edges_top = o_tt;
o_edges o_edges_bottom = o_ff;
long o_edges_power = 3;
void o_edges_init(void)
{
}
unsigned int o_edges_hash(o_edges x)
{
         return((unsigned int)x);
}
static char string[]="(o_edges)";
char *o_edges_len(void)
{
        return(string);
}
int  o_edges_istop(o_edges x) {
        return(x == o_tt);
}
int  o_edges_isbottom(o_edges x) {
        return(x == o_ff);
}
static o_edges_list o_edges_in_1_step(o_edges x,o_edges_direction d)
{
        int i;
        o_edges_list l;
        ((void) (( x<3 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   17140 ,  "x<3" ), 0) )) ;
        ((void) (( d==o_edges_forward || d==o_edges_backward ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   17141 ,  "d==o_edges_forward || d==o_edges_backward" ), 0) )) ;
        l = o_edges_list_new();
        for(i=0;i<0;i++) {
                if ((d == o_edges_forward && mg[i].von == x) ||
                        (d == o_edges_backward && mg[i].nach == x))
                        l = o_edges_list_extend(d == o_edges_forward ?mg[i].nach:mg[i].von,l);
        }
        return(l);
}
static o_edges_list o_edges_in_n_step(o_edges x,int n,o_edges_direction d)
{
        o_edges_list r,q,l,l1,h;
        ((void) (( x<3 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   17156 ,  "x<3" ), 0) )) ;
        h = l1 = o_edges_in_1_step(x,d);
        l = o_edges_list_new();
        while (!o_edges_list_is_empty(l1)) {
                r = q = o_edges_in_1_step(o_edges_list_head(l1),d);
                while(!o_edges_list_is_empty(q)) {
                        if (!o_edges_list_is_elem(l,o_edges_list_head(q))) {
                                l = o_edges_list_extend(o_edges_list_head(q),l);
                        }
                        q = o_edges_list_tail(q);
                }               o_edges_list_destroy(r);
                        l1 = o_edges_list_tail(l1);
        }
        o_edges_list_destroy(h);
        return(l);
}
static o_edges o_edges_lgub(o_edges e1, o_edges e2, o_edges_direction d) {
        int x,i = 1;
        o_edges ret;
        o_edges_list l,l1,l2;
        ((void) (( e1<3 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   17178 ,  "e1<3" ), 0) )) ;
        ((void) (( e2<3 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   17179 ,  "e2<3" ), 0) )) ;
        if (e1 == e2) return(e1);
        while(!o_edges_list_is_empty((l1=o_edges_in_n_step(e1,i,d)))) {
                l2 = o_edges_in_n_step(e2,i,d);
                l = o_edges_list_schnitt(l1,l2);
                x = o_edges_list_length(l);
                ((void) (( x<=1 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   17185 ,  "x<=1" ), 0) )) ;  
                if (x>0) {
                        ret = o_edges_list_head(l);
                        o_edges_list_destroy(l);
                        o_edges_list_destroy(l1);
                        o_edges_list_destroy(l2);
                        return(ret);
                }
                o_edges_list_destroy(l);
                o_edges_list_destroy(l1);
                o_edges_list_destroy(l2);
        }
        ein_neur_name= fatal_error__("1.c",17197)+fatal_error_ ("o_edges ist kein korrekter Verband");
        return(0);
}
o_edges o_edges_glb(o_edges e1, o_edges e2) {
        return(o_edges_lgub(e1,e2,o_edges_backward));
}
o_edges o_edges_lub(o_edges e1, o_edges e2) {
        return(o_edges_lgub(e1,e2,o_edges_forward));
}
int  o_edges_le(o_edges e1, o_edges e2) {
        o_edges x;
        x = o_edges_lub(e1,e2);
        if (e2 == x) return(1 );
        return(0 );
}
int  o_edges_ge(o_edges e1, o_edges e2) {
        o_edges x;
        x = o_edges_glb(e1,e2);
        if (e1 == x) return(1 );
        return(0 );
}
int  o_edges_eq(o_edges e1, o_edges e2) {
        return(e1 == e2);
}
int  o_edges_neq(o_edges e1, o_edges e2) {
        return(e1 != e2);
}
o_edges o_edges_dublicate_whole(o_edges e) {
        return(e);
}
o_edges o_edges_dublicate(o_edges e) {
        return(e);
}
void o_edges_print_stdout(o_edges e) {
        char *s;
        s = o_edges_print(e);
        fprintf((&_iob[1]) ,s);
        free((void *) s ) ;
}
char *o_edges_print(o_edges e) {
        char *s;
        static char *text[] = {
                "o_tt",
                "o_ff",
                "o_norm" 
        };
        s = malloc( sizeof(char)*8 ) ;
        if (s== ((void *)0) ) ein_neur_name= fatal_error__("1.c",17262)+fatal_error_ ("No space");
        sprintf(s,"%s",text[e]);
        return(s);
}
char *o_edges_vcgprint(o_edges e) {
        char *s;
        static char *text[] = {
                "o_tt",
                "o_ff",
                "o_norm" 
        };
        s = malloc( sizeof(char)*(8+13) ) ;
        if (s== ((void *)0) ) ein_neur_name= fatal_error__("1.c",17280)+fatal_error_ ("No space");
        sprintf(s,"\\verb+%s+",text[e]);
        return(s);
}
void o_edges_destroy(o_edges e) {
}
void o_edges_destroy_whole(o_edges e) {
}
static o_edges_list _o_edges_list_dublicate ( o_edges_list, int  );
long o_edges_list_power;
void o_edges_list_init(void)
{
   static int  flag = 0 ;
        if (!flag) {
        flag = 1 ;
        o_edges_list_power = -1 ;
                o_edges_init();
        }
}
o_edges_list o_edges_list_new()
{
        return(((void *)0) );
}
o_edges_list o_edges_list_extend(o_edges head, o_edges_list tail)
{
        o_edges_list help;
        help = (o_edges_list) malloc( sizeof(*help) ) ;
        if (help == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",17435)+fatal_error_ ("No Memory in  function o_edges_list_extend\n");        
        help->head = head;
        help->tail = tail;
        return(help);
}
void o_edges_list_destroy(list)
o_edges_list list;
{
        o_edges_list help;
        while(list!= ((void *)0) )
        {
                help = list;
                list = list->tail;
                free((void *) help ) ;
        }
}
void o_edges_list_destroy_whole(list)
o_edges_list list;
{
        o_edges_list help;
        while(list!= ((void *)0) )
        {
                help = list;
                o_edges_destroy_whole(help->head);
                list = list->tail;
      free((void *) help ) ;
        }
}
o_edges_list o_edges_list_conc(o_edges_list list1, o_edges_list list2)
{
        o_edges_list help,merk;
        if (list1== ((void *)0) )
                return(list2);
        if (list2 == ((void *)0) )
                return(list1);
        merk = help = o_edges_list_dublicate(list1);
        while(help->tail!= ((void *)0) )
                help=help->tail;
        help->tail=list2;
        return(merk);
}
        
int o_edges_list_length(o_edges_list list)
{
        if (list == ((void *)0) )
                return(0);
        return(o_edges_list_length(list->tail)+1);
}
o_edges o_edges_list_head(o_edges_list list)
{
        ((void) (( list!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   17511 ,  "list!= NULL" ), 0) )) ;
        return(list->head);
}
o_edges_list o_edges_list_tail(list)
o_edges_list list;
{
        ((void) (( list!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   17521 ,  "list!= NULL" ), 0) )) ;
        return(list->tail);
}
o_edges o_edges_list_get_elem(o_edges_list list, int nr)
{
        ((void) (( nr>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   17531 ,  "nr>0" ), 0) )) ;
        ((void) (( list!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   17532 ,  "list!=NULL" ), 0) )) ;
        if (nr == 1)
                return(list->head);
        return(o_edges_list_get_elem(list->tail,nr-1));
}
o_edges_list o_edges_list_set_elem(o_edges_list list,int nr,o_edges elem)
{
        int i;
        o_edges_list help;
        ((void) (( nr>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   17548 ,  "nr>0" ), 0) )) ;
        help = list;
        for(i=1;i<nr;i++)
        {
                ((void) (( help!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   17552 ,  "help!=NULL" ), 0) )) ;
                help = help->tail;
        }
        ((void) (( help!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   17555 ,  "help!=NULL" ), 0) )) ;
        help->head = elem;
        return(list);
}
o_edges_list o_edges_list_insert_bevor(list1,nr,list2)
o_edges_list list1,list2;
int nr;
{
        int i;
        o_edges_list help,help2;
        ((void) (( nr>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   17573 ,  "nr>0" ), 0) )) ;
        help = list1;
        if (nr == 1) 
        {
                return(o_edges_list_conc(list2,list1));
        }
        for(i=2;i<nr;i++)
        {
                ((void) (( help != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   17581 ,  "help != NULL" ), 0) )) ;
                help = help ->tail;
        }
        ((void) (( help= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",       "1.c" ,   17584 ,  "help=NULL" ), 0) )) ;
        help2 = help->tail;
        help->tail = list2;
        (void) o_edges_list_conc(help,help2);
        return(list1);
}
int  o_edges_list_is_empty(list)
o_edges_list list;
{
        return(list == ((void *)0) );
}
void o_edges_list_print_stdout(o_edges_list list)
{
        char *s;
        s = o_edges_list_print(list);
        fprintf((&_iob[1]) ,"%s",s);
        free((void *) s ) ;
}
o_edges_list o_edges_list_append(o_edges_list list, o_edges elem)
{
        o_edges_list help1,help2;
        help1 = help2 = list;
        while (help2!= ((void *)0) ) {
                help1 = help2;
                help2 = help2->tail;
        }
        help2 = o_edges_list_extend(elem,((void *)0) );
        if (help1!= ((void *)0) ) {
                help1->tail = help2;
                return(list);
        }
        return(help2);
}
o_edges_list o_edges_list_dublicate_whole(o_edges_list list)
{
        return(_o_edges_list_dublicate(list,1 ));
}
o_edges_list o_edges_list_dublicate(o_edges_list list)
{
        return(_o_edges_list_dublicate(list,0 ));
}
static o_edges_list _o_edges_list_dublicate(o_edges_list list, int  whole_mode)
{
        o_edges_list help1,head,help;
        head = ((void *)0) ;
        help1 = ((void *)0) ;
        if (list== ((void *)0) ) return(((void *)0) );
        while(list != ((void *)0) ) {
   help = (o_edges_list) malloc( sizeof(*help) ) ;
                if (help == ((void *)0) ) {
                        ein_neur_name= fatal_error__("1.c",17675)+fatal_error_ ("No Memory in  function o_edges_list_extend\n");        
                }
                help->head = whole_mode?o_edges_dublicate_whole(list->head):list->head;
                if (help1!= ((void *)0) )
                        help1->tail = help;
                else
                        head = help;
                help1 = help;
                list = list->tail;
        }
        help->tail= ((void *)0) ;
        return(head);
}
                
                
int  o_edges_list_is_elem(o_edges_list list,o_edges elem)
{
        while(list!= ((void *)0) ) {
                if (o_edges_eq(list->head,elem))
                        return(1 );
                list = list->tail;
        }
        return(0 );
}
o_edges_list o_edges_list_remove(o_edges_list list, o_edges elem)
{
        o_edges_list help1,help2;
        help1 =help2 = list;
        while(help2!= ((void *)0) ) {
                if (o_edges_eq(help2->head,elem))
                        break;
                help1 = help2;
                help2 = help2->tail;
        }
        if (help2== ((void *)0) )
                return(list);
        if (help2==list)
                return(help2->tail);
        help1->tail = help2->tail;
        return(list);
}
char *o_edges_list_print(o_edges_list list)
{
        int x,len = 1024;
        char *s,*e,*w;
        e = (char*)  malloc( sizeof(char)*len ) ;
        if (e== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",17741)+fatal_error_ ("No memeory");
        e[0]='\0';
        strcpy(e,"[");
        x=strlen(e);
        while (!o_edges_list_is_empty(list)) {
                s = o_edges_print(o_edges_list_head(list));
                x += strlen(s);
                if (x+5 >= len) {
                        len *= 2;
                        w = (char*)malloc( sizeof(char)*len ) ;
                        if (w== ((void *)0) )
                                ein_neur_name= fatal_error__("1.c",17756)+fatal_error_ ("No memeory");
                        w[0] = '\0';
                        strcpy(w,e);
                        free((void *) e ) ;
                        e = w;
                }
                strcat(e,s);
                strcat(e,",");
                x = strlen(e);
                list = o_edges_list_tail(list);
        }
        strcat(e,"]");
        return(e);
}
int  o_edges_list_eq(o_edges_list l1, o_edges_list l2)
{
        while(!o_edges_list_is_empty(l1)) {
                if  (o_edges_list_is_empty(l2)) 
                        return(0 );
                if (!o_edges_eq(o_edges_list_head(l1),o_edges_list_head(l2)))
                        return(0 );
                l1 = o_edges_list_tail(l1);
                l2 = o_edges_list_tail(l2);
        }
        if  (!o_edges_list_is_empty(l2)) 
                return(0 );
        return(1 );
}
                
o_edges_list o_edges_list_rest(o_edges_list l1, o_edges_list l2)
{
   o_edges_list l3;
   o_edges elem;
   l3 = o_edges_list_new();
   while(!o_edges_list_is_empty(l1)) {
      elem = o_edges_list_head(l1);
      if (!o_edges_list_is_elem(l2,elem))
         l3 = o_edges_list_extend(elem,l3);
      l1 = o_edges_list_tail(l1);
   }
   return(l3);
}
o_edges_list o_edges_list_vereinigung(o_edges_list l1, o_edges_list l2)
{
   o_edges_list l3;
   o_edges elem;
   l3 = o_edges_list_dublicate(l2);
   while(!o_edges_list_is_empty(l1)) {
      elem = o_edges_list_head(l1);
      if (!o_edges_list_is_elem(l2,elem))
         l3 = o_edges_list_extend(elem,l3);
      l1 = o_edges_list_tail(l1);
   }
   return(l3);
}
o_edges_list o_edges_list_schnitt(o_edges_list l1, o_edges_list l2)
{
   o_edges_list l3;
   o_edges elem;
   l3 = o_edges_list_new();
   while(!o_edges_list_is_empty(l1)) {
      elem = o_edges_list_head(l1);
      if (o_edges_list_is_elem(l2,elem))
         l3 = o_edges_list_extend(elem,l3);
      l1 = o_edges_list_tail(l1);
   }
   return(l3);
}
static o_noth_dom_kante mm_g[] = {
{o_noth_dom_dummy,o_noth_dom_dummy}
};
o_noth_dom o_noth_dom_top = o_nix;
o_noth_dom o_noth_dom_bottom = o_nix;
long o_noth_dom_power = 1;
void o_noth_dom_init(void)
{
}
unsigned int o_noth_dom_hash(o_noth_dom x)
{
         return((unsigned int)x);
}
char *o_noth_dom_len(void)
{
        return(string);
}
int  o_noth_dom_istop(o_noth_dom x) {
        return(x == o_nix);
}
int  o_noth_dom_isbottom(o_noth_dom x) {
        return(x == o_nix);
}
static o_noth_dom_list o_noth_dom_in_1_step(o_noth_dom x,o_noth_dom_direction d)
{
        int i;
        o_noth_dom_list l;
        ((void) (( x<1 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   18018 ,  "x<1" ), 0) )) ;
        ((void) (( d==o_noth_dom_forward || d==o_noth_dom_backward ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",          "1.c" ,   18019 ,  "d==o_noth_dom_forward || d==o_noth_dom_backward" ), 0) )) ;
        l = o_noth_dom_list_new();
        for(i=0;i<0;i++) {
                if ((d == o_noth_dom_forward && mm_g[i].von == x) ||
                        (d == o_noth_dom_backward && mm_g[i].nach == x))
                        l = o_noth_dom_list_extend(d == o_noth_dom_forward ?mm_g[i].nach:mm_g[i].von,l);
        }
        return(l);
}
static o_noth_dom_list o_noth_dom_in_n_step(o_noth_dom x,int n,o_noth_dom_direction d)
{
        o_noth_dom_list r,q,l,l1,h;
        ((void) (( x<1 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   18034 ,  "x<1" ), 0) )) ;
        h = l1 = o_noth_dom_in_1_step(x,d);
        l = o_noth_dom_list_new();
        while (!o_noth_dom_list_is_empty(l1)) {
                r = q = o_noth_dom_in_1_step(o_noth_dom_list_head(l1),d);
                while(!o_noth_dom_list_is_empty(q)) {
                        if (!o_noth_dom_list_is_elem(l,o_noth_dom_list_head(q))) {
                                l = o_noth_dom_list_extend(o_noth_dom_list_head(q),l);
                        }
                        q = o_noth_dom_list_tail(q);
                }               o_noth_dom_list_destroy(r);
                        l1 = o_noth_dom_list_tail(l1);
        }
        o_noth_dom_list_destroy(h);
        return(l);
}
static o_noth_dom o_noth_dom_lgub(o_noth_dom e1, o_noth_dom e2, o_noth_dom_direction d) {
        int x,i = 1;
        o_noth_dom ret;
        o_noth_dom_list l,l1,l2;
        ((void) (( e1<1 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   18056 ,  "e1<1" ), 0) )) ;
        ((void) (( e2<1 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   18057 ,  "e2<1" ), 0) )) ;
        if (e1 == e2) return(e1);
        while(!o_noth_dom_list_is_empty((l1=o_noth_dom_in_n_step(e1,i,d)))) {
                l2 = o_noth_dom_in_n_step(e2,i,d);
                l = o_noth_dom_list_schnitt(l1,l2);
                x = o_noth_dom_list_length(l);
                ((void) (( x<=1 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   18063 ,  "x<=1" ), 0) )) ;  
                if (x>0) {
                        ret = o_noth_dom_list_head(l);
                        o_noth_dom_list_destroy(l);
                        o_noth_dom_list_destroy(l1);
                        o_noth_dom_list_destroy(l2);
                        return(ret);
                }
                o_noth_dom_list_destroy(l);
                o_noth_dom_list_destroy(l1);
                o_noth_dom_list_destroy(l2);
        }
        ein_neur_name= fatal_error__("1.c",18075)+fatal_error_ ("o_noth_dom ist kein korrekter Verband");
        return(0);
}
o_noth_dom o_noth_dom_glb(o_noth_dom e1, o_noth_dom e2) {
        return(o_noth_dom_lgub(e1,e2,o_noth_dom_backward));
}
o_noth_dom o_noth_dom_lub(o_noth_dom e1, o_noth_dom e2) {
        return(o_noth_dom_lgub(e1,e2,o_noth_dom_forward));
}
int  o_noth_dom_le(o_noth_dom e1, o_noth_dom e2) {
        o_noth_dom x;
        x = o_noth_dom_lub(e1,e2);
        if (e2 == x) return(1 );
        return(0 );
}
int  o_noth_dom_ge(o_noth_dom e1, o_noth_dom e2) {
        o_noth_dom x;
        x = o_noth_dom_glb(e1,e2);
        if (e1 == x) return(1 );
        return(0 );
}
int  o_noth_dom_eq(o_noth_dom e1, o_noth_dom e2) {
        return(e1 == e2);
}
int  o_noth_dom_neq(o_noth_dom e1, o_noth_dom e2) {
        return(e1 != e2);
}
o_noth_dom o_noth_dom_dublicate_whole(o_noth_dom e) {
        return(e);
}
o_noth_dom o_noth_dom_dublicate(o_noth_dom e) {
        return(e);
}
void o_noth_dom_print_stdout(o_noth_dom e) {
        char *s;
        s = o_noth_dom_print(e);
        fprintf((&_iob[1]) ,s);
        free((void *) s ) ;
}
char *o_noth_dom_print(o_noth_dom e) {
        char *s;
        static char *text[] = {
                "o_nix" 
        };
        s = malloc( sizeof(char)*7 ) ;
        if (s== ((void *)0) ) ein_neur_name= fatal_error__("1.c",18138)+fatal_error_ ("No space");
        sprintf(s,"%s",text[e]);
        return(s);
}
char *o_noth_dom_vcgprint(o_noth_dom e) {
        char *s;
        static char *text[] = {
                "o_nix" 
        };
        s = malloc( sizeof(char)*(7+13) ) ;
        if (s== ((void *)0) ) ein_neur_name= fatal_error__("1.c",18154)+fatal_error_ ("No space");
        sprintf(s,"\\verb+%s+",text[e]);
        return(s);
}
void o_noth_dom_destroy(o_noth_dom e) {
}
void o_noth_dom_destroy_whole(o_noth_dom e) {
}
static o_noth_dom_list _o_noth_dom_list_dublicate ( o_noth_dom_list, int  );
long o_noth_dom_list_power;
void o_noth_dom_list_init(void)
{
   static int  flag = 0 ;
        if (!flag) {
        flag = 1 ;
        o_noth_dom_list_power = -1 ;
                o_noth_dom_init();
        }
}
o_noth_dom_list o_noth_dom_list_new()
{
        return(((void *)0) );
}
o_noth_dom_list o_noth_dom_list_extend(o_noth_dom head, o_noth_dom_list tail)
{
        o_noth_dom_list help;
        help = (o_noth_dom_list) malloc( sizeof(*help) ) ;
        if (help == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",18309)+fatal_error_ ("No Memory in  function o_noth_dom_list_extend\n");     
        help->head = head;
        help->tail = tail;
        return(help);
}
void o_noth_dom_list_destroy(list)
o_noth_dom_list list;
{
        o_noth_dom_list help;
        while(list!= ((void *)0) )
        {
                help = list;
                list = list->tail;
                free((void *) help ) ;
        }
}
void o_noth_dom_list_destroy_whole(list)
o_noth_dom_list list;
{
        o_noth_dom_list help;
        while(list!= ((void *)0) )
        {
                help = list;
                o_noth_dom_destroy_whole(help->head);
                list = list->tail;
      free((void *) help ) ;
        }
}
o_noth_dom_list o_noth_dom_list_conc(o_noth_dom_list list1, o_noth_dom_list list2)
{
        o_noth_dom_list help,merk;
        if (list1== ((void *)0) )
                return(list2);
        if (list2 == ((void *)0) )
                return(list1);
        merk = help = o_noth_dom_list_dublicate(list1);
        while(help->tail!= ((void *)0) )
                help=help->tail;
        help->tail=list2;
        return(merk);
}
        
int o_noth_dom_list_length(o_noth_dom_list list)
{
        if (list == ((void *)0) )
                return(0);
        return(o_noth_dom_list_length(list->tail)+1);
}
o_noth_dom o_noth_dom_list_head(o_noth_dom_list list)
{
        ((void) (( list!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   18385 ,  "list!= NULL" ), 0) )) ;
        return(list->head);
}
o_noth_dom_list o_noth_dom_list_tail(list)
o_noth_dom_list list;
{
        ((void) (( list!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   18395 ,  "list!= NULL" ), 0) )) ;
        return(list->tail);
}
o_noth_dom o_noth_dom_list_get_elem(o_noth_dom_list list, int nr)
{
        ((void) (( nr>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   18405 ,  "nr>0" ), 0) )) ;
        ((void) (( list!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   18406 ,  "list!=NULL" ), 0) )) ;
        if (nr == 1)
                return(list->head);
        return(o_noth_dom_list_get_elem(list->tail,nr-1));
}
o_noth_dom_list o_noth_dom_list_set_elem(o_noth_dom_list list,int nr,o_noth_dom elem)
{
        int i;
        o_noth_dom_list help;
        ((void) (( nr>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   18422 ,  "nr>0" ), 0) )) ;
        help = list;
        for(i=1;i<nr;i++)
        {
                ((void) (( help!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   18426 ,  "help!=NULL" ), 0) )) ;
                help = help->tail;
        }
        ((void) (( help!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   18429 ,  "help!=NULL" ), 0) )) ;
        help->head = elem;
        return(list);
}
o_noth_dom_list o_noth_dom_list_insert_bevor(list1,nr,list2)
o_noth_dom_list list1,list2;
int nr;
{
        int i;
        o_noth_dom_list help,help2;
        ((void) (( nr>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   18447 ,  "nr>0" ), 0) )) ;
        help = list1;
        if (nr == 1) 
        {
                return(o_noth_dom_list_conc(list2,list1));
        }
        for(i=2;i<nr;i++)
        {
                ((void) (( help != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   18455 ,  "help != NULL" ), 0) )) ;
                help = help ->tail;
        }
        ((void) (( help= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",       "1.c" ,   18458 ,  "help=NULL" ), 0) )) ;
        help2 = help->tail;
        help->tail = list2;
        (void) o_noth_dom_list_conc(help,help2);
        return(list1);
}
int  o_noth_dom_list_is_empty(list)
o_noth_dom_list list;
{
        return(list == ((void *)0) );
}
void o_noth_dom_list_print_stdout(o_noth_dom_list list)
{
        char *s;
        s = o_noth_dom_list_print(list);
        fprintf((&_iob[1]) ,"%s",s);
        free((void *) s ) ;
}
o_noth_dom_list o_noth_dom_list_append(o_noth_dom_list list, o_noth_dom elem)
{
        o_noth_dom_list help1,help2;
        help1 = help2 = list;
        while (help2!= ((void *)0) ) {
                help1 = help2;
                help2 = help2->tail;
        }
        help2 = o_noth_dom_list_extend(elem,((void *)0) );
        if (help1!= ((void *)0) ) {
                help1->tail = help2;
                return(list);
        }
        return(help2);
}
o_noth_dom_list o_noth_dom_list_dublicate_whole(o_noth_dom_list list)
{
        return(_o_noth_dom_list_dublicate(list,1 ));
}
o_noth_dom_list o_noth_dom_list_dublicate(o_noth_dom_list list)
{
        return(_o_noth_dom_list_dublicate(list,0 ));
}
static o_noth_dom_list _o_noth_dom_list_dublicate(o_noth_dom_list list, int  whole_mode)
{
        o_noth_dom_list help1,head,help;
        head = ((void *)0) ;
        help1 = ((void *)0) ;
        if (list== ((void *)0) ) return(((void *)0) );
        while(list != ((void *)0) ) {
   help = (o_noth_dom_list) malloc( sizeof(*help) ) ;
                if (help == ((void *)0) ) {
                        ein_neur_name= fatal_error__("1.c",18549)+fatal_error_ ("No Memory in  function o_noth_dom_list_extend\n");     
                }
                help->head = whole_mode?o_noth_dom_dublicate_whole(list->head):list->head;
                if (help1!= ((void *)0) )
                        help1->tail = help;
                else
                        head = help;
                help1 = help;
                list = list->tail;
        }
        help->tail= ((void *)0) ;
        return(head);
}
                
                
int  o_noth_dom_list_is_elem(o_noth_dom_list list,o_noth_dom elem)
{
        while(list!= ((void *)0) ) {
                if (o_noth_dom_eq(list->head,elem))
                        return(1 );
                list = list->tail;
        }
        return(0 );
}
o_noth_dom_list o_noth_dom_list_remove(o_noth_dom_list list, o_noth_dom elem)
{
        o_noth_dom_list help1,help2;
        help1 =help2 = list;
        while(help2!= ((void *)0) ) {
                if (o_noth_dom_eq(help2->head,elem))
                        break;
                help1 = help2;
                help2 = help2->tail;
        }
        if (help2== ((void *)0) )
                return(list);
        if (help2==list)
                return(help2->tail);
        help1->tail = help2->tail;
        return(list);
}
char *o_noth_dom_list_print(o_noth_dom_list list)
{
        int x,len = 1024;
        char *s,*e,*w;
        e = (char*)  malloc( sizeof(char)*len ) ;
        if (e== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",18615)+fatal_error_ ("No memeory");
        e[0]='\0';
        strcpy(e,"[");
        x=strlen(e);
        while (!o_noth_dom_list_is_empty(list)) {
                s = o_noth_dom_print(o_noth_dom_list_head(list));
                x += strlen(s);
                if (x+5 >= len) {
                        len *= 2;
                        w = (char*)malloc( sizeof(char)*len ) ;
                        if (w== ((void *)0) )
                                ein_neur_name= fatal_error__("1.c",18630)+fatal_error_ ("No memeory");
                        w[0] = '\0';
                        strcpy(w,e);
                        free((void *) e ) ;
                        e = w;
                }
                strcat(e,s);
                strcat(e,",");
                x = strlen(e);
                list = o_noth_dom_list_tail(list);
        }
        strcat(e,"]");
        return(e);
}
int  o_noth_dom_list_eq(o_noth_dom_list l1, o_noth_dom_list l2)
{
        while(!o_noth_dom_list_is_empty(l1)) {
                if  (o_noth_dom_list_is_empty(l2)) 
                        return(0 );
                if (!o_noth_dom_eq(o_noth_dom_list_head(l1),o_noth_dom_list_head(l2)))
                        return(0 );
                l1 = o_noth_dom_list_tail(l1);
                l2 = o_noth_dom_list_tail(l2);
        }
        if  (!o_noth_dom_list_is_empty(l2)) 
                return(0 );
        return(1 );
}
                
o_noth_dom_list o_noth_dom_list_rest(o_noth_dom_list l1, o_noth_dom_list l2)
{
   o_noth_dom_list l3;
   o_noth_dom elem;
   l3 = o_noth_dom_list_new();
   while(!o_noth_dom_list_is_empty(l1)) {
      elem = o_noth_dom_list_head(l1);
      if (!o_noth_dom_list_is_elem(l2,elem))
         l3 = o_noth_dom_list_extend(elem,l3);
      l1 = o_noth_dom_list_tail(l1);
   }
   return(l3);
}
o_noth_dom_list o_noth_dom_list_vereinigung(o_noth_dom_list l1, o_noth_dom_list l2)
{
   o_noth_dom_list l3;
   o_noth_dom elem;
   l3 = o_noth_dom_list_dublicate(l2);
   while(!o_noth_dom_list_is_empty(l1)) {
      elem = o_noth_dom_list_head(l1);
      if (!o_noth_dom_list_is_elem(l2,elem))
         l3 = o_noth_dom_list_extend(elem,l3);
      l1 = o_noth_dom_list_tail(l1);
   }
   return(l3);
}
o_noth_dom_list o_noth_dom_list_schnitt(o_noth_dom_list l1, o_noth_dom_list l2)
{
   o_noth_dom_list l3;
   o_noth_dom elem;
   l3 = o_noth_dom_list_new();
   while(!o_noth_dom_list_is_empty(l1)) {
      elem = o_noth_dom_list_head(l1);
      if (o_noth_dom_list_is_elem(l2,elem))
         l3 = o_noth_dom_list_extend(elem,l3);
      l1 = o_noth_dom_list_tail(l1);
   }
   return(l3);
}
long o_numbool_power;
void o_numbool_init(void) {
        static int  flag = 0 ;
        flag = 1 ;
        unum_init();
        bool_init();
        o_numbool_power = 
                unum_power== -1 ?-1 :
                bool_power== -1 ?-1 :
                unum_power *bool_power *1;
}
unsigned int o_numbool_hash(o_numbool x) {
        unsigned int h=0;
        h += unum_hash(x->unum_1_comp)*2;
        h += bool_hash(x->bool_2_comp)*3;
        return(h);
}
o_numbool o_numbool_create(unum c1, bool c2  )
{
        o_numbool x;
        x = o_numbool_new();
        x->unum_1_comp = c1;
        x->bool_2_comp = c2;
        return(x);
}
static o_numbool o_numbool_new(void)
{
        o_numbool help;
        help = (o_numbool) calloc(sizeof(_o_numbool),1);
        if (help== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",18927)+fatal_error_ ("No memory");
        return(help);
}
o_numbool o_numbool_update_1(o_numbool tup, unum x)
{
        o_numbool help;
        help = o_numbool_dublicate(tup);
        help->unum_1_comp = x;
        return(help);
}
o_numbool o_numbool_update_2(o_numbool tup, bool x)
{
        o_numbool help;
        help = o_numbool_dublicate(tup);
        help->bool_2_comp = x;
        return(help);
}
unum o_numbool_select_1(o_numbool tup)
{
        return(tup->unum_1_comp);
}
bool o_numbool_select_2(o_numbool tup)
{
        return(tup->bool_2_comp);
}
int  o_numbool_eq(o_numbool x1, o_numbool x2)
{
        if (unum_neq(x1->unum_1_comp, x2->unum_1_comp)) {
                        return(0 );
                }
        if (bool_neq(x1->bool_2_comp, x2->bool_2_comp)) {
                        return(0 );
                }
        return(1 );
}
o_numbool o_numbool_dublicate(o_numbool x)
{
        return(_o_numbool_dublicate(x,0 ));
}
o_numbool o_numbool_dublicate_whole(o_numbool x)
{
        return(_o_numbool_dublicate(x,1 ));
}
static o_numbool _o_numbool_dublicate(o_numbool x,int  whole_mode)
{
        o_numbool erg;
        erg = o_numbool_new();
        erg->unum_1_comp = whole_mode?unum_dublicate_whole(x->unum_1_comp):x->unum_1_comp;
        erg->bool_2_comp = whole_mode?bool_dublicate_whole(x->bool_2_comp):x->bool_2_comp;
        return(erg);
}
void o_numbool_print_stdout(o_numbool x)
{
        char *my_str;
        my_str = o_numbool_print(x);
        fprintf((&_iob[1]) ,"%s",my_str);
        free((void *) my_str ) ;
}
char *o_numbool_print(o_numbool x)
{
        char *s1;
        char *s2;
        char *s;
        int len =4;
        s1 = unum_print(x->unum_1_comp);
        len += strlen(s1)+2;
        s2 = bool_print(x->bool_2_comp);
        len += strlen(s2)+2;
        s = malloc( sizeof(char)*len ) ;
        if (s == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",19054)+fatal_error_ ("No_memory");
        sprintf(s,"(");
        strcat(s,s1);
        free((void *) s1 ) ;
        strcat(s,",");
        strcat(s,s2);
        free((void *) s2 ) ;
        strcat(s,")");
        return(s);
}
char *o_numbool_vcgprint(o_numbool x)
{
        char *s1;
        char *s2;
        char *s;
        int len =60;
        s1 = unum_vcgprint(x->unum_1_comp);
        len += strlen(s1)+2;
        s2 = bool_vcgprint(x->bool_2_comp);
        len += strlen(s2)+2;
        s = malloc( sizeof(char)*len ) ;
        if (s == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",19088)+fatal_error_ ("No_memory");
        sprintf(s,"\\left(");
        strcat(s,"\\begin{array}{|c|}\n");
        strcat(s,"\\hline");
        strcat(s,s1);
        free((void *) s1 ) ;
        strcat(s,"\\\\  \\hline ");
        strcat(s,s2);
        free((void *) s2 ) ;
        strcat(s,"\\\\ \\hline");
        strcat(s,"\\end{array}");
        strcat(s,"\\right)");
        return(s);
}
void o_numbool_destroy(o_numbool x)
{
        free((void *) x ) ;
}
void o_numbool_destroy_whole(o_numbool x)
{
        unum_destroy_whole(x->unum_1_comp);
        bool_destroy_whole(x->bool_2_comp);
        free((void *) x ) ;
}
static o_t1 o_t1_new(void) ;
o_t1 o_t1_top,o_t1_bottom;
long o_t1_power; 
void o_t1_init(void) 
{
        static int  flag = 0 ;
        if (!flag) {
                flag = 1 ;
                unum_init();
                o_t1_power = unum_power== -1 ? -1 :unum_power+2;
                o_t1_top =  o_t1_new();
                o_t1_bottom = o_t1_new();
                o_t1_top->flag = o_t1_top_class;
                o_t1_bottom->flag = o_t1_bottom_class;
        }
}
unsigned int o_t1_hash(o_t1 x) {
        switch(x->flag) {
                case o_t1_top_class : return(0U);
                case o_t1_bottom_class: return(1U);
                default: return(unum_hash(x->inh)+2U);
        }
}
char *o_t1_len(void)
{
char *s;
        s = malloc( 4 ) ;
sprintf(s,"2");
return s;
}
static o_t1 o_t1_new(void) 
{
        o_t1 help;
        help = (o_t1) malloc( sizeof(_o_t1) ) ;
        if (help == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",19261)+fatal_error_ ("No memeory!");
        return(help);
}
void o_t1_destroy(o_t1 help) 
{
        ((void) (( help!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   19316 ,  "help!=NULL" ), 0) )) ;
        free((void *) help ) ;
}
void o_t1_destroy_whole(o_t1 help) 
{
        if (help->flag == o_t1_norm_class)
                unum_destroy_whole(help->inh);
        free((void *) help ) ;
}
o_t1 o_t1_dublicate_whole(o_t1 help) 
{
        o_t1 help1;
        help1 = o_t1_new();
        help1->flag = help->flag;
        if (help1->flag == o_t1_norm_class)
                help1->inh = unum_dublicate_whole(help->inh);
        return(help1);
}
o_t1 o_t1_dublicate(o_t1 help) 
{
        o_t1 help1;
        help1 = o_t1_new();
        help1->flag = help->flag;
        if (help1->flag == o_t1_norm_class)
                help1->inh = help->inh;
        return(help1);
}
char *o_t1_print(o_t1 elem) 
{
        char *s;
        switch(elem->flag) { 
                case o_t1_top_class: 
                        s = (char*) malloc( sizeof(char)*5 ) ;
                        sprintf(s,"TOP");
                        break;
                case o_t1_bottom_class: 
                        s = (char*) malloc( sizeof(char)*8 ) ;
                        sprintf(s,"BOTTOM");
                        break;
                case o_t1_norm_class:  
                        s= unum_print(elem->inh); 
                        break;
                default: 
                        ein_neur_name= fatal_error__("1.c",19406)+fatal_error_ ("Unknown case in o_t1_print");
        }
        return(s);
}
char *o_t1_vcgprint(o_t1 elem)
{
        char *s;
        switch(elem->flag) {
                case o_t1_top_class:
                        s = (char*) malloc( sizeof(char)*5 ) ;
                        sprintf(s,"\\top");
                        break;
                case o_t1_bottom_class:
                        s = (char*) malloc( sizeof(char)*8 ) ;
                        sprintf(s,"\\bot");
                        break;
                case o_t1_norm_class:
                        s= unum_vcgprint(elem->inh);
                        break;
                default:
                        ein_neur_name= fatal_error__("1.c",19440)+fatal_error_ ("Unknown case in o_t1_print");
        }
        return(s);
}
void o_t1_print_stdout(o_t1 elem)
{
        char *s;
        s = o_t1_print(elem);
        fprintf((&_iob[1]) ,"%s",s);
        free((void *) s ) ;
}
int  o_t1_isbottom(o_t1 ob) 
{
        return(ob->flag == o_t1_bottom_class); 
}
int  o_t1_istop(o_t1 ob) 
{
        return(ob->flag == o_t1_top_class);
}
int  o_t1_eq(o_t1 s,o_t1 t) 
{
        if (s->flag != t->flag)
                return(0 );
        if (s->flag == o_t1_norm_class)
                return(unum_eq(s->inh,t->inh));
        return(1 );
}
int  o_t1_neq(o_t1 e1, o_t1 e2)
{
        return(!o_t1_eq(e1,e2));
}
o_t1 o_t1_lift(unum inh) 
{
        o_t1 help;
        help = o_t1_new();
        help->inh = inh;
        help->flag = o_t1_norm_class;
        return(help);
}
o_t1 o_t1_glb(o_t1 e1, o_t1 e2) 
{
        if (o_t1_istop(e1)) 
                return (e2);
        if (o_t1_istop(e2)) 
                return (e1);
        if (o_t1_eq(e1,e2)) 
                return(e1);
        return(o_t1_bottom);
}
o_t1 o_t1_lub(o_t1 e1, o_t1 e2) 
{
        if (o_t1_isbottom(e1)) 
                return (e2); 
        if (o_t1_isbottom(e2)) 
                return (e1); 
        if (o_t1_eq(e1,e2)) 
                return e1;
        return(o_t1_top);
}
int  o_t1_le(o_t1 e1, o_t1 e2) 
{
        switch(e1->flag) {
                case o_t1_bottom_class:
                        return(!o_t1_isbottom(e2));
                case o_t1_norm_class:
                        return(o_t1_istop(e2));
                case o_t1_top_class:
                        return(0 );
                default:
                        ein_neur_name= fatal_error__("1.c",19580)+fatal_error_ ("Unknown case in o_t1_le");
                        return(0 );
        }
}
unum o_t1_drop(o_t1 elem)
{
        switch(elem->flag) {
                case o_t1_top_class:
                case o_t1_bottom_class:
                        ein_neur_name= fatal_error__("1.c",19600)+fatal_error_ ("Drop auf top oder bottom angewandt (o_t1)");
                case o_t1_norm_class:
                        return(elem->inh);
                default:
                        ein_neur_name= fatal_error__("1.c",19604)+fatal_error_ ("Unknown case in o_t1_drop");
        }
        return(0);
}
static o_va o_va_new(void) ;
o_va o_va_top,o_va_bottom;
long o_va_power; 
void o_va_init(void) 
{
        static int  flag = 0 ;
        if (!flag) {
                flag = 1 ;
                o_const_val_init();
                o_va_power = o_const_val_power== -1 ? -1 :o_const_val_power+2;
                o_va_top =  o_va_new();
                o_va_bottom = o_va_new();
                o_va_top->flag = o_va_top_class;
                o_va_bottom->flag = o_va_bottom_class;
        }
}
unsigned int o_va_hash(o_va x) {
        switch(x->flag) {
                case o_va_top_class : return(0U);
                case o_va_bottom_class: return(1U);
                default: return(o_const_val_hash(x->inh)+2U);
        }
}
char *o_va_len(void)
{
char *s;
        s = malloc( 4 ) ;
sprintf(s,"2");
return s;
}
static o_va o_va_new(void) 
{
        o_va help;
        help = (o_va) malloc( sizeof(_o_va) ) ;
        if (help == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",19738)+fatal_error_ ("No memeory!");
        return(help);
}
void o_va_destroy(o_va help) 
{
        ((void) (( help!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   19793 ,  "help!=NULL" ), 0) )) ;
        free((void *) help ) ;
}
void o_va_destroy_whole(o_va help) 
{
        if (help->flag == o_va_norm_class)
                o_const_val_destroy_whole(help->inh);
        free((void *) help ) ;
}
o_va o_va_dublicate_whole(o_va help) 
{
        o_va help1;
        help1 = o_va_new();
        help1->flag = help->flag;
        if (help1->flag == o_va_norm_class)
                help1->inh = o_const_val_dublicate_whole(help->inh);
        return(help1);
}
o_va o_va_dublicate(o_va help) 
{
        o_va help1;
        help1 = o_va_new();
        help1->flag = help->flag;
        if (help1->flag == o_va_norm_class)
                help1->inh = help->inh;
        return(help1);
}
char *o_va_print(o_va elem) 
{
        char *s;
        switch(elem->flag) { 
                case o_va_top_class: 
                        s = (char*) malloc( sizeof(char)*5 ) ;
                        sprintf(s,"TOP");
                        break;
                case o_va_bottom_class: 
                        s = (char*) malloc( sizeof(char)*8 ) ;
                        sprintf(s,"BOTTOM");
                        break;
                case o_va_norm_class:  
                        s= o_const_val_print(elem->inh); 
                        break;
                default: 
                        ein_neur_name= fatal_error__("1.c",19883)+fatal_error_ ("Unknown case in o_va_print");
        }
        return(s);
}
char *o_va_vcgprint(o_va elem)
{
        char *s;
        switch(elem->flag) {
                case o_va_top_class:
                        s = (char*) malloc( sizeof(char)*5 ) ;
                        sprintf(s,"\\top");
                        break;
                case o_va_bottom_class:
                        s = (char*) malloc( sizeof(char)*8 ) ;
                        sprintf(s,"\\bot");
                        break;
                case o_va_norm_class:
                        s= o_const_val_vcgprint(elem->inh);
                        break;
                default:
                        ein_neur_name= fatal_error__("1.c",19917)+fatal_error_ ("Unknown case in o_va_print");
        }
        return(s);
}
void o_va_print_stdout(o_va elem)
{
        char *s;
        s = o_va_print(elem);
        fprintf((&_iob[1]) ,"%s",s);
        free((void *) s ) ;
}
int  o_va_isbottom(o_va ob) 
{
        return(ob->flag == o_va_bottom_class); 
}
int  o_va_istop(o_va ob) 
{
        return(ob->flag == o_va_top_class);
}
int  o_va_eq(o_va s,o_va t) 
{
        if (s->flag != t->flag)
                return(0 );
        if (s->flag == o_va_norm_class)
                return(o_const_val_eq(s->inh,t->inh));
        return(1 );
}
int  o_va_neq(o_va e1, o_va e2)
{
        return(!o_va_eq(e1,e2));
}
o_va o_va_lift(o_const_val inh) 
{
        o_va help;
        help = o_va_new();
        help->inh = inh;
        help->flag = o_va_norm_class;
        return(help);
}
o_va o_va_glb(o_va e1, o_va e2) 
{
        if (o_va_istop(e1)) 
                return (e2);
        if (o_va_istop(e2)) 
                return (e1);
        if (o_va_eq(e1,e2)) 
                return(e1);
        return(o_va_bottom);
}
o_va o_va_lub(o_va e1, o_va e2) 
{
        if (o_va_isbottom(e1)) 
                return (e2); 
        if (o_va_isbottom(e2)) 
                return (e1); 
        if (o_va_eq(e1,e2)) 
                return e1;
        return(o_va_top);
}
int  o_va_le(o_va e1, o_va e2) 
{
        switch(e1->flag) {
                case o_va_bottom_class:
                        return(!o_va_isbottom(e2));
                case o_va_norm_class:
                        return(o_va_istop(e2));
                case o_va_top_class:
                        return(0 );
                default:
                        ein_neur_name= fatal_error__("1.c",20057)+fatal_error_ ("Unknown case in o_va_le");
                        return(0 );
        }
}
o_const_val o_va_drop(o_va elem)
{
        switch(elem->flag) {
                case o_va_top_class:
                case o_va_bottom_class:
                        ein_neur_name= fatal_error__("1.c",20077)+fatal_error_ ("Drop auf top oder bottom angewandt (o_va)");
                case o_va_norm_class:
                        return(elem->inh);
                default:
                        ein_neur_name= fatal_error__("1.c",20081)+fatal_error_ ("Unknown case in o_va_drop");
        }
        return(0);
}
f80_t x64tof80(x64_t *);
int f80tox64(x64_t*,f80_t f80);
long real_power= -1 ;
static real r_null;
unsigned int real_hash(real x) {
        return((unsigned int)f80toi(x));
}
char *real_len(void)
{
   ein_neur_name= fatal_error__("1.c",20161)+fatal_error_ ("len in real");
        return("0");
}
void real_init(void)
{
   static int  flag = 0 ;
   flag = 1 ;
        r_null = real_create("0");
}
real real_add(real n1, real n2)
{
        return(fadd80(n1,n2));
}
real real_dublicate(real inh)
{
        real h;
        memmove(&inh,&h,sizeof(real));
        return(h);
}
real real_dublicate_whole(real inh)
{
                return(real_dublicate(inh));
}
int  real_eq(real e1, real e2)
{
        return(fcmp80(&e1,&e2,0 ));
}
int  real_neq(real e1, real e2)
{
        return(fcmp80(&e1,&e2,0 )!=0);
}
snum real_to_snum(real a)
{
        return(real_trunc(a));
}
unum real_to_unum(real a)
{
        return(snum_to_unum(real_trunc(a)));
}
snum real_trunc(real e)
{
        snum a;
        a = snum_create("0");
        if (f80tox64(a,e)!=0) 
                ein_neur_name= fatal_error__("1.c",20235)+fatal_error_ ("overflow");
        return(a);
}
real real_create(char *s)
{
        return(atof80(s));
}
void real_free(real h)
{
}
void real_destroy_whole(real h)
{
}
char *real_print(real l)
{
        char *s;
        s = (char*) malloc( 1024 ) ;
        if (s== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",20270)+fatal_error_ ("No memory");
        f80toa(s,"g",l);
        return(s);
}
char *real_vcgprint(real l)
{
        char *s;
        s = (char*) malloc( 1024 ) ;
        if (s== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",20285)+fatal_error_ ("No memory");
        f80toa(s,"g",l);
        return(s);
}
void real_print_stdout(real l)
{
char *s;
        fprintf((&_iob[1]) ,"%s",s=real_print(l));
free((void *) s ) ;
}
int  real_le(real a,real b)
{
        return(fcmp80(&a,&b,0 )<0);
}
int  real_leq(real a,real b)
{
        return(fcmp80(&a,&b,0 )<=0);
}
int  real_ge(real a,real b)
{
        return(fcmp80(&a,&b,0 )>0);
}
int  real_geq(real a,real b)
{
        return(fcmp80(&a,&b,0 )>=0);
}
real real_mal(real a,real b)
{
        return(fmul80(a,b));
}
real real_min(real a,real b)
{
        return(fsub80(a,b));
}
real real_hoch(real a,real b)
{
        return(fpow80(a,b));
}
real real_geteilt(real a,real b)
{
        return(fdiv80(a,b));
}
real real_unplus(real a)
{
        return(a);
}
real real_unmin(real a)
{
        return(fneg80(a));
}
real real_abs(real a)
{
        return(fabs80(a));
}
real real_modulo(real a, real b)
{
        return(frem80(a,b));
}
real real_to_real(real a)
{
        return(a);
}
snum real_sgn(real a)
{
   if (real_le(a,r_null))
      return(snum_create("-1"));
   if (real_ge(a,r_null))
      return(snum_create("1"));
   return(snum_create("0"));
}
        
f80_t real_to_f80(real x)
{
   return(x);
}
static snum snum_create_ ( void );
f80_t x64tof80(x64_t *);
int f80tox64(x64_t*,f80_t f80);
static snum s_null;
static snum s_eins;
long snum_power= -1 ;
char *snum_len(void)
{
   ein_neur_name= fatal_error__("1.c",20468)+fatal_error_ ("len in snum");
        return("0");
}
unsigned int snum_hash(snum x) {
        unsigned int i;
        x64toui(x,&i);
        return((int) i);
}
void snum_init(void)
{
   static int  flag = 0 ;
        if (!flag) {
        flag = 1 ;
                s_null = snum_create("0");
                s_eins = snum_create("1");
        }
}
x64_t snum_to_x64(unum x)
{
   return(*x);
}
unum snum_to_unum(snum n)
{
        return(snum_abs(n));
}
real snum_to_real(snum n)
{
        char s[1024];
        x64toa(n,s);
        return(real_create(s));
}
snum snum_add(snum n1, snum n2)
{
        snum n;
        n = snum_dublicate(n1);
         
        x64add(n,n2);
        return(n);
}
snum snum_dublicate(snum inh)
{
        snum n;
        n = snum_create_();
        memmove(n,inh,sizeof(*inh));
        return(n);
}
static snum snum_create_(void)
{
        snum n;
        { ( n ) = ( snum )malloc( sizeof(*( n )) ) ; if (( n )== ((void *)0) ) ein_neur_name= fatal_error__("1.c",20572)+fatal_error_ ("Out of memory (malloc)"); }; ;
        return(n);
}
snum snum_dublicate_whole(snum inh)
{
                return(snum_dublicate(inh));
}
snum snum_abs(snum i)
{
        snum n;
        n = snum_dublicate(i);
        x64abs(n);
         
        return(n);
}
int snum_eq(snum e1, snum e2)
{
        return(x64cmp(e1,e2)==0);
}
int snum_neq(snum e1, snum e2)
{
        return(x64cmp(e1,e2)!=0);
}
snum snum_create(char *s)
{
        snum h;
        char *x;
        h= snum_create_();
        x=atox64(h,s);
         
        return(h);
}
void snum_free(snum h)
{
        free((void *) h ) ;
}
void snum_destroy_whole(snum h)
{
        free((void *) h ) ;
}
char *snum_print(snum l)
{
        char *s;
        s = (char*)malloc( sizeof(char)*20 ) ;
        x64toa(l,s);
        return(s);
}
char *snum_vcgprint(snum l)
{
        char *s;
        s = (char*)malloc( sizeof(char)*20 ) ;
        x64toa(l,s);
        return(s);
}
void snum_print_stdout(snum l)
{
        char *s;
        s = snum_print(l);
        fprintf((&_iob[1]) ,"%s",s);
        free((void *) s ) ;
}
int  snum_le(snum a,snum b)
{
        return(x64cmp(a,b)<0);
}
int  snum_leq(snum a,snum b)
{
        return(x64cmp(a,b)<=0);
}
int  snum_ge(snum a,snum b)
{
        return(x64cmp(a,b)>0);
}
int  snum_geq(snum a,snum b)
{
        return(x64cmp(a,b)>=0);
}
snum snum_modulo(snum a,snum b)
{
        snum n;
        n = snum_dublicate(a);
        x64mod(n,b);
         
        return(n);
}
snum snum_mal(snum a,snum b)
{
        snum n;
        n = snum_dublicate(a);
        x64mul(n,b); 
         
        return(n);
}
snum snum_min(snum a,snum b)
{
        snum n;
        n = snum_dublicate(a);
        x64sub(n,b);
         
        return(n);
}
snum snum_hoch(snum a,snum b)
{
        snum h;
        if (f80tox64(h,fpow80(x64tof80(a),x64tof80(b)))!=0) 
                ein_neur_name= fatal_error__("1.c",20753)+fatal_error_ ("Overflow");
        return(h);
}
snum snum_geteilt(snum a,snum b)
{
        snum n;
        n = snum_dublicate(a);
        x64div(n,b);
         
        return(n);
}
snum snum_unplus(snum a)
{
        return(a);
}
snum snum_unmin(snum a)
{
        snum n;
        n = snum_dublicate(a);
        x64neg(n);
         
        return(n);
}
snum snum_rnd(snum a)
{
        return(a);
}
snum snum_sgn(snum a)
{
        if (snum_le(a,s_null)) 
                return(snum_unmin(s_eins));
        if (snum_ge(a,s_null)) 
                return(s_eins);
        return(s_null);
}
snum snum_bor(snum a,snum b)
{
        ein_neur_name= fatal_error__("1.c",20803)+fatal_error_ ("no bor implemented");
        return(((void *)0) );
}
snum snum_band(snum a,snum b)
{
        ein_neur_name= fatal_error__("1.c",20809)+fatal_error_ ("no band implemented");
        return(((void *)0) );
}
snum snum_to_snum(snum a)
{
        return(a);
}
static int     *VISITED;
static int     *THERE;
int            *DFSNUM;
static int     *PRES_L;
static int     *PRES_R;
static int      n_o_n;
static int      dfsnum;
static int      dfsmode;
typedef struct _1_ {
        KFG             cfg;
        struct _1_     *next;
}               ADJ, *PADJ;
PADJ           *ADJACENT = ((void *)0) ;
extern double get_time();
static KFG_NODE *queue = ((void *)0) ;
static int      head = 0, tail = 0;
static int      size = -1;
static KFG_NODE *stack = ((void *)0) ;
static int      stackp = 0;
static int      s_size = -1;
build_szk(int m_mode);
stack_free()
{
        ((void) (( stack != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   21004 ,  "stack != NULL" ), 0) )) ;
        free((void *)stack);
}
queue_free()
{
        free((void *)queue);
}
init_queue(int anfang)
{
        queue = (KFG_NODE *) calloc( anfang ,  sizeof(KFG_NODE) ) ;
        size = anfang;
        head = tail = 0;
}
init_stack(int anfang)
{
        stack = (KFG_NODE *) calloc( anfang ,  sizeof(KFG_NODE) ) ;
        s_size = anfang;
        stackp = 0;
}
int 
queue_empty(void)
{
        ((void) (( (queue != ((void *)0) ) && (size != -1) ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",          "1.c" ,   21037 ,  "(queue != NULL) && (size != -1)" ), 0) )) ;
        return (head == tail);
}
KFG_NODE 
queue_get(void)
{
        KFG_NODE        t;
        ((void) (( queue != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   21045 ,  "queue != NULL" ), 0) )) ;
        t = queue[head++];
        if (head >= size)
                head = 0;
        return t;
}
void 
queue_put(KFG_NODE cfg)
{
        ((void) (( queue != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   21055 ,  "queue != NULL" ), 0) )) ;
        queue[tail++] = cfg;
        if (tail >= size)
                tail = 0;
        ((void) (( head != tail ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   21059 ,  "head != tail" ), 0) )) ;
}
KFG_NODE 
stack_get(void)
{
        KFG_NODE        t;
        ((void) (( stack != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   21066 ,  "stack != NULL" ), 0) )) ;
        ((void) (( stackp > 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",       "1.c" ,   21067 ,  "stackp > 0" ), 0) )) ;
        t = stack[--stackp];
        return t;
}
void 
stack_put(KFG_NODE cfg)
{
        ((void) (( stack != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   21075 ,  "stack != NULL" ), 0) )) ;
        stack[stackp++] = cfg;
}
int 
do_bfs(KFG_NODE cfg, int j)
{
        KFG_NODE_LIST   cfgcur;
        KFG_NODE        cfg2, cf;
        if (VISITED[kfg_get_id(K, cfg ) ] == 1)
                return 0;
        queue_put(cfg);
        while (!queue_empty()) {
                cf = queue_get();
                VISITED[kfg_get_id(K, cf ) ] = 1;
                DFSNUM[kfg_get_id(K, cf ) ] = dfsnum++;
                cfgcur = kfg_successors(K, cf ) ;
                while (!node_list_is_empty(cfgcur)) {
                        cfg2 = node_list_head(cfgcur);
                        if (VISITED[kfg_get_id(K, cfg2 ) ] != 1)
                                if ((j == -1) || (THERE[kfg_get_id(K, cfg2 ) ] == j))
                                        queue_put(cfg2);
                        cfgcur = node_list_tail(cfgcur);
                }
        }
        return 0;
}
int 
do_dfs(KFG_NODE cfg, int j)
{
        KFG_NODE_LIST   cfgcur;
        KFG_NODE        cfg2;
        if (VISITED[kfg_get_id(K, cfg ) ] == 1)
                return 0;
        VISITED[kfg_get_id(K, cfg ) ] = 1;
        DFSNUM[kfg_get_id(K, cfg ) ] = dfsnum++;
        cfgcur = kfg_successors(K, cfg ) ;
        while (!node_list_is_empty(cfgcur)) {
                cfg2 = node_list_head(cfgcur);
                if ((j == -1) || (THERE[kfg_get_id(K, cfg2 ) ] == j))
                        do_dfs(cfg2, j);
                cfgcur = node_list_tail(cfgcur);
        }
}
static int      number = 0;
static int      szk = 0;
int 
do_szk(KFG_NODE cfg, int mode)
{
        KFG_NODE_LIST   lll;
        KFG_NODE_LIST   cfgcur;
        int             min, m;
        KFG_NODE        fg, cfg2;
        VISITED[kfg_get_id(K, cfg ) ] = ++number;
        min = number;
        stack_put(cfg);
        if ((kfg_node_type(K, cfg) != END) || (mode == 0)) {
                cfgcur = kfg_successors(K, cfg ) ;
                while (!node_list_is_empty(cfgcur)) {
                        cfg2 = node_list_head(cfgcur);
                        if ((mode == 0) || (kfg_node_type(K, cfg2) != START)) {
                                m = (VISITED[kfg_get_id(K, cfg2 ) ] == -1) ? do_szk(cfg2, mode)
                                        : VISITED[kfg_get_id(K, cfg2 ) ];
                                if (m < min)
                                        min = m;
                        }
                        cfgcur = node_list_tail(cfgcur);
                }
        }
        if (min == VISITED[kfg_get_id(K, cfg ) ]) {
                PADJ            hh;
                while ((fg = stack_get()) != cfg) {
                        THERE[kfg_get_id(K, fg ) ] = szk;
                        VISITED[kfg_get_id(K, fg ) ] = n_o_n + 10;
                        hh = (PADJ) malloc(sizeof(ADJ));
                        hh->cfg = fg;
                        hh->next = ADJACENT[szk];
                        ADJACENT[szk] = hh;
                }
                THERE[kfg_get_id(K, fg ) ] = szk;
                VISITED[kfg_get_id(K, fg ) ] = n_o_n + 10;
                hh = (PADJ) malloc(sizeof(ADJ));
                hh->cfg = fg;
                hh->next = ADJACENT[szk];
                ADJACENT[szk] = hh;
                szk++;
        }
        return min;
}
void 
sort(KFG KK, int jj)
{
        KFG_NODE_LIST   listcfgcur;
        KFG_NODE_LIST   listcfg, intras;
        KFG_NODE        cfg;
        double   start;
        start = get_time();
        K = KK;
        dfsmode = jj;
        n_o_n = 10 + kfg_anz_node(K);
        if (dfsmode != 0) {
                listcfg = kfg_all_nodes(K);
                VISITED = (int *) malloc( n_o_n * sizeof(int) ) ;
                THERE = (int *) malloc( n_o_n * sizeof(int) ) ;
                DFSNUM = (int *) malloc( n_o_n * sizeof(int) ) ;
                PRES_L = (int *) malloc( n_o_n * sizeof(int) ) ;
                PRES_R = (int *) malloc( n_o_n * sizeof(int) ) ;
                ADJACENT = (PADJ *) calloc( n_o_n ,  sizeof(PADJ) ) ;
                if ((VISITED == ((void *)0) ) || (THERE == ((void *)0) ) || (DFSNUM == ((void *)0) )
                    || (PRES_L == ((void *)0) ) || (PRES_R == ((void *)0) )) {
                        fprintf((&_iob[2]) , "memory exhausted (sorting) \n");
                        exit(1);
                }
                listcfgcur = listcfg;
                while (!node_list_is_empty(listcfgcur)) {
                        cfg = node_list_head(listcfgcur);
                        THERE[kfg_get_id(K, cfg ) ] = -1;
                        VISITED[kfg_get_id(K, cfg ) ] = -1;
                        DFSNUM[kfg_get_id(K, cfg ) ] = -1;
                        PRES_R[kfg_get_id(K, cfg ) ] = PRES_L[kfg_get_id(K, cfg ) ] = node_list_length(kfg_predecessors(K, cfg ) );
                        listcfgcur = node_list_tail(listcfgcur);
                }
                dfsnum = 1;
                if (dfsmode == 2)
                        init_queue(n_o_n);
                if ((dfsmode == 3) || (dfsmode == 4) || (dfsmode == 5)) {
                        init_stack(n_o_n);
                        init_queue(n_o_n);
                }
                intras = kfg_entrys(K);
                while (!node_list_is_empty(intras)) {
                        cfg = node_list_head(intras);
                        if (node_list_length(kfg_predecessors(K, cfg ) ) == 0) {
                                if (dfsmode == 2)
                                        do_bfs(cfg, -1);
                                if (dfsmode == 1)
                                        do_dfs(cfg, -1);
                                if (dfsmode == 3)
                                        do_szk(cfg, 0);
                                if (dfsmode == 4)
                                        do_szk(cfg, 1);
                                if (dfsmode == 5)
                                        do_szk(cfg, 2);
                        }
                        intras = node_list_tail(intras);
                }
                listcfgcur = listcfg;
                while (!node_list_is_empty(listcfgcur)) {
                        cfg = node_list_head(listcfgcur);
                        if ((kfg_node_type(K, cfg) == START) &&
                            (THERE[kfg_get_id(K, cfg ) ] == -1)) {
                                if (dfsmode == 2)
                                        do_bfs(cfg, -1);
                                if (dfsmode == 1)
                                        do_dfs(cfg, -1);
                                if (dfsmode == 3)
                                        do_szk(cfg, 0);
                                if (dfsmode == 4)
                                        do_szk(cfg, 1);
                                if (dfsmode == 5)
                                        do_szk(cfg, 2);
                        }
                        listcfgcur = node_list_tail(listcfgcur);
                }
                if ((dfsmode == 3) || (dfsmode == 4) || (dfsmode == 5)) {
                        listcfgcur = listcfg;
                        while (!node_list_is_empty(listcfgcur)) {
                                cfg = node_list_head(listcfgcur);
                                VISITED[kfg_get_id(K, cfg ) ] = 0;
                                listcfgcur = node_list_tail(listcfgcur);
                        }
                        build_szk(dfsmode);
                }
                if (dfsmode == 2)
                        queue_free();
                if ((dfsmode == 3) || (dfsmode == 4) || (dfsmode == 5)) {
                        stack_free();
                        queue_free();
                }
        free((void *) ADJACENT ) ;
        free((void *) PRES_L ) ;
        free((void *) PRES_R ) ;
        free((void *) VISITED ) ;
        free((void *) THERE ) ;
        } else {
                DFSNUM = (int *) calloc( n_o_n ,  sizeof(int) ) ;
        }
fprintf((&_iob[2]) ,"+ sorting done in %lf \n",get_time() - start);
}

static int     *top_szk = ((void *)0) ;
static int     *unfertig = ((void *)0) ;
print_szk(KFG_NODE_LIST all, int szk, int *top, int mode);
on_cycle(KFG_NODE cfg, KFG_NODE_LIST all);
build_szk(int m_mode)
{
        int             i, j;
        KFG_NODE_LIST   all;
        KFG_NODE        cfg, cfg2;
        KFG_NODE_LIST   lcur, listcur;
        PADJ            ll;
        struct _iobuf            *fp;
        int             erfolg;
        int             mode;
        mode = m_mode == 4;
        erfolg = 0;
        all = kfg_all_nodes(K);
        print_szk(all, szk, ((void *)0) , mode);
        top_szk = (int *) malloc((2 + szk) * sizeof(int));
        unfertig = (int *) malloc((2 + szk) * sizeof(int));
        ((void) (( unfertig != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   21226 ,  "unfertig != NULL" ), 0) )) ;
        ((void) (( top_szk != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",          "1.c" ,   21227 ,  "top_szk != NULL" ), 0) )) ;
        for (i = 0; i < szk; i++) {
                unfertig[i] = 1;
                top_szk[i] = -1;
        }
        {
                int             mm;
                int             such, re, jj;
                int             topsorted = 0;
                int             found = 0;
                jj = 1;
                while (erfolg != szk) {
                        such = 0;
                        jj++;
                        re = 0;
                        found = -1;
                        for (j = 0; j < szk; j++) {
                                if (unfertig[j]) {
                                        re = 1;
                                        ll = ADJACENT[j];
                                        while (ll != ((void *)0) ) {
                                                ((void) (( j == THERE[kfg_get_id(K, ll->cfg ) ] ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   21261 ,  "j == THERE[id(ll->cfg)]" ), 0) )) ;
                                                lcur = kfg_predecessors(K, ll->cfg ) ;
                                                while (!node_list_is_empty(lcur)) {
                                                        cfg2 = node_list_head(lcur);
                                                        if ((j != THERE[kfg_get_id(K, cfg2 ) ]) &&
                                                            ((unfertig[THERE[kfg_get_id(K, cfg2 ) ]]))) {
                                                                re = 0;
                                                                goto loop;
                                                        }
                                                        lcur = node_list_tail(lcur);
                                                }
                                                ll = ll->next;
                                        }
                        loop:
                                        if (re == 1) {
                                                ((void) (( top_szk[j] == -1 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   21287 ,  "top_szk[j] == -1" ), 0) )) ;
                                                top_szk[j] = topsorted++;
                                                unfertig[j] = 0;
                                                erfolg++;
                                                found = j;
                                                update(j);
                                        }
                                }
                        }
                        if (found == -1) {
                                int             cand = -1;
                                int   cand1,cand2,cand3;
                                int   deg1,deg2,deg3,r,t;
                                deg1 = deg2 = deg3  = n_o_n;
                                cand1 = cand2 = cand3 = -1;
                                listcur = all;
                                while (!node_list_is_empty(listcur)) {
                                        cfg = node_list_head(listcur);
                                        if ((unfertig[THERE[kfg_get_id(K, cfg ) ]]) &&
                                            (on_cycle(cfg, all))) {
                                         r = PRES_R[kfg_get_id(K, cfg ) ];t=THERE[kfg_get_id(K, cfg ) ];
                                         switch(kfg_node_type(K, cfg)) {
                                             case START:     
                                              if (r < deg1) {
                                                cand1 = t;
                                                deg1 =  r;
                                                if (deg1 == 0) goto have;
                                              } break;
                                             case RETURN:
                                              if (r  < deg2) {
                                                cand2 = t;
                                                deg2 =  r;
                                              } break;
                                             default:
                                              if (r < deg3) {
                                                cand3 =  t;
                                                deg3 = r;
                                              } 
                                        }}
                                        listcur = node_list_tail(listcur);
                                }
                                have:
                                if (cand1 != -1) cand = cand1;
                                else if (cand2 != -1) cand = cand2;
                                else cand = cand3;
                                if (cand == -1)
                                        print_szk(all, szk, -1, mode);
                                ((void) (( cand != -1 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",       "1.c" ,   21345 ,  "cand != -1" ), 0) )) ;
                                ((void) (( cand < szk ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",       "1.c" ,   21346 ,  "cand < szk" ), 0) )) ;
                                ((void) (( top_szk[cand] == -1 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   21347 ,  "top_szk[cand] == -1" ), 0) )) ;
                                top_szk[cand] = topsorted++;
                                erfolg++;
                                unfertig[cand] = 0;
                                update(cand);
                        }
                        fprintf((&_iob[2]) , "==> %2.2lf%%\b\b\b\b\b\b\b\b\b\b", (double) erfolg * 100 / (double) szk);
                }
                fprintf((&_iob[2]) , "\n");
        }
        print_szk(all, szk, top_szk, mode);
        for (i = 0; i < n_o_n; i++)
                VISITED[i] = 0;
        for (i = 0; i < szk; i++)
                for (j = 0; j < szk; j++)
                        if (top_szk[j] == i) {
                                listcur = all;
                                while (!node_list_is_empty(listcur)) {
                                        cfg = node_list_head(listcur);
                                        if (THERE[kfg_get_id(K, cfg ) ] == j) {
                                                lcur = kfg_predecessors(K, cfg ) ;
                                                while (!node_list_is_empty(lcur)) {
                                                        cfg2 = node_list_head(lcur);
                                                        if (top_szk[THERE[kfg_get_id(K, cfg2 ) ]] != j)
                                                                if (m_mode == 5)
                                                                        do_dfs(cfg, j);
                                                                else
                                                                        do_bfs(cfg, j);
                                                        lcur = node_list_tail(lcur);
                                                }
                                        }
                                        listcur = node_list_tail(listcur);
                                }
                                listcur = all;
                                while (!node_list_is_empty(listcur)) {
                                        cfg = node_list_head(listcur);
                                        if (THERE[kfg_get_id(K, cfg ) ] == j)
                                                if (m_mode == 5)
                                                        do_dfs(cfg, j);
                                                else
                                                        do_bfs(cfg, j);
                                        listcur = node_list_tail(listcur);
                                }
                        }
}
print_szk(KFG_NODE_LIST all, int szk, int *top, int mode)
{
        struct _iobuf            *fp;
        int             i;
        KFG_NODE_LIST   listcur;
        PADJ            lcur;
        KFG_NODE        cfg, cfg2;
        extern int      dfsmode;
        switch ((int) top) {
        case -1:
                fp = fopen("CAND", "w");
                break;
        case 0:
                fp = fopen("SZK", "w");
                break;
        default:
                fp = fopen("T_SZK", "w");
        }
        fprintf(fp, "graph: { \n");
        switch (dfsmode) {
        case 1:
                fprintf(fp, "title: \"dfs algorithm\"\n");
                break;
        case 2:
                fprintf(fp, "title: \"bfs algorithm\"\n");
                break;
        case 3:
                fprintf(fp, "title: \"szk algorithm\"\n");
                break;
        case 4:
                fprintf(fp, "title: \"szk (intra) bfs  algorithm\"\n");
                break;
        case 5:
                fprintf(fp, "title: \"szk (intra) dfs  algorithm\"\n");
                break;
dafault:        fprintf((&_iob[2]) , "unknown dfsmode \n");
                exit(1);
        }
        for (i = 0; i < szk; i++) {
                fprintf(fp, "node: { title:\"%d\"\n", i);
                if (((int) top == -1) && (unfertig[i]))
                        fprintf(fp, "        color:red\n");
                fprintf(fp, "        label:\"");
                lcur = ADJACENT[i];
                while (lcur != ((void *)0) ) {
                        cfg = lcur->cfg;
                        if (THERE[kfg_get_id(K, cfg ) ] == i)
                                fprintf(fp, "%d ", kfg_get_id(K, cfg ) );
                        lcur = lcur->next;
                }
                if ((top != ((void *)0) ) && ((int) top != -1))
                        fprintf(fp, "\n++ %d ++\n", top[i]);
                fprintf(fp, "\n** %d **\"\n}\n", i);
        }
        for (i = 0; i < szk; i++) {
                lcur = ADJACENT[i];
                while (lcur != ((void *)0) ) {
                        cfg = lcur->cfg;
                        if (THERE[kfg_get_id(K, cfg ) ] == i) {
                                listcur = kfg_successors(K, cfg ) ;
                                while (!node_list_is_empty(listcur)) {
                                        cfg2 = node_list_head(listcur);
                                        if (THERE[kfg_get_id(K, cfg2 ) ] != i) {
                                                fprintf(fp, "edge: {sourcename:\"%d\" \n", i);
                                                fprintf(fp, "       targetname:\"%d\" \n", THERE[kfg_get_id(K, cfg2 ) ]);
                                                if (mode) {
                                                        if ((kfg_node_type(K, cfg2) == START) ||
                                                            (kfg_node_type(K, cfg) == END))
                                                                fprintf(fp, "       linestyle:dotted \n");
                                                }
                                                fprintf(fp, "\n}\n");
                                        }
                                        listcur = node_list_tail(listcur);
                                }
                        }
                        lcur = lcur->next;
                }
        }
        fprintf(fp, "}\n");
        fclose(fp);
}
static int      initia = 0;
static int     *ctable = ((void *)0) ;
static int 
reach(KFG_NODE root, KFG_NODE cfg)
{
        KFG_NODE_LIST   listcfgcur;
        KFG_NODE        cfg2;
        if (root == cfg) {
                ctable[kfg_get_id(K, root ) ] = 1;
                return 1;
        }
        if (ctable[kfg_get_id(K, root ) ] != -1)
                return ctable[kfg_get_id(K, root ) ];
        VISITED[kfg_get_id(K, cfg ) ] = 1;
        listcfgcur = kfg_successors(K, cfg ) ;
        while (!node_list_is_empty(listcfgcur)) {
                cfg2 = node_list_head(listcfgcur);
                if (!VISITED[kfg_get_id(K, cfg2 ) ])
                        if (reach(root, cfg2)) {
                                ctable[kfg_get_id(K, root ) ] = 1;
                                ctable[kfg_get_id(K, cfg2 ) ] = 1;
                                return 1;
                        }
                listcfgcur = node_list_tail(listcfgcur);
        }
        return 0;
}
int 
on_cycle(KFG_NODE cfg, KFG_NODE_LIST all)
{
        KFG_NODE_LIST   listcfgcur;
        KFG_NODE        cfg2;
        if (!initia) {
                int             j = 10 + node_list_length(all);
                int             i;
                initia = j;
                ctable = (int *) malloc((j + 1) * sizeof(int));
                ((void) (( ctable != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   21547 ,  "ctable != NULL" ), 0) )) ;
                for (i = 0; i <= j; i++)
                        ctable[i] = -1;
        }
        ((void) (( (kfg_get_id(K, cfg )  >= 0) && (kfg_get_id(K, cfg )  < initia) ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   21551 ,  "(id(cfg) >= 0) && (id(cfg) < initia)" ), 0) )) ;
        switch (ctable[kfg_get_id(K, cfg ) ]) {
        case 1:
                return 1;
        case 0:
                return 0;
        default:
                listcfgcur = all;
                while (!node_list_is_empty(listcfgcur)) {
                        cfg2 = node_list_head(listcfgcur);
                        VISITED[kfg_get_id(K, cfg2 ) ] = 0;
                        listcfgcur = node_list_tail(listcfgcur);
                }
                listcfgcur = kfg_successors(K, cfg ) ;
                while (!node_list_is_empty(listcfgcur)) {
                        cfg2 = node_list_head(listcfgcur);
                        if (reach(cfg, cfg2)) {
                                ctable[kfg_get_id(K, cfg ) ] = 1;
                                ctable[kfg_get_id(K, cfg2 ) ] = 1;
                                return 1;
                        }
                        listcfgcur = node_list_tail(listcfgcur);
                }
                ctable[kfg_get_id(K, cfg ) ] = 0;
                return 0;
        }
        return 0;
}
update(int cand)
{
        KFG_NODE        cfg;
        KFG_NODE_LIST   sucs;
        PADJ            hh = ADJACENT[cand];
        while (hh != ((void *)0) ) {
                cfg = hh->cfg;
                sucs = kfg_successors(K, cfg ) ;
                while (!node_list_is_empty(sucs)) {
                        PRES_R[kfg_get_id(K, node_list_head(sucs) ) ]--;
                        ((void) (( PRES_R[kfg_get_id(K, node_list_head(sucs) ) ] >= 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",       "1.c" ,   21606 ,  "PRES_R[id(node_list_head(sucs))] >= 0" ), 0) )) ;
                        sucs = node_list_tail(sucs);
                }
                hh = hh->next;
        }
}
long str_power = -1 ;
char *str_len(void)
{
   ein_neur_name= fatal_error__("1.c",21670)+fatal_error_ ("len in str");
}
unsigned int str_hash(str x) {
        char *y;
        int *i;
        unsigned int h=0;
        for(y=x,i=prime;y!='\0' && i!=last_prime;y++)
                h += *y * *i;
        return(h);
}
void str_init(void)
{
        static int  flag = 0 ;
        flag = 1 ;
}
static str str_create_(int  len)
{
        str x;
        x = (str)malloc( sizeof(char)*len ) ;
        if (x== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",21708)+fatal_error_ ("Memory runoff");
        return(x);
}
str str_conc(str s1, str s2) {
        str s;
        s = str_create_(strlen(s1)+strlen(s2)+1);
        strcpy(s,s1);
        strcat(s,s2);
        return(s);
}
str str_dublicate(str inh)
{
        return(strcpy(str_create_(strlen(inh)+1),inh));
}
str str_dublicate_whole(str inh)
{
                return(str_dublicate(inh));
}
int str_eq(str e1, str e2)
{
        return(!strcmp(e1,e2));
}
int str_neq(str e1, str e2)
{
        return(!str_eq(e1,e2));
}
str str_create(char *i)
{
        return(str_dublicate(i));
}
void str_free(str h)
{
        free((void *) h ) ;
}
void str_destroy_whole(str h)
{
        free((void *) h ) ;
}
char *str_print(str l)
{
         char *s;
         s = str_dublicate(l);
         return(s);
}
char *str_vcgprint(str l)
{
         char *s;
         s = str_dublicate(l);
         return(s);
}
void str_print_stdout(str l)
{
        fprintf((&_iob[1]) ,"%s",l);
}
int  str_le(str a,str b)
{
        return(strcmp(a,b)<0);
}
int  str_leq(str a,str b)
{
        return(strcmp(a,b)<=0);
}
int  str_ge(str a,str b)
{
        return(strcmp(a,b)>0);
}
int  str_geq(str a,str b)
{
        return(strcmp(a,b)>=0);
}
int  str_is_top(str a)
{
        ein_neur_name= fatal_error__("1.c",21814)+fatal_error_ ("No top");
        return(0);
}
int  str_is_bottom(str a)
{
        ein_neur_name= fatal_error__("1.c",21821)+fatal_error_ ("No bot");
        return(0);
}
str str_top(void)
{
        ein_neur_name= fatal_error__("1.c",21828)+fatal_error_ ("No top");
        return(((void *)0) );
}
str str_bottom(void)
{
        ein_neur_name= fatal_error__("1.c",21835)+fatal_error_ ("No bot");
        return(((void *)0) );
}
str str_lub(str a,str b)
{
        ein_neur_name= fatal_error__("1.c",21842)+fatal_error_ ("No bot");
        return(((void *)0) );
}
str str_glb(str a,str b)
{
        ein_neur_name= fatal_error__("1.c",21849)+fatal_error_ ("No bot");
        return(((void *)0) );
}
typedef int     faultcode_t;     
void    (*signal())();
void  (*sigset())();
int   sighold  (int)  ;
int   sigrelse  (int)  ;
int   sigignore  (int)  ;
struct  sigvec {
        void    (*sv_handler)();         
        int     sv_mask;                 
        int     sv_flags;                
};
struct  sigstack {
        char    *ss_sp;                  
        int     ss_onstack;              
};
struct  sigcontext {
        int     sc_onstack;              
        int     sc_mask;                 
        int     sc_sp;                   
        int     sc_pc;                   
        int     sc_npc;                  
        int     sc_psr;                  
        int     sc_g1;                   
        int     sc_o0;
        int     sc_wbcnt;                
        char    *sc_spbuf[31 ];  
        int     sc_wbuf[31 ][16];  
};
struct  sigaction {
        void            (*sa_handler)();
        sigset_t        sa_mask;
        int             sa_flags;
};
void    (*signal())();
int     kill  (int , int)  ;
int     sigaction  (int, const struct sigaction *, struct sigaction *)  ;
int     sigaddset  (sigset_t *, int)  ;
int     sigdelset  (sigset_t *, int)  ;
int     sigemptyset  (sigset_t *)  ;
int     sigfillset  (sigset_t *)  ;
int     sigismember  (const sigset_t *, int)  ;
int     sigpending  (sigset_t *)  ;
int     sigprocmask  (int, const sigset_t *, sigset_t *)  ;
int     sigsuspend  (sigset_t *)  ;
typedef int ident;
ident ident_lookup ( char * );
ident ident_insert ( char * );
int ident_hashinit ( void );
char *ident_decode ( ident );
int ident_settable ( int );
int ident_symtab ( struct _iobuf  * );
void ident_print_stdout ( ident );
char *ident_get_version ( void );
int  ident_eq ( ident, ident );
ident ident_dublicate ( ident );
ident ident_dublicate_whole ( ident );
char *ident_print ( ident );
void ident_destroy ( ident );
void ident_destroy_whole ( ident );
static char *memory ( unsigned int );
static ident hash ( char * );
static void forbid ( void );
static void permit ( void );
static void exit_handler ( int );
static char version[] = "";
typedef struct node {
    ident num;
    struct node *next;
} *hashentry, HASH_NODE;
static int stringcode = 0;
static hashentry *hashtable = ((void *)0) ;
static char **_decode = ((void *)0) ;
static int hash_size = 211;
static int table_size = 20000;
static int is_init = 0;
static char *memory(size)
register unsigned int size;
{
    register char *p = (char *) calloc(size, 1);
    if (p == ((void *)0) )
        ein_neur_name= fatal_error__("1.c",21919)+fatal_error_ ("insufficient memory");
    return (p);
}
static ident hash(string)
register char *string;
{
    register int hashval = 0;
    register char *s = string;
    for (hashval = 0; *s != '\0';)
        hashval += *s++;
    hashval = (hashval < 0) ? -hashval : hashval;
    return (hashval % hash_size);
}
ident ident_lookup(char *s)
{
    register hashentry help;
    if (!is_init)
        ein_neur_name= fatal_error__("1.c",21955)+fatal_error_ ("library not initialized\n");
    help = hashtable[hash(s)];
    while (help != ((void *)0) ) {
        if (strcmp(s, _decode[(( help ) -> num) ]) == 0)
            return ((( help ) -> num) );
        help = (( help ) -> next) ;
    }
    return ((-1) );
}
static void forbid()
{
    if ((int) signal(2 , (void (*)())1 ) == -1)
        ein_neur_name= fatal_error__("1.c",21970)+fatal_error_ ("can't forbid ^C process signal\n");
}
static void permit()
{
    if ((int) signal(2 , exit_handler) == -1)
        ein_neur_name= fatal_error__("1.c",21976)+fatal_error_ ("can't set ^C process signal\n");
}
static void exit_handler(int x)
{
    exit(x);
}
ident ident_insert(char *s)
{
    register hashentry new_node;
    register int hashval;
    register int look;
        if (!is_init)
                ein_neur_name= fatal_error__("1.c",21996)+fatal_error_ ("library not initialized\n");
        ((void) (((void *)  strcmp(s,"")!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   21998 ,  "strcmp(s,\"\")!=NULL" ), 0) )) ;
    forbid();
    hashval = hash(s);
    look = ident_lookup(s);
    if (look == (-1) ) {
        if (stringcode - 1 == table_size) {
            ein_neur_name= fatal_error__("1.c",22005)+fatal_error_ ("table space exhausted");
            exit(1);
        }
        new_node = (hashentry) memory(sizeof(HASH_NODE));
        (( new_node ) -> num)  = stringcode++;
        (( new_node ) -> next)  = hashtable[hashval];
        _decode[(( new_node ) -> num) ] = strcpy(memory((int) (strlen(s) + 1)), s);
        hashtable[hashval] = new_node;
        look = (( new_node ) -> num) ;
    }
    permit();
    return (look);
}
int ident_hashinit(void)
{
    if (is_init)
        return ((-3) );
    hashtable = (hashentry *) calloc(hash_size, sizeof(hashentry));
    if (hashtable == ((void *)0) )
        return ((-4) );
    _decode = (char **) calloc(table_size, sizeof(char *));
    if (_decode == ((void *)0) )
        return ((-4) );
    is_init = 1;
    ident_insert("SYMTAB Library version 1.0");
    return ((-3) );
}
char *ident_decode(ident x)
{
   if (!is_init) {
                ein_neur_name= fatal_error__("1.c",22053)+fatal_error_ ("library not initialized\n");
                exit(1);
   }
        if ( x == (-1) )
                return("SYM_NOT_DEFINED");
   if ((x < 1) || (x >= stringcode)) {
                ein_neur_name= fatal_error__("1.c",22059)+fatal_error_ ("no valid code (%d)\n",x);
                exit(1);
   } else
                return (_decode[x]);
}
int ident_settable(int x)
{
    register int i;
    if (!is_init) {
        ein_neur_name= fatal_error__("1.c",22075)+fatal_error_ ("library not initialized\n");
        exit(1);
    }
    if (x < table_size)
        return ((-5) );
    else
        table_size = x;
    if (_decode != ((void *)0) ) {
        char **help = (char **) calloc(table_size, sizeof(char *));
        if (help == ((void *)0) )
            return ((-4) );
        for (i = 0; i < stringcode; i++)
            help[i] = _decode[i];
        free((void *)_decode);           
        _decode = help;
    }
    return ((-3) );
}
int ident_symtab(struct _iobuf  * fp)
{
    register int i;
    double fg;
    if (!is_init) {
        ein_neur_name= fatal_error__("1.c",22104)+fatal_error_ ("library not initialized\n");
        exit(1);
    }
    fg = 100.0 * (stringcode - 1) / (1.0 * table_size);
    fprintf(fp, "SYMTABLE:\n");
    fprintf(fp, "size %d -- occupied %f%% \n", table_size, fg);
    for (i = 1; i < stringcode; i++)
        fprintf(fp, "%5d: Identifier:%s  HashValue:%d\n", i, ident_decode(i), hash(ident_decode(i)));
    return ((-3) );
}
void ident_print_stdout(ident id)
{
         char *s;
         s = ident_print(id);
    fprintf((&_iob[1]) ,"%s",s);
         free((void *)s);
}
char *ident_get_version(void)
{
    return (version);
}
int  ident_eq(ident symb1, ident symb2)
{
        ((void) (( strcmp(ident_decode(symb1),ident_decode(symb2))==0?symb1==symb2:symb1!=symb2 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   22150 ,  "strcmp(ident_decode(symb1),ident_decode(symb2))==0?symb1==symb2:symb1!=symb2" ), 0) )) ;
        return (symb1 == symb2);
}
ident ident_dublicate(ident id)
{
        return(id);
}
ident ident_dublicate_whole(ident id)
{
        return(id);
}
char *ident_print(ident id)
{
    char s1[] = "Identifier:";
    char s2[] = "\n";
    char a1[10], *a2, *s;
    int len;
    sprintf(a1, "%5d:", id);
    a2 = ident_decode(id);
    len = strlen(s1) + strlen(s2) + strlen(a1) + strlen(a2) + 1;
    s = (char *) malloc(sizeof(char) * len);
         if (s== ((void *)0) ) 
                ein_neur_name= fatal_error__("1.c",22183)+fatal_error_ ("No free space");
    s[0] = '\0';
    strcat(s, a1);
    strcat(s, s1);
    strcat(s, a2);
    strcat(s, s2);
    return (s);
}
void ident_destroy(ident id)
{
}
void ident_destroy_whole(ident id)
{
}
static void print_o ( struct _iobuf  *, mirObject );
char * INT_get_value(INT i)
{
        char *s;
        s= (char*) malloc( sizeof(char)*24 ) ;
        if (s== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",22276)+fatal_error_ ("No memory!");
        sprintf(s,"%d",i);
        return(s);
}
        
char * UNIV_REAL_get_value(f80_t x)
{
        char *s;
        s= (char*) malloc( sizeof(char)*24 ) ;
        if (s== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",22293)+fatal_error_ ("No memory!");
        f80toa(s,"g",x);
        return(s);
}
char * UNIV_INT_get_value(x64_t x)
{
        char *s;
        s= (char*) malloc( sizeof(char)*24 ) ;
        if (s== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",22310)+fatal_error_ ("No memory!");
        x64toa(&x,s);
        return(s);
}
char * UNIV_ADDRESS_get_value(x64_t x)
{
        char *s;
        s= (char*) malloc( sizeof(char)*24 ) ;
        if (s== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",22326)+fatal_error_ ("No memory!");
        x64toa(&x,s);
        return(s);
}
void mirObject_print_stdout(mirObject ob)
{
        print_o((&_iob[1]) ,ob);
}
static void print_o(struct _iobuf  *fp,mirObject ob) 
{
        switch ((((DMCP_xnode)(   ob   ) )->op) )  {
                break; case xop_Sort10_mirLocal  : 
                        fprintf(fp," Local: ");
                        out_UNIV_INT(fp,(       0 , ((struct x_Sort10_mirLocal *)((  ob  ) ))->f_Offset) );
                        fprintf(fp,"(%x)",(     0 , ((struct x_Sort10_mirLocal *)((  ob  ) ))->f_Procedure) );
                        break ;
                break; case xop_Sort10_mirDataGlobal  : 
                        fprintf(fp," Global:");
                        out_NAME(fp,(   0 , ((struct x_Sort10_mirDataGlobal *)((  ob  ) ))->f_Name) );
                        break ;
                break; case xop_Sort10_mirField  : 
                        fprintf(fp," Field:  ");
                        out_UNIV_INT(fp,(       0 , ((struct x_Sort10_mirField *)((  ob  ) ))->f_Offset) );
                        break ;
                break; case xop_Sort10_mirParameter  : 
                        fprintf(fp," Parameter:  ");
                        out_UNIV_INT(fp,(       0 , ((struct x_Sort10_mirParameter *)((  ob  ) ))->f_Offset) );
                        fprintf(fp,"(%x)",(     0 , ((struct x_Sort10_mirParameter *)((  ob  ) ))->f_Procedure) );
                        break ;
                break; case xop_Sort10_mirPlaced  : 
                        fprintf(fp," Placed:  ");
                        out_UNIV_ADDRESS(fp,(   0 , ((struct x_Sort10_mirPlaced *)((  ob  ) ))->f_Address) );
                        break ;
                break; case xop_Sort10_mirProcGlobal  : 
                        fprintf(fp," ProcGlobal: ");
                        out_NAME(fp,(   0 , ((struct x_Sort10_mirProcGlobal *)((  ob  ) ))->f_Name) );
                        break ;
                break; case xop_Sort10_mirBasicBlock  : 
                        fprintf(fp," <BB%x> ",ob);
                        break ;
                break; default:; 
                        fprintf(fp,"unknown object %s ",opcode_name((((DMCP_xnode)(  ob  ) )->op) ));exit(0);
        }  
        if ((   0 , ((struct x_Sort10_mirDataGlobal *)((  ob  ) ))->f_IsVolatile) ) 
        fprintf(fp,"(volatile)");
        fprintf(fp," ");
}
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;
typedef struct abstract_tree_node {
          int  content;
          short int  sons;
          long  position;
          long  position2;
          short int intern;
                                        unsigned long id;
          union { struct abstract_tree_node  *son[1];
                  double  doubles;
                  int     ints;
                } indirect;
            } *PTREE,TREE_NODE;
void tree_print_label(PTREE,struct _iobuf *);
void tree_print_subgraph(PTREE,struct _iobuf *);
int tree_get_content ( PTREE );
char* tree_print_pos ( PTREE );
long tree_line ( PTREE );
int tree_pos ( PTREE );
long tree_line_ends ( PTREE );
int tree_pos_ends ( PTREE );
PTREE tree_get_son ( PTREE, int );
int tree_get_number_of_sons ( PTREE );
void tree_set ( yyltype  * );
void tree_set2 ( yyltype  *, yyltype  * );
PTREE tree_build_ ( int, ... );
PTREE tree_mkbasic ( int, int );
PTREE tree_mkdouble ( int, double );
unsigned long tree_get_nodeid ( PTREE );
int tree_get_intern ( PTREE );
int tree_get_int ( PTREE );
double tree_get_double ( PTREE );
void tree_print_stdout ( PTREE );
char* tree_print ( PTREE );
void tree_print_file ( PTREE, struct _iobuf  * );
char *tree_get_version ( void );
void tree_set_content ( PTREE, int );
void tree_set_son ( PTREE, PTREE, int );
unsigned long tree_get_maxid ( void );
unsigned long tree_renewid ( PTREE );
void tree_destroy_node ( PTREE );
void tree_edge_print ( PTREE, char * );
void tree_copy_pos ( PTREE, PTREE );
void tree_set_nodeid ( PTREE, int );
PTREE tree_copy ( PTREE );
void tree_set_int ( PTREE, int );
void tree_destroy_whole ( PTREE );
PTREE tree_dublicate ( PTREE );
int  tree_isbottom ( PTREE );
int  tree_istop ( PTREE );
PTREE tree_glb ( PTREE, PTREE );
PTREE tree_lub ( PTREE, PTREE );
int  tree_le ( PTREE, PTREE );
int  tree_leq ( PTREE, PTREE );
int  tree_eq ( PTREE, PTREE );
static PTREE tree_getmem ( int );
static long tree_to_pos_to ( yyltype  * );
static long tree_to_pos_from ( yyltype  * );
static void tree_renewid_rec ( PTREE );
static void tree_print_nodes ( PTREE, struct _iobuf * );
static void tree_print_edges ( PTREE, struct _iobuf  * );
static void tree_pprint ( PTREE, struct _iobuf *, int );
static PTREE tree_copy_node ( PTREE );
static unsigned long id_next= 0;
int tree_get_content(PTREE x)
{
        ((void) (( x != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22429 ,  "x != NULL" ), 0) )) ;
        return ((x) -> content);
}
char* tree_print_pos(PTREE vtree)
{
        char *s;
        s = (char*)malloc( sizeof(char)*1024 ) ;
        if (s== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",22442)+fatal_error_ ("Buy more Memory!");
        sprintf(s,"%ld,%d-%ld,%d",tree_line(vtree), tree_pos(vtree)
                ,tree_line_ends(vtree), tree_pos_ends(vtree));
        return(s);
}
long  tree_line(PTREE x) 
{
        ((void) (( sizeof(long) == 4 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22453 ,  "sizeof(long) == 4" ), 0) )) ;
        ((void) (( x != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22454 ,  "x != NULL" ), 0) )) ;
        return ((long) ((x)->position) & 0x00FFFFFF);
}
int tree_pos(PTREE x)
{
        ((void) (( sizeof(long) == 4 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22463 ,  "sizeof(long) == 4" ), 0) )) ;
        ((void) (( x != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22464 ,  "x != NULL" ), 0) )) ;
        return((int)((x)->position) >> 24) ;
}
long tree_line_ends(PTREE x)
{
        ((void) (( sizeof(long) == 4 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22474 ,  "sizeof(long) == 4" ), 0) )) ;
        ((void) (( x != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22475 ,  "x != NULL" ), 0) )) ;
        return((long)  ((x)->position2) & 0x00FFFFFF);
}
int tree_pos_ends(PTREE x)
{
        ((void) (( sizeof(long) == 4 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22484 ,  "sizeof(long) == 4" ), 0) )) ;
        ((void) (( x != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22485 ,  "x != NULL" ), 0) )) ;
        return( ((x)->position2) >> 24);
}
PTREE tree_get_son(PTREE p,int i)
{
        ((void) (( p!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   22495 ,  "p!=NULL" ), 0) )) ;
        ((void) (( i>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   22496 ,  "i>=0" ), 0) )) ;
        if (i>=p->sons)
                ((void) (( 0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22499 ,  "0" ), 0) )) ;
        return((p)->indirect.son[i]);
}
int tree_get_number_of_sons(PTREE x)
{
        ((void) (( x!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   22509 ,  "x!=NULL" ), 0) )) ;
        return (((x) -> sons));
}
static yyltype  pos_help;
void tree_set(yyltype  *x)
{
        ((void) (( x!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   22522 ,  "x!=NULL" ), 0) )) ;
        pos_help.first_line = x->first_line;
        pos_help.last_line  = x->last_line;
        pos_help.first_column =  x->first_column;
        pos_help.last_column  =  x->last_column;
}
void tree_set2(yyltype  *x,yyltype  *y)
{
        ((void) (( x!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   22537 ,  "x!=NULL" ), 0) )) ;
        ((void) (( y!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   22538 ,  "y!=NULL" ), 0) )) ;
        pos_help.first_line = x->first_line;
        pos_help.last_line  = y->last_line;
        pos_help.first_column =  x->first_column;
        pos_help.last_column  =  y->last_column;
}
static PTREE tree_getmem(int anz)
{
        register PTREE help;
        ((void) (( anz>=1 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   22554 ,  "anz>=1" ), 0) )) ;
        help = (PTREE) malloc( (anz-1)*sizeof(PTREE) + (sizeof(TREE_NODE))  ) ;
        if (help == ((void *)0) ) {
                ein_neur_name= fatal_error__("1.c",22558)+fatal_error_ ("memory exhausted");
        }
        return(help);
}
PTREE tree_build_( int cont , ...)
{
        va_list ap;
        PTREE help;
        int anz,i;
        ((void) (( cont>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",   "1.c" ,   22575 ,  "cont>0" ), 0) )) ;
        anz = 0;
        (__builtin_next_arg ( cont ),  ap  = (char *) __builtin_saveregs ()) ;
        
        do {
                help=(void *) 0; 
                anz+=1;
        }
        while (help != ((void *)0) ) ;
        anz-=1;
        ((void) (( anz<7 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   22585 ,  "anz<7" ), 0) )) ;  
        ((void) (( anz>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   22586 ,  "anz>0" ), 0) )) ;
        ((void)0) ;
        help = tree_getmem(anz);
        (__builtin_next_arg ( cont ),  ap  = (char *) __builtin_saveregs ()) ;
        i = 0;
        while(i<anz)
        {
                help->indirect.son[i++] = (void *) 0; 
        }
        ((void)0) ;
        help->sons = anz;
        help->content= cont;
        help->intern = 1;
        help->id = id_next++;
        help->position = tree_to_pos_from(&pos_help);
        help->position2 = tree_to_pos_to(&pos_help);
        
        return help;
}
PTREE tree_mkbasic(int cont,int value)
{
        register PTREE help;
        help = tree_getmem(1);
        help->content = cont;
        help->indirect.ints = value;
        help->sons = 0;
        help->intern = 2;
        help->id = id_next++;
        help->position = tree_to_pos_from(&pos_help);
        help->position2 = tree_to_pos_to(&pos_help);
        return (help);
}
PTREE tree_mkdouble(int cont,double value)
{
        register PTREE help;
        help = (PTREE) tree_getmem(1);
        help->content = cont;
        help->indirect.doubles = value;
        help->sons   = 0;
        help->intern = 3;
        help->id = id_next++;
        help->position = tree_to_pos_from(&pos_help);
        help->position2 = tree_to_pos_to(&pos_help);
        return (help);
} 
unsigned long tree_get_nodeid(PTREE node)
{
        ((void) (( node != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   22649 ,  "node != NULL" ), 0) )) ;
        return(node->id);
}
static long tree_to_pos_to(yyltype  *pos)
{
        int erg =0;
        ((void) (( sizeof(long) == 4 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22658 ,  "sizeof(long) == 4" ), 0) )) ;
        erg = pos->last_line & 0x00FFFFFF;
        erg = erg | (pos->last_column << 24 ) ;
        return(erg);
}
static long tree_to_pos_from(yyltype  *pos)
{
        int erg =0;
        ((void) (( sizeof(long) == 4 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22670 ,  "sizeof(long) == 4" ), 0) )) ;
        erg = pos->first_line & 0x00FFFFFF;
        erg = erg | (pos->first_column << 24 ) ;
        return(erg);
}
        
int tree_get_intern(PTREE x)
{
        return (x->intern);
}
int tree_get_int(PTREE x)
{
        ((void) (( x!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   22692 ,  "x!=NULL" ), 0) )) ;
        ((void) (( tree_get_intern(x)==2 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   22693 ,  "tree_get_intern(x)==2" ), 0) )) ;
        return (x->indirect.ints);
}
double tree_get_double(PTREE x)
{
        ((void) (( x!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   22700 ,  "x!=NULL" ), 0) )) ;
        ((void) (( tree_get_intern(x)==3 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   22701 ,  "tree_get_intern(x)==3" ), 0) )) ;
        return (x->indirect.doubles);
}
void tree_print_stdout(PTREE x)
{
        tree_print_file(x,(&_iob[1]) );
}
char* tree_print(PTREE x)
{
        ein_neur_name= fatal_error__("1.c",22714)+fatal_error_ ("No print for tree");
        return(((void *)0) );
}
void tree_print_file(PTREE x,struct _iobuf  *fp)
{
        ((void) (( x!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   22721 ,  "x!=NULL" ), 0) )) ;
        if (fp == ((void *)0) ) 
                fp = (&_iob[1]) ;
        ((void) (( fp!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22724 ,  "fp!=NULL" ), 0) )) ;
        tree_pprint(x,fp,1);
}
char *tree_get_version(void)
{
        return (version);
}
void tree_set_content(PTREE node,int con)
{
        ((void) (( node!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   22739 ,  "node!=NULL" ), 0) )) ;
        node->content = con;
}
void tree_set_son(PTREE node,PTREE son,int nr)
{
        ((void) (( node!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   22747 ,  "node!=NULL" ), 0) )) ;
        ((void) (( son!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",       "1.c" ,   22748 ,  "son!=NULL" ), 0) )) ;
        ((void) (( nr>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",    "1.c" ,   22749 ,  "nr>=0" ), 0) )) ;
        ((void) (( tree_get_intern(node)==1 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   22750 ,  "tree_get_intern(node)==1" ), 0) )) ;
        ((void) (( nr<tree_get_number_of_sons(node) ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   22751 ,  "nr<tree_get_number_of_sons(node)" ), 0) )) ;
        node->indirect.son[nr] = son;
}
unsigned long tree_get_maxid(void)
{
        return(id_next-1);
}
unsigned long tree_renewid(PTREE root)
{
        ((void) (( root!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   22764 ,  "root!=NULL" ), 0) )) ;
        id_next = 0;
        tree_renewid_rec(root);
        return(id_next-1);
}
static void tree_renewid_rec(PTREE node)
{
        int i,n;
        ((void) (( node!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   22776 ,  "node!=NULL" ), 0) )) ;
        node->id = id_next++;
        n = tree_get_number_of_sons(node);
        for(i=0;i<n;i++) {
                tree_renewid_rec(tree_get_son(node,i));
        }
}
void tree_destroy_node(PTREE node)
{
        ((void) (( node != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   22788 ,  "node !=NULL" ), 0) )) ;
        free((void *) node ) ;
}
        
static int *besucht;
void tree_edge_print(PTREE root,char *filename)
{
        struct _iobuf  *file;
        ((void) (( root != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   22799 ,  "root != NULL" ), 0) )) ;
        file = fopen(filename,"w");
        if (file == ((void *)0) )
        {
                fprintf((&_iob[2]) ,"unable to open %s",filename);
                exit(1);
        }
        fprintf(file,"graph: { title: \"%s\" \n","PARSE_TREE");
        fprintf(file,"layoutalgorithm: tree\n");
         
        besucht = (int*)calloc( tree_get_maxid()+1 , sizeof(int) ) ;
        if (besucht == ((void *)0) )
        {
                fprintf((&_iob[2]) ,"no more memory (buy more!)");
                exit(1);
        }
        tree_print_nodes(root,file);
        free((void *) besucht ) ;
        tree_print_edges(root,file);
        fprintf(file,"}\n");
        fclose(file);
}
static void tree_print_nodes(PTREE node,struct _iobuf * fl)
{
        int i,n;
        long node_id;
        static int sid = 0;
        ((void) (( node!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   22830 ,  "node!=NULL" ), 0) )) ;
        ((void) (( fl!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22831 ,  "fl!=NULL" ), 0) )) ;
        node_id = tree_get_nodeid(node);
        if (!besucht[node_id]) {
                fprintf(fl,"graph: { label:\"");
                tree_print_label(node,fl);
                fprintf(fl,"\"\n");
                fprintf(fl,"title: \"gr_%d\"\n",sid++);
                fprintf(fl,"status: black\n");
                tree_print_subgraph(node,fl);
                fprintf(fl,"node: { label:\"");
                tree_print_label(node,fl);
                fprintf(fl,"\"   ");
                fprintf(fl,"title:\"main_%ld\"}\n",node_id);
                fprintf(fl,"}\n");
                besucht[node_id]=1;
                n = tree_get_number_of_sons(node);
                for (i=0;i<n;i++) {
                        tree_print_nodes(tree_get_son(node,i),fl);
                }
        }
}
static void tree_print_edges(PTREE node,struct _iobuf  *fl)
{
        int i,n;
        long id;
        PTREE n1;
        
        ((void) (( node!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   22864 ,  "node!=NULL" ), 0) )) ;
        ((void) (( fl!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22865 ,  "fl!=NULL" ), 0) )) ;
        id = tree_get_nodeid(node);
        n = tree_get_number_of_sons(node);
        for (i=0;i<n;i++)
        {
                n1 = tree_get_son(node,i);
                ((void) (( n1!=node ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   22871 ,  "n1!=node" ), 0) )) ;
                fprintf(fl,"edge: { sourcename:\"main_%ld\" ", id);
                fprintf(fl,"targetname:\"main_%ld\" \n",tree_get_nodeid(n1));
                fprintf(fl," label: \"%d\" \n",i );
                fprintf(fl,"}\n");
                tree_print_edges(n1,fl);
        }
}
static void tree_pprint(PTREE x,struct _iobuf * fp,int n)
{
        PTREE t;
        int i;
        ((void) (( x!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   22889 ,  "x!=NULL" ), 0) )) ;
        ((void) (( fp!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",        "1.c" ,   22890 ,  "fp!=NULL" ), 0) )) ;
        ((void) (( n>=0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   22891 ,  "n>=0" ), 0) )) ;
        for(i=0;i<=n;i++) 
                (--( fp )->_cnt >= 0 ?  (int)(*( fp )->_ptr++ = (unsigned char)( '-' )) :       ((( fp )->_flag & 0200 ) && -( fp )->_cnt < ( fp )->_bufsiz ?   ((*( fp )->_ptr = (unsigned char)( '-' )) != '\n' ?     (int)(*( fp )->_ptr++) :        _flsbuf(*(unsigned char *)( fp )->_ptr,  fp )) :        _flsbuf((unsigned char)( '-' ),  fp ))) ;
        if (x->sons > 0) {
                fprintf(fp,"* (%d)",x->sons);
                tree_print_label(x,fp);
                fprintf(fp,"\n");
                for(i=0;i<x->sons;i++) { 
                        t = tree_get_son(x,i);
                        ((void) (( t!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   22900 ,  "t!=NULL" ), 0) )) ;
                        tree_pprint(t,fp,n+2);
                }
        }
        else {
                fprintf(fp,"> ");
                tree_print_label(x,fp);
                fprintf(fp,"\n");
        }
}
void tree_copy_pos(PTREE no1,PTREE no2)
{
        ((void) (( no1!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",       "1.c" ,   22918 ,  "no1!=NULL" ), 0) )) ;
        ((void) (( no2!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",       "1.c" ,   22919 ,  "no2!=NULL" ), 0) )) ;
        no2->position = no1->position;
        no2->position2 = no1->position2;
}
void tree_set_nodeid(PTREE node,int id)
{
        node->id = id;
}
static PTREE tree_copy_node(PTREE node)
{
        PTREE help;
        int anz,i;
        anz = node->sons;
        anz = anz>1?anz:1;
        help = tree_getmem(anz);
        help->content = node->content;
        help->sons = node->sons;
        help->position = node->position;
        help->position2 = node->position2;
        help->intern = node->intern;
        help->id = node->id;
        switch(help->intern) {
                case 1:
                        for (i=0;i<anz;i++) {
                                help->indirect.son[i] = node->indirect.son[i];
                        }
                        break;
                case 2:
                        help->indirect.ints = node->indirect.ints;
                        break;
                case 3:
                        help->indirect.doubles = node->indirect.doubles;
                        break;
                default: 
                        ein_neur_name= fatal_error__("1.c",22961)+fatal_error_ ("Help!!!");
                        break;
        }
        return(help);
}
PTREE tree_copy(PTREE root)
{
        PTREE help;
        int i,n;
        n = tree_get_number_of_sons( root ) ;
        help = tree_copy_node(root);
        for (i=0;i<n;i++) {
                tree_set_son(help,tree_copy(tree_get_son(root,i)),i);
        }
        return(help);
}
void tree_set_int(PTREE root, int i)
{
        ((void) (( tree_get_intern(root)==2 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   22988 ,  "tree_get_intern(root)==2" ), 0) )) ;
        root->indirect.ints = i;
}
void tree_destroy_whole(PTREE x)
{
        ein_neur_name= fatal_error__("1.c",22996)+fatal_error_ ("No destroy whole on tree");
}
PTREE tree_dublicate(PTREE x)
{
        return(tree_copy(x));
}
int  tree_isbottom(PTREE x)
{
        ein_neur_name= fatal_error__("1.c",23010)+fatal_error_ ("No on tree");
        return(0 );
}
int  tree_istop(PTREE x)
{
        ein_neur_name= fatal_error__("1.c",23017)+fatal_error_ ("No on tree");
        return(0 );
}
PTREE tree_glb(PTREE x,PTREE y)
{
        ein_neur_name= fatal_error__("1.c",23024)+fatal_error_ ("No on tree");
        return(((void *)0) );
}
PTREE tree_lub(PTREE x,PTREE y)
{
        ein_neur_name= fatal_error__("1.c",23031)+fatal_error_ ("No on tree");
        return(((void *)0) );
}
int  tree_le(PTREE x,PTREE y)
{
        ein_neur_name= fatal_error__("1.c",23038)+fatal_error_ ("No on tree");
        return(0 );
}
int  tree_leq(PTREE x,PTREE y)
{
        ein_neur_name= fatal_error__("1.c",23045)+fatal_error_ ("No on tree");
        return(0 );
}
int  tree_eq(PTREE x,PTREE y)
{
        ein_neur_name= fatal_error__("1.c",23052)+fatal_error_ ("No eq on tree");
        return(0 );
}
        
static unum unum_create_ ( void );
f80_t x64tof80(x64_t *);
int f80tox64(x64_t*,f80_t f80);
long unum_power= -1 ;
static unum u_eins,u_null;
char *unum_len(void)
{
   ein_neur_name= fatal_error__("1.c",23127)+fatal_error_ ("len in unum");
}
unsigned int unum_hash(unum x) {
        unsigned int i;
        x64toui(x,&i);
        return(i);
}
void unum_init(void)
{
   static int  flag = 0 ;
        if (!flag) {
        flag = 1 ;
                u_eins = unum_create("1");
                u_null = unum_create("0");
                ((void) (( unum_power== -1  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",         "1.c" ,   23149 ,  "unum_power==UNENDLICH" ), 0) )) ;
        }
}
x64_t unum_to_x64(unum x)
{
        return(*x);
}
unum unum_to_unum(unum n)
{
        return(n);
}
real unum_to_real(unum n)
{
        char s[1024];
        x64toa(n,s);
        return(real_create(s));
}
snum unum_to_snum(unum u)
{
        return(u);
}
unum unum_add(unum n1, unum n2)
{
        unum n;
        n = unum_dublicate(n1);
        x64add(n,n2);
         
        return(n);
}
unum unum_dublicate(unum inh)
{
        unum n;
        n = unum_create_();
        memmove(n,inh,sizeof(*inh));
        return(n);
}
static unum unum_create_(void)
{
        unum n;
        { ( n ) = ( unum )malloc( sizeof(*( n )) ) ; if (( n )== ((void *)0) ) ein_neur_name= fatal_error__("1.c",23238)+fatal_error_ ("Out of memory (malloc)"); }; ;
        return(n);
}
unum unum_dublicate_whole(unum inh)
{
                return(unum_dublicate(inh));
}
unum unum_abs(unum i)
{
        unum n;
        n = unum_dublicate(i);
        x64abs(n);
         
        return(n);
}
int unum_eq(unum e1, unum e2)
{
        return(x64ucmp(e1,e2)==0);
}
int unum_neq(unum e1, unum e2)
{
        return(x64ucmp(e1,e2)!=0);
}
unum unum_create(char *s)
{
        unum h;
        char *x;
        h= unum_create_();
        x=atox64(h,s);
        if (*x>'0' && *x<'9')
                ein_neur_name= fatal_error__("1.c",23284)+fatal_error_ ("Overflow");
        return(h);
}
void unum_free(unum h)
{
        free((void *) h ) ;
}
void unum_destroy_whole(unum h)
{
        free((void *) h ) ;
}
char *unum_print(unum l)
{
        char *s;
        s = (char*)malloc( sizeof(char)*20 ) ;
        x64toa(l,s);
        return(s);
}
char *unum_vcgprint(unum l)
{
        char *s;
        s = (char*)malloc( sizeof(char)*20 ) ;
        x64toa(l,s);
        return(s);
}
void unum_print_stdout(unum l)
{
        char *s;
        s = unum_print(l);
        fprintf((&_iob[1]) ,"%s",s);
        free((void *) s ) ;
}
int  unum_le(unum a,unum b)
{
        return(x64ucmp(a,b)<0);
}
int  unum_leq(unum a,unum b)
{
        return(x64ucmp(a,b)<=0);
}
int  unum_ge(unum a,unum b)
{
        return(x64ucmp(a,b)>0);
}
int  unum_geq(unum a,unum b)
{
        return(x64ucmp(a,b)>=0);
}
unum unum_modulo(unum a,unum b)
{
        unum n;
        n = unum_dublicate(a);
        x64mod(n,b);
         
        return(n);
}
unum unum_mal(unum a,unum b)
{
        unum n;
        n = unum_dublicate(a);
        x64umul(n,b);
         
        return(n);
}
unum unum_min(unum a,unum b)
{
        unum n;
        n = unum_dublicate(a);
        x64sub(n,b);
         
        return(n);
}
unum unum_hoch(unum a,unum b)
{
        unum h;
        f80tox64(h,fpow80(x64tof80(a),x64tof80(b)));
         
        return(h);
}
unum unum_geteilt(unum a,unum b)
{
        unum n;
        n = unum_dublicate(a);
        x64udiv(n,b);
         
        return(n);
}
unum unum_unplus(unum a)
{
        return(a);
}
unum unum_unmin(unum a)
{
        ein_neur_name= fatal_error__("1.c",23442)+fatal_error_ ("kein unmin auf unum");
        return(((void *)0) );
}
unum unum_rnd(unum a)
{
        return(a);
}
unum unum_bor(unum a,unum b)
{
   ein_neur_name= fatal_error__("1.c",23455)+fatal_error_ ("no bor implemented");
        return(((void *)0) );
}
unum unum_band(unum a,unum b)
{
   ein_neur_name= fatal_error__("1.c",23461)+fatal_error_ ("no band implemented");
        return(((void *)0) );
}
snum unum_sgn(unum a)
{
   if (unum_ge(a,u_null))
      return(u_eins);
   return(u_null);
}
unsigned int unum_to_unsigned(unum x) {
        unsigned int i;
        x64toui(x,&i);
        return(i);
}
unum int_to_unum(int i) {
        unum x;
        x = unum_create_();
        itox64(x,i);
        return(x);
}
typedef struct unum_list_base_ 
{
        unum head;
        struct unum_list_base_ *tail;
} *unum_list,_unum_list;
extern long unum_list_power;
void unum_list_init(void);
void unum_list_destroy_whole(unum_list);
void unum_list_destroy(unum_list);
unum_list unum_list_new();
unum_list unum_list_extend(unum head, unum_list tail);
unum_list unum_list_conc ( unum_list, unum_list );
int unum_list_length ( unum_list );
unum unum_list_head ( unum_list );
unum_list unum_list_tail ( );
unum unum_list_get_elem ( unum_list, int );
unum_list unum_list_set_elem ( unum_list, int, unum );
unum_list unum_list_insert_bevor ( );
int  unum_list_is_empty ( );
void unum_list_print_stdout ( unum_list );
unum_list unum_list_append ( unum_list, unum );
unum_list unum_list_dublicate_whole ( unum_list );
unum_list unum_list_dublicate ( unum_list );
int  unum_list_is_elem ( unum_list, unum );
unum_list unum_list_remove ( unum_list, unum );
char *unum_list_print ( unum_list );
unsigned int unum_list_hash(unum_list);
char *unum_list_vcgprint ( unum_list );
int  unum_list_eq ( unum_list, unum_list );
unum_list unum_list_rest ( unum_list, unum_list );
unum_list unum_list_vereinigung ( unum_list, unum_list );
unum_list unum_list_schnitt ( unum_list, unum_list );
static unum_list _unum_list_dublicate ( unum_list, int  );
long unum_list_power;
void unum_list_init(void)
{
   static int  flag = 0 ;
        if (!flag) {
        flag = 1 ;
        unum_list_power = -1 ;
                unum_init();
        }
}
unum_list unum_list_new()
{
        return(((void *)0) );
}
unum_list unum_list_extend(unum head, unum_list tail)
{
        unum_list help;
        help = (unum_list) malloc( sizeof(*help) ) ;
        if (help == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",23632)+fatal_error_ ("No Memory in  function unum_list_extend\n");   
        help->head = head;
        help->tail = tail;
        return(help);
}
void unum_list_destroy(list)
unum_list list;
{
        unum_list help;
        while(list!= ((void *)0) )
        {
                help = list;
                list = list->tail;
                free((void *) help ) ;
        }
}
void unum_list_destroy_whole(list)
unum_list list;
{
        unum_list help;
        while(list!= ((void *)0) )
        {
                help = list;
                unum_destroy_whole(help->head);
                list = list->tail;
      free((void *) help ) ;
        }
}
unum_list unum_list_conc(unum_list list1, unum_list list2)
{
        unum_list help,merk;
        if (list1== ((void *)0) )
                return(list2);
        if (list2 == ((void *)0) )
                return(list1);
        merk = help = unum_list_dublicate(list1);
        while(help->tail!= ((void *)0) )
                help=help->tail;
        help->tail=list2;
        return(merk);
}
        
int unum_list_length(unum_list list)
{
        if (list == ((void *)0) )
                return(0);
        return(unum_list_length(list->tail)+1);
}
unum unum_list_head(unum_list list)
{
        ((void) (( list!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   23708 ,  "list!= NULL" ), 0) )) ;
        return(list->head);
}
unum_list unum_list_tail(list)
unum_list list;
{
        ((void) (( list!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   23718 ,  "list!= NULL" ), 0) )) ;
        return(list->tail);
}
unum unum_list_get_elem(unum_list list, int nr)
{
        ((void) (( nr>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   23728 ,  "nr>0" ), 0) )) ;
        ((void) (( list!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   23729 ,  "list!=NULL" ), 0) )) ;
        if (nr == 1)
                return(list->head);
        return(unum_list_get_elem(list->tail,nr-1));
}
unum_list unum_list_set_elem(unum_list list,int nr,unum elem)
{
        int i;
        unum_list help;
        ((void) (( nr>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   23745 ,  "nr>0" ), 0) )) ;
        help = list;
        for(i=1;i<nr;i++)
        {
                ((void) (( help!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   23749 ,  "help!=NULL" ), 0) )) ;
                help = help->tail;
        }
        ((void) (( help!= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",      "1.c" ,   23752 ,  "help!=NULL" ), 0) )) ;
        help->head = elem;
        return(list);
}
unum_list unum_list_insert_bevor(list1,nr,list2)
unum_list list1,list2;
int nr;
{
        int i;
        unum_list help,help2;
        ((void) (( nr>0 ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   23770 ,  "nr>0" ), 0) )) ;
        help = list1;
        if (nr == 1) 
        {
                return(unum_list_conc(list2,list1));
        }
        for(i=2;i<nr;i++)
        {
                ((void) (( help != ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",     "1.c" ,   23778 ,  "help != NULL" ), 0) )) ;
                help = help ->tail;
        }
        ((void) (( help= ((void *)0)  ) ? 0 : (__eprintf ("%s:%u: failed assertion `%s'\n",       "1.c" ,   23781 ,  "help=NULL" ), 0) )) ;
        help2 = help->tail;
        help->tail = list2;
        (void) unum_list_conc(help,help2);
        return(list1);
}
int  unum_list_is_empty(list)
unum_list list;
{
        return(list == ((void *)0) );
}
void unum_list_print_stdout(unum_list list)
{
        char *s;
        s = unum_list_print(list);
        fprintf((&_iob[1]) ,"%s",s);
        free((void *) s ) ;
}
unum_list unum_list_append(unum_list list, unum elem)
{
        unum_list help1,help2;
        help1 = help2 = list;
        while (help2!= ((void *)0) ) {
                help1 = help2;
                help2 = help2->tail;
        }
        help2 = unum_list_extend(elem,((void *)0) );
        if (help1!= ((void *)0) ) {
                help1->tail = help2;
                return(list);
        }
        return(help2);
}
unum_list unum_list_dublicate_whole(unum_list list)
{
        return(_unum_list_dublicate(list,1 ));
}
unum_list unum_list_dublicate(unum_list list)
{
        return(_unum_list_dublicate(list,0 ));
}
static unum_list _unum_list_dublicate(unum_list list, int  whole_mode)
{
        unum_list help1,head,help;
        head = ((void *)0) ;
        help1 = ((void *)0) ;
        if (list== ((void *)0) ) return(((void *)0) );
        while(list != ((void *)0) ) {
   help = (unum_list) malloc( sizeof(*help) ) ;
                if (help == ((void *)0) ) {
                        ein_neur_name= fatal_error__("1.c",23872)+fatal_error_ ("No Memory in  function unum_list_extend\n");   
                }
                help->head = whole_mode?unum_dublicate_whole(list->head):list->head;
                if (help1!= ((void *)0) )
                        help1->tail = help;
                else
                        head = help;
                help1 = help;
                list = list->tail;
        }
        help->tail= ((void *)0) ;
        return(head);
}
                
                
int  unum_list_is_elem(unum_list list,unum elem)
{
        while(list!= ((void *)0) ) {
                if (unum_eq(list->head,elem))
                        return(1 );
                list = list->tail;
        }
        return(0 );
}
unum_list unum_list_remove(unum_list list, unum elem)
{
        unum_list help1,help2;
        help1 =help2 = list;
        while(help2!= ((void *)0) ) {
                if (unum_eq(help2->head,elem))
                        break;
                help1 = help2;
                help2 = help2->tail;
        }
        if (help2== ((void *)0) )
                return(list);
        if (help2==list)
                return(help2->tail);
        help1->tail = help2->tail;
        return(list);
}
char *unum_list_print(unum_list list)
{
        int x,len = 1024;
        char *s,*e,*w;
        e = (char*)  malloc( sizeof(char)*len ) ;
        if (e== ((void *)0) )
                ein_neur_name= fatal_error__("1.c",23938)+fatal_error_ ("No memeory");
        e[0]='\0';
        strcpy(e,"[");
        x=strlen(e);
        while (!unum_list_is_empty(list)) {
                s = unum_print(unum_list_head(list));
                x += strlen(s);
                if (x+5 >= len) {
                        len *= 2;
                        w = (char*)malloc( sizeof(char)*len ) ;
                        if (w== ((void *)0) )
                                ein_neur_name= fatal_error__("1.c",23953)+fatal_error_ ("No memeory");
                        w[0] = '\0';
                        strcpy(w,e);
                        free((void *) e ) ;
                        e = w;
                }
                strcat(e,s);
                strcat(e,",");
                x = strlen(e);
                list = unum_list_tail(list);
        }
        strcat(e,"]");
        return(e);
}
int  unum_list_eq(unum_list l1, unum_list l2)
{
        while(!unum_list_is_empty(l1)) {
                if  (unum_list_is_empty(l2)) 
                        return(0 );
                if (!unum_eq(unum_list_head(l1),unum_list_head(l2)))
                        return(0 );
                l1 = unum_list_tail(l1);
                l2 = unum_list_tail(l2);
        }
        if  (!unum_list_is_empty(l2)) 
                return(0 );
        return(1 );
}
                
unum_list unum_list_rest(unum_list l1, unum_list l2)
{
   unum_list l3;
   unum elem;
   l3 = unum_list_new();
   while(!unum_list_is_empty(l1)) {
      elem = unum_list_head(l1);
      if (!unum_list_is_elem(l2,elem))
         l3 = unum_list_extend(elem,l3);
      l1 = unum_list_tail(l1);
   }
   return(l3);
}
unum_list unum_list_vereinigung(unum_list l1, unum_list l2)
{
   unum_list l3;
   unum elem;
   l3 = unum_list_dublicate(l2);
   while(!unum_list_is_empty(l1)) {
      elem = unum_list_head(l1);
      if (!unum_list_is_elem(l2,elem))
         l3 = unum_list_extend(elem,l3);
      l1 = unum_list_tail(l1);
   }
   return(l3);
}
unum_list unum_list_schnitt(unum_list l1, unum_list l2)
{
   unum_list l3;
   unum elem;
   l3 = unum_list_new();
   while(!unum_list_is_empty(l1)) {
      elem = unum_list_head(l1);
      if (unum_list_is_elem(l2,elem))
         l3 = unum_list_extend(elem,l3);
      l1 = unum_list_tail(l1);
   }
   return(l3);
}
static char    *normal = "green";
static char    *high = "red";
static char    *prog = "blue";
static char    *gelb = "purple";
static char    *green = "green";
static struct _iobuf     *fp = ((void *)0) ;
static KFG      kfg;
static KFG_NODE_LIST entries, help;
static KFG_NODE node;
void
dataflow(char *s1, char *s2)
{
}
void
create_vcg(char *s1, char *s, int mode, DFI_STORE dfi, int cur, int pos, WORKLIST w)
{
        char            sd[128];
        char            sd2[128];
        int             j;
        extern int     *DFSNUM;
        if (mode == 0) {
                sprintf(sd, "/bin/rm -f %s/*.vcg", s1);
                system(sd);
        }
        kfg = kfg_get();
        sprintf(sd, "%s/%s", s1, s);
        sprintf(sd2, "%s/%s_icon", s1, s);
        fprintf((&_iob[2]) , "printing %s with mode %d\n", s, mode);
        fp = fopen(sd, "w");
        if (fp == ((void *)0) ) {
                fprintf((&_iob[2]) , "cannot open file `%s' \n", sd);
                exit(1);
        }
        /*if (mkdir(sd2, 0744)) {
                fprintf((&_iob[2]) , "cannot create dir `%s' \n", sd2);
                exit(1);
        }*/
        fprintf(fp, "graph: { \n");
        fprintf(fp, "amax:0\n");
        fprintf(fp, "scaling:maxspect\n");
        fprintf(fp, "ignore_singles:yes\n");
        fprintf(fp, "fasticons:yes\n");
        fprintf(fp, "iconcolors:4\n");
        fprintf(fp, "layout_algorithm:maxdepth\n");
        kfg = kfg_get();
        help = entries = kfg_all_nodes(kfg);
        while (!node_list_is_empty(help)) {
                int             id,now;
                int             ok;
                node = node_list_head(help);
                fprintf(fp, "node: { title:\"%d\" \n", kfg_get_id(kfg, node ) );
                fprintf(fp, "        label:\"%d(%d)\"\n", kfg_get_id(kfg, node ) , DFSNUM[kfg_get_id(kfg, node ) ]);
                ok = 0;
                if (cur == -1) {
                        fprintf(fp, "        color:%s\n", normal);
                        ok = 1;
                }
                if (!ok && (kfg_get_id(kfg, node )  == cur)) {
                        fprintf(fp, "        color:%s\n", high);
                        ok = 1;
                }
                if (!ok && (worklist_in(w, kfg_get_id(kfg, node ) ))) {
                        fprintf(fp, "        color:%s\n", prog);
                        ok = 1;
                }
                if (!ok)
                        fprintf(fp, "        color:%s\n", normal);
                fprintf(fp, "        shape:ellipse\n");
                fprintf(fp, "      }\n");
                now=0;
                for (j = 0; j < kfg_arrity(kfg, node); j++) {
                        int             k, b, bit = kfg_which_edges(node);
                        for (k = 0, b = 1; k < kfg_max_edge(); b = b << 1, k++) {
                                if (bit & b) {
                                        now+=1;
                                        fprintf(fp, "node: { title:\"%d_%d\" \n", kfg_get_id(kfg, node ) , now);
                                        if (mode == 0) {
                                                fprintf(fp, "        label:\"bot\"\n");
                                        } else {
                                                fprintf(fp, "        label:\"\"\n");
                                                fprintf(fp, "        borderwidth:5\n");
                                                 
                                                fprintf(fp, "        iconfile:\"%s\"\n", dfi_vcgprintout(dfi, kfg_get_id(kfg, node ) , j, k, sd2, s));
                                        }
                                        ok = 0;
                                        if (cur == -1) {
                                                fprintf(fp, "        bordercolor:%s\n", normal);
                                                ok = 1;
                                        }
                                        if (!ok && (kfg_get_id(kfg, node )  == cur) && (pos == j)) {
                                                fprintf(fp, "        bordercolor:%s\n", high);
                                                ok = 1;
                                        }
                                        if (!ok && (worklist_ine(w, kfg_get_id(kfg, node ) , j))) {
                                                fprintf(fp, "        bordercolor:%s\n", prog);
                                                ok = 1;
                                        }
                                        if (!ok) {
                                                fprintf(fp, "        bordercolor:%s\n", normal);
                                        }
                                        fprintf(fp, "      }\n");
                                        if (now == 1)
                                                fprintf(fp, "nearedge: { sourcename:\"%d\" \n", kfg_get_id(kfg, node ) );
                                        else
                                                fprintf(fp, "nearedge: { sourcename:\"%d_%d\" \n", kfg_get_id(kfg, node ) , now-1);
                                        fprintf(fp, "        targetname:\"%d_%d\"\n", kfg_get_id(kfg, node ) , now);
                                         
                                        fprintf(fp, "        class    :2\n");
                                        fprintf(fp, "        label : \"%s_%d\"\n", o_edges_print(k),j);
                                        fprintf(fp, "      }\n");
                                }
                        }
                }
                help = node_list_tail(help);
        }
        help = entries;
        while (!node_list_is_empty(help)) {
                KFG_NODE_LIST   sucs;
                int             id;
                int             type;
                int             ja = 0;
                type = 0;
                node = node_list_head(help);
                sucs = kfg_successors(kfg, node);
                if ((kfg_node_type(kfg, node) == RETURN) ||
                    (kfg_node_type(kfg, node) == CALL))
                        type = 1;
                while (!node_list_is_empty(sucs)) {
                        KFG_NODE        l1 = node_list_head(sucs);
                        char            sd1[128];
                        char            sd2[128];
                        fprintf(fp, "edge: { sourcename:\"%d\" \n", kfg_get_id(kfg, node ) );
                        fprintf(fp, "        targetname:\"%d\"\n", kfg_get_id(kfg, l1 ) );
                        fprintf(fp, "        class     : 1\n", kfg_get_id(kfg, l1 ) );
                        if (type)
                                fprintf(fp, "        linestyle:dotted\n");
                        fprintf(fp, "      }\n");
                        ja = 0;
                        if ((kfg_node_type(kfg, node) == CALL) &&
                            (kfg_node_type(kfg, l1) == START)) {
                                ja = 1;
                                for (j = 0; j < kfg_arrity(kfg, node); j++) {
                                        sprintf(sd1, "%d_%d", kfg_get_id(kfg, node ) , j);
                                        sprintf(sd2, "%d_%d", kfg_get_id(kfg, l1 ) , get_mapping(node, l1, j));
                                        dataflow(sd1, sd2);
                                }
                        }
                        if ((kfg_node_type(kfg, l1) == RETURN) &&
                            (kfg_node_type(kfg, node) == END)) {
                                KFG_NODE        cc = kfg_get_call(kfg, l1);
                                KFG_NODE        bb = kfg_get_start(kfg, node);
                                ja = 1;
                                for (j = 0; j < kfg_arrity(kfg, l1); j++) {
                                        sprintf(sd1, "%d_%d", kfg_get_id(kfg, node ) , get_mapping(cc, bb, j));
                                        sprintf(sd2, "%d_%d", kfg_get_id(kfg, l1 ) , j);
                                        dataflow(sd1, sd2);
                                }
                        }
                        if (ja == 0) {
                                for (j = 0; j < kfg_arrity(kfg, node); j++) {
                                        sprintf(sd1, "%d_%d", kfg_get_id(kfg, node ) , j);
                                        sprintf(sd2, "%d_%d", kfg_get_id(kfg, l1 ) , j);
                                        dataflow(sd1, sd2);
                                }
                        }
                        sucs = node_list_tail(sucs);
                }
                help = node_list_tail(help);
        }
        fprintf(fp, "}\n");
        fclose(fp);
}
static W_LISTELEM *alloc_w_listelem ( void );
static void destroy_w_listelem ( W_LISTELEM * );
static W_LISTELEM *search_worklist ( WORKLIST, int, int );
long w_length = 0;
double w_dlength = 0.0;
WORKLIST worklist_create(void)
{
        w_length = 0;
        w_dlength = 0.0;
        return(((void *)0) );
}
static W_LISTELEM *alloc_w_listelem(void)
{
        W_LISTELEM *help;
        help = (W_LISTELEM*)malloc( sizeof(W_LISTELEM) ) ;
        if (help == ((void *)0) )
                ein_neur_name= fatal_error__("1.c",24475)+fatal_error_ ("Kein Speicherplatz in alloc_w_listelem");
        return(help);
}
static void destroy_w_listelem(W_LISTELEM *x)
{
        free((void *) x ) ;
}
extern int *DFSNUM;
extern int cfg_ordering;
WORKLIST        worklist_insert(WORKLIST w, int node ,int pos)
{
        W_LISTELEM *help,*help2,*help1; 
        help = alloc_w_listelem();
        (( help )->node)  = node;
        (( help )->pos)  = pos;
        w_length += 1;
      if (cfg_ordering) {
        if (w == ((void *)0) ) {
        (( help )->next)  = w;
        return(help);
        }
        if (DFSNUM[(( w )->node) ] >= DFSNUM[node])
           {  (( help )->next)  = w; return(help);}
        help2 = (( w )->next) ;
        help1 = w;
        while ((help2 != ((void *)0) ) && (DFSNUM[(( help2 )->node) ] < DFSNUM[node]))  {
               help1 = help2;
               help2 = (( help2 )->next) ;
               if (help2 == ((void *)0) ) break;
        } 
        (( help1 )->next)  = help;
        (( help )->next)   = help2; 
        return w;
        } else  {
        (( help )->next)  = w;
        return help;
        }
        return(help);
}
static W_LISTELEM *search_worklist(WORKLIST w, int node,int pos)
{
        while(w!= ((void *)0) ) {
                if ((( w )->node) ==node && (( w )->pos) ==pos)
                        return(w);
                if (cfg_ordering && (DFSNUM[(( w )->node) ] > DFSNUM[node])) 
                    return ((void *)0) ;
                w = (( w )->next) ;
        }
        return(((void *)0) );
}
int worklist_in(WORKLIST w, int id ) {
while(w!= ((void *)0) ) {
                if ((( w )->node) ==id)  return(1);
                w = (( w )->next) ;
        }
        return(0);
}
int worklist_ine(WORKLIST w, int id,int pos ) {
while(w!= ((void *)0) ) {
                if (((( w )->node) ==id)&&((( w )->pos) ==pos))  return(1);
                w = (( w )->next) ;
        }
        return(0);
}
WORKLIST worklist_insert_if_not_there(WORKLIST w, int node ,int pos) 
{
        W_LISTELEM *help;
        help = search_worklist(w,node,pos);
        if (help == ((void *)0) )
                return(worklist_insert(w,node,pos));
        return(w);
}
        
WORKLIST worklist_pop(WORKLIST w,int *node, int *pos)
{
        W_LISTELEM *help;
        *node = (( w )->node) ;
        *pos = (( w )->pos) ;
        help = (( w )->next) ;
        destroy_w_listelem(w); 
        w_length -= 1;
        return(help);
}
        
int     worklist_is_empty(WORKLIST w)
{
        return(w == ((void *)0) );
}
void worklist_print_stdout(WORKLIST w)
{
        printf("Worklist: ");
        while(w!= ((void *)0) ) {
                printf("(%d,%d), ",(( w )->node) ,(( w )->pos) );
                w = (( w )->next) ;
        }
        printf("\n");
}


main() {

gc_ipcp2_doit((KFG) 0L);
}
