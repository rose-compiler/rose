class A
{
public:
    enum a { ADD, MULT };
    A(enum a e)
    {
        if (e == ADD)
            op = ADD;
        else
            op = MULT;
    }
    int perform(int a, int b)
    {
        if (op == ADD)
            return a + b;
        else
            return a * b;
    }

protected:
    enum a op;
};

int main()
{
    A adder(A::ADD);
    A *multiplier = new A(A::MULT);

    return adder.perform(3, 7) - multiplier->perform(19, 8);
}
