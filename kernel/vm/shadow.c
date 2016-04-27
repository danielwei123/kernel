
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

#include "util/string.h"
#include "util/debug.h"

#include "mm/mmobj.h"
#include "mm/pframe.h"
#include "mm/mm.h"
#include "mm/page.h"
#include "mm/slab.h"
#include "mm/tlb.h"

#include "vm/vmmap.h"
#include "vm/shadow.h"
#include "vm/shadowd.h"

#define SHADOW_SINGLETON_THRESHOLD 5

int shadow_count = 0; /* for debugging/verification purposes */
#ifdef __SHADOWD__
/*
 * number of shadow objects with a single parent, that is another shadow
 * object in the shadow objects tree(singletons)
 */
static int shadow_singleton_count = 0;
#endif

static slab_allocator_t *shadow_allocator;

static void shadow_ref(mmobj_t *o);
static void shadow_put(mmobj_t *o);
static int  shadow_lookuppage(mmobj_t *o, uint32_t pagenum, int forwrite, pframe_t **pf);
static int  shadow_fillpage(mmobj_t *o, pframe_t *pf);
static int  shadow_dirtypage(mmobj_t *o, pframe_t *pf);
static int  shadow_cleanpage(mmobj_t *o, pframe_t *pf);

static mmobj_ops_t shadow_mmobj_ops = {
        .ref = shadow_ref,
        .put = shadow_put,
        .lookuppage = shadow_lookuppage,
        .fillpage  = shadow_fillpage,
        .dirtypage = shadow_dirtypage,
        .cleanpage = shadow_cleanpage
};

/*
 * This function is called at boot time to initialize the
 * shadow page sub system. Currently it only initializes the
 * shadow_allocator object.
 */
void
shadow_init()
{
        /*NOT_YET_IMPLEMENTED("VM: shadow_init");*/
        dbg(DBG_PRINT, "Shadowing: 01\n");
         shadow_allocator = slab_allocator_create("shadowobj",  sizeof(mmobj_t));
}

/*
 * You'll want to use the shadow_allocator to allocate the mmobj to
 * return, then then initialize it. Take a look in mm/mmobj.h for
 * macros which can be of use here. Make sure your initial
 * reference count is correct.
 */
mmobj_t *
shadow_create()
{
        /*NOT_YET_IMPLEMENTED("VM: shadow_create");*/
	dbg(DBG_PRINT, "Shadowing: 02\n");
        mmobj_t *temp = slab_obj_alloc(shadow_allocator);
        if (temp)
        {
        dbg(DBG_PRINT, "Shadowing: 03\n");
            mmobj_init(temp, &shadow_mmobj_ops);
        }
        return temp;
}

/* Implementation of mmobj entry points: */

/*
 * Increment the reference count on the object.
 */
static void
shadow_ref(mmobj_t *o)
{
        /*NOT_YET_IMPLEMENTED("VM: shadow_ref");*/
        dbg(DBG_PRINT, "Shadowing: 04\n");
           o->mmo_refcount+=1;
}

/*
 * Decrement the reference count on the object. If, however, the
 * reference count on the object reaches the number of resident
 * pages of the object, we can conclude that the object is no
 * longer in use and, since it is a shadow object, it will never
 * be used again. You should unpin and uncache all of the object's
 * pages and then free the object itself.
 */
static void
shadow_put(mmobj_t *o)
{
        /*NOT_YET_IMPLEMENTED("VM: shadow_put");*/
        dbg(DBG_PRINT, "Shadowing: 05\n");
       /* if(o== NULL)
        {
        dbg(DBG_PRINT, "Shadowing: 06\n");
        	return;
        }
*/
        pframe_t *temp;
        if(o->mmo_refcount != o->mmo_nrespages + 1)
        {
        dbg(DBG_PRINT, "Shadowing: 07\n");
            o->mmo_refcount-=1;
        }
        else
        {
        dbg(DBG_PRINT, "Shadowing: 08\n");
            list_iterate_begin(&(o->mmo_respages),temp,pframe_t,pf_olink)
            {
                pframe_unpin(temp);
                pframe_free(temp);
            }
            list_iterate_end();
            /*Might need to put bottom obj too here*/
            
            o->mmo_shadowed->mmo_ops->put(o->mmo_shadowed);
            o->mmo_un.mmo_bottom_obj->mmo_ops->put(o->mmo_un.mmo_bottom_obj);
            slab_obj_free(shadow_allocator,(void *)o);
        }


}

