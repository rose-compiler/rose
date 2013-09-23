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

template <typename T>
class sortedVector // : public vector<T>
{
    public:
    void insert (T val)
    {
#if 0
        if ( length == vec_size )   // length is the number of elements
        {
            vec_size *= 2;    // we'll just ignore overflow possibility!
            vec_data = new T[vec_size];
        }
        ++length;  // we are about to add an element
        
        // we'll start at the end, sliding elements back until we find the
        // place to insert the new element
        int pos;
        for( pos = length; pos > 0 && val > vec_data[pos - 1]; --pos )
        {
            vec_data[pos] = vec_data[pos - 1];
        }
        vec_data[pos] = val;
#endif
    }
    // other functions...
    private:
    T *vec_data;
    int length;
    int size;
};

// Now, notice that in the above for loop, we're making a direct comparison between elements of type T. That's OK for most things, but it would probably make more sense to have sorted on the actual object type instead of the pointer address. To do that, we'd need to write code that had this line:

// for( pos = length; pos > 0 && *val > *vec_data[pos - 1]; --pos )

// Of course, that would break for any non-pointer type. What we want to do here is use a partial specialization based on whether the type is a pointer or a non-pointer (you could get fancy and have multiple levels of pointers, but we'll stay simple).

// To declare a partially specialized template that handles any pointer types, we'd add this class declaration:

template <typename T>
class sortedVector<T *>
{
    public:
    // same functions as before.  Now the insert function looks like this:
    void insert( T *val )
    {
#if 0
        if ( length == vec_size )   // length is the number of elements
        {
            vec_size *= 2;    // we'll just ignore overflow possibility!
            vec_data = new T[vec_size];
        }
        ++length;  // we are about to add an element
        
        // we'll start at the end, sliding elements back until we find the
        // place to insert the new element
        int pos;
        for( pos = length; pos > 0 && *val > *vec_data[pos - 1]; --pos )
        {
            vec_data[pos] = vec_data[pos - 1];
        }
        vec_data[pos] = val;
#endif
    }

    private:
    T** vec_data;
    int length;
    int size;
};

// There are a couple of syntax points to notice here. First, our template parameter list still names T as the parameter, but the declaration now has a T * after the name of the class; this tells the compiler to match a pointer of any type with this template instead of the more general template. The second thing to note is that T is now the type pointed to; it is not itself a pointer. For instance, when you declare a sortedVector<int *>, T will refer to the int type! This makes some sense if you think of it as a form of pattern matching where T matches the type if that type is followed by an asterisk. This does mean that you have to be a tad bit more careful in your implementation: note that vec_data is a T** because we need a dynamically sized array made up of pointers.

// You might wonder if you really want your sortedVector type to work like this--after all, if you're putting them in an array of pointers, you'd expect them to be sorted by pointer type. But there's a practical reason for doing this: when you allocate memory for an array of objects, the default constructor must be called to construct each object. If no default constructor exists (for instance, if every object needs some data to be created), you're stuck needing a list of pointers to objects, but you probably want them to be sorted the same way the actual objects themselves would be!

// Note, by the way, that you can also partially specialize on template arguments--for instance, if you had a fixedVector type that allowed the user of the class to specify both a type to store and the length of the vector (possibly to avoid the cost of dynamic memory allocations), it might look something like this:

template <typename T, unsigned length>
class fixedVector 
   { 
   };

// Then you could partially specialize for booleans with the following syntax

template <unsigned length>
class fixedVector<bool, length> 
   {
   };

