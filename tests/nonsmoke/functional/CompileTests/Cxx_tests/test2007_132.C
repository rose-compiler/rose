/*
Bug name:	empty-sizeof-class

Reported by:	Brian White

Date:		Sept 29, 2007

Date sent:      Sept 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:  	SPEC CPU 2006 	471.omnetpp/src/libs/sim/cmodule.cc
                                483.xalancbmk/src/ReaderMgr.cpp
                                483.xalancbmk/src/XSAnnotation.cpp
				

Brief description:  	Presumably related to the bug reported as
                        empty-sizeof.  Please see that first.

                        ROSE backend does not emit the actual arg
                        of a sizeof when sizeof(class_name) is added to
                        an integer.  The output is shown in rose_bug.cc.

Files
-----
bug.cc		Invokes new char[m+sizeof(cSimpleModule)], 
                where cSimpleModule is a class.

		This is the error returned when we try to compile
		the file rose_bug.cc:

rose_bug.cc: In function `void memAlloc()':
rose_bug.cc:11: error: syntax error before `)' token


rose_bug.cc	The output from the backend for bug.cc.  In it, we can
		see that nothing is passed to sizeof:

  char *p = new char [(m) + sizeof()];
*/

class cSimpleModule 
{

};


void memAlloc()
{
    int m;
    char *p = new char[m+sizeof(cSimpleModule)];  
}
