template<typename... Arguments>
class VariadicTemplate{
private:
    static const unsigned short int size = sizeof...(Arguments);
};

