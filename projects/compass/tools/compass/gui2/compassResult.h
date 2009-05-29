#ifndef COMPASS_GUI_RESULT_H
#define COMPASS_GUI_RESULT_H

#include <compass.h>
#include <iostream>

#include "compassViolation.h"

class CompassResult : public Compass::OutputObject
{
  public:
    friend std::ostream& operator<<(std::ostream&, const CompassResult&);

    CompassResult();
    ~CompassResult();
    virtual void addOutput(Compass::OutputViolationBase* theOutput);
    void setViolationList(std::vector<CompassViolation> *v);
    void reset();

  protected:
    std::vector<CompassViolation> *violations;
}; //class CompassResult

#endif
