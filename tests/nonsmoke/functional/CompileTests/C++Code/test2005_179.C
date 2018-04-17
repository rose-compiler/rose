#if 0
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

void prod (int x);

void foo::prod ()
   {
  // Error: global scope name qualification is dropped in generated code!
     ::prod (0);
   }
