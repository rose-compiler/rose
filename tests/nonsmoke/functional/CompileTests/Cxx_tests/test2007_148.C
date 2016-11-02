/*
Bug name:	unqualified-assignment-operator

Reported by:	Brian White

Date:		Sept 28, 2007

Date sent:      Sep 30, 2007

E-mail subject: more SPEC 2006 bugs

Occurs in:  	SPEC CPU 2006 450.soplex/src/didxset.cc
                              450.soplex/src/dsvector.cc
                              450.soplex/src/soplex.cc
                              450.soplex/src/ssvector.cc
                              450.soplex/src/svset.cc
                              450.soplex/src/updatevector.cc
                              471.omnetpp/src/EtherApp_m.cc
                              471.omnetpp/src/EtherCtrl_m.cc
                              471.omnetpp/src/EtherFrame_m.cc
                              471.omnetpp/src/MACAddress.cc
                              471.omnetpp/src/libs/sim/carray.cc
                              471.omnetpp/src/libs/sim/cmessage.cc
                              471.omnetpp/src/libs/sim/cpar.cc
                              471.omnetpp/src/libs/sim/ctypes.cc
                              471.omnetpp/src/libs/sim/cchannel.cc
                              471.omnetpp/src/libs/sim/cfsm.cc
                              471.omnetpp/src/libs/sim/cmodule.cc
                              471.omnetpp/src/libs/sim/cpsquare.cc
                              471.omnetpp/src/libs/sim/cvarhist.cc
                              471.omnetpp/src/libs/sim/cgate.cc
                              471.omnetpp/src/libs/sim/cmsgheap.cc
                              471.omnetpp/src/libs/sim/cqueue.cc
                              471.omnetpp/src/libs/sim/cdensity.cc
                              471.omnetpp/src/libs/sim/cnetmod.cc
                              471.omnetpp/src/libs/sim/cdetect.cc
                              471.omnetpp/src/libs/sim/chist.cc
                              471.omnetpp/src/libs/sim/cstat.cc
                              471.omnetpp/src/libs/sim/cksplit.cc
                              471.omnetpp/src/libs/sim/cenum.cc
                              471.omnetpp/src/libs/sim/cllist.cc
                              471.omnetpp/src/libs/sim/cpacket.cc
                              483.xalancbmk/src/DOM_Attr.cpp
                              483.xalancbmk/src/DOM_CDATASection.cpp
                              483.xalancbmk/src/DOM_CharacterData.cpp
                              483.xalancbmk/src/DOM_Comment.cpp
                              483.xalancbmk/src/DOM_Document.cpp
                              483.xalancbmk/src/DOM_DocumentFragment.cpp
                              483.xalancbmk/src/DOM_DocumentType.cpp
                              483.xalancbmk/src/DOM_Element.cpp
                              483.xalancbmk/src/DOM_Entity.cpp
                              483.xalancbmk/src/DOM_EntityReference.cpp
                              483.xalancbmk/src/DOM_Notation.cpp
                              483.xalancbmk/src/DOM_ProcessingInstruction.cpp
                              483.xalancbmk/src/DOM_Text.cpp
                              483.xalancbmk/src/DOM_XMLDecl.cpp
                              483.xalancbmk/src/SAXParseException.cpp
                              483.xalancbmk/src/MutableNodeRefList.cpp
                              483.xalancbmk/src/NodeRefList.cpp
                              483.xalancbmk/src/XalanAttr.cpp
                              483.xalancbmk/src/XalanCharacterData.cpp
                              483.xalancbmk/src/XalanCDataSection.cpp
                              483.xalancbmk/src/XalanComment.cpp
                              483.xalancbmk/src/XalanDocumentFragment.cpp
                              483.xalancbmk/src/XalanDocumentType.cpp
                              483.xalancbmk/src/XalanDocument.cpp
                              483.xalancbmk/src/XalanElement.cpp
                              483.xalancbmk/src/XalanEntity.cpp
                              483.xalancbmk/src/XalanEmptyNamedNodeMap.cpp
                              483.xalancbmk/src/XalanEntityReference.cpp
                              483.xalancbmk/src/XalanNotation.cpp
                              483.xalancbmk/src/XalanText.cpp
                              483.xalancbmk/src/XalanProcessingInstruction.cpp

Brief description:  	The invocation of operator= on a base class instance o
                        is improperly emited as =o.  This bug is related to/
                        the same as that previously reported as
                        incomplete-qualification-for-equality-operator
                        and
                        incomplete-qualification-for-assignment-operator
                        in an earlier bug report.  Those bug reports
                        indicated that a templated class was required
                        to manifest the bug.  Not so, as shown here.

                        This problem is not particular to =.  e.g.,
                        -= and += also manifest the same bug, as in
                        ssvector.cc.

Files
-----
bug.cc		Contains the invocation IdxSet::operator=(old), within a 
                copy constructor of the derived class DIdxSet.

rose_bug.cc	Contains the backend output of bug.cc.  In particular,
		it shows the invocation of operator= is emitted 
                as =old.
*/

class IdxSet
{
public:
  IdxSet& operator=(const IdxSet& set)
  {
    return *this;
  }
};

class DIdxSet : public IdxSet
{
public:

   DIdxSet(const IdxSet& old);
   DIdxSet& operator=(const DIdxSet& set)
   {
     return *this;
   }
};

DIdxSet::DIdxSet(const IdxSet& old)
{
  // The following invocation of DIdxSet::operator=
  // correctly produces (*(this))=(old);
  //   DIdxSet::operator= ( old );
  // The following invocation of IdxSet::operator=
  // improperly leads to =old;
   IdxSet::operator= ( old );
}
