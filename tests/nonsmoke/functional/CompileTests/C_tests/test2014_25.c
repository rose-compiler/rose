
#if 0
typedef struct
	{
	AES_KEY ks1, ks2;	/* AES key schedules to use */
	XTS128_CONTEXT xts;
	void     (*stream)(const unsigned char *in,
			unsigned char *out, size_t length,
			const AES_KEY *key1, const AES_KEY *key2,
			const unsigned char iv[16]);
	} EVP_AES_XTS_CTX;
#endif

#if 1
typedef struct
   {
  // int ks1;

  // void (*stream)(const unsigned char *in, unsigned char *out, const unsigned char iv[16]);

  // Original code is:
  // void (*stream)(const unsigned char *in, unsigned char iv[16]);
  // unparser generates:
  // void (*stream)(const unsigned char *, unsigned char ()[16]);
     void (*stream)(const unsigned char *in, unsigned char iv[16]);

   } EVP_AES_XTS_CTX;
#endif

// Original code is:
//    void (*stream)(const unsigned char *in, unsigned char iv[16]);
// unparser generates:
//    void (*stream)(const unsigned char *, unsigned char ()[16]);
      void (*stream)(const unsigned char *in, unsigned char iv[16]);


// Another example with const used.
void (*stream2)(const unsigned char *in, const unsigned char iv[16]);

