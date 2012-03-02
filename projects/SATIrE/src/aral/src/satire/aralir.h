#ifndef ARAL_H
#define ARAL_H

/* 
	ARAL Intermediate Represenation
	Author: Markus Schordan, 2009.
	(C) Markus Schordan

	      Supported Data: Set,List,Tuple,Value,Int32,TopElement,BotElement,Label,Label,ProgId,VarId,ExpId;	
	      Data Methods  : getParent,deepCopy,isEqual
		              + specific methods for adding elements
              Iterators : Tuple::iterator, Set::iterator, Map::iterator List::iterator
	      Visitor   : pre/in/post (accept,visitX)
			: AbstractDataVisitor, EmptyDataVisitor, DataToStringVisitor

*/

#include <set>
#include <map>
#include <list>
#include <vector>
#include <sstream>
#include <iostream>
#include <cassert>
#include <cmath>

namespace Aral {

	enum FlowSpecifier {E_PRE,E_POST,E_NOFLOW};
	typedef signed int TuplePos;
	class Data;
	class Container;
	class Set;
	class List;
	class Tuple;
	class MapPair;
	class Map;
	class Int32;
	class TopElement;
	class BotElement;
	class Label;
	class AralFile;

	class LocationSpecifier;
	class InfoElementList;
	class InfoElement;
	class AnnotationData;
	class AnnotationDataList;

	class AbstractDataVisitor;
	class EmptyDataVisitor;
	class ToStringDataVisitor;
	
	class ResultSection;
	class ResultSectionList;

	class UnaryOperator;
	class BinaryOperator;

	class Translator {
	public:
	  static AralFile* frontEnd();
	  static std::string backEnd(AralFile* root);
	};

	class Data {
	public:
		Data();
		virtual std::string toString();
		virtual void accept(Aral::AbstractDataVisitor&)=0;
		virtual Data* deepCopy()=0;
		virtual bool isEqual(Data*)=0;
		virtual bool isLessThan(Data*) { assert(0); }
		void autoLinkParent(Data*);
		Data* getParent();
	private:
		Data* _parent;
		bool _isLifted;
	};

	class ResultSection : public Data {
	public:
		ResultSection(std::string name,Data* type,AnnotationDataList* data);
		void accept(Aral::AbstractDataVisitor&);
		Data* deepCopy();
		bool isEqual(Data*) { return false; }
		std::string getName() { return _name; }
		AnnotationDataList* getAnnotationDataList() { return _annotationDataList; }
		virtual ~ResultSection();
	private:
		std::string _name;
		AnnotationDataList* _annotationDataList;
	};
	class LocationSpecifier {
	public:
		enum Specifier {E_PROGRAM_LOCSPEC,E_FUNCTION_LOCSPEC,E_FILE_LOCSPEC,E_LABEL_LOCSPEC};
		LocationSpecifier();
		LocationSpecifier(Specifier spec);
		LocationSpecifier(Specifier spec,std::string name);
		LocationSpecifier(Aral::Label* lab);
		//LocationSpecifier(Specifier spec,Aral::Label* lab); // removed. use constructor LocationSpecifier(Label*)
		void setSpecifier(Specifier spec);
		Specifier getSpecifier();
		void setName(std::string s);
		std::string getName();
		Aral::Label* getLabel();
		std::string toString();
		~LocationSpecifier();
	protected:
		Specifier _specifier;
		std::string _name;
		Label* _label;
	};


	class AnnotationData : public Data {
	public:
		AnnotationData(LocationSpecifier* scopespec,FlowSpecifier flowspec,InfoElementList* resDataList) {
			_locationSpecifier=scopespec;_flowSpecifier=flowspec; _infoElementList=resDataList;
		}
		~AnnotationData();
		std::string specifiersToString();
		bool isEqual(Aral::Data*) { return false; }
		virtual Data* deepCopy();
		virtual void accept(Aral::AbstractDataVisitor&);
		LocationSpecifier* getLocationSpecifier() { return _locationSpecifier; }
		FlowSpecifier getFlowSpecifier() { return _flowSpecifier; }
		InfoElementList* getInfoElementList() { return _infoElementList; }
	private:
		LocationSpecifier* _locationSpecifier;
		FlowSpecifier _flowSpecifier;
		InfoElementList* _infoElementList;
	};

