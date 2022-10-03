struct request {
struct request_header

{ char *name; }
*headers;
};

void foo ()

{ struct request_header *hdr; hdr->name; }

void bar() { struct request_header *hdr; hdr->name; }

