#ifndef COMPASS_GUI_INTERFACE_H
#define COMPASS_GUI_INTERFACE_H

#include <compass.h>
#include <vector>

#include "compassResult.h"
#include "compassChecker.h"

class CompassInterface
{
  public:
    CompassInterface();
    ~CompassInterface();

    void init( int argc_, char **argv_ );
    void reset();
    int numCheckers() const;
    int numEnabledCheckers() const;
    void sort(int);

    SgProject* getProject();
    CompassResult* getResult();
    Compass::Parameters* getParams();

    std::vector<const Compass::Checker*>& getCheckers();
    const CompassCheckers_v & getCompassCheckers() const;
    CompassChecker* getCompassChecker(int);
    typedef enum { SORT_NAME, SORT_TIMING, SORT_VIOLATIONS} sort_t;

  protected:
    SgProject *project;
    CompassResult *result;
    Compass::Parameters *params;
    int argc;
    char **argv;
    CompassCheckers_v compassCheckers;
    std::vector<const Compass::Checker*> theCheckers;
}; //class CompassInterface

#endif
