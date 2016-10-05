// operator <?

// originally found in package gambit

// a.ii:4:24: Parse error (state 102) at <?

// ERR-MATCH: Parse error.*at <[?]$

struct S1 {
    friend S1 operator <? (const S1& x, const S1& y)
    {
        return y;
    }
};


S1 operator >? (const S1& x, const S1& y) {
    return x;
}

int main()
{
  S1 a,b;
  
  a <? b;
  a >? b;
  
  return 0;
}
