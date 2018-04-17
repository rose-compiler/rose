int fake(int x) {
    int y = 2;
    return y;
}

int main() {
    int x = 3;
    int y = 4;
    x = fake(x);
    int t;
    if (y > 2) {
        t = 1;
    }
    else {
        t = 2;
    }
    return t;
}
    
