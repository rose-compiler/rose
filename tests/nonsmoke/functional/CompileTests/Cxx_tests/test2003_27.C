/* This program generates an error in EDG/SAGE connection

s->kind = template (kind = 51) iek_last = 52 
Error: case not implemented in generateFileInfo ( a_source_sequence_entry_ptr s) 

NOTE: Since these templates are not instantiated anywhere, no code is produced
      from this file within the unparsing phase.
 */

template < class T1 >
class reverse_iterator
   {
     T1 abc;
   };

template <class T1, class T2 = int, class T3 = long >
class XYZ
   {
     public:
          typedef T1* pointer;
          typedef pointer iterator;
          typedef ::reverse_iterator<iterator> reverse_iterator;
   };
