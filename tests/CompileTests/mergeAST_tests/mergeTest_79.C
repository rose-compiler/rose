template<typename _T1, typename _T2> void _Construct(_T1* __p, const _T2& __value) {}
template<typename _T1> void _Construct(_T1* __p) {}

char*
uninitialized_copy(const char* __first, const char* __last, char* __result)
  {
    return 0L; // __result + (__last - __first);
  }

#if 0
wchar_t*
uninitialized_copy(const wchar_t* __first, const wchar_t* __last, wchar_t* __result)
  {
     return 0L; // __result + (__last - __first);
  }
#endif

