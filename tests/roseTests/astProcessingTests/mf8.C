int haveagoodday(int x, int y) {
    int ret;
     if (x == 1) {
        ret = 0;
    }

    else if (y == 1) {
        ret =  1;
    }
    else {
        ret =  2;
    }
    return ret;
}

int main() {
    //int q,w;
    int q = 2;
    int w = 3;
    int z = haveagoodday(q,w);
    return 0;
}
