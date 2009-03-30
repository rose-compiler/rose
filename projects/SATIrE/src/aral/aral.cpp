#include "aral.h"

/* Implementation */

/* Constructors */
	Aral::Data::Data():_parent(0) {}
	
Aral::Int32::Int32(int v) {
	_value=v;
}

Aral::Set::Set() {}

Aral::List::List() {}

Aral::Tuple::Tuple(int size) {
	for(int i=0;i<size;i++) {
		push_back(0);
	}
}

Aral::TopElement::TopElement() {}
Aral::BotElement::BotElement() {}

/* Destructors (All destructors are virtual) */
Aral::Int32::~Int32() {
}

Aral::Set::~Set() {
	for(Aral::Set::iterator i=begin();i!=end();i++) {
		delete *i;
	}
}

Aral::List::~List() {
	for(Aral::List::iterator i=begin();i!=end();i++) {
		delete *i;
	}
}

Aral::Tuple::~Tuple() {
	for(Aral::Tuple::iterator i=begin();i!=end();i++) {
		delete *i;
	}
}

Aral::VarId::~VarId() {}
Aral::ExpId::~ExpId() {}
Aral::StmtLabel::~StmtLabel() {}
Aral::TopElement::~TopElement() {}
Aral::BotElement::~BotElement() {}

/* Special Methods */
Aral::Data* Aral::Data::getParent() {
	return _parent;
}
void Aral::Data::autoLinkParent(Data* parent) {
	if(getParent()!=0) {
		std::cout << "ARAL: ERROR: sharing of data is not allowed @" << toString() <<std::endl;
		exit(1);
	} else {
		_parent=parent;
	}
}

/* Data Read Methods */
int Aral::Int32::getValue() {
	return (int)_value;
}
int Aral::Tuple::getSize() {
	return size();
}
Aral::Data* Aral::Tuple::getAt(Aral::TuplePos pos) {
	return operator[](pos);
}
/* Data Write Methods */
void Aral::List::addFront(Aral::Data* s) {
	push_front(s);
}
void Aral::List::addBack(Aral::Data* s) {
	s->autoLinkParent(this);
	push_back(s);
}
void Aral::Set::add(Aral::Data* s) {
	// first check whether there is a pointer to an equivalent object
	// if not treat it as usual STL set operation
	// otherwise do not add and delete object;
	// to achive properly sorted sets: implement functor DataPtrLess for Data*
	for(std::set<Aral::Data*>::iterator i=begin(); i!=end(); i++) {
		if(s->isEqual(*i)) {
			if(s==*i) {
				std::cout << "ARAL: ERROR: detected external pointer to aral-owned object while adding object to a set." << std::endl;
				exit(1);
			}
			delete s;
			return;
		}
	}
	s->autoLinkParent(this);
	insert(s);
}
void Aral::Map::add(Aral::Data* s,Aral::Data* s2) {
	// first check whether there is a pointer to an equivalent object
	// if not treat it as usual STL set operation
	// otherwise do not add and delete object;
	// to achive properly sorted sets: implement functor DataPtrLess for Data*
	for(std::map<Aral::Data*,Aral::Data*>::iterator i=begin(); i!=end(); i++) {
		if(s->isEqual((*i).first)) {
			if(s==(*i).first) {
				std::cout << "ARAL: ERROR: detected external pointer to aral-owned object while adding object to a map." << std::endl;
				exit(1);
			}
			if(!(s2->isEqual((*i).second))) {
				std::cout << "ARAL: ERROR: attempt of adding same key but different values to a map." << std::endl;
				exit(1);
			}
			delete s;
			return;
		}
	}
	s->autoLinkParent(this);
	s2->autoLinkParent(this);

	//insert(s,s2);
}
void Aral::Tuple::setAt(Aral::TuplePos pos, Aral::Data* data) {
	data->autoLinkParent(this);
	operator[](pos)=data;
}

/* isEqual */

