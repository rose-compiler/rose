struct pair
   {
     int first;
  // int second;

  // pair() : first(), second() { }
  // pair(const int & __a, const int & __b) : first(__a), second(__b) { }

     pair(const pair & __p) : first(__p.first) /* , second(__p.second) */ { }
   };
