struct C_style { // acceptable, all data members are public
    int a, b;
    double d;
};

class WellProtected { // acceptable, no data members are public
public:
    int get_a() const;
    void set_a(int);

    double get_d() const;
    void set_d(double);

protected:
    int a;

private:
    double d;
};

class NoNo { // not acceptable, contains both public and nonpublic data members
public:
    int get_a() const;
    void set_a(int);

    double d;

protected:
    int a;
};
