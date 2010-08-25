typedef void *voidp;
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
extern int system(const char*);
/*extern int mkdir(const* char, int);*/
extern int _flsbuf(unsigned char, struct _iobuf  *);
extern int      printf  (char *, ...)  ;
extern int memcmp(void*, void*, int);
extern int      fflush  (struct _iobuf  *)  ;
extern void perror(const char*);
extern void* memcpy(void*,void*, int);
extern void* memset(void*,void*,int);


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
typedef int             sigset_t;        
typedef unsigned int    speed_t;         
typedef unsigned long   tcflag_t;        
typedef unsigned char   cc_t;            
typedef int             pid_t;           
typedef unsigned short  mode_t;          
typedef short           nlink_t;         
typedef long            clock_t;         
typedef long            time_t;          
typedef /*long*/ unsigned int size_t;            
typedef int ptrdiff_t;   
typedef short unsigned int wchar_t;      
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
typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;
extern int method;          
extern  uch    inbuf [] ;           
extern  uch    outbuf [] ;          
extern  ush    d_buf [] ;           
extern  uch    window [] ;          
   extern  ush    prev  [] ;   
extern unsigned insize;  
extern unsigned inptr;   
extern unsigned outcnt;  
extern long bytes_in;    
extern long bytes_out;   
extern long header_bytes; 
extern int  ifd;         
extern int  ofd;         
extern char ifname[];    
extern char ofname[];    
extern char *progname;   
extern long time_stamp;  
extern long ifile_size;  
typedef int file_t;      
extern int decrypt;         
extern int exit_code;       
extern int verbose;         
extern int quiet;           
extern int level;           
extern int test;            
extern int to_stdout;       
extern int save_orig_name;  
         
extern int zip         (int in, int out)  ;
extern int file_read   (char *buf,  unsigned size)  ;
         
extern int unzip       (int in, int out)  ;
extern int check_zipfile  (int in)  ;
         
extern int unpack      (int in, int out)  ;
         
extern int unlzh       (int in, int out)  ;
         
void  abort_gzip  (void)  ;
void lm_init  (int pack_level, ush *flags)  ;
ulg  deflate  (void)  ;
void ct_init      (ush *attr, int *method)  ;
int  ct_tally     (int dist, int lc)  ;
ulg  flush_block  (char *buf, ulg stored_len, int eof)  ;
void     bi_init     (file_t zipfile)  ;
void     send_bits   (int value, int length)  ;
unsigned bi_reverse  (unsigned value, int length)  ;
void     bi_windup   (void)  ;
void     copy_block  (char *buf, unsigned len, int header)  ;
extern   int (*read_buf)  (char *buf, unsigned size)  ;
         
extern int copy            (int in, int out)  ;
extern ulg  updcrc         (uch *s, unsigned n)  ;
extern void clear_bufs     (void)  ;
extern int  fill_inbuf     (int eof_ok)  ;
extern void flush_outbuf   (void)  ;
extern void flush_window   (void)  ;
extern void write_buf      (int fd, voidp buf, unsigned cnt)  ;
extern char *strlwr        (char *s)  ;
extern char *basename      (char *fname)  ;
extern void make_simple_name  (char *name)  ;
extern char *add_envopt    (int *argcp, char ***argvp, char *env)  ;
extern void error          (char *m)  ;
extern void warn           (char *a, char *b)  ;
extern void read_error     (void)  ;
extern void write_error    (void)  ;
extern void display_ratio  (long num, long den, struct _iobuf  *file)  ;
extern voidp xmalloc       (unsigned int size)  ;
         
extern int inflate  (void)  ;
static  file_t zfile;  
static  unsigned short bi_buf;
static  int bi_valid;
int (*read_buf)  (char *buf, unsigned size)  ;
void bi_init (zipfile)
    file_t zipfile;  
{
    zfile  = zipfile;
    bi_buf = 0;
    bi_valid = 0;
    if (zfile != (-1) ) {
        read_buf  = file_read;
    }
}
void send_bits(value, length)
    int value;   
    int length;  
{
    if (bi_valid > (int)(8 * 2*sizeof(char))  - length) {
        bi_buf |= (value << bi_valid);
        { if (outcnt < 16384 -2) { outbuf[outcnt++] = (uch) (( bi_buf ) & 0xff); outbuf[outcnt++] = (uch) ((ush)( bi_buf ) >> 8); } else { {outbuf[outcnt++]=(uch)( (uch)(( bi_buf ) & 0xff) ); if (outcnt==16384 ) flush_outbuf();} ; {outbuf[outcnt++]=(uch)( (uch)((ush)( bi_buf ) >> 8) ); if (outcnt==16384 ) flush_outbuf();} ; } } ;
        bi_buf = (ush)value >> ((8 * 2*sizeof(char))  - bi_valid);
        bi_valid += length - (8 * 2*sizeof(char)) ;
    } else {
        bi_buf |= value << bi_valid;
        bi_valid += length;
    }
}
unsigned bi_reverse(code, len)
    unsigned code;  
    int len;        
{
    register unsigned res = 0;
    do {
        res |= code & 1;
        code >>= 1, res <<= 1;
    } while (--len > 0);
    return res >> 1;
}
void bi_windup()
{
    if (bi_valid > 8) {
        { if (outcnt < 16384 -2) { outbuf[outcnt++] = (uch) (( bi_buf ) & 0xff); outbuf[outcnt++] = (uch) ((ush)( bi_buf ) >> 8); } else { {outbuf[outcnt++]=(uch)( (uch)(( bi_buf ) & 0xff) ); if (outcnt==16384 ) flush_outbuf();} ; {outbuf[outcnt++]=(uch)( (uch)((ush)( bi_buf ) >> 8) ); if (outcnt==16384 ) flush_outbuf();} ; } } ;
    } else if (bi_valid > 0) {
        {outbuf[outcnt++]=(uch)( bi_buf ); if (outcnt==16384 ) flush_outbuf();} ;
    }
    bi_buf = 0;
    bi_valid = 0;
}
void copy_block(buf, len, header)
    char     *buf;     
    unsigned len;      
    int      header;   
{
    bi_windup();               
    if (header) {
        { if (outcnt < 16384 -2) { outbuf[outcnt++] = (uch) (( (ush)len ) & 0xff); outbuf[outcnt++] = (uch) ((ush)( (ush)len ) >> 8); } else { {outbuf[outcnt++]=(uch)( (uch)(( (ush)len ) & 0xff) ); if (outcnt==16384 ) flush_outbuf();} ; {outbuf[outcnt++]=(uch)( (uch)((ush)( (ush)len ) >> 8) ); if (outcnt==16384 ) flush_outbuf();} ; } } ;   
        { if (outcnt < 16384 -2) { outbuf[outcnt++] = (uch) (( (ush)~len ) & 0xff); outbuf[outcnt++] = (uch) ((ush)( (ush)~len ) >> 8); } else { {outbuf[outcnt++]=(uch)( (uch)(( (ush)~len ) & 0xff) ); if (outcnt==16384 ) flush_outbuf();} ; {outbuf[outcnt++]=(uch)( (uch)((ush)( (ush)~len ) >> 8) ); if (outcnt==16384 ) flush_outbuf();} ; } } ;
    }
    while (len--) {
        {outbuf[outcnt++]=(uch)( *buf++ ); if (outcnt==16384 ) flush_outbuf();} ;
    }
}
extern int maxbits;       
extern int block_mode;    
extern int lzw     (int in, int out)  ;
extern int unlzw   (int in, int out)  ;
typedef ush Pos;
typedef unsigned IPos;
ulg window_size = (ulg)2*0x8000 ;
long block_start;
static  unsigned ins_h;   
unsigned int   prev_length;
      unsigned   strstart;       
      unsigned   match_start;    
static  int           eofile;         
static  unsigned      lookahead;      
unsigned   max_chain_length;
static  unsigned int max_lazy_match;
static  int compr_level;
unsigned   good_match;
typedef struct config {
   ush good_length;  
   ush max_lazy;     
   ush nice_length;  
   ush max_chain;
} config;
  int   nice_match;  
static  config configuration_table[10] = {
  {0,    0,  0,    0},   
  {4,    4,  8,    4},   
  {4,    5, 16,    8},
  {4,    6, 32,   32},
  {4,    4, 16,   16},   
  {8,   16, 32,   32},
  {8,   16, 128, 128},
  {8,   32, 128, 256},
  {32, 128, 258, 1024},
  {32, 258, 258, 4096}};  
static  void fill_window    (void)  ;
static  ulg deflate_fast    (void)  ;
      int  longest_match  (IPos cur_match)  ;
void lm_init (pack_level, flags)
    int pack_level;  
    ush *flags;      
{
    register unsigned j;
    if (pack_level < 1 || pack_level > 9) error("bad pack level");
    compr_level = pack_level;
    memset ((voidp)( (char*)(prev+0x8000 )  ), 0, (  (unsigned)(1<<15 ) *sizeof(*(prev+0x8000 ) ) )) ;
    max_lazy_match   = configuration_table[pack_level].max_lazy;
    good_match       = configuration_table[pack_level].good_length;
    nice_match       = configuration_table[pack_level].nice_length;
    max_chain_length = configuration_table[pack_level].max_chain;
    if (pack_level == 1) {
       *flags |= 4 ;
    } else if (pack_level == 9) {
       *flags |= 2 ;
    }
    strstart = 0;
    block_start = 0L;
    lookahead = read_buf((char*)window,
                         sizeof(int) <= 2 ? (unsigned)0x8000  : 2*0x8000 );
    if (lookahead == 0 ) /* || lookahead == (unsigned)(-1) ) */ {
       eofile = 1, lookahead = 0;
       return;
    }
    eofile = 0;
    while (lookahead < (258 +3 +1)  && !eofile) fill_window();
    ins_h = 0;
    for (j=0; j<3 -1; j++) ( ins_h  = ((( ins_h )<<((15 +3 -1)/3 ) ) ^ (  window[j] )) & ((unsigned)(1<<15 ) -1) ) ;
}
int longest_match(cur_match)
    IPos cur_match;                              
{
    unsigned chain_length = max_chain_length;    
    register uch *scan = window + strstart;      
    register uch *match;                         
    register int len;                            
    int best_len = prev_length;                  
    IPos limit = strstart > (IPos)(0x8000 - (258 +3 +1) )  ? strstart - (IPos)(0x8000 - (258 +3 +1) )  : 0 ;
    register uch *strend = window + strstart + 258 ;
    register uch scan_end1  = scan[best_len-1];
    register uch scan_end   = scan[best_len];
    if (prev_length >= good_match) {
        chain_length >>= 2;
    }
     ;
    do {
         ;
        match = window + cur_match;
        if (match[best_len]   != scan_end  ||
            match[best_len-1] != scan_end1 ||
            *match            != *scan     ||
            *++match          != scan[1])      continue;
        scan += 2, match++;
        do {
        } while (*++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 *++scan == *++match && *++scan == *++match &&
                 scan < strend);
        len = 258  - (int)(strend - scan);
        scan = strend - 258 ;
        if (len > best_len) {
            match_start = cur_match;
            best_len = len;
            if (len >= nice_match) break;
            scan_end1  = scan[best_len-1];
            scan_end   = scan[best_len];
        }
    } while ((cur_match = prev[cur_match & (0x8000 -1) ]) > limit
             && --chain_length != 0);
    return best_len;
}
static  void fill_window()
{
    register unsigned n, m;
    unsigned more = (unsigned)(window_size - (ulg)lookahead - (ulg)strstart);
    if (more == (unsigned)(-1) ) {
        more--;
    } else if (strstart >= 0x8000 +(0x8000 - (258 +3 +1) ) ) {
         ;
        memcpy((char*)window, (char*)window+0x8000 , (unsigned)0x8000 );
        match_start -= 0x8000 ;
        strstart    -= 0x8000 ;  
        block_start -= (long) 0x8000 ;
        for (n = 0; n < (unsigned)(1<<15 ) ; n++) {
            m = (prev+0x8000 ) [n];
            (prev+0x8000 ) [n] = (Pos)(m >= 0x8000  ? m- 0x8000  : 0 );
        }
        for (n = 0; n < 0x8000 ; n++) {
            m = prev[n];
            prev[n] = (Pos)(m >= 0x8000  ? m- 0x8000  : 0 );
        }
        more += 0x8000 ;
    }
    if (!eofile) {
        n = read_buf((char*)window+strstart+lookahead, more);
        if (n == 0 ) /* || n == (unsigned)(-1) ) */ {
            eofile = 1;
        } else {
            lookahead += n;
        }
    }
}
static  ulg deflate_fast()
{
    IPos hash_head;  
    int flush;       
    unsigned match_length = 0;   
    prev_length = 3 -1;
    while (lookahead != 0) {
        (( ins_h  = ((( ins_h )<<((15 +3 -1)/3 ) ) ^ (  window[( strstart ) + 3 -1] )) & ((unsigned)(1<<15 ) -1) ) , prev[( strstart ) & (0x8000 -1) ] =   hash_head  = (prev+0x8000 ) [ins_h], (prev+0x8000 ) [ins_h] = ( strstart )) ;
        if (hash_head != 0  && strstart - hash_head <= (0x8000 - (258 +3 +1) ) ) {
            match_length = longest_match (hash_head);
            if (match_length > lookahead) match_length = lookahead;
        }
        if (match_length >= 3 ) {
             ;
            flush = ct_tally(strstart-match_start, match_length - 3 );
            lookahead -= match_length;
             
            if (match_length <= max_lazy_match ) {
                match_length--;  
                do {
                    strstart++;
                    (( ins_h  = ((( ins_h )<<((15 +3 -1)/3 ) ) ^ (  window[( strstart ) + 3 -1] )) & ((unsigned)(1<<15 ) -1) ) , prev[( strstart ) & (0x8000 -1) ] =   hash_head  = (prev+0x8000 ) [ins_h], (prev+0x8000 ) [ins_h] = ( strstart )) ;
                } while (--match_length != 0);
                strstart++; 
            } else {
                strstart += match_length;
                match_length = 0;
                ins_h = window[strstart];
                ( ins_h  = ((( ins_h )<<((15 +3 -1)/3 ) ) ^ (  window[strstart+1] )) & ((unsigned)(1<<15 ) -1) ) ;
            }
        } else {
             ;
            flush = ct_tally (0, window[strstart]);
            lookahead--;
            strstart++; 
        }
        if (flush) flush_block(block_start >= 0L ? (char*)&window[(unsigned)block_start] : (char*)0 , (long)strstart - block_start, ( 0 )) , block_start = strstart;
        while (lookahead < (258 +3 +1)  && !eofile) fill_window();
    }
    return flush_block(block_start >= 0L ? (char*)&window[(unsigned)block_start] : (char*)0 , (long)strstart - block_start, ( 1 )) ;  
}
ulg deflate()
{
    IPos hash_head;           
    IPos prev_match;          
    int flush;                
    int match_available = 0;  
    register unsigned match_length = 3 -1;  
    if (compr_level <= 3) return deflate_fast();  
    while (lookahead != 0) {
        (( ins_h  = ((( ins_h )<<((15 +3 -1)/3 ) ) ^ (  window[( strstart ) + 3 -1] )) & ((unsigned)(1<<15 ) -1) ) , prev[( strstart ) & (0x8000 -1) ] =   hash_head  = (prev+0x8000 ) [ins_h], (prev+0x8000 ) [ins_h] = ( strstart )) ;
        prev_length = match_length, prev_match = match_start;
        match_length = 3 -1;
        if (hash_head != 0  && prev_length < max_lazy_match &&
            strstart - hash_head <= (0x8000 - (258 +3 +1) ) ) {
            match_length = longest_match (hash_head);
            if (match_length > lookahead) match_length = lookahead;
            if (match_length == 3  && strstart-match_start > 4096 ){
                match_length--;
            }
        }
        if (prev_length >= 3  && match_length <= prev_length) {
             ;
            flush = ct_tally(strstart-1-prev_match, prev_length - 3 );
            lookahead -= prev_length-1;
            prev_length -= 2;
            do {
                strstart++;
                (( ins_h  = ((( ins_h )<<((15 +3 -1)/3 ) ) ^ (  window[( strstart ) + 3 -1] )) & ((unsigned)(1<<15 ) -1) ) , prev[( strstart ) & (0x8000 -1) ] =   hash_head  = (prev+0x8000 ) [ins_h], (prev+0x8000 ) [ins_h] = ( strstart )) ;
            } while (--prev_length != 0);
            match_available = 0;
            match_length = 3 -1;
            strstart++;
            if (flush) flush_block(block_start >= 0L ? (char*)&window[(unsigned)block_start] : (char*)0 , (long)strstart - block_start, ( 0 )) , block_start = strstart;
        } else if (match_available) {
             ;
            if (ct_tally (0, window[strstart-1])) {
                flush_block(block_start >= 0L ? (char*)&window[(unsigned)block_start] : (char*)0 , (long)strstart - block_start, ( 0 )) , block_start = strstart;
            }
            strstart++;
            lookahead--;
        } else {
            match_available = 1;
            strstart++;
            lookahead--;
        }
         ;
        while (lookahead < (258 +3 +1)  && !eofile) fill_window();
    }
    if (match_available) ct_tally (0, window[strstart-1]);
    return flush_block(block_start >= 0L ? (char*)&window[(unsigned)block_start] : (char*)0 , (long)strstart - block_start, ( 1 )) ;  
}
extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;
struct option
{
  const char *name;
  int has_arg;
  int *flag;
  int val;
};
extern int getopt_long (int argc, char *const *argv, const char *shortopts,
                        const struct option *longopts, int *longind);
