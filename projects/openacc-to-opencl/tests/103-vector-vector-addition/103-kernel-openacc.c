
void kernel_103(
  unsigned long n, float * a, float * b, float * c,
  unsigned long num_gang, unsigned long num_worker, unsigned long vector_length,
  acc_timer_t data_timer, acc_timer_t comp_timer
) {
  unsigned i;

  acc_timer_start(data_timer);

  #pragma acc data copyin(a[0:n], b[0:n]) copyout(c[0:n])
  { // (1)

    acc_timer_start(comp_timer);

    #pragma acc parallel num_gang(num_gang) num_worker(num_worker) vector_length(vector_length)
    { // (2)
      #pragma acc loop gang worker vector
      for (i = 0; i < n; i++)
        c[i] = a[i] + b[i];
    } // (2)

    acc_timer_stop(comp_timer);

  } // (1)

  acc_timer_stop(data_timer);
}

/*! @} */

