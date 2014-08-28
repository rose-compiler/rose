int Bar(){return 0;}
int Foo(int i) { return 0;}


int main(){
    int i = 1;
    i = 1;
    while(i < 0)
        i++;
    i = Foo(Bar()) + 5;
    return 0;
}

