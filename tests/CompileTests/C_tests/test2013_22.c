
#if 0
// Unpased as:
static const struct protoidlist 
   {
     const char protoid[5];
     const char *name;
   } protoidlist[] = {{{(0), (0), (0x0c), (0x01), (0x07)},  = "CiscoMLS"}, 
                      {{(0), (0), (0x0c), (0x20), (0)},     = "CiscoCDP"}, 
                      {{(0), (0), (0x0c), (0x20), (0x01)},  = "CiscoCGMP"}, 
                      {{(0), (0), (0x0c), (0x20), (0x03)},  = "CiscoVTP"}, 
                      {{(0), (0xe0), (0x2b), (0), (0xbb)},  = "ExtremeEDP"}, 
                      {{(0), (0), (0), (0), (0)},  = ((void *)0)}};
#endif

static const struct protoidlist {
 const char protoid[5];
 const char *name;
} protoidlist[] = {
 {{ 0x00, 0x00, 0x0c, 0x01, 0x07 }, "CiscoMLS" },
 {{ 0x00, 0x00, 0x0c, 0x20, 0x00 }, "CiscoCDP" },
 {{ 0x00, 0x00, 0x0c, 0x20, 0x01 }, "CiscoCGMP" },
 {{ 0x00, 0x00, 0x0c, 0x20, 0x03 }, "CiscoVTP" },
 {{ 0x00, 0xe0, 0x2b, 0x00, 0xbb }, "ExtremeEDP" },
 {{ 0x00, 0x00, 0x00, 0x00, 0x00 }, ((void *)0) }
};

