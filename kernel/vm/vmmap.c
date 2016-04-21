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
#include "errno.h"
#include "globals.h"

#include "vm/vmmap.h"
#include "vm/shadow.h"
#include "vm/anon.h"

#include "proc/proc.h"

#include "util/debug.h"
#include "util/list.h"
#include "util/string.h"
#include "util/printf.h"

#include "fs/vnode.h"
#include "fs/file.h"
#include "fs/fcntl.h"
#include "fs/vfs_syscall.h"

#include "mm/slab.h"
#include "mm/page.h"
#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/mmobj.h"

static slab_allocator_t *vmmap_allocator;
static slab_allocator_t *vmarea_allocator;

#define MIN_PAGE ADDR_TO_PN(USER_MEM_LOW)
#define MAX_PAGE ADDR_TO_PN(USER_MEM_HIGH)

void
vmmap_init(void)
{
        vmmap_allocator = slab_allocator_create("vmmap", sizeof(vmmap_t));
        KASSERT(NULL != vmmap_allocator && "failed to create vmmap allocator!");
        vmarea_allocator = slab_allocator_create("vmarea", sizeof(vmarea_t));
        KASSERT(NULL != vmarea_allocator && "failed to create vmarea allocator!");
}

vmarea_t *
vmarea_alloc(void)
{
        vmarea_t *newvma = (vmarea_t *) slab_obj_alloc(vmarea_allocator);
        if (newvma) {
                newvma->vma_vmmap = NULL;
        }
        return newvma;
}

void
vmarea_free(vmarea_t *vma)
{
        KASSERT(NULL != vma);
        slab_obj_free(vmarea_allocator, vma);
}

/* a debugging routine: dumps the mappings of the given address space. */
size_t
vmmap_mapping_info(const void *vmmap, char *buf, size_t osize)
{
        KASSERT(0 < osize);
        KASSERT(NULL != buf);
        KASSERT(NULL != vmmap);

        vmmap_t *map = (vmmap_t *)vmmap;
        vmarea_t *vma;
        ssize_t size = (ssize_t)osize;

        int len = snprintf(buf, size, "%21s %5s %7s %8s %10s %12s\n",
                           "VADDR RANGE", "PROT", "FLAGS", "MMOBJ", "OFFSET",
                           "VFN RANGE");

        list_iterate_begin(&map->vmm_list, vma, vmarea_t, vma_plink) {
                size -= len;
                buf += len;
                if (0 >= size) {
                        goto end;
                }

                len = snprintf(buf, size,
                               "%#.8x-%#.8x  %c%c%c  %7s 0x%p %#.5x %#.5x-%#.5x\n",
                               vma->vma_start << PAGE_SHIFT,
                               vma->vma_end << PAGE_SHIFT,
                               (vma->vma_prot & PROT_READ ? 'r' : '-'),
                               (vma->vma_prot & PROT_WRITE ? 'w' : '-'),
                               (vma->vma_prot & PROT_EXEC ? 'x' : '-'),
                               (vma->vma_flags & MAP_SHARED ? " SHARED" : "PRIVATE"),
                               vma->vma_obj, vma->vma_off, vma->vma_start, vma->vma_end);
        } list_iterate_end();

end:
        if (size <= 0) {
                size = osize;
                buf[osize - 1] = '\0';
        }
        /*
        KASSERT(0 <= size);
        if (0 == size) {
                size++;
                buf--;
                buf[0] = '\0';
        }
        */
        return osize - size;
}

/* Create a new vmmap, which has no vmareas and does
 * not refer to a process. */
vmmap_t *
vmmap_create(void)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_create");*/
        vmmap_t *temp = (vmmap_t *)slab_obj_alloc(vmmap_allocator);
        if(temp == NULL)
        {
            return temp;
        }
        list_init(&temp->vmm_list);
        temp->vmm_proc = NULL;
        return temp;
}

/* Removes all vmareas from the address space and frees the
 * vmmap struct. */
