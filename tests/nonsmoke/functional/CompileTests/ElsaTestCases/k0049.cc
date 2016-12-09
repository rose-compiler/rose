// 'restrict' as name

// originally found in package crawl_1:4.0.0beta26-4

// a.ii:3:8: Parse error (state 138) at restrict

// ERR-MATCH: Parse error.*?at restrict

struct restrict {
};

int main() {
    int restrict = 1;
    return restrict == 1;
}
