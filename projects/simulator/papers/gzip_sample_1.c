/* Sample of code from gzip from util.c, with a couple minor changes:
 *
 *   1. The "crc" variable is local and used only to initialize "c".  In real gzip, many of the functions have global variable
 *      side effects, and this was one.
 *
 *   2. The crc_32_tab table is a local array and initialized each time we call this function.  The original code initialized
 *      this table, a global array, statically.
 */

#include <stdio.h>
#include <string.h>

unsigned long
updcrc(const unsigned char *s, unsigned n)
{
    unsigned long crc_32_tab[256];
    register unsigned long c;
    unsigned long crc = (unsigned long)0xffffffffL;

    crc_32_tab[0] = 0x00000000L;
    crc_32_tab[1] = 0x77073096L;
    crc_32_tab[2] = 0xee0e612cL;
    crc_32_tab[3] = 0x990951baL;
    crc_32_tab[4] = 0x076dc419L;
    crc_32_tab[5] = 0x706af48fL;
    crc_32_tab[6] = 0xe963a535L;
    crc_32_tab[7] = 0x9e6495a3L;
    crc_32_tab[8] = 0x0edb8832L;
    crc_32_tab[9] = 0x79dcb8a4L;
    crc_32_tab[10] = 0xe0d5e91eL;
    crc_32_tab[11] = 0x97d2d988L;
    crc_32_tab[12] = 0x09b64c2bL;
    crc_32_tab[13] = 0x7eb17cbdL;
    crc_32_tab[14] = 0xe7b82d07L;
    crc_32_tab[15] = 0x90bf1d91L;
    crc_32_tab[16] = 0x1db71064L;
    crc_32_tab[17] = 0x6ab020f2L;
    crc_32_tab[18] = 0xf3b97148L;
    crc_32_tab[19] = 0x84be41deL;
    crc_32_tab[20] = 0x1adad47dL;
    crc_32_tab[21] = 0x6ddde4ebL;
    crc_32_tab[22] = 0xf4d4b551L;
    crc_32_tab[23] = 0x83d385c7L;
    crc_32_tab[24] = 0x136c9856L;
    crc_32_tab[25] = 0x646ba8c0L;
    crc_32_tab[26] = 0xfd62f97aL;
    crc_32_tab[27] = 0x8a65c9ecL;
    crc_32_tab[28] = 0x14015c4fL;
    crc_32_tab[29] = 0x63066cd9L;
    crc_32_tab[30] = 0xfa0f3d63L;
    crc_32_tab[31] = 0x8d080df5L;
    crc_32_tab[32] = 0x3b6e20c8L;
    crc_32_tab[33] = 0x4c69105eL;
    crc_32_tab[34] = 0xd56041e4L;
    crc_32_tab[35] = 0xa2677172L;
    crc_32_tab[36] = 0x3c03e4d1L;
    crc_32_tab[37] = 0x4b04d447L;
    crc_32_tab[38] = 0xd20d85fdL;
    crc_32_tab[39] = 0xa50ab56bL;
    crc_32_tab[40] = 0x35b5a8faL;
    crc_32_tab[41] = 0x42b2986cL;
    crc_32_tab[42] = 0xdbbbc9d6L;
    crc_32_tab[43] = 0xacbcf940L;
    crc_32_tab[44] = 0x32d86ce3L;
    crc_32_tab[45] = 0x45df5c75L;
    crc_32_tab[46] = 0xdcd60dcfL;
    crc_32_tab[47] = 0xabd13d59L;
    crc_32_tab[48] = 0x26d930acL;
    crc_32_tab[49] = 0x51de003aL;
    crc_32_tab[50] = 0xc8d75180L;
    crc_32_tab[51] = 0xbfd06116L;
    crc_32_tab[52] = 0x21b4f4b5L;
    crc_32_tab[53] = 0x56b3c423L;
    crc_32_tab[54] = 0xcfba9599L;
    crc_32_tab[55] = 0xb8bda50fL;
    crc_32_tab[56] = 0x2802b89eL;
    crc_32_tab[57] = 0x5f058808L;
    crc_32_tab[58] = 0xc60cd9b2L;
    crc_32_tab[59] = 0xb10be924L;
    crc_32_tab[60] = 0x2f6f7c87L;
    crc_32_tab[61] = 0x58684c11L;
    crc_32_tab[62] = 0xc1611dabL;
    crc_32_tab[63] = 0xb6662d3dL;
    crc_32_tab[64] = 0x76dc4190L;
    crc_32_tab[65] = 0x01db7106L;
    crc_32_tab[66] = 0x98d220bcL;
    crc_32_tab[67] = 0xefd5102aL;
    crc_32_tab[68] = 0x71b18589L;
    crc_32_tab[69] = 0x06b6b51fL;
    crc_32_tab[70] = 0x9fbfe4a5L;
    crc_32_tab[71] = 0xe8b8d433L;
    crc_32_tab[72] = 0x7807c9a2L;
    crc_32_tab[73] = 0x0f00f934L;
    crc_32_tab[74] = 0x9609a88eL;
    crc_32_tab[75] = 0xe10e9818L;
    crc_32_tab[76] = 0x7f6a0dbbL;
    crc_32_tab[77] = 0x086d3d2dL;
    crc_32_tab[78] = 0x91646c97L;
    crc_32_tab[79] = 0xe6635c01L;
    crc_32_tab[80] = 0x6b6b51f4L;
    crc_32_tab[81] = 0x1c6c6162L;
    crc_32_tab[82] = 0x856530d8L;
    crc_32_tab[83] = 0xf262004eL;
    crc_32_tab[84] = 0x6c0695edL;
    crc_32_tab[85] = 0x1b01a57bL;
    crc_32_tab[86] = 0x8208f4c1L;
    crc_32_tab[87] = 0xf50fc457L;
    crc_32_tab[88] = 0x65b0d9c6L;
    crc_32_tab[89] = 0x12b7e950L;
    crc_32_tab[90] = 0x8bbeb8eaL;
    crc_32_tab[91] = 0xfcb9887cL;
    crc_32_tab[92] = 0x62dd1ddfL;
    crc_32_tab[93] = 0x15da2d49L;
    crc_32_tab[94] = 0x8cd37cf3L;
    crc_32_tab[95] = 0xfbd44c65L;
    crc_32_tab[96] = 0x4db26158L;
    crc_32_tab[97] = 0x3ab551ceL;
    crc_32_tab[98] = 0xa3bc0074L;
    crc_32_tab[99] = 0xd4bb30e2L;
    crc_32_tab[100] = 0x4adfa541L;
    crc_32_tab[101] = 0x3dd895d7L;
    crc_32_tab[102] = 0xa4d1c46dL;
    crc_32_tab[103] = 0xd3d6f4fbL;
    crc_32_tab[104] = 0x4369e96aL;
    crc_32_tab[105] = 0x346ed9fcL;
    crc_32_tab[106] = 0xad678846L;
    crc_32_tab[107] = 0xda60b8d0L;
    crc_32_tab[108] = 0x44042d73L;
    crc_32_tab[109] = 0x33031de5L;
    crc_32_tab[110] = 0xaa0a4c5fL;
    crc_32_tab[111] = 0xdd0d7cc9L;
    crc_32_tab[112] = 0x5005713cL;
    crc_32_tab[113] = 0x270241aaL;
    crc_32_tab[114] = 0xbe0b1010L;
    crc_32_tab[115] = 0xc90c2086L;
    crc_32_tab[116] = 0x5768b525L;
    crc_32_tab[117] = 0x206f85b3L;
    crc_32_tab[118] = 0xb966d409L;
    crc_32_tab[119] = 0xce61e49fL;
    crc_32_tab[120] = 0x5edef90eL;
    crc_32_tab[121] = 0x29d9c998L;
    crc_32_tab[122] = 0xb0d09822L;
    crc_32_tab[123] = 0xc7d7a8b4L;
    crc_32_tab[124] = 0x59b33d17L;
    crc_32_tab[125] = 0x2eb40d81L;
    crc_32_tab[126] = 0xb7bd5c3bL;
    crc_32_tab[127] = 0xc0ba6cadL;
    crc_32_tab[128] = 0xedb88320L;
    crc_32_tab[129] = 0x9abfb3b6L;
    crc_32_tab[130] = 0x03b6e20cL;
    crc_32_tab[131] = 0x74b1d29aL;
    crc_32_tab[132] = 0xead54739L;
    crc_32_tab[133] = 0x9dd277afL;
    crc_32_tab[134] = 0x04db2615L;
    crc_32_tab[135] = 0x73dc1683L;
    crc_32_tab[136] = 0xe3630b12L;
    crc_32_tab[137] = 0x94643b84L;
    crc_32_tab[138] = 0x0d6d6a3eL;
    crc_32_tab[139] = 0x7a6a5aa8L;
    crc_32_tab[140] = 0xe40ecf0bL;
    crc_32_tab[141] = 0x9309ff9dL;
    crc_32_tab[142] = 0x0a00ae27L;
    crc_32_tab[143] = 0x7d079eb1L;
    crc_32_tab[144] = 0xf00f9344L;
    crc_32_tab[145] = 0x8708a3d2L;
    crc_32_tab[146] = 0x1e01f268L;
    crc_32_tab[147] = 0x6906c2feL;
    crc_32_tab[148] = 0xf762575dL;
    crc_32_tab[149] = 0x806567cbL;
    crc_32_tab[150] = 0x196c3671L;
    crc_32_tab[151] = 0x6e6b06e7L;
    crc_32_tab[152] = 0xfed41b76L;
    crc_32_tab[153] = 0x89d32be0L;
    crc_32_tab[154] = 0x10da7a5aL;
    crc_32_tab[155] = 0x67dd4accL;
    crc_32_tab[156] = 0xf9b9df6fL;
    crc_32_tab[157] = 0x8ebeeff9L;
    crc_32_tab[158] = 0x17b7be43L;
    crc_32_tab[159] = 0x60b08ed5L;
    crc_32_tab[160] = 0xd6d6a3e8L;
    crc_32_tab[161] = 0xa1d1937eL;
    crc_32_tab[162] = 0x38d8c2c4L;
    crc_32_tab[163] = 0x4fdff252L;
    crc_32_tab[164] = 0xd1bb67f1L;
    crc_32_tab[165] = 0xa6bc5767L;
    crc_32_tab[166] = 0x3fb506ddL;
    crc_32_tab[167] = 0x48b2364bL;
    crc_32_tab[168] = 0xd80d2bdaL;
    crc_32_tab[169] = 0xaf0a1b4cL;
    crc_32_tab[170] = 0x36034af6L;
    crc_32_tab[171] = 0x41047a60L;
    crc_32_tab[172] = 0xdf60efc3L;
    crc_32_tab[173] = 0xa867df55L;
    crc_32_tab[174] = 0x316e8eefL;
    crc_32_tab[175] = 0x4669be79L;
    crc_32_tab[176] = 0xcb61b38cL;
    crc_32_tab[177] = 0xbc66831aL;
    crc_32_tab[178] = 0x256fd2a0L;
    crc_32_tab[179] = 0x5268e236L;
    crc_32_tab[180] = 0xcc0c7795L;
    crc_32_tab[181] = 0xbb0b4703L;
    crc_32_tab[182] = 0x220216b9L;
    crc_32_tab[183] = 0x5505262fL;
    crc_32_tab[184] = 0xc5ba3bbeL;
    crc_32_tab[185] = 0xb2bd0b28L;
    crc_32_tab[186] = 0x2bb45a92L;
    crc_32_tab[187] = 0x5cb36a04L;
    crc_32_tab[188] = 0xc2d7ffa7L;
    crc_32_tab[189] = 0xb5d0cf31L;
    crc_32_tab[190] = 0x2cd99e8bL;
    crc_32_tab[191] = 0x5bdeae1dL;
    crc_32_tab[192] = 0x9b64c2b0L;
    crc_32_tab[193] = 0xec63f226L;
    crc_32_tab[194] = 0x756aa39cL;
    crc_32_tab[195] = 0x026d930aL;
    crc_32_tab[196] = 0x9c0906a9L;
    crc_32_tab[197] = 0xeb0e363fL;
    crc_32_tab[198] = 0x72076785L;
    crc_32_tab[199] = 0x05005713L;
    crc_32_tab[200] = 0x95bf4a82L;
    crc_32_tab[201] = 0xe2b87a14L;
    crc_32_tab[202] = 0x7bb12baeL;
    crc_32_tab[203] = 0x0cb61b38L;
    crc_32_tab[204] = 0x92d28e9bL;
    crc_32_tab[205] = 0xe5d5be0dL;
    crc_32_tab[206] = 0x7cdcefb7L;
    crc_32_tab[207] = 0x0bdbdf21L;
    crc_32_tab[208] = 0x86d3d2d4L;
    crc_32_tab[209] = 0xf1d4e242L;
    crc_32_tab[210] = 0x68ddb3f8L;
    crc_32_tab[211] = 0x1fda836eL;
    crc_32_tab[212] = 0x81be16cdL;
    crc_32_tab[213] = 0xf6b9265bL;
    crc_32_tab[214] = 0x6fb077e1L;
    crc_32_tab[215] = 0x18b74777L;
    crc_32_tab[216] = 0x88085ae6L;
    crc_32_tab[217] = 0xff0f6a70L;
    crc_32_tab[218] = 0x66063bcaL;
    crc_32_tab[219] = 0x11010b5cL;
    crc_32_tab[220] = 0x8f659effL;
    crc_32_tab[221] = 0xf862ae69L;
    crc_32_tab[222] = 0x616bffd3L;
    crc_32_tab[223] = 0x166ccf45L;
    crc_32_tab[224] = 0xa00ae278L;
    crc_32_tab[225] = 0xd70dd2eeL;
    crc_32_tab[226] = 0x4e048354L;
    crc_32_tab[227] = 0x3903b3c2L;
    crc_32_tab[228] = 0xa7672661L;
    crc_32_tab[229] = 0xd06016f7L;
    crc_32_tab[230] = 0x4969474dL;
    crc_32_tab[231] = 0x3e6e77dbL;
    crc_32_tab[232] = 0xaed16a4aL;
    crc_32_tab[233] = 0xd9d65adcL;
    crc_32_tab[234] = 0x40df0b66L;
    crc_32_tab[235] = 0x37d83bf0L;
    crc_32_tab[236] = 0xa9bcae53L;
    crc_32_tab[237] = 0xdebb9ec5L;
    crc_32_tab[238] = 0x47b2cf7fL;
    crc_32_tab[239] = 0x30b5ffe9L;
    crc_32_tab[240] = 0xbdbdf21cL;
    crc_32_tab[241] = 0xcabac28aL;
    crc_32_tab[242] = 0x53b39330L;
    crc_32_tab[243] = 0x24b4a3a6L;
    crc_32_tab[244] = 0xbad03605L;
    crc_32_tab[245] = 0xcdd70693L;
    crc_32_tab[246] = 0x54de5729L;
    crc_32_tab[247] = 0x23d967bfL;
    crc_32_tab[248] = 0xb3667a2eL;
    crc_32_tab[249] = 0xc4614ab8L;
    crc_32_tab[250] = 0x5d681b02L;
    crc_32_tab[251] = 0x2a6f2b94L;
    crc_32_tab[252] = 0xb40bbe37L;
    crc_32_tab[253] = 0xc30c8ea1L;
    crc_32_tab[254] = 0x5a05df1bL;
    crc_32_tab[255] = 0x2d02ef8dL;

    if (s==NULL) {
        c = 0xffffffffL;
    } else {
        c = crc;
        if (n) do {
            c = crc_32_tab[((int)c ^ (*s++)) & 0xff] ^ (c >> 8);
        } while (--n);
    }
    crc = c;
    return c ^ 0xffffffffL;
}

int
main()
{
#if 1 /* we don't actually need to ever call updcrc() */
    const char *s = "hello world!";
    size_t n = strlen(s);
    unsigned long crc = updcrc((const unsigned char*)s, n);
    printf("s=\"%s\"; crc=0x%08lx\n", s, crc);
#endif
    return 0;
}
