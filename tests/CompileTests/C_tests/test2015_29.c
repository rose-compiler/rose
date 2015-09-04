typedef struct CirrusVGAState 
   {
      int x[5];
   } CirrusVGAState;

typedef unsigned long size_t;
typedef unsigned char uint8_t;

typedef struct VMStateDescription VMStateDescription;

typedef struct {
    const char *name;
    size_t offset;
    size_t size;
    size_t start;
    int num;
    size_t num_offset;
    size_t size_offset;
    int version_id;
} VMStateField;

typedef struct VMStateSubsection {
    const VMStateDescription *vmsd;
} VMStateSubsection;

struct VMStateDescription {
    const char *name;
    int unmigratable;
    int version_id;
    int minimum_version_id;
    int minimum_version_id_old;
    int (*pre_load)(void *opaque);
    int (*post_load)(void *opaque, int version_id);
    void (*pre_save)(void *opaque);
    VMStateField *fields;
    const VMStateSubsection *subsections;
};


static const VMStateDescription vmstate_cirrus_vga = {
    .fields = (VMStateField []) {
        { 
          .offset = ( ((uint8_t(*) [sizeof(typeof(((CirrusVGAState *)0)->x))]) 0 ) ), 
        },
        {}
    }
};

