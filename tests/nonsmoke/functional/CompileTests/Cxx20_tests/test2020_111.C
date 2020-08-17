struct HasNoRelational {};
 
struct C 
   {
     friend HasNoRelational operator<=>(const C&, const C&);

  // DQ (7/22/2020): EDG 6.0 complains that the member function must be const.
  // bool operator<(const C&) = default;   // ok, function is deleted
     bool operator<(const C&) const = default;   // ok, function is deleted
   };

