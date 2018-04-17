// comparing long long to enum value

// originally found in package d4x

// k0005.cc:14:5: error: no viable candidate for function call; arguments:
//   0: long long int &
//   1: enum /*anonymous*/ &
//  original candidates:
//   <init>:1:1: bool ::operator==(<prom_arith> x, <prom_arith> y)

// ERR-MATCH: error: no viable candidate for function call

enum{
    someEnumValue = 42
};

int main()
{
    long long longVar = 42;
    return longVar == someEnumValue;
}


