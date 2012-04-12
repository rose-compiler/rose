int bar(int foo) {
    int t;
    if (foo == 1) {
        t = 0;
    }
    else {
        t = 1;
    }
    return t;
}

int main() {
    //int i;
    int i = bar(2);
    if (i == 2) {
        //return 2;
        i = i + 1;
    }
    else {
        i = i - 1;
        //return 0;
    }
    return i;
}
