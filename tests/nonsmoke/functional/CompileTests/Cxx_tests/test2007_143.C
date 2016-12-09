/*
Bug name:	omitted-qualifier2

Reported by:	Brian White

Date:		Sep 30, 2007

Date sent:      Sept 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:  	SPEC CPU 2006 483.xalancbmk/src/ElemLiteralResult.cpp


Brief description:  	The backend does not emit a 'NamespacesHandler::' 
                        qualifier
                        that is present (and necessary) in the input source.

                        This is probably the same bug as reported in
                        omitted-qualifier.

Files
-----
bug.cc		Declares a class NamespacesHandler, within which is
                defined a class PrefixChecker.  Then declares a
                class AVTPrefixChecker in global scope that inherits
                from NamespacesHandler::PrefixChecker.

                We get the following error

rose_bug.cc:16: error: syntax error before `{' token

                because the qualifier 'NamespacesHandler::' is
                dropped in 
                class AVTPrefixChecker : public NamespacesHandler::PrefixChecker

rose_bug.cc	Contains the backend output of bug.cc.  Notice that the
                following class declaration for AvtPrefixChecker is 
                incorrect because it omits the qualifier:

class AVTPrefixChecker : public PrefixChecker
{
}
*/


class NamespacesHandler
{
public:

        class PrefixChecker
	{
	public:
	};
};


class AVTPrefixChecker : public NamespacesHandler::PrefixChecker
{
public:

};
