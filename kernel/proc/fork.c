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

     while(parent_link != parent_list)
     {
         vmarea_t parent_area = list_item(parent_link,vmarea_t,vma_plink);
         vmarea_t child_area = list_item(child_link,vmarea_t,vma_plink);

         child_area->vma_obj = parent_area->vma_obj;
         child_area->vma_obj->mmo_ops->ref(child_area->vma_obj);

         /*DO SHADOW STUFF HERE TOMORROW */

         child_link = child_link->l_next;
         parent_link = parent_link->l_next;

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

        return 0;
}
