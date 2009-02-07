/*
The code:
gives the following error message:

Inside of backend(SgProject*): SgProject::get_verbose() = 0
Inside of backend(SgProject*): project->numberOfFiles() = 1
sourceFilenames.size() = 1
sourceFileName = test.C oldFileName = test.C
At base of SgFile::generateOutputFileName(): objectFileName = test.o
rose_test.C: In member function `void foo::prod(const foo&, foo&) const':
rose_test.C:17: error: no matching function for call to `foo::prod(foo&,
const
   foo&, foo&) const'
rose_test.C:16: error: candidates are: void foo::prod(const foo&, foo&) const

the cause of the error can be found in the output from ROSE on line 17
where you can see that '::' has been removed from '::prod(...)':
*/
#if 0


class foo
{
  public: void prod(const class foo &x,class foo &y) const;
}

;

inline void prod(class foo &y,const class foo &A,const class foo &x)
{
  A.prod(x,y);
}


void foo::prod(const class foo &x,class foo &y) const
{
  prod(y,x,y);
}


int main()
{
  return 0;
}
#endif


class foo
   {
     public:
          void prod ();
   };

// Function appearing before declaration of prod in global scope
void foobar ()
   {
  // forward declaration does not imply that the "prod" function is in global scope (just that it exists).
  // This declaration does not associate a scope to "void prod()"
     void prod (int x);

  // Can't use global qualification here since there is no "prod" function declared in global scope.
  // Since we don't know exactly where "void prod()" is we can't use explicit qualification.
     prod (0);
   }

// This says that it exists and that it is declared in global scope (some sort of stronger forward declaration)
// This defines the scope of "void prod()"
// void prod (int x);

void foo::prod ()
   {
  // Enough to declare the function, but not strong enough to say what the qualified name is when it is called.
  // Again, this does not associate the scope with "void prod(int)"
     void prod (int x);

  // Error: global scope name qualification is dropped in generated code!
  // global qualification here is required!
#if 0
     ::prod (0);
#else
     prod (0);
#endif
   }
