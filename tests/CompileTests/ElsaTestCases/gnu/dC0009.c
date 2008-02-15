// from the kernel

typedef unsigned char UCHAR8;
typedef short INT16;
typedef int INT32;

struct wf_sample_offset
{
    INT32 Fraction:4;
    INT32 Integer:20;
    INT32 Unused:8;
};

typedef struct wf_alias {
    INT16 OriginalSample __attribute__ ((packed));

    struct wf_sample_offset sampleStartOffset __attribute__ ((packed));
    struct wf_sample_offset loopStartOffset __attribute__ ((packed));
    struct wf_sample_offset sampleEndOffset __attribute__ ((packed));
    struct wf_sample_offset loopEndOffset __attribute__ ((packed));

    INT16 FrequencyBias __attribute__ ((packed));

    UCHAR8 SampleResolution:2 __attribute__ ((packed));
    UCHAR8 Unused1:1 __attribute__ ((packed));
    UCHAR8 Loop:1 __attribute__ ((packed));
    UCHAR8 Bidirectional:1 __attribute__ ((packed));
    UCHAR8 Unused2:1 __attribute__ ((packed));
    UCHAR8 Reverse:1 __attribute__ ((packed));
    UCHAR8 Unused3:1 __attribute__ ((packed));
    UCHAR8 sixteen_bit_padding __attribute__ ((packed));
} wavefront_alias;
