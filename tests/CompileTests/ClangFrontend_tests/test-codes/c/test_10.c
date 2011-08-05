
int f(int a);

void g() {

    int a = f(2);
    int b = f(a);
    int c = f(f(2));

}

