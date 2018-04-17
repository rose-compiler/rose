
typedef enum _HTDirShow {
    HT_DS_SIZE = 0x1,
    HT_DS_DATE = 0x2,
#if 0
    HT_DS_HID = 0x4,
    HT_DS_DES = 0x8,
    HT_DS_ICON = 0x10,
    HT_DS_HOTI = 0x20
#endif
} HTDirShow;

// This is unparsed as: static HTDirShow dir_show = _HTDirShow(27);
// static HTDirShow dir_show = HT_DS_SIZE+HT_DS_DATE+HT_DS_DES+HT_DS_ICON;
static HTDirShow dir_show = HT_DS_SIZE+HT_DS_DATE;
