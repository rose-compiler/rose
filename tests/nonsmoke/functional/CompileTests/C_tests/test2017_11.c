void foo()
   {
     int i;
     int hexdump = 1;
     int lmp_obj_ctype;
     switch (i)
        {
          case 3:
          case 4:
               switch(lmp_obj_ctype) 
                  {
                    case 1:
                    case 2:
#if 0
                    printf("\n\t    IPv4 Link ID: %s (0x%08x)",
                       getname((const u_char *)(obj_tptr)),
                       ((u_int32_t)(__extension__ ({ register unsigned int __v, __x = (*(const u_int32_t *)(obj_tptr)); if (__builtin_constant_p (__x)) __v = ((((__x) & 0xff000000) >> 24) | (((__x) & 0x00ff0000) >> 8) | (((__x) & 0x0000ff00) << 8) | (((__x) & 0x000000ff) << 24)); else __asm__ ("bswap %0" : "=r" (__v) : "0" (__x)); __v; }))));
#endif
                    break;
// # 479 "./print-lmp.c"
                    case 5:
                    case 6:
#if 0
                    printf("\n\t    Link ID: %u (0x%08x)",
                       ((u_int32_t)(__extension__ ({ register unsigned int __v, __x = (*(const u_int32_t *)(obj_tptr)); if (__builtin_constant_p (__x)) __v = ((((__x) & 0xff000000) >> 24) | (((__x) & 0x00ff0000) >> 8) | (((__x) & 0x0000ff00) << 8) | (((__x) & 0x000000ff) << 24)); else __asm__ ("bswap %0" : "=r" (__v) : "0" (__x)); __v; }))),
                       ((u_int32_t)(__extension__ ({ register unsigned int __v, __x = (*(const u_int32_t *)(obj_tptr)); if (__builtin_constant_p (__x)) __v = ((((__x) & 0xff000000) >> 24) | (((__x) & 0x00ff0000) >> 8) | (((__x) & 0x0000ff00) << 8) | (((__x) & 0x000000ff) << 24)); else __asm__ ("bswap %0" : "=r" (__v) : "0" (__x)); __v; }))));
#endif
                    break;
                    default:
                         hexdump=1;
                  }
          break;
        }
   }
