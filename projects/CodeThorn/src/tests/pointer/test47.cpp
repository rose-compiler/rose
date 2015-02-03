// test to skip templates

template <class S>
S myfunc(S s) {
    return s;
}

template <class T>
class TmplClass
{
public:
    T data;
    TmplClass(T _data) : data(_data) { }
    T* getData() {
        T t = myfunc<T>(data);
        return &data;
    }
};

template <>
class TmplClass <char>
{
public:
    char data[10];
    TmplClass(char (&_data)[10]) { 
        for(int i=0; i < sizeof(data); ++i)
            data[i] = _data[i];
    }
};

// copied from cplusplus.com
template <class T>
T* GetMax (T a, T b) {
    T result;
    result = (a>b)? a : b;
    return (&result);
}

int main()
{
    int i = 10, j = -1;
    int* result = GetMax<int>(i, j);
    TmplClass<int> tmplClass(4);
    tmplClass.getData();
    TmplClass<float> tmplFloat(5.0);
    char mydata[] = { "123456789" };
    TmplClass<char> tmplChar (mydata);
    float* tmplData = tmplFloat.getData();
    return 0;
}
