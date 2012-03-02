/***************************************************************
 * ARAL Intermediate Representation                            *
 * Author: Markus Schordan, 2009.                              *
 * (C) Markus Schordan                                         *
 ***************************************************************/

#include "aralir.h"
#include <cstdlib>

/* Implementation */

Aral::AralFile* Aral::Translator::frontEnd() { 
	aralparse(); 
	if(aralIrRoot) 
		return dynamic_cast<AralFile*>(aralIrRoot);
	else 
		return 0;
	}
std::string Aral::Translator::backEnd(AralFile* r) {
	return r->toString(); 
} 


/* Constructors */
Aral::Data::Data():_parent(0) {}

Aral::Int32::Int32(int v) {
	_value=v;
}

Aral::Label::Label(long label):_label(label) {}

Aral::Set::Set() {}

Aral::List::List() {}

Aral::MapList::MapList() {}

Aral::Map::Map():_defaultElement(0) {}

Aral::InfoElementList::InfoElementList() {}

Aral::ResultSection::ResultSection(std::string name,Data* type,AnnotationDataList* data):
	    _name(name),
	    _annotationDataList(data) {}

Aral::AnnotationDataList::AnnotationDataList() {}

Aral::Tuple::Tuple(int size) {
	for(int i=0;i<size;i++) {
		push_back(0);
	}
}
Aral::MapPair::MapPair():Tuple(2) {}

Aral::TopElement::TopElement() {}
Aral::BotElement::BotElement() {}

Aral::BinaryOperator::BinaryOperator(std::string op):_operator(op),_left(0),_right(0) {}
Aral::BinaryOperator::BinaryOperator(std::string op, Data* l, Data* r):_operator(op),_left(l),_right(r) {}

Aral::UnaryOperator::UnaryOperator(std::string op):_operator(op),_operand(0) {}
Aral::UnaryOperator::UnaryOperator(std::string op, Data* operand):_operator(op),_operand(operand) {}

Aral::Constraint::Constraint(Data* d):_expr(d) {}

Aral::AralFile::AralFile(Data* configData,MapList* mapList, ResultSectionList* resultSectionList):
			_configData(configData),
			_mapList(mapList),
			_resultSectionList(resultSectionList) {}


/* Destructors (All destructors are virtual) */
Aral::Int32::~Int32() {
}
Aral::ResultSection::~ResultSection() {
}

Aral::Set::~Set() {
	for(Aral::Set::iterator i=begin();i!=end();i++) {
		delete *i;
	}
}

Aral::Map::~Map() {
	if(_defaultElement) delete _defaultElement;
	for(Aral::Map::iterator i=begin();i!=end();i++) {
		delete *i;
	}
}
Aral::List::~List() {
	for(Aral::List::iterator i=begin();i!=end();i++) {
		delete *i;
	}
}

Aral::MapList::~MapList() {
	for(Aral::MapList::iterator i=begin();i!=end();i++) {
		delete *i;
	}
}

Aral::InfoElementList::~InfoElementList() {
	for(Aral::InfoElementList::iterator i=begin();i!=end();i++) {
		delete *i;
	}
}

Aral::ResultSectionList::~ResultSectionList() {
	for(Aral::ResultSectionList::iterator i=begin();i!=end();i++) {
		delete *i;
	}
}

Aral::AnnotationDataList::~AnnotationDataList() {
	for(Aral::List::iterator i=begin();i!=end();i++) {
		delete *i;
	}
}

Aral::Tuple::~Tuple() {
	for(Aral::Tuple::iterator i=begin();i!=end();i++) {
		delete *i;
	}
}
Aral::MapPair::~MapPair() { 
	assert(size()==2);
	for(Aral::Tuple::iterator i=begin();i!=end();i++) {
		delete *i;
	}
}

