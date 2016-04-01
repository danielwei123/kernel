/******************************************************************************/
/* Important Spring 2016 CSCI 402 usage information:                          */
/*                                                                            */
/* This fils is part of CSCI 402 kernel programming assignments at USC.       */
/*         53616c7465645f5f2e8d450c0c5851acd538befe33744efca0f1c4f9fb5f       */
/*         3c8feabc561a99e53d4d21951738da923cd1c7bbd11b30a1afb11172f80b       */
/*         984b1acfbbf8fae6ea57e0583d2610a618379293cb1de8e1e9d07e6287e8       */
/*         de7e82f3d48866aa2009b599e92c852f7dbf7a6e573f1c7228ca34b9f368       */
/*         faaef0c0fcf294cb                                                   */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove or alter this comment block.          */
/* If this comment block is removed or altered in a submitted file, 20 points */
/*         will be deducted.                                                  */
/******************************************************************************/

/*
 *  FILE: vfs_syscall.c
 *  AUTH: mcc | jal
 *  DESC:
 *  DATE: Wed Apr  8 02:46:19 1998
 *  $Id: vfs_syscall.c,v 1.13 2015/12/15 14:38:24 william Exp $
 */

#include "kernel.h"
#include "errno.h"
#include "globals.h"
#include "fs/vfs.h"
#include "fs/file.h"
#include "fs/vnode.h"
#include "fs/vfs_syscall.h"
#include "fs/open.h"
#include "fs/fcntl.h"
#include "fs/lseek.h"
#include "mm/kmalloc.h"
#include "util/string.h"
#include "util/printf.h"
#include "fs/stat.h"
#include "util/debug.h"

/* To read a file:
 *      o fget(fd)
 *      o call its virtual read fs_op
 *      o update f_pos
 *      o fput() it
 *      o return the number of bytes read, or an error
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not a valid file descriptor or is not open for reading.
 *      o EISDIR
 *        fd refers to a directory.
 *
 * In all cases, be sure you do not leak file refcounts by returning before
 * you fput() a file that you fget()'ed.
 */
int
do_read(int fd, void *buf, size_t nbytes)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_read");*/
        int	bytes_read = -1;
        
        if(fd < 0 || fd >= NFILES)
 	{
 		return	-EBADF;
 	}
        
        file_t	*f = fget(fd);
        
        if(f == NULL)
        {
        	return	-EBADF;	
        }
        
        if(!(f->f_mode&FMODE_READ)){
        	fput(f);
        	return	-EBADF;
        }
        
        
        bytes_read = f->f_vnode->vn_ops->read(f->f_vnode, f->f_pos, buf, nbytes);
        
        if(bytes_read != -1)
        {
        	do_lseek(fd, bytes_read, SEEK_CUR);
        	fput(f);
        	return	bytes_read;
        }
        
        fput(f);
        return -1;
}

/* Very similar to do_read.  Check f_mode to be sure the file is writable.  If
 * f_mode & FMODE_APPEND, do_lseek() to the end of the file, call the write
 * fs_op, and fput the file.  As always, be mindful of refcount leaks.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not a valid file descriptor or is not open for writing.
 */
int
do_write(int fd, const void *buf, size_t nbytes)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_write");*/
        
        int	bytes_written = -1;
        
        if(fd < 0 || fd >= NFILES)
 	{
 		return	-EBADF;
 	}
        
        file_t	*f = fget(fd);
        
       	if(f == NULL){
		return	-EBADF;
	}
	
	if((f->f_mode&FMODE_WRITE) != FMODE_WRITE)
	{
		return	-EBADF;
	}
	
	if((f->f_mode&FMODE_APPEND)==FMODE_APPEND)
	{
		do_lseek(fd, 0, SEEK_END);
	}
	
	bytes_written = f->f_vnode->vn_ops->write(f->f_vnode, f->f_pos, buf, nbytes);
        
        if(bytes_written != -1)
        {
   		do_lseek(fd, bytes_written, SEEK_CUR);
        	fput(f);
        	return	bytes_written;
        }
        
        fput(f);
        return -1;
}

/*
 * Zero curproc->p_files[fd], and fput() the file. Return 0 on success
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd isn't a valid open file descriptor.
 */
int
do_close(int fd)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_close");*/
 
 	if(fd < 0 || fd >= NFILES)
 	{
 		return	-EBADF;
 	}
 
 	file_t	*temp = curproc->p_files[fd];
	curproc->p_files[fd] = NULL;
       	fput(temp);
		
        return 0;
}

