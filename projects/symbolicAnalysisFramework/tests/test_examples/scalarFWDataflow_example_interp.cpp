int addRet(int a1, int a2);

void addParams(int& a1, int& a2)
{
        a1 = a1+a2; // %2 +1
        if(a1<100) {
                a1 = addRet(a1, a2); // %2 +1
        }
}

int addRet(int a1, int a2)
{
        if(a1<100) {
                addParams(a1, a2); // %2 +1
        }
        return a1+a2; // %2 +1
}

int main(int argc,char **argv)
{
        int x = 1; // 1
        int y = 2; // 2

        for(int z=0; z<10; z++)
                x = addRet(x, y); // %2 +1

        int q = (addRet(x,y) + addRet(x,y)) // %2 + 0
                 * 2; // %4 + 0
        
        return x+q; // %4 + 0
}