	class Value : public Data {
	public:
		std::string valueToString();
		bool isEqual(Data*);
	private:
		bool isLessThan(Data*);
	protected:
		int _value;
	};

	class Int32 : public Value {
	public:
		Int32(int);
		virtual ~Int32();
		int getValue();
		void accept(AbstractDataVisitor&);
		Data* deepCopy();
	};

	class TopElement : public Data {
	public:
		TopElement();
		virtual ~TopElement();
		void accept(AbstractDataVisitor&);
		Data* deepCopy();
		bool isEqual(Data*);
		bool isLessThan(Data*);
	};
	class BotElement : public Data {
	public:
		BotElement();
		virtual ~BotElement();
		void accept(AbstractDataVisitor&);
		Data* deepCopy();
		bool isEqual(Data*);
		bool isLessThan(Data*);
	};

	class InfoElement : public Data {
	public:
	  InfoElement(Data* d1,Data* d2) { _context=d1; _data=d2; }
	  Data* getContext() { return _context;}
	  Data* getData() { return _data;}
	  Data* deepCopy();
	  void accept(AbstractDataVisitor& v);
	  bool isEqual(Data*);
	protected:
	  Data* _context;
	  Data* _data;
	};

	class List : public Data, public std::list<Data*> {
	public:
		List();
		virtual ~List();
		void addFront(Aral::Data*);
		void addBack(Aral::Data*);
		void accept(AbstractDataVisitor& v);
		List* deepCopyList();
		Data* deepCopy();
		bool isEqual(Data*);
		bool isLessThan(Data*);
		// V0.9: removeEqualElement(Data&);
	};
	class InfoElementList : public List {
	public:
	  	InfoElementList();
		virtual ~InfoElementList();
		bool isEqual(Data*);
		void accept(AbstractDataVisitor& v);
	};
	class AnnotationDataList : public List {
	public:
	  	AnnotationDataList();
		virtual ~AnnotationDataList();
		bool isEqual(Data*);
		void accept(AbstractDataVisitor& v);
	};
	class ResultSectionList : public List {
	public:
	  	ResultSectionList() {}
		virtual ~ResultSectionList();
		void accept(AbstractDataVisitor& v);
		bool isEqual(Data*) { return false; }
	};
	class MapList : public List {
	public:
		MapList();
		virtual ~MapList();
		void accept(AbstractDataVisitor& v);
		bool isEqual(Data*) { return false; }
	};
	class Tuple : public Data, public std::vector<Data*> {
	public:
		Tuple(int size);
		virtual ~Tuple();
		int getSize();
		void setAt(Aral::TuplePos, Aral::Data*);
		Data* getAt(Aral::TuplePos pos);
		void accept(AbstractDataVisitor& v);
		Data* deepCopy();
		bool isEqual(Data*);
		bool isLessThan(Data*);
	};
	class	MapPair : public Tuple {
	public:
		MapPair();
		virtual ~MapPair();
		void accept(AbstractDataVisitor& v);
	};

