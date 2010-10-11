#include <iostream>
#include <map>

using namespace std;

class num
{
	public:
	int n;
	
	num()
	{
		n=0;
	}
	
	num(int n)
	{
		this->n = n;
	}
	
	bool operator < (num& that)
	{
		return n < that.n;
	}
	
	void inc()
	{
		n++;
	}
};

int main()
{
	num zero(0);
	num one(1);
	num two(2);
	num three(3);
	num four(4);
	num five(5);
	map<num, num> m;
	m[zero] = one;
	m[two]  = three;
	m[four] = five;
	for(map<num, num>::iterator it=m.begin(); it!=m.end(); it++)
	{
		it->first.inc();
		it->second.inc();
	}

	for(map<num, num>::iterator it=m.begin(); it!=m.end(); it++)
   {
		cout << it->first.n << " -> " << it->second.n << "\n";
	}
	return 0;
}
