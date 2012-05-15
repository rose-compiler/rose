int cow(int y) {
    if (y == 0) {
    return y;
    }
    else {
    return 1;
    }
}

int moo(int x) {
    return cow(x);
}


int main() {
    int x = 0;
    x = cow(moo(x)); 
    return x;
}
