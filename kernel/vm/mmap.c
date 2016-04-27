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

#include "globals.h"
#include "errno.h"
#include "types.h"

#include "mm/mm.h"
#include "mm/tlb.h"
#include "mm/mman.h"
#include "mm/page.h"

#include "proc/proc.h"

#include "util/string.h"
#include "util/debug.h"

#include "fs/vnode.h"
#include "fs/vfs.h"
#include "fs/file.h"

#include "vm/vmmap.h"
#include "vm/mmap.h"

/*
 * This function implements the mmap(2) syscall, but only
 * supports the MAP_SHARED, MAP_PRIVATE, MAP_FIXED, and
 * MAP_ANON flags.
 *
 * Add a mapping to the current process's address space.
 * You need to do some error checking; see the ERRORS section
 * of the manpage for the problems you should anticipate.
 * After error checking most of the work of this function is
 * done by vmmap_map(), but remember to clear the TLB.
 */
int
do_mmap(void *addr, size_t len, int prot, int flags,
        int fd, off_t off, void **ret)
{
        /*NOT_YET_IMPLEMENTED("VM: do_mmap");*/
        /*
        if(curproc->p_files[fd]->f_vnode->vn_mode&S_IFREG)
        {
        	return	-EACCES;
        }
        */
        dbg(DBG_PRINT,"AFINAL mmap 1\n");
        if(!((flags&MAP_TYPE)==MAP_SHARED) && !((flags&MAP_TYPE)==MAP_PRIVATE) )
        {
        	dbg(DBG_PRINT,"AFINAL mmap 2\n");
        	return	-EINVAL;
        }
        
        if(len == 0 || len > USER_MEM_HIGH)
        {
        	dbg(DBG_PRINT,"AFINAL mmap 3\n");
        	return -EINVAL;
        }
        
        
        if(!PAGE_ALIGNED(off))
        {
        	dbg(DBG_PRINT,"AFINAL mmap 4\n");
        	return	-EINVAL;
        }
        
                
        if(!PAGE_ALIGNED(addr) && !(flags&MAP_ANON) && (flags&MAP_FIXED))
        {
        	dbg(DBG_PRINT,"AFINAL mmap 5\n");
        	return	-EINVAL;
        }

        /*if(addr != NULL && (uint32_t)addr < USER_MEM_LOW)
        {
        	dbg(DBG_PRINT,"AFINAL mmap 6\n");
        	return	-EINVAL;
        }*/
        
        /*if(addr != NULL && (uint32_t)addr > USER_MEM_HIGH)
        {
        	dbg(DBG_PRINT,"AFINAL mmap 7\n");
        	return	-EINVAL;
        }*/
        
        /*if(addr != NULL && len > (USER_MEM_HIGH - (uint32_t)addr))
        {
        	dbg(DBG_PRINT,"PAFINAL mmap 8\n");
        	return -EINVAL;
        }*/	
        
        if( (flags&MAP_FIXED) && addr==NULL)
        {
        	dbg(DBG_PRINT,"AFINAL mmap 9\n");
        	return	-EINVAL;
        }
        
        vnode_t	*vnode_param = NULL;
        
        if(!(flags&MAP_ANON))
        {
        	dbg(DBG_PRINT,"AFINAL mmap 10\n");
        	if((fd<0 || fd >=NFILES))
			{	
			dbg(DBG_PRINT,"AFINAL mmap 11\n");
				return -EBADF;
			}
			
			if(curproc->p_files[fd] == NULL)
        	{
        		dbg(DBG_PRINT,"AFINAL mmap 12\n");
        		return -EBADF;
       	 	}
       	 	
       	 	int	map_flag = MAP_TYPE&flags;
       	 	
       	 	/*if((map_flag == MAP_PRIVATE) && !(curproc->p_files[fd]->f_mode&FMODE_READ))
        	{
        		dbg(DBG_PRINT,"PAFINAL mmap 13\n");
        		return	-EACCES;
        	}*/
        	
        	if(flags&MAP_SHARED && (prot&PROT_WRITE) && !(curproc->p_files[fd]->f_mode&FMODE_READ && curproc->p_files[fd]->f_mode&FMODE_WRITE))
        	{
        		dbg(DBG_PRINT,"AFINAL mmap 14\n");
        		return	-EACCES;
        	}
        	
        	/*if((prot&PROT_WRITE) && (curproc->p_files[fd]->f_mode&(FMODE_APPEND))) 
        	{
        		dbg(DBG_PRINT,"PAFINAL mmap 15\n");
        		return	-EACCES;
        	}*/
			
			vnode_param = curproc->p_files[fd]->f_vnode;
        
        }/*maps anon ends*/
        
        
			

        /*vmmap_t	* newmap = vmmap_create();*/
       
       vmarea_t	*newret;
       
    int	ret_val = vmmap_map(curproc->p_vmmap, vnode_param, ADDR_TO_PN(addr), ((uint32_t)PAGE_ALIGN_UP(len)/PAGE_SIZE), prot, flags, off, VMMAP_DIR_HILO, &newret);
        
        
        if(ret_val >= 0 && (ret)!= NULL)
        {
        	dbg(DBG_PRINT,"AFINAL mmap 16\n");
        	*ret = PN_TO_ADDR(newret->vma_start);
        	/*pt_unmap_range(curproc->p_pagedir, (uint32_t)PN_TO_ADDR(newret->vma_start), (uint32_t)PN_TO_ADDR(newret->vma_end));*/
       		tlb_flush_all();
       		/*tlb_flush_range( (uintptr_t)newret->vma_start, ((uint32_t)PAGE_ALIGN_UP(len)/PAGE_SIZE));*/
        }
        /*else
        {
			dbg(DBG_PRINT,"PAFINAL mmap 17\n");
        	*ret = ((void *)-1);
        	
        }*/
        
        /*curproc->p_vmmap = newmap;
        newmap->vmm_proc = curproc;*/
		dbg(DBG_PRINT,"AFINAL mmap 18\n");        
       return	ret_val;
}


/*
 * This function implements the munmap(2) syscall.
 *
 * As with do_mmap() it should perform the required error checking,
 * before calling upon vmmap_remove() to do most of the work.
 * Remember to clear the TLB.
 */
int
do_munmap(void *addr, size_t len)
{
        /*NOT_YET_IMPLEMENTED("VM: do_munmap");*/
		if(!PAGE_ALIGNED(addr))
        {
        	dbg(DBG_PRINT,"AFINAL mmap 19\n");
        	return	-EINVAL;
        }
        if(len == 0 || len >= USER_MEM_HIGH)
        { 
        	dbg(DBG_PRINT,"AFINAL mmap 20\n");
        	return	-EINVAL;
        		
		}
		
		if(addr == NULL)
		{
			dbg(DBG_PRINT,"AFINAL mmap 21\n");
			return -EINVAL;
		}
		
		if(addr != NULL && len > (USER_MEM_HIGH - (uint32_t)addr))
        {
        		dbg(DBG_PRINT,"PAFINAL mmap 22\n");
		      return -EINVAL;
        }	
				        
        vmmap_remove( curproc->p_vmmap, ADDR_TO_PN(addr), ((uint32_t)PAGE_ALIGN_UP(len)/PAGE_SIZE));
        
        tlb_flush_all();
		dbg(DBG_PRINT,"AFINAL mmap 23\n");        
        return 0;
}

