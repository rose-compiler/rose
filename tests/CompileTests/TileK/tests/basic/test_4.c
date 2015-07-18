
#include <stdlib.h>

struct point_t {
  double x[3];
  double v;
};

struct operator_t {
  double m[3][3];
  double v[3];
};

struct point_t * create_points(int n) {
  struct point_t * res = malloc(n * sizeof(struct point_t));

  // TODO

  return res;
}

struct operator_t * create_operators(int n) {
  struct operator_t * res = malloc(n * sizeof(struct operator_t));

  // TODO

  return res;
}

void free_array(float ** a) {
  free(a[0]);
  free(a);
}

void kernel_0(int n, struct point_t * points, const struct operator_t op) {
  int p;
  #pragma tilek kernel data(points[0:n])
  {
    #pragma tilek loop tile[0](dynamic)
    for (p = 0; p < n; p++) {
      double y[3];
      y[0] = op.m[0][0] * points[p].x[0] + op.m[0][1] * points[p].x[1] + op.m[0][2] * points[p].x[2] + op.v[0];
      y[1] = op.m[1][0] * points[p].x[0] + op.m[1][1] * points[p].x[1] + op.m[1][2] * points[p].x[2] + op.v[1];
      y[2] = op.m[2][0] * points[p].x[0] + op.m[2][1] * points[p].x[1] + op.m[2][2] * points[p].x[2] + op.v[2];
      points[p].x[0] = y[0];
      points[p].x[1] = y[1];
      points[p].x[2] = y[2];
    }
  }
}

void kernel_1(int n, int m, struct point_t * points, const struct operator_t * operators) {
  int p, q;
  #pragma tilek kernel data(points[0:n], operators[0:m])
  {
    #pragma tilek loop tile[0](dynamic)
    for (p = 0; p < n; p++) {
      #pragma tilek loop tile[1](dynamic)
      for (q = 0; q < m; q++) {
        double y[3];
        y[0] = operators[q].m[0][0] * points[p].x[0] + operators[q].m[0][1] * points[p].x[1] + operators[q].m[0][2] * points[p].x[2] + operators[q].v[0];
        y[1] = operators[q].m[1][0] * points[p].x[0] + operators[q].m[1][1] * points[p].x[1] + operators[q].m[1][2] * points[p].x[2] + operators[q].v[1];
        y[2] = operators[q].m[2][0] * points[p].x[0] + operators[q].m[2][1] * points[p].x[1] + operators[q].m[2][2] * points[p].x[2] + operators[q].v[2];
        points[p].x[0] = y[0];
        points[p].x[1] = y[1];
        points[p].x[2] = y[2];
      }
    }
  }
}

int main() {

  const int n = 16;
  const int m = 4;

  struct point_t * points;
  struct operator_t * operators;

  {
    points = create_points(n);
    operators = create_operators(1);

    kernel_0(n, points, operators[0]);

    free(points);
    free(operators);
  }

  {
    points = create_points(n);
    operators = create_operators(m);

    kernel_1(n, m, points, operators);

    free(points);
    free(operators);
  }

  return 0;
}

