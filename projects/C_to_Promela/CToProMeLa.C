#include "rose.h"
#include "CToProMeLa.h"
#include <iostream>
using namespace std;

bool CToProMeLa::evaluateInheritedAttribute(SgNode * node, bool inher)
{
	return inher;
}

bool CToProMeLa::evaluateSynthesizedAttribute(SgNode * node,bool inher,SubTreeSynthesizedAttributes atts)
{
	if (isSgStatement(node))
	{
//		cout <<"Unparsing statement "<<node->class_name()<<":\n";
//		cout << (node->unparseToCompleteString ())<<endl;
	}
	return true;
}
bool CToProMeLa::defaultSynthesizedAttribute(bool inh)
{
	//std::cout<<"DEFAULT SYNTHESIZED ATTRIBUTE CONSTRUCTOR CALLED\n";     
	return inh;   
}

