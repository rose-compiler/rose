#define NULL 0

void printIntLine(int p) {
  // do nothing
}

static void goodG2BSink(int * data)
{
    /* POTENTIAL FLAW: Attempt to use data, which may be NULL */
    printIntLine(*data);
}

static void goodG2B()
{
    int * data;
    int tmpData = 5;
    void (*funcPtr) (int *) = goodG2BSink;
    /* FIX: Initialize data */
    {
        data = &tmpData;
    }
    funcPtr(data);
}

/* goodB2G() uses the BadSource with the GoodSink */
static void goodB2GSink(int * data)
{
    /* FIX: Check for NULL before attempting to print data */
    if (data != NULL)
    {
        printIntLine(*data);
    }
    else
    {
      //printLine("data is NULL");
    }
}

static void goodB2G()
{
    int * data;
    void (*funcPtr) (int *) = goodB2GSink;
    /* POTENTIAL FLAW: Set data to NULL */
    data = NULL;
    funcPtr(data);
}

void CWE476_NULL_Pointer_Dereference__int_44_good()
{
    goodG2B();
    goodB2G();
}

int main(int argc, char * argv[])
{
    /* seed randomness */
    CWE476_NULL_Pointer_Dereference__int_44_good();
    return 0;
}
