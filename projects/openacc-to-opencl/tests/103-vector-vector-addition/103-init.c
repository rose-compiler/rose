
void read_params(
  int argc, char ** argv,
  unsigned long * n
) {
  assert(argc == 5);

  *n = atoi(argv[4]);
}

void init_datas(unsigned long n, float ** a, float ** b, float ** c) {
  unsigned long i;

  *a = (float*)malloc(n * sizeof(float));
  for (i = 0; i < n; i++)
    (*a)[i] = rand();

  *b = (float*)malloc(n * sizeof(float));
  for (i = 0; i < n; i++)
    (*b)[i] = rand();

  *c = (float*)malloc(n * sizeof(float));
  for (i = 0; i < n; i++)
    (*c)[i] = 0.;
}

void launch(
  int argc, char ** argv,
  unsigned long num_gang,
  unsigned long num_worker,
  unsigned long vector_length,
  acc_timer_t data_timer, acc_timer_t comp_timer
) {
  unsigned long n;
  float offset;

  read_params(argc, argv, &n);

  float * a;
  float * b;
  float * c;

  init_datas(n, &a, &b, &c);

  kernel_103(n, a, b, c, num_gang, num_worker, vector_length, data_timer, comp_timer);

  acc_timer_delta(data_timer);
  acc_timer_delta(comp_timer);

  free(a);
  free(b);
  free(c);
}

