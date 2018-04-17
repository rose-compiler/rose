/*
This code (extracted from the IU C++ bindings for MPI) causes an error
message from the EDG frontend used in ROSE (even using
identityPreprocessor), and the error points beyond the end of the file.
This code works in g++ and icc, except that it does not have a main()
function.  Could you please look it over?  Pieces of code similar to this
also trigger bugs in the EDG/SAGE connection, but those might really be
EDG problems based on the behavior of this code.
Jeremiah Willcock
*/

class mpi2cppList {
public:
  class iter;
  class Link {
    friend class iter;
    Link *next;
  };
  class iter {
    Link* node;
  public:
    iter& operator++() { node = node->next; return *this; }
    iter operator++(int) { iter tmp = *this; ++(*this); return tmp; }
  };
};