	class Id : public Data {
	public:
		Id(int id):_id(id){}
		std::string idToString();
		bool isEqual(Data*);
		bool isLessThan(Data*);
	protected:
		int _id;
	};
	class VarId : public Id {
	public:
		VarId(int varId):Id(varId){}
		virtual ~VarId();
		void accept(AbstractDataVisitor& v);
		Data* deepCopy();
	};
	class ExpId : public Id {
	public:
		ExpId(int expId):Id(expId) {}
		virtual ~ExpId();
		void accept(AbstractDataVisitor& v);
		Data* deepCopy();
	};
	class Label : public Data {
	public:
		Label(long label);
		virtual ~Label();
		std::string labelToString();
	  	long getNumber();
		bool isEqual(Data*);
		bool isLessThan(Data*);
		void accept(AbstractDataVisitor& v);
		Data* deepCopy();
	protected:
		long _label;
	};
	class Ident : public Data {
	public:
		Ident(char* ident):_ident(ident) {}
		virtual ~Ident();
		std::string identToString();
		bool isEqual(Data*);
		void accept(AbstractDataVisitor& v);
		Data* deepCopy();
	protected:
		char* _ident;
	};
	class String : public Data {
	public:
		String(char* s):_string(s) {}
		virtual ~String();
		std::string stringToString();
		bool isEqual(Data*);
		void accept(AbstractDataVisitor& v);
		Data* deepCopy();
	protected:
		char* _string;
	};
	class Set : public Data, public std::set<Data*> {
	public:
		Set();
		virtual ~Set();
		void add(Aral::Data*);
		// V0.9: void removeEqualElement(Aral::Data&);
		// V0.9: bool equalElementIn(Aral::Data&);
		void accept(AbstractDataVisitor& v);
		Data* deepCopy();
		Set* deepCopySet();
		bool isEqual(Data*);
		bool isLessThan(Data*);
	protected:
	};
	class AralFile : public Data {
	public:
		bool isEqual(Data*) { return false; }
		AralFile(Data* configData,MapList* mapList, ResultSectionList* resultSectionList);
		~AralFile();	
		void accept(AbstractDataVisitor&);
		AralFile* deepCopy();
		void setConfigData(Aral::Data* d) { _configData=d; }
		Aral::Data* getConfigData() { return _configData; }
		void setMapList(Aral::MapList* m) { _mapList=m; }
		Aral::MapList* getMapList() { return _mapList; }
		void setResultSectionList(Aral::ResultSectionList* m) { _resultSectionList=m; }
		Aral::ResultSectionList* getResultSectionList() { return _resultSectionList; }
	private:
		Data* _configData;
		MapList* _mapList;
		ResultSectionList* _resultSectionList;
	};

	class Map : public Aral::Set {
	public:
		Map();
		Map(Aral::Data*,Aral::Set*);
		virtual ~Map();
		void add(Aral::Data* d1,Aral::Data* d2);
		void add(Aral::MapPair* t);
		void accept(AbstractDataVisitor& v);
		Data* deepCopy();
		Map* deepCopyMap();
		bool isEqual(Data*);
		bool isLessThan(Data*);
		void setDefaultElement(Aral::Data* d) { _defaultElement=d; }
		Data* getDefaultElement() { return _defaultElement; }
		Data* getMappedElement(Aral::Data* d);
		Data* getDomainElement(Aral::Data* d); // returns 0 if domain element(s) is/are undefined.
		Set* getDomainElementSet(Aral::Data* d); // returns empty set if the domain element(s) is/are undefined.
	private:
		Data* _defaultElement;
		//Aral::Set* _set;
		Set* getSet() { return this; }
	};

	class BinaryOperator: public Data {
	public:
		BinaryOperator(std::string op);
		BinaryOperator(std::string op, Data* l, Data* r);
		virtual ~BinaryOperator();
		void setLeftOperand(Data* l) { _left=l; }
		void setRightOperand(Data* r) { _right=r; }
		std::string getOperator() { return _operator; }
		Data* getLeftOperand() { return _left; }
		Data* getRightOperand() { return _right; }
		Data* deepCopy();
		std::string binaryOperatorToString();
		void accept(AbstractDataVisitor& v);
		bool isEqual(Data*);
	private:
		bool isConsistentNode();
		std::string _operator;
		Aral::Data* _left;
		Aral::Data* _right;
	};

	class UnaryOperator: public Data {
	public:
		UnaryOperator(std::string op);
		UnaryOperator(std::string op, Data* operand);
		virtual ~UnaryOperator();
		void setOperand(Data* l) { _operand=l; }
		std::string getOperator() { return _operator; }
		Data* getOperand() { return _operand; }
		Data* deepCopy();
		std::string unaryOperatorToString();
		void accept(AbstractDataVisitor& v);
		bool isEqual(Data*);
	private:
		bool isConsistentNode();
		std::string _operator;
		Aral::Data* _operand;
	};

	class Constraint : public Data {
	public:
		Constraint(Data*);
		virtual ~Constraint();
		Data* getExpr() { return _expr; }
		Data* deepCopy();
		void accept(AbstractDataVisitor& v);
		bool isEqual(Data*);
	private:
		Data* _expr;
	};

