/*
Bug name:	empty-sizeof

Reported by:	Brian White

Date:		Sept 28, 2007

Date sent:      Sept 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:  	SPEC CPU 2006 	444.namd/src/LJTable.C
				471.omnetpp/src/libs/sim/carray.cc

Brief description:  	ROSE backend does not emit the actual arg
                        of a sizeof when sizeof is multiplied
                        by a member and the entire 
                        expression is passed to new.  The
                        output is shown in rose_bug.cc.

                        Note that if the private member, table_dim,
                        is replaced with 2 (for example), then the
                        problem does not occur.

Files
-----
bug.cc		Invokes sizeof(int)*lj.table_dim where table_dim is
                a public member of object lj.  Note that the bug
                still occurs if the invocation occurs within a
                method and/or when table_dim is private.

		This is the error returned when we try to compile
		the file rose_bug.cc:

rose_bug.cc: In function `int main()':
rose_bug.cc:13: error: syntax error before `)' token

rose_bug.cc	The output from the backend for bug.cc.  In it, we can
		see that nothing is passed to sizeof:

  lj.::LJTable::table_alloc = (new char [((lj.::LJTable::table_dim)) * sizeof()]);
*/

class LJTable
{
public:

  char *table_alloc;
  int table_dim;
};

int main()
{
  LJTable lj;
  lj.table_alloc = new char[lj.table_dim*sizeof(int)];

  return 0;
}
