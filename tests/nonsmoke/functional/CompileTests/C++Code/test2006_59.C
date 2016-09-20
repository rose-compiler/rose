#if 0
Compiling the following code:

typedef struct { } a, *b;
b foo ();
int main()
{
  return 0;
}

with ROSE gives the following error:

testTranslator: Cxx_Grammar.C:135430: virtual SgName
SgPointerType::get_mangled() const: Assertion base_name.get_length ()
failed.
/home/saebjornsen1/links/g++3.3.2: line 4: 10474 Aborted                
/export/0/tmp.saebjornsen1/BUILDS/ROSE-CVS/gcc3.3.2-WITHOUT-WAVE/tests/testTranslator
$@

This error will disappear if "a" is removed from "typedef struct {} a,*b;"

Thanks
Andreas
#endif


typedef struct { } a, *b;

b foo ();

int main()
   {
     return 0;
   }

