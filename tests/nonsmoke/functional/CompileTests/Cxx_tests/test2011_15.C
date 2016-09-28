/*
The following example causes an iterator error in HiddenList.C. The iterator error is pasted below. If I compile ROSE
without debug iterators, I simply get a crash (EXC_BAD_ACCESS in the debugger).

Sample code:
----------------
namespace foo
{
  namespace bar
  {
  }
}

using namespace foo::bar;
using namespace foo;
----------------

Iterator error output:
----------------
/usr/include/c++/4.4/debug/safe_iterator.h:624:error: attempt to compute
    the difference between a singular iterator to a        dereferenceable (start-of-sequence) iterator.

Objects involved in the operation:
iterator "lhs" @ 0x0x83857a0 {
type = N11__gnu_debug14_Safe_iteratorIN9__gnu_cxx17__normal_iteratorIPN23Hidden_List_Computation20NamespaceInformationEN
St6__norm6vectorIS4_SaIS4_EEEEENSt7__debug6vectorIS4_S8_EEEE (mutable iterator);
  state = singular;
}
iterator "rhs" @ 0x0xbfffde1c {
type = N11__gnu_debug14_Safe_iteratorIN9__gnu_cxx17__normal_iteratorIPN23Hidden_List_Computation20NamespaceInformationEN
St6__norm6vectorIS4_SaIS4_EEEEENSt7__debug6vectorIS4_S8_EEEE (mutable iterator);
  state = dereferenceable (start-of-sequence);
  references sequence with type `NSt7__debug6vectorIN23Hidden_List_Computation20NamespaceInformationESaIS2_EEE'
@ 0x0xbfffde1c
}
----------------

The error occurs on line 121 of HiddenList.C:

               if (locationOfIteratorForUsedNamespace != this->NamespacesIteratorHashMap.end())
                  {
                 // This line is the one causing the crash. The lhs iterator is invalid
                    bound = locationOfIteratorForUsedNamespace->second.it_vec_namesp_info - X.begin();
                  }


-George 
*/

namespace foo
   {
     namespace bar
        {
        }
   }

using namespace foo::bar;
using namespace foo;
