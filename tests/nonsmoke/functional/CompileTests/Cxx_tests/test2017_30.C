

// typedef unsigned long size_t;
// typedef unsigned int my_bool;


template <typename T>
// my_bool mysql_key_fetch(const char *key_id, char **key_type, const char *user_id, void **key, size_t *key_len)
bool mysql_key_fetch()
   {
   }

// my_bool mysql_key_fetch(const char *key_id, char **key_type, const char *user_id, void **key, size_t *key_len)
bool mysql_key_fetch()
   {
  // return mysql_key_fetch<int>(key_id, key_type, user_id, key, key_len);
     return mysql_key_fetch<int>();
   }

