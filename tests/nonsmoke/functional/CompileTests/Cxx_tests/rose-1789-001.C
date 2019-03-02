template <class T>
class SinglyLinkedListWrap;

template <class T>
class SinglyLinkedEntryWrap {
 friend class SinglyLinkedListWrap<T>;
};

template <class T>
class SinglyLinkedListWrap {
   void foo() {
     Iterator iter;
     T & c = iter.Current();
   }
   class Iterator {};
};
