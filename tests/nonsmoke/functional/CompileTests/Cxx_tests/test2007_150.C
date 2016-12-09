/*
Bug name:	unqualified-return-value

Reported by:	Brian White

Date:		Sept 29, 2007

Date sent:      Sep 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:  	SPEC CPU 2006 471.omnetpp/src/libs/sim/ctopo.cc

Brief description:  	The return type 'cTopology::Link *' of
                        a method 'cTopology::in' is not qualified.

Files
-----
bug.cc		Defines a method 'cTopology::in' that returns a 
                value of type 'cTopology::Link *'.

rose_bug.cc	The type of the return of 'cTopology::in' is
                'Link *', rather than 'cTopology::Link *',
                and hence undefined.
*/

class cTopology 
{
  public:
    class Link;

    Link *in(int i);
  
    class Link
    {
    };
};

cTopology::Link *cTopology::in(int i)
{
  return 0;
}

