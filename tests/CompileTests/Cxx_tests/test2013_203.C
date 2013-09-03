// class alt_OptionInterpreter;

namespace protobuf {

// class alt_OptionInterpreter;

class DescriptorBuilder 
   {
  // DQ (6/11/2013): This friend has no effect since it has not yet seen the OptionInterpreter class 
  // declaration and thus will build a non-defining reference to another one in the protobuf namespace.
  // Note also that since OptionInterpreter is nested in DescriptorBuilder, any private or protected data
  // is accessible without the requirement of a friend declaration.

  // DQ (6/10/2013): Ths scope of this non-defining declaration is not correct (set incorrectly to the outer scope).
  // EDG reports the scope of the secondary declaration to be the namespace protobuf!!!
     friend class OptionInterpreter;
  // friend class protobuf::alt_OptionInterpreter;
  // friend class ::alt_OptionInterpreter;

     class OptionInterpreter {};

//   friend class OptionInterpreter;
   };

// class alt_OptionInterpreter {};

}  // namespace protobuf

// class alt_OptionInterpreter {};

