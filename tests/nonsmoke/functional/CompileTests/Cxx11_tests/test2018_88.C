template<typename _Rep>
struct duration {
  constexpr duration() = default;
};

template<typename _Rep1, typename _Rep2>
constexpr duration<int>
operator/(const duration<_Rep1>& __d,
          const          _Rep2&  __s);

void func1()
{
   duration<int> var1;
   duration<int> var2;
   int                        var3;
   var1 = var2 / ( var3 - 1 );
}

 
#if 0
unparses to:
template < typename _Rep >
struct duration {
  constexpr duration ( ) = default;
};

template < typename _Rep1, typename _Rep2 >
constexpr duration < int >
operator / ( const duration < _Rep1 > & __d,
          const _Rep2 & __s );

void func1()
{
  struct duration< int  > var1;
  struct duration< int  > var2;
  int var3;

  var1 = var2/var3 - 1;
}

Which gets this error:

rose_ROSE-29.cc: In function 'void func1()':
rose_ROSE-29.cc:15:20: error: no match for 'operator-' (operand types are 'duration<int>' and 'int')
   var1 = var2/var3 - 1;
                    ^
#endif
