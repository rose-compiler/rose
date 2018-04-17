/*
Bug name:	omitted-qualifier

Reported by:	Brian White

Date:		Sep 30, 2007

Date sent:      Sept 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:  	SPEC CPU 2006 483.xalancbmk/src/DOMServies.cpp
                              483.xalancbmk/src/XObjectResultTreeFragProxyText.cpp
                              483.xalancbmk/src/Constants.cpp
                              483.xalancbmk/src/XSLTEngineImpl.cpp


Brief description:  	The backend does not emit a '::' qualifier
                        that is present (and necessary) in the input source.

Files
-----
bug.cc		Declares a variable x in the global scope and a const
                var xalanc_1_8::x in scope xalanc_1_8.  Then attempts to
                assign to ::x within main, which is in scope xalanc_1_8.

                Leads to the error

rose_bug.cc: In function `int xalanc_1_8::main()':
rose_bug.cc:8: error: assignment of read-only reference `xalanc_1_8::x'

rose_bug.cc	Contains the backend output of bug.cc.  The error occurs
                because the assignment to x in main drops the '::x', so
                that 'x = 5' attempts to assign 5 to the const var
                'xalanc_1_8::x' (since the assignment occurs within
                the scope xalanc_1_8.
*/

int x;

namespace xalanc_1_8
{
  const int &x = ::x;

int main()
{
  ::x = 5;
  return 0;
}

} // end namespace
