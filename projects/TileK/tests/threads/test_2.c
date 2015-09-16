
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct test_timer_t_ {
  struct timespec start;
  struct timespec stop;
  long delta;
} * test_timer_t;

test_timer_t timer_build() {
  return malloc(sizeof(struct test_timer_t_));
}

void timer_start(test_timer_t timer) {
   if (timer == NULL) return;

  clock_gettime(CLOCK_REALTIME, &(timer->start));
}

void timer_stop (test_timer_t timer) {
   if (timer == NULL) return;

  clock_gettime(CLOCK_REALTIME, &(timer->stop));

  timer->delta = (timer->stop.tv_nsec - timer->start.tv_nsec) / 1000000;
  if (timer->delta > 0)
    timer->delta += (timer->stop.tv_sec - timer->start.tv_sec) * 1000;
  else
    timer->delta = (timer->stop.tv_sec - timer->start.tv_sec) * 1000 - timer->delta;
}

float ** create_array(int n, int m) {
  float ** a = malloc(n * sizeof(float *));
  float * a_ = malloc(n * m * sizeof(float));

  int i, j;

  for (i = 0; i < n; i++) {
    a[i] = a_ + i * m;
    for (j = 0; j < m; j++) {
      a[i][j] = i+j;
    }
  }

  return a;
}

void free_array(float ** a) {
  free(a[0]);
  free(a);
}

void kernel_0(int n, int m, float ** A, float b) {
  int i, j;
  #pragma tilek kernel data(A[0:n][0:m]) num_threads(8)
  {
  #pragma tilek loop tile[0](thread) tile[1](dynamic)
  for (i = 0; i < n; i++)
    #pragma tilek loop tile[2](dynamic)
    for (j = 0; j < m; j++)
      A[i][j]+=b;
  }
}

void kernel_1(int n, int m, float ** A, float b) {
  int i, j;
  #pragma tilek kernel data(A[0:n][0:m]) num_threads(8)
  {
  #pragma tilek loop tile[0](dynamic) tile[1](thread)
  for (i = 0; i < n; i++)
    #pragma tilek loop tile[2](dynamic)
    for (j = 0; j < m; j++)
      A[i][j]+=b;
  }
}

void kernel_2(int n, int m, float ** A, float b) {
  int i, j;
  #pragma tilek kernel data(A[0:n][0:m]) num_threads(8)
  {
  #pragma tilek loop tile[0](dynamic)
  for (i = 0; i < n; i++)
    #pragma tilek loop tile[1](thread) tile[2](dynamic)
    for (j = 0; j < m; j++)
      A[i][j]+=b;
  }
}

void kernel_3(int n, int m, float ** A, float b) {
  int i, j;
  #pragma tilek kernel data(A[0:n][0:m]) num_threads(8)
  {
  #pragma tilek loop tile[0](dynamic)
  for (i = 0; i < n; i++)
    #pragma tilek loop tile[1](dynamic) tile[2](thread)
    for (j = 0; j < m; j++)
      A[i][j]+=b;
  }
}

int main() {

  const int n = 16 * 1024;
  const int m = 16 * 1024;

  float ** a;

  test_timer_t timer = timer_build();

  {
    timer_start(timer);

    a = create_array(n, m);

    kernel_0(n, m, a, 3.5);

    free_array(a);

    timer_stop(timer);
    printf("#0 : %d\n", timer->delta);
  }

  {
    timer_start(timer);

    a = create_array(n, m);

    kernel_1(n, m, a, 3.5);

    free_array(a);

    timer_stop(timer);
    printf("#1 : %d\n", timer->delta);
  }

  {
    timer_start(timer);

    a = create_array(n, m);

    kernel_2(n, m, a, 3.5);

    free_array(a);

    timer_stop(timer);
    printf("#2 : %d\n", timer->delta);
  }

  {
    timer_start(timer);

    a = create_array(n, m);

    kernel_3(n, m, a, 3.5);

    free_array(a);

    timer_stop(timer);
    printf("#3 : %d\n", timer->delta);
  }

  return 0;
}

