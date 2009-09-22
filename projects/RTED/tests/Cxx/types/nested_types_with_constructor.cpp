
class Sub {
    public:
        Sub() { }

        int x[ 3 ];
};

class Composite {
    public:
        Composite() { }

        Sub item;
};


int main() {
    Composite c;
    // simple bounds error
    int y = c.item.x[ 3 ];
}