/* To dup a file:
 *      o fget(fd) to up fd's refcount
 *      o get_empty_fd()
 *      o point the new fd to the same file_t* as the given fd
 *      o return the new file descriptor
 *
 * Don't fput() the fd unless something goes wrong.  Since we are creating
 * another reference to the file_t*, we want to up the refcount.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd isn't an open file descriptor.
 *      o EMFILE
 *        The process already has the maximum number of file descriptors open
 *        and tried to open a new one.
 */
int
do_dup(int fd)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_dup");*/
        file_t  *f;
        int fd_new;

 	if(fd < 0 || fd >= NFILES || (curproc->p_files[fd] == NULL))
 	{
 		return	-EBADF;
 	}
 	
        f = fget(fd);
        /* fd isn't an open file descriptor. */
        
        if( f == NULL)
        {
                return -EBADF;
        }

        fd_new = get_empty_fd(curproc);

        if(fd_new == -EMFILE)
        {
               fput(f);
               return -EMFILE; 
        }
        else
        {
                curproc->p_files[fd_new] = f;
                return fd_new;
        }

}

/* Same as do_dup, but insted of using get_empty_fd() to get the new fd,
 * they give it to us in 'nfd'.  If nfd is in use (and not the same as ofd)
 * do_close() it first.  Then return the new file descriptor.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        ofd isn't an open file descriptor, or nfd is out of the allowed
 *        range for file descriptors.
 */
int
do_dup2(int ofd, int nfd)
{
        NOT_YET_IMPLEMENTED("VFS: do_dup2");
        file_t  *f_ofd;
        file_t  *f_nfd;
        int   fd_new;
 	
 	if(ofd < 0 || ofd >= NFILES || (curproc->p_files[ofd] == NULL))
 	{
 		return	-EBADF;
 	}
 	
        f_ofd = fget(ofd);
        /* fd isn't an open file descriptor. */
        if( f_ofd == NULL || nfd < 0 || nfd >= NFILES)
        {
                return -EBADF;
        }

        f_nfd = curproc->p_files[nfd];
        if( f_nfd != NULL && nfd != ofd)
        {
                do_close(nfd);
        }
        curproc->p_files[nfd] = f_ofd;
        return nfd;

}

/*
 * This routine creates a special file of the type specified by 'mode' at
 * the location specified by 'path'. 'mode' should be one of S_IFCHR or
 * S_IFBLK (you might note that mknod(2) normally allows one to create
 * regular files as well-- for simplicity this is not the case in Weenix).
 * 'devid', as you might expect, is the device identifier of the device
 * that the new special file should represent.
 *
 * You might use a combination of dir_namev, lookup, and the fs-specific
 * mknod (that is, the containing directory's 'mknod' vnode operation).
 * Return the result of the fs-specific mknod, or an error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL
 *        mode requested creation of something other than a device special
 *        file.
 *      o EEXIST
 *        path already exists.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_mknod(const char *path, int mode, unsigned devid)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_mknod");*/
        
        if((mode != S_IFCHR) || (mode != S_IFBLK))
        {
        	return	-EINVAL;		
        }

        size_t	*nameLength = NULL;
        const char *name;
        vnode_t	**result_node = NULL;
        vnode_t	*ret_node = NULL;

	/* might have to change BASE */
	int res = dir_namev(path, nameLength, &name, NULL, result_node);
	
	if( res < 0){
	
		return	res;
	}
	
	res = lookup(*result_node, name, *nameLength, &ret_node);
	
	if(res == -ENAMETOOLONG){
		return	res;
	}
	
	if(res == 0){
		return	-EEXIST;
	}
	        
        return	(*result_node)->vn_ops->mknod(*result_node, name, *nameLength, mode, devid);
      
}

/* Use dir_namev() to find the vnode of the dir we want to make the new
 * directory in.  Then use lookup() to make sure it doesn't already exist.
 * Finally call the dir's mkdir vn_ops. Return what it returns.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EEXIST
 *        path already exists.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_mkdir(const char *path)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_mkdir");*/
        
       size_t	*nameLength = NULL;
        const char	*name;
        vnode_t	**result_node = NULL;
        vnode_t	*ret_node = NULL;

	/*might have to change BASE */
	int	res = dir_namev(path, nameLength, &name, NULL, result_node);
	
	if( res < 0){
	
		return	res;
	}
	
	res = lookup(*result_node, name, *nameLength, &ret_node);
	
	if(res == -ENAMETOOLONG){
		return	res;
	}
	if(res == 0){
		return	-EEXIST;
	}
        return	(*result_node)->vn_ops->mkdir(*result_node, name, *nameLength);
}

