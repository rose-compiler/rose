#include <iostream>
#include <list>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#include <unistd.h>

using namespace std;

class httpRequest
{
  public:
  int i;
 };

#define LARGE_NUMBER 100
//#define LARGE_NUMBER 1000000
list<httpRequest> request_queue (LARGE_NUMBER);

void process ( httpRequest input)
{
//  printf("processing %d by thread %d ...\n", input.i, omp_get_thread_num());
  cout<<"processed by thread "<<omp_get_thread_num();
  //unsigned int cost = rand()%3;
  unsigned int cost = 1; // constant cost for simplicity
  sleep(cost);
 // printf("Took %d seconds.\n", cost);
}

int
main ()
{
  srand (time (NULL));
  list<httpRequest>::iterator i;
#pragma omp parallel
  {
#pragma omp single
    for (std::list<httpRequest>::iterator i = request_queue.begin(); i!=request_queue.end();i++)
    {
#pragma omp task
      process (*i);
    }
  }
}

