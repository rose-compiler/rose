enum {
	CKPT_MAXNAME=1024
};
struct ckptconfig {
	/* For users */
	char name[CKPT_MAXNAME];
	unsigned int asyncsig;
	unsigned int continues;
	unsigned int msperiod;

	/* For internal use */
	char tryname[CKPT_MAXNAME];
	char exename[CKPT_MAXNAME];
	unsigned long ckptcnt;
};
void ckpt_config(struct ckptconfig *cfg, struct ckptconfig *old);
int ckpt_ckpt(char *name);

typedef void (*fn_t)(void *);
void ckpt_on_preckpt(fn_t f, void *arg);
void ckpt_on_postckpt(fn_t f, void *arg);
void ckpt_on_restart(fn_t f, void *arg);
