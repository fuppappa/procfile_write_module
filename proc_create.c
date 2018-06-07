#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <asm/uaccess.h>

MODULE_DESCRIPTION("proc file system test");
MODULE_AUTHOR("yfujieda");
MODULE_LICENSE("GPL");

static const char const module_name[] = "proc_test";

static int proc_test_read(char *page, char **start, off_t off,
			  int count, int *eof, void *data);

static int proc_test_write(struct file *file, const char __user *buffer,
			   unsigned long count, void *data);

static struct proc_dir_entry *pdir_entry;



//init function

static int proc_test_init(void)
{
  printk(KERN_INFO "%s\n", __FUNCTION__);

  pdir_entry = create_proc_entry(module_name, 0644, NULL);
  if (!pdir_entry) {
    remove_proc_entry(module_name, NULL);
    return -EFAULT;
  }

  pdir_entry->mode = S_IRUGO;
  pdir_entry->gid = 0;
  pdir_entry->uid = 0;
  pdir_entry->read_proc = (read_proc_t *) proc_test_read;
  pdir_entry->write_proc = (write_proc_t *) proc_test_write;

  return 0;
}



//exit function

static void proc_test_cleanup(void)
{
  printk(KERN_INFO "%s\n", __FUNCTION__);
  remove_proc_entry(module_name, NULL);
}

static char proc_test_buf[256] = "Hello, World";

static int proc_test_read(char *page, char **start, off_t off,
			  int count, int *eof, void *data)
{
  int len = 0;

  len = sprintf(page, "%s\n", proc_test_buf);
  *peof = 1;

  return len;
}


static int proc_test_write(struct file *file, const char __user *buffer,
			   unsigned long count, void *data)
{
  if (count > 256)
    return -EINVAL;

  if (!buffer)
    return -EINVAL;
  
  memset(proc_test_buf, 0x0, sizeof(proc_test_buf));
  if (copy_from_user(proc_test_buf, buffer, count))
    return -EFAULT;

  return count;
}


module_init(proc_test_init);
module_exit(proc_test_cleanup);
