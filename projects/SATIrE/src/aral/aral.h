#ifndef ARAL_H
#define ARAL_H

/* 
	ARAL Intermediate Represenation
	Author: Markus Schordan, 2009.

	TODO1: remove drop

	V0.7: Supported Data: Set,List,Tuple,Value,Int32,TopElement,BotElement,Label,StmtLabel,Id,VarId,ExpId;	
	      Data Methods  : getParent,deepCopy,isEqual
		                  + specific methods for adding elements
		  Iterators		: Tuple::iterator, Set::iterator, List::iterator
		  Visitor       : pre/in/post (accept,visitX)
						: AbstractDataVisitor, EmptyDataVisitor, DataToStringVisitor

	V0.8: planned: add order-criterion for Set and Map, finalize iterators
    V0.9: planned: add Type classes and Type tests, add lift/drop
	V1.0: planned: consolidate add/modify/remove/access interfaces
	V1.1: planned: add misc operators for set,list,map,tuple
*/

#include <set>
#include <map>
#include <list>
#include <vector>
#include <sstream>
#include <iostream>
#include <cassert>

namespace Aral {

	typedef signed int TuplePos;
	class Data;
	class Container;
	class Set;
	class List;
	class Tuple;
	class Map;
	class Int32;
	class TopElement;
	class BotElement;

	class AbstractDataVisitor;
	class EmptyDataVisitor;
	class ToStringDataVisitor;

   class Data {
	public:
		Data();
		virtual std::string toString();
		virtual void accept(Aral::AbstractDataVisitor&)=0;
		virtual Data* deepCopy()=0;
		virtual bool isEqual(Data*)=0;
		virtual bool isLessThan(Data*)=0;
		void autoLinkParent(Data*);
		Data* getParent();
		//bool isLifted() { return _isLifted; };
	private:
		Data* _parent;
		bool _isLifted;
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
	class Map : public Data, public std::map<Data*,Data*> {
		Map();
		virtual ~Map();
		void add(Aral::Data*,Aral::Data*);
		void accept(AbstractDataVisitor& v);
		Data* deepCopy();
		bool isEqual(Data*);
		bool isLessThan(Data*);
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
		std::string labelToString();
		bool isEqual(Data*);
		bool isLessThan(Data*);
	protected:
		int _label;
	};
	class StmtLabel : public Label {
	public:
		StmtLabel(int stmtLabel) { _label=stmtLabel;}
		virtual ~StmtLabel();
		void accept(AbstractDataVisitor& v);
		Data* deepCopy();
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

	class AbstractDataVisitor {
	public:
		virtual  void preVisitTuple(Tuple*)=0;
		virtual  void inVisitTuple(Tuple*)=0;
		virtual  void postVisitTuple(Tuple*)=0;
		virtual  void preVisitSet(Set*)=0;
		virtual  void inVisitSet(Set*)=0;
		virtual  void postVisitSet(Set*)=0;
		virtual  void preVisitList(List*)=0;
		virtual  void inVisitList(List*)=0;
		virtual  void postVisitList(List*)=0;
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
		virtual  void preVisitStmtLabel(StmtLabel*)=0;
		virtual  void postVisitStmtLabel(StmtLabel*)=0;
	};

	class EmptyDataVisitor : public AbstractDataVisitor {
	public:
		virtual  void preVisitTuple(Tuple*);
		virtual  void inVisitTuple(Tuple*);
		virtual  void postVisitTuple(Tuple*);
		virtual  void preVisitSet(Set*);
		virtual  void inVisitSet(Set*);
		virtual  void postVisitSet(Set*);
		virtual  void preVisitList(List*);
		virtual  void inVisitList(List*);
		virtual  void postVisitList(List*);
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
		virtual  void preVisitStmtLabel(StmtLabel*);
		virtual  void postVisitStmtLabel(StmtLabel*);
	};
	
	class DataToStringVisitor : public EmptyDataVisitor {
	public:
		std::string dataToString(Aral::Data*);
		virtual  void preVisitTuple(Tuple*);
		virtual  void inVisitTuple(Tuple*);
		virtual  void postVisitTuple(Tuple*);
		virtual  void preVisitSet(Set*);
		virtual  void inVisitSet(Set*);
		virtual  void postVisitSet(Set*);
		virtual  void preVisitList(List*);
		virtual  void inVisitList(List*);
		virtual  void postVisitList(List*);
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
		virtual  void preVisitStmtLabel(StmtLabel*);
		virtual  void postVisitStmtLabel(StmtLabel*);
	protected:
		std::string s;
	};

} // end of namespace Aral
#endif // ARAL
