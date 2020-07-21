struct HasNoRelational {};
 
struct C {
  friend HasNoRelational operator<=>(const C&, const C&);
  bool operator<(const C&) = default;                       // ok, function is deleted
};