void
vmmap_destroy(vmmap_t *map)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_destroy");*/
        vmarea_t *temp;
        list_iterate_begin(&(map->vmm_list),temp,vmarea_t,vma_plink)
        {
            /*CHECK STUFF HERE */
            if(temp->vma_obj)
            {
                temp->vma_obj->mmo_ops->put(temp->vma_obj);
            }
            list_remove(&(temp->vma_plink));
            if(list_link_is_linked(&(temp->vma_olink)))
            {
                list_remove(&(temp->vma_olink));
            }
            vmarea_free(temp);
        }
        list_iterate_end();
        slab_obj_free(vmmap_allocator,map);
}

/* Add a vmarea to an address space. Assumes (i.e. asserts to some extent)
 * the vmarea is valid.  This involves finding where to put it in the list
 * of VM areas, and adding it. Don't forget to set the vma_vmmap for the
 * area. */
void
vmmap_insert(vmmap_t *map, vmarea_t *newvma)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_insert");*/
         vmarea_t 	*temp;

         if(list_empty(&(map->vmm_list)))
         {
         	list_insert_head(&(map->vmm_list), &(newvma->vma_plink));
         	newvma->vma_vmmap = map;
         	return;
         }


        list_iterate_begin(&(map->vmm_list), temp, vmarea_t, vma_plink)
        {
        	if(  temp->vma_start >= newvma->vma_end )
        	{
        		list_insert_before(&temp->vma_plink, &newvma->vma_plink);
        		newvma->vma_vmmap = map;
        		return;

        	}
        }
        list_iterate_end();

        list_insert_tail(&(map->vmm_list), &newvma->vma_plink);
       	newvma->vma_vmmap = map;

		return;
}

/*
	sufficient space
*/

int
sufficient_space(vmarea_t *prev, vmarea_t	*next, int	npages)
{
	return ((prev!=NULL) && (next!=NULL) && ((next->vma_start - prev->vma_end)>=(uint32_t)npages));
}

/*
    Function to check the ends of address space
*/

int
check_boundary(vmmap_t *map,int32_t npages,int dir)
{
    list_t *vmareas = &(map->vmm_list);
    if(dir == VMMAP_DIR_LOHI)
    {
        if(list_empty(vmareas))
        {
            return MIN_PAGE;
        }
        vmarea_t *temp = list_item(vmareas->l_next,vmarea_t,vma_plink);
        if(temp->vma_start - MIN_PAGE >= (uint32_t)npages)
        {
            return MIN_PAGE;
        }
    }
    else
    {
        if(list_empty(vmareas))
        {
            return MAX_PAGE-npages;
        }
        vmarea_t *temp = list_item(vmareas->l_prev,vmarea_t,vma_plink);
        if(MAX_PAGE - temp->vma_end >= (uint32_t) npages)
        {
            return MAX_PAGE-npages;
        }
    }
    return -1;
}

/* Find a contiguous range of free virtual pages of length npages in
 * the given address space. Returns starting vfn for the range,
 * without altering the map. Returns -1 if no such range exists.
 *
 * Your algorithm should be first fit. If dir is VMMAP_DIR_HILO, you
 * should find a gap as high in the address space as possible; if dir
 * is VMMAP_DIR_LOHI, the gap should be as low as possible. */
int
vmmap_find_range(vmmap_t *map, uint32_t npages, int dir)
{
       /* NOT_YET_IMPLEMENTED("VM: vmmap_find_range");*/

       /*

       			check for boundary limits while returning vfn

       			and

       			if else for first fit algo

       */
       
       
       int x = check_boundary(map,npages,dir);

       if(x != -1)
       {
           return x;
       }

       vmarea_t 	*prev = NULL;
       vmarea_t		*cur = NULL;
       list_link_t	*prevList = NULL;
       list_link_t	*curList = NULL;


       if(dir == VMMAP_DIR_HILO)
       {
			prevList = ((map->vmm_list).l_prev);

			while(prevList != &(map->vmm_list))
			{
				if(curList != NULL)
				{
					cur = list_item(curList, vmarea_t, vma_plink);
				}
				else
				{
					cur = NULL;
				}

				if(prevList != NULL)
				{
					prev = list_item(prevList, vmarea_t, vma_plink);
				}
				else
				{
					prev = NULL;
				}

				if(sufficient_space(prev, cur, npages))
				{
					return	cur->vma_start-npages;
				}

				curList = prevList;
				prevList = prevList->l_prev;

			}


       }
       else/*VMM7AP_DIR_LOHI*/
       {


			curList = ((map->vmm_list).l_next);

			while(curList != &(map->vmm_list))
			{
				if(curList != NULL)
				{
					cur = list_item(curList, vmarea_t, vma_plink);
				}
				else
				{
					cur = NULL;
				}

				if(prevList != NULL)
				{
					prev = list_item(prevList, vmarea_t, vma_plink);
				}
				else
				{
					prev = NULL;
				}

				if(sufficient_space(prev, cur, npages))
				{
					return	(cur->vma_end);
				}

				prevList = curList;
				curList = curList->l_next;

			}

       }

       return -1;
}

