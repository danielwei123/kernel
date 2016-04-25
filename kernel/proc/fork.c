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

#include "types.h"
#include "globals.h"
#include "errno.h"

#include "util/debug.h"
#include "util/string.h"

#include "proc/proc.h"
#include "proc/kthread.h"

#include "mm/mm.h"
#include "mm/mman.h"
#include "mm/page.h"
#include "mm/pframe.h"
#include "mm/mmobj.h"
#include "mm/pagetable.h"
#include "mm/tlb.h"

#include "fs/file.h"
#include "fs/vnode.h"

#include "vm/shadow.h"
#include "vm/vmmap.h"

#include "api/exec.h"

#include "main/interrupt.h"

/* Pushes the appropriate things onto the kernel stack of a newly forked thread
 * so that it can begin execution in userland_entry.
 * regs: registers the new thread should have on execution
 * kstack: location of the new thread's kernel stack
 * Returns the new stack pointer on success. */
static uint32_t
fork_setup_stack(const regs_t *regs, void *kstack)
{
        /* Pointer argument and dummy return address, and userland dummy return
         * address */
        uint32_t esp = ((uint32_t) kstack) + DEFAULT_STACK_SIZE - (sizeof(regs_t) + 12);
        *(void **)(esp + 4) = (void *)(esp + 8); /* Set the argument to point to location of struct on stack */
        memcpy((void *)(esp + 8), regs, sizeof(regs_t)); /* Copy over struct */
        return esp;
}

/*
 *Modular function to clean up a non curproc
 *process
 */
 static void clean_my_proc(proc_t *child)
 {
     list_remove(&child->p_list_link);
     pt_destroy_pagedir(child->p_pagedir);
     list_remove(&p->p_child_link);
     vput(p->p_cwd);
     vmmap_destroy(p->p_vmmap);
 }

/*
 * Modular function to link a shadow object
 * to an area
 */
void link_shadow_obj(vmarea_t *area, mmobj_t *shadow)
{
    mmobj_t lower_obj = mmobj_bottom_obj(area->vma_obj);

    /* Link the shadow object directly to bottom object*/
    shadow->mmo_un.mmo_bottom_obj = lower_obj;
    /*Ref once?*/
    lower_obj->mmo_ops->ref(lower_obj);

    /*Should I ref the vma_obj of area here? Not really sure. Maybe we should put and ref once but it wont matter then?*/
    shadow->mmo_shadowed = area->vma_obj;

    /*Really not sure about this part. Need to check*/
    if(list_link_is_linked(&area->vma_olink))
    {
        list_remove(&vma->vma_olink);
    }
    list_insert_tail(&lower_obj->mmo_un.mmo_vmas,&area->vma_olink);

    /*No need to ref for shadow object since I did it in previous function*/
    area->vma_obj = shadow;
}


/*
 * Modular function to create shadow objects
 */

int create_shadow_objects(vmarea_t parent_area , vmarea_t child_area)
{
    mmobj_t *shadow1 = shadow_create();

    if(shadow1 == NULL)
    {
        return -ENOMEM;
    }

    shadow1->mmo_ops->ref(shadow1);

    mmobj_t *shadow2 = shadow_create();

    if(shadow2 == NULL)
    {
        shadow1->mmo_ops->put(shadow1);
        return -ENOMEM;
    }

    shadow2->mmo_ops->ref(shadow2);

    link_shadow_obj(parent_area,shadow1);
    link_shadow_obj(child_area,shadow2);
}

/*
 * Modular function to clean up the vmmap I
 * created. This is to be done in case of
 * problems while creating the vmmap for
 * child process
 */

