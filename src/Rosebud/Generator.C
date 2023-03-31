#include <Rosebud/Generator.h>

namespace Rosebud {

std::string
Generator::propertyDataMemberName(const Ast::Property::Ptr &p) const {
    ASSERT_not_null(p);
    if (p->dataMemberName) {
        return *p->dataMemberName;
    } else {
        return p->name + "_";
    }
}

std::vector<std::string>
Generator::propertyAccessorNames(const Ast::Property::Ptr &p) const {
    ASSERT_not_null(p);
    if (p->accessorNames) {
        return *p->accessorNames;
    } else {
        return {p->name};
    }
}

std::vector<std::string>
Generator::propertyMutatorNames(const Ast::Property::Ptr &p) const {
    ASSERT_not_null(p);
    if (p->mutatorNames) {
        return *p->mutatorNames;
    } else {
        return {p->name};
    }
}


} // namespace
