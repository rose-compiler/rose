// aggregate initialization

#include<string>

// simple example of aggregate initialization
struct user
{
    uint32_t id_;
    std::string name_;
};

user u1{10, "Alice"};

