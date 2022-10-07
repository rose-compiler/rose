namespace RERS_Problem
{
int input;
int output;
int a21 = 1;
int a316 = 11;
int a64[] = {(42), (43), (44), (45), (46), (47)};
int *a62 = a64;
int cf = 1;

void errorCheck(int thread_id) {};

void calculate_outputm1(int thread_id)
{
  cf = 0;
  a21 = 32;
  a62 = a64;
  a316 = a21;
  a21 = (a21 * a316 % 14999 % 14901 + - 15097) / 5 + - 2185;
  a316 = a62[a21 - 29];
  output = 24;
  //fflush(stdout);
}

void calculate_output(int thread_id)
{
cf = 1;
if (a21 <= 199 &&  cf == 1) {
  if (a21 == 33 && 46 == a64[4] && (a21 == a62[0] && (cf == 1))) {
    calculate_outputm1(thread_id);
  }
}

errorCheck(thread_id);
if (output <= - 100) 
  return ;
if (cf == 1) 
  output = - 2;
}
}
