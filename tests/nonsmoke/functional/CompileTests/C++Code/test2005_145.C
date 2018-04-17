// this is a test of the global namespace qualifier "::"
// this was a bug in ROSE handling of KULL.

typedef int number;
typedef ::number globalNumber;

namespace X
   {
     typedef int number;
     typedef number localNumber;
     typedef ::number globalNumber;
   }

namespace Y
   {
     typedef int number;
     typedef number localNumber;
     typedef ::number globalNumber;
     typedef X::number otherNamespaceNumber;
   }

namespace X
   {
     typedef Y::number otherNamespaceNumber;
     typedef Y::otherNamespaceNumber YotherNamespaceNumber;
   }
