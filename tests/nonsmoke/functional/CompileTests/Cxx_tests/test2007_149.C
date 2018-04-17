/*
Bug name:	unqualified-bracket-operator

Reported by:	Brian White

Date:		Sept 28, 2007

Date sent:      Sep 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:  	SPEC CPU 2006 450.soplex/src/svset.cc

Brief description:  	This is almost certainly the same bug as reported
                        in unqualified-assignment-operator.  Please
                        see that bug report first.

                        The invocation of operator[] on a base class instance o
                        with arg x is improperly emit as [x].  

Files
-----
bug.cc		Contains the invocation IdxSet::operator[](x), within a 
                copy constructor of the derived class DIdxSet.

rose_bug.cc	Contains the backend output of bug.cc.  In particular,
		it shows the invocation of operator[] is emitted 
                as [x].
*/

class IdxSet
{
public:
   IdxSet& operator[](int n)
   {
     return *this;
   }

};

class DIdxSet : public IdxSet
{
public:

   DIdxSet(const IdxSet& old);
};

DIdxSet::DIdxSet(const IdxSet& old)
{
  int x;
  this->IdxSet::operator[](x);
}
