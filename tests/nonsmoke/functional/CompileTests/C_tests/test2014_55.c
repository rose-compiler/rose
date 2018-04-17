#define G_BYTE_ORDER 1
#define G_BIG_ENDIAN 1

typedef int guint32;
typedef int guint16;
typedef int* gpointer;
typedef int gboolean;

typedef int WapiImageResourceDirectory;
typedef int WapiImageNTHeaders32;

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



static gpointer
scan_resource_dir (WapiImageResourceDirectory *root,
		   WapiImageNTHeaders32 *nt_headers,
		   gpointer file_map,
		   WapiImageResourceDirectoryEntry *entry,
		   int level, guint32 res_id, guint32 lang_id,
		   guint32 *size)
   {
	WapiImageResourceDirectoryEntry swapped_entry;
	gboolean is_string, is_dir;
	guint32 name_offset, dir_offset, data_offset;
	
//	swapped_entry.Name = GUINT32_FROM_LE (entry->Name);
//	swapped_entry.OffsetToData = GUINT32_FROM_LE (entry->OffsetToData);
	
	is_string = swapped_entry.NameIsString;
	is_dir = swapped_entry.DataIsDirectory;
	name_offset = swapped_entry.NameOffset;
	dir_offset = swapped_entry.OffsetToDirectory;
	data_offset = swapped_entry.OffsetToData;
   }

