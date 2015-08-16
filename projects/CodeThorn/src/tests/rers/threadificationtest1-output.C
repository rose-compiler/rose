namespace RERS_Problem
{
int * input;
int * output;
int * a21;
int * a316;
int a64[] = {(42), (43), (44), (45), (46), (47)};
int ** a62;
int * cf;

void errorCheck(int thread_id) {};

void calculate_outputm1(int thread_id)
{
  cf[thread_id] = 0;
  a21[thread_id] = 32;
  a62[thread_id] = a64;
  a316[thread_id] = a21[thread_id];
  a21[thread_id] = (a21[thread_id] * a316[thread_id] % 14999 % 14901 + - 15097) / 5 + - 2185;
  a316[thread_id] = a62[thread_id][a21[thread_id] - 29];
  output[thread_id] = 24;
  //fflush(stdout);
}

void calculate_output(int thread_id)
{
cf[thread_id] = 1;
if (a21[thread_id] <= 199 &&  cf[thread_id] == 1) {
  if (a21[thread_id] == 33 && 46 == a64[4] && (a21[thread_id] == a62[thread_id][0] && (cf[thread_id] == 1))) {
    calculate_outputm1(thread_id);
  }
}

errorCheck(thread_id);
if (output[thread_id] <= - 100) 
  return ;
if (cf[thread_id] == 1) 
  output[thread_id] = - 2;
}
}