Aral::VarId::~VarId() {}
Aral::ExpId::~ExpId() {}
Aral::Label::~Label() {}
Aral::Ident::~Ident() {}
Aral::String::~String() {}
Aral::TopElement::~TopElement() {}
Aral::BotElement::~BotElement() {}
Aral::BinaryOperator::~BinaryOperator() { 
	if(_left) delete _left;
	if(_right) delete _right;
}
Aral::UnaryOperator::~UnaryOperator() { 
	if(_operand) delete _operand;
}
Aral::Constraint::~Constraint() {
	assert(_expr);
	delete _expr;
}

Aral::AnnotationData::~AnnotationData() {
	delete _locationSpecifier;
	delete _infoElementList;
}

Aral::AralFile::~AralFile() {
	if(_configData) delete _configData;
	if(_mapList) delete _mapList;
	if(_resultSectionList) delete _resultSectionList;
}

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
long Aral::Label::getNumber() {
        return _label; 
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

void Aral::Map::add(Aral::MapPair* t) {
	// we call the parent's class set add method.
	// since MapPair is a subclass of Tuple this works
	Aral::Set::add(t); 
}
void Aral::Map::add(Aral::Data* d1,Aral::Data* d2) {
	MapPair* t=new MapPair();
	assert(t);
	t->setAt(0,d1);
	t->setAt(1,d2); 
	this->add(t);
}
Aral::Data* Aral::Map::getMappedElement(Aral::Data* d) {
	for(Aral::Set::iterator i=begin(); i!=end();i++) {
		Tuple* tuple=dynamic_cast<Aral::Tuple*>(*i);
		assert(tuple);
		if((tuple)->getAt(0)->isEqual(d)) {
			// element found, return mapped element
			return (tuple)->getAt(1);
		}
	}
	// otherwise use the default-element mapping (this is different to std:map behaviour)
	return getDefaultElement();
}
// returns one domain-element. If there exist more than one, one random element is returned.
// returns 0 if the domain-element is undefined.
Aral::Data* Aral::Map::getDomainElement(Aral::Data* d) { 
	for(Aral::Set::iterator i=begin(); i!=end();i++) {
		Tuple* tuple=dynamic_cast<Aral::Tuple*>(*i);
		assert(tuple);
		if((tuple)->getAt(1)->isEqual(d)) {
			// element found, return mapped element
			return tuple->getAt(0);
		}
	}
	return 0;
}

// returns empty set if the inverse element is undefined.
Aral::Set* Aral::Map::getDomainElementSet(Aral::Data* d) { 
	Set* domain=new Set();
	for(Aral::Set::iterator i=begin(); i!=end();i++) {
		Tuple* tuple=dynamic_cast<Aral::Tuple*>(*i);
		assert(tuple);
		if((tuple)->getAt(1)->isEqual(d)) {
			// element found, return mapped element
			domain->add(tuple->getAt(0));
		}
	}
	return domain;
}
void Aral::Tuple::setAt(Aral::TuplePos pos, Aral::Data* data) {
	data->autoLinkParent(this);
	operator[](pos)=data;
}

/* isEqual */
bool Aral::BinaryOperator::isEqual(Data* d) {
	if(BinaryOperator* op=dynamic_cast<Aral::BinaryOperator*>(d)) {
		return (_operator==op->getOperator() 
			&& _left->isEqual(op->getLeftOperand())
			&& _right->isEqual(op->getRightOperand()));
	} else {
		return false;
	}
}
bool Aral::UnaryOperator::isEqual(Data* d) {
	if(UnaryOperator* op=dynamic_cast<Aral::UnaryOperator*>(d)) {
		return (_operator==op->getOperator() 
			&& _operand->isEqual(op->getOperand()));
	} else {
		return false;
	}
}
bool Aral::Constraint::isEqual(Data* d) {
	if(Constraint* constr=dynamic_cast<Aral::Constraint*>(d)) {
		return getExpr()->isEqual(constr->getExpr());
	} else {
		return false;
	}
}

bool Aral::InfoElement::isEqual(Aral::Data* o) { 
	if(Aral::InfoElement* ord=dynamic_cast<Aral::InfoElement*>(o)) {
		bool contextIsEqual=false;
		Data* d1=getContext();
		Data* od1=ord->getContext();
		// we define equivalence based on the fact
		// whether a context exists for both elements or not
		if(!d1 && !od1)
			contextIsEqual=true;
		if(!d1 && od1 || d1 && !od1)
			contextIsEqual=false;
		if(d1 && od1)
			contextIsEqual=getContext()->isEqual(d1);
	
		Data* od2=ord->getData();
		bool dataIsEqual=getData()->isEqual(od2);
	
		return contextIsEqual && dataIsEqual;
	} else {
		return false;
	}
}
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
bool Aral::Ident::isEqual(Data* o) { 
	if(Aral::Ident* o2=dynamic_cast<Aral::Ident*>(o))
		return _ident==o2->_ident;
	else
		return false;
}
bool Aral::String::isEqual(Data* o) { 
	if(Aral::String* o2=dynamic_cast<Aral::String*>(o))
		return _string==o2->_string;
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
	if(Aral::Map* o2=dynamic_cast<Aral::Map*>(o)) {
		bool equalDefaultElements = _defaultElement->isEqual(o2->getDefaultElement());
		bool equalSets = false;
		if(size()==o2->size()) {
			equalSets=true;
			// sets are sorted by memory address of distinct representant (Aral*)
			for(Aral::Map::iterator i=begin(),j=o2->begin(); i!=end() && j!=o2->end(); i++,j++) {
				if(!(*i)->isEqual(*j))
					equalSets=false;
			}
		} else {
			equalSets=false;
		}
		return equalDefaultElements && equalSets;
	} else {
		return false;
	}
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
bool Aral::AnnotationDataList::isEqual(Data* o) { 
	if(Aral::List* o2=dynamic_cast<Aral::AnnotationDataList*>(o)) {
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
bool Aral::InfoElementList::isEqual(Data* o) { 
	if(Aral::List* o2=dynamic_cast<Aral::InfoElementList*>(o)) {
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
	std::cout << "Aral: Error: Less than not implemented on map.\n";
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
Aral::Data* Aral::ResultSection::deepCopy() {
  // the 0 represents the type which is not implemented yet (and not necessary for using ARAL)
  return new Aral::ResultSection(getName(),0,dynamic_cast<AnnotationDataList*>(getAnnotationDataList()->deepCopy()));
}
Aral::AralFile* Aral::AralFile::deepCopy() {
  	ResultSectionList* rsl=dynamic_cast<ResultSectionList*>(this->getResultSectionList()->deepCopy());
  	assert(rsl);
  	MapList* ml=dynamic_cast<MapList*>(getMapList()->deepCopy());
  	assert(ml);
  	return new Aral::AralFile(getConfigData()->deepCopy(),ml,rsl);
}

Aral::Data* Aral::AnnotationData::deepCopy() { 
  return new AnnotationData(_locationSpecifier,_flowSpecifier,dynamic_cast<InfoElementList*>(_infoElementList->deepCopy()));
};

Aral::Data* Aral::InfoElement::deepCopy() {
  Aral::Data* d1=getContext();
  Aral::Data* d2=getData();
  Aral::Data* d1copy=0;
  if(d1) {
    d1copy=d1->deepCopy();
  }
  assert(d2!=0);
  return new Aral::InfoElement(d1copy,d2->deepCopy());
}

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

Aral::Data* Aral::Map::deepCopy() {
	return deepCopyMap();
}
Aral::Map* Aral::Map::deepCopyMap() {
	Aral::Map* d=new Aral::Map();
	for(Aral::Map::iterator i=begin(); i!=end(); i++) {
		Aral::MapPair* dcopy=dynamic_cast<Aral::MapPair*>((*i)->deepCopy());
		assert(dcopy);
		d->add(dcopy);
	}
	d->setDefaultElement(getDefaultElement()->deepCopy());
	return d;
}

Aral::Data* Aral::BotElement::deepCopy() {
	return new BotElement();
}
Aral::Data* Aral::TopElement::deepCopy() {
	return new TopElement();
}
Aral::Data* Aral::Int32::deepCopy() {
	return new Int32(_value);
}
Aral::Data* Aral::Label::deepCopy() {
	return new Label(_label);
}
Aral::Data* Aral::Ident::deepCopy() {
	return new Ident(_ident);
}
Aral::Data* Aral::String::deepCopy() {
	return new String(_string);
}
Aral::Data* Aral::VarId::deepCopy() {
	return new VarId(_id);
}
Aral::Data* Aral::ExpId::deepCopy() {
	return new ExpId(_id);
}
Aral::Data* Aral::BinaryOperator::deepCopy() {
	return new Aral::BinaryOperator(_operator,_left->deepCopy(),_right->deepCopy());
}
Aral::Data* Aral::UnaryOperator::deepCopy() {
	return new Aral::UnaryOperator(_operator,_operand->deepCopy());
}
Aral::Data* Aral::Constraint::deepCopy() {
	return new Aral::Constraint(_expr->deepCopy());
}

/* accept Methods */
void Aral::AralFile::accept(AbstractDataVisitor& v) {
	v.preVisitAralFile(this);
	if(_configData) _configData->accept(v);
	if(_mapList) _mapList->accept(v);
	if(_resultSectionList) _resultSectionList->accept(v);
	v.postVisitAralFile(this);
}

void Aral::ResultSection::accept(AbstractDataVisitor& v) {
	v.preVisitResultSection(this);
	_annotationDataList->accept(v);
	v.postVisitResultSection(this);
}
void Aral::AnnotationData::accept(AbstractDataVisitor& v) {
	v.preVisitAnnotationData(this);
	_infoElementList->accept(v);
	v.postVisitAnnotationData(this);
}
void Aral::InfoElement::accept(AbstractDataVisitor& v) {
	v.preVisitInfoElement(this);
	Aral::Data* d1=getContext();
	if(d1) {
    		d1->accept(v);
  	}
	v.inVisitInfoElement(this);
	Aral::Data* d2=getData();
  	assert(d2!=0);
	d2->accept(v);	
	v.postVisitInfoElement(this);
}
void Aral::Set::accept(AbstractDataVisitor& v) {
	v.preVisitSet(this);
	for(std::set<Aral::Data*>::iterator i=begin(); i!=end(); i++) {
	if(i!=begin())
			v.inVisitSet(this);					
		(*i)->accept(v);
	}
	v.postVisitSet(this);
}
void Aral::Map::accept(AbstractDataVisitor& v) {
	v.preVisitMap(this);
	for(Aral::Map::iterator i=begin(); i!=end(); i++) {
		if(i!=begin())
			v.inVisitMap(this);					
		(*i)->accept(v);
	}
	v.postVisitMap(this);
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
void Aral::MapPair::accept(AbstractDataVisitor& v) {
	assert(size()==2);
	v.preVisitMapPair(this);
	getAt(0)->accept(v);	
	v.inVisitMapPair(this);
	getAt(1)->accept(v);	
	v.postVisitMapPair(this);
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
void Aral::MapList::accept(AbstractDataVisitor& v) {
	v.preVisitMapList(this);
	for(MapList::iterator i=begin(); i!=end(); i++) {
		if(i!=begin())
			v.inVisitMapList(this);
		(*i)->accept(v);
	}
	v.postVisitMapList(this);
}
void Aral::InfoElementList::accept(AbstractDataVisitor& v) {
	v.preVisitInfoElementList(this);
	for(InfoElementList::iterator i=begin(); i!=end(); i++) {
		if(i!=begin())
			v.inVisitInfoElementList(this);
		(*i)->accept(v);
	}
	v.postVisitInfoElementList(this);
}
void Aral::ResultSectionList::accept(AbstractDataVisitor& v) {
	v.preVisitResultSectionList(this);
	for(ResultSectionList::iterator i=begin(); i!=end(); i++) {
		if(i!=begin())
			v.inVisitResultSectionList(this);
		(*i)->accept(v);
	}
	v.postVisitResultSectionList(this);
}
void Aral::AnnotationDataList::accept(AbstractDataVisitor& v) {
	v.preVisitAnnotationDataList(this);
	for(AnnotationDataList::iterator i=begin(); i!=end(); i++) {
		if(i!=begin())
			v.inVisitAnnotationDataList(this);
		(*i)->accept(v);
	}
	v.postVisitAnnotationDataList(this);
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
void Aral::Label::accept(AbstractDataVisitor& v) {
	v.preVisitLabel(this);
	v.postVisitLabel(this);
}
void Aral::Ident::accept(AbstractDataVisitor& v) {
	v.preVisitIdent(this);
	v.postVisitIdent(this);
}
void Aral::String::accept(AbstractDataVisitor& v) {
	v.preVisitString(this);
	v.postVisitString(this);
}
std::string Aral::Data::toString() {
	Aral::DataToStringVisitor v;
	return v.dataToString(this);
}
void Aral::BinaryOperator::accept(AbstractDataVisitor& v) {
	v.preVisitBinaryOperator(this);
	if(_left) _left->accept(v);
	v.inVisitBinaryOperator(this);
	if(_right) _right->accept(v);
	v.postVisitBinaryOperator(this);
}
void Aral::UnaryOperator::accept(AbstractDataVisitor& v) {
	v.preVisitUnaryOperator(this);
	if(_operand) _operand->accept(v);
	v.postVisitUnaryOperator(this);
}
void Aral::Constraint::accept(AbstractDataVisitor& v) {
	v.preVisitConstraint(this);
	assert(_expr);
	_expr->accept(v);
	v.postVisitConstraint(this);
}


Aral::LocationSpecifier::LocationSpecifier():_specifier(E_PROGRAM_LOCSPEC),_name(""),_label(0){}
Aral::LocationSpecifier::LocationSpecifier(Aral::LocationSpecifier::Specifier spec):_specifier(spec),_name(""),_label(0){}
Aral::LocationSpecifier::LocationSpecifier(Aral::LocationSpecifier::Specifier spec,std::string name):_specifier(spec),_name(name),_label(0){}
Aral::LocationSpecifier::LocationSpecifier(Aral::Label* lab):_specifier(E_LABEL_LOCSPEC),_name(""),_label(lab){}
void Aral::LocationSpecifier::setSpecifier(Aral::LocationSpecifier::Specifier spec) { _specifier=spec; }
Aral::LocationSpecifier::Specifier Aral::LocationSpecifier::getSpecifier() { return _specifier;}
void Aral::LocationSpecifier::setName(std::string s) {_name=s;}
std::string Aral::LocationSpecifier::getName() {return _name;}
Aral::Label* Aral::LocationSpecifier::getLabel() {return _label;}
Aral::LocationSpecifier::~LocationSpecifier() { if(_label) delete _label; }


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
std::string Aral::Ident::identToString() {
	std::stringstream ss;
	ss << _ident;
	return ss.str();
}
std::string Aral::String::stringToString() {
	std::stringstream ss;
	ss << _string;
	return ss.str();
}
std::string Aral::Value::valueToString() {
	std::stringstream ss;
	ss << _value;
	return ss.str();
}
std::string Aral::LocationSpecifier::toString() {
	std::stringstream ss;
	switch(getSpecifier()) {
	case E_PROGRAM_LOCSPEC: ss << "program";break;
	case E_FUNCTION_LOCSPEC: ss << "function(\""<<getName()<<"\")";break;
	case E_FILE_LOCSPEC: ss << "file(\""<<getName()<<"\")";break;
	case E_LABEL_LOCSPEC: assert(_label);ss << getLabel()->toString();break;
	default: assert(0);
	} 
	return ss.str();
}


std::string Aral::AnnotationData::specifiersToString() {
	std::stringstream ss;
	assert(_locationSpecifier);
	ss<<_locationSpecifier->toString();
	ss<<" ";
	switch(_flowSpecifier) {
	case E_PRE: ss<<"pre";break;
	case E_POST: ss<<"post";break;
	case E_NOFLOW: ss<<"noflow";break;
	default:assert(0);
	}
	return ss.str();
}

std::string Aral::BinaryOperator::binaryOperatorToString() { return " "+_operator+" "; }
std::string Aral::UnaryOperator::unaryOperatorToString() { return _operator+" "; }

bool
Aral::BinaryOperator::isConsistentNode() {
	return (_operator!="" && _left!=0 && _right!=0);
}

bool
Aral::UnaryOperator::isConsistentNode() {
	return (_operator!="" && _operand!=0);
}

/* Visitor */
void Aral::EmptyDataVisitor::preVisitAralFile(AralFile* o){}
void Aral::EmptyDataVisitor::postVisitAralFile(AralFile* o){}

void Aral::EmptyDataVisitor::preVisitResultSectionList(ResultSectionList* o){}
void Aral::EmptyDataVisitor::inVisitResultSectionList(ResultSectionList* o){}
void Aral::EmptyDataVisitor::postVisitResultSectionList(ResultSectionList* o){}
void Aral::EmptyDataVisitor::preVisitResultSection(ResultSection* o){}
void Aral::EmptyDataVisitor::postVisitResultSection(ResultSection* o){}

void Aral::EmptyDataVisitor::preVisitMapList(MapList* o){}
void Aral::EmptyDataVisitor::inVisitMapList(MapList* o){}
void Aral::EmptyDataVisitor::postVisitMapList(MapList* o){}
void Aral::EmptyDataVisitor::preVisitAnnotationDataList(AnnotationDataList* o){}
void Aral::EmptyDataVisitor::inVisitAnnotationDataList(AnnotationDataList* o){}
void Aral::EmptyDataVisitor::postVisitAnnotationDataList(AnnotationDataList* o){}
void Aral::EmptyDataVisitor::preVisitMap(Map* o){}
void Aral::EmptyDataVisitor::inVisitMap(Map* o){}
void Aral::EmptyDataVisitor::postVisitMap(Map* o){}

void Aral::EmptyDataVisitor::preVisitAnnotationData(AnnotationData* o){}
void Aral::EmptyDataVisitor::postVisitAnnotationData(AnnotationData* o){}
void Aral::EmptyDataVisitor::preVisitInfoElement(InfoElement* o){}
void Aral::EmptyDataVisitor::inVisitInfoElement(InfoElement* o){}
void Aral::EmptyDataVisitor::postVisitInfoElement(InfoElement* o){}
void Aral::EmptyDataVisitor::preVisitTuple(Tuple* o){}
void Aral::EmptyDataVisitor::inVisitTuple(Tuple* o){}
void Aral::EmptyDataVisitor::postVisitTuple(Tuple* o){}
void Aral::EmptyDataVisitor::preVisitMapPair(MapPair* o){}
void Aral::EmptyDataVisitor::inVisitMapPair(MapPair* o){}
void Aral::EmptyDataVisitor::postVisitMapPair(MapPair* o){}
void Aral::EmptyDataVisitor::preVisitSet(Set* o){}
void Aral::EmptyDataVisitor::inVisitSet(Set* o){}
void Aral::EmptyDataVisitor::postVisitSet(Set* o){}
void Aral::EmptyDataVisitor::preVisitList(List* o){}
void Aral::EmptyDataVisitor::inVisitList(List* o){}
void Aral::EmptyDataVisitor::postVisitList(List* o){}
void Aral::EmptyDataVisitor::preVisitInfoElementList(InfoElementList* o){}
void Aral::EmptyDataVisitor::inVisitInfoElementList(InfoElementList* o){}
void Aral::EmptyDataVisitor::postVisitInfoElementList(InfoElementList* o){}
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
void Aral::EmptyDataVisitor::preVisitLabel(Label* o){}
void Aral::EmptyDataVisitor::postVisitLabel(Label* o){}
void Aral::EmptyDataVisitor::preVisitIdent(Ident* o){}
void Aral::EmptyDataVisitor::postVisitIdent(Ident* o){}
void Aral::EmptyDataVisitor::preVisitString(String* o){}
void Aral::EmptyDataVisitor::postVisitString(String* o){}
void Aral::EmptyDataVisitor::preVisitBinaryOperator(BinaryOperator* o){}
void Aral::EmptyDataVisitor::inVisitBinaryOperator(BinaryOperator* o){}
void Aral::EmptyDataVisitor::postVisitBinaryOperator(BinaryOperator* o){}
void Aral::EmptyDataVisitor::preVisitUnaryOperator(UnaryOperator* o){}
void Aral::EmptyDataVisitor::postVisitUnaryOperator(UnaryOperator* o){}
void Aral::EmptyDataVisitor::preVisitConstraint(Constraint* o){}
void Aral::EmptyDataVisitor::postVisitConstraint(Constraint* o){}

Aral::DataToStringVisitor::DataToStringVisitor():_withinMappingSection(false),s("") {
}
std::string Aral::DataToStringVisitor::dataToString(Aral::Data* o){
	s="";
	o->accept(*this);
	return s;
}
void Aral::DataToStringVisitor::preVisitAralFile(AralFile* o){ s+="ANALYSIS\n";}
void Aral::DataToStringVisitor::postVisitAralFile(AralFile* o){ s+="END\n";}

void Aral::DataToStringVisitor::preVisitResultSectionList(ResultSectionList* o){}
void Aral::DataToStringVisitor::inVisitResultSectionList(ResultSectionList* o){}
void Aral::DataToStringVisitor::postVisitResultSectionList(ResultSectionList* o){}
void Aral::DataToStringVisitor::preVisitResultSection(ResultSection* o){ s+="\nRESULT\nNAME "+o->getName()+"\nTYPE\nDATA\n"; }
void Aral::DataToStringVisitor::postVisitResultSection(ResultSection* o){ s+="END\n";}

void Aral::DataToStringVisitor::preVisitAnnotationData(AnnotationData* o){ s+=o->specifiersToString()+" "; }
void Aral::DataToStringVisitor::postVisitAnnotationData(AnnotationData* o){ s+="\n";}

void Aral::DataToStringVisitor::preVisitInfoElement(InfoElement* o){
  if(Data* d=o->getContext()) // if no context exists the pointer is 0.
    s+="<";
}
void Aral::DataToStringVisitor::inVisitInfoElement(InfoElement* o){
  if(Data* d=o->getContext()) // if no context exists the pointer is 0.
    s+=">";
}
void Aral::DataToStringVisitor::postVisitInfoElement(InfoElement* o){}
void Aral::DataToStringVisitor::preVisitTuple(Tuple* o){ s+="("; }
void Aral::DataToStringVisitor::inVisitTuple(Tuple* o){ s+=","; }
void Aral::DataToStringVisitor::postVisitTuple(Tuple* o){ s+=")"; }
void Aral::DataToStringVisitor::preVisitMapPair(MapPair* o){}
void Aral::DataToStringVisitor::inVisitMapPair(MapPair* o){ s+="->"; }
void Aral::DataToStringVisitor::postVisitMapPair(MapPair* o){}
void Aral::DataToStringVisitor::preVisitSet(Set* o){ s+="{"; }
void Aral::DataToStringVisitor::inVisitSet(Set* o){ s+=","; }
void Aral::DataToStringVisitor::postVisitSet(Set* o){ s+="}"; }
void Aral::DataToStringVisitor::preVisitList(List* o){ s+="["; }
void Aral::DataToStringVisitor::inVisitList(List* o){ s+=","; }
void Aral::DataToStringVisitor::postVisitList(List* o){ s+="]"; }
void Aral::DataToStringVisitor::preVisitInfoElementList(InfoElementList* o){}
void Aral::DataToStringVisitor::inVisitInfoElementList(InfoElementList* o){s+=",\n";}
void Aral::DataToStringVisitor::postVisitInfoElementList(InfoElementList* o){s+=";";}

void Aral::DataToStringVisitor::preVisitMapList(MapList* o){s+="MAPPING\n"; _withinMappingSection=true;}

/* we only have a MapList in the mapping section. In this MapList we have maps that must be separated by ';'. */
void Aral::DataToStringVisitor::inVisitMapList(MapList* o){ s+=";\n"; }
/* we only have a MapList in the mapping section. In this MapList we have maps that must be terminated by ';'. */
void Aral::DataToStringVisitor::postVisitMapList(MapList* o){
	if(o->size()>0)
		s+=";\n"; // only if the MapList has at least one element it must be terminated by ';'.
	_withinMappingSection=false;
}
void Aral::DataToStringVisitor::preVisitAnnotationDataList(AnnotationDataList* o){}
void Aral::DataToStringVisitor::inVisitAnnotationDataList(AnnotationDataList* o){}
void Aral::DataToStringVisitor::postVisitAnnotationDataList(AnnotationDataList* o){}
void Aral::DataToStringVisitor::preVisitMap(Map* o){ 
	/* The type of a map is only printed in the mapping section right before the map-data */
	if(_withinMappingSection) {
		s+="map(string,string):"; // TODO: type info
	}
	s+="{default->" + o->getDefaultElement()->toString();
	if(o->size()>0)
		s+=" \\ "; // we only need the map-separator, if there is at least one element in the map.
}
void Aral::DataToStringVisitor::inVisitMap(Map* o){ s+=","; }
void Aral::DataToStringVisitor::postVisitMap(Map* o){ s+="}"; }

void Aral::DataToStringVisitor::preVisitInt32(Int32* o){ s+= o->valueToString(); }
void Aral::DataToStringVisitor::postVisitInt32(Int32* o){}
void Aral::DataToStringVisitor::preVisitTopElement(TopElement* o){ s+="top"; }
void Aral::DataToStringVisitor::postVisitTopElement(TopElement* o){}
void Aral::DataToStringVisitor::preVisitBotElement(BotElement* o){ s+="bot"; }
void Aral::DataToStringVisitor::postVisitBotElement(BotElement* o){}
void Aral::DataToStringVisitor::preVisitVarId(VarId* o){ s+="#"+o->idToString(); }
void Aral::DataToStringVisitor::postVisitVarId(VarId* o){}
void Aral::DataToStringVisitor::preVisitExpId(ExpId* o){ s+="#"+o->idToString(); }
void Aral::DataToStringVisitor::postVisitExpId(ExpId* o){}
void Aral::DataToStringVisitor::preVisitLabel(Label* o){ s+="@"+o->labelToString(); }
void Aral::DataToStringVisitor::postVisitLabel(Label* o){}
void Aral::DataToStringVisitor::preVisitIdent(Ident* o){ s+=o->identToString(); }
void Aral::DataToStringVisitor::postVisitIdent(Ident* o){}
void Aral::DataToStringVisitor::preVisitString(String* o){ s+="\""+o->stringToString()+"\""; }
void Aral::DataToStringVisitor::postVisitString(String* o){}
void Aral::DataToStringVisitor::preVisitBinaryOperator(BinaryOperator* o){s+="(";}
void Aral::DataToStringVisitor::inVisitBinaryOperator(BinaryOperator* o){s+=o->binaryOperatorToString(); }
void Aral::DataToStringVisitor::postVisitBinaryOperator(BinaryOperator* o){s+=")";}

void Aral::DataToStringVisitor::preVisitUnaryOperator(UnaryOperator* o){s+="("+o->unaryOperatorToString();}
void Aral::DataToStringVisitor::postVisitUnaryOperator(UnaryOperator* o){s+=")";}

void Aral::DataToStringVisitor::preVisitConstraint(Constraint* o){s+="$";}
void Aral::DataToStringVisitor::postVisitConstraint(Constraint* o){s+="$";}
