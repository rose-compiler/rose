#ifndef ARAL_H
#define ARAL_H

/* 
	ARAL Intermediate Represenation
	Author: Markus Schordan, 2009.
	(C) Markus Schordan

	V0.9: Supported Data: Set,List,Tuple,Value,Int32,TopElement,BotElement,Label,Label,ProgId,VarId,ExpId;	
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

namespace Aral {

	enum LocationSpecifier {E_PRE,E_POST,E_NOFLOW};
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
	class Analysis;

	class ScopeSpecifier;
	class ResultDataList;
	class ResultData;
	class Label;
	class DataSectionItem;
	class DataSectionItemList;

	class AbstractDataVisitor;
	class EmptyDataVisitor;
	class ToStringDataVisitor;
	
	class ResultSection;
	class ResultSectionList;

	class InfixOperator;

	class Translator {
	public:
	  static Analysis* frontEnd();
	  static std::string backEnd(Analysis* root);
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
		ResultSection(std::string name,Data* type,Data* data) {
			_name=name;
			_data=data;
		}
		void accept(Aral::AbstractDataVisitor&);
		Data* deepCopy();
		bool isEqual(Data*) { return false; }
		std::string getName() { return _name; }
		Data* getData() { return _data; }
		virtual ~ResultSection();
	private:
		std::string _name;
		Data* _data;
	};
	class DataSectionItem : public Data {
	public:
		DataSectionItem(ScopeSpecifier* scopespec,LocationSpecifier locspec,ResultDataList* resDataList) {
			_scopeSpecifier=scopespec;_locationSpecifier=locspec; _resultDataList=resDataList;
		}
		~DataSectionItem();
		std::string specifiersToString();
		bool isEqual(Aral::Data*) { return false; }
		virtual Data* deepCopy() { assert(0); /* todo */};
		virtual void accept(Aral::AbstractDataVisitor&);
	public: // to be private
		ScopeSpecifier* _scopeSpecifier;
		LocationSpecifier _locationSpecifier;
		ResultDataList* _resultDataList;
	};

	class ScopeSpecifier {
	public:
		enum Specifier {E_PROGRAM_SCOPE,E_FUNCTION_SCOPE,E_FILE_SCOPE,E_LABEL_SCOPE};
		ScopeSpecifier(){ _specifier=E_PROGRAM_SCOPE; _name=""; _label=0;}
		ScopeSpecifier(Specifier spec){ _specifier=spec; _name="";_label=0;}
		ScopeSpecifier(Specifier spec,std::string name){ _specifier=spec; _name=name;_label=0;};
		ScopeSpecifier(Specifier spec,Aral::Label* lab){ _specifier=spec; _name=""; _label=lab;};
		void setSpecifier(Specifier spec) { _specifier=spec; };
		Specifier getSpecifier() { return _specifier;}
		void setName(std::string s) {_name=s;}
		std::string getName() {return _name;}
		Aral::Label* getLabel() {return _label;}
		std::string toString();
		//~ScopeSpecifier() { if(_label) delete _label; }
	protected:
		Specifier _specifier;
		std::string _name;
		Aral::Label* _label;
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

	class ResultData : public Data {
	public:
	  ResultData(Data* d1,Data* d2) { _context=d1; _data=d2; }
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
	class ResultDataList : public List {
	public:
	  	ResultDataList();
		virtual ~ResultDataList();
		bool isEqual(Data*);
		void accept(AbstractDataVisitor& v);
	};
	class DataSectionItemList : public List {
	public:
	  	DataSectionItemList();
		virtual ~DataSectionItemList();
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
		std::string idToString();
		bool isEqual(Data*);
		bool isLessThan(Data*);
	protected:
		int _id;
	};
	class VarId : public Id {
	public:
		VarId(int varId) { _id=varId;}
		virtual ~VarId();
		void accept(AbstractDataVisitor& v);
		Data* deepCopy();
	};
	class ExpId : public Id {
	public:
		ExpId(int expId) { _id=expId;}
		virtual ~ExpId();
		void accept(AbstractDataVisitor& v);
		Data* deepCopy();
	};
	class Label : public Data {
	public:
		Label(int label) { _label=label;}
		virtual ~Label();
		std::string labelToString();
		bool isEqual(Data*);
		bool isLessThan(Data*);
		void accept(AbstractDataVisitor& v);
		Data* deepCopy();
	protected:
		int _label;
	};
	class Ident : public Data {
	public:
		Ident(char* ident) { _ident=ident;}
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
		String(char* s) { _string=s;}
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
	class Analysis : public Data {
	public:
		bool isEqual(Data*) { return false; }
		Analysis(Data* configData,MapList* mapList, ResultSectionList* resultSectionList);
		~Analysis();	
		void accept(AbstractDataVisitor&);
		Analysis* deepCopy();
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

	class InfixOperator: public Data {
	public:
		InfixOperator(std::string op);
		InfixOperator(std::string op, Data* l, Data* r);
		virtual ~InfixOperator();
		void setLeftOperand(Data* l) { _left=l; }
		void setRightOperand(Data* r) { _right=r; }
		std::string getOperator() { return _operator; }
		Data* getLeftOperand() { return _left; }
		Data* getRightOperand() { return _right; }
		Data* deepCopy();
		std::string infixOperatorToString();
		void accept(AbstractDataVisitor& v);
		bool isEqual(Data*);
	private:
		bool isConsistentNode();
		std::string _operator;
		Aral::Data* _left;
		Aral::Data* _right;
	};

	class AbstractDataVisitor {
	public:
		virtual  void preVisitAnalysis(Analysis*)=0;
		virtual  void postVisitAnalysis(Analysis*)=0;
		virtual  void preVisitMapList(MapList*)=0;
		virtual  void inVisitMapList(MapList*)=0;
		virtual  void postVisitDataSectionItemList(DataSectionItemList*)=0;
		virtual  void preVisitDataSectionItemList(DataSectionItemList*)=0;
		virtual  void inVisitDataSectionItemList(DataSectionItemList*)=0;
		virtual  void postVisitMapList(MapList*)=0;
		virtual  void preVisitResultSectionList(ResultSectionList*)=0;
		virtual  void inVisitResultSectionList(ResultSectionList*)=0;
		virtual  void postVisitResultSectionList(ResultSectionList*)=0;
		virtual  void preVisitResultSection(ResultSection*)=0;
		virtual  void postVisitResultSection(ResultSection*)=0;
		virtual  void preVisitDataSectionItem(DataSectionItem*)=0;
		virtual  void postVisitDataSectionItem(DataSectionItem*)=0;
		virtual  void preVisitResultData(ResultData*)=0;
		virtual  void inVisitResultData(ResultData*)=0;
		virtual  void postVisitResultData(ResultData*)=0;
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
		virtual  void preVisitResultDataList(List*)=0;
		virtual  void inVisitResultDataList(List*)=0;
		virtual  void postVisitResultDataList(List*)=0;
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
		virtual  void preVisitInfixOperator(InfixOperator*)=0;
		virtual  void inVisitInfixOperator(InfixOperator*)=0;
		virtual  void postVisitInfixOperator(InfixOperator*)=0;
	};

	class EmptyDataVisitor : public AbstractDataVisitor {
	public:
		virtual  void preVisitAnalysis(Analysis*);
		virtual  void postVisitAnalysis(Analysis*);
		virtual  void preVisitMapList(MapList*);
		virtual  void inVisitMapList(MapList*);
		virtual  void postVisitMapList(MapList*);
		virtual  void preVisitDataSectionItemList(DataSectionItemList*);
		virtual  void inVisitDataSectionItemList(DataSectionItemList*);
		virtual  void postVisitDataSectionItemList(DataSectionItemList*);
		virtual  void preVisitResultSectionList(ResultSectionList*);
		virtual  void inVisitResultSectionList(ResultSectionList*);
		virtual  void postVisitResultSectionList(ResultSectionList*);
		virtual  void preVisitResultSection(ResultSection*);
		virtual  void postVisitResultSection(ResultSection*);
		virtual  void preVisitDataSectionItem(DataSectionItem*);
		virtual  void postVisitDataSectionItem(DataSectionItem*);
		virtual  void preVisitResultData(ResultData*);
		virtual  void inVisitResultData(ResultData*);
		virtual  void postVisitResultData(ResultData*);
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
		virtual  void preVisitResultDataList(List*);
		virtual  void inVisitResultDataList(List*);
		virtual  void postVisitResultDataList(List*);
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
		virtual  void preVisitInfixOperator(InfixOperator*);
		virtual  void inVisitInfixOperator(InfixOperator*);
		virtual  void postVisitInfixOperator(InfixOperator*);
	};
	
	class DataToStringVisitor : public EmptyDataVisitor {
	public:
		std::string dataToString(Aral::Data*);
		virtual  void preVisitAnalysis(Analysis*);
		virtual  void postVisitAnalysis(Analysis*);
		virtual  void preVisitMapList(MapList*);
		virtual  void inVisitMapList(MapList*);
		virtual  void postVisitMapList(MapList*);
		virtual  void preVisitDataSectionItemList(DataSectionItemList*);
		virtual  void inVisitDataSectionItemList(DataSectionItemList*);
		virtual  void postVisitDataSectionItemList(DataSectionItemList*);
		virtual  void preVisitResultSectionList(ResultSectionList*);
		virtual  void inVisitResultSectionList(ResultSectionList*);
		virtual  void postVisitResultSectionList(ResultSectionList*);
		virtual  void preVisitResultSection(ResultSection*);
		virtual  void postVisitResultSection(ResultSection*);
		virtual  void preVisitDataSectionItem(DataSectionItem*);
		virtual  void postVisitDataSectionItem(DataSectionItem*);
		virtual  void preVisitResultData(ResultData*);
		virtual  void inVisitResultData(ResultData*);
		virtual  void postVisitResultData(ResultData*);
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
		virtual  void preVisitResultDataList(List*);
		virtual  void inVisitResultDataList(List*);
		virtual  void postVisitResultDataList(List*);
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
		virtual  void preVisitInfixOperator(InfixOperator*);
		virtual  void inVisitInfixOperator(InfixOperator*);
		virtual  void postVisitInfixOperator(InfixOperator*);
	protected:
		std::string s;
	};

} // end of namespace Aral

extern int aralparse();
extern Aral::Data* aralIrRoot;

#endif // ARAL
