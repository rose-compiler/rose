// catch namespace_qualified::class_name

// WARNING: there is no action to merge nonterm HandlerParameter

// originally found in package aiksaurus

// ERR-MATCH: merge nonterm HandlerParameter

namespace N {
    struct exception {
    };
}

int main() {
    try {
    }
    catch(N::exception) {
    }
}

