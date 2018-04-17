/*
when compiling the following code in ROSE

class nsACString_internal
 {
   protected:
     nsACString_internal()
       {}
 };

class nsDependentCString : public nsACString_internal
 {
   public:
     explicit
     nsDependentCString()
       {
       }
 };

void
ProcessManifestBuffer()
{
  & (const nsACString_internal&) nsDependentCString();
}

I get the following error

lt-identityTranslator: Cxx_Grammar.C:14587: bool Sg_File_Info::ok()
const: Assertion this != __null failed.
/home/andreas/links/g++-411: line 2: 10667 Aborted
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/ROSE/June-29a-2007-Unsafe/tutorial/identityTranslator  
*/

class nsACString_internal
 {
   protected:
     nsACString_internal()
       {}
 };

class nsDependentCString : public nsACString_internal
 {
   public:
     explicit
     nsDependentCString()
       {
       }
 };

void
ProcessManifestBuffer()
{
  & (const nsACString_internal&) nsDependentCString();
}
