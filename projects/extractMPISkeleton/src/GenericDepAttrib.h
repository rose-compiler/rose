#ifndef __GenericDepAttrib_H_LOADED__
#define __GenericDepAttrib_H_LOADED__

#include <boost/bimap/bimap.hpp>
#include "APIDepAttrib.h"

#ifdef _MSC_VER
  using boost::uint32_t;
#endif

typedef boost::bimaps::bimap<uint32_t,std::string> name_table;

class GenericDepAttribute : public APIDepAttribute {
    private:

    DependenceForm depForm;
    uint32_t depType;
    name_table *nameTable;
    bool backward;

    public:

    GenericDepAttribute(name_table *tab, uint32_t ty);
    GenericDepAttribute(name_table *tab, std::string ty);

    uint32_t getDepType() const;

    GenericDepAttribute *copy() const;

    void join(const APIDepAttribute *attr);
    void joinForm(const DependenceForm form);
    void joinType(const uint32_t type);

    bool matches(const APIDepAttribute *attr) const;

    bool contains(const APIDepAttribute *attr) const;

    DependenceForm getDepForm() const;
    void setDepForm(DependenceForm newDepForm);

    std::string attribute_class_name() const;

    std::string toString() const;
};

#endif
