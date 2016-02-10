#include <rose.h>

#define SOURCE_FILE_NAME "copyFile-source"
#define TARGET_FILE_NAME "copyFile-target"
#define FILE_CONTENT "Contents of the file"             // No line feed

static void
createSourceFile() {
    std::ofstream out(SOURCE_FILE_NAME);
    out <<FILE_CONTENT;
    ASSERT_always_require(out.good());
}

static void
copySourceToTarget() {
    rose::FileSystem::copyFile(SOURCE_FILE_NAME, TARGET_FILE_NAME);
}

static void
checkTargetFile() {
    std::ifstream in(TARGET_FILE_NAME);
    char buf[256];
    in.getline(buf, sizeof buf);
    ASSERT_always_require(in.eof());
    ASSERT_always_require2(strcmp(buf, FILE_CONTENT)==0, "got \""+StringUtility::cEscape(buf)+"\"");
}

int main() {
    createSourceFile();
    copySourceToTarget();
    checkTargetFile();
}
