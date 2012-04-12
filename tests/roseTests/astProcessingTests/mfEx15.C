int cow(int y) {
    if (y == 0) {
    return 0;
    }
    else {
    return 1;
    }
}

int moo(int x) {
    int y = cow(x);
    if (y == 0) {
    return cow(x);
    }
    else {
    return 1;
    }
}


int main() {
    int x = 0;
    x = cow(moo(x)); 
    //return x;
}
