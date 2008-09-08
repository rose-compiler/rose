#ifndef COMPASS_GUI_CHECKER_H
#define COMPASS_GUI_CHECKER_H

#include <compass.h>
#include <vector>
#include <string>

#include "timer.h"
#include "compassViolation.h"

class CompassInterface;		//forward declaration

class CompassChecker
{
  public:
    CompassChecker(const Compass::Checker *checker_);
    ~CompassChecker();

    void reset();
    void process(bool,CompassInterface&);
    std::string getName() const;
    std::string getLongDescription() const;
    std::string getShortDescription() const;

    bool hasErrorMsg() const;
    std::string getErrorMsg() const;
    double getEllapsedTime() const;
    int getNumViolations() const;
    const std::vector<CompassViolation>* getViolations() const;

    bool isEnabled() const;
    void setEnabled(bool);

    static bool lessBasedOnName(const CompassChecker *left, const CompassChecker *right);
    static bool lessBasedOnTiming(const CompassChecker *left, const CompassChecker *right);
    static bool lessBasedOnViolations(const CompassChecker *left, const CompassChecker *right);

  private:
    const Compass::Checker *checker;
    bool enabled;
    std::string error;
    double ellapsedTime;
    std::vector<CompassViolation> violations;
}; //class CompassChecker

typedef std::vector< CompassChecker* > CompassCheckers_v;

#endif
