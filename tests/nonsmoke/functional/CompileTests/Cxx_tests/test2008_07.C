class A
{
public:
    enum a { ADD, MULT };
    int perform(int a, int b)
    {
        if (op == ADD)
            return a + b;
        else
            return a * b;
    }
    enum a op;
};

