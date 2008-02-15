#include "intone.h"

class Nanos
   {
     public:
         static inline void begin_for (int start,int end,int step,int chunk,int schedule /* , int & loopId */ )
            {
               in__tone_begin_for_(&start,&end,&step,&chunk,&schedule /* , &loopId */);
            }

         static inline int next_iters ( int & start, int & end, int & last )
            {
              return in__tone_next_iters_ (&start,&end,&last);
            }

         static inline void end_for ( bool barrier_required )
            {
              int required = barrier_required;
              in__tone_end_for_ (&required);
            }

         static inline int numberOfCPUs()
            {
              return in__tone_cpus_current_();
            }

         static inline void spawn ( void (* f) (), int numberOfArgs, int numberOfProcs, ... )
            {
           // Need to handle the ... explicitly
           // in__tone_spawnparallel_(f,&numberOfArgs,&numberOfProcs, ...);
            }

   };

