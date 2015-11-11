// Demo how to use non-IR attributes. For IR nodes, use AstAttributeMechanism instead.
// The commentary for this program is the doxygen documentation for the rose::BinaryAnalysis::Attribute namespace.

#include <rose.h>

//! [setup]
#include <BinaryAttribute.h>
using namespace rose::BinaryAnalysis;
//! [setup]


//! [attribute types]
struct WindowPosition {
    int x, y;

    WindowPosition()
        : x(-1), y(-1) {}

    WindowPosition(int x, int y)
        : x(x), y(y) {}
};

typedef std::string FileName;
//! [attribute types]


//! [providing]
struct MyClass: public Attribute::Storage {
    // additional members...
};
//! [providing]



int
main() {

    //! [declaring]
    const Attribute::Id DUCKS    = Attribute::declare("number of ducks");
    const Attribute::Id WINDOW   = Attribute::declare("GUI window information");
    const Attribute::Id GRAPHVIZ = Attribute::declare("GraphViz output file");
    //! [declaring]

    MyClass obj;


    //! [storing values]
    obj.setAttribute(DUCKS, 10);
    obj.setAttribute(WINDOW, WindowPosition(100, 200));
    obj.setAttribute(GRAPHVIZ, std::string("/dev/null"));
    //! [storing values]


    //! [retrieve with getAttribute]
    int nDucks = obj.attributeExists(DUCKS) ? obj.getAttribute<int>(DUCKS) : 1;

    WindowPosition win;
    if (obj.attributeExists(WINDOW))
        win = obj.getAttribute<WindowPosition>(WINDOW);

    std::string fname;
    if (obj.attributeExists(GRAPHVIZ))
        fname = obj.getAttribute<std::string>(GRAPHVIZ); 
    //! [retrieve with getAttribute]
    

    //! [retrieve with default]
    nDucks = obj.attributeOrElse(DUCKS, 1); // stored ducks or 1

    win = obj.attributeOrElse(WINDOW, WindowPosition());

    fname = obj.attributeOrDefault<std::string>(GRAPHVIZ);
    //! [retrieve with default]


    //! [retrieve optional]
    if (obj.optionalAttribute<int>(DUCKS).assignTo(nDucks)) {
        // reached even if nDucks == 0
    } else {
        // reached only if DUCKS attribute is not stored
    }
    //! [retrieve optional]


    //! [erasing]
    obj.eraseAttribute(DUCKS);
    //! [erasing]
}
