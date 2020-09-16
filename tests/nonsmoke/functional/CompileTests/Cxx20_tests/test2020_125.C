template<typename ...Args>
int sum(Args&&... args) {
//    return (args + ... + 1 * 2); // Error: operator with precedence below cast
    return (args + ... + (1 * 2)); // OK
}

