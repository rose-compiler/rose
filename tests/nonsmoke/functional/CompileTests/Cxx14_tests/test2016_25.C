// Extended constexpr:
// C++14 now allows more things inside the body of constexpr functions, notably:

//    local variable declarations (not static or thread_local, and no uninitialized variables)
//    mutating objects whose lifetime began with the constant expression evaluation
//    if, switch, for, while, do-while (not goto)

// So in C++14, the above function generalized to strings can stay idiomatic, and use a normal loop directly:

// C++14 also removes the C++11 rule that constexpr member functions are implicitly const.

#if 1
// C++11 example:
constexpr int my_charcmp( char c1, char c2 ) 
   {
     return (c1 == c2) ? 0 : (c1 < c2) ? -1 : 1;
   }
#endif

// C++14 example:
constexpr int my_strcmp( const char* str1, const char* str2 ) 
   {
     int i = 0;

     for( ; str1[i] && str2[i] && str1[i] == str2[i]; ++i )
        { }

     if( str1[i] == str2[i] ) return 0;

     if( str1[i] < str2[i] ) return -1;

     return 1;
   }
