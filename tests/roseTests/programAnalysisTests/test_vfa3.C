#include <iostream>
class B {        
public:        
     int data;   
     virtual B* createObj();
     virtual void changeData( B& b);
};
B* B::createObj() {        return new B;}
void B::changeData(B &q) {        q.data = 40;}

class C : public B {        
	int Cdata;        
public:        
	void changeData(B &q);
};

void C::changeData(B &q) {        q.data = 50;}

class D : public B {
        public:
        B* createObj();
        void changeData(B &q);
};

B* D::createObj() {
       return new D;
}

void D::changeData(B &q) {

        q.data = 20;
}

class E : public D {
        public:
        B* createObj();
};

B* E::createObj() {
       return new E;
}

int main(void) {
        B q, *r, *b, **p;
        C c;
        p = &r;
        int tt =1;
        if(!tt) {
                b = new D;
                r = b->createObj();
        }else {
                b = new E;
                r = b->createObj();
        }
      (*p)->changeData(q);
      std::cout << q.data <<"\n";
}
