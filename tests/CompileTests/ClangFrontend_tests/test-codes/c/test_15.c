
struct matrix_t {
    float ** data;
    unsigned n;
    unsigned m;
};

int matrix_mul(struct matrix_t * a, struct matrix_t * b, struct matrix_t * res) {
    if (a->m != b->n) return 1;
    if (res->n != a->n || res->m != b->m) return 2;

    int i, j, k;

    for (i = 0; i < a->n; i++) {
        for (j = 0; j < b->m; j++) {
            float t = 0;
            for (k = 0; k < a->m; k++) {
                t += a->data[i][k] * b->data[k][j];
            }
            res->data[i][j] = t;
        }
    }

    return 0;
}