	class AbstractDataVisitor {
	public:
		virtual  void preVisitAralFile(AralFile*)=0;
		virtual  void postVisitAralFile(AralFile*)=0;
		virtual  void preVisitMapList(MapList*)=0;
		virtual  void inVisitMapList(MapList*)=0;
		virtual  void postVisitAnnotationDataList(AnnotationDataList*)=0;
		virtual  void preVisitAnnotationDataList(AnnotationDataList*)=0;
		virtual  void inVisitAnnotationDataList(AnnotationDataList*)=0;
		virtual  void postVisitMapList(MapList*)=0;
		virtual  void preVisitResultSectionList(ResultSectionList*)=0;
		virtual  void inVisitResultSectionList(ResultSectionList*)=0;
		virtual  void postVisitResultSectionList(ResultSectionList*)=0;
		virtual  void preVisitResultSection(ResultSection*)=0;
		virtual  void postVisitResultSection(ResultSection*)=0;
		virtual  void preVisitAnnotationData(AnnotationData*)=0;
		virtual  void postVisitAnnotationData(AnnotationData*)=0;
		virtual  void preVisitInfoElement(InfoElement*)=0;
		virtual  void inVisitInfoElement(InfoElement*)=0;
		virtual  void postVisitInfoElement(InfoElement*)=0;
		virtual  void preVisitTuple(Tuple*)=0;
		virtual  void inVisitTuple(Tuple*)=0;
		virtual  void postVisitTuple(Tuple*)=0;
		virtual  void preVisitMapPair(MapPair*)=0;
		virtual  void inVisitMapPair(MapPair*)=0;
		virtual  void postVisitMapPair(MapPair*)=0;
		virtual  void preVisitSet(Set*)=0;
		virtual  void inVisitSet(Set*)=0;
		virtual  void postVisitSet(Set*)=0;
		virtual  void preVisitList(List*)=0;
		virtual  void inVisitList(List*)=0;
		virtual  void postVisitList(List*)=0;
		virtual  void preVisitInfoElementList(InfoElementList*)=0;
		virtual  void inVisitInfoElementList(InfoElementList*)=0;
		virtual  void postVisitInfoElementList(InfoElementList*)=0;
		virtual  void preVisitMap(Map*)=0;
		virtual  void inVisitMap(Map*)=0;
		virtual  void postVisitMap(Map*)=0;
		virtual  void preVisitInt32(Int32*)=0;
		virtual  void postVisitInt32(Int32*)=0;
		virtual  void preVisitTopElement(TopElement*)=0;
		virtual  void postVisitTopElement(TopElement*)=0;
		virtual  void preVisitBotElement(BotElement*)=0;
		virtual  void postVisitBotElement(BotElement*)=0;
		virtual  void preVisitVarId(VarId*)=0;
		virtual  void postVisitVarId(VarId*)=0;
		virtual  void preVisitExpId(ExpId*)=0;
		virtual  void postVisitExpId(ExpId*)=0;
		virtual  void preVisitLabel(Label*)=0;
		virtual  void postVisitLabel(Label*)=0;
		virtual  void preVisitIdent(Ident*)=0;
		virtual  void postVisitIdent(Ident*)=0;
		virtual  void preVisitString(String*)=0;
		virtual  void postVisitString(String*)=0;
		virtual  void preVisitBinaryOperator(BinaryOperator*)=0;
		virtual  void inVisitBinaryOperator(BinaryOperator*)=0;
		virtual  void postVisitBinaryOperator(BinaryOperator*)=0;

		virtual  void preVisitUnaryOperator(UnaryOperator*)=0;
		virtual  void postVisitUnaryOperator(UnaryOperator*)=0;

		virtual  void preVisitConstraint(Constraint*)=0;
		virtual  void postVisitConstraint(Constraint*)=0;
	};

