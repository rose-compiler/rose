

typedef unsigned short uint8_t;

struct vcpu;

typedef void (*xen_event_channel_notification_t)(struct vcpu *v, unsigned int port);

static xen_event_channel_notification_t xen_consumers[8];

static uint8_t get_xen_consumer(xen_event_channel_notification_t fn)
   {
     unsigned int i;
#if 0
     if ( fn == ((void*)0) )
          fn = default_xen_notification_fn;
#endif
#if 0
     for ( i = 0; i < (sizeof(xen_consumers) / sizeof((xen_consumers)[0]) + sizeof(struct { int:-!!(__builtin_types_compatible_p(typeof(xen_consumers), typeof(&xen_consumers[0]))); })); i++ )
        {
          if ( xen_consumers[i] == ((void*)0) )
               xen_consumers[i] = fn;
          if ( xen_consumers[i] == fn )
               break;
        }
#endif

 // Expansion of Xen BUG_ON macro.
     do { 
          if (__builtin_expect((i >= (sizeof(xen_consumers) / sizeof((xen_consumers)[0]) + sizeof(struct { int:-!!(__builtin_types_compatible_p(typeof(xen_consumers), typeof(&xen_consumers[0]))); }))),0)) 
               do { 
                    do {
                         ((void)sizeof(struct { int:-!!((91) >> ((31 - 24) + (31 - 24))); })); 
                         asm volatile ( ".Lbug%=: ud2\n" ".pushsection .bug_frames.%c0, \"a\", @progbits\n" ".p2align 2\n" ".Lfrm%=:\n" ".long (.Lbug%= - .Lfrm%=) + %c4\n" ".long (%c1 - .Lfrm%=) + %c3\n" ".if " "0" "\n" ".long 0, %c2 - .Lfrm%=\n" ".endif\n" ".popsection" : : "i" (2), "i" ("event_channel.c"), "i" (((void*)0)), "i" ((91 & ((1 << (31 - 24)) - 1)) << 24), "i" (((91) >> (31 - 24)) << 24));
                       }
                    while (0);

                    do {} while (1); 

                  }
                while (0); 
        }
     while (0);

     return i+1;
   }
