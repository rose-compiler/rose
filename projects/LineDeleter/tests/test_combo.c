static void Foo(){
    int i = 0;
    for(i = 0; i < 100; i++)
        ;
}
int main(){
    int i = 1;
    if( i )  {
        i++;
    } else {
        if( i > 100 ) {
            while(i<100) {
                i--;
                i++;
                i--;
                i++;
                if( i )
                    i++;
                else if (i <20 )
                    i++; }
        } else {
            i ++;
        }
    }
    Foo();
    i = 1;
    while(i < 0) i++;
    
    i = 1;
    while(i < 0)
        i++;
    
    return 0;
}

