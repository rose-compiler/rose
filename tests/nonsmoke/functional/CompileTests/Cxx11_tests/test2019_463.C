#include<thread>
// #include<atomic>

int cnt = 0;
auto f = [&]{cnt++;};
// std::thread t1{f}, t2{f}, t3{f}; // undefined behavior
std::thread t1{f};

// std::atomic<int> cnt2{0};
// auto f2 = [&]{cnt2++;};
// std::thread t4{f2}, t5{f2}, t6{f2}; // OK