/* Find the vm_area that vfn lies in. Simply scan the address space
 * looking for a vma whose range covers vfn. If the page is unmapped,
 * return NULL. */
vmarea_t *
vmmap_lookup(vmmap_t *map, uint32_t vfn)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_lookup");*/
        vmarea_t	*temp;	
        list_iterate_begin(&(map->vmm_list),temp,vmarea_t,vma_plink)
        {
            if(temp->vma_start <= vfn && temp->vma_end > vfn)
            {
                return temp;
              
            }
        }
        list_iterate_end();
        return NULL;
}

/* Allocates a new vmmap containing a new vmarea for each area in the
 * given map. The areas should have no mmobjs set yet. Returns pointer
 * to the new vmmap on success, NULL on failure. This function is
 * called when implementing fork(2). */
vmmap_t *
vmmap_clone(vmmap_t *map)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_clone");*/
        vmmap_t *temp = vmmap_create();
        if(temp == NULL)
        {
            return temp;
        }
        
        temp->vmm_proc = map->vmm_proc;
        vmarea_t *temparea;
        list_iterate_begin(&(map->vmm_list),temparea,vmarea_t,vma_plink)
        {
            vmarea_t *newtemparea = vmarea_alloc();
            if(newtemparea == NULL)
            {
                vmmap_destroy(temp);
                return NULL;
            }
            newtemparea->vma_start = temparea->vma_start;
            newtemparea->vma_end = temparea->vma_end;
            newtemparea->vma_off = temparea->vma_off;
            newtemparea->vma_prot = temparea->vma_prot;
            newtemparea->vma_flags = temparea->vma_flags;
            newtemparea->vma_vmmap = temp;
            newtemparea->vma_obj = NULL;
            list_link_init(&(newtemparea->vma_olink));
            list_link_init(&(newtemparea->vma_plink));
            list_insert_tail(&(temp->vmm_list),&(newtemparea->vma_plink));
        }
        list_iterate_end();
        return temp;
}

/* Insert a mapping into the map starting at lopage for npages pages.
 * If lopage is zero, we will find a range of virtual addresses in the
 * process that is big enough, by using vmmap_find_range with the same
 * dir argument.  If lopage is non-zero and the specified region
 * contains another mapping that mapping should be unmapped.
 *
 * If file is NULL an anon mmobj will be used to create a mapping
 * of 0's.  If file is non-null that vnode's file will be mapped in
 * for the given range.  Use the vnode's mmap operation to get the
 * mmobj for the file; do not assume it is file->vn_obj. Make sure all
 * of the area's fields except for vma_obj have been set before
 * calling mmap.
 *
 * If MAP_PRIVATE is specified set up a shadow object for the mmobj.
 *
 * All of the input to this function should be valid (KASSERT!).
 * See mmap(2) for for description of legal input.
 * Note that off should be page aligned.
 *
 * Be very careful about the order operations are performed in here. Some
 * operation are impossible to undo and should be saved until there
 * is no chance of failure.
 *
 * If 'new' is non-NULL a pointer to the new vmarea_t should be stored in it.
 */
