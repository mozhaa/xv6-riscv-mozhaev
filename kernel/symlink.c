#include "types.h"
#include "param.h"
#include "stat.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "riscv.h"
#include "proc.h"
#include "buf.h"
#include "defs.h"
#include "file.h"

static int create_symlink(const char* target, char* filename) {
	char name[DIRSIZ];
	struct inode *dp;

    pr_msg("creating symlink: %s -> %s", filename, target);
	begin_op();
	if ((dp = nameiparent(filename, name)) == 0) {
        // invalid filename
        pr_msg("file %s does not exist", filename);
		end_op();
        return -1;
    }
	ilock(dp);
    struct inode* ip = ialloc(dp->dev, T_SYMLINK);
    if (ip == 0) {
        // failed to allocate inode 
        pr_msg("failed to allocate inode");
        iunlockput(dp);
		end_op();
        return -2;
    }
    ilock(ip);
    ++ip->nlink;
    iupdate(ip);
	if (dirlink(dp, name, ip->inum) < 0) {
        // failed to write dir entry to parent dir
        pr_msg("failed to dirlink(%d, %s, %d)", dp->inum, name, ip->inum);
		iunlockput(dp);
        --ip->nlink;
        iupdate(ip);
        iunlockput(ip);
		end_op();
        return -3;
	}
    int target_len = strlen(target);
    if (writei(ip, 0, (uint64)target, 0, target_len) < target_len) {
        // failed to write target path into symlink file
        pr_msg("failed to write target path to symlink");
        iunlockput(dp);
        --ip->nlink;
        iupdate(ip);
        iunlockput(ip);
		end_op();
        return -4;
    }
	iunlockput(dp);
	iunlockput(ip);
    pr_msg("symlink has been succesfully created");

	end_op();

	return 0;
}

int read_symlink(char* filename, char* buf) {
	begin_op();

	struct inode *ip = namei(filename);
	if (ip == 0) {
        // invalid filename
		end_op();
        return -1;
    }
	ilock(ip);
    int off = readi(ip, 1, (uint64)buf, 0, ip->size);
    if (off < 0) {
        // failed to read to buffer
        iunlockput(ip);
        end_op();
        return -2;
    }
    if (copyout(myproc()->pagetable, (uint64)buf + off, "", 1) < 0) {
        // failed to write null-terminator
        iunlockput(ip);
        end_op();
        return -3;
    }
    
	iunlockput(ip);
	end_op();

	return off + 1;
}

// SYS CALLS

uint64 sys_symlink(void) {
	char target[MAXPATH], filename[MAXPATH];
	argstr(0, target, MAXPATH);
	argstr(1, filename, MAXPATH);
	return create_symlink((const char*)target, (char*)filename);
}

uint64 sys_readlink(void) {
    char filename[MAXPATH];
	uint64 buf;
	argstr(0, filename, MAXPATH);
	argaddr(1, &buf);
	return read_symlink((char*)filename, (char*)buf);
}