#ifndef FLOWEQUATIONS_H
#define FLOWEQUATIONS_H

#include <vector>

class Constraint;

class Variable {
    std::vector<Constraint*> users; // Constraints that use the value of this variable
    void addUser(Constraint* c) {
        users.push_back(c);
    }

  protected:
    Variable()
        : users()
        {}
    // After updating the value of the variable (done in a subclass-specific
    // way), this function will update all downstream variables, cycling as
    // necessary to reach a fixpoint
    void pushChanges() const;

  public:
    virtual ~Variable() {}
    friend class Constraint;
};

class Constraint {
    bool needsToRun;
    bool getNeedsToRun() const {
        return needsToRun;
    }
    void setNeedsToRun() {
        needsToRun = true;
    }
    void resetNeedsToRun() {
        needsToRun = false;
    }
    void runFull() { // Does changed checking and updating
        if (!getNeedsToRun())
            return;
        resetNeedsToRun();
        run();
    }

  protected:
    Constraint()
        : needsToRun(true)
        {}

    virtual void run() const = 0;
    virtual void markDependencies() = 0;
    void addDependency(Variable* var) {
        var->addUser(this);
    }

  public:
    void activate() {
        markDependencies();
        runFull();
    }

    virtual ~Constraint() {}
    friend class Variable;
    friend class FlowEquationSet;
};

inline void
Variable::pushChanges() const
{
    for (size_t i = 0; i < users.size(); ++i) {
        users[i]->setNeedsToRun();
    }
    for (size_t i = 0; i < users.size(); ++i) {
        users[i]->run();
    }
}

#endif /* FLOWEQUATIONS_H */