/* Use dir_namev() to find the vnode of the directory containing the dir to be
 * removed. Then call the containing dir's rmdir v_op.  The rmdir v_op will
 * return an error if the dir to be removed does not exist or is not empty, so
 * you don't need to worry about that here. Return the value of the v_op,
 * or an error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL
 *        path has "." as its final component.
 *      o ENOTEMPTY
 *        path has ".." as its final component.
 *      o ENOENT
 *        A directory component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_rmdir(const char *path)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_rmdir");*/
	        
        size_t	*nameLength = NULL;
        const char	*name;
        vnode_t	**result_node = NULL;
        vnode_t	*ret_node = NULL;

	/*might have to change BASE*/
	int res = dir_namev(path, nameLength, &name, NULL, result_node);
	
	
	if(name[0] == '.'){
	
		if(name[1] == '.')
		{
			return	-ENOTEMPTY;
		}	
		
		return	-EINVAL;
	}
	if( res < 0){
	
		return	res;
	}
        
        return	(*result_node)->vn_ops->rmdir(*result_node, name, *nameLength);
}

/*
 * Same as do_rmdir, but for files.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EISDIR
 *        path refers to a directory.
 *      o ENOENT
 *        A component in path does not exist.
 *      o ENOTDIR
 *        A component used as a directory in path is not, in fact, a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_unlink(const char *path)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_unlink");*/
         size_t	*nameLength = NULL;
        const char	*name;
        vnode_t	**result_node = NULL;
        vnode_t	*ret_node = NULL;

	/*might have to change BASE*/
	int	res = dir_namev(path, nameLength, &name, NULL, result_node);
	
	if( res < 0){
	
		return	res;
	}
	
        res = lookup(*result_node, name, *nameLength, &ret_node);
        
        if(res < 0)
        {
        	return	res;
        }
        
        if(ret_node->vn_mode & S_IFDIR )
	{
		return -EISDIR;
	}        
        
        return	(*result_node)->vn_ops->unlink(*result_node, name, *nameLength);

}

/* To link:
 *      o open_namev(from)
 *      o dir_namev(to)
 *      o call the destination dir's (to) link vn_ops.
 *      o return the result of link, or an error
 *
 * Remember to vput the vnodes returned from open_namev and dir_namev.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EEXIST
 *        to already exists.
 *      o ENOENT
 *        A directory component in from or to does not exist.
 *      o ENOTDIR
 *        A component used as a directory in from or to is not, in fact, a
 *        directory.
 *      o ENAMETOOLONG
 *        A component of from or to was too long.
 *      o EISDIR
 *        from is a directory.
 */
int
do_link(const char *from, const char *to)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_link");*/
        vnode_t	**ret = NULL;
        vnode_t	**res_vnode = NULL;
        vnode_t **ret_vnode = NULL;
        const char	*name;
        size_t	*nameLength = NULL;
        
        int res = open_namev(from, O_RDONLY, ret, NULL);
        
        if( res < 0)
	{
		return res;
	}    
	    
        res = dir_namev(to, nameLength, &name, NULL, res_vnode);
      
        if( res < 0)
        {
        	return res;
        }
        
        res = lookup(*ret, name, *nameLength, ret_vnode);
        if(res == 0)
        {
        	return -EEXIST;
        }
        
        return (*res_vnode)->vn_ops->link(*res_vnode, *ret, name, *nameLength);
}

/*      o link newname to oldname
 *      o unlink oldname
 *      o return the value of unlink, or an error
 *
 * Note that this does not provide the same behavior as the
 * Linux system call (if unlink fails then two links to the
 * file could exist).
 */
int
do_rename(const char *oldname, const char *newname)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_rename");*/
        
        do_link(oldname, newname);
        
        do_unlink(oldname);
        
        return -1;
}

/* Make the named directory the current process's cwd (current working
 * directory).  Don't forget to down the refcount to the old cwd (vput()) and
 * up the refcount to the new cwd (open_namev() or vget()). Return 0 on
 * success.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o ENOENT
 *        path does not exist.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 *      o ENOTDIR
 *        A component of path is not a directory.
 */
int
do_chdir(const char *path)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_chdir");*/
        vnode_t	**ret = NULL;
        vnode_t	**out = NULL;
        
        int res = open_namev(path, 0, ret, NULL);
        
        if(res < 0)
        {
        	return	res;
        }
        
        *out = curproc->p_cwd;
	vput(*out);

        curproc->p_cwd = *ret;
        
        return 0;
}

/* Call the readdir fs_op on the given fd, filling in the given dirent_t*.
 * If the readdir fs_op is successful, it will return a positive value which
 * is the number of bytes copied to the dirent_t.  You need to increment the
 * file_t's f_pos by this amount.  As always, be aware of refcounts, check
 * the return value of the fget and the virtual function, and be sure the
 * virtual function exists (is not null) before calling it.
 *
 * Return either 0 or sizeof(dirent_t), or -errno.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        Invalid file descriptor fd.
 *      o ENOTDIR
 *        File descriptor does not refer to a directory.
 */