	class EmptyDataVisitor : public AbstractDataVisitor {
	public:
		virtual  void preVisitAralFile(AralFile*);
		virtual  void postVisitAralFile(AralFile*);
		virtual  void preVisitMapList(MapList*);
		virtual  void inVisitMapList(MapList*);
		virtual  void postVisitMapList(MapList*);
		virtual  void preVisitAnnotationDataList(AnnotationDataList*);
		virtual  void inVisitAnnotationDataList(AnnotationDataList*);
		virtual  void postVisitAnnotationDataList(AnnotationDataList*);
		virtual  void preVisitResultSectionList(ResultSectionList*);
		virtual  void inVisitResultSectionList(ResultSectionList*);
		virtual  void postVisitResultSectionList(ResultSectionList*);
		virtual  void preVisitResultSection(ResultSection*);
		virtual  void postVisitResultSection(ResultSection*);
		virtual  void preVisitAnnotationData(AnnotationData*);
		virtual  void postVisitAnnotationData(AnnotationData*);
		virtual  void preVisitInfoElement(InfoElement*);
		virtual  void inVisitInfoElement(InfoElement*);
		virtual  void postVisitInfoElement(InfoElement*);
		virtual  void preVisitTuple(Tuple*);
		virtual  void inVisitTuple(Tuple*);
		virtual  void postVisitTuple(Tuple*);
		virtual  void preVisitMapPair(MapPair*);
		virtual  void inVisitMapPair(MapPair*);
		virtual  void postVisitMapPair(MapPair*);
		virtual  void preVisitSet(Set*);
		virtual  void inVisitSet(Set*);
		virtual  void postVisitSet(Set*);
		virtual  void preVisitList(List*);
		virtual  void inVisitList(List*);
		virtual  void postVisitList(List*);
		virtual  void preVisitInfoElementList(InfoElementList*);
		virtual  void inVisitInfoElementList(InfoElementList*);
		virtual  void postVisitInfoElementList(InfoElementList*);
		virtual  void preVisitMap(Map*);
		virtual  void inVisitMap(Map*);
		virtual  void postVisitMap(Map*);
		virtual  void preVisitInt32(Int32*);
		virtual  void postVisitInt32(Int32*);
		virtual  void preVisitTopElement(TopElement*);
		virtual  void postVisitTopElement(TopElement*);
		virtual  void preVisitBotElement(BotElement*);
		virtual  void postVisitBotElement(BotElement*);
		virtual  void preVisitVarId(VarId*);
		virtual  void postVisitVarId(VarId*);
		virtual  void preVisitExpId(ExpId*);
		virtual  void postVisitExpId(ExpId*);
		virtual  void preVisitLabel(Label*);
		virtual  void postVisitLabel(Label*);
		virtual  void preVisitIdent(Ident*);
		virtual  void postVisitIdent(Ident*);
		virtual  void preVisitString(String*);
		virtual  void postVisitString(String*);
		virtual  void preVisitBinaryOperator(BinaryOperator*);
		virtual  void inVisitBinaryOperator(BinaryOperator*);
		virtual  void postVisitBinaryOperator(BinaryOperator*);
		virtual  void preVisitUnaryOperator(UnaryOperator*);
		virtual  void postVisitUnaryOperator(UnaryOperator*);
		virtual  void preVisitConstraint(Constraint*);
		virtual  void postVisitConstraint(Constraint*);
	};
	
