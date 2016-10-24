/*
Bug name:	ctor-not-emitted

Reported by:	Brian White

Date:		Sept 29, 2007

Date sent:      Sept 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:  	SPEC CPU 2006 	471.omnetpp/src/libs/sim/cmodule.cc
                                471.omnetpp/src/libs/sim/cqueue.cc
                                471.omnetpp/src/libs/sim/cllist.cc

Brief description:  	ROSE backend does not emit the constructor
                        name for a declaration within a for loop
                        initializer.  Instead, only the actual args
                        are emitted.  The same declaration outside
                        of a for loop is properly emitted.

Files
-----
bug.cc		Declares an object within a for loop initializer
                and outside the for loop.  The constructor
                takes two actual arguments.

                This results in the following error:

rose_bug.cc: In function `void receiveOn()':
rose_bug.cc:27: error: conversion from `int' to non-scalar type `
   cQueue::Iterator' requested

rose_bug.cc	The output from the backend for bug.cc.  In it, we can
		see that the constructor name is not omitted when
                the object is declared within the for loop initializer.
                The constructor name is emitted when the declaration
                occurs outside the for loop.  Also, notice that
                the syntax is different outside:

  class ::cQueue::Iterator qiter2(putAsideQueue,(0));

                and inside the loop (i.e., the =):

  class ::cQueue::Iterator qiter = (putAsideQueue,(0));
*/

class cQueue 
{

  public:
    class Iterator
    {
      public:
      Iterator(const cQueue& q, bool athead) { }
    };
};

void receiveOn()
{
  cQueue putAsideQueue;
  // backend outputs qiter2 declaration properly.
  cQueue::Iterator qiter2( putAsideQueue, 0 );
  // backend does not emit constructor name, just args, for qiter.
  for( cQueue::Iterator qiter( putAsideQueue , 0 ); ; ) { }
}