extern int getopt_long_only (int argc, char *const *argv,
                             const char *shortopts,
                             const struct option *longopts, int *longind);
extern int _getopt_internal (int argc, char *const *argv,
                             const char *shortopts,
                             const struct option *longopts, int *longind,
                             int long_only);
char *optarg = 0;
int optind = 0;
static char *nextchar;
int opterr = 1;
int optopt = '\0' ;
static enum
{
  REQUIRE_ORDER, PERMUTE, RETURN_IN_ORDER
} ordering;
extern char *getenv(const char *name);
extern int  strcmp (const char *s1, const char *s2);
static int my_strlen(const char *s);
static char *my_index (const char *str, int chr);
static int
my_strlen (str)
     const char *str;
{
  int n = 0;
  while (*str++)
    n++;
  return n;
}
static char *
my_index (str, chr)
     const char *str;
     int chr;
{
  while (*str)
    {
      if (*str == chr)
        return (char *) str;
      str++;
    }
  return 0;
}
static int first_nonopt;
static int last_nonopt;
static void exchange (char **argv);
static void
exchange (argv)
     char **argv;
{
  char *temp, **first, **last;
  first = &argv[first_nonopt];
  last  = &argv[optind-1];
  while (first < last) {
    temp = *first; *first = *last; *last = temp; first++; last--;
  }
  first = &argv[first_nonopt];
  first_nonopt += (optind - last_nonopt);
  last  = &argv[first_nonopt - 1];
  while (first < last) {
    temp = *first; *first = *last; *last = temp; first++; last--;
  }
  first = &argv[first_nonopt];
  last_nonopt = optind;
  last  = &argv[last_nonopt-1];
  while (first < last) {
    temp = *first; *first = *last; *last = temp; first++; last--;
  }
}
int
_getopt_internal (argc, argv, optstring, longopts, longind, long_only)
     int argc;
     char *const *argv;
     const char *optstring;
     const struct option *longopts;
     int *longind;
     int long_only;
{
  int option_index;
  optarg = 0;
  if (optind == 0)
    {
      first_nonopt = last_nonopt = optind = 1;
      nextchar = 0 ;
      if (optstring[0] == '-')
        {
          ordering = RETURN_IN_ORDER;
          ++optstring;
        }
      else if (optstring[0] == '+')
        {
          ordering = REQUIRE_ORDER;
          ++optstring;
        }
      else if (getenv ("POSIXLY_CORRECT") != 0 )
        ordering = REQUIRE_ORDER;
      else
        ordering = PERMUTE;
    }
  if (nextchar == 0  || *nextchar == '\0')
    {
      if (ordering == PERMUTE)
        {
           
          if (first_nonopt != last_nonopt && last_nonopt != optind)
            exchange ((char **) argv);
          else if (last_nonopt != optind)
            first_nonopt = optind;
           
          while (optind < argc
                 && (argv[optind][0] != '-' || argv[optind][1] == '\0')
                 )
            optind++;
          last_nonopt = optind;
        }
      if (optind != argc && !strcmp (argv[optind], "--"))
        {
          optind++;
          if (first_nonopt != last_nonopt && last_nonopt != optind)
            exchange ((char **) argv);
          else if (first_nonopt == last_nonopt)
            first_nonopt = optind;
          last_nonopt = argc;
          optind = argc;
        }
      if (optind == argc)
        {
           
          if (first_nonopt != last_nonopt)
            optind = first_nonopt;
          return (-1) ;
        }
      if ((argv[optind][0] != '-' || argv[optind][1] == '\0')
          )
        {
          if (ordering == REQUIRE_ORDER)
            return (-1) ;
          optarg = argv[optind++];
          return 1;
        }
      nextchar = (argv[optind] + 1
                  + (longopts != 0  && argv[optind][1] == '-'));
    }
  if (longopts != 0 
      && ((argv[optind][0] == '-'
           && (argv[optind][1] == '-' || long_only))
          ))
    {
      const struct option *p;
      char *s = nextchar;
      int exact = 0;
      int ambig = 0;
      const struct option *pfound = 0 ;
      int indfound = 0;
      while (*s && *s != '=')
        s++;
      for (p = longopts, option_index = 0; p->name;
           p++, option_index++)
        if (!strncmp (p->name, nextchar, s - nextchar))
          {
            if (s - nextchar == my_strlen (p->name))
              {
                 
                pfound = p;
                indfound = option_index;
                exact = 1;
                break;
              }
            else if (pfound == 0 )
              {
                 
                pfound = p;
                indfound = option_index;
              }
            else
               
              ambig = 1;
          }
      if (ambig && !exact)
        {
          if (opterr)
            fprintf ((&_iob[2]) , "%s: option `%s' is ambiguous\n",
                     argv[0], argv[optind]);
          nextchar += my_strlen (nextchar);
          optind++;
          return '\0' ;
        }
      if (pfound != 0 )
        {
          option_index = indfound;
          optind++;
          if (*s)
            {
               
              if (pfound->has_arg)
                optarg = s + 1;
              else
                {
                  if (opterr)
                    {
                      if (argv[optind - 1][1] == '-')
                         
                        fprintf ((&_iob[2]) ,
                                 "%s: option `--%s' doesn't allow an argument\n",
                                 argv[0], pfound->name);
                      else
                         
                        fprintf ((&_iob[2]) ,
                             "%s: option `%c%s' doesn't allow an argument\n",
                             argv[0], argv[optind - 1][0], pfound->name);
                    }
                  nextchar += my_strlen (nextchar);
                  return '\0' ;
                }
            }
          else if (pfound->has_arg == 1)
            {
              if (optind < argc)
                optarg = argv[optind++];
              else
                {
                  if (opterr)
                    fprintf ((&_iob[2]) , "%s: option `%s' requires an argument\n",
                             argv[0], argv[optind - 1]);
                  nextchar += my_strlen (nextchar);
                  return optstring[0] == ':' ? ':' : '\0' ;
                }
            }
          nextchar += my_strlen (nextchar);
          if (longind != 0 )
            *longind = option_index;
          if (pfound->flag)
            {
              *(pfound->flag) = pfound->val;
              return 0;
            }
          return pfound->val;
        }
      if (!long_only || argv[optind][1] == '-'
          || my_index (optstring, *nextchar) == 0 )
        {
          if (opterr)
            {
              if (argv[optind][1] == '-')
                 
                fprintf ((&_iob[2]) , "%s: unrecognized option `--%s'\n",
                         argv[0], nextchar);
              else
                 
                fprintf ((&_iob[2]) , "%s: unrecognized option `%c%s'\n",
                         argv[0], argv[optind][0], nextchar);
            }
          nextchar = (char *) "";
          optind++;
          return '\0' ;
        }
    }
  {
    char c = *nextchar++;
    char *temp = my_index (optstring, c);
    if (*nextchar == '\0')
      ++optind;
    if (temp == 0  || c == ':')
      {
        if (opterr)
          {
             
            fprintf ((&_iob[2]) , "%s: illegal option -- %c\n", argv[0], c);
          }
        optopt = c;
        return '\0' ;
      }
    if (temp[1] == ':')
      {
        if (temp[2] == ':')
          {
             
            if (*nextchar != '\0')
              {
                optarg = nextchar;
                optind++;
              }
            else
              optarg = 0;
            nextchar = 0 ;
          }
        else
          {
             
            if (*nextchar != '\0')
              {
                optarg = nextchar;
                 
                optind++;
              }
            else if (optind == argc)
              {
                if (opterr)
                  {
                     
                    fprintf ((&_iob[2]) , "%s: option requires an argument -- %c\n",
                             argv[0], c);
                  }
                optopt = c;
                if (optstring[0] == ':')
                  c = ':';
                else
                  c = '\0' ;
              }
            else
               
              optarg = argv[optind++];
            nextchar = 0 ;
          }
      }
    return c;
  }
}
int
getopt (argc, argv, optstring)
     int argc;
     char *const *argv;
     const char *optstring;
{
  return _getopt_internal (argc, argv, optstring,
                           (const struct option *) 0,
                           (int *) 0,
                           0);
}
int
getopt_long (argc, argv, options, long_options, opt_index)
     int argc;
     char *const *argv;
     const char *options;
     const struct option *long_options;
     int *opt_index;
{
  return _getopt_internal (argc, argv, options, long_options, opt_index, 0);
}
static char  *license_msg[] = {
"   Copyright (C) 1992-1993 Jean-loup Gailly",
"   This program is free software; you can redistribute it and/or modify",
"   it under the terms of the GNU General Public License as published by",
"   the Free Software Foundation; either version 2, or (at your option)",
"   any later version.",
"",
"   This program is distributed in the hope that it will be useful,",
"   but WITHOUT ANY WARRANTY; without even the implied warranty of",
"   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
"   GNU General Public License for more details.",
"",
"   You should have received a copy of the GNU General Public License",
"   along with this program; if not, write to the Free Software",
"   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.",
0};
extern  char    _ctype_[];
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
struct  stat {
        dev_t   st_dev;
        ino_t   st_ino;
        mode_t  st_mode;
        short   st_nlink;
        uid_t   st_uid;
        gid_t   st_gid;
        dev_t   st_rdev;
        off_t   st_size;
        time_t  st_atime;
        int     st_spare1;
        time_t  st_mtime;
        int     st_spare2;
        time_t  st_ctime;
        int     st_spare3;
        long    st_blksize;
        long    st_blocks;
        long    st_spare4[2];
};
int     chmod  (const char *, unsigned short )  ;
int     fstat  (int, struct stat *)  ;
int     mkdir  (const char *, unsigned short )  ;
int     mkfifo  (const char *, unsigned short )  ;
int     stat  (const char *, struct stat *)  ;
mode_t  umask  (unsigned short )  ;
         
         
         
extern int errno;
extern int maxbits;       
extern int block_mode;    
extern int lzw     (int in, int out)  ;
extern int unlzw   (int in, int out)  ;
                 
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
struct flock {
        short   l_type;          
        short   l_whence;        
        long    l_start;         
        long    l_len;           
        short   l_pid;           
        short   l_xxx;           
};
struct eflock {
        short   l_type;          
        short   l_whence;        
        long    l_start;         
        long    l_len;           
        short   l_pid;           
        short   l_xxx;           
        long    l_rpid;          
        long    l_rsys;          
};
int     open  (const char *, int, ...)  ;
int     creat  (const char *, unsigned short )  ;
int     fcntl  (int, int, ...)  ;
extern void     _exit  (int)  ;
extern int      access  (const char *, int)  ;
extern unsigned alarm  (unsigned int)  ;
extern int      chdir  (const char *)  ;
extern int      chmod  (const char *, unsigned short )  ;
extern int      chown  (const char *, unsigned short , unsigned short )  ;
extern int      close  (int)  ;
extern char     *ctermid  (char *)  ;
extern char     *cuserid  (char *)  ;
extern int      dup  (int)  ;
extern int      dup2  (int, int)  ;
extern int      execl  (const char *, const char *, ...)  ;
extern int      execle  (const char *, const char *, ...)  ;
extern int      execlp  (const char *, const char *, ...)  ;
extern int      execv  (const char *, char *const *)  ;
extern int      execve  (const char *, char *const *, char *const *)  ;
extern int      execvp  (const char *, char *const *)  ;
extern pid_t    fork( );
extern long     fpathconf  (int, int)  ;
extern char     *getcwd  (char *, long unsigned int )  ;
extern gid_t    getegid( );
extern uid_t    geteuid( );
extern gid_t    getgid( );
extern int      getgroups  (int, unsigned short *)  ;
extern char     *getlogin( );
extern pid_t    getpgrp( );
extern pid_t    getpid( );
extern pid_t    getppid( );
extern uid_t    getuid( );
extern int      isatty  (int)  ;
extern int      link  (const char *, const char *)  ;
extern off_t    lseek  (int, long , int)  ;
extern long     pathconf  (const char *, int)  ;
extern int      pause( );
extern int      pipe  (int *)  ;
extern int      read  (int, void *, long unsigned int )  ;
extern int      rmdir  (const char *)  ;
extern int      setgid  (unsigned short )  ;
extern int      setpgid  (int , int )  ;
extern pid_t    setsid( );
extern int      setuid  (unsigned short )  ;
extern unsigned sleep  (unsigned int)  ;
extern long     sysconf  (int)  ;
extern pid_t    tcgetpgrp  (int)  ;
extern int      tcsetpgrp  (int, int )  ;
extern char     *ttyname  (int)  ;
extern int      unlink  (const char *)  ;
extern int      write  (int, const void *, long unsigned int )  ;
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
extern void *   malloc  (long unsigned int )  ;
extern int      qsort  (void *, long unsigned int , long unsigned int , int (*) (const void *, const void *))  ;
extern int      rand( );
extern void *   realloc  (void *, long unsigned int )  ;
extern int      srand  (unsigned int)  ;
extern int    mbtowc  (short unsigned int *, const char *, long unsigned int )  ;
extern int    wctomb  (char *, short unsigned int )  ;
extern size_t mbstowcs  (short unsigned int *, const char *, long unsigned int )  ;
extern size_t wcstombs  (char *, const short unsigned int *, long unsigned int )  ;
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
extern  void rewinddir  (DIR *)  ;
struct  dirent {
        off_t           d_off;           
        unsigned long   d_fileno;        
        unsigned short  d_reclen;        
        unsigned short  d_namlen;        
        char            d_name[255+1];   
};
   typedef struct dirent dir_type;
struct  utimbuf {
        time_t  actime;          
        time_t  modtime;         
};
int     utime( );
typedef void  (*sig_type)  (int)  ;
                 
 uch    inbuf [   0x8000  +64  ] ;
 uch    outbuf [  16384 +2048  ] ;
 ush    d_buf [   0x8000  ] ;
 uch    window [  2L*0x8000  ] ;
     ush    prev  [  1L<<16  ] ;
                 
int ascii = 0;         
int to_stdout = 0;     
int decompress = 0;    
int force = 0;         
int no_name = -1;      
int no_time = -1;      
int recursive = 0;     
int list = 0;          
int verbose = 0;       
int quiet = 0;         
int do_lzw = 0;        
int test = 0;          
int foreground;        
char *progname;        
int maxbits = 16 ;    
int method = 8 ; 
int level = 6;         
int exit_code = 0 ;    
int save_orig_name;    
int last_member;       
int part_nb;           
long time_stamp;       
long ifile_size;       
char *env;             
char **args = 0 ;    
char z_suffix[30 +1];  
int  z_len;            
long bytes_in;              
long bytes_out;             
long total_in = 0;          
long total_out = 0;         
char ifname[1024 ];  
char ofname[1024 ];  
int  remove_ofname = 0;     
struct stat istat;          
int  ifd;                   
int  ofd;                   
unsigned insize;            
unsigned inptr;             
unsigned outcnt;            
struct option longopts[] =
{
    {"ascii",      0, 0, 'a'},  
    {"to-stdout",  0, 0, 'c'},  
    {"stdout",     0, 0, 'c'},  
    {"decompress", 0, 0, 'd'},  
    {"uncompress", 0, 0, 'd'},  
    {"force",      0, 0, 'f'},  
    {"help",       0, 0, 'h'},  
    {"list",       0, 0, 'l'},  
    {"license",    0, 0, 'L'},  
    {"no-name",    0, 0, 'n'},  
    {"name",       0, 0, 'N'},  
    {"quiet",      0, 0, 'q'},  
    {"silent",     0, 0, 'q'},  
    {"recursive",  0, 0, 'r'},  
    {"suffix",     1, 0, 'S'},  
    {"test",       0, 0, 't'},  
    {"no-time",    0, 0, 'T'},  
    {"verbose",    0, 0, 'v'},  
    {"version",    0, 0, 'V'},  
    {"fast",       0, 0, '1'},  
    {"best",       0, 0, '9'},  
    {"lzw",        0, 0, 'Z'},  
    {"bits",       1, 0, 'b'},  
    { 0, 0, 0, 0 }
};
static  void usage         (void)  ;
static  void help          (void)  ;
static  void license       (void)  ;
static  void version       (void)  ;
static  void treat_stdin   (void)  ;
static  void treat_file    (char *iname)  ;
static  int create_outfile  (void)  ;
static  int  do_stat       (char *name, struct stat *sbuf)  ;
static  char *get_suffix   (char *name)  ;
static  int  get_istat     (char *iname, struct stat *sbuf)  ;
static  int  make_ofname   (void)  ;
static  int  same_file     (struct stat *stat1, struct stat *stat2)  ;
static  int name_too_long  (char *name, struct stat *statb)  ;
static  void shorten_name   (char *name)  ;
static  int  get_method    (int in)  ;
static  void do_list       (int ifd, int method)  ;
static  int  check_ofname  (void)  ;
static  void copy_stat     (struct stat *ifstat)  ;
static  void do_exit       (int exitcode)  ;
      int main           (int argc, char **argv)  ;