int
do_getdent(int fd, struct dirent *dirp)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_getdent");*/
        
        file_t	*f;
        
        if(fd < 0 || fd >= NFILES)
 	{
 		return	-EBADF;
 	}
        
        f = fget(fd);
        
        if(f == NULL)
        {
        	return	-EBADF;	
        }
        
        if(!((f->f_vnode->vn_mode)&S_IFDIR))
        {
        	fput(f);
        	return	-ENOTDIR;	
        }
        
        if(f->f_vnode->vn_ops->readdir == NULL){
                fput(f);
        	return	-EBADF;
        }
        
        int x = f->f_vnode->vn_ops->readdir(f->f_vnode, f->f_pos, dirp);
        
        if(x < 0)
        {
                fput(f);
        	return	x;
        }
        
        f->f_pos = f->f_pos + x;
        fput(f);
        return x;
}

/*
 * Modify f_pos according to offset and whence.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EBADF
 *        fd is not an open file descriptor.
 *      o EINVAL
 *        whence is not one of SEEK_SET, SEEK_CUR, SEEK_END; or the resulting
 *        file offset would be negative.
 */
int
do_lseek(int fd, int offset, int whence)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_lseek");*/
        
        int	x;

	/*handle neg offset*/	

 	if(fd < 0 || fd >= NFILES)
 	{
 		return	EBADF;
 	}

	file_t	*f = fget(fd);
	
	if(f == NULL){
		return	EBADF;
	}
        if(whence == SEEK_SET){
        
        	
        	
        	if(offset < 0){
        		fput(f);
        		return	EINVAL;
        	}
                f->f_pos = offset;
        	fput(f);
        	return	offset;
        }
        else
        {
        	if(whence == SEEK_CUR){
        		x = f->f_pos + offset;
        		
        		if(x >= 0){
        			f->f_pos = x;
        			fput(f);
        			return	f->f_pos;
        		}
        		else
        		{
        			fput(f);
        			return	EINVAL;
        		}
        	}
        	else
        	{
        		if(whence == SEEK_END){
        			
        			x = f->f_pos + offset;
        			if(x < 0){
        				fput(f);
        				return	EINVAL;
        			}
        			else
        			{
        				f->f_pos = x;
        				fput(f);
        				return	f->f_pos;
        			}
        		}
        		else
        		{
        			fput(f);
        			return	EINVAL;	
        		}
        	}
        }
     
        return -1;
}

/*
 * Find the vnode associated with the path, and call the stat() vnode operation.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o ENOENT
 *        A component of path does not exist.
 *      o ENOTDIR
 *        A component of the path prefix of path is not a directory.
 *      o ENAMETOOLONG
 *        A component of path was too long.
 */
int
do_stat(const char *path, struct stat *buf)
{
        /*NOT_YET_IMPLEMENTED("VFS: do_stat");*/
        /* Find the base directory*/
        size_t *namelen;
        const char **name;
        vnode_t *base = NULL;
        vnode_t **res_vnode = NULL;

        int res = open_namev(path, 0, res_vnode, base);
        if( res == 0)
        {
                return (*res_vnode)->vn_ops->stat(*res_vnode, buf);
        }
        else 
        {
        	return res;
        }

        return -1;

}

#ifdef __MOUNTING__
/*
 * Implementing this function is not required and strongly discouraged unless
 * you are absolutely sure your Weenix is perfect.
 *
 * This is the syscall entry point into vfs for mounting. You will need to
 * create the fs_t struct and populate its fs_dev and fs_type fields before
 * calling vfs's mountfunc(). mountfunc() will use the fields you populated
 * in order to determine which underlying filesystem's mount function should
 * be run, then it will finish setting up the fs_t struct. At this point you
 * have a fully functioning file system, however it is not mounted on the
 * virtual file system, you will need to call vfs_mount to do this.
 *
 * There are lots of things which can go wrong here. Make sure you have good
 * error handling. Remember the fs_dev and fs_type buffers have limited size
 * so you should not write arbitrary length strings to them.
 */
int
do_mount(const char *source, const char *target, const char *type)
{
        NOT_YET_IMPLEMENTED("MOUNTING: do_mount");
        return -EINVAL;
}

/*
 * Implementing this function is not required and strongly discouraged unless
 * you are absolutley sure your Weenix is perfect.
 *
 * This function delegates all of the real work to vfs_umount. You should not worry
 * about freeing the fs_t struct here, that is done in vfs_umount. All this function
 * does is figure out which file system to pass to vfs_umount and do good error
 * checking.
 */
int
do_umount(const char *target)
{
        NOT_YET_IMPLEMENTED("MOUNTING: do_umount");
        return -EINVAL;
}
#endif
