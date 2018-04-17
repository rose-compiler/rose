/*
Bug name:	dereference-of-array-base

Reported by:	Brian White

Date:		Sept 28, 2007

Date sent:      Sept 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:  	SPEC CPU 2006 	450.soplex/src/mpsinput.cc
                                450.soplex/src/spxmpsread.cc
				471.omnetpp/src/libs/sim/cpar.cc

Brief description:  	Assertion failure occurs when attempt to
                        dereference an array base.

Files
-----
bug.cc		Declares an array char m_buf[24] and then attempts
                to dereference the base: *m_buf.  This results
                in the following assertion failure:

is_array_decay_cast(node) == true, handling case of cast that does array-to-pointer decay 

rosec: Cxx_Grammar.C:21864: void SgLocatedNode::setCompilerGenerated(): Assertion `get_startOfConstruct() != __null' failed.

*/

int main()
{
  // Dereferencing an m_buf pointer would not cause a problem.
  //  char *m_buf;
  // But dereferencing an array base does:
  char m_buf[24];
  *m_buf;
  return 0;
}