int (*work)  (int infile, int outfile)   = zip;  
static  void treat_dir     (char *dir)  ;
static  void reset_times   (char *name, struct stat *statb)  ;
static  void usage()
{
    fprintf((&_iob[2]) , "usage: %s [-%scdfhlLnN%stvV19] [-S suffix] [file ...]\n",
            progname,
            "",
            "r"
            );
}
static  void help()
{
    static char  *help_msg[] = {
 " -c --stdout      write on standard output, keep original files unchanged",
 " -d --decompress  decompress",
 " -f --force       force overwrite of output file and compress links",
 " -h --help        give this help",
 " -l --list        list compressed file contents",
 " -L --license     display software license",
 " -n --no-name     do not save or restore the original name and time stamp",
 " -N --name        save or restore the original name and time stamp",
 " -q --quiet       suppress all warnings",
 " -r --recursive   operate recursively on directories",
 " -S .suf  --suffix .suf     use suffix .suf on compressed files",
 " -t --test        test compressed file integrity",
 " -v --verbose     verbose mode",
 " -V --version     display version number",
 " -1 --fast        compress faster",
 " -9 --best        compress better",
 " file...          files to (de)compress. If none given, use standard input.",
  0};
    char **p = help_msg;
    fprintf((&_iob[2]) ,"%s %s (%s)\n", progname, "1.2.4" , "18 Aug 93" );
    usage();
    while (*p) fprintf((&_iob[2]) , "%s\n", *p++);
}
static  void license()
{
    char **p = license_msg;
    fprintf((&_iob[2]) ,"%s %s (%s)\n", progname, "1.2.4" , "18 Aug 93" );
    while (*p) fprintf((&_iob[2]) , "%s\n", *p++);
}
static  void version()
{
    fprintf((&_iob[2]) ,"%s %s (%s)\n", progname, "1.2.4" , "18 Aug 93" );
    fprintf((&_iob[2]) , "Compilation options:\n%s %s ", "DIRENT" , "UTIME" );
    fprintf((&_iob[2]) , "HAVE_UNISTD_H ");
    fprintf((&_iob[2]) , "\n");
}
int main (argc, argv)
    int argc;
    char **argv;
{
    int file_count;      
    int proglen;         
    int optc;            
     ;  
    progname = basename(argv[0]);
    proglen = strlen(progname);
    if (proglen > 4 && (strcmp(( progname+proglen-4 ),(  ".exe" )) == 0) ) {
        progname[proglen-4] = '\0';
    }
    env = add_envopt(&argc, &argv, "GZIP" );
    if (env != 0 ) args = argv;
    foreground = signal(2 , (void (*)())1 ) != (void (*)())1 ;
    if (foreground) {
        (void) signal (2 , (sig_type)abort_gzip);
    }
    if (signal(15 , (void (*)())1 ) != (void (*)())1 ) {
        (void) signal(15 , (sig_type)abort_gzip);
    }
    if (signal(1 , (void (*)())1 ) != (void (*)())1 ) {
        (void) signal(1 ,  (sig_type)abort_gzip);
    }
    if (  strncmp(progname, "un",  2) == 0      
       || strncmp(progname, "gun", 3) == 0) {   
        decompress = 1;
    } else if ((strcmp(( progname+1 ),(  "cat" )) == 0)         
            || (strcmp(( progname ),(  "gzcat" )) == 0) ) {     
        decompress = to_stdout = 1;
    }
    strncpy(z_suffix, ".gz" , sizeof(z_suffix)-1);
    z_len = strlen(z_suffix);
    while ((optc = getopt_long (argc, argv, "ab:cdfhH?lLmMnNqrS:tvVZ123456789",
                                longopts, (int *)0)) != (-1) ) {
        switch (optc) {
        case 'a':
            ascii = 1; break;
        case 'b':
            maxbits = atoi(optarg);
            break;
        case 'c':
            to_stdout = 1; break;
        case 'd':
            decompress = 1; break;
        case 'f':
            force++; break;
        case 'h': case 'H': case '?':
            help(); do_exit(0 ); break;
        case 'l':
            list = decompress = to_stdout = 1; break;
        case 'L':
            license(); do_exit(0 ); break;
        case 'm':  
            no_time = 1; break;
        case 'M':  
            no_time = 0; break;
        case 'n':
            no_name = no_time = 1; break;
        case 'N':
            no_name = no_time = 0; break;
        case 'q':
            quiet = 1; verbose = 0; break;
        case 'r':
            recursive = 1; break;
        case 'S':
            z_len = strlen(optarg);
            strcpy(z_suffix, optarg);
            break;
        case 't':
            test = decompress = to_stdout = 1;
            break;
        case 'v':
            verbose++; quiet = 0; break;
        case 'V':
            version(); do_exit(0 ); break;
        case 'Z':
            fprintf((&_iob[2]) , "%s: -Z not supported in this version\n",
                    progname);
            usage();
            do_exit(1 ); break;
        case '1':  case '2':  case '3':  case '4':
        case '5':  case '6':  case '7':  case '8':  case '9':
            level = optc - '0';
            break;
        default:
             
            usage();
            do_exit(1 );
        }
    }  
    if (no_time < 0) no_time = decompress;
    if (no_name < 0) no_name = decompress;
    file_count = argc - optind;
    if (ascii && !quiet) {
        fprintf((&_iob[2]) , "%s: option --ascii ignored on this system\n",
                progname);
    }
    if ((z_len == 0 && !decompress) || z_len > 30 ) {
        fprintf((&_iob[2]) , "%s: incorrect suffix '%s'\n",
                progname, optarg);
        do_exit(1 );
    }
    if (do_lzw && !decompress) work = lzw;
     ;
     ;
     ;
     ;
     ;
    if (file_count != 0) {
        if (to_stdout && !test && !list && (!decompress || !ascii)) {
             ;
        }
        while (optind < argc) {
            treat_file(argv[optind++]);
        }
    } else {   
        treat_stdin();
    }
    if (list && !quiet && file_count > 1) {
        do_list(-1, -1);  
    }
    do_exit(exit_code);
    return exit_code;  
}
static  void treat_stdin()
{
    if (!force && !list &&
        isatty((( (struct _iobuf  *)(decompress ? (&_iob[0])  : (&_iob[1]) ) )->_file) )) {
         
        fprintf((&_iob[2]) ,
    "%s: compressed data not %s a terminal. Use -f to force %scompression.\n",
                progname, decompress ? "read from" : "written to",
                decompress ? "de" : "");
        fprintf((&_iob[2]) ,"For help, type: %s -h\n", progname);
        do_exit(1 );
    }
    if (decompress || !ascii) {
         ;
    }
    if (!test && !list && (!decompress || !ascii)) {
         ;
    }
    strcpy(ifname, "stdin");
    strcpy(ofname, "stdout");
    time_stamp = 0;  
    if (list || !no_time) {
        if (fstat((( (&_iob[0])  )->_file) , &istat) != 0) {
            error("fstat(stdin)");
        }
            time_stamp = istat.st_mtime;
    }
    ifile_size = -1L;  
    clear_bufs();  
    to_stdout = 1;
    part_nb = 0;
    if (decompress) {
        method = get_method(ifd);
        if (method < 0) {
            do_exit(exit_code);  
        }
    }
    if (list) {
        do_list(ifd, method);
        return;
    }
    for (;;) {
        if ((*work)((( (&_iob[0])  )->_file) , (( (&_iob[1])  )->_file) ) != 0 ) return;
        if (!decompress || last_member || inptr == insize) break;
         
        method = get_method(ifd);
        if (method < 0) return;  
        bytes_out = 0;             
    }
    if (verbose) {
        if (test) {
            fprintf((&_iob[2]) , " OK\n");
        } else if (!decompress) {
            display_ratio(bytes_in-(bytes_out-header_bytes), bytes_in, (&_iob[2]) );
            fprintf((&_iob[2]) , "\n");
        }
    }
}
static  void treat_file(iname)
    char *iname;
{
    if ((strcmp(( iname ),(  "-" )) == 0) ) {
        int cflag = to_stdout;
        treat_stdin();
        to_stdout = cflag;
        return;
    }
    if (get_istat(iname, &istat) != 0 ) return;
    if (((( istat.st_mode )&0170000 ) == 0040000 ) ) {
        if (recursive) {
            struct stat st;
            st = istat;
            treat_dir(iname);
             
            reset_times (iname, &st);
        } else
        {if (!quiet) fprintf  ((&_iob[2]) ,"%s: %s is a directory -- ignored\n", progname, ifname)  ; if (exit_code == 0 ) exit_code = 2 ;} ;
        return;
    }
    if (!((( istat.st_mode )&0170000 ) == 0100000 ) ) {
        {if (!quiet) fprintf  ((&_iob[2]) ,
              "%s: %s is not a directory or a regular file - ignored\n",
              progname, ifname)  ; if (exit_code == 0 ) exit_code = 2 ;} ;
        return;
    }
    if (istat.st_nlink > 1 && !to_stdout && !force) {
        {if (!quiet) fprintf  ((&_iob[2]) , "%s: %s has %d other link%c -- unchanged\n",
              progname, ifname,
              (int)istat.st_nlink - 1, istat.st_nlink > 2 ? 's' : ' ')  ; if (exit_code == 0 ) exit_code = 2 ;} ;
        return;
    }
    ifile_size = istat.st_size;
    time_stamp = no_time && !list ? 0 : istat.st_mtime;
    if (to_stdout && !list && !test) {
        strcpy(ofname, "stdout");
    } else if (make_ofname() != 0 ) {
        return;
    }
    ifd = open( ifname ,   ascii && !decompress ? 0  : 0  | 0  ,  
               (0000400  | 0000200 )  ) ;
    if (ifd == -1) {
        fprintf((&_iob[2]) , "%s: ", progname);
        perror(ifname);
        exit_code = 1 ;
        return;
    }
    clear_bufs();  
    part_nb = 0;
    if (decompress) {
        method = get_method(ifd);  
        if (method < 0) {
            close(ifd);
            return;                
        }
    }
    if (list) {
        do_list(ifd, method);
        close(ifd);
        return;
    }
    if (to_stdout) {
        ofd = (( (&_iob[1])  )->_file) ;
         
    } else {
        if (create_outfile() != 0 ) return;
        if (!decompress && save_orig_name && !verbose && !quiet) {
            fprintf((&_iob[2]) , "%s: %s compressed to %s\n",
                    progname, ifname, ofname);
        }
    }
    if (!save_orig_name) save_orig_name = !no_name;
    if (verbose) {
        fprintf((&_iob[2]) , "%s:\t%s", ifname, (int)strlen(ifname) >= 15 ? 
                "" : ((int)strlen(ifname) >= 7 ? "\t" : "\t\t"));
    }
    for (;;) {
        if ((*work)(ifd, ofd) != 0 ) {
            method = -1;  
            break;
        }
        if (!decompress || last_member || inptr == insize) break;
         
        method = get_method(ifd);
        if (method < 0) break;     
        bytes_out = 0;             
    }
    close(ifd);
    if (!to_stdout && close(ofd)) {
        write_error();
    }
    if (method == -1) {
        if (!to_stdout) unlink (ofname);
        return;
    }
    if(verbose) {
        if (test) {
            fprintf((&_iob[2]) , " OK");
        } else if (decompress) {
            display_ratio(bytes_out-(bytes_in-header_bytes), bytes_out,(&_iob[2]) );
        } else {
            display_ratio(bytes_in-(bytes_out-header_bytes), bytes_in, (&_iob[2]) );
        }
        if (!test && !to_stdout) {
            fprintf((&_iob[2]) , " -- replaced with %s", ofname);
        }
        fprintf((&_iob[2]) , "\n");
    }
    if (!to_stdout) {
        copy_stat(&istat);
    }
}
static  int create_outfile()
{
    struct stat ostat;  
    int flags = 1  | 0x0200   | 0x0800   | 0 ;
    if (ascii && decompress) {
        flags &= ~0 ;  
    }
    for (;;) {
         
        if (check_ofname() != 0 ) {
            close(ifd);
            return 1 ;
        }
         
        remove_ofname = 1;
        ofd = open( ofname ,   flags ,   (0000400  | 0000200 )  ) ;
        if (ofd == -1) {
            perror(ofname);
            close(ifd);
            exit_code = 1 ;
            return 1 ;
        }
         
        if (fstat(ofd, &ostat) != 0) {
            fprintf((&_iob[2]) , "%s: ", progname);
            perror(ofname);
            close(ifd); close(ofd);
            unlink(ofname);
            exit_code = 1 ;
            return 1 ;
        }
        if (!name_too_long(ofname, &ostat)) return 0 ;
        if (decompress) {
             
            {if (!quiet) fprintf  ((&_iob[2]) , "%s: %s: warning, name truncated\n",
                  progname, ofname)  ; if (exit_code == 0 ) exit_code = 2 ;} ;
            return 0 ;
        }
        close(ofd);
        unlink(ofname);
        shorten_name(ofname);
    }
}
static  int do_stat(name, sbuf)
    char *name;
    struct stat *sbuf;
{
    errno = 0;
    if (!to_stdout && !force) {
        return stat(name, sbuf);
    }
    return stat(name, sbuf);
}
static  char *get_suffix(name)
    char *name;
{
    int nlen, slen;
    char suffix[30 +3];  
    static char *known_suffixes[] =
       {z_suffix, ".gz", ".z", ".taz", ".tgz", "-gz", "-z", "_z",
          0 };
    char **suf = known_suffixes;
    if ((strcmp(( z_suffix ),(  "z" )) == 0) ) suf++;  
    nlen = strlen(name);
    if (nlen <= 30 +2) {
        strcpy(suffix, name);
    } else {
        strcpy(suffix, name+nlen- 30 -2);
    }
    strlwr(suffix);
    slen = strlen(suffix);
    do {
       int s = strlen(*suf);
       if (slen > s && suffix[slen-s-1] != '/' 
           && (strcmp(( suffix + slen - s ),(  *suf )) == 0) ) {
           return name+nlen-s;
       }
    } while (*++suf != 0 );
    return 0 ;
}
static  int get_istat(iname, sbuf)
    char *iname;
    struct stat *sbuf;
{
    int ilen;   
    static char *suffixes[] = {z_suffix, ".gz", ".z", "-z", ".Z", 0 };
    char **suf = suffixes;
    char *s;
    strcpy(ifname, iname);
    if (do_stat(ifname, sbuf) == 0) return 0 ;
    if (!decompress || errno != 2 ) {
        perror(ifname);
        exit_code = 1 ;
        return 1 ;
    }
    s = get_suffix(ifname);
    if (s != 0 ) {
        perror(ifname);  
        exit_code = 1 ;
        return 1 ;
    }
    ilen = strlen(ifname);
    if ((strcmp(( z_suffix ),(  ".gz" )) == 0) ) suf++;
    do {
        s = *suf;
        strcat(ifname, s);
        if (do_stat(ifname, sbuf) == 0) return 0 ;
        ifname[ilen] = '\0';
    } while (*++suf != 0 );
    strcat(ifname, z_suffix);
    perror(ifname);
    exit_code = 1 ;
    return 1 ;
}
static  int make_ofname()
{
    char *suff;             
    strcpy(ofname, ifname);
    suff = get_suffix(ofname);
    if (decompress) {
        if (suff == 0 ) {
             
            if (!recursive && (list || test)) return 0 ;
             
            if (verbose || (!recursive && !quiet)) {
                {if (!quiet) fprintf  ((&_iob[2]) ,"%s: %s: unknown suffix -- ignored\n",
                      progname, ifname)  ; if (exit_code == 0 ) exit_code = 2 ;} ;
            }
            return 2 ;
        }
         
        strlwr(suff);
        if ((strcmp(( suff ),(  ".tgz" )) == 0)  || (strcmp(( suff ),(  ".taz" )) == 0) ) {
            strcpy(suff, ".tar");
        } else {
            *suff = '\0';  
        }
    } else if (suff != 0 ) {
         
        if (verbose || (!recursive && !quiet)) {
            fprintf((&_iob[2]) , "%s: %s already has %s suffix -- unchanged\n",
                    progname, ifname, suff);
        }
        if (exit_code == 0 ) exit_code = 2 ;
        return 2 ;
    } else {
        save_orig_name = 0;
        strcat(ofname, z_suffix);
    }  
    return 0 ;
}
static  int get_method(in)
    int in;         
{
    uch flags;      
    char magic[2];  
    ulg stamp;      
    if (force && to_stdout) {
        magic[0] = (char)(inptr < insize ? inbuf[inptr++] : fill_inbuf(1)) ;
        magic[1] = (char)(inptr < insize ? inbuf[inptr++] : fill_inbuf(1)) ;
         
    } else {
        magic[0] = (char)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;
        magic[1] = (char)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;
    }
    method = -1;                  
    part_nb++;                    
    header_bytes = 0;
    last_member = 0 ;
    if (memcmp(magic, "\037\213" , 2) == 0
        || memcmp(magic, "\037\236" , 2) == 0) {
        method = (int)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;
        if (method != 8 ) {
            fprintf((&_iob[2]) ,
                    "%s: %s: unknown method %d -- get newer version of gzip\n",
                    progname, ifname, method);
            exit_code = 1 ;
            return -1;
        }
        work = unzip;
        flags  = (uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;
        if ((flags & 0x20 ) != 0) {
            fprintf((&_iob[2]) ,
                    "%s: %s is encrypted -- get newer version of gzip\n",
                    progname, ifname);
            exit_code = 1 ;
            return -1;
        }
        if ((flags & 0x02 ) != 0) {
            fprintf((&_iob[2]) ,
           "%s: %s is a a multi-part gzip file -- get newer version of gzip\n",
                    progname, ifname);
            exit_code = 1 ;
            if (force <= 1) return -1;
        }
        if ((flags & 0xC0 ) != 0) {
            fprintf((&_iob[2]) ,
                    "%s: %s has flags 0x%x -- get newer version of gzip\n",
                    progname, ifname, flags);
            exit_code = 1 ;
            if (force <= 1) return -1;
        }
        stamp  = (ulg)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;
        stamp |= ((ulg)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ) << 8;
        stamp |= ((ulg)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ) << 16;
        stamp |= ((ulg)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ) << 24;
        if (stamp != 0 && !no_time) time_stamp = stamp;
        (void)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;   
        (void)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;   
        if ((flags & 0x02 ) != 0) {
            unsigned part = (unsigned)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;
            part |= ((unsigned)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) )<<8;
            if (verbose) {
                fprintf((&_iob[2]) ,"%s: %s: part number %u\n",
                        progname, ifname, part);
            }
        }
        if ((flags & 0x04 ) != 0) {
            unsigned len = (unsigned)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;
            len |= ((unsigned)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) )<<8;
            if (verbose) {
                fprintf((&_iob[2]) ,"%s: %s: extra field of %u bytes ignored\n",
                        progname, ifname, len);
            }
            while (len--) (void)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;
        }
         
        if ((flags & 0x08 ) != 0) {
            if (no_name || (to_stdout && !list) || part_nb > 1) {
                 
                char c;  
                do {c=(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;} while (c != 0);
            } else {
                 
                char *p = basename(ofname);
                char *base = p;
                for (;;) {
                    *p = (char)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  ;
                    if (*p++ == '\0') break;
                    if (p >= ofname+sizeof(ofname)) {
                        error("corrupted input -- file name too large");
                    }
                }
                if (!list) {
                    ;
                   if (base) list=0;  
                }
            }  
        }  
         
        if ((flags & 0x10 ) != 0) {
            while ((inptr < insize ? inbuf[inptr++] : fill_inbuf(0))   != 0)   ;
        }
        if (part_nb == 1) {
            header_bytes = inptr + 2*sizeof(long);  
        }
    } else if (memcmp(magic, "\120\113\003\004" , 2) == 0 && inptr == 2
            && memcmp((char*)inbuf, "\120\113\003\004" , 4) == 0) {
         
        inptr = 0;
        work = unzip;
        if (check_zipfile(in) != 0 ) return -1;
         
        last_member = 1;
    } else if (memcmp(magic, "\037\036" , 2) == 0) {
        work = unpack;
        method = 2 ;
    } else if (memcmp(magic, "\037\235" , 2) == 0) {
        work = unlzw;
        method = 1 ;
        last_member = 1;
    } else if (memcmp(magic, "\037\240" , 2) == 0) {
        work = unlzh;
        method = 3 ;
        last_member = 1;
    } else if (force && to_stdout && !list) {  
        method = 0 ;
        work = copy;
        inptr = 0;
        last_member = 1;
    }
    if (method >= 0) return method;
    if (part_nb == 1) {
        fprintf((&_iob[2]) , "\n%s: %s: not in gzip format\n", progname, ifname);
        exit_code = 1 ;
        return -1;
    } else {
        {if (!quiet) fprintf  ((&_iob[2]) , "\n%s: %s: decompression OK, trailing garbage ignored\n",
              progname, ifname)  ; if (exit_code == 0 ) exit_code = 2 ;} ;
        return -2;
    }
}
static  void do_list(ifd, method)
    int ifd;      
    int method;   
{
    ulg crc;   
    static int first_time = 1;
    static char* methods[9 ] = {
        "store",   
        "compr",   
        "pack ",   
        "lzh  ",   
        "", "", "", "",  
        "defla"};  
    char *date;
    if (first_time && method >= 0) {
        first_time = 0;
        if (verbose)  {
            printf("method  crc     date  time  ");
        }
        if (!quiet) {
            printf("compressed  uncompr. ratio uncompressed_name\n");
        }
    } else if (method < 0) {
        if (total_in <= 0 || total_out <= 0) return;
        if (verbose) {
            printf("                            %9lu %9lu ",
                   total_in, total_out);
        } else if (!quiet) {
            printf("%9ld %9ld ", total_in, total_out);
        }
        display_ratio(total_out-(total_in-header_bytes), total_out, (&_iob[1]) );
         
        printf(" (totals)\n");
        return;
    }
    crc = (ulg)~0;  
    bytes_out = -1L;
    bytes_in = ifile_size;
    if (method == 8  && !last_member) {
        bytes_in = (long)lseek(ifd, (off_t)(-8), 2 );
        if (bytes_in != -1L) {
            uch buf[8];
            bytes_in += 8L;
            if (read(ifd, (char*)buf, sizeof(buf)) != sizeof(buf)) {
                read_error();
            }
            crc       = ((ulg)(((ush)(uch)((  buf  )[0]) | ((ush)(uch)((  buf  )[1]) << 8)) ) | ((ulg)(((ush)(uch)(( ( buf )+2 )[0]) | ((ush)(uch)(( ( buf )+2 )[1]) << 8)) ) << 16)) ;
            bytes_out = ((ulg)(((ush)(uch)((  buf+4  )[0]) | ((ush)(uch)((  buf+4  )[1]) << 8)) ) | ((ulg)(((ush)(uch)(( ( buf+4 )+2 )[0]) | ((ush)(uch)(( ( buf+4 )+2 )[1]) << 8)) ) << 16)) ;
        }
    }
    date = ctime((time_t*)&time_stamp) + 4;  
    date[12] = '\0';                
    if (verbose) {
        printf("%5s %08lx %11s ", methods[method], crc, date);
    }
    printf("%9ld %9ld ", bytes_in, bytes_out);
    if (bytes_in  == -1L) {
        total_in = -1L;
        bytes_in = bytes_out = header_bytes = 0;
    } else if (total_in >= 0) {
        total_in  += bytes_in;
    }
    if (bytes_out == -1L) {
        total_out = -1L;
        bytes_in = bytes_out = header_bytes = 0;
    } else if (total_out >= 0) {
        total_out += bytes_out;
    }
    display_ratio(bytes_out-(bytes_in-header_bytes), bytes_out, (&_iob[1]) );
    printf(" %s\n", ofname);
}
static  int same_file(stat1, stat2)
    struct stat *stat1;
    struct stat *stat2;
{
    return stat1->st_ino   == stat2->st_ino
        && stat1->st_dev   == stat2->st_dev
            ;
}
static  int name_too_long(name, statb)
    char *name;            
    struct stat *statb;    
{
    int s = strlen(name);
    char c = name[s-1];
    struct stat tstat;  
    int res;
    tstat = *statb;       
    name[s-1] = '\0';
    res = stat(name, &tstat) == 0 && same_file(statb, &tstat);
    name[s-1] = c;
     ;
    return res;
}
static  void shorten_name(name)
    char *name;
{
    int len;                  
    char *trunc = 0 ;       
    int plen;                 
    int min_part = 3 ;  
    char *p;
    len = strlen(name);
    if (decompress) {
        if (len <= 1) error("name too short");
        name[len-1] = '\0';
        return;
    }
    p = get_suffix(name);
    if (p == 0 ) error("can't recover suffix\n");
    *p = '\0';
    save_orig_name = 1;
    if (len > 4 && (strcmp(( p-4 ),(  ".tar" )) == 0) ) {
        strcpy(p-4, ".tgz");
        return;
    }
    do {
        p = strrchr(name, '/' );
        p = p ? p+1 : name;
        while (*p) {
            plen = strcspn(p, "." );
            p += plen;
            if (plen > min_part) trunc = p-1;
            if (*p) p++;
        }
    } while (trunc == 0  && --min_part != 0);
    if (trunc != 0 ) {
        do {
            trunc[0] = trunc[1];
        } while (*trunc++);
        trunc--;
    } else {
        trunc = strrchr(name, "." [0]);
        if (trunc == 0 ) error("internal error in shorten_name");
        if (trunc[1] == '\0') trunc--;  
    }
    strcpy(trunc, z_suffix);
}
static  int check_ofname()
{
    struct stat ostat;  
    errno = 0;
    while (stat(ofname, &ostat) != 0) {
        if (errno != 63 ) return 0;  
        shorten_name(ofname);
    }
    if (!decompress && name_too_long(ofname, &ostat)) {
        shorten_name(ofname);
        if (stat(ofname, &ostat) != 0) return 0;
    }
    if (same_file(&istat, &ostat)) {
        if ((strcmp(( ifname ),(  ofname )) == 0) ) {
            fprintf((&_iob[2]) , "%s: %s: cannot %scompress onto itself\n",
                    progname, ifname, decompress ? "de" : "");
        } else {
            fprintf((&_iob[2]) , "%s: %s and %s are the same file\n",
                    progname, ifname, ofname);
        }
        exit_code = 1 ;
        return 1 ;
    }
    if (!force) {
        char response[80];
        strcpy(response,"n");
        fprintf((&_iob[2]) , "%s: %s already exists;", progname, ofname);
        if (foreground && isatty((( (&_iob[0])  )->_file) )) {
            fprintf((&_iob[2]) , " do you wish to overwrite (y or n)? ");
            fflush((&_iob[2]) );
            (void)fgets(response, sizeof(response)-1, (&_iob[0]) );
        }
        if ((((_ctype_+1)[  *response  ]&01 )  ? ( *response )-'A'+'a' : ( *response ))  != 'y') {
            fprintf((&_iob[2]) , "\tnot overwritten\n");
            if (exit_code == 0 ) exit_code = 2 ;
            return 1 ;
        }
    }
    (void) chmod(ofname, 0777);
    if (unlink(ofname)) {
        fprintf((&_iob[2]) , "%s: ", progname);
        perror(ofname);
        exit_code = 1 ;
        return 1 ;
    }
    return 0 ;
}
static  void reset_times (name, statb)
    char *name;
    struct stat *statb;
{
    struct utimbuf      timep;
    timep.actime  = statb->st_atime;
    timep.modtime = statb->st_mtime;
    if (utime(name, &timep) && !((( statb->st_mode )&0170000 ) == 0040000 ) ) {
        {if (!quiet) fprintf  ((&_iob[2]) , "%s: ", progname)  ; if (exit_code == 0 ) exit_code = 2 ;} ;
        if (!quiet) perror(ofname);
    }
}
static  void copy_stat(ifstat)
    struct stat *ifstat;
{
    if (decompress && time_stamp != 0 && ifstat->st_mtime != time_stamp) {
        ifstat->st_mtime = time_stamp;
        if (verbose > 1) {
            fprintf((&_iob[2]) , "%s: time stamp restored\n", ofname);
        }
    }
    reset_times(ofname, ifstat);
    if (chmod(ofname, ifstat->st_mode & 07777)) {
        {if (!quiet) fprintf  ((&_iob[2]) , "%s: ", progname)  ; if (exit_code == 0 ) exit_code = 2 ;} ;
        if (!quiet) perror(ofname);
    }
    chown(ofname, ifstat->st_uid, ifstat->st_gid);   
    remove_ofname = 0;
    (void) chmod(ifname, 0777);
    if (unlink(ifname)) {
        {if (!quiet) fprintf  ((&_iob[2]) , "%s: ", progname)  ; if (exit_code == 0 ) exit_code = 2 ;} ;
        if (!quiet) perror(ifname);
    }
}
static  void treat_dir(dir)
    char *dir;
{
    dir_type *dp;
    DIR      *dirp;
    char     nbuf[1024 ];
    int      len;
    dirp = opendir(dir);
    if (dirp == 0 ) {
        fprintf((&_iob[2]) , "%s: %s unreadable\n", progname, dir);
        exit_code = 1 ;
        return ;
    }
    while ((dp = readdir(dirp)) != 0 ) {
        if ((strcmp(( dp->d_name ),( "." )) == 0)  || (strcmp(( dp->d_name ),( ".." )) == 0) ) {
            continue;
        }
        len = strlen(dir);
        if (len + ((int)strlen(( dp )->d_name))  + 1 < 1024  - 1) {
            strcpy(nbuf,dir);
            if (len != 0  
            ) {
                nbuf[len++] = '/' ;
            }
            strcpy(nbuf+len, dp->d_name);
            treat_file(nbuf);
        } else {
            fprintf((&_iob[2]) ,"%s: %s/%s: pathname too long\n",
                    progname, dir, dp->d_name);
            exit_code = 1 ;
        }
    }
    closedir(dirp);
}
static  void do_exit(exitcode)
    int exitcode;
{
    static int in_exit = 0;
    if (in_exit) exit(exitcode);
    in_exit = 1;
    if (env != 0 )  free(env),  env  = 0 ;
    if (args != 0 ) free((char*)args), args = 0 ;
     ;
     ;
     ;
     ;
     ;
    exit(exitcode);
}
void  abort_gzip()
{
   if (remove_ofname) {
       close(ofd);
       unlink (ofname);
   }
   do_exit(1 );
}
struct huft {
  uch e;                 
  uch b;                 
  union {
    ush n;               
    struct huft *t;      
  } v;
};
int huft_build  (unsigned *, unsigned, unsigned, ush *, ush *,
                   struct huft **, int *)  ;
