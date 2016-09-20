// CoderInfo-ootm.ii:9:14: error: cannot find scope name `std'
// CoderInfo-ootm.ii:9:14: error: there is no type called `std::string'

// sm: This is invalid C++, and is properly rejected by gcc-3 
// and by icc.

template<class T> class basic_string {};

typedef basic_string<char> string;

class CoderInfo {
  CoderInfo (std::string &name);
};
