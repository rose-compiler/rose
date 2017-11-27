// List the fully qualified names of all the snippets that take no arguments

#include "rose.h"
#include "midend/astSnippet/Snippet.h"

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/foreach.hpp>
#include <cstdio>
#include <cstdlib>

using namespace Rose;

// We must create a temporary file whose name ends with the specified suffix (".c" or ".java"). I'm not aware of any
// portable way to do this.  The tmpnam and mkstemp C functions can't do it for us, and I don't think the STL library
// has anything like this.  So we have to do it the hard way.
static std::pair<FILE*, std::string> createTemporaryFile(const std::string &suffix) {
    int fd = -1;
    char name1[32];
    while (1) {
        // Clean up from last iteration
        if (fd>=0) {
            int status __attribute__((unused)) = close(fd);
            assert(status>=0);
            status = unlink(name1);
            assert(status>=0);
        }

        // Create a temporary file (possibly with the wrong name) with the tools we have available. File name must also
        // be a valid Java class name
        strcpy(name1, "x_junk_XXXXXX");
        fd = mkstemp(name1);
        assert(fd>=0);

        // If the name is wrong, then see if we can rename it. Avoid using rename() since its behavior is not consistent
        // across systems and it may destroy an existing file.
        char name2[128];
        assert(strlen(name1) + strlen(suffix.c_str()) < sizeof name2);
        strcpy(name2, name1);
        if (strlen(suffix.c_str()) > 0) {
            strcat(name2, suffix.c_str());
            if (-1==link(name1, name2))
                continue;
        }

        // Try to open the file as a FILE* since there's no function in std::ofstream that can do this.
        FILE *f = fdopen(fd, "w");
        assert(f!=NULL);
        return std::make_pair(f, std::string(name2));
    }
}

int main(int argc, char *argv[]) {
    // Very rudimentary command-line parsing.  The last argument must be the name of the snippet file.
    ROSE_ASSERT(argc>=2);
    std::vector<std::string> args(argv+0, argv+argc);
    std::string snippetFileName = args.back();
    args.pop_back();

    // A snippet file cannot be loaded without first having called frontend, and we can't just call frontend on the
    // snippet file itself because then there's a failed assertion under SageBuilder::buildFile() where it gets confused that
    // we're trying to parse the same file a second time.  So we'll create and load some stupid little compilation unit first.
    std::pair<FILE*, std::string> junkFile(NULL, "");
    if (boost::ends_with(snippetFileName, ".c")) {
        junkFile = createTemporaryFile(".c");
        fputs("void jUn_CfUnKt__IuN_() {}\n", junkFile.first);
    } else {
        assert(boost::ends_with(snippetFileName, ".java"));
        junkFile = createTemporaryFile(".java");
        std::string className = boost::erase_last_copy(junkFile.second, ".java");
        fprintf(junkFile.first, "class %s { private void unused() {} }\n", className.c_str());
    }
    fclose(junkFile.first);
    args.push_back(junkFile.second);
    frontend(args);
    unlink(junkFile.second.c_str());

    // Now for the real work... load the snippet file and list its contents. Only snippets that have zero arguments
    // are included in the output (for Java this includes any overloaded method as long as one version has no arguments).
    SnippetFilePtr snippetFile = SnippetFile::instance(snippetFileName);
    assert(snippetFile!=NULL);
    BOOST_FOREACH (const std::string &name, snippetFile->getSnippetNames()) {
        BOOST_FOREACH (SnippetPtr snippet, snippetFile->findSnippets(name)) {
            SgFunctionDeclaration *fdecl = snippet->getDefinition()->get_declaration();
            if (fdecl->get_args().empty()) {
                // We must print something distinct besides the snippet name because ROSE inconveniently spits out
                // all kinds of other junk on std::cout.  The distinctive strings allows us to filter out all that
                // other junk.
                std::cout <<name <<" __THIS_IS_A_SNIPPET_NAME__\n";
            }
        }
    }

    return 0;
}
