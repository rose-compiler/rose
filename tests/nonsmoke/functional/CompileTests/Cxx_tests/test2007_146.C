/*

Bug name:	two-declarations-in-for-loop

Reported by:	Brian White

Date:		Sept 29, 2007

Date sent:      Sep 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:  	SPEC CPU 2006 	483.xalancbmk/src/DOMRangleImpl.cpp

Brief description:  	When two objects are declared within a for loop
                        initializer, the backend adorns each with the type:

                        for (class ::foo *x = (0), class ::foo *y = (0); ; ) {
                        }

                        Instead, the type should only occur once:

                        for(foo *x = 0, *y = 0; ; ) { }
                        

Files
-----
bug.cc		Includes the for loop above, which declares both x and y
                in the initializer.

rose_bug.cc	The output from the backend for bug.cc.  Shows that
                both x and y are adorned with the type foo, as shown above.

*/

class foo { 
public:
  foo *next;
};

int main()
{
  for(foo *x = 0, *y = 0; ; ) { }
  return 0;
}
