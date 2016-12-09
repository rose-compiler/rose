namespace std
{
template<typename _Tp>
inline const _Tp &
min(const _Tp & __a, const _Tp & __b)
   {
     if (__b < __a) return __b; return __a;
   }
}

#if 0
template<>
inline const int &
min<int>(const int & __a, const int & __b)
   {
     if (__b < __a) return __b; return __a;
   }
#endif

// using namespace X;
void foo()
   {
     int x = std::min(1,2);
   }

