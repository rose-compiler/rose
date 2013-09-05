void configError(){}
void outOfMemory(){}
void ioError(){}
void panic(char *f){}
int bzerr;
void Foo()
{
    switch (bzerr)
    {
        case (-9):
            configError ();
            break;
        case (-3):
            outOfMemory ();
            break;
        case (-6):
        errhandler_io:
            ioError ();
            break;
        default:
            panic ("compress:unexpected error");
    }
}

int main(){
    return 0;
}

