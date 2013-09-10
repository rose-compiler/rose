
template<typename T>
class vector
   {
      T var;
   };

typedef void OnShutdownFunc();

// This should be unparsed as: 
//    vector<void (*)()>* shutdown_functions = 0L;
// but instead is unparsed as:
//    class vector< void (* > *shutdown_functions = 0L;
vector<void (*)()>* shutdown_functions = 0L;

#if 1
void InitShutdownFunctions() 
   {
     shutdown_functions = new vector<void (*)()>;
   }
#endif
