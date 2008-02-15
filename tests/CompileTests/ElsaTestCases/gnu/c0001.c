// c0001.c
// complicated statement expression from Pine

typedef struct MC {
  int lockcount;
  int msgno;
} MC;
void *fs_get(int);
typedef unsigned char __uint8_t;

MC *mail_new_cache_elt (unsigned long msgno)
{
  MC *elt = (MC *)
    (__extension__
      (__builtin_constant_p ( sizeof (MC) ) && ( sizeof (MC) ) <= 16    ?
        (( sizeof (MC) ) == 1 ?
          ({ void *__s = (  fs_get (sizeof (MC))  );
           *((__uint8_t *) __s) = (__uint8_t)0 ;
            __s; }) :
          ({ void *__s = (  fs_get (sizeof (MC))  );
             union {
               unsigned int __ui;
               unsigned short int __usi;
               unsigned char __uc;
             } *__u = __s;
             __uint8_t __c = (__uint8_t) ( 0  );
             switch ((unsigned int) (   sizeof (MC)  ))      {
               case 15:
                 __u->__ui = __c * 0x01010101;
                 __u = __extension__ ((void *) __u + 4);
               case 11:
                 __u->__ui = __c * 0x01010101;
                 __u = __extension__ ((void *) __u + 4);
               case 7:
                 __u->__ui = __c * 0x01010101;
                 __u = __extension__ ((void *) __u + 4);
               case 3:
                 __u->__usi = (unsigned short int) __c * 0x0101;
                 __u = __extension__ ((void *) __u + 2);
                 __u->__uc = (unsigned char) __c;
                 break;
               case 14:
                 __u->__ui = __c * 0x01010101;
                 __u = __extension__ ((void *) __u + 4);
               case 10:
                 __u->__ui = __c * 0x01010101;
                 __u = __extension__ ((void *) __u + 4);
               case 6:
                 __u->__ui = __c * 0x01010101;
                 __u = __extension__ ((void *) __u + 4);
               case 2:
                 __u->__usi = (unsigned short int) __c * 0x0101;
                 break;
               case 13:
                 __u->__ui = __c * 0x01010101;
                 __u = __extension__ ((void *) __u + 4);
               case 9:
                 __u->__ui = __c * 0x01010101;
                 __u = __extension__ ((void *) __u + 4);
               case 5:
                 __u->__ui = __c * 0x01010101;
                 __u = __extension__ ((void *) __u + 4);
               case 1:
                 __u->__uc = (unsigned char) __c;
                 break;
               case 16:
                 __u->__ui = __c * 0x01010101;
                 __u = __extension__ ((void *) __u + 4);
               case 12:
                 __u->__ui = __c * 0x01010101;
                 __u = __extension__ ((void *) __u + 4);
               case 8:
                 __u->__ui = __c * 0x01010101;
                 __u = __extension__ ((void *) __u + 4);
               case 4:
                 __u->__ui = __c * 0x01010101;
               case 0:
                 break;
             }
             __s;
           }) 
         )     :
       (__builtin_constant_p ( 0 ) && ( 0 ) == '\0' ? 
         ({ void *__s = ( fs_get (sizeof (MC)) ); 
            __builtin_memset ( __s , '\0',    sizeof (MC)  ) ; 
            __s; })   : 
         memset ( fs_get (sizeof (MC)) , 0 ,  sizeof (MC) )
       )
     )
   ) ;

  elt->lockcount = 1;
  elt->msgno = msgno;
  return elt;
}


