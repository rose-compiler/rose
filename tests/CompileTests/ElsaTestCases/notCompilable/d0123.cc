struct inode {};
struct super_block {};
typedef int (*find_inode_t)(struct inode *, unsigned long, void *);
extern struct inode * iget4(struct super_block *, unsigned long, find_inode_t, void *);
struct inode *iget(struct super_block *sb, unsigned long ino)
{
  return iget4(sb, ino, ((void *)0), ((void *)0));
}
