#include "boolPlus.h"

boolPlus boolPlus::Unset(boolPlus::UnsetVal);
boolPlus boolPlus::False(boolPlus::FalseVal);
boolPlus boolPlus::True(boolPlus::TrueVal);
	

/*int boolPlus::UnsetVal;
int boolPlus::FalseVal;
int boolPlus::TrueVal;*/
		
boolPlus::boolPlus() { val = UnsetVal; }

boolPlus::boolPlus(bool initVal)
{
	val = (initVal? TrueVal: FalseVal);
}

boolPlus::boolPlus(int initVal)
{
	val = initVal;
}

boolPlus::boolPlus(const boolPlus &initVal)
{
	val = initVal.val;
}
	
void boolPlus::set(int newVal)
{
	val = newVal;
}

void boolPlus::set(bool newVal)
{
	val = newVal? TrueVal: FalseVal;
}

bool boolPlus::isUnset() const
{
	return val==UnsetVal;
}

bool boolPlus::isFalse() const
{
	return val==FalseVal;
}

bool boolPlus::isTrue() const
{
	return val==TrueVal;
}

bool boolPlus::operator == (const boolPlus &that) const
{
	return val == that.val;
}

bool boolPlus::operator == (const bool &that) const
{
	boolPlus thatVal(that);
	return (*this) == thatVal;
}

bool boolPlus::operator != (const boolPlus &that) const
{
	return !(*this == that);
}

bool boolPlus::operator != (const bool &that) const
{
	return !(*this == that);
}

bool boolPlus::operator < (const boolPlus &that) const
{
	if(val == that.val)
		return false;
	
	if(val==UnsetVal) return true;
	if(that.val==UnsetVal) return false;
	if(val==false) return true;
	if(that.val==false) return false;
	
	ROSE_ASSERT(!"ERROR: impossible case for boolPlus::<!");
}
bool boolPlus::operator < (const bool &that) const
{
	boolPlus thatVal(that);
	return (*this) < thatVal;
}

bool boolPlus::operator > (const boolPlus &that) const
{
	return !(*this < that);
}
bool boolPlus::operator > (const bool &that) const
{
	return !(*this < that);
}

bool boolPlus::operator >= (const boolPlus &that) const
{
	return (*this > that) || (*this == that);
}
bool boolPlus::operator >= (const bool &that) const
{
	return (*this > that) || (*this == that);
}

bool boolPlus::operator <= (const boolPlus &that) const
{
	return (*this < that) || (*this == that);
}
bool boolPlus::operator <= (const bool &that) const
{
	return (*this < that) || (*this == that);
}

boolPlus boolPlus::operator && (const boolPlus &that) const
{
	boolPlus res;
	if(*this <= that) res.val = val;
	else if(that <= *this) res.val = that.val;
	else ROSE_ASSERT(!"ERROR in boolPlus::&&. Impossible case!");
	
	return res;
}
boolPlus boolPlus::operator && (const bool &that) const
{
	boolPlus thatVal(that);
	return (*this) && thatVal;
}

boolPlus boolPlus::operator || (const boolPlus &that) const
{
	boolPlus res;
	if(*this >= that) res.val = val;
	else if(that >= *this) res.val = that.val;
	else ROSE_ASSERT(!"ERROR in boolPlus::&&. Impossible case!");
	
	return res;
}
bool boolPlus::operator || (const bool &that) const
{
	boolPlus thatVal(that);
	boolPlus ret = (*this) || thatVal;
	ROSE_ASSERT(ret.val != UnsetVal);
	
	if(ret.val == TrueVal) return true;
	else return false;
}

std::string boolPlus::str()
{
	return (val==UnsetVal? "Unset":
	       (val==FalseVal? "False":
	       (val==TrueVal? "True": "???"
	       )));
}