bool Aral::Value::isEqual(Data* o) { 
	if(Aral::Value* o2=dynamic_cast<Aral::Value*>(o))
		return _value==o2->_value;
	else
		return false;
}
bool Aral::Label::isEqual(Data* o) { 
	if(Aral::Label* o2=dynamic_cast<Aral::Label*>(o))
		return _label==o2->_label;
	else
		return false;
}
bool Aral::Id::isEqual(Data* o) { 
	if(Aral::Id* o2=dynamic_cast<Aral::Id*>(o))
		return _id==o2->_id;
	else
		return false;
}
bool Aral::Value::isLessThan(Data* o) { 
	if(Aral::Value* o2=dynamic_cast<Aral::Value*>(o))
		return _value<o2->_value;
	else
		return false;
}
bool Aral::Label::isLessThan(Data* o) { 
	if(Aral::Label* o2=dynamic_cast<Aral::Label*>(o))
		return _label<o2->_label;
	else
		return false;
}
bool Aral::Id::isLessThan(Data* o) { 
	if(Aral::Id* o2=dynamic_cast<Aral::Id*>(o))
		return _id<o2->_id;
	else
		return false;
}
bool Aral::Set::isEqual(Aral::Data* o) { 
	if(Aral::Set* o2=dynamic_cast<Aral::Set*>(o)) {
		if(size()==o2->size()) {
			// sets are sorted by memory address of distinct representant (Aral*)
			for(Aral::Set::iterator i=begin(),j=o2->begin(); i!=end() && j!=o2->end(); i++,j++) {
				if(!(*i)->isEqual(*j))
					return false;
			}
			return true;
		}
	}
	return false;
}
bool Aral::Map::isEqual(Aral::Data* o) { 
#if 0
	if(Aral::Map* o2=dynamic_cast<Aral::Map*>(o)) {
		if(size()==o2->size()) {
			// maps are sorted by memory address of distinct representant (Aral*)
			for(Aral::Map::iterator i=begin(),j=o2->begin(); i!=end() && j!=o2->end(); i++,j++) {
				if(!(*i).first->isEqual((*j).first))
					return false;
			}
			return true;
		}
	}
	return false;
#endif
	std::cout << "Aral:Error:Map: Not working with g++ yet.\n";
	exit(1);
}
bool Aral::List::isEqual(Data* o) { 
	if(Aral::List* o2=dynamic_cast<Aral::List*>(o)) {
		if(size()==o2->size()) {
			for(Aral::List::iterator i=begin(),j=o2->begin(); i!=end() && j!=o2->end(); i++,j++) {
				if(!(*i)->isEqual(*j))
					return false;
			}
			return true;
		}
	}
	return false;
}
bool Aral::Tuple::isEqual(Data* o) { 
	if(Aral::Tuple* o2=dynamic_cast<Aral::Tuple*>(o)) {
		if(size()==o2->size()) {
			for(Aral::Tuple::iterator i=begin(),j=o2->begin(); i!=end() && j!=o2->end(); i++,j++) {
				if(!(*i)->isEqual(*j))
					return false;
			}
			return true;
		}
	}
	return false;
}
bool Aral::Set::isLessThan(Aral::Data* o) { 
	if(Aral::Set* o2=dynamic_cast<Aral::Set*>(o)) {
			for(Aral::Set::iterator i=begin(),j=o2->begin(); i!=end() && j!=o2->end(); i++,j++) {
				if((*i)->isLessThan(*j))
					return true;
			}
			return false;
	}
	std::cout << "Aral: Error: set comparison between non-compariable types.\n";
	exit(1);
}
bool Aral::Map::isLessThan(Aral::Data* o) { 
#if 0
	if(Aral::Map* o2=dynamic_cast<Aral::Map*>(o)) {
			for(Aral::Map::iterator i=begin(),j=o2->begin(); i!=end() && j!=o2->end(); i++,j++) {
				if((*i).first->isLessThan((*j).first)) // we simply compare the keys to define *some* order
					return true;
			}
			return false;
	}
#endif
	std::cout << "Aral: Error: not working with g++ yet.\n";
	//std::cout << "Aral: Error: map comparison between non-compariable types.\n";
	exit(1);
}
bool Aral::List::isLessThan(Data* o) { 
	if(Aral::List* o2=dynamic_cast<Aral::List*>(o)) {
			for(Aral::List::iterator i=begin(),j=o2->begin(); i!=end() && j!=o2->end(); i++,j++) {
				if((*i)->isLessThan(*j))
					return true;
			}
			return false;
	}
	std::cout << "Aral: Error: list comparison between non-compariable types.\n";
	exit(1);
}
bool Aral::Tuple::isLessThan(Data* o) { 
	if(Aral::Tuple* o2=dynamic_cast<Aral::Tuple*>(o)) {
			for(Aral::Tuple::iterator i=begin(),j=o2->begin(); i!=end() && j!=o2->end(); i++,j++) {
				if((*i)->isLessThan(*j))
					return true;
			}
			return false;
	}
	std::cout << "Aral: Error: tuple comparison between non-compariable types.\n";
	exit(1);
}
bool Aral::TopElement::isEqual(Data* o) { 
	if(Aral::TopElement* o2=dynamic_cast<Aral::TopElement*>(o))
		return true;
	else
		return false;
}
bool Aral::BotElement::isEqual(Data* o) { 
	if(Aral::BotElement* o2=dynamic_cast<Aral::BotElement*>(o))
		return true;
	else
		return false;
}
bool Aral::TopElement::isLessThan(Data* o) { 
	return false;
}
bool Aral::BotElement::isLessThan(Data* o) { 
	if(Aral::BotElement* o2=dynamic_cast<Aral::BotElement*>(o))
		return false;
	else
		return true;
}