int huft_free  (struct huft *)  ;
int inflate_codes  (struct huft *, struct huft *, int, int)  ;
int inflate_stored  (void)  ;
int inflate_fixed  (void)  ;
int inflate_dynamic  (void)  ;
int inflate_block  (int *)  ;
int inflate  (void)  ;
static unsigned border[] = {     
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
static ush cplens[] = {          
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};
static ush cplext[] = {          
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99};  
static ush cpdist[] = {          
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
        257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
        8193, 12289, 16385, 24577};
static ush cpdext[] = {          
        0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
        7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
        12, 12, 13, 13};
ulg bb;                          
unsigned bk;                     
ush mask_bits[] = {
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};
int lbits = 9;           
int dbits = 6;           
unsigned hufts;          
int huft_build(b, n, s, d, e, t, m)
unsigned *b;             
unsigned n;              
unsigned s;              
ush *d;                  
ush *e;                  
struct huft **t;         
int *m;                  
{
  unsigned a;                    
  unsigned c[16 +1];            
  unsigned f;                    
  int g;                         
  int h;                         
  register unsigned i;           
  register unsigned j;           
  register int k;                
  int l;                         
  register unsigned *p;          
  register struct huft *q;       
  struct huft r;                 
  struct huft *u[16 ];          
  unsigned v[288 ];             
  register int w;                
  unsigned x[16 +1];            
  unsigned *xp;                  
  int y;                         
  unsigned z;                    
  memset ((voidp)( c ), 0, (  sizeof(c) )) ;
  p = b;  i = n;
  do {
     ;
    c[*p]++;                     
    p++;                       
  } while (--i);
  if (c[0] == n)                 
  {
    *t = (struct huft *)0 ;
    *m = 0;
    return 0;
  }
  l = *m;
  for (j = 1; j <= 16 ; j++)
    if (c[j])
      break;
  k = j;                         
  if ((unsigned)l < j)
    l = j;
  for (i = 16 ; i; i--)
    if (c[i])
      break;
  g = i;                         
  if ((unsigned)l > i)
    l = i;
  *m = l;
  for (y = 1 << j; j < i; j++, y <<= 1)
    if ((y -= c[j]) < 0)
      return 2;                  
  if ((y -= c[i]) < 0)
    return 2;
  c[i] += y;
  x[1] = j = 0;
  p = c + 1;  xp = x + 2;
  while (--i) {                  
    *xp++ = (j += *p++);
  }
  p = b;  i = 0;
  do {
    if ((j = *p++) != 0)
      v[x[j]++] = i;
  } while (++i < n);
  x[0] = i = 0;                  
  p = v;                         
  h = -1;                        
  w = -l;                        
  u[0] = (struct huft *)0 ;    
  q = (struct huft *)0 ;       
  z = 0;                         
  for (; k <= g; k++)
  {
    a = c[k];
    while (a--)
    {
      while (k > w + l)
      {
        h++;
        w += l;                  
        z = (z = g - w) > (unsigned)l ? l : z;   
        if ((f = 1 << (j = k - w)) > a + 1)      
        {                        
          f -= a + 1;            
          xp = c + k;
          while (++j < z)        
          {
            if ((f <<= 1) <= *++xp)
              break;             
            f -= *xp;            
          }
        }
        z = 1 << j;              
        if ((q = (struct huft *)malloc((z + 1)*sizeof(struct huft))) ==
            (struct huft *)0 )
        {
          if (h)
            huft_free(u[0]);
          return 3;              
        }
        hufts += z + 1;          
        *t = q + 1;              
        *(t = &(q->v.t)) = (struct huft *)0 ;
        u[h] = ++q;              
        if (h)
        {
          x[h] = i;              
          r.b = (uch)l;          
          r.e = (uch)(16 + j);   
          r.v.t = q;             
          j = i >> (w - l);      
          u[h-1][j] = r;         
        }
      }
      r.b = (uch)(k - w);
      if (p >= v + n)
        r.e = 99;                
      else if (*p < s)
      {
        r.e = (uch)(*p < 256 ? 16 : 15);     
        r.v.n = (ush)(*p);              
        p++;                            
      }
      else
      {
        r.e = (uch)e[*p - s];    
        r.v.n = d[*p++ - s];
      }
      f = 1 << (k - w);
      for (j = i >> w; j < z; j += f)
        q[j] = r;
      for (j = 1 << (k - 1); i & j; j >>= 1)
        i ^= j;
      i ^= j;
      while ((i & ((1 << w) - 1)) != x[h])
      {
        h--;                     
        w -= l;
      }
    }
  }
  return y != 0 && g != 1;
}
int huft_free(t)
struct huft *t;          
{
  register struct huft *p, *q;
  p = t;
  while (p != (struct huft *)0 )
  {
    q = (--p)->v.t;
    free((char*)p);
    p = q;
  } 
  return 0;
}
int inflate_codes(tl, td, bl, bd)
struct huft *tl, *td;    
int bl, bd;              
{
  register unsigned e;   
  unsigned n, d;         
  unsigned w;            
  struct huft *t;        
  unsigned ml, md;       
  register ulg b;        
  register unsigned k;   
  b = bb;                        
  k = bk;
  w = outcnt ;                        
  ml = mask_bits[bl];            
  md = mask_bits[bd];
  for (;;)                       
  {
    {while(k<( (unsigned)bl )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
    if ((e = (t = tl + ((unsigned)b & ml))->e) > 16)
      do {
        if (e == 99)
          return 1;
        {b>>=( t->b );k-=( t->b );} 
        e -= 16;
        {while(k<( e )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
      } while ((e = (t = t->v.t + ((unsigned)b & mask_bits[e]))->e) > 16);
    {b>>=( t->b );k-=( t->b );} 
    if (e == 16)                 
    {
      window [w++] = (uch)t->v.n;
       ;
      if (w == 0x8000 )
      {
        (outcnt =( w ),flush_window()) ;
        w = 0;
      }
    }
    else                         
    {
      if (e == 15)
        break;
      {while(k<( e )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
      n = t->v.n + ((unsigned)b & mask_bits[e]);
      {b>>=( e );k-=( e );} ;
      {while(k<( (unsigned)bd )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
      if ((e = (t = td + ((unsigned)b & md))->e) > 16)
        do {
          if (e == 99)
            return 1;
          {b>>=( t->b );k-=( t->b );} 
          e -= 16;
          {while(k<( e )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
        } while ((e = (t = t->v.t + ((unsigned)b & mask_bits[e]))->e) > 16);
      {b>>=( t->b );k-=( t->b );} 
      {while(k<( e )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
      d = w - t->v.n - ((unsigned)b & mask_bits[e]);
      {b>>=( e );k-=( e );} 
       ;
      do {
        n -= (e = (e = 0x8000  - ((d &= 0x8000 -1) > w ? d : w)) > n ? n : e);
        if (w - d >= e)          
        {
          memcpy(window  + w, window  + d, e);
          w += e;
          d += e;
        }
        else                       
          do {
            window [w++] = window [d++];
             ;
          } while (--e);
        if (w == 0x8000 )
        {
          (outcnt =( w ),flush_window()) ;
          w = 0;
        }
      } while (n);
    }
  }
  outcnt  = w;                        
  bb = b;                        
  bk = k;
  return 0;
}
int inflate_stored()
{
  unsigned n;            
  unsigned w;            
  register ulg b;        
  register unsigned k;   
  b = bb;                        
  k = bk;
  w = outcnt ;                        
  n = k & 7;
  {b>>=( n );k-=( n );} ;
  {while(k<( 16 )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
  n = ((unsigned)b & 0xffff);
  {b>>=( 16 );k-=( 16 );} 
  {while(k<( 16 )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
  if (n != (unsigned)((~b) & 0xffff))
    return 1;                    
  {b>>=( 16 );k-=( 16 );} 
  while (n--)
  {
    {while(k<( 8 )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
    window [w++] = (uch)b;
    if (w == 0x8000 )
    {
      (outcnt =( w ),flush_window()) ;
      w = 0;
    }
    {b>>=( 8 );k-=( 8 );} 
  }
  outcnt  = w;                        
  bb = b;                        
  bk = k;
  return 0;
}
int inflate_fixed()
{
  int i;                 
  struct huft *tl;       
  struct huft *td;       
  int bl;                
  int bd;                
  unsigned l[288];       
  for (i = 0; i < 144; i++)
    l[i] = 8;
  for (; i < 256; i++)
    l[i] = 9;
  for (; i < 280; i++)
    l[i] = 7;
  for (; i < 288; i++)           
    l[i] = 8;
  bl = 7;
  if ((i = huft_build(l, 288, 257, cplens, cplext, &tl, &bl)) != 0)
    return i;
  for (i = 0; i < 30; i++)       
    l[i] = 5;
  bd = 5;
  if ((i = huft_build(l, 30, 0, cpdist, cpdext, &td, &bd)) > 1)
  {
    huft_free(tl);
    return i;
  }
  if (inflate_codes(tl, td, bl, bd))
    return 1;
  huft_free(tl);
  huft_free(td);
  return 0;
}
int inflate_dynamic()
{
  int i;                 
  unsigned j;
  unsigned l;            
  unsigned m;            
  unsigned n;            
  struct huft *tl;       
  struct huft *td;       
  int bl;                
  int bd;                
  unsigned nb;           
  unsigned nl;           
  unsigned nd;           
  unsigned ll[286+30];   
  register ulg b;        
  register unsigned k;   
  b = bb;
  k = bk;
  {while(k<( 5 )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
  nl = 257 + ((unsigned)b & 0x1f);       
  {b>>=( 5 );k-=( 5 );} 
  {while(k<( 5 )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
  nd = 1 + ((unsigned)b & 0x1f);         
  {b>>=( 5 );k-=( 5 );} 
  {while(k<( 4 )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
  nb = 4 + ((unsigned)b & 0xf);          
  {b>>=( 4 );k-=( 4 );} 
  if (nl > 286 || nd > 30)
    return 1;                    
  for (j = 0; j < nb; j++)
  {
    {while(k<( 3 )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
    ll[border[j]] = (unsigned)b & 7;
    {b>>=( 3 );k-=( 3 );} 
  }
  for (; j < 19; j++)
    ll[border[j]] = 0;
  bl = 7;
  if ((i = huft_build(ll, 19, 19, 0 , 0 , &tl, &bl)) != 0)
  {
    if (i == 1)
      huft_free(tl);
    return i;                    
  }
  n = nl + nd;
  m = mask_bits[bl];
  i = l = 0;
  while ((unsigned)i < n)
  {
    {while(k<( (unsigned)bl )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
    j = (td = tl + ((unsigned)b & m))->b;
    {b>>=( j );k-=( j );} 
    j = td->v.n;
    if (j < 16)                  
      ll[i++] = l = j;           
    else if (j == 16)            
    {
      {while(k<( 2 )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
      j = 3 + ((unsigned)b & 3);
      {b>>=( 2 );k-=( 2 );} 
      if ((unsigned)i + j > n)
        return 1;
      while (j--)
        ll[i++] = l;
    }
    else if (j == 17)            
    {
      {while(k<( 3 )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
      j = 3 + ((unsigned)b & 7);
      {b>>=( 3 );k-=( 3 );} 
      if ((unsigned)i + j > n)
        return 1;
      while (j--)
        ll[i++] = 0;
      l = 0;
    }
    else                         
    {
      {while(k<( 7 )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
      j = 11 + ((unsigned)b & 0x7f);
      {b>>=( 7 );k-=( 7 );} 
      if ((unsigned)i + j > n)
        return 1;
      while (j--)
        ll[i++] = 0;
      l = 0;
    }
  }
  huft_free(tl);
  bb = b;
  bk = k;
  bl = lbits;
  if ((i = huft_build(ll, nl, 257, cplens, cplext, &tl, &bl)) != 0)
  {
    if (i == 1) {
      fprintf((&_iob[2]) , " incomplete literal tree\n");
      huft_free(tl);
    }
    return i;                    
  }
  bd = dbits;
  if ((i = huft_build(ll + nl, nd, 0, cpdist, cpdext, &td, &bd)) != 0)
  {
    if (i == 1) {
      fprintf((&_iob[2]) , " incomplete distance tree\n");
      huft_free(td);
    }
    huft_free(tl);
    return i;                    
  }
  if (inflate_codes(tl, td, bl, bd))
    return 1;
  huft_free(tl);
  huft_free(td);
  return 0;
}
int inflate_block(e)
int *e;                  
{
  unsigned t;            
  register ulg b;        
  register unsigned k;   
  b = bb;
  k = bk;
  {while(k<( 1 )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
  *e = (int)b & 1;
  {b>>=( 1 );k-=( 1 );} 
  {while(k<( 2 )){b|=((ulg)(uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0))  )<<k;k+=8;}} 
  t = (unsigned)b & 3;
  {b>>=( 2 );k-=( 2 );} 
  bb = b;
  bk = k;
  if (t == 2)
    return inflate_dynamic();
  if (t == 0)
    return inflate_stored();
  if (t == 1)
    return inflate_fixed();
  return 2;
}
int inflate()
{
  int e;                 
  int r;                 
  unsigned h;            
  outcnt  = 0;
  bk = 0;
  bb = 0;
  h = 0;
  do {
    hufts = 0;
    if ((r = inflate_block(&e)) != 0)
      return r;
    if (hufts > h)
      h = hufts;
  } while (!e);
  while (bk >= 8) {
    bk -= 8;
    inptr--;
  }
  (outcnt =( outcnt  ),flush_window()) ;
  return 0;
}
extern int maxbits;       
extern int block_mode;    
extern int lzw     (int in, int out)  ;
extern int unlzw   (int in, int out)  ;
static int msg_done = 0;
int lzw(in, out)
    int in, out;
{
    if (msg_done) return 1 ;
    msg_done = 1;
    fprintf((&_iob[2]) ,"output in compress .Z format not supported\n");
    if (in != out) {  
        exit_code = 1 ;
    }
    return 1 ;
}
extern  char    _ctype_[];
static  int   extra_lbits[29 ]  
   = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};
static  int   extra_dbits[30 ]  
   = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};
static  int   extra_blbits[19 ] 
   = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,7};
typedef struct ct_data {
    union {
        ush  freq;        
        ush  code;        
    } fc;
    union {
        ush  dad;         
        ush  len;         
    } dl;
} ct_data;
static  ct_data   dyn_ltree[(2*(256 +1+29 ) +1) ];    
static  ct_data   dyn_dtree[2*30 +1];  
static  ct_data   static_ltree[(256 +1+29 ) +2];
static  ct_data   static_dtree[30 ];
static  ct_data   bl_tree[2*19 +1];
typedef struct tree_desc {
    ct_data   *dyn_tree;       
    ct_data   *static_tree;    
    int       *extra_bits;     
    int     extra_base;           
    int     elems;                
    int     max_length;           
    int     max_code;             
} tree_desc;
static  tree_desc   l_desc =
{dyn_ltree, static_ltree, extra_lbits, 256 +1, (256 +1+29 ) , 15 , 0};
static  tree_desc   d_desc =
{dyn_dtree, static_dtree, extra_dbits, 0,          30 , 15 , 0};
static  tree_desc   bl_desc =
{bl_tree, (ct_data   *)0, extra_blbits, 0,      19 , 7 , 0};
static  ush   bl_count[15 +1];
static  uch   bl_order[19 ]
   = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
static  int   heap[2*(256 +1+29 ) +1];  
static  int heap_len;                
static  int heap_max;                
static  uch   depth[2*(256 +1+29 ) +1];
static  uch length_code[258 - 3 +1];
static  uch dist_code[512];
static  int   base_length[29 ];
static  int   base_dist[30 ];
static  uch   flag_buf[(0x8000 /8)];
static  unsigned last_lit;     
static  unsigned last_dist;    
static  unsigned last_flags;   
static  uch flags;             
static  uch flag_bit;          
static  ulg opt_len;         
static  ulg static_len;      
static  ulg compressed_len;  
static  ulg input_len;       
ush *file_type;         
int *file_method;       
extern long block_start;        
extern unsigned   strstart;  
static  void init_block      (void)  ;
static  void pqdownheap      (ct_data   *tree, int k)  ;
static  void gen_bitlen      (tree_desc   *desc)  ;
static  void gen_codes       (ct_data   *tree, int max_code)  ;
static  void build_tree      (tree_desc   *desc)  ;
static  void scan_tree       (ct_data   *tree, int max_code)  ;
static  void send_tree       (ct_data   *tree, int max_code)  ;
static  int  build_bl_tree   (void)  ;
static  void send_all_trees  (int lcodes, int dcodes, int blcodes)  ;
static  void compress_block  (ct_data   *ltree, ct_data   *dtree)  ;
static  void set_file_type   (void)  ;
void ct_init(attr, methodp)
    ush  *attr;    
    int  *methodp;  
{
    int n;         
    int bits;      
    int length;    
    int code;      
    int dist;      
    file_type = attr;
    file_method = methodp;
    compressed_len = input_len = 0L;
    if (static_dtree[0].dl.len  != 0) return;  
    length = 0;
    for (code = 0; code < 29 -1; code++) {
        base_length[code] = length;
        for (n = 0; n < (1<<extra_lbits[code]); n++) {
            length_code[length++] = (uch)code;
        }
    }
     ;
    length_code[length-1] = (uch)code;
    dist = 0;
    for (code = 0 ; code < 16; code++) {
        base_dist[code] = dist;
        for (n = 0; n < (1<<extra_dbits[code]); n++) {
            dist_code[dist++] = (uch)code;
        }
    }
     ;
    dist >>= 7;  
    for ( ; code < 30 ; code++) {
        base_dist[code] = dist << 7;
        for (n = 0; n < (1<<(extra_dbits[code]-7)); n++) {
            dist_code[256 + dist++] = (uch)code;
        }
    }
     ;
    for (bits = 0; bits <= 15 ; bits++) bl_count[bits] = 0;
    n = 0;
    while (n <= 143) static_ltree[n++].dl.len  = 8, bl_count[8]++;
    while (n <= 255) static_ltree[n++].dl.len  = 9, bl_count[9]++;
    while (n <= 279) static_ltree[n++].dl.len  = 7, bl_count[7]++;
    while (n <= 287) static_ltree[n++].dl.len  = 8, bl_count[8]++;
    gen_codes((ct_data   *)static_ltree, (256 +1+29 ) +1);
    for (n = 0; n < 30 ; n++) {
        static_dtree[n].dl.len  = 5;
        static_dtree[n].fc.code  = bi_reverse(n, 5);
    }
    init_block();
}
static  void init_block()
{
    int n;  
    for (n = 0; n < (256 +1+29 ) ;  n++) dyn_ltree[n].fc.freq  = 0;
    for (n = 0; n < 30 ;  n++) dyn_dtree[n].fc.freq  = 0;
    for (n = 0; n < 19 ; n++) bl_tree[n].fc.freq  = 0;
    dyn_ltree[256 ].fc.freq  = 1;
    opt_len = static_len = 0L;
    last_lit = last_dist = last_flags = 0;
    flags = 0; flag_bit = 1;
}
static  void pqdownheap(tree, k)
    ct_data   *tree;   
    int k;                
{
    int v = heap[k];
    int j = k << 1;   
    while (j <= heap_len) {
        if (j < heap_len && ( tree [  heap[j+1] ].fc.freq  <  tree [  heap[j] ].fc.freq  || ( tree [  heap[j+1] ].fc.freq  ==  tree [  heap[j] ].fc.freq  && depth[  heap[j+1] ] <= depth[  heap[j] ])) ) j++;
        if (( tree [  v ].fc.freq  <  tree [  heap[j] ].fc.freq  || ( tree [  v ].fc.freq  ==  tree [  heap[j] ].fc.freq  && depth[  v ] <= depth[  heap[j] ])) ) break;
        heap[k] = heap[j];  k = j;
        j <<= 1;
    }
    heap[k] = v;
}
static  void gen_bitlen(desc)
    tree_desc   *desc;  
{
    ct_data   *tree  = desc->dyn_tree;
    int   *extra     = desc->extra_bits;
    int base            = desc->extra_base;
    int max_code        = desc->max_code;
    int max_length      = desc->max_length;
    ct_data   *stree = desc->static_tree;
    int h;               
    int n, m;            
    int bits;            
    int xbits;           
    ush f;               
    int overflow = 0;    
    for (bits = 0; bits <= 15 ; bits++) bl_count[bits] = 0;
    tree[heap[heap_max]].dl.len  = 0;  
    for (h = heap_max+1; h < (2*(256 +1+29 ) +1) ; h++) {
        n = heap[h];
        bits = tree[tree[n].dl.dad ].dl.len  + 1;
        if (bits > max_length) bits = max_length, overflow++;
        tree[n].dl.len  = (ush)bits;
        if (n > max_code) continue;  
        bl_count[bits]++;
        xbits = 0;
        if (n >= base) xbits = extra[n-base];
        f = tree[n].fc.freq ;
        opt_len += (ulg)f * (bits + xbits);
        if (stree) static_len += (ulg)f * (stree[n].dl.len  + xbits);
    }
    if (overflow == 0) return;
     ;
    do {
        bits = max_length-1;
        while (bl_count[bits] == 0) bits--;
        bl_count[bits]--;       
        bl_count[bits+1] += 2;  
        bl_count[max_length]--;
        overflow -= 2;
    } while (overflow > 0);
    for (bits = max_length; bits != 0; bits--) {
        n = bl_count[bits];
        while (n != 0) {
            m = heap[--h];
            if (m > max_code) continue;
            if (tree[m].dl.len  != (unsigned) bits) {
                 ;
                opt_len += ((long)bits-(long)tree[m].dl.len )*(long)tree[m].fc.freq ;
                tree[m].dl.len  = (ush)bits;
            }
            n--;
        }
    }
}
static  void gen_codes (tree, max_code)
    ct_data   *tree;         
    int max_code;               
{
    ush next_code[15 +1];  
    ush code = 0;               
    int bits;                   
    int n;                      
    for (bits = 1; bits <= 15 ; bits++) {
        next_code[bits] = code = (code + bl_count[bits-1]) << 1;
    }
     ;
     ;
    for (n = 0;  n <= max_code; n++) {
        int len = tree[n].dl.len ;
        if (len == 0) continue;
        tree[n].fc.code  = bi_reverse(next_code[len]++, len);
         ;
    }
}
static  void build_tree(desc)
    tree_desc   *desc;  
{
    ct_data   *tree   = desc->dyn_tree;
    ct_data   *stree  = desc->static_tree;
    int elems            = desc->elems;
    int n, m;           
    int max_code = -1;  
    int node = elems;   
    heap_len = 0, heap_max = (2*(256 +1+29 ) +1) ;
    for (n = 0; n < elems; n++) {
        if (tree[n].fc.freq  != 0) {
            heap[++heap_len] = max_code = n;
            depth[n] = 0;
        } else {
            tree[n].dl.len  = 0;
        }
    }
    while (heap_len < 2) {
        int new = heap[++heap_len] = (max_code < 2 ? ++max_code : 0);
        tree[new].fc.freq  = 1;
        depth[new] = 0;
        opt_len--; if (stree) static_len -= stree[new].dl.len ;
    }
    desc->max_code = max_code;
    for (n = heap_len/2; n >= 1; n--) pqdownheap(tree, n);
    do {
        {   n  = heap[1 ]; heap[1 ] = heap[heap_len--]; pqdownheap( tree , 1 ); } ;    
        m = heap[1 ];   
        heap[--heap_max] = n;  
        heap[--heap_max] = m;
        tree[node].fc.freq  = tree[n].fc.freq  + tree[m].fc.freq ;
        depth[node] = (uch) (( depth[n]  >=   depth[m]  ?  depth[n]  :   depth[m] )  + 1);
        tree[n].dl.dad  = tree[m].dl.dad  = (ush)node;
        heap[1 ] = node++;
        pqdownheap(tree, 1 );
    } while (heap_len >= 2);
    heap[--heap_max] = heap[1 ];
    gen_bitlen((tree_desc   *)desc);
    gen_codes ((ct_data   *)tree, max_code);
}
static  void scan_tree (tree, max_code)
    ct_data   *tree;  
    int max_code;        
{
    int n;                      
    int prevlen = -1;           
    int curlen;                 
    int nextlen = tree[0].dl.len ;  
    int count = 0;              
    int max_count = 7;          
    int min_count = 4;          
    if (nextlen == 0) max_count = 138, min_count = 3;
    tree[max_code+1].dl.len  = (ush)0xffff;  
    for (n = 0; n <= max_code; n++) {
        curlen = nextlen; nextlen = tree[n+1].dl.len ;
        if (++count < max_count && curlen == nextlen) {
            continue;
        } else if (count < min_count) {
            bl_tree[curlen].fc.freq  += count;
        } else if (curlen != 0) {
            if (curlen != prevlen) bl_tree[curlen].fc.freq ++;
            bl_tree[16 ].fc.freq ++;
        } else if (count <= 10) {
            bl_tree[17 ].fc.freq ++;
        } else {
            bl_tree[18 ].fc.freq ++;
        }
        count = 0; prevlen = curlen;
        if (nextlen == 0) {
            max_count = 138, min_count = 3;
        } else if (curlen == nextlen) {
            max_count = 6, min_count = 3;
        } else {
            max_count = 7, min_count = 4;
        }
    }
}
static  void send_tree (tree, max_code)
    ct_data   *tree;  
    int max_code;        
{
    int n;                      
    int prevlen = -1;           
    int curlen;                 
    int nextlen = tree[0].dl.len ;  
    int count = 0;              
    int max_count = 7;          
    int min_count = 4;          
    if (nextlen == 0) max_count = 138, min_count = 3;
    for (n = 0; n <= max_code; n++) {
        curlen = nextlen; nextlen = tree[n+1].dl.len ;
        if (++count < max_count && curlen == nextlen) {
            continue;
        } else if (count < min_count) {
            do { send_bits(  bl_tree [ curlen ].fc.code ,   bl_tree [ curlen ].dl.len ) ; } while (--count != 0);
        } else if (curlen != 0) {
            if (curlen != prevlen) {
                send_bits(  bl_tree [ curlen ].fc.code ,   bl_tree [ curlen ].dl.len ) ; count--;
            }
             ;
            send_bits(  bl_tree [ 16  ].fc.code ,   bl_tree [ 16  ].dl.len ) ; send_bits(count-3, 2);
        } else if (count <= 10) {
            send_bits(  bl_tree [ 17  ].fc.code ,   bl_tree [ 17  ].dl.len ) ; send_bits(count-3, 3);
        } else {
            send_bits(  bl_tree [ 18  ].fc.code ,   bl_tree [ 18  ].dl.len ) ; send_bits(count-11, 7);
        }
        count = 0; prevlen = curlen;
        if (nextlen == 0) {
            max_count = 138, min_count = 3;
        } else if (curlen == nextlen) {
            max_count = 6, min_count = 3;
        } else {
            max_count = 7, min_count = 4;
        }
    }
}
static  int build_bl_tree()
{
    int max_blindex;   
    scan_tree((ct_data   *)dyn_ltree, l_desc.max_code);
    scan_tree((ct_data   *)dyn_dtree, d_desc.max_code);
    build_tree((tree_desc   *)(&bl_desc));
    for (max_blindex = 19 -1; max_blindex >= 3; max_blindex--) {
        if (bl_tree[bl_order[max_blindex]].dl.len  != 0) break;
    }
    opt_len += 3*(max_blindex+1) + 5+5+4;
     ;
    return max_blindex;
}
static  void send_all_trees(lcodes, dcodes, blcodes)
    int lcodes, dcodes, blcodes;  
{
    int rank;                     
     ;
     ;
     ;
    send_bits(lcodes-257, 5);  
    send_bits(dcodes-1,   5);
    send_bits(blcodes-4,  4);  
    for (rank = 0; rank < blcodes; rank++) {
         ;
        send_bits(bl_tree[bl_order[rank]].dl.len , 3);
    }
     ;
    send_tree((ct_data   *)dyn_ltree, lcodes-1);  
     ;
    send_tree((ct_data   *)dyn_dtree, dcodes-1);  
     ;
}
ulg flush_block(buf, stored_len, eof)
    char *buf;         
    ulg stored_len;    
    int eof;           
{
    ulg opt_lenb, static_lenb;  
    int max_blindex;   
    flag_buf[last_flags] = flags;  
    if (*file_type == (ush)0xffff ) set_file_type();
    build_tree((tree_desc   *)(&l_desc));
     ;
    build_tree((tree_desc   *)(&d_desc));
     ;
    max_blindex = build_bl_tree();
    opt_lenb = (opt_len+3+7)>>3;
    static_lenb = (static_len+3+7)>>3;
    input_len += stored_len;  
     ;
    if (static_lenb <= opt_lenb) opt_lenb = static_lenb;
    if (stored_len <= opt_lenb && eof && compressed_len == 0L && 0 ) {
        if (buf == (char*)0) error ("block vanished");
        copy_block(buf, (unsigned)stored_len, 0);  
        compressed_len = stored_len << 3;
        *file_method = 0 ;
    } else if (stored_len+4 <= opt_lenb && buf != (char*)0) {
        send_bits((0 <<1)+eof, 3);   
        compressed_len = (compressed_len + 3 + 7) & ~7L;
        compressed_len += (stored_len + 4) << 3;
        copy_block(buf, (unsigned)stored_len, 1);  
    } else if (static_lenb == opt_lenb) {
        send_bits((1 <<1)+eof, 3);
        compress_block((ct_data   *)static_ltree, (ct_data   *)static_dtree);
        compressed_len += 3 + static_len;
    } else {
        send_bits((2 <<1)+eof, 3);
        send_all_trees(l_desc.max_code+1, d_desc.max_code+1, max_blindex+1);
        compress_block((ct_data   *)dyn_ltree, (ct_data   *)dyn_dtree);
        compressed_len += 3 + opt_len;
    }
     ;
    init_block();
    if (eof) {
         ;
        bi_windup();
        compressed_len += 7;   
    }
     ;
    return compressed_len >> 3;
}
int ct_tally (dist, lc)
    int dist;   
    int lc;     
{
    inbuf [last_lit++] = (uch)lc;
    if (dist == 0) {
        dyn_ltree[lc].fc.freq ++;
    } else {
        dist--;              
         ;
        dyn_ltree[length_code[lc]+256 +1].fc.freq ++;
        dyn_dtree[(( dist ) < 256 ? dist_code[ dist ] : dist_code[256+(( dist )>>7)]) ].fc.freq ++;
        d_buf[last_dist++] = (ush)dist;
        flags |= flag_bit;
    }
    flag_bit <<= 1;
    if ((last_lit & 7) == 0) {
        flag_buf[last_flags++] = flags;
        flags = 0, flag_bit = 1;
    }
    if (level > 2 && (last_lit & 0xfff) == 0) {
        ulg out_length = (ulg)last_lit*8L;
        ulg in_length = (ulg)strstart-block_start;
        int dcode;
        for (dcode = 0; dcode < 30 ; dcode++) {
            out_length += (ulg)dyn_dtree[dcode].fc.freq *(5L+extra_dbits[dcode]);
        }
        out_length >>= 3;
         ;
        if (last_dist < last_lit/2 && out_length < in_length/2) return 1;
    }
    return (last_lit == 0x8000 -1 || last_dist == 0x8000 );
}
static  void compress_block(ltree, dtree)
    ct_data   *ltree;  
    ct_data   *dtree;  
{
    unsigned dist;       
    int lc;              
    unsigned lx = 0;     
    unsigned dx = 0;     
    unsigned fx = 0;     
    uch flag = 0;        
    unsigned code;       
    int extra;           
    if (last_lit != 0) do {
        if ((lx & 7) == 0) flag = flag_buf[fx++];
        lc = inbuf [lx++];
        if ((flag & 1) == 0) {
            send_bits(  ltree [ lc ].fc.code ,   ltree [ lc ].dl.len ) ;  
             ;
        } else {
            code = length_code[lc];
            send_bits(  ltree [ code+256 +1 ].fc.code ,   ltree [ code+256 +1 ].dl.len ) ;  
            extra = extra_lbits[code];
            if (extra != 0) {
                lc -= base_length[code];
                send_bits(lc, extra);         
            }
            dist = d_buf[dx++];
            code = (( dist ) < 256 ? dist_code[ dist ] : dist_code[256+(( dist )>>7)]) ;
             ;
            send_bits(  dtree [ code ].fc.code ,   dtree [ code ].dl.len ) ;        
            extra = extra_dbits[code];
            if (extra != 0) {
                dist -= base_dist[code];
                send_bits(dist, extra);    
            }
        }  
        flag >>= 1;
    } while (lx < last_lit);
    send_bits(  ltree [ 256  ].fc.code ,   ltree [ 256  ].dl.len ) ;
}
static  void set_file_type()
{
    int n = 0;
    unsigned ascii_freq = 0;
    unsigned bin_freq = 0;
    while (n < 7)        bin_freq += dyn_ltree[n++].fc.freq ;
    while (n < 128)    ascii_freq += dyn_ltree[n++].fc.freq ;
    while (n < 256 ) bin_freq += dyn_ltree[n++].fc.freq ;
    *file_type = bin_freq > (ascii_freq >> 2) ? 0  : 1 ;
    if (*file_type == 0  && 0 ) {
        warn("-l used on binary file", "");
    }
}
extern int maxbits;       
extern int block_mode;    
extern int lzw     (int in, int out)  ;
extern int unlzw   (int in, int out)  ;
static  unsigned  decode   (unsigned count, uch buffer[])  ;
static  void decode_start  (void)  ;
static  void huf_decode_start  (void)  ;
static  unsigned decode_c      (void)  ;
static  unsigned decode_p      (void)  ;
static  void read_pt_len       (int nn, int nbit, int i_special)  ;
static  void read_c_len        (void)  ;
static  void fillbuf       (int n)  ;
static  unsigned getbits   (int n)  ;
static  void init_getbits  (void)  ;
static  void make_table  (int nchar, uch bitlen[],
                          int tablebits, ush table[])  ;
         
static  uch pt_len[(16  + 3)  ];
static  unsigned blocksize;
static  ush pt_table[256];
static  ush       bitbuf;
static  unsigned  subbitbuf;
static  int       bitcount;
static  void fillbuf(n)   
    int n;
{
    bitbuf <<= n;
    while (n > bitcount) {
        bitbuf |= subbitbuf << (n -= bitcount);
        subbitbuf = (unsigned)(inptr < insize ? inbuf[inptr++] : fill_inbuf(1)) ;
        if ((int)subbitbuf == (-1) ) subbitbuf = 0;
        bitcount = 8 ;
    }
    bitbuf |= subbitbuf >> (bitcount -= n);
}
static  unsigned getbits(n)
    int n;
{
    unsigned x;
    x = bitbuf >> ((8  * 2 * sizeof(char))  - n);  fillbuf(n);
    return x;
}
static  void init_getbits()
{
    bitbuf = 0;  subbitbuf = 0;  bitcount = 0;
    fillbuf((8  * 2 * sizeof(char)) );
}
static  void make_table(nchar, bitlen, tablebits, table)
    int nchar;
    uch bitlen[];
    int tablebits;
    ush table[];
{
    ush count[17], weight[17], start[18], *p;
    unsigned i, k, len, ch, jutbits, avail, nextcode, mask;
    for (i = 1; i <= 16; i++) count[i] = 0;
    for (i = 0; i < (unsigned)nchar; i++) count[bitlen[i]]++;
    start[1] = 0;
    for (i = 1; i <= 16; i++)
        start[i + 1] = start[i] + (count[i] << (16 - i));
    if ((start[17] & 0xffff) != 0)
        error("Bad table\n");
    jutbits = 16 - tablebits;
    for (i = 1; i <= (unsigned)tablebits; i++) {
        start[i] >>= jutbits;
        weight[i] = (unsigned) 1 << (tablebits - i);
    }
    while (i <= 16) {
        weight[i] = (unsigned) 1 << (16 - i);
        i++;
    }
    i = start[tablebits + 1] >> jutbits;
    if (i != 0) {
        k = 1 << tablebits;
        while (i != k) table[i++] = 0;
    }
    avail = nchar;
    mask = (unsigned) 1 << (15 - tablebits);
    for (ch = 0; ch < (unsigned)nchar; ch++) {
        if ((len = bitlen[ch]) == 0) continue;
        nextcode = start[len] + weight[len];
        if (len <= (unsigned)tablebits) {
            for (i = start[len]; i < nextcode; i++) table[i] = ch;
        } else {
            k = start[len];
            p = &table[k >> jutbits];
            i = len - tablebits;
            while (i != 0) {
                if (*p == 0) {
                    (prev+0x8000 )  [avail] = prev [avail] = 0;
                    *p = avail++;
                }
                if (k & mask) p = &(prev+0x8000 )  [*p];
                else          p = &prev [*p];
                k <<= 1;  i--;
            }
            *p = ch;
        }
        start[len] = nextcode;
    }
}
static  void read_pt_len(nn, nbit, i_special)
    int nn;
    int nbit;
    int i_special;
{
    int i, c, n;
    unsigned mask;
    n = getbits(nbit);
    if (n == 0) {
        c = getbits(nbit);
        for (i = 0; i < nn; i++) pt_len[i] = 0;
        for (i = 0; i < 256; i++) pt_table[i] = c;
    } else {
        i = 0;
        while (i < n) {
            c = bitbuf >> ((8  * 2 * sizeof(char))  - 3);
            if (c == 7) {
                mask = (unsigned) 1 << ((8  * 2 * sizeof(char))  - 1 - 3);
                while (mask & bitbuf) {  mask >>= 1;  c++;  }
            }
            fillbuf((c < 7) ? 3 : c - 3);
            pt_len[i++] = c;
            if (i == i_special) {
                c = getbits(2);
                while (--c >= 0) pt_len[i++] = 0;
            }
        }
        while (i < nn) pt_len[i++] = 0;
        make_table(nn, pt_len, 8, pt_table);
    }
}
static  void read_c_len()
{
    int i, c, n;
    unsigned mask;
    n = getbits(9 );
    if (n == 0) {
        c = getbits(9 );
        for (i = 0; i < (255  + 256  + 2 - 3 ) ; i++) outbuf [i] = 0;
        for (i = 0; i < 4096; i++) d_buf [i] = c;
    } else {
        i = 0;
        while (i < n) {
            c = pt_table[bitbuf >> ((8  * 2 * sizeof(char))  - 8)];
            if (c >= (16  + 3) ) {
                mask = (unsigned) 1 << ((8  * 2 * sizeof(char))  - 1 - 8);
                do {
                    if (bitbuf & mask) c = (prev+0x8000 )  [c];
                    else               c = prev  [c];
                    mask >>= 1;
                } while (c >= (16  + 3) );
            }
            fillbuf((int) pt_len[c]);
            if (c <= 2) {
                if      (c == 0) c = 1;
                else if (c == 1) c = getbits(4) + 3;
                else             c = getbits(9 ) + 20;
                while (--c >= 0) outbuf [i++] = 0;
            } else outbuf [i++] = c - 2;
        }
        while (i < (255  + 256  + 2 - 3 ) ) outbuf [i++] = 0;
        make_table((255  + 256  + 2 - 3 ) , outbuf , 12, d_buf );
    }
}
static  unsigned decode_c()
{
    unsigned j, mask;
    if (blocksize == 0) {
        blocksize = getbits(16);
        if (blocksize == 0) {
            return (255  + 256  + 2 - 3 ) ;  
        }
        read_pt_len((16  + 3) , 5 , 3);
        read_c_len();
        read_pt_len((13  + 1) , 4 , -1);
    }
    blocksize--;
    j = d_buf [bitbuf >> ((8  * 2 * sizeof(char))  - 12)];
    if (j >= (255  + 256  + 2 - 3 ) ) {
        mask = (unsigned) 1 << ((8  * 2 * sizeof(char))  - 1 - 12);
        do {
            if (bitbuf & mask) j = (prev+0x8000 )  [j];
            else               j = prev  [j];
            mask >>= 1;
        } while (j >= (255  + 256  + 2 - 3 ) );
    }
    fillbuf((int) outbuf [j]);
    return j;
}
static  unsigned decode_p()
{
    unsigned j, mask;
    j = pt_table[bitbuf >> ((8  * 2 * sizeof(char))  - 8)];
    if (j >= (13  + 1) ) {
        mask = (unsigned) 1 << ((8  * 2 * sizeof(char))  - 1 - 8);
        do {
            if (bitbuf & mask) j = (prev+0x8000 )  [j];
            else               j = prev  [j];
            mask >>= 1;
        } while (j >= (13  + 1) );
    }
    fillbuf((int) pt_len[j]);
    if (j != 0) j = ((unsigned) 1 << (j - 1)) + getbits((int) (j - 1));
    return j;
}
static  void huf_decode_start()
{
    init_getbits();  blocksize = 0;
}
static  int j;     
static  int done;  
static  void decode_start()
{
    huf_decode_start();
    j = 0;
    done = 0;
}
static  unsigned decode(count, buffer)
    unsigned count;
    uch buffer[];
{
    static  unsigned i;
    unsigned r, c;
    r = 0;
    while (--j >= 0) {
        buffer[r] = buffer[i];
        i = (i + 1) & (((unsigned) 1 << 13 )  - 1);
        if (++r == count) return r;
    }
    for ( ; ; ) {
        c = decode_c();
        if (c == (255  + 256  + 2 - 3 ) ) {
            done = 1;
            return r;
        }
        if (c <= 255 ) {
            buffer[r] = c;
            if (++r == count) return r;
        } else {
            j = c - (255  + 1 - 3 );
            i = (r - decode_p() - 1) & (((unsigned) 1 << 13 )  - 1);
            while (--j >= 0) {
                buffer[r] = buffer[i];
                i = (i + 1) & (((unsigned) 1 << 13 )  - 1);
                if (++r == count) return r;
            }
        }
    }
}
int unlzh(in, out)
    int in;
    int out;
{
    unsigned n;
    ifd = in;
    ofd = out;
    decode_start();
    while (!done) {
        n = decode((unsigned) ((unsigned) 1 << 13 ) , window);
        if (!test && n > 0) {
            write_buf(out, (char*)window, n);
        }
    }
    return 0 ;
}
extern int maxbits;       
extern int block_mode;    
extern int lzw     (int in, int out)  ;
extern int unlzw   (int in, int out)  ;
typedef unsigned char   char_type;
typedef          long   code_int;
typedef unsigned long   count_int;
typedef unsigned short  count_short;
typedef unsigned long   cmp_code_int;
        
union   bytes {
    long  word;
    struct {
        int  dummy;
    } bytes;
};
int block_mode = 0x80 ;  
int unlzw(in, out) 
    int in, out;     
{
    register    char_type  *stackp;
        code_int   code;
        int        finchar;
        code_int   oldcode;
        code_int   incode;
        long       inbits;
        long       posbits;
        int        outpos;
       unsigned   bitmask;
       code_int   free_ent;
       code_int   maxcode;
       code_int   maxmaxcode;
       int        n_bits;
       int        rsize;
    maxbits = (inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;
    block_mode = maxbits & 0x80 ;
    if ((maxbits & 0x60 ) != 0) {
        {if (!quiet) fprintf  ((&_iob[2]) , "\n%s: %s: warning, unknown flags 0x%x\n",
              progname, ifname, maxbits & 0x60 )  ; if (exit_code == 0 ) exit_code = 2 ;} ;
    }
    maxbits &= 0x1f ;
    maxmaxcode = (1L << ( maxbits )) ;
    if (maxbits > 16 ) {
        fprintf((&_iob[2]) ,
                "\n%s: %s: compressed with %d bits, can only handle %d bits\n",
                progname, ifname, maxbits, 16 );
        exit_code = 1 ;
        return 1 ;
    }
    rsize = insize;
    maxcode = (1L << ( n_bits = 9  )) -1;
    bitmask = (1<<n_bits)-1;
    oldcode = -1;
    finchar = 0;
    outpos = 0;
    posbits = inptr<<3;
    free_ent = ((block_mode) ? (256 +1)  : 256);
    memset ((voidp)( prev  ), 0, (  256 )) ; ;  
    for (code = 255 ; code >= 0 ; --code) {
        window [ code ]  = (char_type)code;
    }
    do {
        register  int i;
        int  e;
        int  o;
        
    resetbuf:
        e = insize-(o = (posbits>>3));
        
        for (i = 0 ; i < e ; ++i) {
            inbuf[i] = inbuf[i+o];
        }
        insize = e;
        posbits = 0;
        
        if (insize < 64 ) {
            if ((rsize = read(in, (char*)inbuf+insize, 0x8000 )) == (-1) ) {
                read_error();
            }
            insize += rsize;
            bytes_in += (ulg)rsize;
        }
        inbits = ((rsize != 0) ? ((long)insize - insize%n_bits)<<3 : 
                  ((long)insize<<3)-(n_bits-1));
        
        while (inbits > posbits) {
            if (free_ent > maxcode) {
                posbits = ((posbits-1) +
                           ((n_bits<<3)-(posbits-1+(n_bits<<3))%(n_bits<<3)));
                ++n_bits;
                if (n_bits == maxbits) {
                    maxcode = maxmaxcode;
                } else {
                    maxcode = (1L << ( n_bits )) -1;
                }
                bitmask = (1<<n_bits)-1;
                goto resetbuf;
            }
            { register  char_type *p = &( inbuf )[( posbits )>>3]; ( code ) = ((((long)(p[0]))|((long)(p[1])<<8)| ((long)(p[2])<<16))>>(( posbits )&0x7))&( bitmask ); ( posbits ) += ( n_bits ); } ;
             ;
            if (oldcode == -1) {
                if (code >= 256) error("corrupt input.");
                outbuf[outpos++] = (char_type)(finchar = (int)(oldcode=code));
                continue;
            }
            if (code == 256  && block_mode) {
                memset ((voidp)( prev  ), 0, (  256 )) ; ;
                free_ent = (256 +1)  - 1;
                posbits = ((posbits-1) +
                           ((n_bits<<3)-(posbits-1+(n_bits<<3))%(n_bits<<3)));
                maxcode = (1L << ( n_bits = 9  )) -1;
                bitmask = (1<<n_bits)-1;
                goto resetbuf;
            }
            incode = code;
            stackp = ((char_type *)(&d_buf[0x8000 -1])) ;
            
            if (code >= free_ent) {  
                if (code > free_ent) {
                    if (!test && outpos > 0) {
                        write_buf(out, (char*)outbuf, outpos);
                        bytes_out += (ulg)outpos;
                    }
                    error(to_stdout ? "corrupt input." :
                          "corrupt input. Use zcat to recover some data.");
                }
                *--stackp = (char_type)finchar;
                code = oldcode;
            }
            while ((cmp_code_int)code >= (cmp_code_int)256) {
                 
                *--stackp = window [ code ] ;
                code = prev [ code ] ;
            }
            *--stackp = (char_type)(finchar = window [ code ] );
            
             
            {
                register  int   i;
            
                if (outpos+(i = (((char_type *)(&d_buf[0x8000 -1])) -stackp)) >= 16384 ) {
                    do {
                        if (i > 16384 -outpos) i = 16384 -outpos;
                        if (i > 0) {
                            memcpy(outbuf+outpos, stackp, i);
                            outpos += i;
                        }
                        if (outpos >= 16384 ) {
                            if (!test) {
                                write_buf(out, (char*)outbuf, outpos);
                                bytes_out += (ulg)outpos;
                            }
                            outpos = 0;
                        }
                        stackp+= i;
                    } while ((i = (((char_type *)(&d_buf[0x8000 -1])) -stackp)) > 0);
                } else {
                    memcpy(outbuf+outpos, stackp, i);
                    outpos += i;
                }
            }
            if ((code = free_ent) < maxmaxcode) {  
                prev [ code ]  = (unsigned short)oldcode;
                window [ code ]  = (char_type)finchar;
                free_ent = code+1;
            } 
            oldcode = incode;    
        }
    } while (rsize != 0);
    if (!test && outpos > 0) {
        write_buf(out, (char*)outbuf, outpos);
        bytes_out += (ulg)outpos;
    }
    return 0 ;
}
static  ulg orig_len;        
static  int max_len;         
static  uch literal[256 ];
static  int lit_base[25 +1];
static  int leaves [25 +1];  
static  int parents[25 +1];  
static  int peek_bits;  
static  int valid;                   
static  void read_tree   (void)  ;
static  void build_tree1  (void)  ;
static  void read_tree()
{
    int len;   
    int base;  
    int n;
    orig_len = 0;
    for (n = 1; n <= 4; n++) orig_len = (orig_len << 8) | (ulg)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;
    max_len = (int)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;  
    if (max_len > 25 ) {
        error("invalid compressed data -- Huffman code > 32 bits");
    }
    n = 0;
    for (len = 1; len <= max_len; len++) {
        leaves[len] = (int)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;
        n += leaves[len];
    }
    if (n > 256 ) {
        error("too many leaves in Huffman tree");
    }
     ;
    leaves[max_len]++;
    base = 0;
    for (len = 1; len <= max_len; len++) {
         
        lit_base[len] = base;
         
        for (n = leaves[len]; n > 0; n--) {
            literal[base++] = (uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;
        }
    }
    leaves[max_len]++;  
}
static  void build_tree1()
{
    int nodes = 0;  
    int len;        
    uch *prefixp;   
    for (len = max_len; len >= 1; len--) {
         
        nodes >>= 1;
        parents[len] = nodes;
         
        lit_base[len] -= nodes;
         
        nodes += leaves[len];
    }
    peek_bits = (( max_len ) <= (  12  ) ? ( max_len ) : (  12  )) ;
    prefixp = &outbuf [1<<peek_bits];
    for (len = 1; len <= peek_bits; len++) {
        int prefixes = leaves[len] << (peek_bits-len);  
        while (prefixes--) *--prefixp = (uch)len;
    }
    while (prefixp > outbuf ) *--prefixp = 0;
}
int unpack(in, out)
    int in, out;             
{
    int len;                 
    unsigned eob;            
    register unsigned peek;  
    unsigned peek_mask;      
    ifd = in;
    ofd = out;
    read_tree();      
    build_tree1();     
    (valid = 0, bitbuf = 0) ;   
    peek_mask = (1<<peek_bits)-1;
    eob = leaves[max_len]-1;
     ;
    for (;;) {
         
        { while (valid < (  peek_bits )) bitbuf = (bitbuf<<8) | (ulg)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) , valid += 8;  peek  = (bitbuf >> (valid-(  peek_bits ))) & (  peek_mask ); } ;
        len = outbuf [peek];
        if (len > 0) {
            peek >>= peek_bits - len;  
        } else {
             
            ulg mask = peek_mask;
            len = peek_bits;
            do {
                len++, mask = (mask<<1)+1;
                { while (valid < (  len )) bitbuf = (bitbuf<<8) | (ulg)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) , valid += 8;  peek  = (bitbuf >> (valid-(  len ))) & (  mask ); } ;
            } while (peek < (unsigned)parents[len]);
             
        }
         
        if (peek == eob && len == max_len) break;  
        {window[outcnt++]=(uch)( literal[peek+lit_base[len]] ); if (outcnt==0x8000 ) flush_window();} ;
         ;
        (valid -= ( len )) ;
    }  
    flush_window();
     ;
    if (orig_len != (ulg)bytes_out) {
        error("invalid compressed data--length error");
    }
    return 0 ;
}
int decrypt;         
char *key;           
int pkzip = 0;       
int ext_header = 0;  
int check_zipfile(in)
    int in;    
{
    uch *h = inbuf + inptr;  
    ifd = in;
    inptr += 30  + ((ush)(uch)(( h + 26  )[0]) | ((ush)(uch)(( h + 26  )[1]) << 8))  + ((ush)(uch)(( h + 28  )[0]) | ((ush)(uch)(( h + 28  )[1]) << 8)) ;
    if (inptr > insize || ((ulg)(((ush)(uch)((  h  )[0]) | ((ush)(uch)((  h  )[1]) << 8)) ) | ((ulg)(((ush)(uch)(( ( h )+2 )[0]) | ((ush)(uch)(( ( h )+2 )[1]) << 8)) ) << 16))  != 0x04034b50L ) {
        fprintf((&_iob[2]) , "\n%s: %s: not a valid zip file\n",
                progname, ifname);
        exit_code = 1 ;
        return 1 ;
    }
    method = h[8 ];
    if (method != 0  && method != 8 ) {
        fprintf((&_iob[2]) ,
                "\n%s: %s: first entry not deflated or stored -- use unzip\n",
                progname, ifname);
        exit_code = 1 ;
        return 1 ;
    }
    if ((decrypt = h[6 ] & 1 ) != 0) {
        fprintf((&_iob[2]) , "\n%s: %s: encrypted file -- use unzip\n",
                progname, ifname);
        exit_code = 1 ;
        return 1 ;
    }
    ext_header = (h[6 ] & 8 ) != 0;
    pkzip = 1;
    return 0 ;
}
int unzip(in, out)
    int in, out;    
{
    ulg orig_crc = 0;        
    ulg orig_len = 0;        
    int n;
    uch buf[16 ];         
    ifd = in;
    ofd = out;
    updcrc(0 , 0);            
    if (pkzip && !ext_header) {   
        orig_crc = ((ulg)(((ush)(uch)((  inbuf + 14   )[0]) | ((ush)(uch)((  inbuf + 14   )[1]) << 8)) ) | ((ulg)(((ush)(uch)(( ( inbuf + 14  )+2 )[0]) | ((ush)(uch)(( ( inbuf + 14  )+2 )[1]) << 8)) ) << 16)) ;
        orig_len = ((ulg)(((ush)(uch)((  inbuf + 22   )[0]) | ((ush)(uch)((  inbuf + 22   )[1]) << 8)) ) | ((ulg)(((ush)(uch)(( ( inbuf + 22  )+2 )[0]) | ((ush)(uch)(( ( inbuf + 22  )+2 )[1]) << 8)) ) << 16)) ;
    }
    if (method == 8 )  {
        int res = inflate();
        if (res == 3) {
            error("out of memory");
        } else if (res != 0) {
            error("invalid compressed data--format violated");
        }
    } else if (pkzip && method == 0 ) {
        register ulg n = ((ulg)(((ush)(uch)((  inbuf + 22   )[0]) | ((ush)(uch)((  inbuf + 22   )[1]) << 8)) ) | ((ulg)(((ush)(uch)(( ( inbuf + 22  )+2 )[0]) | ((ush)(uch)(( ( inbuf + 22  )+2 )[1]) << 8)) ) << 16)) ;
        if (n != ((ulg)(((ush)(uch)((  inbuf + 18   )[0]) | ((ush)(uch)((  inbuf + 18   )[1]) << 8)) ) | ((ulg)(((ush)(uch)(( ( inbuf + 18  )+2 )[0]) | ((ush)(uch)(( ( inbuf + 18  )+2 )[1]) << 8)) ) << 16))  - (decrypt ? 12  : 0)) {
            fprintf((&_iob[2]) , "len %ld, siz %ld\n", n, ((ulg)(((ush)(uch)((  inbuf + 18   )[0]) | ((ush)(uch)((  inbuf + 18   )[1]) << 8)) ) | ((ulg)(((ush)(uch)(( ( inbuf + 18  )+2 )[0]) | ((ush)(uch)(( ( inbuf + 18  )+2 )[1]) << 8)) ) << 16)) );
            error("invalid compressed data--length mismatch");
        }
        while (n--) {
            uch c = (uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;
            {window[outcnt++]=(uch)( c ); if (outcnt==0x8000 ) flush_window();} ;
        }
        flush_window();
    } else {
        error("internal error, invalid method");
    }
    if (!pkzip) {
        for (n = 0; n < 8; n++) {
            buf[n] = (uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;  
        }
        orig_crc = ((ulg)(((ush)(uch)((  buf  )[0]) | ((ush)(uch)((  buf  )[1]) << 8)) ) | ((ulg)(((ush)(uch)(( ( buf )+2 )[0]) | ((ush)(uch)(( ( buf )+2 )[1]) << 8)) ) << 16)) ;
        orig_len = ((ulg)(((ush)(uch)((  buf+4  )[0]) | ((ush)(uch)((  buf+4  )[1]) << 8)) ) | ((ulg)(((ush)(uch)(( ( buf+4 )+2 )[0]) | ((ush)(uch)(( ( buf+4 )+2 )[1]) << 8)) ) << 16)) ;
    } else if (ext_header) {   
         
        for (n = 0; n < 16 ; n++) {
            buf[n] = (uch)(inptr < insize ? inbuf[inptr++] : fill_inbuf(0)) ;  
        }
        orig_crc = ((ulg)(((ush)(uch)((  buf+4  )[0]) | ((ush)(uch)((  buf+4  )[1]) << 8)) ) | ((ulg)(((ush)(uch)(( ( buf+4 )+2 )[0]) | ((ush)(uch)(( ( buf+4 )+2 )[1]) << 8)) ) << 16)) ;
        orig_len = ((ulg)(((ush)(uch)((  buf+12  )[0]) | ((ush)(uch)((  buf+12  )[1]) << 8)) ) | ((ulg)(((ush)(uch)(( ( buf+12 )+2 )[0]) | ((ush)(uch)(( ( buf+12 )+2 )[1]) << 8)) ) << 16)) ;
    }
    if (orig_crc != updcrc(outbuf, 0)) {
        error("invalid compressed data--crc error");
    }
    if (orig_len != (ulg)bytes_out) {
        error("invalid compressed data--length error");
    }
    if (pkzip && inptr + 4 < insize && ((ulg)(((ush)(uch)((  inbuf+inptr  )[0]) | ((ush)(uch)((  inbuf+inptr  )[1]) << 8)) ) | ((ulg)(((ush)(uch)(( ( inbuf+inptr )+2 )[0]) | ((ush)(uch)(( ( inbuf+inptr )+2 )[1]) << 8)) ) << 16))  == 0x04034b50L ) {
        if (to_stdout) {
            {if (!quiet) fprintf  ((&_iob[2]) ,
                  "%s: %s has more than one entry--rest ignored\n",
                  progname, ifname)  ; if (exit_code == 0 ) exit_code = 2 ;} ;
        } else {
             
            fprintf((&_iob[2]) ,
                    "%s: %s has more than one entry -- unchanged\n",
                    progname, ifname);
            exit_code = 1 ;
            ext_header = pkzip = 0;
            return 1 ;
        }
    }
    ext_header = pkzip = 0;  
    return 0 ;
}
extern  char    _ctype_[];
extern ulg crc_32_tab[];    
int copy(in, out)
    int in, out;    
{
    errno = 0;
    while (insize != 0 && (int)insize != (-1) ) {
        write_buf(out, (char*)inbuf, insize);
        bytes_out += insize;
        insize = read(in, (char*)inbuf, 0x8000 );
    }
    if ((int)insize == (-1)  && errno != 0) {
        read_error();
    }
    bytes_in = bytes_out;
    return 0 ;
}
ulg updcrc(s, n)
    uch *s;                  
    unsigned n;              
{
    register ulg c;          
    static ulg crc = (ulg)0xffffffffL;  
    if (s == 0 ) {
        c = 0xffffffffL;
    } else {
        c = crc;
        if (n) do {
            c = crc_32_tab[((int)c ^ (*s++)) & 0xff] ^ (c >> 8);
        } while (--n);
    }
    crc = c;
    return c ^ 0xffffffffL;        
}
void clear_bufs()
{
    outcnt = 0;
    insize = inptr = 0;
    bytes_in = bytes_out = 0L;
}
int fill_inbuf(eof_ok)
    int eof_ok;           
{
    int len;
    insize = 0;
    errno = 0;
    do {
        len = read(ifd, (char*)inbuf+insize, 0x8000 -insize);
        if (len == 0 || len == (-1) ) break;
        insize += len;
    } while (insize < 0x8000 );
    if (insize == 0) {
        if (eof_ok) return (-1) ;
        read_error();
    }
    bytes_in += (ulg)insize;
    inptr = 1;
    return inbuf[0];
}
void flush_outbuf()
{
    if (outcnt == 0) return;
    write_buf(ofd, (char *)outbuf, outcnt);
    bytes_out += (ulg)outcnt;
    outcnt = 0;
}
void flush_window()
{
    if (outcnt == 0) return;
    updcrc(window, outcnt);
    if (!test) {
        write_buf(ofd, (char *)window, outcnt);
    }
    bytes_out += (ulg)outcnt;
    outcnt = 0;
}
void write_buf(fd, buf, cnt)
    int       fd;
    voidp     buf;
    unsigned  cnt;
{
    unsigned  n;
    while ((n = write(fd, buf, cnt)) != cnt) {
        if (n == (unsigned)(-1)) {
            write_error();
        }
        cnt -= n;
        buf = (voidp)((char*)buf+n);
    }
}
char *strlwr(s)
    char *s;
{
    char *t;
    for (t = s; *t; t++) *t = (((_ctype_+1)[  *t  ]&01 )  ? ( *t )-'A'+'a' : ( *t )) ;
    return s;
}
char *basename(fname)
    char *fname;
{
    char *p;
    if ((p = strrchr(fname, '/' ))  != 0 ) fname = p+1;
    if (( 'A' )  == 'a') strlwr(fname);
    return fname;
}
void make_simple_name(name)
    char *name;
{
    char *p = strrchr(name, '.');
    if (p == 0 ) return;
    if (p == name) p++;
    do {
        if (*--p == '.') *p = '_';
    } while (p != name);
}
char *add_envopt(argcp, argvp, env)
    int *argcp;           
    char ***argvp;        
    char *env;            
{
    char *p;              
    char **oargv;         
    char **nargv;         
    int  oargc = *argcp;  
    int  nargc = 0;       
    env = (char*)getenv(env);
    if (env == 0 ) return 0 ;
    p = (char*)xmalloc(strlen(env)+1);
    env = strcpy(p, env);                     
    for (p = env; *p; nargc++ ) {             
        p += strspn(p, " \t" );       
        if (*p == '\0') break;
        p += strcspn(p, " \t" );              
        if (*p) *p++ = '\0';                  
    }
    if (nargc == 0) {
        free(env);
        return 0 ;
    }
    *argcp += nargc;
    nargv = (char**)calloc(*argcp+1, sizeof(char *));
    if (nargv == 0 ) error("out of memory");
    oargv  = *argvp;
    *argvp = nargv;
    if (oargc-- < 0) error("argc<=0");
    *(nargv++) = *(oargv++);
    for (p = env; nargc > 0; nargc--) {
        p += strspn(p, " \t" );       
        *(nargv++) = p;                       
        while (*p++) ;                        
    }
    while (oargc--) *(nargv++) = *(oargv++);
    *nargv = 0 ;
    return env;
}
void error(m)
    char *m;
{
    fprintf((&_iob[2]) , "\n%s: %s: %s\n", progname, ifname, m);
    abort_gzip();
}
void warn(a, b)
    char *a, *b;             
{
    {if (!quiet) fprintf  ((&_iob[2]) , "%s: %s: warning: %s%s\n", progname, ifname, a, b)  ; if (exit_code == 0 ) exit_code = 2 ;} ;
}
void read_error()
{
    fprintf((&_iob[2]) , "\n%s: ", progname);
    if (errno != 0) {
        perror(ifname);
    } else {
        fprintf((&_iob[2]) , "%s: unexpected end of file\n", ifname);
    }
    abort_gzip();
}
void write_error()
{
    fprintf((&_iob[2]) , "\n%s: ", progname);
    perror(ofname);
    abort_gzip();
}
void display_ratio(num, den, file)
    long num;
    long den;
    struct _iobuf  *file;
{
    long ratio;   
    if (den == 0) {
        ratio = 0;  
    } else if (den < 2147483L) {  
        ratio = 1000L*num/den;
    } else {
        ratio = num/(den/1000L);
    }
    if (ratio < 0) {
        (--(  file )->_cnt >= 0 ?       (int)(*(  file )->_ptr++ = (unsigned char)( '-' )) :    (((  file )->_flag & 0200 ) && -(  file )->_cnt < (  file )->_bufsiz ?  ((*(  file )->_ptr = (unsigned char)( '-' )) != '\n' ?  (int)(*(  file )->_ptr++) :     _flsbuf(*(unsigned char *)(  file )->_ptr,   file )) :  _flsbuf((unsigned char)( '-' ),   file ))) ;
        ratio = -ratio;
    } else {
        (--(  file )->_cnt >= 0 ?       (int)(*(  file )->_ptr++ = (unsigned char)( ' ' )) :    (((  file )->_flag & 0200 ) && -(  file )->_cnt < (  file )->_bufsiz ?  ((*(  file )->_ptr = (unsigned char)( ' ' )) != '\n' ?  (int)(*(  file )->_ptr++) :     _flsbuf(*(unsigned char *)(  file )->_ptr,   file )) :  _flsbuf((unsigned char)( ' ' ),   file ))) ;
    }
    fprintf(file, "%2ld.%1ld%%", ratio / 10L, ratio % 10L);
}
voidp xmalloc (size)
    unsigned size;
{
    voidp cp = (voidp)malloc (size);
    if (cp == 0 ) error("out of memory");
    return cp;
}
ulg crc_32_tab[] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};
extern  char    _ctype_[];
static  ulg crc;        
long header_bytes;    
int zip(in, out)
    int in, out;             
{
    uch  flags = 0;          
    ush  attr = 0;           
    ush  deflate_flags = 0;  
    ifd = in;
    ofd = out;
    outcnt = 0;
    method = 8 ;
    {outbuf[outcnt++]=(uch)( "\037\213" [0] ); if (outcnt==16384 ) flush_outbuf();} ;  
    {outbuf[outcnt++]=(uch)( "\037\213" [1] ); if (outcnt==16384 ) flush_outbuf();} ;
    {outbuf[outcnt++]=(uch)( 8  ); if (outcnt==16384 ) flush_outbuf();} ;       
    if (save_orig_name) {
        flags |= 0x08 ;
    }
    {outbuf[outcnt++]=(uch)( flags ); if (outcnt==16384 ) flush_outbuf();} ;          
    { { if (outcnt < 16384 -2) { outbuf[outcnt++] = (uch) (( ( time_stamp ) & 0xffff ) & 0xff); outbuf[outcnt++] = (uch) ((ush)( ( time_stamp ) & 0xffff ) >> 8); } else { {outbuf[outcnt++]=(uch)( (uch)(( ( time_stamp ) & 0xffff ) & 0xff) ); if (outcnt==16384 ) flush_outbuf();} ; {outbuf[outcnt++]=(uch)( (uch)((ush)( ( time_stamp ) & 0xffff ) >> 8) ); if (outcnt==16384 ) flush_outbuf();} ; } } ; { if (outcnt < 16384 -2) { outbuf[outcnt++] = (uch) (( ((ulg)( time_stamp )) >> 16 ) & 0xff); outbuf[outcnt++] = (uch) ((ush)( ((ulg)( time_stamp )) >> 16 ) >> 8); } else { {outbuf[outcnt++]=(uch)( (uch)(( ((ulg)( time_stamp )) >> 16 ) & 0xff) ); if (outcnt==16384 ) flush_outbuf();} ; {outbuf[outcnt++]=(uch)( (uch)((ush)( ((ulg)( time_stamp )) >> 16 ) >> 8) ); if (outcnt==16384 ) flush_outbuf();} ; } } ; } ;
    crc = updcrc(0, 0);
    bi_init(out);
    ct_init(&attr, &method);
    lm_init(level, &deflate_flags);
    {outbuf[outcnt++]=(uch)( (uch)deflate_flags ); if (outcnt==16384 ) flush_outbuf();} ;  
    {outbuf[outcnt++]=(uch)( 0x03  ); if (outcnt==16384 ) flush_outbuf();} ;             
    if (save_orig_name) {
        char *p = basename(ifname);  
        do {
            {outbuf[outcnt++]=(uch)(  *p  ); if (outcnt==16384 ) flush_outbuf();}  ;
        } while (*p++);
    }
    header_bytes = (long)outcnt;
    (void)deflate();
    { { if (outcnt < 16384 -2) { outbuf[outcnt++] = (uch) (( ( crc ) & 0xffff ) & 0xff); outbuf[outcnt++] = (uch) ((ush)( ( crc ) & 0xffff ) >> 8); } else { {outbuf[outcnt++]=(uch)( (uch)(( ( crc ) & 0xffff ) & 0xff) ); if (outcnt==16384 ) flush_outbuf();} ; {outbuf[outcnt++]=(uch)( (uch)((ush)( ( crc ) & 0xffff ) >> 8) ); if (outcnt==16384 ) flush_outbuf();} ; } } ; { if (outcnt < 16384 -2) { outbuf[outcnt++] = (uch) (( ((ulg)( crc )) >> 16 ) & 0xff); outbuf[outcnt++] = (uch) ((ush)( ((ulg)( crc )) >> 16 ) >> 8); } else { {outbuf[outcnt++]=(uch)( (uch)(( ((ulg)( crc )) >> 16 ) & 0xff) ); if (outcnt==16384 ) flush_outbuf();} ; {outbuf[outcnt++]=(uch)( (uch)((ush)( ((ulg)( crc )) >> 16 ) >> 8) ); if (outcnt==16384 ) flush_outbuf();} ; } } ; } ;
    { { if (outcnt < 16384 -2) { outbuf[outcnt++] = (uch) (( ( bytes_in  ) & 0xffff ) & 0xff); outbuf[outcnt++] = (uch) ((ush)( ( bytes_in  ) & 0xffff ) >> 8); } else { {outbuf[outcnt++]=(uch)( (uch)(( ( bytes_in  ) & 0xffff ) & 0xff) ); if (outcnt==16384 ) flush_outbuf();} ; {outbuf[outcnt++]=(uch)( (uch)((ush)( ( bytes_in  ) & 0xffff ) >> 8) ); if (outcnt==16384 ) flush_outbuf();} ; } } ; { if (outcnt < 16384 -2) { outbuf[outcnt++] = (uch) (( ((ulg)( bytes_in  )) >> 16 ) & 0xff); outbuf[outcnt++] = (uch) ((ush)( ((ulg)( bytes_in  )) >> 16 ) >> 8); } else { {outbuf[outcnt++]=(uch)( (uch)(( ((ulg)( bytes_in  )) >> 16 ) & 0xff) ); if (outcnt==16384 ) flush_outbuf();} ; {outbuf[outcnt++]=(uch)( (uch)((ush)( ((ulg)( bytes_in  )) >> 16 ) >> 8) ); if (outcnt==16384 ) flush_outbuf();} ; } } ; } ;
    header_bytes += 2*sizeof(long);
    flush_outbuf();
    return 0 ;
}
int file_read(buf, size)
    char *buf;
    unsigned size;
{
    unsigned len;
    len = read(ifd, buf, size);
    if (len == (unsigned)(-1) || len == 0) return (int)len;
    crc = updcrc((uch*)buf, len);
    bytes_in  += (ulg)len;
    return (int)len;
}