int
vmmap_map(vmmap_t *map, vnode_t *file, uint32_t lopage, uint32_t npages,
          int prot, int flags, off_t off, int dir, vmarea_t **new)
{

        /*Anirudh's vmmap_map()*/
        vmarea_t *temp = vmarea_alloc();

        int x=lopage;

        if(lopage == 0)
        {
            x=vmmap_find_range(map,npages,dir);
            if(x < 0)
            {
                vmarea_free(temp);
                return -ENOMEM;
            }
        }else if(!vmmap_is_range_empty(map, lopage, npages))
        {
        	int remove_status = vmmap_remove(map,x,npages);
        }

        temp->vma_start=x;
        temp->vma_end=x+npages;
        temp->vma_off=ADDR_TO_PN(off);
        
      	temp->vma_prot = prot;
       	temp->vma_flags = flags;
       	temp->vma_off = off;

        list_link_init(&(temp->vma_plink));
        list_link_init(&(temp->vma_olink));

        mmobj_t *tempobj;

		int	ret_val = 0;

        if(file==NULL)
        {
            tempobj=anon_create();
        }
        else
        {
            ret_val=file->vn_ops->mmap(file,temp,&tempobj);
        }
        
		if(ret_val < 0)
		{
		}        
	        
       temp->vma_obj=tempobj;

        tempobj->mmo_ops->ref(tempobj);
        vmmap_insert(map,temp);
        if(new)
        {
            *new = temp;
        }
        return 0;
        /*
       dbg(DBG_PRINT, "XXE Amey funtion\n");

       int	x = 0;

       	if(lopage == 0){
       	dbg(DBG_PRINT, "XXE Amey funtion 2\n");
       		x = vmmap_find_range(map, npages, dir);
       			if(x < 0)
				{
					return	x;
				}
       	}
		else
		{
		dbg(DBG_PRINT, "XXE Amey funtion 3\n");
			x = vmmap_is_range_empty(map, lopage, npages);
			if(x == 0)
			{
				int	res = vmmap_remove(map, lopage, npages);

				if(res < 0)
				{
					return	res;
				}

			}
		}
dbg(DBG_PRINT, "XXE Amey funtion 4\n");
       	vmarea_t	*temp = vmarea_alloc();;


       	if(lopage == 0){
dbg(DBG_PRINT, "XXE Amey funtion 5\n");
       		temp->vma_start = x;
       	}
       	else
       	{
       	dbg(DBG_PRINT, "XXE Amey funtion 6\n");
       		temp->vma_start = lopage;
       	}
       	dbg(DBG_PRINT, "XXE Amey funtion 6.5\n");
       	temp->vma_end = temp->vma_start + npages;

       	dbg(DBG_PRINT, "XXE Amey funtion 6.75\n");
       	if(file == NULL)
       	{
dbg(DBG_PRINT, "XXE Amey funtion 7\n");


       		temp->vma_obj = anon_create();
       	}
       	else
       	{
       	dbg(DBG_PRINT, "XXE Amey funtion 8\n");
       		temp->vma_obj = &(file->vn_mmobj);
       	}

dbg(DBG_PRINT, "XXE Amey funtion 9\n");
       vmmap_insert(map, temp);
dbg(DBG_PRINT, "XXE Amey funtion 10\n");
		if(new)
		{
       		*new = temp;
       	}
dbg(DBG_PRINT, "XXE Amey funtion 11\n");
        return 0;
        */


}

/*
 * We have no guarantee that the region of the address space being
 * unmapped will play nicely with our list of vmareas.
 *
 * You must iterate over each vmarea that is partially or wholly covered
 * by the address range [addr ... addr+len). The vm-area will fall into one
 * of four cases, as illustrated below:
 *
 * key:
 *          [             ]   Existing VM Area
 *        *******             Region to be unmapped
 *
 * Case 1:  [   ******    ]
 * The region to be unmapped lies completely inside the vmarea. We need to
 * split the old vmarea into two vmareas. be sure to increment the
 * reference count to the file associated with the vmarea.
 *
 * Case 2:  [      *******]**
 * The region overlaps the end of the vmarea. Just shorten the length of
 * the mapping.
 *
 * Case 3: *[*****        ]
 * The region overlaps the beginning of the vmarea. Move the beginning of
 * the mapping (remember to update vma_off), and shorten its length.
 *
 * Case 4: *[*************]**
 * The region completely contains the vmarea. Remove the vmarea from the
 * list.
 */
 
