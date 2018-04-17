#include <cassert>
#include <boost/fusion/adapted/boost_tuple.hpp>
#include <boost/fusion/adapted/std_pair.hpp>

int main() {
    boost::tuple<char, int, char, int> decim('-', 10, 'e', 5);
    assert(stringize(decim) == "-10e5");

    std::pair<short, std::string> value_and_type(270, "Kelvin");
    assert(stringize(value_and_type) == "270Kelvin");
}


