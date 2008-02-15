/*
Dan and Rich,

Sorry, your ChangeLog does clearly says on the 5th line that it works with
a 64-bit compiler.

I have updated to this new version ROSE-0.8.7a and am having the same
problems.  I have included my translator again since I forgot the
Makefile-ROSE.inc file previously.  You should only have to edit this file
to compile.

I would like to determine:
1)  For a given SgMemberFunctionDeclaration, representing a
constructor/destructor/operator=, whether it was compiler
generated.  Perhaps if there is a SgMemberFunctionDeclaration for a
method, it was necessarily user-defined?  That would be fine, but doesnt
jibe with my experience.
2)  For a constructor/destructor/operator= call, whether the directly
invoked method is user-defined or compiler generated.

I am using Sg_File_Info::isCompilerGenerated.  Perhaps this is intended
for templates instead (as the documentation only talks about templates).
I am referring to compiler generated methods in the sense that a user need
not specify a default constructor, Foo::Foo(), for example.  Instead, the
compiler can supply one.  The same is true for copy constructors,
operator=, and destructors.

Here again is the problem.  This example is in foo.C; the results are in
foo.out.

class Foo {
 public:
  Foo() { ; }  // Foo::Foo is user-defined
};

class Bar : public Foo {
 public:
  // Bar::Bar is compiler generated, but
  // Bar::Bar invoekes the user-defined Foo::Foo
};

class Baz {
 public:
  // Baz::Baz is compiler generated.
};

int main()
{
  Bar *b = new Bar;  // Invokes the compiler-generated Bar::Bar,
                     // translator says Bar::Bar is not
                     // compiler generated.  WRONG!
  Foo *f = new Foo;  // Invokes the user-defined Foo::Foo,
                     // translator says Foo::Foo is not
                     // compiler generated.  Correct.
  Baz *bz = new Baz; // Invokes the compiler-generated Baz::Baz.
                     // Translator says SgMemberFunctionDeclaration
                     // is NULL.  If this is synonymous with
                     // compiler-generated, OK.
  return 0;
}


Am I doing something incorrectly?  I do a NodeQuery to get a
SgConstructorInitializer, from that I get a SgMemberFunctionDeclaration,
from that I get a Sg_File_Info and call its isCompilerGenerated()
accessor.

I wouldnt mind if ROSE tells me that the 'new Bar' case is not compiler
generated, so long as the member function declaration were Foo::Foo.
Unfortunately, it gives me the Bar::Bar constructor.  Why does this exist?

Thanks,
Brian
*/
class Foo {
 public:
  Foo() { ; }  // Foo::Foo is user-defined
};

class Bar : public Foo {
 public:
  // Bar::Bar is compiler generated, but
  // Bar::Bar invoekes the user-defined Foo::Foo
};

class Baz {
 public:
  // Baz::Baz is compiler generated.
};

int main()
{
  Bar *b = new Bar;  // Invokes the compiler-generated Bar::Bar,
                     // translator says Bar::Bar is not
                     // compiler generated.  WRONG!
  Foo *f = new Foo;  // Invokes the user-defined Foo::Foo,
                     // translator says Foo::Foo is not
                     // compiler generated.  Correct.
  Baz *bz = new Baz; // Invokes the compiler-generated Baz::Baz.
                     // Translator says SgMemberFunctionDeclaration
                     // is NULL.  If this is synonymous with
                     // compiler-generated, OK.
  return 0;
}
