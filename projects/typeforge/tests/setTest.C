
double* set3_function(double * set3_arg_1, double set3_arg_2[10]){
  return set3_arg_1;
}

int main(){
  double set1_1 = 0.0;
  double* set1_2 = &set1_1;
  double set1_3 = *set1_2;
  set1_2 = &set1_3;

  double *set2_1;
  double set2_2[5][5];
  double independent_1 = 0.0;
  set2_1 = set2_2[0];
  set2_1[1] = independent_1;

  double* set3_1;
  double  set3_2[10];
  set3_1 = set3_2;
  set3_1 = set3_function(set3_1, set3_2);
}
