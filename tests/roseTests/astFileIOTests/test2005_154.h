
class X
   {
     public:
          static int isParallel() 
             {
               if (isparallel == -1)
                    isparallel = 42;
               return isparallel;
             }

          static int numProcs;
          static int isInitialized;
          static int isparallel;
   };

