#include <iostream>
#include "aral.h"


void testAral() {
	using namespace Aral;
	Int32* x=new Int32(100); // Int32* x=parse("100");
	Set* s1=new Set();
	s1->add(x);
	s1->add(new Int32(300)); // Set* s1=parse("{100,200}");
	Set* s2=s1->deepCopySet();
	s2->add(new Int32(200));
	s2->add(new Int32(200)); // test: add same element twice
	Tuple* t1=new Tuple(3);
	t1->setAt(0,s1); // t1[0]=s1;
	t1->setAt(1,new Int32(911)); // t1[1]=Int32(911);
	Tuple* t2=new Tuple(3);
	t2->setAt(0,new Int32(42));
	t2->setAt(1,new BotElement());
	t2->setAt(2,new VarId(17));
	t1->setAt(2,t2); // t1[2]=t2;
	std::cout << "SErgebnis:" << t1->toString() << std::endl;
	DataToStringVisitor v;
	t1->accept(v);
	std::cout << "VErgebnis:" << v.dataToString(t1) << std::endl;
	std::cout << "VErgebnis:s1=" << v.dataToString(s1) << std::endl;
	std::cout << "VErgebnis:s2=" << v.dataToString(s2) << std::endl;

	Data* d2=t1->deepCopy();
	std::cout << "CopyErgebnisD:" << v.dataToString(d2) << std::endl;

	Int32* n1=new Int32(10);
	Int32* n2=new Int32(10);
	std::cout << "EqualTest1:1?:" << (n1->isEqual(n2)) << std::endl;
	std::cout << "EqualTest2:1?:" << (s1->isEqual(s1)) << std::endl;
	std::cout << "EqualTest3:1?:" << (t1->isEqual(d2)) << std::endl;
	std::cout << "UnEqualTest1:0?:" << (n1->isEqual(t1)) << std::endl;
	std::cout << "UnEqualTest1:0?:" << (s1->isEqual(s2)) << std::endl;
	//*n1==*t1;
	delete t1;
	delete d2;
	std::cout << "Destruction successful." << std::endl;
}

int main() {
	testAral();
	return 0;
}
