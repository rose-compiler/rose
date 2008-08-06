// Dummy classes, the first of which is designed to be used as a base class
// from which one inherits an interface, the second designed to be used as a
// base class from which one only inherits an implementation.
class Interface { /* ... */ };
class Implementation { /* ... */ };

// not OK: multiple public base classes
class A: public Interface, public Implementation
{
    /* ... */
};

// OK: only one public base class, others may be non-public
class B: public Interface, private Implementation
{
    /* ... */
};
