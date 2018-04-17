// template function declared in another class (as a friend)

// originally found in package apt

// ---- BEGIN: messages from an ambiguity ----
// there is no variable called `foo'
// `T' used as a variable, but it's actually a type
// ---- SEPARATOR: messages from an ambiguity ----
// explicit template arguments were provided after `foo', but that is not the name of a template function
// there is no function called `foo'
// ---- SEPARATOR: messages from an ambiguity ----
// there is no type called `foo<T>'
// ---- END: messages from an ambiguity ----

// ERR-MATCH: used as a variable, but it's actually a type

struct S {
    template<typename F> friend const F& foo(int);
};

template<typename T> T bar() {
    return foo<T>(0);
}
