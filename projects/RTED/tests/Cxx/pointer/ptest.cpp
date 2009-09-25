
class TestClass
{
        int varA;
    public:
        void runA() {
            // this pointer is NULL !!!
            varA=5;
        };
};

TestClass *testclassA;

int main()
{
    // set the pointer testclassA to null
    testclassA=0;

    // invalid acces by using a null pointer
    testclassA->runA();

    return 0;
}
