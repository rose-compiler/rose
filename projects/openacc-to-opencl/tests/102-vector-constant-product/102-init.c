
void read_params(
  int argc, char ** argv,
  unsigned long * n,
  float * scalar
) {
  assert(argc == 5 || argc == 6);

  *n = atoi(argv[4]);

  if (argc == 6)
    *scalar = atof(argv[5]);
  else
    *scalar = 10.58436;
}

void init_datas(unsigned long n, float ** a) {
  *a = (float*)malloc(n * sizeof(float));
  unsigned long i;
  for (i = 0; i < n; i++)
    (*a)[i] = rand();
}

void launch(
  int argc, char ** argv,
  unsigned long num_gang,
  unsigned long num_worker,
  unsigned long vector_length,
  acc_timer_t data_timer, acc_timer_t comp_timer
) {
  unsigned long n;
  float scalar;

  read_params(argc, argv, &n, &scalar);

  float * a;

  init_datas(n, &a);

  kernel_102(n, a, scalar, num_gang, num_worker, vector_length, data_timer, comp_timer);

  acc_timer_delta(data_timer);
  acc_timer_delta(comp_timer);

  free(a);
}

