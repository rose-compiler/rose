#include <tr1/memory>

namespace scallop {
using std::tr1::shared_ptr;
} // namespace scallop

#include <vector>

// **************************************
namespace scallop {

class MessagePackage;
class MessageProcessor;

namespace common {

class MessageProcessorData {
private:
public:

    void addMessage(shared_ptr<MessagePackage const> const& message) 
       {
#if 1
         messages.push_back(message);
#endif
       }

private:
    std::vector<shared_ptr<MessagePackage const> > messages;
};


} // namespace common
} // namespace scallop

