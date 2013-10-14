

void Foo(){
    1 > 2 ? 3 : 4;
}

void Bar(){
    if(1 && 2) {
        if(5 || 6)
            ;
    } else if( 3 || 4){
    }
}

int main(){
    for(int i = 0 ; i < 100; i ++){
        if(i %2)
            Foo();
        else
            Bar();
    }

    int i = 0;
    goto L;
    {
        L: i ++;
    }
    return 0;

}
