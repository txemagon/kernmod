#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

#define proc_fs_name "hello_world"

struct proc_dir_entry *our_proc_file;

int
proc_file_read (char *buffer,
		char **buffer_location,
		off_t offset, int buffer_length, int *eof, void *data)
{
  int ret;

  printk (KERN_INFO "proc_file_read (/proc/%s) called.\n", proc_fs_name);

  if (offset > 0)
    ret = 0;
  else
    ret = sprintf (buffer, "The World is a Vampire.\n");

  return ret;
}

int
init_module ()
{
  our_proc_file = create_proc_entry (proc_fs_name, 0644, NULL);

  if (our_proc_file == NULL)
    {
      remove_proc_entry (proc_fs_name, NULL);
      printk (KERN_ALERT "Error: Could not initialize /proc/%s\n",
	      proc_fs_name);
      return -ENOMEM;
    }

  our_proc_file->read_proc = proc_file_read;
  our_proc_file->owner = THIS_MODULE;
  our_proc_file->mode = S_IFREG | S_IRUGO;
  our_proc_file->uid = 0;
  our_proc_file->gid = 0;
  our_proc_file->size = 37;

  printk (KERN_INFO "/proc/%s created.\n", proc_fs_name);
  return 0;
}

void
cleanup_module ()
{
  remove_proc_entry (proc_fs_name, NULL);
  printk (KERN_INFO "/proc/%s removed.\n", proc_fs_name);
}
