#define DEMO_BUG 0

struct pending_eoi 
   {
//   int vector;
   };

static __typeof__(struct pending_eoi) per_cpu__pending_eoi[2];

int foobar(void)
   {
#if DEMO_BUG
     typeof(&per_cpu__pending_eoi) x;
#endif

     ({
        unsigned long __ptr;
     // Original code:
     //    (typeof(&per_cpu__pending_eoi) ) __ptr;
     // Unparses as:
     //    (typeof(typeof(struct pending_eoi ) * )[2]) )__ptr;

     // Since we can't generate:
     //    (typeof(&per_cpu__pending_eoi) )
     // because EDG does not have the required information if they 
     // type has not been defined previously, I think what we what is:
     //    (typeof(typeof(struct pending_eoi ) [2]) *)  __ptr;
     // But at present we generate:
     //    (typeof(typeof(struct pending_eoi ) (*)[2]) )__ptr;
     // which might also be OK and equivalent.
     // This subject of equivalence and/or correctness needs a bit more review, I think.

        (typeof(&per_cpu__pending_eoi) ) __ptr;
        0;
     });

   }
