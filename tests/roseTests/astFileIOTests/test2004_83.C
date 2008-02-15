// This is an example of a const pointer to a member function from Boost (concept_check.hpp)

// DQ (9/14/2004): I have not made it into a compilable test program yet!

template <class Concept>
inline void function_requires(type<Concept>* = 0)
{
#if !defined(NDEBUG)
    void (Concept::*x)() = BOOST_FPTR Concept::constraints;
    ignore_unused_variable_warning(x);
#endif
}

