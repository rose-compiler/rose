#include <vector>
#include <cstdint>
#include <iostream>

// DQ (10/18/2017): Added string support.
#include<string>

class Container {
    private:
        std::vector<int> m_Values;
    public:
        Container(){}
        
        void storeValue(int value)
        {
                m_Values.push_back(value); 
        }

        void dump()
        {
                for(auto &i : m_Values)
                {
                        std::cout << i << std::endl;
                }
        }
};

//Entry point for C++
// extern "C" void process(void);
extern "C" void process(char* top_builddir, char* sourceFileNameWithPath);

//Used by C# to store the results
extern "C" void storeResults(uint64_t container, int value);

