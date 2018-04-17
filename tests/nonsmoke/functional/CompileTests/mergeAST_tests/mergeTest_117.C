// Node is not in parent's child list, node: 0x609110 = SgTypedefSeq = default name parent: 0x752470 = SgPointerType
// This is now fixed (added nodes pointed to by the type->get_ptr_to() and type->get_ref_to() functions to the 
// nodeList in addAssociatedNodes().

namespace std
   {
     template<typename _Tp, typename _Pointer = _Tp*, typename _Reference = _Tp&> struct iterator {};
   }

namespace std
   {
     struct _Bit_iterator_base : public iterator<bool>
        {
#if 1
          unsigned int _M_offset;
          void _M_bump_up() { _M_offset++; }
#endif
        };
   }