void clean_my_vmmap(list_t *parent_list,list_t *child_list)
{
    list_link_t *parent_link = parent_list->l_next;
    list_link_t *child_link = child_list->l_next;

    while(parent_link != parent_list)
    {
        vmarea_t *parent_area = list_item(parent_link,vmarea_t,vma_plink);
        vmarea_t *child_area = list_item(child_link,vmarea_t,vma_plink);

        /*If we find an area with no object, we can stop cleaning up*/
        if(child_area->vma_obj == NULL)
        {
            return;
        }

        int map = parent_area->vma_flags & MAP_TYPE;

        /*Screwed. Need to clean shadow objects*/
        /*SOMEONE PLEASE CHECK THIS PART AGAIN!!!*/
        if(map == MAP_PRIVATE)
        {
            mmobj_t *lower_obj = parent_area->vma_obj->mmo_shadowed;
            lower_obj->mmo_ops->ref(lower_obj);

            parent_area->vma_obj->mmo_ops->put(parent_area->vma_obj);
            parent_area->vma_obj=lower_obj;
        }

        parent_link = parent_link->l_next;
        child_link = child_link->l_next;
    }
}

/*
 * Modular function to copy vmmap of parent
 * to child. Returning 0 on success. Negative
 * value on error
 */

int vmmap_copy(proc_t *child)
 {
    int error = 0;

    vmmap_t *childmap = vmmap_clone(curproc->p_vmmap);

    if(childmap == NULL)
    {
        return -ENOMEM;
    }

    childmap->vmm_proc = child;

    list_t *parent_list = &curproc->p_vmmap->vmm_list;
    list_t *child_list = childmap->vmm_list;
    list_link_t *parent_link = parent_list->l_next;
    list_link_t *child_link = child_list->l_next;

    while(parent_link != parent_list && error == 0)
    {
        vmarea_t parent_area = list_item(parent_link,vmarea_t,vma_plink);
        vmarea_t child_area = list_item(child_link,vmarea_t,vma_plink);

        child_area->vma_obj = parent_area->vma_obj;
        child_area->vma_obj->mmo_ops->ref(child_area->vma_obj);

        /*DO SHADOW STUFF HERE TOMORROW */
        int map = parent_area->vma_flags & MAP_TYPE;

        if(map == MAP_PRIVATE)
        {
            error = create_shadow_objects(parent_area,child_area);

            if(error < 0)
            {
                child_area->vma_obj->mmo_ops->put(child_area->vma_obj);
                child_area->vma_obj=NULL;
            }
        }

        child_link = child_link->l_next;
        parent_link = parent_link->l_next;

        if(error != 0)
        {
            clean_my_vmmap(parent_list,child_list);
            vmmap_destroy(childmap);
            return error;
        }

        vmmap_destroy(child->p_vmmap);
        child->p_vmmap = childmap;
        return 0;
    }

 }


/*
 * The implementation of fork(2). Once this works,
 * you're practically home free. This is what the
 * entirety of Weenix has been leading up to.
 * Go forth and conquer.
 */
int
do_fork(struct regs *regs)
{
        /*NOT_YET_IMPLEMENTED("VM: do_fork");*/
        proc_t *child = proc_create("childproc");
        int error=0;

        if(child == NULL)
        {
            curthr->kt_errno = ENOMEM;
            return -1;
        }

        error = vmmap_copy(child);

        if(error != 0)
        {
            clean_my_proc(child);
            curthr->kt_errno = -error;
            return -1;
        }

        kthread_t *childthr = kthread_clone(curthr);

        if(childthr == NULL)
        {
            clean_my_vmmap(&curproc->p_vmmap->vmm_list,&child->p_vmmap->vmm_list);
            clean_my_proc(child);
            curthr->kt_errno = ENOMEM;
            return -1;
        }

        childthr->kt_proc = child;
        list_insert_tail(&child->p_threads,&childthr->kt_plink);

        /*This part is based on talk with Ajitesh. Need to check with Karishma or Harsha once*/
        regs->r_eax = 0;
        int stack_ret_val = fork_setup_stack(regs,childthr->kt_kstack);
        childthr->kt_ctx.c_pdptr = child->p_pagedir;
        childthr->kt_ctx.c_eip = (uint32_t) userland_entry;
        childthr->kt_ctx.c_esp = stack_ret_val;
        childthr->kt_ctx.c_kstack = (uintptr_t) curthr->kt_stack;
        childthr->kt_ctx.c_kstacksz = DEFAULT_STACK_SIZE;

        return 0;
}
