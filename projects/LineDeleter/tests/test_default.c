int Foo(){
    int i = 1;
    switch(i){
        default:
        case 2:
            i=100;
            return i;
    }
    return i;
}

int main(){
    Foo();
    return 0;
}
