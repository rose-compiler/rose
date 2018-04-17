// t0088.cc
// excerpt from /home/scott/bld/mozilla-1.0/extensions/inspector/base/src/inDOMUtils.i
// dealing with pointer to member

struct nsStyleStruct {};
struct nsCSSStruct {};
class nsIStyleContext;
typedef int PRBool;

class nsRuleNode {
  enum RuleDetail {
    eRuleNone,
    // etc.
  };

  typedef const nsStyleStruct*
  (nsRuleNode::*ComputeStyleDataFn)(nsStyleStruct* aStartStruct,
                                    const nsCSSStruct& aStartData,
                                    nsIStyleContext* aContext,
                                    nsRuleNode* aHighestNode,
                                    const RuleDetail& aRuleDetail,
                                    PRBool aInherited);
};
