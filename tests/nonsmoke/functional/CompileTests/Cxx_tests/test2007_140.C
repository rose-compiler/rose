/*
Bug name:	interaction-between-inlined-method-and-enum

Reported by:	Brian White

Date:		Sept 30, 2007

Date sent:      Sept 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:  	SPEC CPU 2006 	src/483.xalancbmk/src/MutableNodeRefList.hpp
                                src/483.xalancbmk/src/ElemTemplateElement.hpp

Brief description:  	Access to an enum from a method inlined within the
                        class causes the following assertion failure:

rosec: Cxx_Grammar.C:4682: bool Sg_File_Info::ok() const: Assertion `this != __null' failed.

                        The enum is declared within the class.

                        The assertion failure does not occur if
                        the method is not inlined (i.e., un-#define INLINED).

Files
-----
bug.cc		Declares an enum eOrder within a class MutableNodeRefList.
                A variable temp of type eOrder is assigned within 
                method swap.  If swap is defined within the class
                (i.e., #define INLINED), then the assertion results.
                If swap is defined outside of the class, thers is
                no assertion failure.
*/

#define INLINED

class MutableNodeRefList 
{
public:

#ifdef INLINED
	void
	swap()
	{
		eOrder	temp = m_order;

	}
#else
        void swap();
#endif
	enum eOrder { eUnknownOrder };

        eOrder	m_order;

};



#ifndef INLINED
void
MutableNodeRefList::swap()
{
  eOrder	temp = m_order;
  
}
#endif
