void printIntLine(int x) {
  
}
int main() 
{
    int * data;
    goto source;
source:
    data = 0;
    goto sink;
sink:
    printIntLine(*data);
    return 0;
}
