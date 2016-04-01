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

#include "kernel.h"
#include "globals.h"
#include "types.h"
#include "errno.h"

#include "util/string.h"
#include "util/printf.h"
#include "util/debug.h"

#include "fs/dirent.h"
#include "fs/fcntl.h"
#include "fs/stat.h"
#include "fs/vfs.h"
#include "fs/vnode.h"

/* This takes a base 'dir', a 'name', its 'len', and a result vnode.
 * Most of the work should be done by the vnode's implementation
 * specific lookup() function, but you may want to special case
 * "." and/or ".." here depnding on your implementation.
 *
 * If dir has no lookup(), return -ENOTDIR.
 *
 * Note: returns with the vnode refcount on *result incremented.
 */
int
lookup(vnode_t *dir, const char *name, size_t len, vnode_t **result)
{

       /*NOT_YET_IMPLEMENTED("VFS: lookup");*/
        /* Check if to incremnet reference count or no*/	
	if(dir->vn_ops->lookup == NULL)
	{
		return	-ENOTDIR;
	}
	KASSERT(name!= NULL);
	if(len > NAME_LEN)
	{
		return -ENAMETOOLONG;
	}
	int res = dir->vn_ops->lookup(dir, name, len, result);
	return	res;

}


/* When successful this function returns data in the following "out"-arguments:
 *  o res_vnode: the vnode of the parent directory of "name"
 *  o name: the `basename' (the element of the pathname)
 *  o namelen: the length of the basename
 *
 * For example: dir_namev("/s5fs/bin/ls", &namelen, &name, NULL,
 * &res_vnode) would put 2 in namelen, "ls" in name, and a pointer to the
 * vnode corresponding to "/s5fs/bin" in res_vnode.
 *
 * The "base" argument defines where we start resolving the path from:
 * A base value of NULL means to use the process's current working directory,
 * curproc->p_cwd.  If pathname[0] == '/', ignore base and start with
 * vfs_root_vn.  dir_namev() should call lookup() to take care of resolving each
 * piece of the pathname.
 *
 * Note: A successful call to this causes vnode refcount on *res_vnode to
 * be incremented.
 */


int
dir_namev(const char *pathname, size_t *namelen, const char **name,
          vnode_t *base, vnode_t **res_vnode)
{

        /*NOT_YET_IMPLEMENTED("VFS: dir_namev");*/
        vnode_t	*myBase = NULL;
        vnode_t	*temp = NULL;
        if( pathname[0] == '/')
        {
            myBase = vfs_root_vn;  
            while(*pathname == '/')
            {
            	pathname++;
            } 
        }
        else if( base == NULL)
        {
            myBase = curproc->p_cwd;
        }
        else 
        {
        	myBase = base;
        }

	int	nextslash = 0;
	int	prevslash = 0;
     	int	retval = 1;   
     	int 	len;
        while(retval>0 && pathname[nextslash]!='\0')
        {
        	if(temp!=NULL)
        	{
        		vput(temp);
        	}
        	temp=myBase;
        	while(pathname[nextslash]!='/' && pathname[nextslash]!='\0')nextslash++;
        	len=nextslash - prevslash;
        	retval = lookup(temp,pathname+prevslash,len,&myBase);
        	while(pathname[nextslash]=='/' && pathname[nextslash]!='\0')nextslash++;
        	prevslash=nextslash;
        }
        if(retval < 0)
        {
        	return retval;
        }
        vput(myBase);
        *namelen = len;
        *name = (pathname + prevslash);
    	res_vnode = &temp; 
        return 0;
}

/* This returns in res_vnode the vnode requested by the other parameters.
 * It makes use of dir_namev and lookup to find the specified vnode (if it
 * exists).  flag is right out of the parameters to open(2); see
 * <weenix/fcntl.h>.  If the O_CREAT flag is specified, and the file does
 * not exist call create() in the parent directory vnode.
 *
 * Note: Increments vnode refcount on *res_vnode.
 */
int
open_namev(const char *pathname, int flag, vnode_t **res_vnode, vnode_t *base)
{
       	/*NOT_YET_IMPLEMENTED("VFS: open_namev");*/
        size_t	namelen = -1;
       	const char 	*name = NULL;
       	vnode_t	**ret_node = NULL;
       	
        int res = dir_namev((const char *)pathname, &namelen, &name, base, ret_node);
        
        if(res < 0)
        {
        	return	res;
        }
        
        res = lookup(*ret_node, name, namelen, res_vnode);
        
        if(res == -ENOENT)
        {
            if((flag & O_CREAT) == O_CREAT)
            {
                int x = (*ret_node)->vn_ops->create(*ret_node, name, namelen, res_vnode);
                
                if(x != 0)
                {
                	vput(*ret_node);
                    	return x;
                }

            }
            else
            { 
            	vput(*ret_node);
            	return res;
            }
        }
        
        vput(*ret_node);
        return	res;
}

#ifdef __GETCWD__
/* Finds the name of 'entry' in the directory 'dir'. The name is writen
 * to the given buffer. On success 0 is returned. If 'dir' does not
 * contain 'entry' then -ENOENT is returned. If the given buffer cannot
 * hold the result then it is filled with as many characters as possible
 * and a null terminator, -ERANGE is returned.
 *
 * Files can be uniquely identified within a file system by their
 * inode numbers. */
int
lookup_name(vnode_t *dir, vnode_t *entry, char *buf, size_t size)
{
        NOT_YET_IMPLEMENTED("GETCWD: lookup_name");
        return -ENOENT;
}


/* Used to find the absolute path of the directory 'dir'. Since
 * directories cannot have more than one link there is always
 * a unique solution. The path is writen to the given buffer.
 * On success 0 is returned. On error this function returns a
 * negative error code. See the man page for getcwd(3) for
 * possible errors. Even if an error code is returned the buffer
 * will be filled with a valid string which has some partial
 * information about the wanted path. */
ssize_t
lookup_dirpath(vnode_t *dir, char *buf, size_t osize)
{
        NOT_YET_IMPLEMENTED("GETCWD: lookup_dirpath");

        return -ENOENT;
}
#endif /* __GETCWD__ */
