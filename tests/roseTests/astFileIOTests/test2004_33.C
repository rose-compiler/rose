
// Test simple namespace
namespace X_long_name
   {
     int x;
     int Xfoo();
   }

// Test namespace alias
namespace X = X_long_name;

// Build a new namespace to test the using directive
namespace Y
   {
  // int Yfoo();
  // using X::Xfoo;

#if 0
  // g++ error: using namespace not supported
  // Note that translation in ROSE does not use the aliased name 
  // and generates "using namespace X_long_name;"
     using namespace X;
#endif

#if 0
  // g++ error: must specify declaration in X_long_name explicitly (e.g. using X_long_name::x)
  // EDG accepts this correctly (examle in Stroustrup's book)
     using namespace X_long_name;
#endif

  // Simple using directive
     using X_long_name::Xfoo;

  // Simple using directive
     using X_long_name::x;
   }

// Test unnamed namespace declaration
namespace
   {
  // Simple using directive
     using X::x;

     extern int Xfoo();
   }

#if 0
int main()
   {
     return 0;
   }
#endif
