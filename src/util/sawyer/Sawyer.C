#include <sawyer/Message.h>

namespace Sawyer {

bool isInitialized;

bool initializeLibrary() {
    if (!isInitialized) {
        Message::initializeLibrary();
        isInitialized = true;
    }
    return true;
}

} // namespace
