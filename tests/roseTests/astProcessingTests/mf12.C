int go(int z, int y);

int main() {
    int z = 0;
    int y = 0;
    int x = 0;
    y = go(z,y);
    return 0;
}

int go(int z, int y) {
    if (z == 0) {
        return 1;
    }
    else if (y == 0) {
        return 1;
    }
    else {
        return 0;
    }
}
