#ifndef bROwSE_WFunctiionNavigation_H
#define bROwSE_WFunctiionNavigation_H

#include <bROwSE/bROwSE.h>
#include <Partitioner2/Function.h>
#include <Wt/WContainerWidget>
#include <vector>

namespace bROwSE {

/** Navigation stack like the forward/back buttons in a web browser. */
class WFunctionNavigation: public Wt::WContainerWidget {
    std::vector<P2::Function::Ptr> functions_;          // most recent is at the end
    size_t currentIdx_;                                 // index into functions_
    Wt::WPushButton *wBackward_, *wForward_;
    Wt::WComboBox *wFunctions_;                         // most recent is at beginning of the list
    Wt::Signal<P2::Function::Ptr> functionChanged_;     // emitted when the current function changes

public:
    /** Default construct an empty stack. */
    explicit WFunctionNavigation(Wt::WContainerWidget *parent=NULL)
        : Wt::WContainerWidget(parent), currentIdx_(0), wBackward_(NULL), wForward_(NULL), wFunctions_(NULL) {
        init();
    }

    /** Whether list is empty. */
    bool isEmpty() const { return functions_.empty(); }

    /** Total size of list. */
    size_t size() const { return functions_.size(); }

    /** Reset to initial empty state. Does not emit a @ref functionChanged signal. */
    void clear();

    /** Current position as an index. */
    size_t currentIdx() const { return currentIdx_; }

    /** Current item or null. */
    P2::Function::Ptr current() const;

    /** Set the current item.
     *
     *  Makes the indicated item the current item. The @p idx must be valid. Returns the specified function. Emits a @ref
     *  functionChanged signal if there was a change. */
    P2::Function::Ptr setCurrent(size_t idx);

    /** Backward one item.
     *
     *  Moves current position backward one item if possible and returns that item.  If already at the first item when called,
     *  return a null pointer instead. Emits a @ref functionChanged signal when it returns non-null. */
    P2::Function::Ptr backward();                       // do not name it "back" which usually means "last item" in the STL

    /** Forward one item.
     *
     *  Moves current position forward one item if possible and returns that item.  If already at the last item when called,
     *  return a null pointer instead. Emits a @ref functionChanged signal when it returns non-null. */
    P2::Function::Ptr forward();

    /** Push function after current item.
     *
     *  If the current item is equal to the specified function then this is a no-op.  If the next item after the current item
     *  is the specified function then the current item is advanced one position.  Otherwise all items after the current item
     *  are erased from the list and the specified function is appended and made current.  If the current function changes then
     *  a @ref functionChanged signal is emitted. */
    void push(const P2::Function::Ptr&);

    /** Signal emitted when the current function changes. */
    Wt::Signal<P2::Function::Ptr>& functionChanged() { return functionChanged_; }

private:
    void init();

    // Enable or disable the various components
    void adjustEnabledStates();

    // Intermediaries for events
    void backward(const Wt::WMouseEvent&) { backward(); }
    void forward(const Wt::WMouseEvent&) { forward(); }
    void setFromCombo(int x);

};

} // namespace
#endif
