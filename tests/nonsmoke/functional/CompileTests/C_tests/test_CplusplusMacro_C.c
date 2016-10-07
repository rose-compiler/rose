#ifdef __cplusplus
 #error "__cplusplus defined"
#else
 #ifdef _MSC_VER
  #pragma message("__cplusplus NOT defined")
 #else
  #warning "__cplusplus NOT defined"
 #endif
#endif
