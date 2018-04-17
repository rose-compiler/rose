int main() {
    int k = 0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            if (k % 2 == 0) {
            k = i + j;
            }
            if (k - 2 == 0) {
            k++;
            }
        }
    }
    return 0;
}
