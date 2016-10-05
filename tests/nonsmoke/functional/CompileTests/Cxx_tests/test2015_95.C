#include <boost/lexical_cast.hpp>

class BitVector {

public:

 // typedef unsigned Word;

 // This function causes the file to work or fail (uncomment to demonstrate failure).
    BitVector& fromHex() {
        size_t idx;
        throw std::runtime_error(boost::lexical_cast<std::string>(idx));

        return *this;
    } 

};

