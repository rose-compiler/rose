struct pirq 
   {
   };

struct domain
   {
   };

   
// #ifndef USE_ROSE_BACKEND
/* DQ (5/5/2015): Added handling for self-referential macro. */
#define pirq_cleanup_check(pirq, d) \
    ((pirq)->evtchn ? pirq_cleanup_check(pirq, d) : (void)0)
// #endif

void (pirq_cleanup_check)(struct pirq *pirq, struct domain *d)
{
}