/* data deepCopy Methods */

Aral::Data* Aral::Tuple::deepCopy() {
	int n=this->getSize();
	Aral::Tuple* d=new Aral::Tuple(n);
	for(int i=0; i!=n; i++) {
		Data* dcopy=this->getAt(i)->deepCopy();
		d->setAt(i,dcopy);
	}
	return d;
}
Aral::Data* Aral::List::deepCopy() {
	return deepCopyList();
}
Aral::List* Aral::List::deepCopyList() {
	Aral::List* d=new Aral::List();
	for(Aral::List::iterator i=begin(); i!=end(); i++) {
		Data* dcopy=(*i)->deepCopy();
		d->addBack(dcopy);
	}
	return d;
}
Aral::Data* Aral::Set::deepCopy() {
	return deepCopySet();
}
Aral::Set* Aral::Set::deepCopySet() {
	Aral::Set* d=new Aral::Set();
	for(Aral::Set::iterator i=begin(); i!=end(); i++) {
		Data* dcopy=(*i)->deepCopy();
		d->add(dcopy);
	}
	return d;
}
Aral::Data* Aral::BotElement::deepCopy() {
	return new BotElement();
}
Aral::Data* Aral::TopElement::deepCopy() {
	return new BotElement();
}
Aral::Data* Aral::Int32::deepCopy() {
	return new Int32(_value);
}
Aral::Data* Aral::StmtLabel::deepCopy() {
	return new StmtLabel(_label);
}
Aral::Data* Aral::VarId::deepCopy() {
	return new VarId(_id);
}
Aral::Data* Aral::ExpId::deepCopy() {
	return new ExpId(_id);
}
/* accept Methods */
void Aral::Set::accept(AbstractDataVisitor& v) {
	v.preVisitSet(this);
	for(std::set<Aral::Data*>::iterator i=begin(); i!=end(); i++) {
		if(i!=begin())
			v.inVisitSet(this);					
		(*i)->accept(v);
	}
	v.postVisitSet(this);
}
void Aral::Tuple::accept(AbstractDataVisitor& v) {
	v.preVisitTuple(this);
	for(std::vector<Aral::Data*>::iterator i=begin(); i!=end(); i++) {
		if(i!=begin())
			v.inVisitTuple(this);
		(*i)->accept(v);
	}
	v.postVisitTuple(this);
}
void Aral::List::accept(AbstractDataVisitor& v) {
	v.preVisitList(this);
	for(std::list<Aral::Data*>::iterator i=begin(); i!=end(); i++) {
		if(i!=begin())
			v.inVisitList(this);					
		(*i)->accept(v);
	}
	v.postVisitList(this);
}
void Aral::Int32::accept(AbstractDataVisitor& v) {
	v.preVisitInt32(this);
	v.postVisitInt32(this);
}
void Aral::TopElement::accept(AbstractDataVisitor& v) {
	v.preVisitTopElement(this);
	v.postVisitTopElement(this);
}
void Aral::BotElement::accept(AbstractDataVisitor& v) {
	v.preVisitBotElement(this);
	v.postVisitBotElement(this);
}
void Aral::VarId::accept(AbstractDataVisitor& v) {
	v.preVisitVarId(this);
	v.postVisitVarId(this);
}
void Aral::ExpId::accept(AbstractDataVisitor& v) {
	v.preVisitExpId(this);
	v.postVisitExpId(this);
}
void Aral::StmtLabel::accept(AbstractDataVisitor& v) {
	v.preVisitStmtLabel(this);
	v.postVisitStmtLabel(this);
}
std::string Aral::Data::toString() {
	Aral::DataToStringVisitor v;
	return v.dataToString(this);
}

/* *XXX*toString auxiliary Functions for some functions */
std::string Aral::Id::idToString() {
	std::stringstream ss;
	ss << _id;
	return ss.str();
}
std::string Aral::Label::labelToString() {
	std::stringstream ss;
	ss << _label;
	return ss.str();
}
std::string Aral::Value::valueToString() {
	std::stringstream ss;
	ss << _value;
	return ss.str();
}

