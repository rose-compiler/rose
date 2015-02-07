
struct elem_t {
  int num_connections;
  struct { int x; int y; } * connections;
};

void bar(int n, int m, struct elem_t ** a, int ** count) {
    for (int i = 0; i < n; i++)
      for (int j = 0; j < m; j++)
        for (int c = 0; c < a[i][j].num_connections; c++)
          count[a[i][j].connections[c].x][a[i][j].connections[c].y]++;
}

