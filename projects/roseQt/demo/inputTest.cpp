
#include <stdint.h>

#include <iostream>
#include <vector>
#include <cstdlib>
#include <limits>

//#include <boost/random.hpp>
#include <boost/random/linear_congruential.hpp>
//#include <boost/random/minstd_rand.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>

using namespace std;

void flopTest()
{
    float a,b,c,d,e,f,g,h;

    a = 9.9 + 50;
    b = 1.0;
    c = a /b;
    d = a * b - c;
}

void longFunction(long bla);

void longFunction(long);

void unknownFunction();

namespace myNameSpace
{
    class classInNameSpace
    {

    };

    void funcInNamespace() {}

    namespace namespaceInNameSpace
    {
        void test() {};
    }
}

class C1
{
    class InnerClass
    {
        public:
            int f1();
    };

    public:
          void  memberFunc() { }

    protected:
        int memberVar;

};

int C1::InnerClass::f1()
{
    int a=5;
    return a/2;
}

template< typename Engine, typename Distribution >
void randomTest( Engine eng, const Distribution& dist )
{
    typedef typename Distribution::result_type result_type;
    typedef typename boost::variate_generator<Engine&, Distribution> gen_type;

    gen_type gen( eng, dist );

    double res( 0.0 );


    boost::generator_iterator<gen_type> die(&gen);

    const double start( dist.mean() - 4*dist.sigma() );
    const double end( dist.mean() + 4*dist.sigma() );

    //const double num( (end-start)/10.0 + 1 );
    
    int distr[20];// = new double[num];
    double intervall( (end-start)/20.0 );

    std::fill( distr, distr+20, 0.0 );

    for( int i = 0; i < std::numeric_limits<int>::max()/10000; ++i )
    {
        double current_number( *die++ );
        
        res += current_number;

        if( current_number < start+intervall )
        {
            distr[0] += 1;
            continue;
        }
        else if( current_number < start + 2 * intervall )
        {
            distr[1] += 1;
            continue;
        }
        else if( current_number < start + 3 * intervall )
        {
            distr[2] += 1;
            continue;
        }
        else if( current_number < start + 4 * intervall )
        {
            distr[3] += 1;
            continue;
        }
        else if( current_number < start + 5 * intervall )
        {
            distr[4] += 1;
            continue;
        }
        else if( current_number < start + 6 * intervall )
        {
            distr[5] += 1;
            continue;
        }
        else if( current_number < start + 7 * intervall )
        {
            distr[6] += 1;
            continue;
        }
        else if( current_number < start + 8 * intervall )
        {
            distr[7] += 1;
            continue;
        }
        else if( current_number < start + 9 * intervall )
        {
            distr[8] += 1;
            continue;
        }
        else if( current_number < start + 10 * intervall )
        {
            distr[9] += 1;
            continue;
        }
        else if( current_number < start + 11 * intervall )
        {
            distr[10] += 1;
            continue;
        }
        else if( current_number < start + 12 * intervall )
        {
            distr[11] += 1;
            continue;
        }
        else if( current_number < start + 13 * intervall )
        {
            distr[12] += 1;
            continue;
        }
        else if( current_number < start + 14 * intervall )
        {
            distr[13] += 1;
            continue;
        }
        else if( current_number < start + 15 * intervall )
        {
            distr[14] += 1;
            continue;
        }
        else if( current_number < start + 16 * intervall )
        {
            distr[15] += 1;
            continue;
        }
        else if( current_number < start + 17 * intervall )
        {
            distr[16] += 1;
            continue;
        }
        else if( current_number < start + 18 * intervall )
        {
            distr[17] += 1;
            continue;
        }
        else if( current_number < start + 19 * intervall )
        {
            distr[18] += 1;
            continue;
        }
        else /*if( current_number < start + 10 * intervall )*/
        {
            distr[19] += 1;
            continue;
        }

    }

    cout << endl << res << " ";
    std::copy( distr, distr+20, ostream_iterator<double, char>( cout, " " ) );
}

template< typename Generator, typename T >
void randomTest( T seed )
{
    Generator generator/*( seed )*/;

    {
        boost::normal_distribution<> distribution;
        randomTest( generator, distribution );
    }
    std::cout << std::endl;
}

void longFunction(long iterations)
{
	double d=1000;
	for(long int i=0;i<iterations; i++)
	{
	    for(int j=0; j<10; j++)
	    {
	        d=d/2+17;
	    }
	}
	//cout<< "d=" << d << std::endl;
}

void copyVector(vector<int>&a, vector<int>& b)
{
	for(unsigned int i=0; i<a.size(); i++)
	{
		b.push_back(a[i]);
	}
}

void fillVector(vector<int> & v, int size)
{
	v.clear();
	for(int i=0; i<size; i++)
		v.push_back(i);
}

int main(int argc, char**argv)
{
	int size=100;

        cout << std::numeric_limits<int>::max()/10000 << endl;

	/*
	vector<int> v;
	fillVector(v,size);
	vector<int> copy;
	copyVector(v,copy);
	*/

	longFunction(size*1000);

        randomTest<boost::minstd_rand, unsigned int>( 42u );
        /*randomTest<boost::rand48>( (uint64_t)42u );
        randomTest<boost::ecuyer1988>( 42u );
        randomTest<boost::kreutzer1986>( 42u );
        randomTest<boost::hellekalek1995>( 42u );
        randomTest<boost::mt11213b>( 42u );
        randomTest<boost::mt19937>( 42u );
        randomTest<boost::lagged_fibonacci607>( 42u );
        randomTest<boost::lagged_fibonacci1279>( 42u );
        randomTest<boost::lagged_fibonacci2281>( 42u );
        randomTest<boost::lagged_fibonacci3217>( 42u );
        randomTest<boost::lagged_fibonacci4423>( 42u );
        randomTest<boost::lagged_fibonacci9689>( 42u );
        randomTest<boost::lagged_fibonacci19937>( 42u );
        randomTest<boost::lagged_fibonacci23209>( 42u );
        randomTest<boost::lagged_fibonacci44497>( 42u );*/

	return 0;
}
