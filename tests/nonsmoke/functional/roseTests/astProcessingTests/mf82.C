int haveagoodday(int x, int y) {
    if (x == 1) {
        return 0;
    }
    else if (y == 1) {
        return 1;
    }
    else {
        return 2;
    }
}

int main() {
    //int q,w;
    int q = 2;
    int w = 3;
    int z = haveagoodday(q,w);
    return 0;
}
