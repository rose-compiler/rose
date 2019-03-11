// Multithreading memory model
// See also: Memory model (computing)

// The C++11 standardizes support for multithreaded programming.

// There are two parts involved: a memory model which allows multiple threads to co-exist in a program and 
// library support for interaction between threads. (See this article's section on threading facilities.)

// The memory model defines when multiple threads may access the same memory location, and specifies when 
// updates by one thread become visible to other threads.

// This is a library issue independent of the langauge.

// #error "NEED AND EXAMPLE OF THIS!"

#include<thread>
#include<atomic>

int cnt = 0;
auto f = [&]{cnt++;};
std::thread t1{f}, t2{f}, t3{f}; // undefined behavior

std::atomic<int> cnt2{0};
auto f2 = [&]{cnt2++;};
std::thread t4{f2}, t5{f2}, t6{f2}; // OK