vmarea_t*
vmarea_clone(vmarea_t	*t)
{
	vmarea_t	*new = vmarea_alloc();
	
	new->vma_prot = t->vma_prot;
	new->vma_flags = t->vma_flags;
	new->vma_obj = t->vma_obj;
	new->vma_end = t->vma_end;
	new->vma_start = t->vma_start;
	new->vma_off = t->vma_off;

	list_link_init(&(new->vma_plink));
	list_link_init(&(new->vma_olink));
	
	if(new->vma_obj != NULL)
	{
		new->vma_obj->mmo_ops->ref(new->vma_obj);
	}
	
	return	new;

} 
 
int
vmmap_remove(vmmap_t *map, uint32_t lopage, uint32_t npages)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_remove");*/
        vmarea_t *temp;
        uint32_t endvfs = lopage + npages;
        
        if(npages == 0)
        {
        	return	0;
        }
        
        list_link_t	*link = (map->vmm_list).l_next;
        list_link_t	*linknext = NULL;
        
        while(link != &(map->vmm_list)){

			temp = list_item(link, vmarea_t, vma_plink);
		
			linknext = link->l_next;
		
			if( temp->vma_start<lopage && temp->vma_end>endvfs)
			{
				/*Case 1*/
				vmarea_t	*split = vmarea_clone(temp);
	
				temp->vma_end = lopage;

				split->vma_start = endvfs;
				split->vma_off = temp->vma_off + npages + lopage - temp->vma_start;
				
				/*
				split->vma_prot = temp->vma_prot;
				split->vma_flags = temp->vma_flags;

				split->vma_vmmap = temp->vma_vmmap;
				split->vma_obj = temp->vma_obj;
				potential ref on file obj*/

				vmmap_insert(map, split);

				/*Case 1 ends*/

			}
			else if(temp->vma_start<lopage && temp->vma_end<endvfs)
			{
				/*Case 2*/

				temp->vma_end = lopage;

				/*case 2 ends*/

			}
			else if(temp->vma_start>=lopage && temp->vma_end>endvfs)
			{
				/*Case 3*/

				temp->vma_off = (endvfs - temp->vma_start)+ temp->vma_off;
				temp->vma_start = endvfs;

				/*Case 3 ends*/
			}
			else if(temp->vma_start>=lopage && temp->vma_end<=endvfs)
			{
				/*Case 4*/
				temp->vma_obj->mmo_ops->put(temp->vma_obj);
				
				list_remove(&(temp->vma_plink));

				vmarea_free(temp);
				/*
					not sure about ref, put look at mmobj.h
				*/

				/*case4 ends*/
			}
			
			link = linknext;
        }


		pt_unmap_range(curproc->p_pagedir, (uint32_t)PN_TO_ADDR(lopage), (uint32_t)PN_TO_ADDR(endvfs));
		
        return 0;
}

/*
 * Returns 1 if the given address space has no mappings for the
 * given range, 0 otherwise.
 */
int
vmmap_is_range_empty(vmmap_t *map, uint32_t startvfn, uint32_t npages)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_is_range_empty");*/
        dbg(DBG_PRINT, "XXE Anirudh function\n");
        if(npages==0)
        {
            return 1;
        }
        dbg(DBG_PRINT, "XXE Anirudh function 2\n");
        /*Check if you need to add 1 also to endvfs*/
        uint32_t endvfn = startvfn + npages;
        vmarea_t *temp;
                dbg(DBG_PRINT, "XXE Anirudh function 3\n");
        list_iterate_begin(&(map->vmm_list),temp,vmarea_t,vma_plink)
        {
                dbg(DBG_PRINT, "XXE Anirudh function 4\n");
            if (startvfn >= temp->vma_start && startvfn < temp->vma_end)
            {
                    dbg(DBG_PRINT, "XXE Anirudh function 5\n");
                return 0;
            }

            if (endvfn > temp->vma_start && endvfn <= temp->vma_end)
            {
                    dbg(DBG_PRINT, "XXE Anirudh function 6\n");
                return 0;
            }

            if(startvfn < temp->vma_start && endvfn >= temp->vma_end)
            {
                    dbg(DBG_PRINT, "XXE Anirudh function 7\n");
                return 0;
            }
        }
        list_iterate_end();
                dbg(DBG_PRINT, "XXE Anirudh function 9\n");
        return 1;
}

