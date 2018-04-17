/*
The following code does not unparse correctly on my system:

typedef unsigned int size_t;
namespace std {using ::size_t;}
std::size_t x;

The incorrect output is:

typedef unsigned int size_t;
namespace std
{
}
std::size_t x;

The specific names std and size_t are important to triggering this bug.  Is this a known problem?

-- Jeremiah Willcock 

*/

typedef unsigned int size_t;
namespace std {using ::size_t;}
std::size_t x;

