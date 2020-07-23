// helloworld.cpp
// export module helloworld;  // module declaration
export module test2020_45_module;  // module declaration
import <iostream>;         // import declaration
 
export void hello() {      // export declaration
    std::cout << "Hello world!\n";
}

