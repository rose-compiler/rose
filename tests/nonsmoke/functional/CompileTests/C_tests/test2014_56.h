typedef struct
{
	union 
	{
		struct 
		{
#if G_BYTE_ORDER == G_BIG_ENDIAN
			guint32 NameIsString:1;
			guint32 NameOffset:31;
#else
			guint32 NameOffset:31;
			guint32 NameIsString:1;
#endif
		};
		guint32 Name;
#if G_BYTE_ORDER == G_BIG_ENDIAN
		struct
		{
			guint16 __wapi_big_endian_padding;
			guint16 Id;
		};
#else
		guint16 Id;
#endif
	};
	union
	{
		guint32 OffsetToData;
		struct 
		{
#if G_BYTE_ORDER == G_BIG_ENDIAN
			guint32 DataIsDirectory:1;
			guint32 OffsetToDirectory:31;
#else
			guint32 OffsetToDirectory:31;
			guint32 DataIsDirectory:1;
#endif
		};
	};
} WapiImageResourceDirectoryEntry;
