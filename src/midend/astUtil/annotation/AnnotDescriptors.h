
#ifndef ANNOT_DESCRIPTORS_H
#define ANNOT_DESCRIPTORS_H

#include "AnnotIO.h"
#include <set>
#include <string>
#include <map>
#include <sstream>
#include <assert.h>
#include <stdio.h>
//! Input interface of a container
// 'Container' contains element of 'Member' type 
// Its string format uses 'char sep' as the separator (delimitor) , 
// left and right are the beginning and end characters, such as '{' and '}'
template <class Container, class Member, char sep, char left, char right>
class ReadContainer
{
 public:
     //! Fill in the content of container 'c' from the input stream 'in'
     static bool read( Container& c, std::istream& in) ;
};
//! Output interface of a container
template <class Container, char sep, char left, char right>
class WriteContainer 
{
 public:
    //! Write the elements inside a container to string format, with delimitor 'seq'
    //, beginning character 'left' and ending character 'right'
    static void write( const Container& c, std::ostream& out);
};
//! A pair of items separated by 'sep'
template <class First, class Second, char sep>
struct CollectPair : public std::pair<First, Second>
{
 public:
   std::pair<First, Second>::first;
   std::pair<First, Second>::second;
   CollectPair() {}
   CollectPair( const std::pair<First,Second>& that) : std::pair<First, Second>(that) {}
   bool read( std::istream& in) ;
   void write( std::ostream& out) const;
   void Dump() const  { write(std::cerr); }
   std::string ToString() const { std::stringstream out; write(out); return out.str(); }
};
//! Similar to CollectPair, but the second item may start with 'sel' without 'sep' in front of it.
template <class First, class Second, char sep, char sel>
struct SelectPair : public std::pair<First, Second>
{
 public:
   std::pair<First, Second>::first;
   std::pair<First, Second>::second;

   SelectPair() {}
   SelectPair( const std::pair<First,Second>& that) : std::pair<First, Second>(that) {}
   bool read( std::istream& in) ;
   void write( std::ostream& out) const;
   void Dump() const  { write(std::cerr); }
   std::string ToString() const { std::stringstream out; write(out); return out.str(); }
};

template <class Container, class Member, char sep, char left, char right>
class ContainerDescriptor : public Container
{
  public:
    void write( std::ostream& out) const
        { WriteContainer<Container, sep, left, right>::write(*this, out); }
   void Dump() const  { write(std::cerr); }
   std::string ToString() const { std::stringstream out; write(out); return out.str(); }
   bool read( std::istream& in)
        { 
         return ReadContainer<Container, Member,sep, left, right>::read(*this, in); 
        }
};

template <class Member,char sep, char left, char right>
class SetDescriptor : public std::set<Member>
    {
      public:
        std::set<Member>::end;
        void push_back( const Member& n)
         { insert(n); }
        bool is_member( const Member& n)
         { return find(n) != end(); }
        bool replace( const Member& orig, const Member& n) 
         {
           typename std::set<Member>::const_iterator p = find( orig );
           if (p != end()) {
              erase(p);
              insert( n);
              return true;
           }
           return false;
        }
        void write( std::ostream& out) const
             { WriteContainer<std::set<Member>, sep, left, right>::write(*this, out); }
        void Dump() const  { write(std::cerr); }
        std::string ToString() const { std::stringstream out; write(out); return out.str(); }
        bool read( std::istream& in)
            { 
              return ReadContainer<SetDescriptor<Member,sep,left,right>, Member,
                                  sep, left, right>::read(*this, in); 
            }

    };
//! 'char sep' means the separator (delimitor) , 
// left and right are the beginning and end characters, such as '{' and '}'
template <class Key, class Member, char sep, char left, char right>
class MapDescriptor : public std::map< Key,Member>
{
  public:
    class const_iterator : public std::map< Key,Member>::const_iterator {
        public:
         const_iterator( const typename std::map< Key,Member>::const_iterator& that) 
            : std::map< Key,Member>::const_iterator(that) {}
         CollectPair<const Key,Member,':'> operator *() const 
            {
                  return std::map< Key,Member>::const_iterator ::operator*();
            }
     };
     std::map< Key,Member>::end;

     void push_back( const std::pair<Key, Member>& n )
         { operator[](n.first) = n.second;  }
     bool is_member( const Key& key, const Member*& n)
         { 
           typename std::map< Key,Member>::const_iterator p = find(key);
           if (p != end()) {
                n = &(*p).second; 
                return true;
           }
           return false;
         }
     bool read( std::istream& in)
         { 
           return ReadContainer<MapDescriptor<Key,Member,sep,left,right>, 
                                CollectPair<Key,Member,0>, 
                                sep, left, right>::read(*this, in); 
         }

     void write( std::ostream& out) const
          { WriteContainer<MapDescriptor<Key,Member,sep,left,right>, sep, left, right>::write(*this, out); }
     void Dump() const  { write(std::cerr); }
     std::string ToString() const { std::stringstream out; write(out); return out.str(); }
    };

//! A descriptor for an identifier
class StringDescriptor
{
  std::string name;
 protected:
  //Return the identifier
  std::string& get_name() { return name; }
 public:
  StringDescriptor() : name("") {}
  StringDescriptor( const std::string& n ) : name(n) {}

  operator std::string() const { return name; }
  std::string get_string() const { return name; }
  bool operator < ( const StringDescriptor& that) const { return name < that.name; }
  bool operator == ( const StringDescriptor& that) const { return name == that.name; }
  //! Set the descriptor with the next identifier read from an input stream
  bool read( std::istream& in) 
      { 
         name = read_id(in); 
         return name != ""; 
      }
  void write(std::ostream& out) const { out << name; }
  void Dump() const  { write(std::cerr); }
  std::string ToString() const { return name; }
};

class NameDescriptor : public StringDescriptor
{
 public: 
  NameDescriptor()  {}
  NameDescriptor( int i) { char buf[10]; sprintf(buf,"%d", i); get_name() =  "par__"+ std::string(buf); }
  NameDescriptor( const std::string& n ) : StringDescriptor(n) {}
};

class TypeDescriptor : public StringDescriptor
{
 public:
  TypeDescriptor() {}
  TypeDescriptor( const std::string& n ) : StringDescriptor(n) {}
  bool read(std::istream& in);
};

class BoolDescriptor 
{
  bool init;
 public:
  BoolDescriptor() : init(false) {}
  operator bool() const { return init; }
  void set() { init = true; }
  bool read( std::istream& in) { init = true; return true;}
  void write( std::ostream& out) const {}
  void Dump() const { write(std::cerr); }
  std::string ToString() const { std::stringstream out; write(out); return out.str(); }
};

class IntDescriptor
{
  int init;
 public:
  IntDescriptor() : init(0) {}
  IntDescriptor( int i) : init(i) {}
  operator int() const { return init; }
  void set(int i) { init = i; }
  bool read( std::istream& in) { in >> init ; return true; }
  void write( std::ostream& out) const { out << init; }
  void Dump() const { write(std::cerr); }
  std::string ToString() const { std::stringstream out; write(out); return out.str(); }
};
//! A descriptor with begin(left) and end(right) characters, such as {x}, (x), etc
template <class Descriptor, char left, char right>
class CloseDescriptor : public Descriptor {
 public:
  bool read( std::istream& in);
  void write(std::ostream& out) const ;
  void Dump() const { write(std::cerr); }
  std::string ToString() const { std::stringstream out; write(out); return out.str(); }
};

#endif
