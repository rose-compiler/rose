
typedef struct {
  char *buffer;
  int len;
  int allocatedLen;
//  char staticBuf[DBUF_STATIC_SIZE];
} DynamicBuffer;

int DBufPutcFN(DynamicBuffer *dbuf, char c);

#define DBufPutc(dbuf, c) ( (dbuf)->allocatedLen < (dbuf)->len+1 ) ? (dbuf)->buffer[(dbuf)->len++] = c, (dbuf)->buffer[(dbuf)->len] = 0, 0 : DBufPutcFN((dbuf), c)

int main()
{
  char const **in;
  DynamicBuffer* buf;
  DBufPutc(buf, '\v');
  DBufPutc(buf, **in);

};
