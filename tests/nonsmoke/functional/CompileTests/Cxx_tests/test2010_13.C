/*
Hi Rose users,
   The currently used version of EDG (3.3) fails to compile the following code:
saying error: no appropriate operator delete is visible
The above code is correct, though, and EDG 4.0 accepts it, as does g++.
The fact that EDG 3.3 rejects it causes some problems when parsing
boost.wave. Minor ones, because explicitly calling f->~foo (); and then
foo::operator delete works. Depending on how severe you consider this
failure, it might be a good time to upgrade.

Regards,
*/

 typedef __SIZE_TYPE__ size_t;

  struct foo
  {
    void* operator new (size_t bytes);
    void  operator delete (void* ptr, size_t bytes);
  };

  int
  main ()
  {
    foo* f = new foo;
    delete f;
    return 3 + 2;
  }

