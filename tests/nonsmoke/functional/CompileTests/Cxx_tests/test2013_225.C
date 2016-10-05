template <typename T>
class vector
{
    // accessor functions and so forth
    private:
    T* vec_data;   // we'll store the data as block of dynamically allocated 
                   // memory
    int length;    // number of elements used 
    int vec_size;  // actual size of vec_data
};

// But when it comes to bools, you might not really want to do this because most systems are going to use 16 or 32 bits for each boolean type even though all that's required is a single bit. So we might make our boolean vector look a little bit different by representing the data as an array of integers whose bits we manually manipulate. (For more on manipulating bits directly, see bitwise operators and bit manipulations in C and C++.)
// To do this, we still need to specify that we're working with something akin to a template, but this time the list of template parameters will be empty:

// template <>

// and the class name is followed by the specialized type: class className<type>. In this case, the template would look like this:

template <>
class vector <bool>
{
    // interface

    private:
    unsigned int *vector_data;
    int length;
    int size;
};
