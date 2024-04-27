#include "buf.h"
#include "defs.h"
#include "file.h"
#include "fs.h"
#include "param.h"
#include "proc.h"
#include "riscv.h"
#include "sleeplock.h"
#include "spinlock.h"
#include "stat.h"
#include "types.h"

// static int create_symlink(const char* target, const char* filename) {
	// char name[DIRSIZ];
	// struct inode *dp;

	// begin_op();
	// if ((dp = nameiparent(filename, name)) == 0) {
    //     // invalid filename
	// 	end_op();
    //     return -1;
    // }
	// ilock(dp);
    // struct inode* ip = ialloc(dp->dev, T_SYMLINK);
    // if (ip == 0) {
    //     // failed to allocate inode 
    //     iunlockput(dp);
	// 	end_op();
    //     return -2;
    // }
    // ilock(ip);
    // ++ip->nlink;
    // iupdate(ip);
	// if (dirlink(dp, name, ip->inum) < 0) {
    //     // failed to write dir entry to parent dir
	// 	iunlockput(dp);
    //     --ip->nlink;
    //     iupdate(ip);
    //     iunlockput(ip);
	// 	end_op();
    //     return -3;
	// }
    // int target_len = strlen(target);
    // if (writei(ip, 0, target, 0, target_len) < target_len) {
    //     // failed to write target path into symlink file
    //     iunlockput(dp);
    //     --ip->nlink;
    //     iupdate(ip);
    //     iunlockput(ip);
	// 	end_op();
    //     return -4;
    // }
	// iunlockput(dp);
	// iunlockput(ip);

	// end_op();

	// return 0;
// }

// static int read_symlink(const char* filename, char* buf) {}

// SYS CALLS

uint64 sys_symlink(void) {
    return 0;
// 	// char target[MAXPATH], filename[MAXPATH];
// 	// argstr(0, target, MAXPATH);
// 	// argstr(1, filename, MAXPATH);
// 	// return create_symlink((const char*)target, (const char*)filename);
}

uint64 sys_readlink(void) {
    return 0;
//     // char filename[MAXPATH];
// 	// uint64 buf;
// 	// argstr(0, filename, MAXPATH);
// 	// argaddr(1, &buf);
// 	// return read_symlink((const char*)filename, (char*)buf);
}