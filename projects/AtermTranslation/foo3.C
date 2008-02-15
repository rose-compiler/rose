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
