class edu_umd_cs_findbugs_workflow 
   {
     public:
          class Filter
             {
               public: int i;
             };
   };

class edu_umd_cs_findbugs_filter 
   {
     public:
          class Filter 
             {
               public: float f;
             };
   };

int main(int argc, char **argv) 
   {
     edu_umd_cs_findbugs_workflow::Filter a;
     edu_umd_cs_findbugs_filter::Filter b;

     return 0;
   }

