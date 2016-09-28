/*
Compiling the following code in ROSE:

class nsISupports;

template <class T>
class nsDerivedSafe : public T
 {
 };


template <class T>
class nsCOMPtr
 {
   public:
     nsDerivedSafe<T>*
     operator->() const
       {
         return 0;
       }
 };

class nsIComponentRegistrar {
public:
 virtual unsigned int  AutoRegister() = 0;
};


int
main(void)
{
   nsCOMPtr<nsIComponentRegistrar> registrar ;
   registrar->AutoRegister();

   return 0;
}


I get the following error:


lt-identityTranslator: Cxx_Grammar.C:52912: std::set<SgSymbol*,
std::less<SgSymbol*>, std::allocator<SgSymbol*> >&
SgScopeStatement::get_hidden_declaration_list(): Assertion this !=__null failed.
/home/andreas/links/g++-411: line 2: 26106 Aborted
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/ROSE/June-23a-Unsafe/tutorial/identityTranslator
*/

class nsISupports;

template <class T>
class nsDerivedSafe : public T
 {
 };


template <class T>
class nsCOMPtr
 {
   public:
     nsDerivedSafe<T>*
     operator->() const
       {
         return 0;
       }
 };

class nsIComponentRegistrar {
public:
 virtual unsigned int  AutoRegister() = 0;
};


int
main(void)
{
   nsCOMPtr<nsIComponentRegistrar> registrar ;
   registrar->AutoRegister();

   return 0;
}

