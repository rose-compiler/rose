/*
IMPORTANT BUG FOR 7/10/2007:
(this appears to have been reported previously as test2007_91.C)

Hi Dan,
This is an imporatnt bug which needs high priority. I has been
reported hearlier on the date shown below.

Andreas

---------- Forwarded message ----------
From: Andreas Sæbjørnsen <andreas.saebjoernsen@gmail.com>
Date: Jun 25, 2007 2:00 PM
Subject: bug in ROSE
To: "Daniel J. Quinlan" <dquinlan@llnl.gov>


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
SgScopeStatement::get_hidden_declaration_list(): Assertion this != __null failed.
/home/andreas/links/g++-411: line 2: 26106 Aborted
/home/andreas/REPOSITORY-BUILD/gcc-4.1.1/ROSE/June-23a-Unsafe/tutorial/identityTranslator $@
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
