#include "rose.h"
#include "iostream"

#include "CToProMeLaASTAttrib.h"

C2PASTAttribute::C2PASTAttribute():convertToPromela(true){
		std::cout<<"STD-CTOR-CALLED"<<std::endl;};
		
C2PASTAttribute::C2PASTAttribute(bool convert):convertToPromela(convert){
		if (!convert) std::cout<<"Setting to Convert to FALSE"<<std::endl;
	}
void C2PASTAttribute::convert()
{
	convertToPromela=true;
}
void C2PASTAttribute::doNotConvert()
{
	convertToPromela=false;
}
	bool C2PASTAttribute::toConvert()
	{
		if (!convertToPromela) std::cout<<"toConvert()=false"<<std::endl;
		return convertToPromela;
	}
	std::string C2PASTAttribute::toString()
	{
		std::cout<<"calling toString"<<std::endl;
		if (convertToPromela) return std::string("true");
		else return std::string("false");
	}