/* This function looks up the given page in this shadow object. The
 * forwrite argument is true if the page is being looked up for
 * writing, false if it is being looked up for reading. This function
 * must handle all do-not-copy-on-not-write magic (i.e. when forwrite
 * is false find the first shadow object in the chain which has the
 * given page resident). copy-on-write magic (necessary when forwrite
 * is true) is handled in shadow_fillpage, not here. It is important to
 * use iteration rather than recursion here as a recursive implementation
 * can overflow the kernel stack when looking down a long shadow chain */
static int
shadow_lookuppage(mmobj_t *o, uint32_t pagenum, int forwrite, pframe_t **pf)
{
        /*NOT_YET_IMPLEMENTED("VM: shadow_lookuppage");*/
        dbg(DBG_PRINT, "Shadowing: 09\n");
        if(forwrite==1)
        {
        dbg(DBG_PRINT, "Shadowing: 10\n");
            return pframe_get(o,pagenum,pf);
        }

        pframe_t *temp=NULL;
        mmobj_t *cur=o;

        while(temp == NULL && cur->mmo_shadowed != NULL)
        {
        dbg(DBG_PRINT, "Shadowing: 11\n");
            temp=pframe_get_resident(cur,pagenum);
            cur=cur->mmo_shadowed;
        }

        if(temp==NULL)
        {
        dbg(DBG_PRINT, "Shadowing: 12\n");
            return pframe_lookup(cur,pagenum,0,pf);
        }

        *pf=temp;
        dbg(DBG_PRINT, "Shadowing: 13\n");
        return 0;
}

/* As per the specification in mmobj.h, fill the page frame starting
 * at address pf->pf_addr with the contents of the page identified by
 * pf->pf_obj and pf->pf_pagenum. This function handles all
 * copy-on-write magic (i.e. if there is a shadow object which has
 * data for the pf->pf_pagenum-th page then we should take that data,
 * if no such shadow object exists we need to follow the chain of
 * shadow objects all the way to the bottom object and take the data
 * for the pf->pf_pagenum-th page from the last object in the chain).
 * It is important to use iteration rather than recursion here as a
 * recursive implementation can overflow the kernel stack when
 * looking down a long shadow chain */
static int
shadow_fillpage(mmobj_t *o, pframe_t *pf)
{
        /*NOT_YET_IMPLEMENTED("VM: shadow_fillpage");*/
        dbg(DBG_PRINT, "Shadowing: 14\n");
        pframe_t *temp=NULL;
        mmobj_t *cur=o->mmo_shadowed;

        while(temp==NULL && cur!=o->mmo_un.mmo_bottom_obj)
        {
        dbg(DBG_PRINT, "Shadowing: 15\n");
            temp=pframe_get_resident(cur,pf->pf_pagenum);
            cur=cur->mmo_shadowed;
        }

        if(temp==NULL)
        {
        dbg(DBG_PRINT, "Shadowing: 16\n");
            /*bottommost diamond*/
            int x=pframe_lookup(cur,pf->pf_pagenum,1,&temp);
            if(x<0)
            {
            dbg(DBG_PRINT, "Shadowing: 17\n");
                return x;
            }
        }

        pframe_pin(pf);
        memcpy(pf->pf_addr,temp->pf_addr,PAGE_SIZE);
        dbg(DBG_PRINT, "Shadowing: 18\n");
        return 0;
}

/* These next two functions are not difficult. */

static int
shadow_dirtypage(mmobj_t *o, pframe_t *pf)
{
        /*NOT_YET_IMPLEMENTED("VM: shadow_dirtypage");*/
        dbg(DBG_PRINT, "Shadowing: 19\n");
        return 0;
}

static int
shadow_cleanpage(mmobj_t *o, pframe_t *pf)
{
        /*NOT_YET_IMPLEMENTED("VM: shadow_cleanpage");*/
        return 0;
}