/* Visitor */
void Aral::EmptyDataVisitor::preVisitTuple(Tuple* o){}
void Aral::EmptyDataVisitor::inVisitTuple(Tuple* o){}
void Aral::EmptyDataVisitor::postVisitTuple(Tuple* o){}
void Aral::EmptyDataVisitor::preVisitSet(Set* o){}
void Aral::EmptyDataVisitor::inVisitSet(Set* o){}
void Aral::EmptyDataVisitor::postVisitSet(Set* o){}
void Aral::EmptyDataVisitor::preVisitList(List* o){}
void Aral::EmptyDataVisitor::inVisitList(List* o){}
void Aral::EmptyDataVisitor::postVisitList(List* o){}
void Aral::EmptyDataVisitor::preVisitMap(Map* o){}
void Aral::EmptyDataVisitor::inVisitMap(Map* o){}
void Aral::EmptyDataVisitor::postVisitMap(Map* o){}
void Aral::EmptyDataVisitor::preVisitInt32(Int32* o){}
void Aral::EmptyDataVisitor::postVisitInt32(Int32* o){}
void Aral::EmptyDataVisitor::preVisitTopElement(TopElement* o){}
void Aral::EmptyDataVisitor::postVisitTopElement(TopElement* o){}
void Aral::EmptyDataVisitor::preVisitBotElement(BotElement* o){}
void Aral::EmptyDataVisitor::postVisitBotElement(BotElement* o){}
void Aral::EmptyDataVisitor::preVisitVarId(VarId* o){}
void Aral::EmptyDataVisitor::postVisitVarId(VarId* o){}
void Aral::EmptyDataVisitor::preVisitExpId(ExpId* o){}
void Aral::EmptyDataVisitor::postVisitExpId(ExpId* o){}
void Aral::EmptyDataVisitor::preVisitStmtLabel(StmtLabel* o){}
void Aral::EmptyDataVisitor::postVisitStmtLabel(StmtLabel* o){}

std::string Aral::DataToStringVisitor::dataToString(Aral::Data* o){
	s="";
	o->accept(*this);
	return s;
}
void Aral::DataToStringVisitor::preVisitTuple(Tuple* o){ s+="("; }
void Aral::DataToStringVisitor::inVisitTuple(Tuple* o){ s+=","; }
void Aral::DataToStringVisitor::postVisitTuple(Tuple* o){ s+=")"; }
void Aral::DataToStringVisitor::preVisitSet(Set* o){ s+="{"; }
void Aral::DataToStringVisitor::inVisitSet(Set* o){ s+=","; }
void Aral::DataToStringVisitor::postVisitSet(Set* o){ s+="}"; }
void Aral::DataToStringVisitor::preVisitList(List* o){ s+="["; }
void Aral::DataToStringVisitor::inVisitList(List* o){ s+=","; }
void Aral::DataToStringVisitor::postVisitList(List* o){ s+="]"; }
void Aral::DataToStringVisitor::preVisitMap(Map* o){ s+="map("; }
void Aral::DataToStringVisitor::inVisitMap(Map* o){ s+=","; }
void Aral::DataToStringVisitor::postVisitMap(Map* o){ s+=")"; }
void Aral::DataToStringVisitor::preVisitInt32(Int32* o){ s+= o->valueToString(); }
void Aral::DataToStringVisitor::postVisitInt32(Int32* o){}
void Aral::DataToStringVisitor::preVisitTopElement(TopElement* o){ s+="top"; }
void Aral::DataToStringVisitor::postVisitTopElement(TopElement* o){}
void Aral::DataToStringVisitor::preVisitBotElement(BotElement* o){ s+="bot"; }
void Aral::DataToStringVisitor::postVisitBotElement(BotElement* o){}
void Aral::DataToStringVisitor::preVisitVarId(VarId* o){ s+=o->idToString(); }
void Aral::DataToStringVisitor::postVisitVarId(VarId* o){}
void Aral::DataToStringVisitor::preVisitExpId(ExpId* o){ s+=o->idToString(); }
void Aral::DataToStringVisitor::postVisitExpId(ExpId* o){}
void Aral::DataToStringVisitor::preVisitStmtLabel(StmtLabel* o){ s+=o->labelToString(); }
void Aral::DataToStringVisitor::postVisitStmtLabel(StmtLabel* o){}
