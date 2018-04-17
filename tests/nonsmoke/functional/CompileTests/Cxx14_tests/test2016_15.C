// This example code demonstrates the a test code that used the std::unique_ptr<> type.
// I think that this is C++11 specific and might not have any C++14 specfic parts.

// #ifndef USERQUEUES_H
// #define USERQUEUES_H

#include <iostream>

#include <deque>
#include <vector>
#include <memory>
// #include "Job.h"
// #include <Job.h>

class Job {};
typedef Job* JobPtr;

class UserQueues 
   {
     public:
#if 0
          explicit UserQueues();
          UserQueues(const UserQueues&) = delete;
          UserQueues& operator=(const UserQueues&) = delete;
          ~UserQueues() = default;
#else
          UserQueues();
#endif
     public:
          typedef std::unique_ptr<Job> JobPtr;
          typedef std::deque<JobPtr> JobDeque;

     public:
       // UserQueues();
          void printDeques();
          void addToDeque(JobPtr job, int queueId);

     public:
          const uint QUEUE_QTY = 3;

     private:
          std::vector<JobDeque> _jobDeques;
   };

// #endif

// UserQueues.cpp
// #include <iostream>
// #include "UserQueues.h"

UserQueues::UserQueues() : 
    _jobDeques()
{
    for(unsigned int idx = 0 ; idx < QUEUE_QTY ; ++idx)
    {
     // _jobDeques.push_back(JobDeque());
    }
}

void UserQueues::printDeques()
{
    for (std::size_t idx = 0; idx < _jobDeques.size(); ++idx)
    {
        std::cout << "[";

        for(std::size_t jdx = 0 ; jdx < _jobDeques[idx].size() ; ++jdx)
        {
         // std::cout << _jobDeques[idx].at(jdx)->getArrivalTime();

            if(jdx != (_jobDeques[idx].size() - 1))
            {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
    } 
}

void UserQueues::addToDeque(JobPtr job, int queueId)
{
    _jobDeques[0].push_front(std::move(job));
}


