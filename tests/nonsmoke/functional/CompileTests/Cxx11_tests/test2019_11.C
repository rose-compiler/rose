struct AltStruct {
    AltStruct(int x, double y) : x_{x}, y_{y} {}
 
private:
    int x_;
    double y_;
};
 
AltStruct var2{2, 4.3};
