void configError(){}
void outOfMemory(){}
void ioError(){}
void panic(char *f){}
int bzerr;
void Foo()
{
    if(0)
        goto Label;
Label:
    outOfMemory();
}

int main(){
    return 0;
}