	class DataToStringVisitor : public EmptyDataVisitor {
	public:
	  	DataToStringVisitor();
		std::string dataToString(Aral::Data*);
		virtual  void preVisitAralFile(AralFile*);
		virtual  void postVisitAralFile(AralFile*);
		virtual  void preVisitMapList(MapList*);
		virtual  void inVisitMapList(MapList*);
		virtual  void postVisitMapList(MapList*);
		virtual  void preVisitAnnotationDataList(AnnotationDataList*);
		virtual  void inVisitAnnotationDataList(AnnotationDataList*);
		virtual  void postVisitAnnotationDataList(AnnotationDataList*);
		virtual  void preVisitResultSectionList(ResultSectionList*);
		virtual  void inVisitResultSectionList(ResultSectionList*);
		virtual  void postVisitResultSectionList(ResultSectionList*);
		virtual  void preVisitResultSection(ResultSection*);
		virtual  void postVisitResultSection(ResultSection*);
		virtual  void preVisitAnnotationData(AnnotationData*);
		virtual  void postVisitAnnotationData(AnnotationData*);
		virtual  void preVisitInfoElement(InfoElement*);
		virtual  void inVisitInfoElement(InfoElement*);
		virtual  void postVisitInfoElement(InfoElement*);
		virtual  void preVisitTuple(Tuple*);
		virtual  void inVisitTuple(Tuple*);
		virtual  void postVisitTuple(Tuple*);
		virtual  void preVisitMapPair(MapPair*);
		virtual  void inVisitMapPair(MapPair*);
		virtual  void postVisitMapPair(MapPair*);
		virtual  void preVisitSet(Set*);
		virtual  void inVisitSet(Set*);
		virtual  void postVisitSet(Set*);
		virtual  void preVisitList(List*);
		virtual  void inVisitList(List*);
		virtual  void postVisitList(List*);
		virtual  void preVisitInfoElementList(InfoElementList*);
		virtual  void inVisitInfoElementList(InfoElementList*);
		virtual  void postVisitInfoElementList(InfoElementList*);
		virtual  void preVisitMap(Map*);
		virtual  void inVisitMap(Map*);
		virtual  void postVisitMap(Map*);
		virtual  void preVisitInt32(Int32*);
		virtual  void postVisitInt32(Int32*);
		virtual  void preVisitTopElement(TopElement*);
		virtual  void postVisitTopElement(TopElement*);
		virtual  void preVisitBotElement(BotElement*);
		virtual  void postVisitBotElement(BotElement*);
		virtual  void preVisitVarId(VarId*);
		virtual  void postVisitVarId(VarId*);
		virtual  void preVisitExpId(ExpId*);
		virtual  void postVisitExpId(ExpId*);
		virtual  void preVisitLabel(Label*);
		virtual  void postVisitLabel(Label*);
		virtual  void preVisitIdent(Ident*);
		virtual  void postVisitIdent(Ident*);
		virtual  void preVisitString(String*);
		virtual  void postVisitString(String*);
		virtual  void preVisitBinaryOperator(BinaryOperator*);
		virtual  void inVisitBinaryOperator(BinaryOperator*);
		virtual  void postVisitBinaryOperator(BinaryOperator*);
		virtual  void preVisitUnaryOperator(UnaryOperator*);
		virtual  void postVisitUnaryOperator(UnaryOperator*);
		virtual  void preVisitConstraint(Constraint*);
		virtual  void postVisitConstraint(Constraint*);
	protected:
		/* necessary for properly unparsing type of maps in mapping-section and
		   data-section of result-section */
		bool _withinMappingSection; 

		std::string s;
	};

class Type;
class ComplexType;
class BasicType;

class Type {
public:
      virtual bool isEqual(Type*)=0;
      virtual std::string toString()=0;
};

class ComplexType : public Type, public std::vector<Type*> {
public:
	ComplexType(std::string name) { _name=name; }
	ComplexType(std::string name,Type* type) { _name=name; push_back(type); }
	bool isEqual(Type* other0) {
	  if(ComplexType* other=dynamic_cast<ComplexType*>(other0)) {
	     if(!(_name==other->_name && size()==other->size()))
	     	return false;
	     else
	        for(std::vector<Type*>::iterator i=begin(),j=other->begin(); i!=end() && j!=other->end();i++,j++)
	     	  if(!(*i)->isEqual(*j))
		    return false;
	  } else
             return false;
     	   return true;
	}
	std::string toString() {
	   std::string s;
	   s+=_name+"(";
	   std::stringstream ss;
	   ss << size();
	   //s+=ss.str();
	   for(std::vector<Type*>::iterator i=begin(); i!=end();i++) {
	     if(i!=begin())
	       s+=",";
	     s+=(*i)->toString();
	   }
	   s+=")";
	   return s;
        }
protected:
	std::string _name;
};

class IntType : public Type {
public:
      IntType(int bits) {
         _start=0;
	 _end=pow(2,bits);
	 _bits=bits;
      }
      bool isEqual(Type* other) { return true; }
      bool isEqual(ComplexType*) { return true; }
      bool isEqual(BasicType*) { return true;}
      bool isValueOfType(long value) {
          return(value>=_start && value <= _end);
      }
      std::string toString() {
	std::stringstream ss;
	ss << _bits;
	return "int("+ss.str()+")";
      }

private:
      double _start;
      double _end;
      int _bits;
};

} // end of namespace Aral

extern int aralparse();
extern Aral::Data* aralIrRoot;

#endif // ARAL
