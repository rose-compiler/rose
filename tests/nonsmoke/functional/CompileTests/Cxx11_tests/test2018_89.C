template<typename _Tp, _Tp __v>
struct integral_constant {};

template<typename _Tp>
struct alignment_of
   : public integral_constant<int, __alignof__(_Tp)> { };

#if 0
unparses to:

template < typename _Tp, _Tp __v >
    struct integral_constant {
    };

template < typename _Tp >
    struct alignment_of
    : public integral_constant < int, __ALIGNOF__ ( _Tp ) > { };

Which gets this error:
rose_ROSE-30.cc:6:57: error: there are no arguments to '__ALIGNOF__' that depend on a template parameter, so a declaration of '__ALIGNOF__' must be available [-fpermissive]
     : public integral_constant < int, __ALIGNOF__ ( _Tp ) > { };
                                                         
rose_ROSE-30.cc:6:57: note: (if you use '-fpermissive', G++ will accept your code, but allowing the use of an undeclared name is deprecated)
rose_ROSE-30.cc:6:59: error: template argument 2 is invalid
     : public integral_constant < int, __ALIGNOF__ ( _Tp ) > { };
                                                           ^
#endif

