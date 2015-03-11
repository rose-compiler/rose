
struct elem_t {
    int num_edges;
    struct { int x; int y; } * edges;
};

void count_adjacent_edges(
    int n, int m,
    struct elem_t ** a, int ** count
) {
    for (int i = 0; i < n; i++)
      for (int j = 0; j < m; j++)
        for (int c = 0; c < a[i][j].num_edges; c++)
          count[a[i][j].edges[c].x][a[i][j].edges[c].y]++;
}

int main () {
    int n = 10;
    int m = 10;
    struct elem_t * tmp_arr = malloc(n*m*sizeof(struct elem_t));
    struct elem_t ** arr = malloc(n*sizeof(struct elem_t*));
    for (int i = 0; i < n; i++)
        arr[i] = &(tmp_arr[i*m]);

    int * tmp_count = malloc(n*m*sizeof(int));
    int ** count = malloc(n*sizeof(int*));
    for (int i = 0; i < n; i++) {
        count[i] = &(tmp_count[i*m]);
        for (int j = 0; j < m; j++) {
          count[i][j] = 0;
        }
    }

    count_adjacent_edges(n, m, arr, count);

    return 0;
}

