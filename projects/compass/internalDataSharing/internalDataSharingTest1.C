class A
{
public:
    // not OK: returning non-const pointer member
    int *retptr() { return p; }
    // not OK: returning non-const reference to data pointed to by member
    int &retref() { return *p; }
    // not OK: returning non-const reference to member
    int *&retptrref() { return p; }

private:
    int *p;
};

class B
{
public:
    // OK: returning copy of pointed-to data
    int retint() { return *p; }
    // OK: const ptr
    const int *retcptr() { return p; }
    // OK: const ref
    const int &retcref() { return *p; }

    // maybe OK, depending on "operatorsExcepted" parameter
    int &operator*() { return *p; }

private:
    int *p;
};
