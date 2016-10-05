// calling a templated function pointer

// error: you can't use an expression of type `funcType &' as a function

// originally found in package aprsd

// ERR-MATCH: you can't use an expression of type `.*?' as a function

template<typename funcType>
void generate(funcType func) {
    func();
}