/* Read into 'buf' from the virtual address space of 'map' starting at
 * 'vaddr' for size 'count'. To do so, you will want to find the vmareas
 * to read from, then find the pframes within those vmareas corresponding
 * to the virtual addresses you want to read, and then read from the
 * physical memory that pframe points to. You should not check permissions
 * of the areas. Assume (KASSERT) that all the areas you are accessing exist.
 * Returns 0 on success, -errno on error.
 */
int
vmmap_read(vmmap_t *map, const void *vaddr, void *buf, size_t count)
{

        /*NOT_YET_IMPLEMENTED("VM: vmmap_read");*/

        uint32_t dest_pos = 0;
        const void *curraddr = vaddr;

        while (dest_pos < count){
            uint32_t currvfn = ADDR_TO_PN(curraddr);

            vmarea_t *vma = vmmap_lookup(map, currvfn);

            KASSERT(vma != NULL);

            off_t offset = vma->vma_off + (currvfn - vma->vma_start);
            /* Not Sure to Align Up*/
            uint32_t pages_to_read = 0;
            if (ADDR_TO_PN(PAGE_ALIGN_UP(count - dest_pos)) < vma->vma_end - currvfn){
            	pages_to_read = ADDR_TO_PN(PAGE_ALIGN_UP(count - dest_pos));
            }
            else{
            	pages_to_read = vma->vma_end - currvfn;
            }
            uint32_t i;
            for (i = 0; i < pages_to_read; i++){
                pframe_t *p;
                int get_res = pframe_lookup(vma->vma_obj, offset + i, 0, &p);

                if (get_res < 0){
                    return get_res;
                }

                int data_offset = (int) curraddr % PAGE_SIZE;

                int read_size = 0;

                if (PAGE_SIZE - data_offset < count - dest_pos){
                	read_size = PAGE_SIZE - data_offset;
                }
                else{
                	read_size = count - dest_pos;
                }

                memcpy((char *) buf + dest_pos, (char *) p->pf_addr + data_offset, read_size);

                dest_pos += read_size;
                curraddr = (char *) curraddr + read_size;
            }
    }

    return 0;
}

/* Write from 'buf' into the virtual address space of 'map' starting at
 * 'vaddr' for size 'count'. To do this, you will need to find the correct
 * vmareas to write into, then find the correct pframes within those vmareas,
 * and finally write into the physical addresses that those pframes correspond
 * to. You should not check permissions of the areas you use. Assume (KASSERT)
 * that all the areas you are accessing exist. Remember to dirty pages!
 * Returns 0 on success, -errno on error.
 */
int
vmmap_write(vmmap_t *map, void *vaddr, const void *buf, size_t count)
{
        /*NOT_YET_IMPLEMENTED("VM: vmmap_write"); */

        uint32_t dest_pos = 0;
        const void *curraddr = vaddr;

        while (dest_pos < count){
            uint32_t currvfn = ADDR_TO_PN(curraddr);

            vmarea_t *vma = vmmap_lookup(map, currvfn);

            KASSERT(vma != NULL);

            off_t offset = vma->vma_off + (currvfn - vma->vma_start);
            /* Not Sure to Align Up */
            uint32_t pages_to_read = 0;
             if (ADDR_TO_PN(PAGE_ALIGN_UP(count - dest_pos)) < vma->vma_end - currvfn){
            	pages_to_read = ADDR_TO_PN(PAGE_ALIGN_UP(count - dest_pos));
            }
            else{
            	pages_to_read = vma->vma_end - currvfn;
            }
            uint32_t i;
            for (i = 0; i < pages_to_read; i++){
                pframe_t *p;
                int get_res = pframe_lookup(vma->vma_obj, offset + i, 0, &p);

                if (get_res < 0){
                    return get_res;
                }

                int data_offset = (int) curraddr % PAGE_SIZE;

               int write_size = 0;

                if (PAGE_SIZE - data_offset < count - dest_pos){
                	write_size = PAGE_SIZE - data_offset;
                }
                else{
                	write_size = count - dest_pos;
                }
                memcpy((char *) p->pf_addr + data_offset, (char *) buf + dest_pos, write_size);

                int res = pframe_dirty(p);
                
                if(res < 0){
                	return res;
                }

                dest_pos += write_size;
                curraddr = (char *) curraddr + write_size;
            }
    }

    return 0;
}
