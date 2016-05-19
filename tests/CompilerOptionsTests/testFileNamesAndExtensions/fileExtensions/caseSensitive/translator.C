#include <rose.h>

int
main(int argc, char *argv[]) {
    SgProject *project = frontend(argc, argv);
    ASSERT_not_null(project);
    backend(project);
}
