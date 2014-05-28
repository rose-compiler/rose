int bar()
{
    return -1;
}

void foo()
{
    throw;
    throw "hello";
    try {
        throw bar();
    }
    catch(int val) {
        return;
    }
}
